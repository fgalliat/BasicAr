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
405 DL = 0.3
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
510 T(X,Y) = TT
520 IF TT = 1 THEN LINE 2*X+3, 2*Y+3, 2*X+4, 2*Y+3
530 IF TT = 1 THEN LINE 2*X+3, 2*Y+4, 2*X+4, 2*Y+4
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
920 IF NOT ( O( LV(I,1), 1 ) = 0) THEN GOTO 990
930 IF NOT ( ( O( LV(I,2), 1 ) = 0) * ( O( LV(I,3), 1 ) = 0) ) THEN GOTO 980
940 LINE LG(I,1),LG(I,2),LG(I,3),LG(I,4)
950 LINE LG(I,3),LG(I,4),LG(I,5),LG(I,6)
960 LINE LG(I,5),LG(I,6),LG(I,7),LG(I,8)
970 IF I <= 6 THEN LINE LG(I,7),LG(I,8),LG(I,1),LG(I,2)
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
1080 IF DF = 0 THEN OS=60 : HU = 40
2000 LINE 113,0,113,OS : ' barre de soif


1600 ? "Hello"
1610 ? LG(1,1);" ";LG(8, 14)
1615 ? LV(1,1);" ";LV(3, 14)
1620 ? T(1,1);" ";T(3, 14)
