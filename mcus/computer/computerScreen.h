/*
* Fake MCU (computer) Screen header
* 
* designed for Xts-uBASIC (Xtase-fgalliat @Jan16)
*
* Xtase - fgalliat @ Jun. 2018
*/


#ifdef COMPUTER

#ifndef COMPSCREEN 
#define COMPSCREEN 1

static void DrawChar(char c, uint8_t x, uint8_t y, uint8_t brightness);

class Adafruit_SSD1306
{
  private:
    int y = 0, x = 0;
    //int SCREEN_HEIGHT = 64, SCREEN_WIDTH = 128;
    int SCREEN_HEIGHT = 240, SCREEN_WIDTH = 320;
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
       // printf("je passe ICI\n");
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

        // white
        color.r = 255;
        color.g = 255;
        color.b = 255;

        black.r = 0;
        black.g = 0;
        black.b = 0;

        // printf("created surface !!!!\n");

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

    void print(char *str) {
        for (int i = 0; i < strlen(str); i++) {
            print(str[i]);
        }
    }

    void println(char *str) {
        for (int i = 0; i < strlen(str); i++) {
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
            
            // TEMP
            if ( this->y > 240/8 ) { this->clearDisplay(); }

            // scrollIfNeeded();
            return;
        }

        // see also : https://github.com/watterott/Arduino-Libs/blob/master/GraphicsLib/fonts.h
        DrawChar(ch, this->x, this->y, 1);

        // std::printf( "%c", ch );

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

#define INTEL_MODE 1

    // 16bpp
    void pushImage(int x, int y, int scanSize, int height, uint16_t *picBuff)
    {
        int width = scanSize / height / 2;
        width = scanSize; //???
        uint16_t c;
        for (int yy = 0; yy < height; yy++)
        {
            for (int xx = 0; xx < width; xx++)
            {
                c = picBuff[(yy * width) + (xx)];
                drawPixel(x + xx, y + yy, c, false && ( yy == 0 && xx < 3 ) );
            }
        }
    }

#define TFT_eSPI__color565(r, g, b) { ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3) }

    void drawPixel(int x, int y, uint16_t color, bool _DBUG=false)
    {
        int zoom = 1;
        SDL_Rect r;
        r.x = x * zoom;
        r.y = y * zoom;
        r.w = zoom;
        r.h = zoom;

        SDL_Color _color;

if ( color > 8 ) {
      #ifdef INTEL_MODE
        // Intel endian ?
        color = (color%256)*256 + color/256;
      #endif

      int _r = (unsigned int)((color >> 11) * (255/31) /* % (unsigned char)0xF8*/ );
      int _g = (unsigned int)(( ((color) >> 5) % (unsigned char)0x40) * (255/63) /*% (unsigned char)0xFC*/);
      int _b = (unsigned int)(color % (unsigned char)0x20) * (255/31);

if (_DBUG) {
    printf("r=%d g=%d b=%d \n", _r, _g, _b);
}
        _color.r = _r;
        _color.g = _g;
        _color.b = _b;
} else {
    SDL_Color _color;
    if ( color == 2 ) {
        // LIGHTGREY
        _color.r = 200;
        _color.g = 200;
        _color.b = 200;
    } else if ( color == 3 ) {
        // GREY
        _color.r = 128;
        _color.g = 128;
        _color.b = 128;
    } else if ( color == 4 ) {
        // DARKGREY
        _color.r = 50;
        _color.g = 50;
        _color.b = 50;
    } else if ( color == 5 ) {
        // LIGHTGREEN
        _color.r = 150;
        _color.g = 255;
        _color.b = 150;
    } else if ( color == 6 ) {
        // GREEN
        _color.r = 80;
        _color.g = 255;
        _color.b = 80;
    } else if ( color == 7 ) {
        // DARKGREEN
        _color.r = 0;
        _color.g = 150;
        _color.b = 0;
    } else if ( color == 8 ) {
        // PINK
        _color.r = 249;
        _color.g = 13;
        _color.b = 234;
    } 
}


        SDL_Color usedColor = color == 0x00 ? this->black : color == 0x01 ? this->color : _color;

        // draw a rect
        SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, usedColor.r, usedColor.g, usedColor.b));
    }

    void fillRect(int x, int y, int w, int h, unsigned int color)
    {
        int zoom = 1;
        SDL_Rect r;
        r.x = x * zoom;
        r.y = y * zoom;
        r.w = w * zoom;
        r.h = h * zoom;

        SDL_Color usedColor = color == 0x00 ? this->black : this->color;

        // draw a rect
        SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, usedColor.r, usedColor.g, usedColor.b));
    }

    void drawRect(int x, int y, int w, int h, unsigned int color)
    {
        int zoom = 1;
        SDL_Rect r;
        r.x = x * zoom;
        r.y = y * zoom;
        r.w = w * zoom;
        r.h = h * zoom;

        this->drawLine(x, y, x+w, y, color);
        this->drawLine(x+w, y, x+w, y+h, color);
        this->drawLine(x+w, y+h, x, y+h, color);
        this->drawLine(x, y+h, x, y, color);
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
    SDL_Delay(100);
    display._MEGABlitt();
}

