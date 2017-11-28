/**************
 * Computer - Arduino emulation layer
 * Xtase - fgalliat @Nov 2017 
 *************/

#ifndef COMPUTER_H_
#define COMPUTER_H_ 1

 #include <cstdlib>
 #include <cstring>
 #include <stdio.h>
 #include <unistd.h>

 #include <sys/types.h>
 #include <iostream>
 #include <dirent.h>

 #include <ncurses.h>
 #include <sys/ioctl.h>
 #include <fcntl.h>
 #include <linux/kd.h>

 #include <SDL2/SDL.h>
 #include "dev_sound_pcspeaker.h"

  static SDL_Window *window;

 #define min(a,b) a<b ? a : b

// ================ Serial Console emulation ============
  class _Serial {
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
            lastKC = getch(); // ncurses version
            return lastKC > -1 ? 1 : 0;
          }
  
          int read() { 
            int ret = lastKC;
            lastKC = -1;
            return ret;
          }
  
          void _printAt(int x, int y, char* str) {
            mvprintw(y, x, str);
          }


          void print(char v) { printf("%c", v); }
          void print(int v) { printf("%d", v); }
          void println(int v) { printf("%d\n", v); }
          
          void print(const char* v) { 
              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = '\n'; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              printf("%s", vv); 
              free(vv);        
              refresh();      
          }

          void println(const char* v) { 
              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = 0x00; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              printf("%s\n", vv); 
              free(vv);
              refresh();
          }

          void flush() {}
  };


  static _Serial Serial = _Serial();

// ======== VGAT output emulation (extern prgm) =======

  class _FakedSerial {
      private:
        bool portOK = false;
        // N.B. : unsigned long  is very important
        unsigned long lastTime = 0;
        int lastKC = -1;
        FILE* pipeFile;
      public:
          void begin(int speed) {
              pipeFile = fopen("/tmp/vgat", "w");
              if ( !pipeFile ) {
                  Serial.println("Failed to open SerialVGA port\n");
                  return;
              }
              portOK = true;
          }
          
          void close() {
              // 4 is arbitrary EXIT CODE
              // because 3 (break) failed
              if ( portOK ) { fputc(0x04, pipeFile); flush(); fclose(pipeFile); }
              //remove("/tmp/vgat");
          }

          // output ONLY port
          int available() { 
            return 0;
          }
  
          // output ONLY port
          int read() { 
            return -1;
          }
  

          void print(char v) { 
              if ( !portOK ) { return; }
              fprintf(pipeFile, "%c", v); 

              flush();
          }
          void print(int v) { 
              if ( !portOK ) { return; }
              fprintf(pipeFile, "%d", v); 

              flush();
          }
          void println(int v) { 
              if ( !portOK ) { return; }
              fprintf(pipeFile, "%d\n", v); 

              flush();
          }
          
          void print(const char* v) { 
              if ( !portOK ) { return; }

              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = '\n'; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              fprintf(pipeFile, "%s", vv); 
              free(vv);        

              flush();
          }

          void printf(const char* v, int i) { 
              if ( !portOK ) { return; }

              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = '\n'; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              fprintf(pipeFile, vv, i); 
              free(vv);        

              flush();
          }

          void println(const char* v) { 
              if ( !portOK ) { return; }

              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = 0x00; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              fprintf(pipeFile, "%s\n", vv); 
              free(vv);

              flush();
          }

          void flush() {
              if ( !portOK ) { return; }

              fflush(pipeFile);
          }
  };


  // the serial port used for VGAT
  static _FakedSerial Serial3 = _FakedSerial();

// ======== VGAT output emulation (extern prgm) =======

  class _FakedSoftwareSerial {
      private:
        bool portOK = false;
        // N.B. : unsigned long  is very important
        unsigned long lastTime = 0;
        int lastKC = -1;
        FILE* pipeFile;
        FILE* pipeFileIn;
      public:
          _FakedSoftwareSerial(int rxPin, int txPin) {
          }
          ~_FakedSoftwareSerial() {
              close();
          }

          void begin(int speed) {
              pipeFile = fopen("/tmp/rpid_out", "w");
              if ( !pipeFile ) {
                  Serial.println("Failed to open SerialVGA port\n");
                  return;
              }

              pipeFileIn = fopen("/tmp/rpid_in", "r");

              portOK = true;
          }
          
          void close() {
              // 4 is arbitrary EXIT CODE
              // because 3 (break) failed
              if ( portOK ) { 
                  fputc(0x04, pipeFile); 
                  flush(); 
                  fclose(pipeFile);

                  fclose(pipeFileIn); 
              }
          }

          // input port emulation TODO
          int available() { 
            return 0;
          }
  
          // input port emulation TODO
          int read() { 
            return -1;
          }
  

          void print(char v) { 
              if ( !portOK ) { return; }
              fprintf(pipeFile, "%c", v); 

              flush();
          }
          void print(int v) { 
              if ( !portOK ) { return; }
              fprintf(pipeFile, "%d", v); 

              flush();
          }
          void println(int v) { 
              if ( !portOK ) { return; }
              fprintf(pipeFile, "%d\n", v); 

              flush();
          }
          
          void print(const char* v) { 
              if ( !portOK ) { return; }

              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = '\n'; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              fprintf(pipeFile, "%s", vv); 
              free(vv);        

              flush();
          }

          void printf(const char* v, int i) { 
              if ( !portOK ) { return; }

              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = '\n'; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              fprintf(pipeFile, vv, i); 
              free(vv);        

              flush();
          }

          void println(const char* v) { 
              if ( !portOK ) { return; }

              int l = strlen(v);
              char* vv = (char*)malloc(l+1);
              for ( int i=0; i < l; i++ ) {
                  if ( vv[i] == '\r' ) { vv[i] = 0x00; }
                  else vv[i] = v[i];
              }
              vv[l] = 0x00;
              fprintf(pipeFile, "%s\n", vv); 
              free(vv);

              flush();
          }

          void flush() {
              if ( !portOK ) { return; }

              fflush(pipeFile);
          }
  };

  #define SoftwareSerial _FakedSoftwareSerial

