#include <LiquidCrystal_I2C.h>

struct Student {
 String name;
 int matric;
};

LiquidCrystal_I2C lcd(0x20, 16, 2); //  Declare 1602 LCD screen confix at 0x20 address


void setup()
{
  lcd.init(); // Initialize lcd
  lcd.backlight(); // Turn on lcd backlight
  lcd.setCursor(0, 0); // Set the cursor at 0,0 (Top Left)
  lcd.print("Power On"); // Print to the Screen
  delay(1000); //  Wait for one second

}

void loop()
{
  Student student = {"Aina", 20171370};
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("New Student");
  lcd.setCursor(0, 1);
  lcd.print(student.name);
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to class");
  lcd.setCursor(0, 1);
  lcd.print("20171370 30-JUL");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Student 20171370");
  lcd.setCursor(9, 1);
  lcd.print("Deleted");
  delay(2000);
}