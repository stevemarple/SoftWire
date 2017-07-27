#include <util/atomic.h>
#include <SoftWire.h>
#include <AsyncDelay.h>


// Force SDA low
void SoftWire::setSdaLow(const SoftWire *p)
{
  uint8_t sda = p->getSda();
  // Disable interrupts whilst switching pin direction
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    digitalWrite(sda, LOW);
    pinMode(sda, OUTPUT);
  }
}


// Release SDA to float high
void SoftWire::setSdaHigh(const SoftWire *p)
{
  pinMode(p->getSda(), p->getInputMode());
}


// Force SCL low
void SoftWire::setSclLow(const SoftWire *p)
{
  uint8_t scl = p->getScl();
  // Disable interrupts whilst switching pin direction
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    digitalWrite(scl, LOW);
    pinMode(scl, OUTPUT);
  }
}


// Release SCL to float high
void SoftWire::setSclHigh(const SoftWire *p)
{
  pinMode(p->getScl(), p->getInputMode());
}


// Read SDA (for data read)
uint8_t SoftWire::readSda(const SoftWire *p)
{
  return digitalRead(p->getSda());
}


// Read SCL (to detect clock-stretching)
uint8_t SoftWire::readScl(const SoftWire *p)
{
  return digitalRead(p->getScl());
}


// For testing the CRC-8 calculator may be useful:
// http://smbus.org/faq/crc8Applet.htm
uint8_t SoftWire::crc8_update(uint8_t crc, uint8_t data)
{
  const uint16_t polynomial = 0x107;
  crc ^= data;
  for (uint8_t i = 8; i; --i) {
    if (crc & 0x80)
      crc = (uint16_t(crc) << 1) ^ polynomial;
    else
      crc <<= 1;
  }
  
  return crc;
}


SoftWire::SoftWire(uint8_t sda, uint8_t scl) :
  _sda(sda),
  _scl(scl),
  _inputMode(INPUT), // Pullups diabled by default
  _delay_us(defaultDelay_us),
  _timeout_ms(defaultTimeout_ms),
  _setSdaLow(setSdaLow),
  _setSdaHigh(setSdaHigh),
  _setSclLow(setSclLow),
  _setSclHigh(setSclHigh),
  _readSda(readSda),
  _readScl(readScl)
{
  ;
}

void SoftWire::begin(void) const
{
  /*
  // Release SDA and SCL
  _setSdaHigh(this);
  delayMicroseconds(_delay_us);
  _setSclHigh(this);
  */
  stop();
}

void SoftWire::stop(void) const
{
  // Force SCL low
  _setSclLow(this);
  delayMicroseconds(_delay_us);
  
  // Force SDA low
  _setSdaLow(this);
  delayMicroseconds(_delay_us);

  // Release SCL
  _setSclHigh(this);
  delayMicroseconds(_delay_us);

  // Release SDA
  _setSdaHigh(this);
  delayMicroseconds(_delay_us);
}

SoftWire::result_t SoftWire::llStart(uint8_t rawAddr) const
{
  
  // Force SDA low
  _setSdaLow(this);
  delayMicroseconds(_delay_us);
    
  // Force SCL low
  _setSclLow(this);
  delayMicroseconds(_delay_us);
  return write(rawAddr);
}


SoftWire::result_t SoftWire::llRepeatedStart(uint8_t rawAddr) const
{
  // Force SCL low
  _setSclLow(this);
  delayMicroseconds(_delay_us);

  // Release SDA
  _setSdaHigh(this);
  delayMicroseconds(_delay_us);

  // Release SCL
  _setSclHigh(this);
  delayMicroseconds(_delay_us);

  // Force SDA low
  _setSdaLow(this);
  delayMicroseconds(_delay_us);

  return write(rawAddr);
}


SoftWire::result_t SoftWire::llStartWait(uint8_t rawAddr) const
{
  AsyncDelay timeout(_timeout_ms, AsyncDelay::MILLIS);

  while (!timeout.isExpired()) {
    // Force SDA low
    _setSdaLow(this);
    delayMicroseconds(_delay_us);
    
    switch (write(rawAddr)) {
    case ack:
      return ack;
    case nack:
      stop();
    default:
      // timeout, and anything else we don't know about
      stop();
      return timedOut;
    }
  }
  return timedOut;
}

  
SoftWire::result_t SoftWire::write(uint8_t data) const
{
  AsyncDelay timeout(_timeout_ms, AsyncDelay::MILLIS);
  for (uint8_t i = 8; i; --i) {
    // Force SCL low
    _setSclLow(this);
    
    if (data & 0x80) {
      // Release SDA
      _setSdaHigh(this);
    }
    else {
      // Force SDA low
      _setSdaLow(this);
    }
    delayMicroseconds(_delay_us);

    // Release SCL
    _setSclHigh(this);

    delayMicroseconds(_delay_us);
  
    data <<= 1;
    if (timeout.isExpired()) {
      stop(); // Reset bus
      return timedOut;
    }
  }

  // Get ACK
  // Force SCL low
  _setSclLow(this);

  // Release SDA
  _setSdaHigh(this);
  
  delayMicroseconds(_delay_us);

  // Release SCL
  _setSclHigh(this);

  // Wait for SCL to be set high (in case wait states are inserted)
  while (_readScl(this) == LOW)
    if (timeout.isExpired()) {
      stop(); // Reset bus
      return timedOut;
    }

  result_t res = (_readSda(this) == LOW ? ack : nack);

  delayMicroseconds(_delay_us);

  // Keep SCL low between bytes
  _setSclLow(this);

  return res;
}


SoftWire::result_t SoftWire::read(uint8_t &data, bool sendAck) const
{
  data = 0;
  AsyncDelay timeout(_timeout_ms, AsyncDelay::MILLIS);

  for (uint8_t i = 8; i; --i) {
    data <<= 1;

    // Force SCL low
    _setSclLow(this);
    
    // Release SDA (from previous ACK)
    _setSdaHigh(this);
    delayMicroseconds(_delay_us);
    
    // Release SCL
    _setSclHigh(this);
    delayMicroseconds(_delay_us);
    
    // Read clock stretch
    while (_readScl(this) == LOW)
      if (timeout.isExpired()) {
	stop(); // Reset bus
	return timedOut;
      }
    
    if (_readSda(this))
      data |= 1;
  }

  
  // Put ACK/NACK

  // Force SCL low
  _setSclLow(this);
  if (sendAck) {
    // Force SDA low
    _setSdaLow(this);
  }
  else {
    // Release SDA
    _setSdaHigh(this);
  }

  delayMicroseconds(_delay_us);
  
  // Release SCL
  _setSclHigh(this);
  delayMicroseconds(_delay_us);
  
  // Wait for SCL to return high
  while (_readScl(this) == LOW)
    if (timeout.isExpired()) {
      stop(); // Reset bus
      return timedOut;
    }
  
  delayMicroseconds(_delay_us);

  // Keep SCL low between bytes
  _setSclLow(this);

  return ack;
}
