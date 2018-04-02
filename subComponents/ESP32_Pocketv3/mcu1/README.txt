Xtase - fgalliat
@ Mar 2018

Xts-uPocket V3

MCU#1

handles :
 - BASIC interpreter                         (SOFTW)
 - I2C expanded GPIO (buttons & leds) (DONE) (21-22)
 - PWM buzzer                                (27)
 - UART (? USB HOST HID keyboard ?)          (16-4)
 - UART link to MCU#2                 (DONE) (26-14)

refacto : (make a new branch : /refacto for BasicAr)

  MCU.led(x) : void \__ GPIO Object (/!\ btnAddr+8 now)
     .btn(x) : bool /               (/!\ BEWARE : MP3 trigger isn't a PULLUP /!\)

     .tone(x,x) : void   \__ Buzzer Object
     .noTone(x,x) : void /

     .reset() : void

     .setup() : void :: setupGPIO() - setupSerials() - setupScreen() - setupMP3() - setupFS() 
                          \__ cf leds blink msgs        {} on #1        {} on #1
                                 + buzzer

     .getSystemMenuReqState()  : bool
     .getSystemResetReqState() : bool

     .getFs()
     
     .getGPIO()
     .getScreen()    :: on #1 is a bridge to #2 cmd sending // on #2 is direct SPI driving

     .getMp3Player() :: on #1 is a bridge to #2 cmd sending // on #2 is direct UART driving
     .getBuzzer() => can return NULL on #2



to buy:
  - DFPlayer MINI
  - SX1509
  - DOIT devkit v1 (ESP32)
  (- battery + charging kit ?)