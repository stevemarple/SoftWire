#include <SoftWire.h>
#include <AsyncDelay.h>

SoftWire::SoftWire(uint8_t sda, uint8_t scl)
{
  _sda = sda;
  _scl = scl;
  _inputMode = INPUT; // Pullups disabled by default
  _delay_us = defaultDelay_us;
  _timeout_ms = defaultTimeout_ms;
}

void SoftWire::begin(void) const
{
  // Release SDA and SCL
  pinMode(_sda, _inputMode);
  pinMode(_scl, _inputMode);
}

void SoftWire::stop(void) const
{
  // Force SCL low
  pinMode(_scl, OUTPUT);
  digitalWrite(_scl, LOW);
  delayMicroseconds(_delay_us);
  
  // Force SDA low
  pinMode(_sda, OUTPUT);
  digitalWrite(_sda, LOW);
  delayMicroseconds(_delay_us);

  // Release SCL
  pinMode(_scl, _inputMode);
  delayMicroseconds(_delay_us);

  // Release SDA
  pinMode(_sda, _inputMode);
  delayMicroseconds(_delay_us);
}

SoftWire::result_t SoftWire::start(uint8_t rawAddr) const
{
  
  // Force SDA low
  pinMode(_sda, OUTPUT);
  digitalWrite(_sda, LOW);
  delayMicroseconds(_delay_us);
    
  // Force SCL low
  pinMode(_scl, OUTPUT);
  digitalWrite(_scl, LOW);
  delayMicroseconds(_delay_us);
  return rawWrite(rawAddr);
}


SoftWire::result_t SoftWire::repeatedStart(uint8_t rawAddr) const
{
  // Force SCL low
  pinMode(_scl, OUTPUT);
  digitalWrite(_scl, LOW);
  delayMicroseconds(_delay_us);

  // Release SDA
  pinMode(_sda, _inputMode);
  delayMicroseconds(_delay_us);

  // Release SCL
  pinMode(_scl, _inputMode);
  delayMicroseconds(_delay_us);

  // Force SDA low
  pinMode(_sda, OUTPUT);
  digitalWrite(_sda, LOW);
  delayMicroseconds(_delay_us);

  return rawWrite(rawAddr);
}


SoftWire::result_t SoftWire::startWait(uint8_t rawAddr) const
{
  AsyncDelay timeout(_timeout_ms, AsyncDelay::MILLIS);

  while (!timeout.isExpired()) {
    // Force SDA low
    pinMode(_sda, OUTPUT);
    digitalWrite(_sda, LOW);
    delayMicroseconds(_delay_us);
    
    switch (rawWrite(rawAddr)) {
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

  
SoftWire::result_t SoftWire::rawWrite(uint8_t data) const
{
  AsyncDelay timeout(_timeout_ms, AsyncDelay::MILLIS);
  for (uint8_t i = 8; i; --i) {
    // Force SCL low
    pinMode(_scl, OUTPUT);
    digitalWrite(_scl, LOW);
    
    if (data & 0x80) {
      // Release SDA
      pinMode(_sda, _inputMode);
      //digitalWrite(_sda, LOW); // To keep timing symmetrical
    }
    else {
      // Force SDA low
      pinMode(_sda, OUTPUT);
      digitalWrite(_sda, LOW);
    }
    delayMicroseconds(_delay_us);

    // Release SCL
    pinMode(_scl, _inputMode);

    delayMicroseconds(_delay_us);
  
    data <<= 1;
    if (timeout.isExpired()) {
      stop(); // Reset bus
      return timedOut;
    }
  }

  // Get ACK
  // Force SCL low
  pinMode(_scl, OUTPUT);
  digitalWrite(_scl, LOW);

  // Release SDA
  pinMode(_sda, _inputMode);
  
  delayMicroseconds(_delay_us);

  // Release SCL
  pinMode(_scl, _inputMode);

  // Wait for SCL to be set high (in case wait states are inserted)
  while (digitalRead(_scl) == LOW)
    if (timeout.isExpired()) {
      stop(); // Reset bus
      return timedOut;
    }

  return (digitalRead(_sda) == LOW ? ack : nack);
}


SoftWire::result_t SoftWire::read(uint8_t &data, bool sendAck) const
{
  data = 0;
  AsyncDelay timeout(_timeout_ms, AsyncDelay::MILLIS);

  for (uint8_t i = 8; i; --i) {
    data <<= 1;

    // Force SCL low
    pinMode(_scl, OUTPUT);
    digitalWrite(_scl, LOW);
    
    // Release SDA (from previous ACK)
    pinMode(_sda, _inputMode);
    delayMicroseconds(_delay_us);
    
    // Release SCL
    pinMode(_scl, _inputMode);
    delayMicroseconds(_delay_us);
    
    // Read clock stretch
    while (digitalRead(_scl) == LOW)
      if (timeout.isExpired()) {
	stop(); // Reset bus
	return timedOut;
      }
    
    if (digitalRead(_sda) & 1)
      data |= 1;
  }

  // Put ACK/NACK

  // Force SCL low
  pinMode(_scl, OUTPUT);
  digitalWrite(_scl, LOW);
  if (sendAck) {
    // Force SDA low
    pinMode(_sda, OUTPUT);
    digitalWrite(_sda, LOW);
  }
  else {
    // Release SDA
    pinMode(_sda, _inputMode);
  }

  delayMicroseconds(_delay_us);
  
  // Release SCL
  pinMode(_scl, _inputMode);
  delayMicroseconds(_delay_us);
  
  // Wait for SCL to return high
  while (digitalRead(_scl) == LOW)
    if (timeout.isExpired()) {
      stop(); // Reset bus
      return timedOut;
    }
  
  delayMicroseconds(_delay_us);
  return ack;
}
