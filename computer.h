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
#include <fstream>
#include <dirent.h>

#include <ncurses.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>

#include <math.h>
#include <SDL2/SDL.h>

#include "dev_sound_pcspeaker.h"

static void PC_ISR();
static void __DrawPixel(int x, int y, int brightness);
#include "dev_screen_Adafruit_SSD1306_emul.h"

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define abs(a) (a < 0 ? -a : a)
static void swap(int& a, int& b) { int swp=a; a=b; b=swp; }

#define icos(a) cos((double)a / 3.141596 * 180.0)
#define isin(a) sin((double)a / 3.141596 * 180.0)

#define WHITE 0x01
#define BLACK 0x00
#define SSD1306_LCDHEIGHT 64
#define SSD1306_SWITCHCAPVCC 1111

#define KBD_CTRL_C 3
#define KBD_BREAK 1000

// ================ Serial Console emulation ============
class _Serial
{
  private:
    // N.B. : unsigned long  is very important
    unsigned long lastTime = 0;
    int lastKC = -1;

  public:
    void begin(int speed)
    {
        WINDOW *w = initscr();
        cbreak();
        noecho();
        nodelay(w, TRUE); // prevent getch() from blocking !!!!!!
    }

    int available()
    {

        PC_ISR();

        lastKC = getch(); // ncurses version
        return lastKC > -1 ? 1 : 0;
    }

    int read()
    {
        int ret = lastKC;
        lastKC = -1;
        return ret;
    }

    void _printAt(int x, int y, char *str)
    {
        mvprintw(y, x, str);
    }

    void print(char v) { printf("%c", v); }
    void print(int v) { printf("%d", v); }
    void println(int v) { printf("%d\n", v); }

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
        printf("%s", vv);
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
        printf("%s\n", vv);
        free(vv);
        refresh();
    }

    void flush() {}
};

static _Serial Serial = _Serial();

// ======== VGAT output emulation (extern prgm) =======

static bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

class _FakedSerial
{
  private:
    bool portOK = false;
    // N.B. : unsigned long  is very important
    unsigned long lastTime = 0;
    int lastKC = -1;
    FILE *pipeFile;

  public:
    void begin(int speed)
    {

        //   if ( ! is_file_exist("/tmp/vgat") ) {
        //       Serial.println("Failed to open SerialVGA port (x1)\n");
        //       portOK = false;
        //       return;
        //   }

        pipeFile = fopen("/tmp/vgat", "w+");
        if (pipeFile != NULL)
        {
            Serial.println("Failed to open SerialVGA port\n");
            portOK = false;
            return;
        }
        portOK = true;
    }

    void close()
    {
        // 4 is arbitrary EXIT CODE
        // because 3 (break) failed
        if (portOK)
        {
            fputc(0x04, pipeFile);
            flush();
            fclose(pipeFile);
        }
        //remove("/tmp/vgat");
    }

    // output ONLY port
    int available()
    {
        return 0;
    }

    // output ONLY port
    int read()
    {
        return -1;
    }

    void print(char v)
    {
        if (!portOK)
        {
            return;
        }
        fprintf(pipeFile, "%c", v);

        flush();
    }
    void print(int v)
    {
        if (!portOK)
        {
            return;
        }
        fprintf(pipeFile, "%d", v);

        flush();
    }
    void println(int v)
    {
        if (!portOK)
        {
            return;
        }
        fprintf(pipeFile, "%d\n", v);

        flush();
    }

    void print(const char *v)
    {
        if (!portOK)
        {
            return;
        }

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
        fprintf(pipeFile, "%s", vv);
        free(vv);

        flush();
    }

    void printf(const char *v, int i)
    {
        if (!portOK)
        {
            return;
        }

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
        fprintf(pipeFile, vv, i);
        free(vv);

        flush();
    }

    void println(const char *v)
    {
        if (!portOK)
        {
            return;
        }

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
        fprintf(pipeFile, "%s\n", vv);
        free(vv);

        flush();
    }

    void flush()
    {
        if (!portOK)
        {
            return;
        }

        fflush(pipeFile);
    }
};

// the serial port used for VGAT
static _FakedSerial Serial3 = _FakedSerial();

// ======== VGAT output emulation (extern prgm) =======

class _FakedHardwareSerial
{
  private:
    bool portOK = false;
    // N.B. : unsigned long  is very important
    unsigned long lastTime = 0;
    int lastKC = -1;
    FILE *pipeFile;
    FILE *pipeFileIn;

