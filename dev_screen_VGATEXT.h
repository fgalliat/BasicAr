#ifndef screen_VGATEXT_h
#define screen_VGATEXT_h 1

/**
  HobbyTronics : Serial VGA Monitor Driver Board v1.1
  Control routines

  @Xtase - fgalliat Jan2017
*/


void vgat_reboot(bool wait);
void vgat_cls();

void setup_vgat(bool wait=false);

void vgat_reboot(bool wait);

// 1 to 9
void vgat_focuswin(int winNum);

void vgat_clearwin();

void vgat_cls();

void vgat_locate(int left, int top);

// winNum 1 to 9
void vgat_createwin(int winNum, int left, int top, int width, int height, bool border, const char* title);


void vgat_print(char* str, int v);

void vgat_print(char* str);


void vgat_print(const char* str, int v);

void vgat_print(const char* str);


void vgat_printCh(char ch);

/** may not use it to send a command */
void vgat_print(char ch, bool autoReplaceLF);

// =============================================

void vgat_drawPixel(char px, int left, int top);

void vgat_Vline(int x1, int y1, int y2);

void vgat_Hline(int x1, int y1, int x2);


void vgat_line(int x1, int y1, int x2, int y2);

// =============================================

void vgat_startScreen();


extern void setRawOutputType(int out); 

void vgat_switchTo();

#endif 