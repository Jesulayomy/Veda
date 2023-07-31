int b1 = 2;
int b2 = 3;
int b3 = 4;


void setup() {
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  Serial.begin(9600);
}

void loop() {
  while ((digitalRead(b1) == LOW) && (digitalRead(b3) == LOW) && (digitalRead(b2) == LOW)) {
    Serial.println("Released");
    delay(1000);
  }

  while ((digitalRead(b1) == HIGH) || (digitalRead(b2) == HIGH) || (digitalRead(b3) == HIGH)) {
    Serial.println("Pressed");
    delay(1000);
  }
}

void onPress() {
  Serial.println("Pressed a button"); 
}