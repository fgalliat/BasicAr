/******************************
 * Xtase - fgalliat @ Oct2017
 * I/O device managment
 *****************************/

#ifndef __XTS_IO_H_
#define __XTS_IO_H_ 1

enum GFX_DEVICES {
    GFX_DEV_SERIAL = 0,
    GFX_DEV_LCD_MINI,
    GFX_DEV_VGA_SERIAL,
    GFX_DEV_RPID_SERIAL
};

enum OUTPUT_DEVICES {
    OUT_DEV_SERIAL = 0,
    OUT_DEV_LCD_MINI,
    OUT_DEV_VGA_SERIAL,
    OUT_DEV_RPID_SERIAL
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

// ~ 2KB of RAM
#define RPID_TEXT_WIDTH 80
#define RPID_TEXT_HEIGHT 24

#define SER_TEXT_WIDTH 80
#define SER_TEXT_HEIGHT 15


// ??????
extern int OUTPUT_DEVICE;
extern int INPUT_DEVICE;
extern int GFX_DEVICE;
extern void setScreenSize(int cols, int rows);

static int setConsoles(int outDev, int inDev, int gfxDev) {
    if ( outDev == -1 ) {
      if (OUTPUT_DEVICE != OUT_DEV_VGA_SERIAL) { 
        OUTPUT_DEVICE = OUT_DEV_VGA_SERIAL; 
        outDev = OUTPUT_DEVICE;
        setScreenSize(VGA_TEXT_WIDTH, VGA_TEXT_HEIGHT);
        GFX_DEVICE = GFX_DEV_LCD_MINI;  // TMP : DRAWxxx routines to port to vgat
        gfxDev = GFX_DEVICE;
      }
      else { 
        OUTPUT_DEVICE = OUT_DEV_LCD_MINI; 
        outDev = OUTPUT_DEVICE;
        setScreenSize(LCD_TEXT_WIDTH, LCD_TEXT_HEIGHT);
        GFX_DEVICE = GFX_DEV_LCD_MINI;
        gfxDev = GFX_DEVICE;
      }
    } else if ( outDev == OUT_DEV_SERIAL ) { // 0
      OUTPUT_DEVICE = OUT_DEV_SERIAL; 
      outDev = OUTPUT_DEVICE;
      setScreenSize(SER_TEXT_WIDTH, SER_TEXT_HEIGHT);
      GFX_DEVICE = GFX_DEV_LCD_MINI;  // TMP : DRAWxxx routines to port to vgat
      gfxDev = GFX_DEVICE;
    } else if ( outDev == OUT_DEV_LCD_MINI ) { // 1
      OUTPUT_DEVICE = OUT_DEV_LCD_MINI; 
      outDev = OUTPUT_DEVICE;
      setScreenSize(LCD_TEXT_WIDTH, LCD_TEXT_HEIGHT);
      GFX_DEVICE = GFX_DEV_LCD_MINI;  // TMP : DRAWxxx routines to port to serialt
      gfxDev = GFX_DEVICE;
    } else if ( outDev == OUT_DEV_VGA_SERIAL ) { // 2
      OUTPUT_DEVICE = OUT_DEV_VGA_SERIAL; 
      outDev = OUTPUT_DEVICE;
      setScreenSize(VGA_TEXT_WIDTH, VGA_TEXT_HEIGHT);
      GFX_DEVICE = GFX_DEV_LCD_MINI;  // TMP : DRAWxxx routines to port to serialt
      gfxDev = GFX_DEVICE;
    } else if ( outDev == OUT_DEV_RPID_SERIAL ) { // 3
      OUTPUT_DEVICE = OUT_DEV_RPID_SERIAL; 
      outDev = OUTPUT_DEVICE;
      setScreenSize(RPID_TEXT_WIDTH, RPID_TEXT_HEIGHT);
      GFX_DEVICE = GFX_DEV_LCD_MINI;  // just for this time....
      gfxDev = GFX_DEVICE;
    }
    return gfxDev;
  }



#endif
