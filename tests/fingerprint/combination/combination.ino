#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);
#else
#define mySerial Serial1
#endif

#define del 7
#define reg 8
#define rst 4
#define newf 5
#define attend 6


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t id;

void setup() {
  // Start serial communication
  Serial.begin(9600);
  Serial.println("Attendance system");

  // Initialize buttons
  pinMode(del, INPUT_PULLUP);
  pinMode(reg, INPUT_PULLUP);
  pinMode(rst, INPUT_PULLUP);
  pinMode(newf, INPUT_PULLUP);
  pinMode(attend, INPUT_PULLUP);

  // Initialize lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Power On");
  Serial.println("Power On");

  // Start fingerprint
  finger.begin(57600);
  if (finger.verifyPassword()) {
      Serial.println("Fingerprint Sensor Connected");
      lcd.clear();
      lcd.print("Sensor Connected");
  } else {
      Serial.println("Fingerprint Sensor not Connected");
      lcd.clear();
      lcd.print("Sensor not found");
      lcd.setCursor(0, 1);
      lcd.print("Check Connection");
      while (1) { delay(10); }
  }

  Serial.println(F("<===== Sensor Parameters =====>"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  Serial.println(F("<===== Sensor Parameters =====>"));

  finger.getTemplateCount();
  lcd.print("Students: ");
  lcd.setCursor(11, 0);
  lcd.print(finger.templateCount);
  lcd.setCursor(13, 0);
  lcd.print("/");
  lcd.setCursor(14, 0);
  lcd.print(finger.capacity);
  id = finger.templateCount;

  delay(1000);
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
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
  return 0;
}

void loop() {
  // Display standby message
  uint8_t fingerID = -1;
  uint8_t enroll = 0;
  lcd.clear();
  lcd.print("Stand By");
  delay(1000);
  lcd.clear();
  lcd.print("Press Button");
  Serial.println("Stand By");

  // Wait till a button is pressed
  while ((digitalRead(del) == HIGH) && (digitalRead(reg) == HIGH) && (digitalRead(rst) == HIGH) && (digitalRead(newf) == HIGH) && (digitalRead(attend) == HIGH)) {
    delay(100);
    Serial.print(".");
  }

  // Perform  an action depending on which button is pressed
  while ((digitalRead(del) == LOW) || (digitalRead(reg) == LOW) || (digitalRead(rst) == LOW) || (digitalRead(newf) == LOW) || (digitalRead(attend) == LOW)) {
  	// Delete a student
    if (digitalRead(del) == LOW) {
   	  Serial.println("Deleting a student");
      fingerID = getFingerprintID();
      while (fingerID == -1) {
        delay(500);
        fingerID = getFingerprintID();
      }
      deleteFingerprint(fingerID);
      lcd.clear();
      // lcd.print(student2.name);
      // lcd.setCursor(9, 1);
      lcd.print("Deleted");
      delay(1000);
      continue;
  	} // Register a new student
  	else if (digitalRead(reg) == LOW) {
      Serial.println("Registering new student");
      lcd.clear();
      lcd.print("New Student");
      id = getNextID();
      enroll = getFingerprintEnroll();
      if (enroll) {
        lcd.setCursor(0, 1);
        lcd.print("ID: ");
        lcd.print(id);
      }
      else {
        lcd.setCursor(0, 1);
        lcd.print("Enroll Failed");
      }
      // lcd.setCursor(0, 1);
      // lcd.print(student1.name);
      delay(1000);
  	  continue;
    } // Reset everything
  	else if (digitalRead(rst) == LOW) {
      Serial.println("Reset was pressed, Clearing database");
      lcd.clear();
      lcd.print("Resetting Data");
      id = 1;
      finger.emptyDatabase();
      delay(1000);
      continue;
  	} // Add new student
  	// else if (digitalRead(newf) == LOW) {
    //   Serial.println("New finger Placed");
    //   Serial.println("Press del to Cancel or reg to Register");
    //   lcd.clear();
    //   lcd.print("del: Cancel");
    //   lcd.setCursor(0, 1);
    //   lcd.print("reg: Register");
    //   while ((digitalRead(del) == HIGH) && (digitalRead(reg) == HIGH)) {
    //     delay(100);
  	//   }

    //   while ((digitalRead(del) == LOW) || (digitalRead(reg) == LOW)) {
  	//     if (digitalRead(del) == LOW) {
   	//       Serial.println("Cancel new student Registration");
    //       lcd.clear();
    //       lcd.print("Cancelled");
    //       delay(500);
  	//     }
  	//     else if (digitalRead(reg) == LOW) {
    //       Serial.println("New student added");
    //       lcd.clear();
    //       lcd.print("Registration");
    //       lcd.setCursor(0, 1);
    //       lcd.print("Successful");
    //       delay(500);
  	//     }
    //   }
  	// } // Admit for today
  	else if (digitalRead(attend) == LOW) {
      Serial.println("Admit existing student");
      fingerID = getFingerprintID();
      while (fingerID == -1) {
        delay(500);
        fingerID = getFingerprintID();
      }
      lcd.clear();
      lcd.print("Welcome to class");
      lcd.setCursor(0, 1);
      lcd.print(fingerID);
      // lcd.setCursor(8, 0);
      // lcd.print(student3.name);
      // lcd.setCursor(0, 1);
      // lcd.print(student3.matric);
      // lcd.setCursor(9, 1);
      // lcd.print("30-JUL");
      delay(1000);
      continue;
  	}
  }
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
    lcd.clear();
    lcd.print("Deleted");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    lcd.clear();
    lcd.print("Comm Error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    lcd.clear();
    lcd.print("Bad Location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    lcd.clear();
    lcd.print("Flash Error");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    lcd.clear();
    lcd.print("Unknown Error");
    lcd.setCursor(0, 1);
    lcd.print(p);
  }
  return p;
}

uint8_t getFingerprintID() {
  // get a fingerprint image, it stores in the Adafruit_Fingerprint finger
  uint8_t p = finger.getImage();
  // 0 OK, else Not ok
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.clear();
      lcd.print("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      lcd.clear();
      lcd.print("No finger");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.clear();
      lcd.print("Comm Error");
      return -1;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      lcd.clear();
      lcd.print("Imaging Error");
      return -1;
    default:
      Serial.println("Unknown error");
      lcd.clear();
      lcd.print("Unknown Error");
      return -1;
  }

  // OK success!
  // Changes the image to a template
  p = finger.image2Tz();
  // 0 OK, else Not ok
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return -1;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return -1;
    default:
      Serial.println("Unknown error");
      return -1;
  }

  // OK converted!
  // find the ID of the fingerprint
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    lcd.clear();
    lcd.print("Match Found");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    lcd.clear();
    lcd.print("Comm Error");
    return -1;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.clear();
    lcd.print("No Match");
    return -1;
  } else {
    Serial.println("Unknown error");
    lcd.clear();
    lcd.print("Unknown Error");
    return -1;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  lcd.clear();
  lcd.print("ID: ");
  lcd.setCursor(4, 0);
  lcd.print(finger.fingerID);
  lcd.setCursor(0, 1);
  lcd.print("Confidence: ");
  lcd.setCursor(12, 1);
  lcd.print(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  lcd.clear();
  lcd.print("ID: ");
  lcd.print(finger.fingerID);
  lcd.setCursor(0, 1);
  lcd.print("Confidence: ");
  lcd.print(finger.confidence);

  return finger.fingerID;
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
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
    delay(100);
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
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
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
  Serial.print("Creating model for #");  Serial.println(id);

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

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
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
