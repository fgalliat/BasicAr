# BasicAr (version 2)
Basic interpreter for Arduino'like MCUs (Teensy & ESP32 tested) (Xtase/fgalliat version)

**Initialy based on _Robin Edwards_ work (https://github.com/robinhedwards/ArduinoBASIC)**



Originally designed for **Teensy serie** (3.2 & 3.6 tested)

This project is been rewritten for **1 or more ESP32 MCU**'s (& various other layouts)


 https://hackaday.io/project/133023-xts-ubasic-pocket-v3<br/>
 https://hackaday.io/project/95667-xts-ubasic-pocket-v2<br/>
 https://hackaday.io/project/28625-xts-ubasic-pocket<br/> https://hackaday.io/project/19223-xts-ubasic<br/>

 http://xtase.over-blog.com also contains explainations about these projects

## Initial project builtin hardware (handmade computer) :

- Teensy 3.6 180MHz 256KB ram MCU

- Buzzer

- SSD1306 OLED 128x64 I2C Display

- HC-06 BT slave UART

- HobbyTronics USB Keyboard UART interface

- 3 LEDS

- 3 push buttons

- own-designed BUS to deal w/ peripheral cards

  

## Current project builtin hardware (handmade game-console) :

- 2x ESP32 240MHz 300KB ram MCU
- Buzzer / MP3Player
- ILI 9341 TFT 320x240 SPI Display
- (optional) HobbyTronics USB Keyboard UART interface
- some LEDS
- 3 push buttons + 4directions cross

### Keyword set :
```
AND,OR, NOT,MOD
PRINT -----------------------> ADDED <?> shortcut
CLS
LIST, LLIST ,RUN
GOTO,GOSUB,RETURN,
REM, ------------------------> ADDED <'> shortcut
STOP,CONT,PAUSE
INPUT, INKEY$ // INKEY$ : non blocking key listening ....
IF,THEN
FOR,TO,STEP,NEXT
VAL()
INT(),RND() // int(1.5) -> rounds to 1 // rnd -> returns between 0.00 & 1.00
STR$()      // STR$(1) => "1" (allows string concat)
LEN(),      // LEN("abc") => 3 
RIGHT$(),MID$(),LEFT$(),
POSITION,LOCATE <COL>,<ROW>
LET,DIM,

MEM --> function that return free ammount of RAM
SAVE,LOAD,NEW, CHAIN, DELETE
DIR, ----------------------> ADDED optional extension filter ex: DIR ".BAS"
DIRARRAY ------------------> Stores FS list in "DIR$" ARRAY VARIABLE
DIRM ----------------------> show FS content for MCU#2

MUTE (toggle mute buzzer)
BEEP 100,2        : 1-48 produces some notes, direct freq. otherwise, beeps for 2*50msec
PLAY "ABC#"       : play some notes of OCTAVE_4
PLAYT5K & PLAYT53 : to play audio (beep) files

DRAWBPP           : display a 1bpp (B/W) picture to builtin screen
DRAWPCT           : display a 64K colors picture
DRAWSPRT          : draw a sprite (portion of a .PCT image file)
CIRCLE x,y,r      : draw a circle
LINE   x,y,x2,y2  : draw a line
PSET   x,y        : switch ON a pixel
PRESET x,y        : switch OFF a pixel
RECT x,y,w,h,c,m  : c : 0,1,2,3 (black,white,gray, darkGray) // m : 0,1 (draw,fill)
TRIANGLE x,y,x,y,x,y,c,m : as same as RECT but draws a triangle

BLITT x           : 0 stop blitt / 1 trigger blitt / 2 auto blitt (default mode)
SCREEN 0/1/2	  : switch resolution (128x64, 160x128, 320x240)
(TEXT 0/1 -> change text overlay mode)
         
BTN(x)            : function get button state
LED [1..3], {0,1} : switch on/off a LED

( CONSOLE <out>,<in>,<gfx> : set I/O + GFX main devices )
( ECHO {0;1}        : enable/disable console output )

DELAY x           : wait x msec
SECS              : function that returns nb of seconds elapsed since boot
MILLIS            : function that returns nb of milli-seconds elapsed since boot

UPPER$(str)       : function that return UPPERCASE of str
STRING$(nbTimes, chr) : function that returns repeating string of nbTimes * CHR$(chr)
CHR$( chr )       : function that return a char of given ASCII code
SPACE$(nbTimes)   : function that returns repeating string of nbTimes * CHR$(32)
ASC( str )        : returns ascii code of 1st char of str
INSTR( str, toFind ) : returns index of 'toFind' in 'str' (else 0)

ABS(x)            : return absolute value of number
COS(x)            : return cos( x in radians)
SIN(x)            : return sin( x in radians)
SQRT(x)           : return square-root of x
POW(x,y)          : return x pow y
MIN(x,y)		  : guess
MAX(x,y)		  : guess ...

EXEC "<extCmd>", "[<extArg>]" : to launch an extended command
DO "? 3.14"       : ~ as eval() in JS, interprets & launch a String expr.


DATAF "file", "SZ", "A$", "B" -> feeds A$ & B arrays w/ a PSEUDO CSV file content (ex: file.BAD) of a length of SZ

(DATAU "url?", "SZ", "A$", "B" -> will do as same but with an URL)

(COPY "/TOTO.PCT" -> copy an asset to MCU#2 )

BYE               : to reset the MCU
HALT              : to halt the MCU & optional connected RPI
```

With the new ESP32 project design the BASIC interpreter is useable via Telnet on WIFI (AP & STA mode), so you can type or upload codes & resources files.



