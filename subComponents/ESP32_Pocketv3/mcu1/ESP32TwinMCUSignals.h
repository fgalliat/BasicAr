#ifndef __ESP32T_SIG_H__
#define __ESP32T_SIG_H__ 1
/*
* Twin ESP32 Bridge SIGNALS header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Apr. 2018
*/

  #define TEXT_OVERWRITE 0x00 // w/ BG
  #define TEXT_INCRUST   0x01 // w/o BG
  

  #define SIG_MCU_RESET       0x01
  #define SIG_MCU_MASTER_SYNC 0x02 // Query
  #define SIG_MCU_SLAVE_SYNC  0x03 // Answer
  #define SIG_MCU_UPLOAD_BDG  0x04 // Upload via Bridge
  #define SIG_MCU_DIR2        0x05 // DIR onMCU#2
  #define SIG_MCU_UPLOAD_SER  0x06 // Upload via Main Serial line

  #define SIG_MP3_PLAY  0x11
  #define SIG_MP3_PAUSE 0x12
  #define SIG_MP3_NEXT  0x13
  #define SIG_MP3_PREV  0x14
  #define SIG_MP3_VOL   0x15
  #define SIG_MP3_STOP  0x16

  #define SIG_SCR_MODE          0x21
  #define SIG_SCR_CLEAR         0x22
  #define SIG_SCR_CURSOR        0x23 // TTY cursor
  #define SIG_SCR_COLOR         0x24 // Text + Shapes ?
  #define SIG_SCR_BLITT         0x25
  #define SIG_SCR_TXTMODE       0x26
  
  #define SIG_SCR_PRINT_CH      0x31
  #define SIG_SCR_PRINT_STR     0x32
  #define SIG_SCR_PRINT_INT     0x33
  #define SIG_SCR_PRINT_NUM     0x34

  #define SIG_SCR_DRAW_PIX      0x41
  #define SIG_SCR_DRAW_LINE     0x42
  #define SIG_SCR_DRAW_RECT     0x43
  #define SIG_SCR_DRAW_CIRCLE   0x44
  #define SIG_SCR_DRAW_TRIANGLE 0x45
  #define SIG_SCR_DRAW_BPP      0x46
  #define SIG_SCR_DRAW_PCT      0x47
  #define SIG_SCR_DRAW_PCT_SPRITE 0x48

  #define SIG_LAST SIG_SCR_DRAW_PCT_SPRITE

  // bytes to read per cmd
  // -2 : nothing to read ...
  // -1 read until '\0'
  const static uint8_t __bridge_params[] = {
   -2, // 0x00
    0, // 0x01 reset
    0, // 0x02 master sync
    0, // 0x03 slave sync
    0, // 0x04 upload via bridge
    0, // 0x05 dir mcu#2
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
    1, // 0x11 play
    0, // 0x12 pause
    0, // 0x13 prev
    0, // 0x14 next
    1, // 0x15 vol
    0, // 0x16 stop
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
    1, // 0x21 mode
    0, // 0x22 clear
    2, // 0x23 cursor
    2, // 0x24 color
    1, // 0x25 blitt
    5, // 0x26 text mode
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
    1, // 0x31 ch
   -1, // 0x32 str
    4, // 0x33 int
    4, // 0x34 num
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
    6, // 0x41 pix
   10, // 0x42 line
   11, // 0x43 rect
    9, // 0x44 circle
   -2, // 0x45 triangle
   -1, // 0x46 bpp file
   -1, // 0x47 pct file
   -1, // 0x48 pct sprite file
  };



#endif