  public:
    _FakedHardwareSerial()
    {
    }
    ~_FakedHardwareSerial()
    {
        close();
    }

    void setRX(int pin) {}
    void setTX(int pin) {}

    void begin(int speed)
    {
        //   if ( ! is_file_exist("/tmp/rpid_out") ) {
        //       Serial.println("Failed to open SerialRPID out port (x1)\n");
        //       portOK = false;
        //       return;
        //   }

        pipeFile = fopen("/tmp/rpid_out", "w+");
        if (!pipeFile)
        {
            Serial.println("Failed to open SerialRPID out port\n");
            portOK = false;
            return;
        }

        if (is_file_exist("/tmp/rpid_in"))
        {
            pipeFileIn = fopen("/tmp/rpid_in", "r+");
            if (!pipeFileIn)
            {
                Serial.println("Failed to open SerialRPID in port in\n");
                //portOK = false;
                //return;
            }
        }

        portOK = true;
    }

    void close()
    {
        // 4 is arbitrary EXIT CODE
        // because 3 (break) failed
        if (portOK)
        {
            fprintf(pipeFile, "^[halt\n");
            flush();
            fclose(pipeFile);

            fclose(pipeFileIn);
        }
    }

    // input port emulation TODO
    int available()
    {
        return 0;
    }

    // input port emulation TODO
    int read()
    {
        return -1;
    }

    void print(char v)
    {
        if (!portOK)
        {
            return;
        }
        fprintf(pipeFile, "%c", v);

        flush();
    }
    void print(int v)
    {
        if (!portOK)
        {
            return;
        }
        fprintf(pipeFile, "%d", v);

        flush();
    }
    void println(int v)
    {
        if (!portOK)
        {
            return;
        }
        fprintf(pipeFile, "%d\n", v);

        flush();
    }

    void print(const char *v)
    {
        if (!portOK)
        {
            return;
        }

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
        fprintf(pipeFile, "%s", vv);
        free(vv);

        flush();
    }

    void printf(const char *v, int i)
    {
        if (!portOK)
        {
            return;
        }

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
        fprintf(pipeFile, vv, i);
        free(vv);

        flush();
    }

    void println(const char *v)
    {
        if (!portOK)
        {
            return;
        }

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
        fprintf(pipeFile, "%s\n", vv);
        free(vv);

        flush();
    }

    void flush()
    {
        if (!portOK)
        {
            return;
        }

        fflush(pipeFile);
    }
};

// the serial port used for RPIDisplay
static _FakedHardwareSerial Serial1 = _FakedHardwareSerial();

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

class SdFile
{
  private:
    char name[8 + 1 + 3 + 1];
    bool dirMode = false;

    DIR *dir;
    FILE *file;
    bool _isDir = false;
    bool opened = false;

  public:
    SdFile() {}
    ~SdFile() {}

    bool open(char *filename, int mode)
    {

        memcpy(name, filename, min(8 + 1 + 3, strlen(filename)));
        opened = true;

        // a bit dirty I know...
        if (strcmp(filename, "/") == 0)
        {
            dirMode = true;
            //dir = opendir(filename);
            dir = opendir("./FS/");
            if (dir == NULL)
            {
                return false;
            }
        }
        else
        {
            char fullEntryName[5 + 8 + 1 + 3 + 1];
            strcpy(fullEntryName, "./FS/");
            strcat(fullEntryName, filename);

            Serial.println(fullEntryName);

            file = fopen(fullEntryName, mode == O_READ ? "r" : "w");
            if (!file)
            {
                return false;
            }
        }

        return true;
    }

    void close()
    {
        if (dirMode)
        {
            closedir(dir);
        }
        else
        {
            if (file != NULL && opened)
            {
                fclose(file);
            }
        }
    }

    void flush()
    {
    }

    void print(char *str)
    {
        fputs(str, file);
    }

    void print(int v)
    {
        char num[10];
        sprintf(num, "%d");
        fputs(num, file);
    }

    int fgets(char *dest, int maxLen)
    {
        char c;
        int cpt = 0;
        while ((c = fgetc(file)) != EOF && cpt < maxLen)
        {
            if (c == '\r' || c == '\n')
            {
                break;
            }
            dest[cpt++] = c;
            //Serial.print( c );
        }
        return cpt;
    }

    int read(void *dest, int maxLen)
    {
        return fread(dest, 1, maxLen, file);
    }

