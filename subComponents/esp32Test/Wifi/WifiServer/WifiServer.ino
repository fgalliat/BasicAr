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

bool wasNot = true;

void loop() {
    if ( true ) {
    //if ( btn() ) {
      //  while( btn() ) { delay(100); }

        DBUG("begin\n");

        if ( !telnet.isServerStarted() ) {
            if ( telnet.connectWifi() ) {
                telnet.open();
            }
        }


        while ( true ) { 
            telnet.runServerTick();

            if ( telnet.isClientConnected() ) {

                if (wasNot) {
                    telnet.print("Hi, Please press Enter Key...\n");
                    wasNot = false;
                }

                char* line = telnet.readLine();
                if ( line != NULL ) {
                    DBUG("Ya typed >>");DBUG(line);DBUG("<<\n");

                    if ( strcmp(line, "/quit") == 0 ) {
                        telnet.print("OK Bye\n");
                        wasNot = true;
                        telnet.close();
                        telnet.disconnectWifi();
                    }
                }
            }

            delay(250); 
        }

        DBUG("end\n");
    }
    delay(250);
}