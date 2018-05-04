/*****************
 * Xtase - fgalliat @ Sept 2017 
 **********/

// _______________________________
//
// GFX blitt Ops

// enum BLITT_MODE_t {
//   BLITT_LOCKED = 0, // lock gfx blitt
//   BLITT_DIRECT,     // trigger a blitt request
//   BLITT_AUTO        // default mode
// };

//static int BLITT_MODE = BLITT_AUTO;
//static bool isGfxAutoBlitt() { return BLITT_MODE == BLITT_AUTO; }
// _______________________________

void setupHardware();

void led(int ledID, bool state);
void led1(bool state);
void led2(bool state);
void led3(bool state);
void activityLed(bool state);

bool btn1();
bool btn2();
bool btn3();
bool anyBtn();


#define AUDIO_FORMAT_T5K 0
#define AUDIO_FORMAT_T53 1

void playNote(int note, int duration);

// PLAY "abc"
void playTuneString(char* strTune);

// PLAYT5K "monkey.t5k"
//void playTuneFromStorage(const char* tuneName, int format, bool btnStop);
void playTuneFromStorage(char* tuneName, int format, bool btnStop);

// ============================================

bool drawBPPfile(char* filename);
bool drawPCTfile(char* filename, int x, int y);
bool drawSPRITEfile(char* filename, int x, int y, int w, int h, int sx, int sy);

void drawLine(int x1, int y1, int x2, int y2);
void drawCircle(int x1, int y1, int radius);
void drawPixel(int x1, int y1, int color); // 0: black else :white
void draw_blitt();
void drawRect(int x, int y, int w, int h, int color, int mode);

// ============================================

void lsStorage(char* filter, bool sendToArray);

void MCU_halt();
void MCU_reset();
int MCU_freeRam();

void _noInterrupts();
void _interrupts();


// TODO : impl. it =================
 void lcd_println(char* text);
 bool checkbreak();
 
// =================================

void xts_loadTestProgram();

void loadAsciiBas(char* filename);
void saveAsciiBas(char* filename);
void llistAsciiBas(char* filename);
void deleteBasFile(char* filename);

// =================================

#ifndef DBUG_NOLN
    void DBUG(int v);
    void DBUG(float v);
    void DBUG(char* v);
    void DBUG(const char* v);
    void DBUG(const char* v, int v2);

    void DBUG_NOLN(char* v);
    void DBUG_NOLN(const char* v);
    void DBUG_NOLN(const char* v, int v2);
    void DBUG_NOLN(int v);
#endif

void host_system_menu();