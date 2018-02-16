/****
 * 
 * Simple telnet server API sample
 * 
 * 
 */

#define DBUG(a) { Serial.print(a); }

#define ESP32PCKv2
#include "ESP32_Pocketv2.h"
Esp32Pocketv2 esp32;

#include "Esp32WifiServer.h"
Esp32WifiServer telnet;

// ===============
#define LED 2
#define BTN 0

bool btn() {
  return digitalRead( BTN ) == LOW;
}

void led(bool state) {
  return digitalWrite( LED, state ? HIGH : LOW );
}



void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    pinMode(BTN, INPUT_PULLUP);
    //===================================

    DBUG("setup...");
    esp32.setup();
    DBUG("done\n");
}

void loop() {
    if ( true ) {
    //if ( btn() ) {
      //  while( btn() ) { delay(100); }

        DBUG("begin\n");

        telnet.open();


        DBUG("end\n");

        while ( true ) { delay(100); }
    }
    delay(250);
}