/******************************
 * Xtase - fgalliat @ Oct2017
 * I/O device managment
 *****************************/

#ifndef __XTS_IO_H_
#define __XTS_IO_H_ 1

enum OUTPUT_DEVICES {
    OUT_DEV_SERIAL = 0,
    OUT_DEV_LCD_MINI,
    OUT_DEV_VGA_SERIAL
};

enum INPUT_DEVICES {
    IN_DEV_SERIAL = 0,
    IN_DEV_USB_KBD
};

// to move away ?
#define LCD_TEXT_WIDTH 21
#define LCD_TEXT_HEIGHT 8

// ~ 2KB of RAM
#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 24

#define SER_TEXT_WIDTH 80
#define SER_TEXT_HEIGHT 15

#endif