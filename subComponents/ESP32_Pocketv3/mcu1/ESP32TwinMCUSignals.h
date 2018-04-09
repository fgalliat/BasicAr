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

  // bytes to read per cmd
  const static int __bridge_params[] = {
    -1,
    0x00, 0x00, 0x00,  // RESET
    // --------------- PLAY, pause, next...
    1, 0x00, 0x00, 0x00, 1,
    // --------------- MODE, ....
    1, 0x00, 2, 2, 1,
    // --------------- ? CH, ....
    1, -1, 4, 4,
    // --------------- DRAW_PIX, ....
    6, 10, 11, 9, -1

  };



#endif