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