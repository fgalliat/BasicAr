/*****************
 * Xtase - fgalliat @ Sept 2017 
 **********/

void setupHardware();

void led(int ledID, bool state);
void led1(bool state);
void led2(bool state);
void led3(bool state);
void activityLed(bool state);

#define AUDIO_FORMAT_T5K 0
#define AUDIO_FORMAT_T53 1

void playNote(int note, int duration);

// PLAY "abc"
void playTuneString(char* strTune);

// PLAYT5K "monkey.t5k"
//void playTuneFromStorage(const char* tuneName, int format, bool btnStop);
void playTuneFromStorage(char* tuneName, int format, bool btnStop);

// ============================================

void lsStorage();

// TODO : impl. it =================
 void lcd_println(char* text);
 bool checkbreak();
 bool anyBtn();
// =================================


