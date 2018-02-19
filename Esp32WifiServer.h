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
     bool _isWifiConnected = false;
     bool _isServerStarted = false;
     bool _isClientConnected = false;

     static const int MAX_LINE_LEN = 256;
     char lineRead[MAX_LINE_LEN+1];

     // ==========================
     //WiFiMulti wifiMulti;
     WiFiMulti* wifiMulti;
     //WiFiServer server(23);
     WiFiServer* server;
     WiFiClient serverClients[MAX_SRV_CLIENTS];


    public:
      Esp32WifiServer() {
        _isWifiConnected = false;
        _isServerStarted = false;
        _isClientConnected = false;
      }
      ~Esp32WifiServer() {
        _isWifiConnected = false;
        _isServerStarted = false;
        _isClientConnected = false;
      }

      bool isWifiConnected()   { return _isWifiConnected; }
      bool isServerStarted()   { return _isServerStarted; }
      bool isClientConnected() { return _isClientConnected; }

      // connect to wifi 
      // reads SSIDs & PASSs from SPIFF FS
      bool connectWifi() {
        this->_isWifiConnected = false;

        bool ok = esp32.getFs()->openCurrentTextFile("/SSID.TXT");
        if ( !ok ) {
          DBUG("missing /SSID.TXT is missing !!\n");
          return false;
        }

        if (wifiMulti == NULL) wifiMulti = new WiFiMulti(); 

        char* line, *line2;
        while( (line = esp32.getFs()->readCurrentTextLine() ) != NULL ) {
          // trim TODO + file sanity check
          if ( strlen(line) == 0 ) { break; }
          //DBUG( line ); DBUG( "=>" );
          DBUG( "Registering SSID : " );DBUG( line );DBUG( "\n" ); 
          line2 = esp32.getFs()->readCurrentTextLine();
          wifiMulti->addAP( (const char*)line, (const char*)line2);
          // DBUG( line2 ); DBUG( "\n" );
        }
        esp32.getFs()->closeCurrentTextFile();

        DBUG("Connecting Wifi \n");
        for (int loops = 10; loops > 0; loops--) {
          if (wifiMulti->run() == WL_CONNECTED) {
            DBUG("WiFi connected \n");
            DBUG("IP address: ");
            
            //DBUG(WiFi.localIP());
            // BEWARE HERE !!!!!!!!!!!!!!!
            String str = WiFi.localIP().toString();
            int lstr = str.length();
            char ip[lstr+1];
            memset(ip, 0x00, lstr+1);
            str.toCharArray( ip, lstr+1 );
            // BEWARE HERE !!!!!!!!!!!!!!!
            DBUG((char*)ip);

            DBUG("\n");
            break;
          }
          else {
            DBUGi(loops); DBUG("\n");
            delay(1000);
          }
        }
        if (wifiMulti->run() != WL_CONNECTED) {
          DBUG("WiFi connect failed\n");
          delay(1000);
          //ESP.restart();
          return false;
        }

        this->_isWifiConnected = true;
        return true;
      }

      void disconnectWifi() {
        this->_isWifiConnected = false;
        WiFi.disconnect();
      }

      // open telnet server
      bool open() {
        if (server == NULL) server = new WiFiServer(23);
        
        if ( !this->_isWifiConnected ) {
          DBUG("NOT CONNECTED TO WIFI\n");
          return false;
        }
        if ( this->_isServerStarted ) {
          DBUG("SERVER ALREADY STARTED\n");
          return false;
        }
        this->_isServerStarted = false;
        this->_isClientConnected = false;

        server->begin();
        server->setNoDelay(true);

        DBUG("Ready! Use 'telnet ");

        //DBUG(WiFi.localIP());
        // BEWARE HERE !!!!!!!!!!!!!!!
        String str = WiFi.localIP().toString();
        int lstr = str.length();
        char ip[lstr+1];
        memset(ip, 0x00, lstr+1);
        str.toCharArray( ip, lstr+1 );
        // BEWARE HERE !!!!!!!!!!!!!!!
        DBUG((char*)ip);


        DBUG(" 23' to connect\n");

        this->_isServerStarted = true;
      }

      bool runServerTick() {
        if ( !( this->_isWifiConnected && this->_isServerStarted ) ) {
          return false;
        }

        uint8_t i;

        if (wifiMulti->run() == WL_CONNECTED) {
          //check if there are any new clients
          if (server->hasClient()){
            for(i = 0; i < MAX_SRV_CLIENTS; i++){
              //find free/disconnected spot
              if (!serverClients[i] || !serverClients[i].connected()){
                if(serverClients[i]) serverClients[i].stop();
                serverClients[i] = server->available();
                if (!serverClients[i]) Serial.println("available broken");
                DBUG("New client: ");
                DBUGi(i); DBUG(" ");

                // BEWARE HERE !!!!!!!!!!!!!!!
                String str = serverClients[i].remoteIP().toString();
                int lstr = str.length();
                char ip[lstr+1];
                memset(ip, 0x00, lstr+1);
                str.toCharArray( ip, lstr+1 );
                // BEWARE HERE !!!!!!!!!!!!!!!

                DBUG( (char*)ip ); DBUG("\n");
                this->_isClientConnected = true;
                break;
              }
            }
            if (i >= MAX_SRV_CLIENTS) {
              //no free/disconnected spot so reject
              server->available().stop();
            }
          }
        } else {
          DBUG("WiFi not connected!");
          for(i = 0; i < MAX_SRV_CLIENTS; i++) {
            if (serverClients[i]) serverClients[i].stop();
          }
          // delay(1000);
        }

        return true;
      }

      // close telnet server
      void close() {
        if ( !(_isWifiConnected && _isServerStarted) ) {
          return;
        }

        for(uint8_t i = 0; i < MAX_SRV_CLIENTS; i++) {
          if (serverClients[i]) serverClients[i].stop();
        }
        this->_isClientConnected = false;

        server->close();
        this->_isServerStarted = false;

        DBUG("Server closed")
      }

      char* readLine() {
        if ( !_isClientConnected ) { return NULL; }

        // check clients for data
        for(uint8_t i = 0; i < MAX_SRV_CLIENTS; i++){
          if (serverClients[i] && serverClients[i].connected()){
            if(serverClients[i].available()){
              // get data from the telnet client and push it to the UART
              // while(serverClients[i].available()) Serial1.write(serverClients[i].read());
              memset( lineRead, 0x00, MAX_LINE_LEN+1 );

              int cpt = 0;
              bool ok_break = false;
              while( cpt < MAX_LINE_LEN ) {
                while(serverClients[i].available()) { 
                  lineRead[cpt] = serverClients[i].read(); 
                  // DBUG( (int)lineRead[cpt] ); DBUG(' ');
                  if ( lineRead[cpt] == '\r' ) { lineRead[cpt] = 0x00; continue; }
                  if ( lineRead[cpt] == '\n' ) { lineRead[cpt] = 0x00; ok_break = true; break; }
                  cpt++;
                }
                if ( ok_break ) { break; }
              }

            } else {
              return NULL;
            }
          }
          else {
            if (serverClients[i]) {
              serverClients[i].stop();
            }
          }
        }

        return (char*)lineRead;
      }



      void  print(char* str) {
        if ( !_isClientConnected || str == NULL ) { return; }

        //push data to all connected telnet clients
        for(uint8_t i = 0; i < MAX_SRV_CLIENTS; i++){
          if (serverClients[i] && serverClients[i].connected()){
            serverClients[i].write(str, strlen( str ) );
            delay(1);
          }
        }
      }

  };


#endif
