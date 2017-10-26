# BasicAr
Basic for Arduino (Xtase version)

EARLY STAGE of Basic adaptation for Arduino(like)...

Initialy based on Robin Edwards	work (https://github.com/robinhedwards/ArduinoBASIC)

Designed for Teensy serie (3.2 & 3.6 tested)

The software layout isn't finished & is provided as is (very dirty @ this time, I know)

See you later if you want better code ;-)

### Initial command set :
```
 AND,OR, NOT,MOD
 PRINT -----------------------> <?> shortcut
 CLS
 LIST,RUN
 GOTO,GOSUB,RETURN,
 REM, ------------------------> <'> shortcut
 STOP,CONT,PAUSE
 INPUT, INKEY$ // INKEY$ : non blocking key listening ....
 IF,THEN
 FOR,TO,STEP,NEXT
 VAL()
 INT(),RND() // int(1.5) -> rounds to 1 / rnd() -> returns between 0.00 & 1.00
 STR$() // STR$(1) => "1" (allows string concat)
 LEN(), // LEN("abc") => 3 
 RIGHT$(),MID$(),LEFT$(),
 POSITION, 
 LET,DIM,
 PIN, PINMODE,PINREAD,ANALOGRD
 SAVE,LOAD,NEW
 DIR,DELETE ---------------> TODO : FILES as DIR alias
```

### Extended command set :
```
' for REM
? for PRINT
MEM function to get free mem ammount ----------> <*> shortcut to display it
         
LOCATE ROW,COL
Better teensy IRQ managment
FS_SUPPORT for Teensy3.6
DIR/FILES (in progress)
SAVE/LOAD/LLIST

MUTE (toggle mute buzzer)
TONE 100,2
PLAY "ABC#"
PLAYT5K & PLAYT53 : to play audio files
DRAWBPP : display a 1bpp picture to builtin screen
         
BTN(x) : function get button state
LED [1..3], {0,1}
         
ECHO {0;1}
DELAY x : wait x msec
BYE : to reset the MCU
```
