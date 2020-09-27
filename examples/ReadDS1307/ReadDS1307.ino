#include <SoftWire.h>
#include <AsyncDelay.h>

#if defined(PIN_WIRE_SDA) && defined(PIN_WIRE_SCL)
int sdaPin = PIN_WIRE_SDA;
int sclPin = PIN_WIRE_SCL;

#else
int sdaPin = SDA;
int sclPin = SCL;
#endif

// I2C address of DS1307
const uint8_t I2C_ADDRESS = 0x68;

SoftWire sw(sdaPin, sclPin);
// These buffers must be at least as large as the largest read or write you perform.
char swTxBuffer[16];
char swRxBuffer[16];

AsyncDelay readInterval;


// Print with leading zero, as expected for time
void printTwoDigit(int n)
{
  if (n < 10) {
    Serial.print('0');
  }
  Serial.print(n);
}

void readTime(void)
{
  // Ensure register address is valid
  sw.beginTransmission(I2C_ADDRESS);
  sw.write(uint8_t(0)); // Access the first register
  sw.endTransmission();

  uint8_t registers[7]; // There are 7 registers we need to read from to get the date and time.
  int numBytes = sw.requestFrom(I2C_ADDRESS, (uint8_t)7);
  for (int i = 0; i < numBytes; ++i) {
    registers[i] = sw.read();
  }
  if (numBytes != 7) {
    Serial.print("Read wrong number of bytes: ");
    Serial.println((int)numBytes);
    return;
  }

  int tenYear = (registers[6] & 0xf0) >> 4;
  int unitYear = registers[6] & 0x0f;
  int year = (10 * tenYear) + unitYear;

  int tenMonth = (registers[5] & 0x10) >> 4;
  int unitMonth = registers[5] & 0x0f;
  int month = (10 * tenMonth) + unitMonth;

  int tenDateOfMonth = (registers[4] & 0x30) >> 4;
  int unitDateOfMonth = registers[4] & 0x0f;
  int dateOfMonth = (10 * tenDateOfMonth) + unitDateOfMonth;

  // Reading the hour is messy. See the datasheet for register details!
  bool twelveHour = registers[2] & 0x40;
  bool pm = false;
  int unitHour;
  int tenHour;
  if (twelveHour) {
    pm = registers[2] & 0x20;
    tenHour = (registers[2] & 0x10) >> 4;
  } else {
    tenHour = (registers[2] & 0x20) >> 4;
  }
  unitHour = registers[2] & 0x0f;
  int hour = (10 * tenHour) + unitHour;
  if (twelveHour) {
    // 12h clock? Convert to 24h.
    hour += 12;
  }

  int tenMinute = (registers[1] & 0xf0) >> 4;
  int unitMinute = registers[1] & 0x0f;
  int minute = (10 * tenMinute) + unitMinute;

  int tenSecond = (registers[0] & 0xf0) >> 4;
  int unitSecond = registers[0] & 0x0f;
  int second = (10 * tenSecond) + unitSecond;

  // ISO8601 is the only sensible time format
  Serial.print("Time: ");
  Serial.print(year);
  Serial.print('-');
  printTwoDigit(month);
  Serial.print('-');
  printTwoDigit(dateOfMonth);
  Serial.print('T');
  printTwoDigit(hour);
  Serial.print(':');
  printTwoDigit(minute);
  Serial.print(':');
  printTwoDigit(second);
  Serial.println();
}

void setup(void)
{
#if F_CPU >= 12000000UL
  Serial.begin(115200);
#else
  Serial.begin(9600);
#endif
  Serial.println("Read DS1307 example");
  Serial.print("    SDA pin: ");
  Serial.println(int(sdaPin));
  Serial.print("    SCL pin: ");
  Serial.println(int(sclPin));
  Serial.print("    I2C address: ");
  Serial.println(int(I2C_ADDRESS), HEX);
  sw.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
  sw.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
  sw.setDelay_us(5);
  sw.setTimeout(1000);
  sw.begin();
  readInterval.start(2000, AsyncDelay::MILLIS);
}


void loop(void)
{
  if (readInterval.isExpired()) {
    readTime();
    readInterval.restart();
  }
}

