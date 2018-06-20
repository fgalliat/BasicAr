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

      #define PS2_ENTER 13
      char serBuff[32+1];
      int  serBuffCursor = 0;

      int available() {
          PC_ISR(); // by computer available() call

          if ( serBuffCursor >= 32 ) {
              return 32;
          }

          lastKC = getch(); // ncurses version

          if ( lastKC < 0 ) {
              return serBuffCursor;
          }

          if ( lastKC == '\r' ) {
              // ....
          } else if ( lastKC == '\n' ) {
            serBuff[ serBuffCursor++ ] = '\r';
            // serBuff[ serBuffCursor++ ] = '\n';
            serBuff[ serBuffCursor ] = 0x00;
          } else {
            serBuff[ serBuffCursor++ ] = lastKC;
            serBuff[ serBuffCursor ] = 0x00;
          }

          // if (lastKC > -1) { std::printf(">%d<(%c)\r\n", lastKC, lastKC); }

          if ( serBuffCursor < 0 ) { serBuffCursor = 0; }
          return serBuffCursor;
      }


      int read() {
          if ( serBuffCursor <= 0 ) {
              return -1;
          }

          int kc = serBuff[0];
          // std::printf(">%d<(%c) ---->(%d)\r\n", kc, kc, serBuffCursor);

          // TODO : better
          for(int i=1; i < serBuffCursor; i++) {
              serBuff[i-1] = serBuff[i];
          }
          serBuff[serBuffCursor] = 0x00;
          serBuffCursor--;
          serBuff[serBuffCursor] = 0x00;

          return kc;
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