#ifndef ARDUINO_ARCH_ESP32


/************
 * DFPlayer mp3 module - Sound Layer for Xts-uBasic 
 * Xtase - fgalliat @ Nov. 2017
 * 
 * requires : DFRobotDFPlayerMini-1.0.1 library
 ************/

  #include "dev_sound_dfplayer.h"

  // could be a SoftwareSerial too ...
  #define mp3Serial Serial5 
  
  DFRobotDFPlayerMini myDFPlayer;
  bool mp3Ready = false;


  void setupSoundDFPlayer() {
    mp3Serial.begin(9600);

    // needs up to 3secs
    if (!myDFPlayer.begin(mp3Serial)) {
      host_outputString("Unable to begin:\n");
      host_outputString("1.Please recheck the connection!\n");
      host_outputString("2.Please insert the SD card!\n");
      host_showBuffer();
      //while(true);
      mp3Ready = false;
      delay(500);
      return;
    }
    host_outputString("DFPlayer Mini online.\n");
    host_showBuffer();
    delay(500);
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
    myDFPlayer.volume(vol);
  }

  // from 1 to 2999
  void snd_playTrack(int trackNum) {
    // positionate only the current file ???
    myDFPlayer.playMp3Folder(trackNum); // plays SD:/MP3/0001.mp3 - @ least 1st song of /MP3/
    delay(100);
    myDFPlayer.play(trackNum); // ??????
  }

  void snd_next() {
    myDFPlayer.next();
  }

  bool paused = false;

  void snd_pause() {
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