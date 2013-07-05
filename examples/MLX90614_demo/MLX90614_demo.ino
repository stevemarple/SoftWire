#include <AsyncDelay.h>
#include <SoftWire.h>

/* MLX90614_demo
 *
 * Code to illustrate how to communicate with an SMBus device such as
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
  Serial.print("  data = ");
  Serial.println(data, HEX);
  // Remove MSB (error bit, ignored for temperatures)
  return (((data & (uint16_t)0x7FFF)) / 50) - 273.15;
}




uint16_t readMLX90614(uint8_t command)
{
  uint8_t address = 0x5A;
  uint8_t dataLow = 0;
  uint8_t dataHigh = 0;
  uint8_t pec = 0;
  
  digitalWrite(LED_BUILTIN, HIGH); delayMicroseconds(50);
  i2c.startWait(address, SoftWire::writeMode);
  i2c.rawWrite(command);
    
  // read
  i2c.repeatedStart(address, SoftWire::readMode);
  i2c.readThenAck(dataLow);  // Read 1 byte and then send ack
  i2c.readThenAck(dataHigh); // Read 1 byte and then send ack
  i2c.readThenNack(pec);
  i2c.stop();
  digitalWrite(LED_BUILTIN, LOW);

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
   * since the sensor will be parastically powered from the SDA and
   * SCL control lines.
  */
  // i2c.enablePullups();

  i2c.setDelay_us(5);
  i2c.begin();
}


void loop(void)
{
#ifdef CALUNIUM
  digitalWrite(powerPin, HIGH);
  delay(1000);
#endif
  
  Serial.print("Ambient: ");
  Serial.println(convertToDegC(readMLX90614(cmdAmbient)));
  Serial.print("Object 1: ");
  Serial.println(convertToDegC(readMLX90614(cmdObject1)));
  // Uncomment lines below for dual FoV sensors
  // Serial.print("Object 2: ");
  // Serial.println(convertToDegC(readMLX90614(cmdObject2)));
#ifdef CALUNIUM
  digitalWrite(powerPin, LOW);
#endif
  delay(2000);
}
