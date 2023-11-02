#include <Wire.h>

#include <LiquidCrystal_I2C.h>

// Try 0x3F and 0x27
int counter = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup(){
  lcd.init();   // initializing the LCD
  lcd.backlight();
  Serial.begin(9600);
  Serial.println("Counter:");
}
void loop(){
  Serial.println(counter);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ELE CLASS LIST "); // Start Printing
  lcd.setCursor(0, 1);
  lcd.print(counter);
  counter++;
  delay(500);
}
