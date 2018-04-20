/***************************
* Host.h
*
* Xtase - fgalliat @ Sept2017
* Original Code by Robin Edwards @2014
***************************/

#include <stdint.h>

enum BLITT_MODE_t {
  BLITT_LOCKED = 0, // lock gfx blitt
  BLITT_DIRECT,     // trigger a blitt request
  BLITT_AUTO        // default mode
};
extern int BLITT_MODE;
bool isGfxAutoBlitt();

#include "host_xts.h"

void host_init(int buzzerPin);
void host_sleep(long ms);
void host_digitalWrite(int pin,int state);
int host_digitalRead(int pin);
int host_analogRead(int pin);
void host_pinMode(int pin, int mode);
void host_click();
void host_startupTone();
void host_cls();
void host_showBuffer();
void host_moveCursor(int x, int y);

void host_outputString(char *str);
void host_outputString(const char *str);
void host_outputProgMemString(const char *str);

void host_outputChar(char c);
void host_outputFloat(float f);
char *host_floatToStr(float f, char *buf);
int host_outputInt(long val);
void host_newLine();
char *host_readLine();
char host_getKey();
bool host_ESCPressed();
void host_outputFreeMem(unsigned int val);
void host_saveProgram(bool autoexec);
void host_loadProgram();
 