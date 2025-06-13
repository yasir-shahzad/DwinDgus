// Example 1: Basic Initialization and Reset
#include <DwinDgus.h>

DwinDgus lcd(Serial1);

void setup() {
  Serial1.begin(115200);
  lcd.begin();
  lcd.reset();
}

void loop() {
  // Nothing here
}
