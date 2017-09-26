/***************************
* Architectures definitions
*
* Xtase - fgalliat @ Sept2017
***************************/
#ifndef __XTS_ARCH_H__
#define __XTS_ARCH_H__ 1

#if defined(__MK66FX1M0__)
// Teensy 3.6
  #define ARCH_TYPE "Teensy 3.6"
  #define BUT_TEENSY 1

  #define FS_SUPPORT 1
    #define USE_SDFAT_LIB 1

  // ==  GPIO ==
  #define BUZZER_PIN 2
  // finally : White / Blue / Green
  #define LED1_PIN 13
  #define LED2_PIN 39
  #define LED3_PIN 38

  #define BTN1_PIN 35
  #define BTN2_PIN 36
  #define BTN3_PIN 37

  #define MOTHERBOARD_PIN 25
  // ==  GPIO ==

  #define RAM_END (256*1024)-1
  #define EEPROM_END (4*1024)-1
  #define SPEED 180
#elif defined(__MK20DX256__)
// Teensy 3.2
  #define ARCH_TYPE "Teensy 3.2"
  #define BUT_TEENSY 1
  #define BUZZER_PIN 5
  #define LED1_PIN 13
  #define LED2_PIN LED1_PIN
  #define LED3_PIN LED1_PIN

  #define RAM_END (64*1024)-1
  #define EEPROM_END (2*1024)-1
  #define SPEED 72
#else
  #define ARCH_TYPE "Unknown platform"
  #define BUZZER_PIN -1
  #define LED1_PIN -1
  #define LED2_PIN LED1_PIN
  #define LED3_PIN LED1_PIN

  #define RAM_END (4*1024)-1
  #define EEPROM_END (1*1024)-1
  #define SPEED 16
#endif





// #ifdef FS_SUPPORT
//   #ifdef USE_SDFAT_LIB
//     // for teensy 3.6 as example...
//     // include SdFat library
//     // 
//     // Test of SdFat-beta @ 03/03/2017
//     // from : https://github.com/greiman/SdFat-beta
//     // copied SdFat-beta-master.zip\SdFat-beta-master\SdFat into C:\Program Files (x86)\Arduino\libraries\SdFat

//     #include "SdFat.h"
//     static SdFatSdio SD;
//     static char SDentryName[13];
//   #else 
//     // include the SD library: 
//     #include <SD.h>
//   #endif

//   #endif // FS_SUPPORT


  #endif // include