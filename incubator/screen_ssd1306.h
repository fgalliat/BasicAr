#ifndef _screen_ssd1306_h_ 
#define _screen_ssd1306_h_ 1

  #include <SPI.h>
  #include <i2c_t3.h>
  #include <Adafruit_GFX.h>
  #include "screen_Adafruit_SSD1306.h"
  //default is 4 that is an I2C pin on t3.6
  #define OLED_RESET 6
  Adafruit_SSD1306 display(OLED_RESET);
  
  #if (SSD1306_LCDHEIGHT != 64)
   #error("Height incorrect, please fix Adafruit_SSD1306.h!");
  #endif

  // ======= Screen dirty/sync =========
  extern int irqCpt;

  bool screen_dirty = false;
  bool inSync = false;

  void screenSync() {
    if (inSync) { return; }
    inSync = true;
    display.display();
    irqCpt = 0;
    screen_dirty = false;
    inSync = false;
  }
  
  void _dirty() {
    //display.display();
    screen_dirty = true;

    // removed @13/04/2017 -> made crash "SHOW4.BAS" (cls in FOR loop...)
    // if ( irqCpt > 0 ) {
    //   screenSync();
    // }
  }
  
  void setup_lcd() {
   Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.setTextSize(1);
   
   // tmp try
   //display.setTextColor(WHITE);
   display.setTextColor(INVERSE);
   // tmp try

   // avoid firware LOGO display
   display.clearDisplay();
   display.display();
  }
  
  extern int ext_OUTPUT_LCD;
  extern void setRawOutputType(int out);

  void lcd_switchTo() {
    setRawOutputType(ext_OUTPUT_LCD);
  }

  int nbLine=0;
  void lcd_cls() {
    display.setCursor(0,0); 
    nbLine=0;
    display.clearDisplay(); 
    _dirty();
  }

  void lcd_locate(int x, int y) {
    display.setCursor(x*6,y*8); 
    nbLine=y;
  }


  char line[30];
  void lcd_print__ln(const char* str, bool ln, ...) {
    if (ln) {
      nbLine++;
      if ( nbLine >= 8 ) { lcd_cls(); }
    }
    
    for(int i=0; i < 30; i++) { line[i]=0; }

    int v = -1;
    va_list valist;
    /* initialize valist for num number of arguments */
    va_start(valist, ln);
    v = va_arg(valist, int);
    /* clean memory reserved for valist */
    va_end(valist);
  
    sprintf(line, str, v);

    if (ln) { display.println(line); }
    else { display.print(line); }

    // no more cf Timer1
  //display.display();
    //delay(1);
    _dirty();
  }

#define lcd_println(str, ...) lcd_print__ln(str, true, ##__VA_ARGS__)
#define lcd_printNOln(str, ...) lcd_print__ln(str, false, ##__VA_ARGS__)
  
  void lcd_print(char ch) {
    if ( ch == 13 ) { nbLine++; display.println(""); if ( nbLine >= 8 ) { lcd_cls(); } return; }
    display.print(ch);
    // no more cf Timer1
  //display.display();
    _dirty();
  }
  

  // ========= Drawing ==========
 void lcd_line(int x1, int y1, int x2, int y2, uint16_t color) {
   display.drawLine(x1,y1,x2,y2,color);
   // no more cf Timer1
  //display.display();
  _dirty();
 }

 void lcd_circle(int x, int y, int radius, uint16_t color) {
   display.drawCircle(x,y,radius,color);
   // no more cf Timer1
  //display.display();
   _dirty();
 }

 void lcd_pixel(int x, int y, uint16_t color) {
   display.drawPixel(x,y,color);
   // no more cf Timer1
  //display.display();
   _dirty();
 }

 // TODO : add more routines but @ this time : only X07 compatible

 // Bitmap routines => @ this time : not defined in screen.h
 #define BMP_BUFF_SIZE (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8)
 uint8_t bmp_buffer[BMP_BUFF_SIZE];

 #if ((defined STORAGE_SUPPORT) && (STORAGE_SUPPORT > 0))
 
 #define SD_METHOD_2 1


 // seems to not be a good idea to move out of the method ?!
 //File img;
 void drawBmpFromStorage(const char* bmpName) {
  // as file is always exactly 1KB long => not mandatory

  #ifdef SD_METHOD_2
    SdFile rdfile(bmpName, O_READ);
    // How to close file ??????

    if (!rdfile.isOpen()) {
      lcd_println((char*)"image not found !"); // keep that ?? 
      return;
    }

    // @ this time : fast enought
    // later => rdfile.readBytes( bmp_buffer, BMP_BUFF_SIZE );
    for(int i=0; i < BMP_BUFF_SIZE; i++) {
      bmp_buffer[i] = rdfile.read();
    }
 
    rdfile.close();

  #else
    SD_myFile = SD.open(bmpName, O_READ);
    // File SD_myFile = SD.open(bmpName);
    
    if ( !SD_myFile ) { 
      lcd_println((char*)"image not found !"); // keep that ?? 
      return; 
    }
    #ifdef USE_SDFAT_LIB
      SD_myFile.read( bmp_buffer, BMP_BUFF_SIZE );
    #else
      SD_myFile.seek(0);
      SD_myFile.readBytes( bmp_buffer, BMP_BUFF_SIZE );
    #endif
    SD_myFile.close();
  #endif

  display.drawBitmap(0, 0, bmp_buffer, 128, 64, 0x01);
  // no more cf Timer1
  //display.display();
  _dirty();
 }
#endif

#endif
