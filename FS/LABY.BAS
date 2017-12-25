1 ' esp requires a leading blank line @ this time
10 CLS
30 I=1
35 ' Pseudo 3D render matrix
40 DIM LG(14,8) : DIM LV(14,3)
101 LG(I,1)=37 : LG(I,2)=21 : LG(I,3)=37 : LG(I,4)=44 : LG(I,5)=40 : LG(I,6)=41 : LG(I,7)=40 : LG(I,8)=24 : I=I+1
102 LG(I,1)=60 : LG(I,2)=21 : LG(I,3)=60 : LG(I,4)=44 : LG(I,5)=57 : LG(I,6)=41 : LG(I,7)=57 : LG(I,8)=24 : I=I+1
103 LG(I,1)=25 : LG(I,2)= 9 : LG(I,3)=25 : LG(I,4)=56 : LG(I,5)=36 : LG(I,6)=45 : LG(I,7)=36 : LG(I,8)=20 : I=I+1
104 LG(I,1)=72 : LG(I,2)= 9 : LG(I,3)=72 : LG(I,4)=56 : LG(I,5)=61 : LG(I,6)=45 : LG(I,7)=61 : LG(I,8)=20 : I=I+1
105 LG(I,1)=37 : LG(I,2)=21 : LG(I,3)=60 : LG(I,4)=21 : LG(I,5)=60 : LG(I,6)=44 : LG(I,7)=37 : LG(I,8)=44 : I=I+1
106 LG(I,1)=25 : LG(I,2)= 9 : LG(I,3)=72 : LG(I,4)= 9 : LG(I,5)=72 : LG(I,6)=56 : LG(I,7)=25 : LG(I,8)=56 : I=I+1
107 LG(I,1)=24 : LG(I,2)=21 : LG(I,3)=13 : LG(I,4)=21 : LG(I,5)=13 : LG(I,6)=44 : LG(I,7)=24 : LG(I,8)=44 : I=I+1
108 LG(I,1)=73 : LG(I,2)=21 : LG(I,3)=84 : LG(I,4)=21 : LG(I,5)=84 : LG(I,6)=44 : LG(I,7)=73 : LG(I,8)=44 : I=I+1
109 LG(I,1)=25 : LG(I,2)=21 : LG(I,3)=36 : LG(I,4)=21 : LG(I,5)=36 : LG(I,6)=44 : LG(I,7)=25 : LG(I,8)=44 : I=I+1
110 LG(I,1)=72 : LG(I,2)=21 : LG(I,3)=61 : LG(I,4)=21 : LG(I,5)=61 : LG(I,6)=44 : LG(I,7)=72 : LG(I,8)=44 : I=I+1
111 LG(I,1)= 1 : LG(I,2)= 9 : LG(I,3)=24 : LG(I,4)= 9 : LG(I,5)=24 : LG(I,6)=56 : LG(I,7)= 1 : LG(I,8)=56 : I=I+1
112 LG(I,1)=96 : LG(I,2)= 9 : LG(I,3)=73 : LG(I,4)= 9 : LG(I,5)=73 : LG(I,6)=56 : LG(I,7)=96 : LG(I,8)=56 : I=I+1
113 LG(I,1)=17 : LG(I,2)= 1 : LG(I,3)=24 : LG(I,4)= 8 : LG(I,5)=24 : LG(I,6)=57 : LG(I,7)=17 : LG(I,8)=64 : I=I+1
114 LG(I,1)=80 : LG(I,2)= 1 : LG(I,3)=73 : LG(I,4)= 8 : LG(I,5)=73 : LG(I,6)=57 : LG(I,7)=80 : LG(I,8)=64 : I=I+I
200 I=1
201 LV(I,1)= 7 : LV(I,2)= 5 : LV(I,3)= 8 : I=I+1
202 LV(I,1)= 9 : LV(I,2)= 5 : LV(I,3)= 8 : I=I+1
203 LV(I,1)= 4 : LV(I,2)= 5 : LV(I,3)=10 : I=I+1
204 LV(I,1)= 6 : LV(I,2)= 5 : LV(I,3)=10 : I=I+1
205 LV(I,1)= 8 : LV(I,2)= 5 : LV(I,3)=10 : I=I+1
206 LV(I,1)= 5 : LV(I,2)=10 : LV(I,3)=10 : I=I+1
207 LV(I,1)= 7 : LV(I,2)= 1 : LV(I,3)= 4 : I=I+1
208 LV(I,1)= 9 : LV(I,2)= 3 : LV(I,3)= 6 : I=I+1
209 LV(I,1)= 7 : LV(I,2)= 4 : LV(I,3)= 5 : I=I+1
210 LV(I,1)= 9 : LV(I,2)= 5 : LV(I,3)= 6 : I=I+1
211 LV(I,1)= 4 : LV(I,2)= 1 : LV(I,3)=10 : I=I+1
212 LV(I,1)= 6 : LV(I,2)= 3 : LV(I,3)=10 : I=I+1
213 LV(I,1)= 1 : LV(I,2)=10 : LV(I,3)=10 : I=I+1
214 LV(I,1)= 3 : LV(I,2)=10 : LV(I,3)=10 : I=I+1
400 ' Init the labyrinth - T(x,y) is the map
405 DL = 0.22 : ' densite labyrinth
410 DIM T(59,27)
420 FOR X = 1 TO 59
430 FOR Y = 1 TO 27
440 I = X MOD 2
450 J = Y MOD 2
460 ' dont know if conditions will work
465 ' IF ( I * (J=0)+(I=0)*J )*( RND() <= DL ) + (I=0)*(J=0)+(X<=2)+(Y<=2)+(X>=58)+(Y>=26) THEN T(X,Y) = 1
470 ZJ = 0 : IF J = 0 THEN ZJ = 1
475 ZI = 0 : IF I = 0 THEN ZI = 1
480 ZR = 0 : IF RND <= DL THEN ZR = 1
490 TT = 0
500 IF ( I * (ZJ)+(ZI)*J )*( ZR ) + (ZI)*(ZJ)+(X<=2)+(Y<=2)+(X>=58)+(Y>=26) THEN TT = 1
501 ' IF (( I * (ZJ)+(ZI)*J ) = 1) AND( RND() <= DL ) OR (I=0) AND (J=0) OR (X<=2) OR (Y<=2) OR (X>=58) OR (Y>=26) THEN TT = 1
510 T(X,Y) = TT
520 IF TT = 1 THEN LINE 2*X+3, 64-(2*Y+3), 2*X+4, 64-(2*Y+3)
530 IF TT = 1 THEN LINE 2*X+3, 64-(2*Y+4), 2*X+4, 64-(2*Y+4)
560 NEXT Y
570 NEXT X
575 A$=INKEY$ : B=BTN(0)
580 IF A$ <> "" OR B <> 0 THEN DELAY 200 : CLS : GOTO 600
585 GOTO 575
600 ' main PRGM
601 DIM D(4,2) : ' move matrix
605 D(1,1) = 0 : D(2,1) = 1 : D(3,1) = 0 : D(4,1) = -1
610 D(1,2) = 1 : D(2,2) = 0 : D(3,2) = -1 : D(4,2) = 0
620 OX = 3 : OY = 3 : OD = 1 : OS = 60 : HP = 60
650 DIM F(4,2) : F(1,1) = 3 : F(1,2) = 3 : ' 1st fountain
660 ' randomize 3 other fountains .....
800 DIM O(10,1)
810 FOR I=1 TO 3
820 FOR J=1 TO 3
830 O(I+(J*3)-3,1) = T(OX-2+I, OY-1+J)
840 NEXT J
850 NEXT I
900 ' Lbl DEBUT - boucle d'affichage
910 CLS
915 FOR I=1 TO 14
917 ' missing some code in the book
920 IF NOT ( O( LV(I,1), 1 ) <> 0) THEN GOTO 990
930 IF NOT ( ( O( LV(I,2), 1 ) = 0) AND ( O( LV(I,3), 1 ) = 0) ) THEN GOTO 980
940 LINE LG(I,1)-1,64-(LG(I,2))-1,LG(I,3)-1,64-(LG(I,4))-1
950 LINE LG(I,3)-1,64-(LG(I,4))-1,LG(I,5)-1,64-(LG(I,6))-1
960 LINE LG(I,5)-1,64-(LG(I,6))-1,LG(I,7)-1,64-(LG(I,8))-1
970 IF I <= 6 THEN LINE LG(I,7)-1, 64-(LG(I,8))-1,LG(I,1)-1,64-(LG(I,2))-1
980 ' Endif
990 ' Endif
1000 NEXT I
1100 ' calcul de l'humidite
1110 HU = 0
1120 FOR I=1 TO 14
1130 DX = OX-F(I,1)
1140 DY = OY-F(I,2)
1050 ' DF = SQRT( POW(DX,2) + POW(DY,2) ) -> distance lineaire vers le fontaine
1060 DF = 3 : ' TODO
1070 ' IF DF = 0 THEN OS=60 : HU = MAX( HU, 3*MAX(0,20-DF) )
1080 IF DF = 0 THEN OS=60
1085 HU = 40 : ' HU = MAX( HU, 3*MAX(0,20-DF) )
1090 NEXT I
2000 LINE 113,64-1,113,64-OS : ' barre de soif
2010 LINE 114,64-1,114,64-OS
2020 LINE 122,64-1,122,64-HP : ' barre de HP
2030 LINE 123,64-1,123,64-HP
2040 LINE 126,64-1,126,64-HU : ' barre de Humidite
2050 LINE 127,64-1,127,64-HU
2200 HP = HU
2300 ' Lbl SAISIE
2310 A$ = INKEY$ : B = BTN(0) : CK = 0
2320 NX = OX+D( OD, 1 )
2330 NY = OY+D( OD, 2 )
2340 VA = T(NX,NY)
2342 IF A$="" AND B=0 AND BTN(4)=0 AND BTN(5)=0 AND BTN(6)=0 THEN GOTO 2300
2345 ' UP key/btn
2350 IF NOT ( BTN(6)=1 OR A$="r" ) THEN GOTO 2380
2351 ' DELAY 200 : ' wait release
2352 IF VA <> 0 THEN CK = 0 : GOTO 2380
2353 ' IF VA <> 0 THEN CK = 0 : GOTO 6000
2560 CK = 1 : OX = NX : OY = NY : OS = OS-1
2370 IF (OS = 0) THEN CLS : ? "Vous mourrez de soif !" : STOP
2380 ' Endif
2390 ' Left key
2400 IF (BTN(4)=1 OR A$="d") THEN OD=(( OD+2 ) MOD 4) +1 : CK=1
2410 IF (BTN(5)=1 OR A$="f") THEN OD=(( OD   ) MOD 4) +1 : CK=1
2420 IF (BTN(2)=1 OR A$="e") THEN ? "disp map" : CK=1
2430 IF ( A$="q" ) THEN CLS : ? "Bye" : STOP
6000 DELAY 200 : A$="" : IF CK = 0 THEN GOTO 2300
6100 IF NOT (OD = 1) THEN GOTO 6160
6110 FOR I=1 TO 3
6120 FOR J=1 TO 3
6130 O( I+ (J*3)-3, 1 ) = T( OX-2+I, OY-1+J )
6140 NEXT J
6150 NEXT I
6160 ' Endif
6200 IF NOT (OD = 2) THEN GOTO 6260
6210 FOR I=1 TO 3
6220 FOR J=1 TO 3
6230 O( I+ (J*3)-3, 1 ) = T( OX-1+J, OY+2-I )
6240 NEXT J
6250 NEXT I
6260 ' Endif
6300 IF NOT (OD = 3) THEN GOTO 6360
6310 FOR I=1 TO 3
6320 FOR J=1 TO 3
6330 O( I+ (J*3)-3, 1 ) = T( OX+2-I, OY+1-J )
6340 NEXT J
6350 NEXT I
6360 ' Endif
6400 IF NOT (OD = 4) THEN GOTO 6460
6410 FOR I=1 TO 3
6420 FOR J=1 TO 3
6430 O( I+ (J*3)-3, 1 ) = T( OX+1-J, OY-2+I )
6440 NEXT J
6450 NEXT I
6460 ' Endif
7000 ' La sortie ?
7010 IF NOT (OX=57 AND OY=25) THEN GOTO 7050
7020 CLS
7030 ? "La sortie !"
7040 STOP
7050 ' Endif
7100 GOTO 900 : ' GOTO DEBUT
8600 ' ? "Hello"
8610 ' ? LG(1,1);" ";LG(8, 14)
8615 ' ? LV(1,1);" ";LV(3, 14)
8620 ' ? T(1,1);" ";T(3, 14)
