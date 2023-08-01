#include <LiquidCrystal_I2C.h>
#include <string.h>

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

/* Control Unit
 * From ../control/control unit.cpp
 */
int b1 = 2;
int b2 = 3;
int b3 = 4;
int b4 = 5;


void setup() {
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println("Released");
  while ((digitalRead(b1) == LOW) && (digitalRead(b4) == LOW) && (digitalRead(b3) == LOW) && (digitalRead(b2) == LOW)) {
    delay(100);
  }

  while ((digitalRead(b1) == HIGH) || (digitalRead(b4) == HIGH) || (digitalRead(b2) == HIGH) || (digitalRead(b3) == HIGH)) {
  	if (digitalRead(b1) == HIGH) {
   	  Serial.println("Delete was pressed");
      delay(100);
      continue;
  	}
  	else if (digitalRead(b2) == HIGH) {
      Serial.println("Register was pressed");
      delay(100);
  	  continue;
    }
  	else if (digitalRead(b3) == HIGH) {
      Serial.println("Reset was pressed");
      delay(100);
      continue;
  	}
  	else if (digitalRead(b4) == HIGH) {
      Serial.println("New finger Placed");
      Serial.println("Press b1 to Delete or b2 to Register");
      while ((digitalRead(b1) == LOW) && (digitalRead(b3) == LOW) && (digitalRead(b2) == LOW)) {
        delay(100);
  	  }
      while ((digitalRead(b1) == HIGH) || (digitalRead(b4) == HIGH) || (digitalRead(b2) == HIGH) || (digitalRead(b3) == HIGH)) {
  	    if (digitalRead(b1) == HIGH) {
   	      Serial.println("Delete was pressed");
          delay(100);
  	    }
  	    else if (digitalRead(b2) == HIGH) {
          Serial.println("Register was pressed");
          delay(100);
  	    }
  	    else if (digitalRead(b3) == HIGH) {
          Serial.println("Do not press Reset!");
          delay(100);
  	    }
      }
  	}
  }
}