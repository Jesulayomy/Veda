/**********************************************************************************
 * © 2023 - AINA, Jesulayomi Michael                                              *
 * Please Include a copy of the MIT Licence in any copies of this project.        *
 *                                                                                *
 * https://github.com/Jesulayomy/Veda                                             *
 *                                                                                *
 *********************************************************************************/

#include <Adafruit_Fingerprint.h>  // Manages the fingerprint Library
#include <LiquidCrystal_I2C.h>  // LCD display
#include <Wire.h>

#include "WiFiEsp.h"  // Initializes wifi module
#include "arduino_secrets.h"  // Contains Secrets (Wifi ssid and password)

#include "SoftwareSerial.h"


#define ATD 4  // Attend button
#define CNC 5  // Cancel and Enroll Button
#define RST 8  // Reset Button
#define GRN 9  // Attend Indicator
#define YLW 10 // Enroll Indicator
#define RED 11 // Reset Indicator
#define PWR 12 // Power Indicator


char ssid[] = SECRET_SSID;  // WiFi name from secrets
char pass[] = SECRET_PASS;  // WiFi password

// server[] is the Wireless LAN adapter Wi-Fi IPv4 Address (Required hotspot/router)
// PC server running at 0.0.0.0 can connect to this (Run 'ipconfig' in cmd to get it)
// 192.168.246.220 // 192.168.68.220 // 172.20.10.3 // IP may change is router restarts
char server[] = SECRET_SERVER;  // server address
int port = 8080;

int status = WL_IDLE_STATUS;

uint16_t ID, nextID; // ID of attending user & nextID to be used when enrolling
bool failed = false;  //checks if the last operation failed

SoftwareSerial Serial1(6, 7); // RX, TX
SoftwareSerial mySerial(2, 3); // Declare serial communication to fingerprint module
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); // manage with Adafruit

// WifiClient for ESP module
WiFiEspClient client;

// Declare lcd handler
LiquidCrystal_I2C lcd(0x27, 16, 2);


