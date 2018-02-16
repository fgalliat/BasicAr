#ifndef ESP32WIFISRV_H_
#define ESP32WIFISRV_H_ 1

  /*
  * requires : Esp32Pocketv2 esp32
  */

#include <WiFi.h>
#include <WiFiMulti.h>

//how many clients should be able to telnet to this ESP32
#define MAX_SRV_CLIENTS 1



  class Esp32WifiServer {
    private:
     bool isWifiConnected = false;
     bool isClientConnected = false;

     // ==========================
     WiFiMulti wifiMulti;
     //WiFiServer server(23);
     WiFiServer* server;
     WiFiClient serverClients[MAX_SRV_CLIENTS];


    public:
      Esp32WifiServer() {
        server = new WiFiServer(23);
      }
      ~Esp32WifiServer() {}

      // connect to wifi & open telnet server
      // reads SSIDs & PASSs from SPIFF FS
      bool open() {
        bool ok = esp32.getFs()->openCurrentTextFile("/SSID.TXT");
        if ( !ok ) {
          DBUG("missing /SSID.TXT is missing !!\n");
          return false;
        }
        char* line, *line2;
        while( (line = esp32.getFs()->readCurrentTextLine() ) != NULL ) {
          // trim TODO + file sanity check
          if ( strlen(line) == 0 ) { break; }
          DBUG( line );
          DBUG( "=>" );
          line2 = esp32.getFs()->readCurrentTextLine();
          wifiMulti.addAP( (const char*)line, (const char*)line2);
          DBUG( line2 );
          DBUG( "\n" );
        }
        esp32.getFs()->closeCurrentTextFile();

        DBUG("Connecting Wifi \n");
        for (int loops = 10; loops > 0; loops--) {
          if (wifiMulti.run() == WL_CONNECTED) {
            DBUG("WiFi connected \n");
            DBUG("IP address: ");
            DBUG(WiFi.localIP());
            DBUG("\n");
            break;
          }
          else {
            DBUG(loops); DBUG("\n");
            delay(1000);
          }
        }
        if (wifiMulti.run() != WL_CONNECTED) {
          DBUG("WiFi connect failed\n");
          delay(1000);
          //ESP.restart();
          return false;
        }

        server->begin();
        server->setNoDelay(true);

        DBUG("Ready! Use 'telnet ");
        DBUG(WiFi.localIP());
        DBUG(" 23' to connect\n");

        return true;
      }

      // close telnet server
      void close();

      char* readLine();
      void  print(char* str);
  };


#endif
