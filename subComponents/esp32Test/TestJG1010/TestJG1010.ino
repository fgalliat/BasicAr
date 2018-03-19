      #define JG1010_PAD_OUT_P1 13
      #define JG1010_PAD_OUT_P2 12
      #define JG1010_PAD_OUT_P4 14
      // beware to not wire as INPUT_PULLUP @ same time
      #define JG1010_PAD_IN_P3 32
      #define JG1010_PAD_IN_P5 33

void onPin(int pin) { digitalWrite(pin, HIGH); }
void offPin(int pin) { digitalWrite(pin, LOW); }


void setup() {
    Serial.begin(115200);
    Serial.println("OK, go !");

    pinMode(JG1010_PAD_OUT_P1, OUTPUT); // )       
    pinMode(JG1010_PAD_OUT_P2, OUTPUT); //  | ---  output works !!!!
    pinMode(JG1010_PAD_OUT_P4, OUTPUT); // ) 
    pinMode(JG1010_PAD_IN_P3, INPUT);   // )__ input works too !!!!!!
    pinMode(JG1010_PAD_IN_P5, INPUT);   // )

    offPin(JG1010_PAD_OUT_P1);
    offPin(JG1010_PAD_OUT_P2);
    offPin(JG1010_PAD_OUT_P4);

    //========================================
    onPin(JG1010_PAD_OUT_P1);
    delay(500);
    offPin(JG1010_PAD_OUT_P1);
    delay(500);

    onPin(JG1010_PAD_OUT_P2);
    delay(500);
    offPin(JG1010_PAD_OUT_P2);
    delay(500);

    onPin(JG1010_PAD_OUT_P4);
    delay(500);
    offPin(JG1010_PAD_OUT_P4);
    delay(500);

    //========================================
    onPin(JG1010_PAD_OUT_P1);
    //========================================

    Serial.println("OK, completed !");
}


void loop() {
  if ( digitalRead(JG1010_PAD_IN_P3) == HIGH ) {
      Serial.println("P3");
  }
  if ( digitalRead(JG1010_PAD_IN_P5) == HIGH ) {
      Serial.println("P5");
  }
  delay(10);
}