    int read()
    {
        unsigned char res[1];
        fread(res, 1, 1, file);
        return (int)res[0];
    }

    void seekSet(int offset)
    {
        fseek(file, offset, SEEK_SET);
    }

    bool openNext(SdFile *fs_entry, int mode)
    {
        DIR *parent = fs_entry->dir;
        struct dirent *entry;
        while (true)
        {
            entry = readdir(parent);
            if (entry == NULL)
            {
                return false;
            }

            // 10 -> file
            // 4  -> dir
            _isDir = entry->d_type == 4;

            memset(name, 0x00, 8 + 1 + 3);
            if (entry->d_name == NULL)
            {
                memcpy(name, "?????", 5);
            }
            else
            {
                //Serial.println( entry->d_name );
                memcpy(name, entry->d_name, min(8 + 1 + 3, strlen(entry->d_name)));
                //Serial.println( entry->d_type );
            }

            if (_isDir && (strcmp(name, ".") == 0) || (strcmp(name, "..") == 0))
            {
                continue;
            }
            break;
        }
        return true;
    }

    bool isSubDir()
    {
        return false;
    }

    bool isDir()
    {
        return _isDir;
    }

    bool isHidden()
    {
        return false;
    }

    void getName(char *dest, int maxLen)
    {
        if (name == NULL)
        {
            memcpy(dest, "?????", 5);
        }
        // Serial.println( "before" );
        memcpy(dest, name, strlen(name));
    }
};

class SDClass
{
  private:
  public:
    SDClass() {}
    ~SDClass() {}

    bool begin(int pin)
    {
        return true;
    }

    int remove(char *filename)
    {
        Serial.println("FS.remove to impl.");
        return 0;
    }
};

static SDClass SD;

// ===================================

#define Uint32 unsigned int

class Adafruit_SSD1306
{
  private:
    int y = 0, x = 0;
    int SCREEN_HEIGHT = 64, SCREEN_WIDTH = 128;
    Uint32 *pixels = NULL;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Surface *surface;

    SDL_Color color;
    SDL_Color black;

    int txtColor = 1;

    void __blit()
    {
    }

  public:
    Adafruit_SSD1306(int rstPin)
    {
        printf("je passe ICI\n");
    }

    void __init()
    {
        window = SDL_CreateWindow("SSD1306", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS);
        if (window == NULL)
        {
            std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            exit(1);
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == NULL)
        {
            std::cout << "Failed to create renderer : " << SDL_GetError();
            exit(1);
        }

        surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                       32,
                                       0,
                                       0,
                                       0,
                                       0);

        if (surface == NULL)
        {
            printf("Could not create surface !!!!\n");
            exit(1);
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

        // monochrome blue
        color.r = 50;
        color.g = 50;
        color.b = 255;

        black.r = 0;
        black.g = 0;
        black.b = 0;
    }

    ~Adafruit_SSD1306() {}

