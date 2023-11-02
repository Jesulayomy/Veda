#include <ESP8266WiFi.h>

#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();   // initializing the LCD
  lcd.backlight();
  lcd.clear();
  Serial.begin(9600);
  Serial.println();

  WiFi.begin("Layomi", "1be36f1AINA");
  Serial.print("Connecting");
  lcd.print("Conecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println();
  lcd.clear();
  Serial.print("Connected, IP Address: ");
  lcd.print("Connected, IP: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  Serial.println(WiFi.localIP());
}

void loop() {}
