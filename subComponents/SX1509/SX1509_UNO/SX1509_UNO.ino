#include <Wire.h> // Include the I2C library (required)
#include <SparkFunSX1509.h> // Include SX1509 library

SX1509 io; // Create an SX1509 object

// SX1509 pin definitions:
// Note: these aren't Arduino pins. They're the SX1509 I/O:
const int SX1509_LED_PIN = 15; // LED connected to 15 (source ing current)
//const int SX1509_BTN_PIN = 7; // Button connected to 0 (active-low)
const int SX1509_BTN_PIN = 0; // Button connected to 0 (active-low)

bool ledState = false;

const int LED_PIN = 13;

// internal LED
void int_led(bool state) {
  digitalWrite(LED_PIN, state ? HIGH : LOW);
}

void led(bool state) {
  int_led(state);
}

// 0-based
bool readGPIOBtn(int btn) {
  return io.digitalRead(SX1509_BTN_PIN + btn) == LOW;
}


void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT); // Use pin 13 LED as debug output
  digitalWrite(LED_PIN, LOW); // Start it as low

  // Call io.begin(<I2C address>) to initialize the I/O
  // expander. It'll return 1 on success, 0 on fail.
  if (!io.begin(0x3E)) {
    // If we failed to communicate, turn the pin 13 LED on
    led(true);
    while (1)
      ; // And loop forever.
  }

  // Call io.pinMode(<pin>, <mode>) to set any SX1509 pin as
  // either an INPUT, OUTPUT, INPUT_PULLUP, or ANALOG_OUTPUT
  io.pinMode(SX1509_LED_PIN, OUTPUT);

  // setup all GPIO pullup btns
  //io.pinMode(SX1509_BTN_PIN, INPUT_PULLUP);
  for(int btn=0; btn < 7; btn++) {
    io.pinMode(SX1509_BTN_PIN+btn, INPUT_PULLUP);
  }

  // Blink the LED a few times before we start:
  for (int i=0; i<5; i++) {
    // Use io.digitalWrite(<pin>, <LOW | HIGH>) to set an
    // SX1509 pin either HIGH or LOW:
    io.digitalWrite(SX1509_LED_PIN, HIGH);
    led(true);
    delay(100);
    io.digitalWrite(SX1509_LED_PIN, LOW);
    led(false);
    delay(100);
  }
}

void loop() {
  /*
  if (io.digitalRead(SX1509_BTN_PIN) == LOW) {

    // If the button is pressed toggle the LED:
    ledState = !ledState;
    io.digitalWrite(SX1509_LED_PIN, ledState);
    led(ledState);

    while (io.digitalRead(SX1509_BTN_PIN) == LOW)
      ; // Wait for button to release
  }
  */

  for(int btn=0; btn < 7; btn++) {
    if ( readGPIOBtn(btn) ) {
      Serial.print('#');
    } else {
      Serial.print('-');
    }

    if ( btn == 3 ) {
      Serial.print(' ');
      Serial.print('|');
    }

    Serial.print(' ');
  }
  Serial.println();

  delay( 300 );

}

