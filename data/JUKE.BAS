1 ' blank ....
10 cls : drawpct "mp3_gui",0,0 : locate 0,0 : ? "01";
15 MSG$ = "...Xtase is proud to present you XtsMP3Player..." : I = 1
30 GOSUB 1000
35 BT = BTN(0)
40 IF BT <> 0 THEN STOP
50 DELAY 90
60 GOTO 30
100 ' -------------
1000 ' SCROLL TEXT TO LEFT
1020 ' I = 1
1022 L = LEN(MSG$)
1025 ' C = MIN(I+19, L) - I : IF C <= 0 THEN I=1 : GOTO 10
1030 C = I+19
1040 IF C > L THEN C=L+1
1045 C = C - I
1047 IF C <= 0 THEN I = 1 : GOTO 1030
1050 RECT 0,64-8,128,8, 0, 1
1055 LOCATE 0,7
1060 ? MID$( MSG$, I, C );
1070 I = I + 1
1080 RETURN
