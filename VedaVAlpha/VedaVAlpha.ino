/**********************************************************************************
 * © 2023 - AINA, Jesulayomi Michael                                              *
 * Please Include a copy of the MIT Licence in any copies of this project.        *
 *                                                                                *
 * https://github.com/Jesulayomy/Veda                                             *
 *                                                                                *
 *********************************************************************************/

#include <Adafruit_Fingerprint.h>  // Manages the fingerprint Library
#include <ArduinoHttpClient.h>  // Manages HTTP requests
#include <ArduinoJson.h>  // Parses json response from server
#include <LiquidCrystal_I2C.h>  // LCD display
#include <Wire.h>

#include "WiFiEsp.h"  // Initializes wifi module
#include "arduino_secrets.h"  // Contains Secrets (Wifi ssid and password)


#define ATD 4  // Attend button
#define CNC 5  // Cancel and Enroll Button
#define RST 8  // Reset Button
#define GRN 9  // Attend Indicator
#define YLW 10 // Enroll Indicator
#define RED 11 // Reset Indicator
#define PWR 12 // Power Indicator

#define RETRIES 10  // Number of times to scan prints before cancelling

#ifndef HAVE_HWSERIAL1
  #include "SoftwareSerial.h"
  SoftwareSerial Serial1(6, 7); // RX, TX
  #define ESP_BAUDRATE  19200  // Default esp communication baud rate
#endif

char ssid[] = SECRET_SSID;  // WiFi name from secrets
char pass[] = SECRET_PASS;  // WiFi password

// 172.20.10.3 is the Wireless LAN adapter Wi-Fi IPv4 Address (Required hotspot/router)
// PC server running at 0.0.0.0 can connect to this (Run 'ipconfig' in cmd to get it)
// 192.168.246.220 
char serverAddress[] = SERVER_ADDR;  // server address
int port = PORT;

SoftwareSerial mySerial(2, 3); // Declare serial communication to fingerprint module
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); // manage with Adafruit

WiFiEspClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);  //declare a client
int status = WL_IDLE_STATUS;

LiquidCrystal_I2C lcd(0x27, 16, 2);


uint16_t ID, nextID; // ID of attending user & nextID to be used when enrolling
bool failed = false;  //checks if the last operation failed

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
  lcd.noBacklight();

  Serial.begin(115200);  // Begin serial monitor communication

  finger.begin(57600);  // Initialize fingerprint
  setupFingerprint();

  lcd.backlight();
  lcd.print(F("Power On"));
  delay(1000);

  // handleEnroll();
  // Serial1.begin(ESP_BAUDRATE);
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
  	if (digitalRead(ATD) == LOW) { // A student attends
      digitalWrite(GRN, HIGH);
      handleAttend();
      if (ID != 0) {
        switchComms(false);
        HttpClient client = HttpClient(wifi, serverAddress, port);
        client.connectionKeepAlive();
        postInfo();
        client.stop();
        switchComms(true);
      }
      delay(2000);
  	  break;
    }
  	else if (digitalRead(RST) == LOW) { // Remove a student or reset the databases
      resetInfo();
      digitalWrite(YLW, HIGH);
      while ((digitalRead(RST) == HIGH) && (digitalRead(CNC) == HIGH) && (digitalRead(ATD) == HIGH)) {
        delay(100);
      }
      lcd.clear();
      if (digitalRead(CNC) == LOW) {
        lcd.print(F("Operation"));
        lcd.setCursor(7, 1);
        lcd.print(F("Cancelled"));
        digitalWrite(RED, HIGH);
      }
      else if (digitalRead(ATD) == LOW) {
        handleAttend();
        if (ID != 0) {
          HttpClient client = HttpClient(wifi, serverAddress, port);
          client.connectionKeepAlive();
          switchComms(false);
          deleteStudent();
          client.stop();
          switchComms(true);
          digitalWrite(GRN, HIGH);
        }
        else {
          lcd.clear();
          lcd.print(F("Failed!"));
          digitalWrite(RED, HIGH);
        }
      }
      else if (digitalRead(RST) == LOW) {
        finger.emptyDatabase();
        lcd.print(F("Sucessfully"));
        lcd.setCursor(11, 1);
        lcd.print(F("Reset"));
        HttpClient client = HttpClient(wifi, serverAddress, port);
        client.connectionKeepAlive();
        switchComms(false);
        client.del("/api/students");
        client.stop();
        switchComms(true);
        digitalWrite(GRN, HIGH);
      }
      delay(1000);
      break;
  	}
  	else if (digitalRead(CNC) == LOW) {
      digitalWrite(GRN, HIGH);
      handleEnroll();
      HttpClient client = HttpClient(wifi, serverAddress, port);
      client.connectionKeepAlive();
      switchComms(false);
      postStudent();
      client.stop();
      switchComms(true);
      break;
  	}
  }
  switchComms(true);
}

// Switches the serial communication
void switchComms(bool toFingerprint) {
  if (toFingerprint) {
    // Switch to fingerprint module
    WiFi.disconnect();
    Serial1.end();
    finger.begin(57600);
    lcd.clear();
    if (finger.verifyPassword()) {
      lcd.print(F("Sensor: Active"));
    } else {
      Serial.println(F("No sensor :("));
      lcd.print(F("No Sensor :("));
      while (1) { delay(1); }
    }
  } else {
    // Switch to ESP WiFi module
    mySerial.end(); // Close fingerprint module communication
    status = 0;
    Serial1.begin(ESP_BAUDRATE); // Set ESP module baud rate
    startWifi();
  }
}

