#include <LiquidCrystal_I2C.h>
#include <string.h>

#define b1 2
#define b2 3
#define b3 4
#define b4 5
#define b5 6

struct Student {
 String name;
 long int matric;
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
  Student student3 = {"Kenny", 20171374};

  lcd.clear();
  lcd.print("Stand By");
  Serial.println("Stand By");

  // Wait till a button is pressed
  while ((digitalRead(b1) == LOW) && (digitalRead(b2) == LOW) && (digitalRead(b3) == LOW) && (digitalRead(b4) == LOW) && (digitalRead(b5) == LOW)) {
    delay(100);
  }

  // Perform  an action depending on which button is pressed
  while ((digitalRead(b1) == HIGH) || (digitalRead(b2) == HIGH) || (digitalRead(b3) == HIGH) || (digitalRead(b4) == HIGH) || (digitalRead(b5) == HIGH)) {
  	if (digitalRead(b1) == HIGH) {
   	  Serial.println("Deleting a student");
      lcd.clear();
      lcd.print(student2.name);
      lcd.setCursor(9, 1);
      lcd.print("Deleted");
      delay(1000);
      continue;
  	}
  	else if (digitalRead(b2) == HIGH) {
      Serial.println("Registering new student");
      lcd.clear();
      lcd.print("New Student");
      lcd.setCursor(0, 1);
      lcd.print(student1.name);
      delay(1000);
  	  continue;
    }
  	else if (digitalRead(b3) == HIGH) {
      Serial.println("Reset was pressed");
      lcd.clear();
      lcd.print("Resetting Data");
      delay(1000);
      continue;
  	}
  	else if (digitalRead(b4) == HIGH) {
      Serial.println("New finger Placed");
      Serial.println("Press b1 to Cancel or b2 to Register");
      lcd.clear();
      lcd.print("b1: Cancel");
      lcd.setCursor(0, 1);
      lcd.print("b2: Register");
      while ((digitalRead(b1) == LOW) && (digitalRead(b2) == LOW)) {
        delay(100);
  	  }

      while ((digitalRead(b1) == HIGH) || (digitalRead(b2) == HIGH)) {
  	    if (digitalRead(b1) == HIGH) {
   	      Serial.println("Cancel new student Registration");
          lcd.clear();
          lcd.print("Cancelled");
          delay(500);
  	    }
  	    else if (digitalRead(b2) == HIGH) {
          Serial.println("New student added");
          lcd.clear();
          lcd.print("Registration");
          lcd.setCursor(0, 1);
          lcd.print("Successful");
          delay(500);
  	    }
      }
  	}
  	else if (digitalRead(b5) == HIGH) {
      Serial.println("Admit existing student");
      lcd.clear();
      lcd.print("Welcome");
      lcd.setCursor(8, 0);
      lcd.print(student3.name);
      lcd.setCursor(0, 1);
      lcd.print(student3.matric);
      lcd.setCursor(9, 1);
      lcd.print("30-JUL");
      delay(1000);
      continue;
  	}
  }
}
