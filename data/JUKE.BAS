1 ' blank ....
10 cls : drawpct "mp3_gui",0,0 : MP = 1
20 locate 0,0 : 
25 IF MP < 10 THEN ? "0"; : locate 1,0
30 ? MP;
40 ' dataf "juke","sz","a$","b$"
115 MSG$ = "...Xtase is proud to present you XtsMP3Player..." : I = 1
130 GOSUB 1000
135 BT = BTN(0)
140 IF BT = 4 THEN STOP : ' 3rd btn
150 IF (BTN(5) == 1) THEN MP = MP + 1 : DELAY 150
160 IF (BTN(4) == 1 AND MP > 1) THEN MP = MP - 1 : DELAY 150
250 DELAY 90
260 GOTO 130
600 ' -------------
1000 ' SCROLL TEXT TO LEFT
1020 ' I = 1
1022 L = LEN(MSG$)
1025 ' C = MIN(I+19, L) - I : IF C <= 0 THEN I=1 : GOTO 10
1030 C = I+19
1040 IF C > L THEN C=L+1
1045 C = C - I
1047 IF C <= 0 THEN I = 1 : GOTO 1030
1048 BLITT 0
1050 RECT 0,64-8,128,8, 0, 1
1055 LOCATE 0,7
1060 ? MID$( MSG$, I, C );
1065 BLITT 2
1070 I = I + 1
1080 RETURN
