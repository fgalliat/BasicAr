/**************
 * Computer - Arduino equiv
 * Xtase - fgalliat @Nov 2017 
 *************/

// else Arduino IDE will try to compile it !!!!
#ifdef COMPUTER

 #include <cstdlib>
 #include <cstring>
 #include <stdio.h>
 #include <unistd.h>
 #include <iostream>

 #include <ncurses.h>
 #include <sys/ioctl.h>
 #include <fcntl.h>
 #include <linux/kd.h>

 #include <SDL2/SDL.h>
 #include "dev_sound_pcspeaker.h"

 #include "computer.h"

  //SDL_Window *window;

  // MUST NOT be a static variable in an include !!!!
  Beeper buzzer;
  #define Beep(freq,dur) buzzer.beep(freq,dur)


//   class _Serial {
//       private:
//         // N.B. : unsigned long  is very important
//         unsigned long lastTime = 0;
//         int lastKC = -1;
//       public:
//           void begin(int speed) {
//              WINDOW *w = initscr();
//              cbreak();
//              noecho();
//              nodelay(w, TRUE); // prevent getch() from blocking !!!!!!
//           }
          
//           int available() { 
//             lastKC = getch(); // ncurses version
//             return lastKC > -1 ? 1 : 0;
//           }
  
//           int read() { 
//             int ret = lastKC;
//             lastKC = -1;
//             return ret;
//           }
  
//           void _printAt(int x, int y, char* str) {
//             mvprintw(y, x, str);
//           }

//           // TODO : use curses
//           void print(int v) { printf("%d", v); }
//           void println(int v) { printf("%d\n", v); }
          
//           void print(const char* v) { 
//               int l = strlen(v);
//               char* vv = (char*)malloc(l+1);
//               for ( int i=0; i < l; i++ ) {
//                   if ( vv[i] == '\r' ) { vv[i] = '\n'; }
//                   else vv[i] = v[i];
//               }
//               vv[l] = 0x00;
//               printf("%s", vv); 
//               free(vv);        
//               refresh();      
//           }

//           void println(const char* v) { 
//               int l = strlen(v);
//               char* vv = (char*)malloc(l+1);
//               for ( int i=0; i < l; i++ ) {
//                   if ( vv[i] == '\r' ) { vv[i] = 0x00; }
//                   else vv[i] = v[i];
//               }
//               vv[l] = 0x00;
//               printf("%s\n", vv); 
//               free(vv);
//               refresh();
//           }

//           void flush() {}
//   };



  void setupComputer() {
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    //buzzer = new Beeper();

    window = SDL_CreateWindow("SSD1306", 0, 0, 128, 64, SDL_WINDOW_SHOWN);
    if (window == NULL){
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
  }


  void closeComputer() {
    clrtoeol();
	refresh();
	endwin();

    SDL_DestroyWindow(window);
    SDL_Quit();
  }


  void interrupts() {
      printf("TO IMPL. interrupts()\n");
  }
  void noInterrupts() {
      printf("TO IMPL. noInterrupts()\n");
  }



  void delay(long millis) {
    // usleep( millis * 1000 );
    SDL_Delay( millis );
  }

  long millis() {
      printf("TO IMPL. millis()\n");
    return 500;
  }

//   static int consoled = -1;

  void tone(int BUZZPIN, int freq, int dur) {
    //    printf("TO IMPL. tone(%d, %d)\n", freq, dur);
    // if ( consoled < 0 ) {
    //     //consoled = open("/dev/console", O_RDONLY);
    //     consoled = open("/dev/tty6", O_RDONLY);
    //     if ( !consoled ) {
    //         Serial.println("FAILED W/ CONSOLE");
    //         closeComputer();
    //     }
    // }
    // ioctl(consoled, KDMKTONE, (dur<<16 | 1193180/freq));

    // // /* start beep */
    // // ioctl(consoled, KIOCSOUND, 1193180/freq);
    // // /* sleep for the duration time  */
    // // usleep(dur * 1000);
    // // /* stop beep */
    // // ioctl(consoled, KIOCSOUND, 0);

    Beep(freq, dur);
    //buzzer.wait();
  }

  void noTone(int BUZZPIN) {
  }


  void digitalWrite(int pin, int value) {
      //printf("TO IMPL. digitalWrite(%d, %d)\n", pin, value);
  }


  int digitalRead(int pin) {
      //printf("TO IMPL. digitalRead(%d)\n", pin);
      //return LOW;

      // Cf INPUT_PULLUP btns
      return HIGH;
  }

  int analogRead(int pin) {
      printf("TO IMPL. analogRead(%d)\n", pin);
      return 512;
  }

  void pinMode(int pin, int mode) {
      printf("TO IMPL. pinMode(%d, %d)\n", pin, mode);
  }

#endif