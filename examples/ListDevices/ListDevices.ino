#include <SoftWire.h>
#include <AsyncDelay.h>

SoftWire sw(SDA, SCL);

void setup(void)
{
  Serial.begin(9600);
  sw.setTimeout_ms(40);
  sw.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Set how long we are willing to wait for a device to respond
  sw.setTimeout_ms(200);

  const uint8_t firstAddr = 1;
  const uint8_t lastAddr = 0x7F;
  Serial.println();
  Serial.print("Interrogating all addresses in range 0x");
  Serial.print(firstAddr, HEX);
  Serial.print(" - 0x");
  Serial.print(lastAddr, HEX);
  Serial.println(" (inclusive) ...");
  
  for (uint8_t addr = firstAddr; addr <= lastAddr; addr++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delayMicroseconds(10);
    
    uint8_t startResult = sw.llStart((addr << 1) + 1); // Signal a read
    sw.stop();
    
    if (startResult == 0) {
      Serial.print("\rDevice found at 0x");
      Serial.println(addr, HEX);
      Serial.flush();
    }
    digitalWrite(LED_BUILTIN, LOW);
    
    delay(50);
  }
  Serial.println("Finished");

}

void loop(void)
{
  ;
}
