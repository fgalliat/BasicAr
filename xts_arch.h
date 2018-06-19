/***************************
* Architectures definitions
*
*
* NO MORE USED FOR NOW !!!!!!
*
* Xtase - fgalliat @ Sept2017
***************************/
#ifndef __XTS_ARCH_H__
#define __XTS_ARCH_H__ 1


#define PICTURE_BUFF_SIZE (1024)
#define AUDIO_BUFF_SIZE (5*1024)

#if defined(COMPUTER)
  // cf memcpy
  #include <cstring>

  #include <cstdlib>
  #include <stdio.h>
  #include <unistd.h>

  // cf uintX_t
  #include <cstdint>
  #include <sys/types.h>

  #include <iostream>
  #include <fstream>
  #include <dirent.h>
  #include <ncurses.h>
  #include <sys/ioctl.h>
  #include <fcntl.h>
  #include <linux/kd.h>
  #include <math.h>
  #include <SDL2/SDL.h>

  #include "computerArduino.h"
  #include "computerSerial.h"

  #ifdef ANOTHER_CPP
    extern _Serial Serial;
  #elif defined(MAIN_INO_FILE)
    _Serial Serial = _Serial();
  #endif

  #include "computerScreen.h"

#endif

#include "GenericMCU.h"

#if defined(COMPUTER)
    // Generic computer C++/SDL/CURSES

    #define BUILTIN_LCD 1
    #define ARCH_TYPE "Xts_Computer V3" 

    #ifdef MAIN_INO_FILE
        Adafruit_SSD1306 display(-1);

        #include "computer.h"

        GenericMCU mcu;
    #else
      #define TEXT_OVERWRITE 0x00 // w/ BG
      #define TEXT_INCRUST   0x01 // w/o BG

      #define BTN_1 1
      #define BTN_2 2
      #define BTN_3 3

      #ifdef ANOTHER_CPP
        extern _Serial Serial;
        extern Adafruit_SSD1306 display;
        
        extern GenericMCU mcu;
      #endif
    #endif


#elif defined(ARDUINO_ARCH_ESP32)
  #define BUT_ESP32 1

  #define ESP32PCKv3 1

  #ifdef ESP32PCKv3
    // else is slave
    #define MCU_MASTER 1

    #define BUILTIN_LCD 1
    #define ARCH_TYPE "Xts_Pocket V3" 

    #define ESP32_WIFI_SUPPORT 1

    #define SCREEN_BRIDGED 1

    // #define ESP32_I_TFT_eSPI 1
    // // need to be defined in <$lib$>/User_Setup_Select.h
    // #define ESP32_I_USE_ILI9341 1

    #ifdef MAIN_INO_FILE
        #include "ESP32TwinMCU1.h"

        GenericMCU mcu;

  static void IRAM_ATTR _doISR() {
      mcu.doISR();
  }

        #ifdef ESP32_WIFI_SUPPORT
            extern void host_outputString(char* str);
            extern int host_outputInt(long v);
            #define DBUG(a) { Serial.print(a); host_outputString(a); }
            #define DBUGi(a) { Serial.print(a); host_outputInt(a); }
            #include "Esp32WifiServer.h"
            // TODO : use a Generic class Name
            Esp32WifiServer telnet;
            #undef DBUG
            #undef DBUGi
        #endif

    #else
      // BEWARE w/ THAT !!!!
      #define BTN_1    7
      #define BTN_2    6
      #define BTN_3    5

  #define TEXT_OVERWRITE 0x00 // w/ BG
  #define TEXT_INCRUST   0x01 // w/o BG

      #ifdef ANOTHER_CPP
        extern GenericMCU mcu;

        // TODO : better
        #ifdef ESP32_WIFI_SUPPORT
            extern void host_outputString(char* str);
            extern int host_outputInt(long v);
            #define DBUG(a) { Serial.print(a); host_outputString(a); }
            #define DBUGi(a) { Serial.print(a); host_outputInt(a); }
            #include "Esp32WifiServer.h"
            // TODO : use a Generic class Name
            extern Esp32WifiServer telnet;
            // #undef DBUG
            #undef DBUGi
        #endif

      #endif

    #endif

  #endif

#endif





// #if defined(ARDUINO_ARCH_ESP32)
//  // specifically for NON-BUILTIN-OLED module
//  #define ESP32PCKv2 1
//  #warning "You choosed ESP32 Xts-uBasic Pocket Version 2 !"
// #endif




// #if defined(__MK66FX1M0__)
// // Teensy 3.6
//   #define ARCH_TYPE "Teensy 3.6"
//   #define BUT_TEENSY 1
//   #define XTSUBASIC_LAYOUT 1

//   // Xts-uBasic Board Hardware Layout
//   #ifdef XTSUBASIC_LAYOUT
//     #define BUILTIN_LCD 1
//     #define BOARD_VGA 1

//     #define BUILTIN_KBD 1
//     #define KEYB_TYPE_USB_HOBYTRONIC_V1 1

//     #define BOARD_SND 1

