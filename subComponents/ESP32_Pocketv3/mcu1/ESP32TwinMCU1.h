#ifndef __ESP32T1_MCU_H__
#define __ESP32T1_MCU_H__ 1
/*
* Twin ESP32 #1 MCU header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Apr. 2018
*/

  void GenericMCU::reset() { 
    println("Reset...");
    // do what needed ...
  }

  void GenericMCU::init() { 
    Serial.begin(115200);

    println("init...");
    // do what needed ...
  }

  GenericMCU_GPIO*         GenericMCU::getGPIO()         { return NULL; }
  GenericMCU_BUZZER*       GenericMCU::getBUZZER()       { return NULL; }
  GenericMCU_SCREEN*       GenericMCU::getScreen()       { return NULL; }
  GenericMCU_FS*           GenericMCU::getFS()           { return NULL; }
  GenericMCU_MUSIC_PLAYER* GenericMCU::getMusicPlayer()  { return NULL; }

  // ============================================================================

  //void GenericMCU_SCREEN::println(char* str) { GenericMCU_SCREEN::print(str); GenericMCU_SCREEN::print('\n'); }
  void GenericMCU_SCREEN::print(char* str) { Serial.print(str); }
  void GenericMCU_SCREEN::print(char ch) { Serial.print(ch); }

#endif