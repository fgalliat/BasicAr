/******************************
 * Xtase - fgalliat @ Oct2017
 * I/O device managment
 *****************************/

#ifndef __XTS_IO_H_
#define __XTS_IO_H_ 1

enum GFX_DEVICES {
    GFX_DEV_SERIAL = 0,
    GFX_DEV_LCD_MINI,
    GFX_DEV_VGA_SERIAL
};

enum OUTPUT_DEVICES {
    OUT_DEV_SERIAL = 0,
    OUT_DEV_LCD_MINI,
    OUT_DEV_VGA_SERIAL
};

enum INPUT_DEVICES {
    IN_DEV_SERIAL = 0,
    IN_DEV_USB_KBD
};

#define LCD_GFX_WIDTH 128
#define LCD_GFX_HEIGHT 64

#define LCD_TEXT_WIDTH 21
#define LCD_TEXT_HEIGHT 8

// ~ 2KB of RAM
#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 24

#define SER_TEXT_WIDTH 80
#define SER_TEXT_HEIGHT 15

#endif