#include "xts_arch.h"

#if not defined(BOARD_SND)
  // NO MP3 SUPPORT ...
#else
  // Xts-uPocket v2 -or- other MCUs

/************
 * DFPlayer mp3 module - Sound Layer for Xts-uBasic 
 * Xtase - fgalliat @ Nov. 2017
 * 
 * requires : DFRobotDFPlayerMini-1.0.1 library
 ************/
  #include "dev_sound_dfplayer.h"

  #ifdef ESP32PCKv2
    #include "HardwareSerial.h"
    extern HardwareSerial Serial2;
    #define mp3Serial Serial2
  #else
    // could be a SoftwareSerial too ...
    #define mp3Serial Serial5 
  #endif

  DFRobotDFPlayerMini myDFPlayer;
  bool mp3Ready = false;

  extern void host_outputString(char* str);
  extern void host_showBuffer();

  // BEWARE : no trailing '\n' added
  void _DBUG(const char* str) {
    host_outputString((char*)str);
    host_showBuffer();
    Serial.print(str);
  }  

  void setupSoundDFPlayer() {
    _DBUG("Setting up MP3 module\n");

    #ifdef ESP32PCKv2
      mp3Serial.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX, false);
    #else
      mp3Serial.begin(9600);
    #endif

    // needs up to 3secs
    if (!myDFPlayer.begin(mp3Serial)) {
      _DBUG("Unable to begin:\n1.Please recheck the connection!\n2.Please insert the SD card!\n");
      mp3Ready = false;
      delay(500);
      return;
    }
    _DBUG("DFPlayer Mini online.\n");
    delay(100);
    mp3Ready = true;
    
    myDFPlayer.volume(20);  //Set volume value. From 0 to 30

    //myDFPlayer.play(1);  //Play the first SD:/xx.mp3 then subdirs
    // // positionate only the current file ???
    // myDFPlayer.playMp3Folder(1); // plays SD:/MP3/0001.mp3 - @ least 1st song of /MP3/
    // delay(100);
    // myDFPlayer.play();
    //snd_playTrack(1);
  }

  // from 0 to 30
  void snd_setVolume(int vol) {
    if ( !mp3Ready ) { return; }
    myDFPlayer.volume(vol);
  }

  // from 1 to 2999
  void snd_playTrack(int trackNum) {
    if ( !mp3Ready ) { return; }
    // positionate only the current file ???
    myDFPlayer.playMp3Folder(trackNum); // plays SD:/MP3/0001.mp3 - @ least 1st song of /MP3/
    delay(100);
    myDFPlayer.play(trackNum); // ??????
  }

  void snd_next() {
    if ( !mp3Ready ) { return; }
    myDFPlayer.next();
  }

  bool paused = false;

  void snd_pause() {
    if ( !mp3Ready ) { return; }
    if ( !paused ) {
      myDFPlayer.pause();
      paused = true;
    } else {
      myDFPlayer.start();
      paused = false;
    }
  }

#endif
// not to include for ESP32 OLED