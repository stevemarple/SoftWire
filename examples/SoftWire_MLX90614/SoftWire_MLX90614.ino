#include <AsyncDelay.h>
#include <SoftWire.h>

/* SoftWire_MLX90614
 *
 * Code to demonstrate how to communicate with an SMBus device such as
 * the MLX90614 non-contact infrared thermometer.
 *
 * Connections:
 *
 * For Calunium it is assumed the JTAG pins are used, SDA pin = 16
 * (JTAG TDI) and SCL pin = 19 (JTAG TCK) and that the device is
 * powered from pin 18 (JTAG TMS).
 * For all other boards it is assumed that the MLX90614 is permanently
 * powered and that SDA = A4 and SCL = A5.
 *
 */


#ifdef CALUNIUM
#include <DisableJTAG.h>

uint8_t sdaPin = 16; // JTAG TDI
uint8_t sclPin = 19; // JTAG TCK
uint8_t powerPin = 18;
#else
uint8_t sdaPin = A4;
uint8_t sclPin = A5;
#endif

const uint8_t cmdAmbient = 6;
const uint8_t cmdObject1 = 7;
const uint8_t cmdObject2 = 8;
const uint8_t cmdFlags = 0xf0;
const uint8_t cmdSleep = 0xff;


SoftWire i2c(sdaPin, sclPin);

AsyncDelay samplingInterval;


float convertToDegC(uint16_t data)
{
  // Remove MSB (error bit, ignored for temperatures)
  return (((data & (uint16_t)0x7FFF)) / 50.0) - 273.15;
}


// Switch from PWM mode (if it was enabled)
void exitPWM(void)
{
  // Make SMBus request to force SMBus output instead of PWM
  SoftWire::setSclLow(&i2c);
  delay(3); // Must be > 1.44ms
  SoftWire::setSclHigh(&i2c);
  delay(2);
}


uint16_t readMLX90614(uint8_t command, uint8_t &crc)
{
  uint8_t address = 0x5A;
  uint8_t dataLow = 0;
  uint8_t dataHigh = 0;
  uint8_t pec = 0;

  uint8_t errors = 0;
  digitalWrite(LED_BUILTIN, HIGH); delayMicroseconds(50);
  // Send command
  errors += i2c.startWait(address, SoftWire::writeMode);
  errors += i2c.write(command);
  
  // Read results
  errors += i2c.repeatedStart(address, SoftWire::readMode);
  errors += i2c.readThenAck(dataLow);  // Read 1 byte and then send ack
  errors += i2c.readThenAck(dataHigh); // Read 1 byte and then send ack
  errors += i2c.readThenNack(pec);
  i2c.stop();
  digitalWrite(LED_BUILTIN, LOW);

  crc = 0;
  crc = SoftWire::crc8_update(crc, address << 1); // Write address
  crc = SoftWire::crc8_update(crc, command);
  crc = SoftWire::crc8_update(crc, (address << 1) + 1); // Read address
  crc = SoftWire::crc8_update(crc, dataLow);
  crc = SoftWire::crc8_update(crc, dataHigh);
  crc = SoftWire::crc8_update(pec, pec);

  if (errors) {
    crc = 0xFF;
    return 0xFFFF;
  }
  return (uint16_t(dataHigh) << 8) | dataLow;
}

   


void setup(void)
{
  Serial.begin(9600);
  Serial.println("MLX90614_demo");

  pinMode(LED_BUILTIN, OUTPUT);
  
#ifdef CALUNIUM
  disableJTAG();
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  Serial.println("Powering MLX90614");
  delay(1000);
  
#endif

  /*
   * Uncomment if needed. Not a good idea if power switched on/off
   * since the sensor will be parasitically powered from the SDA and
   * SCL control lines.
  */
  //i2c.enablePullups();

  i2c.setDelay_us(5);
  i2c.begin();
  delay(300); // Data is available 0.25s after wakeup
  exitPWM();
}


void loop(void)
{
#ifdef CALUNIUM
  digitalWrite(powerPin, HIGH);
  delay(300); // Data available after 0.25s
  exitPWM();
#endif

  uint8_t crcAmbient;
  uint16_t rawAmbient = readMLX90614(cmdAmbient, crcAmbient);
  uint8_t crcObject1;
  uint16_t rawObject1 = readMLX90614(cmdObject1, crcObject1);
  // Uncomment lines below for dual FoV sensors
  // uint8_t crcObject2;
  // uint16_t rawObject2 = readMLX90614(cmdObject2, crcObject2);

  Serial.print("Ambient: ");
  if (crcAmbient)
    Serial.print("bus error");
  else if (rawAmbient & 0x8000) 
    Serial.print("read error");
  else
    Serial.print(convertToDegC(rawAmbient));

  Serial.print("    Object 1: ");
  if (crcObject1)
    Serial.print("bus error");
  else if (rawObject1 & 0x8000) 
    Serial.print("read error");
  else
    Serial.print(convertToDegC(rawObject1));

  // Uncomment lines below for dual FoV sensors
  // Serial.print("    Object 2: ");
  // if (rawObject2 & 0x8000) 
  //   Serial.print("read error");
  // else if (crcObject2)
  //   Serial.print("bus error");
  // else
  //   Serial.print(convertToDegC(rawObject2));
  Serial.println();
#ifdef CALUNIUM
  digitalWrite(powerPin, LOW);
#endif
  delay(1000);
}