//     #define BOARD_RPID 1
//   #endif

//   #define FS_SUPPORT 1
//     #define USE_SDFAT_LIB 1

//     // 5KB buffer
//     #define AUDIO_BUFF_SIZE (5*1024) 
//     // lcd(128*64) / 8(1bpp)
//     #define PICTURE_BUFF_SIZE (1*1024)

//   // ==  GPIO ==
//   #define BUZZER_PIN 2
//   // finally : White / Blue / Green
//   #define LED1_PIN 13
//   #define LED2_PIN 39
//   #define LED3_PIN 38

//   #define BTN1_PIN 35
//   #define BTN2_PIN 36
//   #define BTN3_PIN 37

//   #define MOTHERBOARD_PIN 25
//   // ==  GPIO ==

//   #define RAM_END (256*1024)-1
//   #define EEPROM_END (4*1024)-1
//   #define SPEED 180
// #elif defined(__MK20DX256__)
// // Teensy 3.2
//   #define ARCH_TYPE "Teensy 3.2"
//   #define BUT_TEENSY 1

//   #define BUZZER_PIN 5
  
//   #define LED1_PIN 13
//   #define LED2_PIN LED1_PIN
//   #define LED3_PIN LED1_PIN

//   #define RAM_END (64*1024)-1
//   #define EEPROM_END (2*1024)-1
//   #define SPEED 72
// #elif defined(COMPUTER)
//   // PC or anything else
//   #define ARCH_TYPE "Generic Computer"
//   #define BUT_TEENSY 1

//   #define FS_SUPPORT 1
//     #define USE_FS_LEGACY 1

//   #define BOARD_VGA 1
//     #define BOARD_VGA_FAKE_SERIAL 1

//   #define BOARD_RPID 1

//   #define BUILTIN_LCD 1

//   // #define BUILTIN_KBD 1
//   // #define KEYB_TYPE_USB_HOBYTRONIC_V1 1

//   #define BUZZER_PIN 5
  
//   #define LED1_PIN 13
//   #define LED2_PIN LED1_PIN
//   #define LED3_PIN LED1_PIN

//   #define BTN1_PIN 35
//   #define BTN2_PIN 36
//   #define BTN3_PIN 37

//   // 5KB buffer
//   #define AUDIO_BUFF_SIZE (5*1024) 
//   // lcd(128*64) / 8(1bpp)
//   #define PICTURE_BUFF_SIZE (1*1024)

//   #define RAM_END (256*1024)-1
//   #define EEPROM_END (2*1024)-1
//   #define SPEED 1000
// #elif defined(ARDUINO_ARCH_ESP32)
//   // ESP32 Boards
//   #define BUT_TEENSY 1
//   #define BUT_ESP32 1

//   #ifdef ESP32PCKv2
//     // DOIT ESP32 DEVKIT V1 board
//     #define ARCH_TYPE "ESP32-DevKit Board"

// //#define LCD_LOCKED 1



//     #define COLOR_64K    1
//     #define COLOR_PICTURE_BUFF_SIZE (((160*128)*2)+7)

//       /// BE SURE TO USE SAME GND side as 3.3 is !!!!!!
//       // #define UART2_NUM 2
//       #define UART2_RX 16
//       // #define UART2_TX 17
//       #define UART2_NUM 1
//       #define UART2_TX 4 // BEWARE : used as SCREEN RST pin

//     #define BUILTIN_LCD 1

//     // MP3 DFPlayer module
//     #define BOARD_SND 1

//     // Wifi support for Esp32
//     #define ESP32_WIFI_SUPPORT 1

//     #include "ESP32_Pocketv2.h"

//   #else
//     // ESP32 + Oled Board
//     // from mackerhawk as example
//     #define ARCH_TYPE "ESP32-Oled Board"
//     #include "ESP32OLED.h"
//     #define BUILTIN_LCD 1
//   #endif

//   #define BUZZER_PIN -1
  
//   #define LED1_PIN LED1
//   #define LED2_PIN LED1_PIN
//   #define LED3_PIN LED1_PIN

//   #define BTN1_PIN BTN1
//   #define BTN2_PIN BTN2
//   #define BTN3_PIN BTN3

//   #define FS_SUPPORT 1 // TODO remove comment
//   #define ESP32_FS 1

//   // 5KB buffer
//   #define AUDIO_BUFF_SIZE (5*1024) 
//   // lcd(128*64) / 8(1bpp)
//   #define PICTURE_BUFF_SIZE (1*1024)

//   #define RAM_END (288*1024)-1
//   #define EEPROM_END (4*1024)-1
//   #define SPEED 240
// #else
//   #define ARCH_TYPE "Unknown platform"
  
//   #define BUZZER_PIN -1
  
//   #define LED1_PIN -1
//   #define LED2_PIN LED1_PIN
//   #define LED3_PIN LED1_PIN

//   #define RAM_END (4*1024)-1
//   #define EEPROM_END (1*1024)-1
//   #define SPEED 16
// #endif


  #endif // include