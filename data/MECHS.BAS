1 ' Mecha GAME
2 RETM = 1
5 SCREEN 2 : CLS
10 DRAWSPRT "MECHS", (320-64)/2,20 , 64,64, 64+8,64
20 LOCATE 19,15 : ? "Mecha Game"
30 EXEC "MP3", "VOL", 10
40 EXEC "MP3", "PLAY", 1
50 IF ( BTN(0) <> 0 ) THEN GOTO 100
60 DELAY 150 : GOTO 50
99 ' == Player selection ==
100 CLS
110 FOR I = 1 TO 5-1 : ' 5th is smaller than 32px
120 DRAWSPRT "", (I-1)*(32+6)+10,20 , 32,32, (I-1)*32,0
125 DRAWSPRT "", ((I+4)-1)*(32+6)+10,20 , 32,32, (I-1)*32,32
130 NEXT I
140 Z=1 : I=1 : ' User Choice
180 IF BTN(4) =1 AND I > 1 THEN I=I-1
190 IF BTN(5) =1 AND I < 8 THEN I=I+1
195 RECT (Z-1)*(32+6)+10 -2 ,20 -2 , 32+4,32+4, 0, 0
200 Z=I : RECT (I-1)*(32+6)+10 -2 ,20 -2 , 32+4,32+4, 1, 0
250 IF ( BTN(0) <> 0 ) THEN GOTO 1000
260 DELAY 50 : IF BTN(4)=1 OR BTN(5)=1 THEN GOTO 260
300 LOCATE 4,8  : ? "Name  : Borg    ";
310 LOCATE 4,9  : ? "Class : Heavy   ";
320 LOCATE 4,10 : ? "Range : Short   ";
330 LOCATE 4,11 : ? "HP    : 50      ";
340 LOCATE 4,12 : ? "ATT   : 20      ";
350 LOCATE 4,13 : ? "DEF   : 15      ";
590 GOTO 180 : ' Choice loop
1000 GOTO 9990
9990 ' === End of PRGM ===
9992 EXEC "MP3", "STOP"
9995 IF RETM <> 0 THEN CHAIN "SYSMENU"
9999 STOP
