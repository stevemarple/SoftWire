#ifndef SOFTWIRE_H
#define SOFTWIRE_H
#include <Arduino.h>

#include <stdint.h>

class SoftWire {
public:
  enum result_t {
    ack = 0,
    nack = 1,
    timedOut = 2,
  };

  enum mode_t {
    writeMode = 0,
    readMode = 1,
  };

  static const uint8_t defaultDelay_us = 10;
  static const uint16_t defaultTimeout_ms = 100;

  static void setSdaLow(const SoftWire *p);
  static void setSdaHigh(const SoftWire *p);
  static void setSclLow(const SoftWire *p);
  static void setSclHigh(const SoftWire *p);
  static uint8_t readSda(const SoftWire *p);
  static uint8_t readScl(const SoftWire *p);

  
  SoftWire(uint8_t sda, uint8_t scl);
  inline uint8_t getSda(void) const;  
  inline uint8_t getScl(void) const;
  inline uint8_t getDelay_us(void) const;
  inline uint16_t getTimeout_ms(void) const;
  inline uint8_t getInputMode(void) const;
  
  // begin() must be called after any changes are made to SDA and/or
  // SCL pins.
  inline void setSda(uint8_t sda); 
  inline void setScl(uint8_t scl);
  inline void enablePullups(bool enablePullups = true);

  inline void setDelay_us(uint8_t delay_us);
  inline void setTimeout_ms(uint16_t timeout_ms);

  // begin() must be called before use, and after any changes are made
  // to the SDA and/or SCL pins.
  void begin(void) const;

  
  void stop(void) const;
  result_t start(uint8_t rawAddr) const;
  result_t repeatedStart(uint8_t rawAddr) const;
  result_t startWait(uint8_t rawAddr) const;

  inline result_t start(uint8_t addr, mode_t rwMode) const;
  inline result_t repeatedStart(uint8_t rawAddr, mode_t rwMode) const;
  inline result_t startWait(uint8_t rawAddr, mode_t rwMode) const;
  
  result_t rawWrite(uint8_t data) const;
  result_t read(uint8_t &data, bool sendAck = true) const;
  inline result_t readThenAck(uint8_t &data) const;
  inline result_t readThenNack(uint8_t &data) const;

private:
  uint8_t _sda;
  uint8_t _scl;
  uint8_t _inputMode;
  uint8_t _delay_us;
  uint16_t _timeout_ms;

  void (*_setSdaLow)(const SoftWire *p);
  void (*_setSdaHigh)(const SoftWire *p);
  void (*_setSclLow)(const SoftWire *p);
  void (*_setSclHigh)(const SoftWire *p);
  uint8_t (*_readSda)(const SoftWire *p);
  uint8_t (*_readScl)(const SoftWire *p);
};


uint8_t SoftWire::getSda(void) const
{
  return _sda;
}


uint8_t SoftWire::getScl(void) const
{
  return _scl;
}


uint8_t SoftWire::getDelay_us(void) const
{
  return _delay_us;
}

uint16_t SoftWire::getTimeout_ms(void) const
{
  return _timeout_ms;
}

uint8_t SoftWire::getInputMode(void) const
{
  return _inputMode;
}

void SoftWire::setSda(uint8_t sda)
{
  _sda = sda;
}


void SoftWire::setScl(uint8_t scl)
{
  _scl = scl;
}


void SoftWire::enablePullups(bool enable)
{
  _inputMode = (enable ? INPUT_PULLUP : INPUT);
}


void SoftWire::setDelay_us(uint8_t delay_us)
{
  _delay_us = delay_us;
}


void SoftWire::setTimeout_ms(uint16_t timeout_ms)
{
  _timeout_ms = timeout_ms;
}



SoftWire::result_t SoftWire::start(uint8_t addr, mode_t rwMode) const
{
  return start((addr << 1) + rwMode);
}


SoftWire::result_t SoftWire::repeatedStart(uint8_t addr, mode_t rwMode) const
{
  return repeatedStart((addr << 1) + rwMode);
}


SoftWire::result_t SoftWire::startWait(uint8_t addr, mode_t rwMode) const
{
  return startWait((addr << 1) + rwMode);
}


SoftWire::result_t SoftWire::readThenAck(uint8_t &data) const
{
  return read(data, true);
}


SoftWire::result_t SoftWire::readThenNack(uint8_t &data) const
{
  return read(data, false);
}

#endif
