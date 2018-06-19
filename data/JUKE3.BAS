1 ' Xts-uPocket V2 MP3 Player
2 ' MP trackNum / MPUS nb of track
3 RETM=1 : ' return to SYSMENU @ end
4 SCREEN 1 : XO=3 : YO=4
5 MPUS = 0
10 DATAF "juke","MPUS","MPNUM$","MPNAM$"
20 cls : drawpct "mp3_gui",0,0 : MP = 1 : VOL = 20 : PAU = 1 : PLY = 0 : PLF = 0
30 GOSUB 800  : ' display track num
90 MSG$ = "...Xtase is proud to present you XtsMP3Player..." : I = 1
95 BCPT = 1 : ' Btn Counter
100 ' === main display loop ===
130 GOSUB 1000 : ' display main baneer
135 BT = BTN(0)
140 IF BT = 4 THEN GOTO 2000 : ' 3rd btn
145 IF BT = 1 THEN GOSUB 500 : ' 1st btn
147 ' 4 left / 5 right / 6 up / 7 down
150 IF (BTN(5) = 1 AND MP < MPUS) THEN MP = MP + 1 : GOSUB 700 : DELAY 150
160 IF (BTN(4) = 1 AND MP > 1)    THEN MP = MP - 1 : GOSUB 700 : DELAY 150
170 IF (BTN(6) = 1 AND VOL < 40)  THEN VOL = VOL + 5 : GOSUB 600 : DELAY 150
180 IF (BTN(7) = 1 AND VOL > 0)   THEN VOL = VOL - 5 : GOSUB 600 : DELAY 150
190 PLF=1 : IF BCPT >= 5 THEN BCPT=1 : PLF = BTN(10) : ' is mp3 chip playing ?
195 BCPT = BCPT+1 : ' to slowdown mp3finished reading
198 ' Auto play next song when prev. finished
200 IF PAU = 0 AND PLF = 0 AND MP < MPUS THEN MP = MP + 1 : GOSUB 700 : GOSUB 640
250 DELAY 90
260 GOTO 100
500 ' main btn behavior (sub)
510 IF PAU = 1 THEN PAU = 0 : GOSUB 640 : GOTO 530
520 IF PAU = 0 THEN PAU = 1 : GOSUB 680
530 DELAY 150
590 RETURN
600 ' SET VOLUME (sub)
610 EXEC "MP3","VOL", VOL
620 RETURN
640 ' PLAY (sub)
650 EXEC "MP3","PLAY", MP
655 PLY = 1
660 RETURN
680 ' PAUSE (sub)
690 EXEC "MP3","PAUSE"
692 PLY = 0
695 RETURN
700 ' CHANGE TRACK NUM  (sub)
710 GOSUB 800  : ' display track num
720 RETURN
800 ' DISPLAY TRACK NUM (sub)
805 ' RECT 0,0,14,8,0,1
810 IF MPUS > 0 THEN GOTO 870 : ' else file not loaded
820 locate 0+XO,0+YO : ' legacy trackNum disp
830 IF MP < 10 THEN ? "0"; : locate 1+XO,0+YO
840 ? MP;
850 RETURN
870 locate 0+XO-1,0+YO : ? MPNUM$(MP); : ' valued trackNum
880 MSG$ = MPNAM$( MP ) + "  " : I = 1 : ' set baneer as trkName
890 RETURN
1000 ' SCROLL TEXT TO LEFT (sub)
1020 ' I = 1
1022 L = LEN(MSG$)
1025 ' C = MIN(I+19, L) - I : IF C <= 0 THEN I=1 : GOTO 10
1030 C = I+19
1040 IF C > L THEN C=L+1
1045 C = C - I
1047 IF C <= 0 THEN I = 1 : GOTO 1030
1048 BLITT 0
1050 ' RECT 0,64-8,128,8, 0, 1
1055 LOCATE 0+XO,7+YO
1060 ? MID$( MSG$, I, C-1 );
1065 BLITT 2
1070 I = I + 1
1080 RETURN
2000 ' Quit Player
2010 IF RETM<>0 THEN CHAIN "SYSMENU"
2020 STOP