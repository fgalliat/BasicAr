#ifndef _pcspeaker_h_
#define _pcspeaker_h_ 1

/**************
 * PC Speaker emulation
 * Xtase - fgalliat @Nov 2017 
 *************/


//#include <SDL/SDL.h>
//#include <SDL/SDL_audio.h>
#include <cmath>
#include <queue>


// const int AMPLITUDE = 28000;
// const int FREQUENCY = 44100;
 
struct BeepObject {
    double freq;
    int samplesLeft;
};
 
#define Sint16 short

class Beeper {
    private:
        double v;
        std::queue<BeepObject> beeps;
    public:
        Beeper();
        ~Beeper();
        void generateSamples(Sint16 *stream, int length);
        void beep(double freq, int duration);
        void wait();
};
 
#endif