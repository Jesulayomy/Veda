#include <LiquidCrystal_I2C.h>
#include <string.h>

#define b1 2
#define b2 3
#define b3 4
#define b4 5

struct Student {
 String name;
 int matric;
};

LiquidCrystal_I2C lcd(0x20, 16, 2); //  Declare 1602 LCD screen confix at 0x20 address


void setup()
{
  pinMode(b1, INPUT); // Initialize buttons
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);
  Serial.begin(9600);
  lcd.init(); // Initialize lcd
  lcd.backlight();
  lcd.setCursor(0, 0); // Set the cursor at 0,0 (Top Left)
  lcd.print("Power On");
  Serial.println("Power On");
  delay(1000);
}

void loop() {
  Student student1 = {"Aina", 20171370};
  Student student2 = {"Adexy", 20171372};

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Stand By");
  Serial.println("Stand By");
  while ((digitalRead(b1) == LOW) && (digitalRead(b4) == LOW) && (digitalRead(b3) == LOW) && (digitalRead(b2) == LOW)) {
    delay(100);
  }

  while ((digitalRead(b1) == HIGH) || (digitalRead(b4) == HIGH) || (digitalRead(b2) == HIGH) || (digitalRead(b3) == HIGH)) {
  	if (digitalRead(b1) == HIGH) {
   	  Serial.println("Deleting a student");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(student2.name);
      lcd.setCursor(9, 1);
      lcd.print("Deleted");
      delay(100);
      continue;
  	}
  	else if (digitalRead(b2) == HIGH) {
      Serial.println("Registering newstudent");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("New Student");
      lcd.setCursor(0, 1);
      lcd.print(student1.name);
      delay(100);
  	  continue;
    }
  	else if (digitalRead(b3) == HIGH) {
      Serial.println("Reset was pressed");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reset Data");
      delay(100);
      continue;
  	}
  	else if (digitalRead(b4) == HIGH) {
      Serial.println("New finger Placed");
      Serial.println("Press b1 to Cancel or b2 to Register");
      while ((digitalRead(b1) == LOW) && (digitalRead(b3) == LOW) && (digitalRead(b2) == LOW)) {
        delay(100);
  	  }

      while ((digitalRead(b1) == HIGH) || (digitalRead(b2) == HIGH) || (digitalRead(b3) == HIGH)) {
  	    if (digitalRead(b1) == HIGH) {
   	      Serial.println("Cancel new student Registration");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Cancelled");
          delay(100);
  	    }
  	    else if (digitalRead(b2) == HIGH) {
          Serial.println("New student added");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Registration");
          lcd.setCursor(0, 1);r
          lcd.print("Successful");
          delay(100);
  	    }
  	    else if (digitalRead(b3) == HIGH) {
          Serial.println("Do not press Reset!");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Operation");
          lcd.setCursor(6, 1);
          lcd.print("Cancelled");
          delay(100);
  	    }
      }
  	}
  }
}