// Starts the wifi connection
void startWifi() {
  WiFi.init(&Serial1);  // Initialize wifi

  while ( status != WL_CONNECTED) {
    Serial.print(F("Connecting to "));
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
}

// prints reset information 
void resetInfo () {
  lcd.clear();
  lcd.print(F("ATD: One Student"));
  lcd.setCursor(0, 1);
  lcd.print(F("RST: All Students"));
  delay(1000);
}

// Deletes the student from the database
void deleteStudent() {
  client.del("/api/students/" + String(ID));
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, response);

  Serial.print(F("Status code: "));
  Serial.println(statusCode);

  const char* matric_number = doc["matric_number"];

  lcd.clear();
  lcd.print(F("Removed "));
  lcd.print(matric_number);
  digitalWrite(GRN, HIGH);

  delay(2000);
}

// Handles the enrollment proedure
void handleEnroll() {
  lcd.clear();
  lcd.print(F("CNC: Cancel"));
  lcd.setCursor(0, 1);
  lcd.print(F("ATD: Enroll"));
  delay(1000);
  while ((digitalRead(ATD) == HIGH) && (digitalRead(CNC) == HIGH)) {
    delay(100);
  }

	if (digitalRead(CNC) == LOW) {
    lcd.clear();
    lcd.print(F("Operation"));
    lcd.setCursor(7, 1);
    lcd.print(F("Cancelled"));
    digitalWrite(YLW, HIGH);
    delay(2000);
	}
  else if (digitalRead(ATD) == LOW) {
    nextID = getNextID();
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

// Sends a new student ID to the api
void postStudent() {
  if (!failed) {
    lcd.clear();
    lcd.print(F("Successful!"));
    String contentType = "application/json";
    String postData = "{\"id\":" + String(nextID) + "}";
    client.post("/api/students", contentType, postData);
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    // Serial.println(F("Response:"));
    // Serial.println(response);
    digitalWrite(YLW, HIGH);
    delay(2000);
  }
  failed = false;
}

// Begin the loop
void preamble() {
  lcd.clear();
  lcd.print(F("<Attend Reset>"));
  lcd.setCursor(0, 1);
  // Attend, Cancel or Enroll, Reset
  lcd.print(F("Cancel/Enroll"));
  digitalWrite(GRN, LOW);
  digitalWrite(YLW, LOW);
  digitalWrite(RED, LOW);
}

// Handles attend button press
void handleAttend() {
  lcd.clear();
  lcd.print(F("Place finger!"));
  for (int i = 0; i < RETRIES; i++) {
    // ID = readnumber();
    ID = getFingerprintIDez();
    if (ID != 0) {
      break;
    }
    lcd.setCursor(i, 1);
    lcd.print(F("."));
    delay(500);
  }
  if (ID == 0) {
    lcd.clear();
    lcd.print(F("Could not detect"));
    digitalWrite(RED, HIGH);
  }
}


uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

// Handles the attend post request
void postInfo() {
  String contentType = "application/json";
  String postData = "{\"id\":" + String(ID) + "}";

  client.post("/api/attend", contentType, postData);
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print(F("Status code: "));
  Serial.println(statusCode);

  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, response);

  const char* first_name = doc["first_name"];
  const char* matric_number = doc["matric_number"];

  lcd.clear();
  lcd.print(F("Welcome "));
  lcd.print(matric_number);
  lcd.setCursor(0, 1);
  lcd.print(first_name);

  // Serial.println("Response: ");
  // Serial.println(response);
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
    Serial.println(F("No sensor :("));
    lcd.print(F("No Sensor :("));
    while (1) { delay(1); }
    return;
  }

  finger.getParameters();

  delay(2000);
  finger.getTemplateCount();
  if (finger.templateCount == 0) {
    lcd.clear();
    lcd.print(F("No data"));
  }
  else {
    lcd.clear();
    lcd.print(F("Students: "));
    lcd.setCursor(10, 0);
    lcd.print(finger.templateCount);
    lcd.setCursor(12, 0);
    lcd.print(F("/"));
    lcd.setCursor(13, 0);
    lcd.print(finger.capacity);
  }
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

// returns 0 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return 0;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return 0;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return 0;

  // found a match!
  lcd.clear();
  lcd.print(F("ID: "));
  lcd.print(finger.fingerID);
  lcd.setCursor(0, 1);
  lcd.print(F("Confidence: "));
  lcd.print(finger.confidence);

  return finger.fingerID;
}

// Enrolls a new id using nextID
uint8_t getFingerprintEnroll() {
  int p = -1;
  lcd.clear();
  lcd.print(F("Enrolling as # "));
  lcd.print(nextID);
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print(F("Place Finger"));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.print(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      lcd.print(F("."));
      Serial.print(F("No Finger"));
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
    lcd.print(F("."));
  }
  p = -1;
  lcd.setCursor(0, 1);
  lcd.print(F("Place same finger"));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      lcd.print(F("."));
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
      lcd.print(F("Image converted"));
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
  lcd.print(F("Model for #"));  lcd.print(nextID);

  p = finger.createModel();
  lcd.clear();
  if (p == FINGERPRINT_OK) {
    lcd.print(F("OK"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print(F("ERR"));
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.print(F("MISMATCH"));
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

  return true;
}