// =============================================
// =============================================
// =============================================

//if defined char range 0x20-0x7F otherwise 0x20-0xFF
#define FONT_END7F
#define FONT_START (0x20) //first character

#define FONT_WIDTH (6)
#define FONT_HEIGHT (8)

const uint8_t font_PGM[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x20
        0x04, 0x0E, 0x0E, 0x04, 0x04, 0x00, 0x04, 0x00, // 0x21
        0x1B, 0x1B, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x22
        0x00, 0x0A, 0x1F, 0x0A, 0x0A, 0x1F, 0x0A, 0x00, // 0x23
        0x08, 0x0E, 0x10, 0x0C, 0x02, 0x1C, 0x04, 0x00, // 0x24
        0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13, 0x00, // 0x25
        0x08, 0x14, 0x14, 0x08, 0x15, 0x12, 0x0D, 0x00, // 0x26
        0x0C, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x27
        0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x00, // 0x28
        0x08, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08, 0x00, // 0x29
        0x00, 0x0A, 0x0E, 0x1F, 0x0E, 0x0A, 0x00, 0x00, // 0x2A
        0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00, 0x00, // 0x2B
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x08, // 0x2C
        0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, // 0x2D
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00, // 0x2E
        0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00, // 0x2F
        0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E, 0x00, // 0x30
        0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0x31
        0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F, 0x00, // 0x32
        0x0E, 0x11, 0x01, 0x0E, 0x01, 0x11, 0x0E, 0x00, // 0x33
        0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02, 0x00, // 0x34
        0x1F, 0x10, 0x10, 0x1E, 0x01, 0x11, 0x0E, 0x00, // 0x35
        0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E, 0x00, // 0x36
        0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, 0x00, // 0x37
        0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E, 0x00, // 0x38
        0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C, 0x00, // 0x39
        0x00, 0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00, // 0x3A
        0x00, 0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x08, // 0x3B
        0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02, 0x00, // 0x3C
        0x00, 0x00, 0x1F, 0x00, 0x00, 0x1F, 0x00, 0x00, // 0x3D
        0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x00, // 0x3E
        0x0E, 0x11, 0x01, 0x06, 0x04, 0x00, 0x04, 0x00, // 0x3F
        0x0E, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0E, 0x00, // 0x40
        0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x00, // 0x41
        0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E, 0x00, // 0x42
        0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E, 0x00, // 0x43
        0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E, 0x00, // 0x44
        0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F, 0x00, // 0x45
        0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10, 0x00, // 0x46
        0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F, 0x00, // 0x47
        0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11, 0x00, // 0x48
        0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0x49
        0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0E, 0x00, // 0x4A
        0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11, 0x00, // 0x4B
        0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, // 0x4C
        0x11, 0x1B, 0x15, 0x11, 0x11, 0x11, 0x11, 0x00, // 0x4D
        0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11, 0x00, // 0x4E
        0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00, // 0x4F
        0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10, 0x00, // 0x50
        0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D, 0x00, // 0x51
        0x1E, 0x11, 0x11, 0x1E, 0x12, 0x11, 0x11, 0x00, // 0x52
        0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E, 0x00, // 0x53
        0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, // 0x54
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00, // 0x55
        0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04, 0x00, // 0x56
        0x11, 0x11, 0x15, 0x15, 0x15, 0x15, 0x0A, 0x00, // 0x57
        0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11, 0x00, // 0x58
        0x11, 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x00, // 0x59
        0x1E, 0x02, 0x04, 0x08, 0x10, 0x10, 0x1E, 0x00, // 0x5A
        0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0E, 0x00, // 0x5B
        0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, // 0x5C
        0x0E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0E, 0x00, // 0x5D
        0x04, 0x0A, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x5E
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, // 0x5F
        0x0C, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x60
        0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x61
        0x10, 0x10, 0x1E, 0x11, 0x11, 0x11, 0x1E, 0x00, // 0x62
        0x00, 0x00, 0x0E, 0x11, 0x10, 0x11, 0x0E, 0x00, // 0x63
        0x01, 0x01, 0x0F, 0x11, 0x11, 0x11, 0x0F, 0x00, // 0x64
        0x00, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x65
        0x06, 0x08, 0x08, 0x1E, 0x08, 0x08, 0x08, 0x00, // 0x66
        0x00, 0x00, 0x0F, 0x11, 0x11, 0x0F, 0x01, 0x0E, // 0x67
        0x10, 0x10, 0x1C, 0x12, 0x12, 0x12, 0x12, 0x00, // 0x68
        0x04, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x69
        0x02, 0x00, 0x06, 0x02, 0x02, 0x02, 0x12, 0x0C, // 0x6A
        0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12, 0x00, // 0x6B
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x6C
        0x00, 0x00, 0x1A, 0x15, 0x15, 0x11, 0x11, 0x00, // 0x6D
        0x00, 0x00, 0x1C, 0x12, 0x12, 0x12, 0x12, 0x00, // 0x6E
        0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E, 0x00, // 0x6F
        0x00, 0x00, 0x1E, 0x11, 0x11, 0x11, 0x1E, 0x10, // 0x70
        0x00, 0x00, 0x0F, 0x11, 0x11, 0x11, 0x0F, 0x01, // 0x71
        0x00, 0x00, 0x16, 0x09, 0x08, 0x08, 0x1C, 0x00, // 0x72
        0x00, 0x00, 0x0E, 0x10, 0x0E, 0x01, 0x0E, 0x00, // 0x73
        0x00, 0x08, 0x1E, 0x08, 0x08, 0x0A, 0x04, 0x00, // 0x74
        0x00, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0x75
        0x00, 0x00, 0x11, 0x11, 0x11, 0x0A, 0x04, 0x00, // 0x76
        0x00, 0x00, 0x11, 0x11, 0x15, 0x1F, 0x0A, 0x00, // 0x77
        0x00, 0x00, 0x12, 0x12, 0x0C, 0x12, 0x12, 0x00, // 0x78
        0x00, 0x00, 0x12, 0x12, 0x12, 0x0E, 0x04, 0x18, // 0x79
        0x00, 0x00, 0x1E, 0x02, 0x0C, 0x10, 0x1E, 0x00, // 0x7A
        0x06, 0x08, 0x08, 0x18, 0x08, 0x08, 0x06, 0x00, // 0x7B
        0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04, 0x00, // 0x7C
        0x0C, 0x02, 0x02, 0x03, 0x02, 0x02, 0x0C, 0x00, // 0x7D
        0x0A, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x7E
        0x04, 0x0E, 0x1B, 0x11, 0x11, 0x1F, 0x00, 0x00, // 0x7F
#ifndef FONT_END7F
        0x0E, 0x11, 0x10, 0x10, 0x11, 0x0E, 0x04, 0x0C, // 0x80
        0x12, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0x81
        0x03, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x82
        0x0E, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x83
        0x0A, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x84
        0x0C, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x85
        0x0E, 0x0A, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0x86
        0x00, 0x0E, 0x11, 0x10, 0x11, 0x0E, 0x04, 0x0C, // 0x87
        0x0E, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x88
        0x0A, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x89
        0x0C, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E, 0x00, // 0x8A
        0x0A, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x8B
        0x0E, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x8C
        0x08, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0x8D
        0x0A, 0x00, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0x8E
        0x0E, 0x0A, 0x0E, 0x1B, 0x11, 0x1F, 0x11, 0x00, // 0x8F
        0x03, 0x00, 0x1F, 0x10, 0x1E, 0x10, 0x1F, 0x00, // 0x90
        0x00, 0x00, 0x1E, 0x05, 0x1F, 0x14, 0x0F, 0x00, // 0x91
        0x0F, 0x14, 0x14, 0x1F, 0x14, 0x14, 0x17, 0x00, // 0x92
        0x0E, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x93
        0x0A, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x94
        0x18, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x95
        0x0E, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0x96
        0x18, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0x97
        0x0A, 0x00, 0x12, 0x12, 0x12, 0x0E, 0x04, 0x18, // 0x98
        0x12, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x99
        0x0A, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0x9A
        0x00, 0x00, 0x01, 0x0E, 0x16, 0x1A, 0x1C, 0x20, // 0x9B
        0x06, 0x09, 0x08, 0x1E, 0x08, 0x09, 0x17, 0x00, // 0x9C
        0x0F, 0x13, 0x15, 0x15, 0x15, 0x19, 0x1E, 0x00, // 0x9D
        0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x00, 0x00, // 0x9E
        0x02, 0x05, 0x04, 0x0E, 0x04, 0x04, 0x14, 0x08, // 0x9F
        0x06, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0xA0
        0x06, 0x00, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00, // 0xA1
        0x06, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xA2
        0x06, 0x00, 0x12, 0x12, 0x12, 0x16, 0x0A, 0x00, // 0xA3
        0x0A, 0x14, 0x00, 0x1C, 0x12, 0x12, 0x12, 0x00, // 0xA4
        0x0A, 0x14, 0x00, 0x12, 0x1A, 0x16, 0x12, 0x00, // 0xA5
        0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, 0x0F, 0x00, // 0xA6
        0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, 0x1E, 0x00, // 0xA7
        0x04, 0x00, 0x04, 0x0C, 0x10, 0x11, 0x0E, 0x00, // 0xA8
        0x1E, 0x25, 0x2B, 0x2D, 0x2B, 0x21, 0x1E, 0x00, // 0xA9
        0x00, 0x00, 0x3F, 0x01, 0x01, 0x00, 0x00, 0x00, // 0xAA
        0x10, 0x12, 0x14, 0x0E, 0x11, 0x02, 0x07, 0x00, // 0xAB
        0x10, 0x12, 0x14, 0x0B, 0x15, 0x07, 0x01, 0x00, // 0xAC
        0x04, 0x00, 0x04, 0x04, 0x0E, 0x0E, 0x04, 0x00, // 0xAD
        0x00, 0x00, 0x09, 0x12, 0x09, 0x00, 0x00, 0x00, // 0xAE
        0x00, 0x00, 0x12, 0x09, 0x12, 0x00, 0x00, 0x00, // 0xAF
        0x15, 0x00, 0x2A, 0x00, 0x15, 0x00, 0x2A, 0x00, // 0xB0
        0x15, 0x2A, 0x15, 0x2A, 0x15, 0x2A, 0x15, 0x2A, // 0xB1
        0x2A, 0x3F, 0x15, 0x3F, 0x2A, 0x3F, 0x15, 0x3F, // 0xB2
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // 0xB3
        0x04, 0x04, 0x04, 0x3C, 0x04, 0x04, 0x04, 0x04, // 0xB4
        0x06, 0x00, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0xB5
        0x0E, 0x00, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0xB6
        0x0C, 0x00, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0xB7
        0x1E, 0x21, 0x2D, 0x29, 0x2D, 0x21, 0x1E, 0x00, // 0xB8
        0x14, 0x34, 0x04, 0x34, 0x14, 0x14, 0x14, 0x14, // 0xB9
        0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, // 0xBA
        0x00, 0x3C, 0x04, 0x34, 0x14, 0x14, 0x14, 0x14, // 0xBB
        0x14, 0x34, 0x04, 0x3C, 0x00, 0x00, 0x00, 0x00, // 0xBC
        0x00, 0x04, 0x0E, 0x10, 0x10, 0x0E, 0x04, 0x00, // 0xBD
        0x11, 0x0A, 0x04, 0x1F, 0x04, 0x1F, 0x04, 0x00, // 0xBE
        0x00, 0x00, 0x00, 0x3C, 0x04, 0x04, 0x04, 0x04, // 0xBF
        0x04, 0x04, 0x04, 0x07, 0x00, 0x00, 0x00, 0x00, // 0xC0
        0x04, 0x04, 0x04, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xC1
        0x00, 0x00, 0x00, 0x3F, 0x04, 0x04, 0x04, 0x04, // 0xC2
        0x04, 0x04, 0x04, 0x07, 0x04, 0x04, 0x04, 0x04, // 0xC3
        0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xC4
        0x04, 0x04, 0x04, 0x3F, 0x04, 0x04, 0x04, 0x04, // 0xC5
        0x05, 0x0A, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00, // 0xC6
        0x05, 0x0A, 0x04, 0x0A, 0x11, 0x1F, 0x11, 0x00, // 0xC7
        0x14, 0x17, 0x10, 0x1F, 0x00, 0x00, 0x00, 0x00, // 0xC8
        0x00, 0x1F, 0x10, 0x17, 0x14, 0x14, 0x14, 0x14, // 0xC9
        0x14, 0x37, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xCA
        0x00, 0x3F, 0x00, 0x37, 0x14, 0x14, 0x14, 0x14, // 0xCB
        0x14, 0x17, 0x10, 0x17, 0x14, 0x14, 0x14, 0x14, // 0xCC
        0x00, 0x3F, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xCD
        0x14, 0x37, 0x00, 0x37, 0x14, 0x14, 0x14, 0x14, // 0xCE
        0x11, 0x0E, 0x11, 0x11, 0x11, 0x0E, 0x11, 0x00, // 0xCF
        0x0C, 0x10, 0x08, 0x04, 0x0E, 0x12, 0x0C, 0x00, // 0xD0
        0x0E, 0x09, 0x09, 0x1D, 0x09, 0x09, 0x0E, 0x00, // 0xD1
        0x0E, 0x00, 0x1F, 0x10, 0x1E, 0x10, 0x1F, 0x00, // 0xD2
        0x0A, 0x00, 0x1F, 0x10, 0x1E, 0x10, 0x1F, 0x00, // 0xD3
        0x0C, 0x00, 0x1F, 0x10, 0x1E, 0x10, 0x1F, 0x00, // 0xD4
        0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xD5
        0x06, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0xD6
        0x0E, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0xD7
        0x0A, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0xD8
        0x04, 0x04, 0x04, 0x3C, 0x00, 0x00, 0x00, 0x00, // 0xD9
        0x00, 0x00, 0x00, 0x07, 0x04, 0x04, 0x04, 0x04, // 0xDA
        0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, // 0xDB
        0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x3F, // 0xDC
        0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04, 0x00, // 0xDD
        0x0C, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x0E, 0x00, // 0xDE
        0x3F, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, // 0xDF
        0x06, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE0
        0x00, 0x1C, 0x12, 0x1C, 0x12, 0x12, 0x1C, 0x10, // 0xE1
        0x0E, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE2
        0x18, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE3
        0x0A, 0x14, 0x00, 0x0C, 0x12, 0x12, 0x0C, 0x00, // 0xE4
        0x0A, 0x14, 0x0C, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE5
        0x00, 0x00, 0x12, 0x12, 0x12, 0x1C, 0x10, 0x10, // 0xE6
        0x00, 0x18, 0x10, 0x1C, 0x12, 0x1C, 0x10, 0x18, // 0xE7
        0x18, 0x10, 0x1C, 0x12, 0x12, 0x1C, 0x10, 0x18, // 0xE8
        0x06, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xE9
        0x0E, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xEA
        0x18, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, // 0xEB
        0x06, 0x00, 0x12, 0x12, 0x12, 0x0E, 0x04, 0x18, // 0xEC
        0x06, 0x00, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x00, // 0xED
        0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xEE
        0x0C, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xEF
        0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, // 0xF0
        0x00, 0x04, 0x0E, 0x04, 0x00, 0x0E, 0x00, 0x00, // 0xF1
        0x00, 0x00, 0x1F, 0x00, 0x00, 0x1F, 0x00, 0x00, // 0xF2
        0x30, 0x1A, 0x34, 0x0B, 0x15, 0x07, 0x01, 0x00, // 0xF3
        0x0F, 0x15, 0x15, 0x0D, 0x05, 0x05, 0x05, 0x00, // 0xF4
        0x0E, 0x11, 0x0C, 0x0A, 0x06, 0x11, 0x0E, 0x00, // 0xF5
        0x00, 0x04, 0x00, 0x1F, 0x00, 0x04, 0x00, 0x00, // 0xF6
        0x00, 0x00, 0x00, 0x0E, 0x06, 0x00, 0x00, 0x00, // 0xF7
        0x0C, 0x12, 0x12, 0x0C, 0x00, 0x00, 0x00, 0x00, // 0xF8
        0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, // 0xF9
        0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, // 0xFA
        0x08, 0x18, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, // 0xFB
        0x1C, 0x08, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, // 0xFC
        0x18, 0x04, 0x08, 0x1C, 0x00, 0x00, 0x00, 0x00, // 0xFD
        0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, // 0xFE
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 0xFF
#endif
};

