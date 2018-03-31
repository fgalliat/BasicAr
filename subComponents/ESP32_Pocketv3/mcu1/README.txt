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

refacto :
  MCU.led(x) : void \__ GPIO Object
     .btn(x) : bool /
     .tone(x,x) : void   \__ Buzzer Object
     .noTone(x,x) : void /
     .reset() : void
     .setup() : void :: setupGPIO() - setupSerials() - setupScreen() - setupMP3() - setupFS() 
                          \__ cf leds blink msgs        {} on #1        {} on #1
                                 + buzzer

     .getSystemMenuReqState()  : bool
     .getSystemResetReqState() : bool

     .getFs()
     .getScreen()

