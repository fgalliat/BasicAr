1 ' blank line for esp32
2 ' original code found @ ti-calc.org :: racing for TI-85
3 ' BEWARE : LOCATE col,row is 0-based
4 ' @ this time : only REAR VIEW is fixed ! - Lbl D
5 CLS
10 A=8 : B=8 : C=8 : D=8 : E=8 : F=8 : G=8 : H=8
20 I=11 : L=0 : M=0 : N=5 : O = 5
30 SPD = 3 : ISP = (10 - SPD) * 45
35 HP = 3
40 ? "Race Game"
50 ? "v0.1"
60 ? "" : GOSUB 3000
80 ' default : rear view 
90 GOTO 500 : ' Lbl D - rear view
95 ' GOTO 100 : ' Lbl B - over view
97 ' GOTO 900 : ' Lbl F - hyper view
100 ' Lbl B : view = over
110 CLS
120 ' Lbl C
130 L=L+1
170 LOCATE A,1 : ? " o   o "
180 LOCATE B,2 : ? " o   o "
190 LOCATE C,3 : ? " o   o "
200 LOCATE D,4 : ? " o   o "
205 LOCATE I,4 : ? "^"
210 LOCATE E,5 : ? " o   o "
220 LOCATE F,6 : ? " o   o "
230 LOCATE G,7 : ? " o   o "
240 LOCATE H,8 : ? " o   o ";
300 IF I<=D+1 OR I >=D+5 THEN GOTO 2000
310 H=G : G=F : F=E : E=D : D=C : C=B : B=A
320 J=INT( 3 * RND -1 )
330 IF A+J < 1 OR A+J > 15 THEN J = 0
340 A = A+J : DELAY ISP
400 A$=INKEY$ : BT = BTN(0)
410 IF A$="m" THEN GOTO 500 : ' goto D
420 IF A$="" THEN GOTO 120 : ' goto C
430 IF A$="d" THEN I=I-1 : ' left
440 IF A$="f" THEN I=I+1 : ' right
450 IF A$="p" THEN GOSUB 3000 : 'PAUSE
460 GOTO 120 : ' goto C
500 ' Lbl D -- mode REAR
510 BLITT 0 : CLS
515 '               123456789012345678901
520 LOCATE 0,2 : ? "---------------------";
525 LOCATE 0,1 : ? "HP:";HP
527 LOCATE 0,0 : ? "     Racing Game"
530 ' Lbl E
540 L=L+1
550 IF NOT ( A=1 OR A=14 ) THEN GOTO 630
560 IF A=1 THEN N=N+1
570 IF A=14 THEN N=N-1
580 IF NOT (N<1 OR N>9) THEN GOTO 620
590 ' LOCATE 0,2 : ? "                     ";
600 IF N<1 THEN N=9
610 IF N>9 THEN N=1
620 ' Endif
630 ' EndIf
650 LOCATE N+1-1,1   : ? " @        @ ";
660 LOCATE A+3-1,3 : ? " o ";
662 LOCATE B+2-1,4 : ? " o o ";
664 LOCATE B+1-1,5 : ? " o   o ";
666 LOCATE C+1-2,6 : ? " O     O ";
668 LOCATE I+1-1,6 : ? "^";
669 LOCATE D-2,7   : ? " O       O ";
670 LOCATE I-1,7   : ? "#_#";
675 BLITT 2
680 ' GOSUB 3000 : ' PAUSE
699 ' growed roadWidth 5 -> 7
700 IF I<=D+1 OR I>=D+7 THEN GOSUB 4000 : ' CRASH
710 H=G : G=F : F=E : E=D : D=C : C=B : B=A
720 J = INT( 3 * RND -1 )
730 IF A+J < 1 OR A+J > 14 THEN J=0
740 A=A+J : DELAY ISP
750 A$=INKEY$ : BT=BTN(0)
800 IF A$="m" THEN GOTO 100 : ' Lbl B - chg view
810 ' IF A$="" THEN GOTO 530 : ' Lbl E
815 IF A$="" AND BTN(4)=0 AND BTN(5)=0 THEN GOTO 500 : ' Lbl D
820 IF A$="d" OR BTN(4)=1 THEN I=I-1 : ' left
830 IF A$="f" OR BTN(5)=1 THEN I=I+1 : ' right
840 IF A$="p" OR BTN(1)=1 THEN GOSUB 3000 : ' PAUSE
845 IF A$="q" OR BTN(2)=1 THEN GOSUB 2000 : ' EXIT
850 ' GOTO 530 : ' Lbl E
855 GOTO 500 : ' Lbl D
900 ' Lbl F - mode Hyper
910 CLS
920 ? "HYPER MODE ENABLED","VIEW CHANGE DISABLED","PRESS ENTER"
930 GOSUB 3000 : ' PAUSE
940 ' Lbl G
950 ? "","","","","","","","",""
960 ' Lbl H
970 L=L+1
980 ? ""
990 LOCATE A,8 : ? " o   o "
1000 LOCATE I,4 : ? " "
1010 LOCATE I,5 : ? "v"
1020 IF I<=D+1 OR I>=D+5 THEN GOTO 2000
1030 D=C : C=B : B=A
1040 J = INT( 3 * RND -1 )
1050 IF A+J<1 OR A+J>15 THEN J=0
1060 A=A+J : DELAY ISP
1070 A$=INKEY$
1080 IF A$="" THEN GOTO 960 : ' Lbl H
1900 IF A$="d" THEN I=I-1 : ' left
1910 IF A$="f" THEN I=I+1 : ' right
1920 IF A$="p" THEN GOSUB 3000 : ' PAUSE
1930 GOTO 960 : Lbl H
2000 ' Lbl I
2010 CLS : ? "Distance : ";L
2015 GOSUB 3000
2020 STOP
3000 ' Lbl == PAUSE ==
3010 ' CLS
3030 LOCATE 6,4 : ? "-PAUSE-"
3032 A$=INKEY$ : BT = BTN(0) : ' wait no key
3034 IF A$ <> "" AND BT <> 0 THEN GOTO 3032
3040 A$=INKEY$ : BT = BTN(0) : ' wait a key
3050 DELAY 100
3060 IF A$ = "" AND BT = 0 THEN GOTO 3040
3070 A$=INKEY$ : BT = BTN(0) : ' wait no key
3080 IF A$ <> "" AND BT <> 0 THEN GOTO 3070
3090 RETURN
4000 ' Lbl CRASH
4010 BEEP 100,2
4020 HP = HP-1
4030 IF HP < 0 THEN GOTO 2000
4040 RETURN