#define lcd_width 128
#define lcd_height 64

static int_least16_t drawChar(int_least16_t x, int_least16_t y, char c, uint_least16_t color, uint_least16_t bg, uint_least8_t size)
{
  int_least16_t ret, pos;
#if FONT_WIDTH <= 8
  uint_least8_t data, mask;
#elif FONT_WIDTH <= 16
  uint_least16_t data, mask;
#elif FONT_WIDTH <= 32
  uint_least32_t data, mask;
#endif
  uint_least8_t i, j, width, height;
  //const PROGMEM uint8_t *ptr;
  const uint8_t *ptr;

#if FONT_WIDTH <= 8
  pos = (c - FONT_START) * (8 * FONT_HEIGHT / 8);
#elif FONT_WIDTH <= 16
  pos = (c - FONT_START) * (16 * FONT_HEIGHT / 8);
#elif FONT_WIDTH <= 32
  pos = (c - FONT_START) * (32 * FONT_HEIGHT / 8);
#endif
  ptr = &font_PGM[pos];
  width = FONT_WIDTH;
  height = FONT_HEIGHT;
  size = size & 0x7F;

  if (size <= 1)
  {
    ret = x + width;
    if ((ret - 1) >= lcd_width)
    {
      return lcd_width + 1;
    }
    else if ((y + height - 1) >= lcd_height)
    {
      return lcd_width + 1;
    }

    //setArea(x, y, (x+width-1), (y+height-1));

    //drawStart();
    int xx = 0, yy = 0;
    for (; height != 0; height--)
    {
      // #if FONT_WIDTH <= 8
      //   data = pgm_read_byte(ptr); ptr+=1;
      data = *ptr;
      ptr += 1;
      // #elif FONT_WIDTH <= 16
      //       data = pgm_read_word(ptr); ptr+=2;
      // #elif FONT_WIDTH <= 32
      //       data = pgm_read_dword(ptr); ptr+=4;
      // #endif
      for (mask = (1 << (width - 1)); mask != 0; mask >>= 1)
      {
        if (data & mask)
        {
          //draw(color);
          __DrawPixel(x + xx, y + yy, color);
        }
        else
        {
          //draw(bg);
          // not for now as real SSD1306 does
          // __DrawPixel(x + xx, y + yy, bg);
        }
        xx++;
      }
      yy++;
      xx = 0;
    }
    //drawStop();
  }
  //   else
  //   {
  //     ret = x+(width*size);
  //     if((ret-1) >= lcd_width)
  //     {
  //       return lcd_width+1;
  //     }
  //     else if((y+(height*size)-1) >= lcd_height)
  //     {
  //       return lcd_width+1;
  //     }

  //     setArea(x, y, (x+(width*size)-1), (y+(height*size)-1));

  //     drawStart();
  //     for(; height!=0; height--)
  //     {
  // #if FONT_WIDTH <= 8
  //       data = pgm_read_byte(ptr); ptr+=1;
  // #elif FONT_WIDTH <= 16
  //       data = pgm_read_word(ptr); ptr+=2;
  // #elif FONT_WIDTH <= 32
  //       data = pgm_read_dword(ptr); ptr+=4;
  // #endif
  //       for(i=size; i!=0; i--)
  //       {
  //         for(mask=(1<<(width-1)); mask!=0; mask>>=1)
  //         {
  //           if(data & mask)
  //           {
  //             for(j=size; j!=0; j--)
  //             {
  //               draw(color);
  //             }
  //           }
  //           else
  //           {
  //             for(j=size; j!=0; j--)
  //             {
  //               draw(bg);
  //             }
  //           }
  //         }
  //       }
  //     }
  //     //drawStop();
  //   }

  return ret;
}

static void DrawChar(char c, uint8_t x, uint8_t y, uint8_t brightness)
{
  drawChar(x, y, c, 1, 0, 1);
}



#endif
#endif