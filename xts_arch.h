/***************************
* Architectures definitions
*
* Xtase - fgalliat @ Sept2017
***************************/
#ifndef __XTS_ARCH_H__
#define __XTS_ARCH_H__ 1

#if defined(ARDUINO_ARCH_ESP32)
 // specifically for NON-BUILTIN-OLED module
 #define ESP32PCKv2 1
 #warning "You choosed ESP32 Xts-uBasic Pocket Version 2 !"
#endif




#if defined(__MK66FX1M0__)
// Teensy 3.6
  #define ARCH_TYPE "Teensy 3.6"
  #define BUT_TEENSY 1
  #define XTSUBASIC_LAYOUT 1

  // Xts-uBasic Board Hardware Layout
  #ifdef XTSUBASIC_LAYOUT
    #define BUILTIN_LCD 1
    #define BOARD_VGA 1

    #define BUILTIN_KBD 1
    #define KEYB_TYPE_USB_HOBYTRONIC_V1 1

    #define BOARD_SND 1

    #define BOARD_RPID 1
  #endif

  #define FS_SUPPORT 1
    #define USE_SDFAT_LIB 1

    // 5KB buffer
    #define AUDIO_BUFF_SIZE (5*1024) 
    // lcd(128*64) / 8(1bpp)
    #define PICTURE_BUFF_SIZE (1*1024)

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
#elif defined(COMPUTER)
  // PC or anything else
  #define ARCH_TYPE "Generic Computer"
  #define BUT_TEENSY 1

  #define FS_SUPPORT 1
    #define USE_FS_LEGACY 1

  #define BOARD_VGA 1
    #define BOARD_VGA_FAKE_SERIAL 1

  #define BOARD_RPID 1

  #define BUILTIN_LCD 1

  // #define BUILTIN_KBD 1
  // #define KEYB_TYPE_USB_HOBYTRONIC_V1 1

  #define BUZZER_PIN 5
  
  #define LED1_PIN 13
  #define LED2_PIN LED1_PIN
  #define LED3_PIN LED1_PIN

  #define BTN1_PIN 35
  #define BTN2_PIN 36
  #define BTN3_PIN 37

  // 5KB buffer
  #define AUDIO_BUFF_SIZE (5*1024) 
  // lcd(128*64) / 8(1bpp)
  #define PICTURE_BUFF_SIZE (1*1024)

  #define RAM_END (256*1024)-1
  #define EEPROM_END (2*1024)-1
  #define SPEED 1000
#elif defined(ARDUINO_ARCH_ESP32)
  // ESP32 Boards
  #define BUT_TEENSY 1
  #define BUT_ESP32 1

  #ifdef ESP32PCKv2
    // DOIT ESP32 DEVKIT V1 board
    #define ARCH_TYPE "ESP32-DevKit Board"
    #include "ESP32_Pocketv2.h"
    
      /// BE SURE TO USE SAME GND side as 3.3 is !!!!!!
      // #define UART2_NUM 2
      #define UART2_RX 16
      // #define UART2_TX 17
      #define UART2_NUM 1
      #define UART2_TX 4 // BEWARE : used as SCREEN RST pin

    #define BUILTIN_LCD 1

    // MP3 DFPlayer module
    #define BOARD_SND 1
  #else
    // ESP32 + Oled Board
    // from mackerhawk as example
    #define ARCH_TYPE "ESP32-Oled Board"
    #include "ESP32OLED.h"
    #define BUILTIN_LCD 1
  #endif

  #define BUZZER_PIN -1
  
  #define LED1_PIN LED1
  #define LED2_PIN LED1_PIN
  #define LED3_PIN LED1_PIN

  #define BTN1_PIN BTN1
  #define BTN2_PIN BTN2
  #define BTN3_PIN -1

  #define FS_SUPPORT 1 // TODO remove comment
  #define ESP32_FS 1

  // 5KB buffer
  #define AUDIO_BUFF_SIZE (5*1024) 
  // lcd(128*64) / 8(1bpp)
  #define PICTURE_BUFF_SIZE (1*1024)

  #define RAM_END (288*1024)-1
  #define EEPROM_END (4*1024)-1
  #define SPEED 240
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


  #endif // include