    void close()
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyWindow(window);
    }

    void begin(int mode, int addr)
    {
    }

    void setTextColor(int color)
    {
        // TODO : XOR ...
        txtColor = color;
    }

    void setTextSize(int size)
    {
        // usually 1
    }

    void setCursor(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void println(char *str)
    {
        for (int i = 0; i < strlen(str); i++)
        {
            print(str[i]);
        }
        y += 8;
    }

    void print(char ch)
    {

        if (surface == NULL)
        {
            // Serial.print(ch);
            // return;
            __init();
        }

        if (ch == '\n')
        {
            this->x = 0;
            this->y += 8;
            // scrollIfNeeded();
            return;
        }

        // see also : https://github.com/watterott/Arduino-Libs/blob/master/GraphicsLib/fonts.h
        DrawChar(ch, this->x, this->y, 1);

        this->x += 6;
    }

    void clearDisplay()
    {
        x = 0;
        y = 0;
        if (surface == NULL)
        {
            Serial.println("NULL Surface");
            return;
        }
        // cls
        SDL_FillRect(surface, NULL, 0);
    }

    void display()
    {
        //__blit();
    }

    void drawBitmap(int x, int y, unsigned char *picBuff, int width, int height, unsigned int color)
    {
        if (color == 0x01)
        {
        }
        else
        {
            // TODO xor .....
        }
        unsigned char c;
        for (int yy = 0; yy < height; yy++)
        {
            for (int xx = 0; xx < width; xx++)
            {
                c = (picBuff[(yy * (width / 8)) + (xx / 8)] >> (7 - ((xx) % 8))) % 2;
                if (c == 0x00)
                {
                    drawPixel(x + xx, y + yy, 0);
                }
                else
                {
                    drawPixel(x + xx, y + yy, 1);
                }
            }
        }
        // __blit();
    }

    void drawPixel(int x, int y, unsigned int color)
    {
        int zoom = 1;
        SDL_Rect r;
        r.x = x * zoom;
        r.y = y * zoom;
        r.w = zoom;
        r.h = zoom;

        SDL_Color usedColor = color == 0x00 ? this->black : this->color;

        // draw a rect
        SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, usedColor.r, usedColor.g, usedColor.b));
    }

    void drawCircle(int x, int y, int radius, unsigned int color)
    {
        // very slow impl.
        for (int i = 0; i < 360; i++)
        {
            int xx = (int)((double)radius * icos(i));
            int yy = (int)((double)radius * isin(i));
            this->drawPixel(x + xx, y + yy, color);
        }
    }

    void drawLine(int x1, int y1, int x2, int y2, unsigned int color)
    {
        if (surface == NULL)
        {
            //return;
            __init();
        }

        if (y1 == y2)
        {
            if (y1 < 0 || y1 >= SCREEN_HEIGHT)
            {
                return;
            }
            if (x2 < x1)
            {
                int swap = x2;
                x2 = x1;
                x1 = swap;
            }

            int e0 = max(x1, 0);
            int e1 = min(x2, this->SCREEN_WIDTH - 1);

            for (int i = e0; i <= e1; i++)
            {
                this->drawPixel(i, y1, color);
            }
        }
        else if (x1 == x2)
        {
            if (x1 < 0 || x1 >= SCREEN_WIDTH)
            {
                return;
            }
            if (y2 < y1)
            {
                int swap = y2;
                y2 = y1;
                y1 = swap;
            }

            int e0 = max(y1, 0);
            int e1 = min(y2, this->SCREEN_HEIGHT - 1);

            for (int i = e0; i <= e1; i++)
            {
                this->drawPixel(x1, i, color);
            }
        }
        else
        {
            //     int X1 = x1, X2 = x2, Y1 = y1, Y2 = y2;
            //     if (x1 > x2)
            //     {
            //         X1 = x2;
            //         Y1 = y2;
            //         X2 = x1;
            //         Y2 = y1;
            //     }

            //     int dx = X2 - X1;
            //     int dy = Y2 - Y1;
            //     int _y;
            //     for (int _x = X1; _x < X2; _x++)
            //     {
            //         _y = Y1 + dy * (_x - X1) / dx;
            //         this->drawPixel(_x, _y, color);
            //     }

            int x0 = x1;
            int y0 = y1;
            x1 = x2;
            y1 = y2;
            int16_t steep = abs(y1 - y0) > abs(x1 - x0);
            if (steep)
            {
                swap(x0, y0);
                swap(x1, y1);
            }

            if (x0 > x1)
            {
                swap(x0, x1);
                swap(y0, y1);
            }

            int16_t dx, dy;
            dx = x1 - x0;
            dy = abs(y1 - y0);

            int16_t err = dx / 2;
            int16_t ystep;

            if (y0 < y1)
            {
                ystep = 1;
            }
            else
            {
                ystep = -1;
            }

            for (; x0 <= x1; x0++)
            {
                if (steep)
                {
                    drawPixel(y0, x0, color);
                }
                else
                {
                    drawPixel(x0, y0, color);
                }
                err -= dy;
                if (err < 0)
                {
                    y0 += ystep;
                    err += dx;
                }
            }
        }
    }

    void drawFastHLine(int x, int y, int x2, int color)
    {
        drawLine(x, y, x2, y, color);
    }

    void _MEGABlitt()
    {
        if (surface == NULL)
        {
            __init();

            // cls
            SDL_FillRect(surface, NULL, 0);
        }

        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
};

//static Adafruit_SSD1306 display(-1);
extern Adafruit_SSD1306 display;

static void __DrawPixel(int x, int y, int brightness)
{
    display.drawPixel(x, y, brightness > 0 ? 1 : 0);
}

static void PC_ISR()
{
    display._MEGABlitt();
}

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
#define LOW 0

#define OUTPUT 1
#define INPUT 2
#define INPUT_PULLUP 3

void digitalWrite(int pin, int value);

int digitalRead(int pin);

int analogRead(int pin);

void pinMode(int pin, int mode);

#endif