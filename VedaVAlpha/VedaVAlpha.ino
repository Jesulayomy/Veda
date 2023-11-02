#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <Wire.h>

#define CNC 5
#define REG 4
#define RST 8

#define ERR 0
#define RETRIES 20

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


struct Student {
 int ID;
 String name;
 long int matric;
};

uint8_t ID, nextID;
bool fail = false;

void setup() {
  pinMode(CNC, INPUT_PULLUP);
  pinMode(REG, INPUT_PULLUP);
  pinMode(RST, INPUT_PULLUP);

  Serial.begin(115200);
  finger.begin(57600);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);

  Serial.println(" ===== ATTENDANCE SYSTEM ===== ");
  lcd.print("Power On");
  delay(2000);

  setupFingerprint();
  delay(2000);
}


void loop() {
  lcd.clear();
  lcd.print("Stand By");
  lcd.setCursor(0, 1);
  lcd.print("REG CNC/ENR RST");
  Serial.println("Stand By");
  Serial.println("Register | Cancel or Enroll | Reset");

  // Wait till a button is pressed
  while ((digitalRead(REG) == HIGH) && (digitalRead(RST) == HIGH) && (digitalRead(CNC) == HIGH)) {
    delay(100);
  }

  // Perform  an action depending on which button is pressed
  while ((digitalRead(REG) == LOW) || (digitalRead(RST) == LOW) || (digitalRead(CNC) == LOW)) {
  	if (digitalRead(REG) == LOW) {
      lcd.clear();
      lcd.print("Place finger ...");
      for (int i = 0; i < RETRIES; i++) {
        ID = getFingerprintID();
        if (ID != ERR) {
          break;
        }
        delay(500);
      }
      if (ID == ERR) {
        Serial.println("Error Reading");
        lcd.clear();
        lcd.print("Could not detect");
        break;
      }
      Serial.println("Attending Class");
      lcd.clear();
      lcd.print("02-NOV-23 | #");
      lcd.print(ID);
      lcd.setCursor(0, 1);
      lcd.print("Validated");
      delay(2000);
  	  continue;
    }
  	else if (digitalRead(RST) == LOW) {
      Serial.println("Reset key was pressed");
      lcd.clear();
      lcd.print("CNC: Cancel");
      lcd.setCursor(0, 1);
      lcd.print("RST: Reset");
      while ((digitalRead(RST) == HIGH) && (digitalRead(CNC) == HIGH)) {
        delay(100);
      }
      lcd.clear();
      if (digitalRead(CNC) == LOW) {
        lcd.print("Operation");
        lcd.setCursor(7, 1);
        lcd.print("Cancelled");
        Serial.println("Operation cancelled");
      }
      else if (digitalRead(RST) == LOW) {
        lcd.print("Sucessfully");
        lcd.setCursor(11, 1);
        lcd.print("Reset");
        Serial.println("Database reset");
        // finger.emptyDatabase();
      }
      delay(2000);
      continue;
  	}
  	else if (digitalRead(CNC) == LOW) {
      Serial.println("Enrolling new student");
      Serial.println("Press CNC to Cancel or REG to Register");
      lcd.clear();
      lcd.print("CNC: Cancel");
      lcd.setCursor(0, 1);
      lcd.print("REG: Register");
      delay(1000);
      while ((digitalRead(REG) == HIGH) && (digitalRead(CNC) == HIGH)) {
        delay(100);
  	  }

	    if (digitalRead(CNC) == LOW) {
   	    Serial.println("Cancel new student Registration");
        lcd.clear();
        lcd.print("Operation");
        lcd.setCursor(7, 1);
        lcd.print("Cancelled");
        delay(2000);
	    }
  	  else if (digitalRead(REG) == LOW) {
        Serial.println("New student registration");

        nextID = getNextID();
        while (!getFingerprintEnroll()) {
          if (digitalRead(CNC) == LOW) {
            lcd.clear();
            lcd.print("Operation");
            lcd.setCursor(7, 1);
            lcd.print("Cancelled");
            fail = true;
            delay(2000);
            break;
          }
        }
        if (!fail) {
          lcd.print("Successful!");
          delay(2000);
        }
        fail = false;
      }
  	}
  }
}


void setupFingerprint() {
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.print("Sensor: Active");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.clear();
    lcd.print("No Sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  delay(2000);
  finger.getTemplateCount();
  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data.");
    lcd.clear();
    lcd.print("No data");
  }
  else {
    Serial.print("Sensor contains "); Serial.print(finger.templateCount);
    Serial.print(" of "); Serial.print(finger.capacity); Serial.println(" templates");
    lcd.clear();
    lcd.print("Students: ");
    lcd.setCursor(11, 0);
    lcd.print(finger.templateCount);
    lcd.setCursor(13, 0);
    lcd.print("/");
    lcd.setCursor(14, 0);
    lcd.print(finger.capacity);
    nextID = finger.templateCount;
  }
}


int getNextID() {
  int m = 1;
  while (m < finger.templateCount) {
    if (finger.loadModel(m) == FINGERPRINT_OK) {
      m++;
    }
    else {
      return m;
    }
  }
  return m;
}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return ERR;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return ERR;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return ERR;
    default:
      Serial.println("Unknown error");
      return ERR;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return ERR;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return ERR;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return ERR;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return ERR;
    default:
      Serial.println("Unknown error");
      return ERR;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return ERR;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return ERR;
  } else {
    Serial.println("Unknown error");
    return ERR;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  lcd.clear();
  lcd.print("Found ID #"); lcd.print(finger.fingerID);
  lcd.print("Confidence: "); lcd.print(finger.confidence);

  return finger.fingerID;
}


uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(nextID);
  lcd.clear();
  lcd.print("Enrolling as # ");
  lcd.print(nextID);
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Place Finger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  lcd.setCursor(0, 1);
  lcd.print("Remove finger");
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    Serial.print(".");
  }
  p = -1;
  Serial.println("Place same finger again");
  lcd.setCursor(0, 1);
  lcd.print("Place same finger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(nextID);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  p = finger.storeModel(nextID);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
