1 ' blank line for esp32
2 ' original code found @ ti-calc.org :: racing for TI-85
5 CLS
10 A=8 : B=8 : C=8 : D=8 : E=8 : F=8 : G=8 : H=8
20 I=11 : L=0 : M=0 : N=5 : O = 5
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
240 LOCATE H,8 : ? " o   o "
300 IF I<=D+1 OR I >=D+5 THEN GOTO 2000
310 H=G : G=F : F=E : E=D : D=C : C=B : B=A
320 J=INT( 3 * RND -1 )
330 IF A+J < 1 OR A+J > 15 THEN J = 0
340 A = A+J
400 A$=INKEY$ : BT = BTN(0)
410 IF A$="m" THEN GOTO 500 : ' goto D
420 IF A$="" THEN GOTO 120 : ' goto C
430 IF A$="d" THEN I=I-1 : ' left
440 IF A$="f" THEN I=I+1 : ' right
450 IF A$="p" THEN GOSUB 3000 : 'PAUSE
460 GOTO 120 : ' goto C
500 ' Lbl D -- mode REAR
510 CLS
520 LOCATE 1,4 : ? "---------------------"
530 ' Lbl E
540 L=L+1
550 IF NOT ( A=1 OR A=14 ) THEN GOTO 630
560 IF A=1 THEN N=N+1
570 IF A=14 THEN N=N-1
580 IF NOT (N<1 OR N>9) THEN GOTO 620
590 LOCATE 1,3 : ? "                     "
600 IF N<1 THEN N=9
610 IF N>9 THEN N=1
620 ' Endif
630 ' EndIf
650 LOCATE N,3   : ? " @        @ "
660 LOCATE A+3,5 : ? " o "
660 LOCATE B+2,6 : ? " o o "
660 LOCATE C+1,7 : ? " O    O "
660 LOCATE I+1,7 : ? "^"
660 LOCATE D,8   : ? " O     O "
660 LOCATE I,8   : ? "#_#"
700 I I<=D+1 OR I>=D+5 THEN GOTO 2000
710 H=G : G=F : F=E : E=D : D=C : C=B : B=A
720 J = INT( 3 * RND -1 )
730 IF A+J < 1 OR A+J > 14 THEN J=0
740 A=A+J
750 A$=INKEY$ : BT=BTN(0)
800 IF A$="m" THEN GOTO 100 : ' Lbl B
810 IF A$="" THEN GOTO 530 : ' Lbl E
820 IF A$="d" THEN I=I-1 : ' left
830 IF A$="f" THEN I=I+1 : ' right
840 IF A$="p" THEN GOSUB 3000 : ' PAUSE
850 GOTO 530 : ' Lbl E
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
1060 A=A+J
1070 A$=INKEY$
1080 IF A$="" THEN GOTO 960 : ' Lbl H
1900 IF A$="d" THEN I=I-1 : ' left
1910 IF A$="f" THEN I=I+1 : ' right
1920 IF A$="p" THEN GOSUB 3000 : ' PAUSE
1930 GOTO 960 : Lbl H
2000 ' Lbl I
2010 CLS : ? "Distance : ";L
2020 STOP
3000 ' Lbl == PAUSE ==
3010 A$=INKEY$ : BT = BTN(0)
3020 DELAY 100
3030 IF A$ = "" AND BT = 0 THEN GOTO 3000
3040 A$=INKEY$ : BT = BTN(0)
3050 IF A$ <> "" AND BT <> 0 THEN GOTO 3040
3060 RETURN