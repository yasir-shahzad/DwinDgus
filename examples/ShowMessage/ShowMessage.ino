// Example 2: Show Message on Page
#include <DwinDgus.h>

DwinDgus lcd(Serial1);

void setup() {
  Serial1.begin(115200);
  lcd.begin();
  lcd.showMessage(1, "Hello, DWIN!");
}

void loop() {
  // Nothing here
}