// ======== File System Emulation ===========

  #define O_READ 1
  #define FILE_READ O_READ
  #define O_WRITE 2
  #define FILE_WRITE O_WRITE
  #define BUILTIN_SDCARD 1

// struct dirent {
//     ino_t d_ino; /* inode number */
//     off_t d_off; /* offset to the next dirent */
//     unsigned short d_reclen; /* length of this record */
//     unsigned char d_type; /* type of file */
//     char d_name[256]; /* filename */
// };

  class SdFile {
      private:
        char name[8+1+3+1];
        bool dirMode = false;

        DIR *dir;
        FILE *file;
        bool _isDir = false;
        bool opened = false;
      public:
        SdFile() {}
        ~SdFile() {}

        bool open(char* filename, int mode) {

            memcpy( name, filename, min( 8+1+3, strlen( filename ) ) );
            opened = true;

            // a bit dirty I know...
            if ( strcmp(filename, "/") == 0 ) {
                dirMode = true;
                //dir = opendir(filename);
                dir = opendir("./FS/");
                if (dir == NULL) {
                    return false;
                }
            } else {
                char fullEntryName[ 5 + 8+1+3 +1 ];
                strcpy(fullEntryName, "./FS/");
                strcat(fullEntryName, filename);

                Serial.println( fullEntryName );

                file = fopen(fullEntryName, mode == O_READ ? "r" : "w");
                if ( !file ) {
                    return false;
                }
            }

            return true;
        }

        void close() {
            if (dirMode) {
                closedir( dir );
            } else {
                if ( file != NULL && opened ) {
                  fclose( file );
                }
            }
        }

        void flush() {
        }

        void print( char* str ) {
            fputs(str, file);
        }

        void print( int v ) {
            char num[10];
            sprintf(num, "%d");
            fputs(num, file);
        }

        int fgets(char* dest, int maxLen) {
            char c; int cpt = 0;
            while ( (c = fgetc(file)) != EOF && cpt < maxLen) {
                if (c == '\r' || c == '\n') { break; }
                dest[cpt++] = c;
                //Serial.print( c );
            }
            return cpt;
        }

        int read(void* dest, int maxLen) {
            return fread( dest, 1, maxLen, file );
        }
        
        int read() {
            unsigned char res[1];
            fread(res, 1, 1, file);
            return (int)res[0];
        }

        void seekSet(int offset) {
            fseek(file, offset, SEEK_SET);
        }

        bool openNext( SdFile* fs_entry, int mode ) {
            DIR* parent = fs_entry->dir;
            struct dirent *entry;
            while( true ) {
                entry = readdir(parent);
                if ( entry == NULL ) {
                    return false;
                }

                // 10 -> file
                // 4  -> dir
                _isDir = entry->d_type == 4;

                memset( name, 0x00, 8+1+3 );
                if ( entry->d_name == NULL ) {
                    memcpy( name, "?????", 5 );
                } else {
                    //Serial.println( entry->d_name );
                    memcpy( name, entry->d_name, min(8+1+3, strlen(entry->d_name) ) );
                    //Serial.println( entry->d_type );
                }

                if ( _isDir && ( strcmp( name, "." ) == 0) || ( strcmp( name, ".." ) == 0) ) {
                    continue;
                }
                break;
            }
            return true;
        }

        bool isSubDir() {
            return false;
        }

        bool isDir() {
            return _isDir;
        }

        bool isHidden() {
            return false;
        }

        void getName(char* dest, int maxLen) {
            if ( name == NULL ) {
                memcpy( dest, "?????", 5 );
            }
            // Serial.println( "before" );
            memcpy(dest, name, strlen(name));
        }

  };

  class SDClass {
      private:
      public:
        SDClass() {}
        ~SDClass() {}

        bool begin(int pin) {
            return true;
        }

        int remove(char* filename) {
            Serial.println("FS.remove to impl.");
            return 0;
        }

  };

  static SDClass SD; 

  // ===================================


  void setupComputer();


  void closeComputer();


  void interrupts();
  void noInterrupts();

  void delay(long millis);

  long millis();

  void tone(int BUZZPIN, int freq, int dur);

  void noTone(int BUZZPIN);


  #define HIGH 255
  #define LOW    0

  #define OUTPUT       1
  #define INPUT        2
  #define INPUT_PULLUP 3

  void digitalWrite(int pin, int value);


  int digitalRead(int pin);

  int analogRead(int pin);

  void pinMode(int pin, int mode);



#endif