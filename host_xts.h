/*****************
 * Xtase - fgalliat @ Sept 2017 
 **********/

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

// ============================================

void lsStorage();

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

void DBUG(int v);
void DBUG(float v);
void DBUG(char* v);
void DBUG(const char* v);
void DBUG(const char* v, int v2);

void DBUG_NOLN(char* v);
void DBUG_NOLN(const char* v);
void DBUG_NOLN(const char* v, int v2);
void DBUG_NOLN(int v);