// Setup functions
void setup() {
  pinMode(CNC, INPUT_PULLUP);  // Sets the pins as pullup inputs
  pinMode(ATD, INPUT_PULLUP);
  pinMode(RST, INPUT_PULLUP);
  pinMode(GRN, OUTPUT);
  pinMode(YLW, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(PWR, OUTPUT);
  digitalWrite(PWR, HIGH);

  lcd.init();  // Initialize lcd
  lcd.clear();
  lcd.backlight();
  lcd.print(F("Power On"));

  Serial.begin(115200);  // Begin serial monitor communication

  finger.begin(57600);  // Initialize fingerprint
  delay(200);
  setupFingerprint();
}

// Loop function
void loop() {
  preamble();

  // Wait till a button is pressed
  while ((digitalRead(ATD) == HIGH) && (digitalRead(RST) == HIGH) && (digitalRead(CNC) == HIGH)) {
    delay(100);
  }

  // Perform  an action depending on which button is pressed
  while ((digitalRead(ATD) == LOW) || (digitalRead(RST) == LOW) || (digitalRead(CNC) == LOW)) {
    digitalWrite(GRN, HIGH);
  	if (digitalRead(ATD) == LOW) { // A student attends
      finger.LEDcontrol(true);
      handleAttend();
      finger.LEDcontrol(false);
      // if ID is not 0 and the operation is not cancelled
      if (ID != 0 && failed != true) {
        lcd.setCursor(0, 1);
        lcd.print(F("Connecting..."));
        mySerial.end();
        Serial1.begin(19200); // // before wifiinit
        WiFi.init(&Serial1);
        if (WiFi.status() == WL_NO_SHIELD) {
          // don't continue
          while (true);
        }
        while(WiFi.status() != WL_CONNECTED){
          WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
        }
        lcd.setCursor(0, 1);
        lcd.print(F("Sending Data"));

        if (client.connect(server, 8080)) {
          // Make a HTTP request
          client.println("GET /api/students/" + String(ID) + " HTTP/1.1");
          client.println("Host: 192.168.68.220");
          client.println("Connection: close");
          client.println();
          String response = "";
          while (client.available()) {
            char c = client.read();
            response += c;
          }
          lcd.clear();
          lcd.print(F("Welcome"));
          lcd.setCursor(0, 1);
          lcd.print(F("> "));
          String firstName;
          if (extractFirstName(response, firstName)) {
            lcd.print(firstName);
          }
          else {
            lcd.print(ID);
          }
        }
        else {
          lcd.clear();
          lcd.print(F("Failed to update"));
          digitalWrite(RED, HIGH);
        }
        client.stop();

        if (client.connect(server, 8080)) {
          // send the HTTP POST request
          String content = "{\"id\":" + String(ID) +"}";
          client.println("POST /api/attend HTTP/1.1");
          client.println("Host: 192.168.68.220");
          client.println("Accept: */*");
          client.print("Content-Length: ");
          client.println(content.length());
          client.println("Content-Type: application/json");
          client.println();
          client.println(content);
        }
        client.stop();
        WiFi.disconnect();
        Serial1.end();
        finger.begin(57600);
      }
      delay(1000);
  	  break;
    }
  	else if (digitalRead(RST) == LOW) { // Remove a student or reset the databases
      resetInfo();
      while ((digitalRead(RST) == HIGH) && (digitalRead(CNC) == HIGH) && (digitalRead(ATD) == HIGH)) {
        delay(100);
      }
      lcd.clear();
      if (digitalRead(CNC) == LOW) {
        lcd.print(F("Operation"));
        lcd.setCursor(7, 1);
        lcd.print(F("Cancelled"));
        digitalWrite(YLW, HIGH);
        delay(500);
      }
      else if (digitalRead(ATD) == LOW) {
        finger.LEDcontrol(true);
        handleAttend();
        finger.LEDcontrol(false);
        if (ID != 0 && failed != true) {
          finger.deleteModel(ID);
          lcd.setCursor(0, 1);
          lcd.print(F("Connecting..."));
          mySerial.end();
          Serial1.begin(19200);
          WiFi.init(&Serial1);
          if (WiFi.status() == WL_NO_SHIELD) {
            // don't continue
            while (true);
          }
          while(WiFi.status() != WL_CONNECTED){
            WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
          }
          lcd.setCursor(0, 1);
          lcd.print(F("Sending Data"));

          if (client.connect(server, 8080)) {
            // Make a HTTP request
            client.println("DELETE /api/students/" + String(ID) + " HTTP/1.1");
            client.println("Host: 192.168.68.220");
            client.println("Connection: close");
            client.println();
            lcd.clear();
            lcd.print(F("Goodbye!"));
          }
          else {
            lcd.clear();
            lcd.print(F("Failed!"));
            digitalWrite(RED, HIGH);
          }
          client.stop();
          WiFi.disconnect();
          Serial1.end();
          finger.begin(57600);
        }
      }
      else if (digitalRead(RST) == LOW) {
        finger.LEDcontrol(true);
        finger.emptyDatabase();
        lcd.print(F("Sucessfully"));
        lcd.setCursor(11, 1);
        lcd.print(F("Reset"));
        finger.LEDcontrol(false);
        mySerial.end();
        Serial1.begin(19200);
        WiFi.init(&Serial1);
        if (WiFi.status() == WL_NO_SHIELD) {
          // don't continue
          while (true);
        }
        while(WiFi.status() != WL_CONNECTED){
          WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
        }
        lcd.setCursor(0, 1);
        lcd.print(F("Sending Data"));

        if (client.connect(server, 8080)) {
          // Make a HTTP request
          client.println("DELETE /api/students HTTP/1.1");
          client.println("Host: 192.168.68.220");
          client.println("Connection: close");
          client.println();
          lcd.clear();
          lcd.print(F("Database"));
          lcd.setCursor(7, 1);
          lcd.print(F("Reset!"));
        }
        else {
          lcd.clear();
          lcd.print(F("DB Failed!"));
          digitalWrite(RED, HIGH);
        }
        client.stop();
        WiFi.disconnect();
        Serial1.end();
        finger.begin(57600);
      }
      delay(1000);
      break;
  	}
  	else if (digitalRead(CNC) == LOW) {
      finger.LEDcontrol(true);
      handleEnroll();
      finger.LEDcontrol(false);
      if (nextID != 0 && failed != true) {
        lcd.setCursor(0, 1);
        lcd.print(F("Connecting..."));
        mySerial.end();
        Serial1.begin(19200);
        WiFi.init(&Serial1);
        if (WiFi.status() == WL_NO_SHIELD) {
          // don't continue
          while (true);
        }
        while(WiFi.status() != WL_CONNECTED){
          WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
        }
        lcd.setCursor(0, 1);
        lcd.print(F("Sending Data"));

        if (client.connect(server, 8080)) {
          // send the HTTP POST request
          String content = "{\"id\":" + String(nextID) +"}";
          client.println("POST /api/students HTTP/1.1");
          client.println("Host: 192.168.68.220");
          client.println("Accept: */*");
          client.print("Content-Length: ");
          client.println(content.length());
          client.println("Content-Type: application/json");
          client.println();
          client.println(content);
          lcd.clear();
          lcd.print(F("Successful!"));
          lcd.setCursor(0, 1);
          lcd.print("Please Attend...");
        }
        client.stop();
        WiFi.disconnect();
        Serial1.end();
        finger.begin(57600);
      }
      delay(1000);
      break;
  	}
  }
  failed = false;
}

// Extract the first name form the json string
bool extractFirstName(const String &json, String &result) {
  int startIndex = json.indexOf("\"first_name\":\"");
  if (startIndex == -1) {
    return false; // "first_name" not found
  }
  startIndex += 14; // Move past the key

  int endIndex = json.indexOf('"', startIndex);
  if (endIndex == -1) {
    return false; // No closing double quote found
  }

  result = json.substring(startIndex, endIndex);
  return true;
}

// prints reset information 
void resetInfo () {
  lcd.clear();
  lcd.print(F("ATD: One Student"));
  lcd.setCursor(0, 1);
  lcd.print(F("RST: All Students"));
  delay(500);
}

// Handles the enrollment proedure
void handleEnroll() {
  lcd.clear();
  lcd.print(F("CNC: Cancel"));
  lcd.setCursor(0, 1);
  lcd.print(F("ATD: Enroll"));
  delay(500);
  while ((digitalRead(ATD) == HIGH) && (digitalRead(CNC) == HIGH)) {
    delay(100);
  }

	if (digitalRead(CNC) == LOW) {
    lcd.clear();
    lcd.print(F("Operation"));
    lcd.setCursor(7, 1);
    lcd.print(F("Cancelled"));
    failed = true;
    digitalWrite(YLW, HIGH);
    delay(1000);
	}
  else if (digitalRead(ATD) == LOW) {
    nextID = getNextID();
    lcd.clear();
    lcd.print(F("Enrolling # "));
    lcd.print(nextID);
    while (!getFingerprintEnroll()) {
      if (digitalRead(CNC) == LOW) {
        lcd.clear();
        lcd.print(F("Operation"));
        lcd.setCursor(7, 1);
        lcd.print(F("Cancelled"));
        digitalWrite(YLW, HIGH);
        failed = true;
        break;
      }
    }
    
  }
}

// Begin the loop
void preamble() {
  lcd.clear();
  lcd.print(F("Attend     Reset"));
  lcd.setCursor(0, 1);
  lcd.print(F("     Enroll     "));
  // Attend, Cancel or Enroll, Reset
  digitalWrite(GRN, LOW);
  digitalWrite(YLW, LOW);
  digitalWrite(RED, LOW);
  finger.LEDcontrol(false);
}

// Handles attend button press
void handleAttend() {
  lcd.clear();
  lcd.print(F("Place finger..."));
  
  while (true) {
    // ID = readnumber();
    ID = getFingerprintID();
    if (ID != 0) {
      break;
    }
    if (digitalRead(CNC) == LOW) {
      lcd.clear();
      lcd.print(F("Operation"));
      lcd.setCursor(7, 1);
      lcd.print(F("Cancelled"));
      digitalWrite(YLW, HIGH);
      failed = true;
      break;
    }
  }
  if (ID == 0 && !failed) {
    lcd.clear();
    lcd.print(F("Could not detect"));
    digitalWrite(RED, HIGH);
  }
  
}

/**
 * Verifies the sensor is connected and prints the number of templates
 * on No sensor, check pins connection and 3v3
 */
void setupFingerprint() {
  lcd.clear();
  if (finger.verifyPassword()) {
    lcd.print(F("Sensor: Active"));
  } else {
    lcd.print(F("No Sensor :("));
    while (1) { delay(1); }
    return;
  }

  finger.getParameters();

  delay(500);
  finger.getTemplateCount();
  lcd.clear();
  // Prnt the amount of data stored in the fingerprint sensor
  if (finger.templateCount == 0) {
    lcd.print(F("No data"));
  }
  else {
    lcd.print(F("Students: "));
    lcd.setCursor(10, 0);
    lcd.print(finger.templateCount);
    lcd.setCursor(12, 0);
    lcd.print(F("/"));
    lcd.setCursor(13, 0);
    lcd.print(300);
    delay(1000);
  }
  finger.LEDcontrol(false);
}

// Gets the next available ID to use in storing a student
uint16_t getNextID() {
  uint16_t model = 1;
  // Loop while model is <= the total existing templates
  while (model <= 255) {
    // if there is a model at that number, check the next one
    if (finger.loadModel(model) == FINGERPRINT_OK) {
      model++;
      continue;
    }
    else {
      // if there is no model at that number, use the number
      return model;
    }
  }
  return 0;
}

// Enrolls a new id using nextID
uint8_t getFingerprintEnroll() {
  int p = -1;
  failed = true;
  while (p != FINGERPRINT_OK) {
    if (digitalRead(CNC) == LOW) {
      return;
    }
    p = finger.getImage();
    lcd.setCursor(0, 1);
    lcd.print(F("Place Finger..."));
    switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(F("No Finger"));
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.print(F("Packet"));
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println(F("Image Fail"));
      break;
    default:
      Serial.println(F("UNKNOWN"));
      break;
    }
    delay(100);
  }

  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(F("Remove finger"));
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(F("Place same finger"));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      break;
    case FINGERPRINT_IMAGEFAIL:
      break;
    default:
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }

  // OK converted!

  p = finger.createModel();
  lcd.clear();
  if (p == FINGERPRINT_OK) {
    lcd.print(F("OK"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print(F("ERR"));
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.print(F("MISMATCH"));
    delay(250);
    return p;
  } else {
    lcd.print(F("ERR"));
    return p;
  }
  lcd.clear();
  p = finger.storeModel(nextID);
  if (p == FINGERPRINT_OK) {
    lcd.print(F("Stored!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print(F("Failed to store!"));
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.print(F("Failed to store!"));
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.print(F("Failed to store!"));
    return p;
  } else {
    return p;
  }

  failed = false;
  return true;
}

// Returns the ID or 0 if there is an error
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print(F("Captured"));
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger");
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Comm error");
      return 0;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return 0;
    default:
      Serial.println("Unknown err");
      return 0;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Comm error");
      return 0;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No features");
      return 0;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No features2");
      return 0;
    default:
      Serial.println("Unknown error");
      return 0;
  }
  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Comm error");
    return 0;
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    lcd.print(F("No match"));
    Serial.println("No match");
    return 0;
  } else {
    Serial.println("Unknown error");
    return 0;
  }

  lcd.clear();
  lcd.print(F("Found ID #")); lcd.print(finger.fingerID);

  return finger.fingerID;
}
