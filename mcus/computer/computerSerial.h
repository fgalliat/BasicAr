/*
* Fake MCU (computer) Serial header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Jun. 2018
*/


#ifdef COMPUTER

#ifndef COMPSERIAL 
#define COMPSERIAL 1

  // ================ Serial Console emulation ============
  static void PC_ISR();

  class _Serial
  {
    private:
      // N.B. : unsigned long  is very important
      unsigned long lastTime = 0;
      int lastKC = -1;

    public:
      void begin(int speed) {
          WINDOW *w = initscr();
          cbreak();
          noecho();
          nodelay(w, TRUE); // prevent getch() from blocking !!!!!!
      }

      int available() {

          PC_ISR(); // by computer available() call

          lastKC = getch(); // ncurses version

          if (lastKC > -1) { std::printf(">%d<(%c)\r\n", lastKC, lastKC); }

          return lastKC > -1 ? 1 : 0;
      }

      #define PS2_ENTER 13

      int read() {
          int ret = lastKC;

          if ( ret == '\n' ) {
              ret = PS2_ENTER;
          }

          lastKC = -1;
          return ret;
      }

      void _printAt(int x, int y, char *str) {
          mvprintw(y, x, str);
      }

      void print(char v) { std::printf("%c", v); }
      void print(int v) { std::printf("%d", v); }
      void print(float v) { std::printf("%f", v); }
      void println(int v) { std::printf("%d\n", v); }

      void print(const char *v)
      {
          int l = strlen(v);
          char *vv = (char *)malloc(l + 1);
          for (int i = 0; i < l; i++)
          {
              if (vv[i] == '\r')
              {
                  vv[i] = '\n';
              }
              else
                  vv[i] = v[i];
          }
          vv[l] = 0x00;
          std::printf("%s", vv);
          free(vv);
          refresh();
      }

      void println(const char *v)
      {
          int l = strlen(v);
          char *vv = (char *)malloc(l + 1);
          for (int i = 0; i < l; i++)
          {
              if (vv[i] == '\r')
              {
                  vv[i] = 0x00;
              }
              else
                  vv[i] = v[i];
          }
          vv[l] = 0x00;
          std::printf("%s\n", vv);
          free(vv);
          refresh();
      }

      void printf(const char* format, char* str, int val) {
        std::printf(format, str, val);
      }

      void printf(const char* format, char* str, char val) {
        std::printf(format, str, val);
      }

      void printf(const char* format, char* str) {
        std::printf(format, str);
      }

      void flush() {}
  };

  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif

#endif