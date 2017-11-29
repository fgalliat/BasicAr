// SoftwareSerial RX 27 not working
// but Serial & Serial1 are NOT the same port !!!
//#include <SoftwareSerial.h>


//#define rxPin 26
//#define txPin 27
#define rxPin 27
#define txPin 26
#define ledPin 13

// set up a new serial port
//SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
#define mySerial Serial1
byte pinState = 0;

void setup()  {
  Serial.begin(115200);
  
  // define pin modes for tx, rx, led pins:
  pinMode(ledPin, OUTPUT);
  
  Serial1.setTX(txPin);
  Serial1.setRX(rxPin);

  mySerial.begin(115200);
  //mySerial.listen();
  mySerial.println("Hello World - SoftwareSerial");

  Serial.println("Hello World - USBSerial");
}

void loop() {
/*
  while ( Serial.available() > 0 ) { 
    char ch = (char)Serial.read();
    mySerial.print( ch );
    Serial.print( ch );
    toggle(ledPin);
  }
*/

  while ( mySerial.available() > 0 ) { 
    char ch = (char)mySerial.read();
    mySerial.print( '$' ); mySerial.print( ch );
    Serial.print( '*' ); Serial.print( ch );
    toggle(ledPin);
  }

}

void toggle(int pinNum) {
  digitalWrite(pinNum, pinState); 
  pinState = !pinState;
}
