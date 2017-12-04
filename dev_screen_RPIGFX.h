#ifndef screen_RPIGFX_h
#define screen_RPIGFX_h 1

/**
  RPI0W : Serial HDMI Controller
  Control routines

  @Xtase - fgalliat Nov2017
*/

/* request to really halt RPI Unit */
void rpid_haltGPU(bool wait=true);

void rpid_reboot(bool wait);
void rpid_cls();

void setup_rpid(bool wait=false);

void rpid_reboot(bool wait);

// 1 to 9
void rpid_focuswin(int winNum);

void rpid_clearwin();

void rpid_cls();

void rpid_locate(int left, int top);

// winNum 1 to 9
void rpid_createwin(int winNum, int left, int top, int width, int height, bool border, const char* title);


void rpid_print(char* str, int v);

void rpid_print(char* str);


void rpid_print(const char* str, int v);

void rpid_print(const char* str);


void rpid_printCh(char ch);

/** may not use it to send a command */
void rpid_print(char ch, bool autoReplaceLF);

// =============================================



void rpid_Vline(int x1, int y1, int y2);
void rpid_Hline(int x1, int y1, int x2);

void rpid_drawPixel(char px, int left, int top);
void rpid_line(int x1, int y1, int x2, int y2);

// ================

void rpid_gfx_circle(int x1, int y1, int radius, int color);
void rpid_gfx_drawPixel(int left, int top, int color);
void rpid_gfx_line(int x1, int y1, int x2, int y2, int color);


// =============================================

void rpid_startScreen();


//extern void setRawOutputType(int out); 
void rpid_switchTo();

#endif 