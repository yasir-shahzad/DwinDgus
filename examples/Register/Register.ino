// Example 3: Set and Get Register
#include <DwinDgus.h>

DwinDgus lcd(Serial1);

void setup() {
  Serial1.begin(115200);
  lcd.begin();
  lcd.setRegister(0x1000, 1234); // Set register at 0x1000 to 1234
  delay(100);
  uint16_t value = lcd.getRegister(0x1000); // Read back the value
  Serial.begin(9600);
  Serial.print("Register 0x1000 value: ");
  Serial.println(value);
}

void loop() {
  // Nothing here
}
