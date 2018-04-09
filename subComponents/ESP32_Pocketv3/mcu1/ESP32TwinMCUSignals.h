#ifndef __ESP32T_SIG_H__
#define __ESP32T_SIG_H__ 1
/*
* Twin ESP32 Bridge SIGNALS header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Apr. 2018
*/

  #define SIG_MCU_RESET       0x01
  #define SIG_MCU_MASTER_SYNC 0x02 // Query
  #define SIG_MCU_SLAVE_SYNC  0x03 // Answer

  #define SIG_MP3_PLAY  0x11
  #define SIG_MP3_PAUSE 0x12
  #define SIG_MP3_NEXT  0x13
  #define SIG_MP3_PREV  0x14
  #define SIG_MP3_VOL   0x15

  #define SIG_SCR_MODE          0x21
  #define SIG_SCR_CLEAR         0x22
  #define SIG_SCR_CURSOR        0x22 // TTY cursor
  #define SIG_SCR_COLOR         0x23 // Text + Shapes ?
  #define SIG_SCR_BLITT         0x24
  
  #define SIG_SCR_PRINT_CH      0x31
  #define SIG_SCR_PRINT_STR     0x32
  #define SIG_SCR_PRINT_INT     0x33
  #define SIG_SCR_PRINT_NUM     0x34

  #define SIG_SCR_DRAW_PIX      0x41
  #define SIG_SCR_DRAW_LINE     0x42
  #define SIG_SCR_DRAW_RECT     0x43
  #define SIG_SCR_DRAW_CIRCLE   0x44
  #define SIG_SCR_DRAW_TRIANGLE 0x45

  #define SIG_LAST SIG_SCR_DRAW_TRIANGLE

  // bytes to read per cmd
  // -2 : nothing to read ...
  // -1 read until '\0'
  const static uint8_t __bridge_params[] = {
   -2, // 0x00
    0, // 0x01
    0, // 0x02
    0, // 0x03
   -2, // 0x04
   -2, // 0x05
   -2, // 0x06
   -2, // 0x07
   -2, // 0x08
   -2, // 0x09
   -2, // 0x0a
   -2, // 0x0b
   -2, // 0x0c
   -2, // 0x0d
   -2, // 0x0e
   -2, // 0x0f
   -2, // 0x10
    1, // 0x11
    0, // 0x12
    0, // 0x13
    0, // 0x14
    1, // 0x15
   -2, // 0x16
   -2, // 0x17
   -2, // 0x18
   -2, // 0x19
   -2, // 0x1a
   -2, // 0x1b
   -2, // 0x1c
   -2, // 0x1d
   -2, // 0x1e
   -2, // 0x1f
   -2, // 0x20
    1, // 0x21
    0, // 0x22
    2, // 0x23
    2, // 0x24
    1, // 0x25
   -2, // 0x26
   -2, // 0x27
   -2, // 0x28
   -2, // 0x29
   -2, // 0x2a
   -2, // 0x2b
   -2, // 0x2c
   -2, // 0x2d
   -2, // 0x2e
   -2, // 0x2f
   -2, // 0x30
    1, // 0x31
   -1, // 0x32
    4, // 0x33
    4, // 0x34
   -2, // 0x35
   -2, // 0x36
   -2, // 0x37
   -2, // 0x38
   -2, // 0x39
   -2, // 0x3a
   -2, // 0x3b
   -2, // 0x3c
   -2, // 0x3d
   -2, // 0x3e
   -2, // 0x3f
   -2, // 0x40
    6, // 0x41
   10, // 0x42
   11, // 0x43
    9, // 0x44
   -2, // 0x45 -- TODO
  };



#endif