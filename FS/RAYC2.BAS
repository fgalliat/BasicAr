1 ' CONSOLE 3,1,3 
2 ' Raycaster Demo - Xts-uBasic version
4 CLS
5 DIM L(8, 8)
10 L(1,1) = 1: L(2,1) = 1: L(3,1) = 1: L(4,1) = 1: L(5,1) = 1: L(6,1) = 1 : L(7,1) = 1
20 L(1,2) = 1: L(2,2) = 0: L(3,2) = 0: L(4,2) = 0: L(5,2) = 0: L(6,2) = 0 : L(7,2) = 1
30 L(1,3) = 1: L(2,3) = 0: L(3,3) = 0: L(4,3) = 0: L(5,3) = 0: L(6,3) = 0 : L(7,3) = 1
40 L(1,4) = 1: L(2,4) = 0: L(3,4) = 0: L(4,4) = 0: L(5,4) = 0: L(6,4) = 0 : L(7,4) = 1
50 L(1,5) = 1: L(2,5) = 0: L(3,5) = 0: L(4,5) = 0: L(5,5) = 0: L(6,5) = 0 : L(7,5) = 1
60 L(1,6) = 1: L(2,6) = 0: L(3,6) = 0: L(4,6) = 0: L(5,6) = 0: L(6,6) = 0 : L(7,6) = 1
70 L(1,7) = 1: L(2,7) = 1: L(3,7) = 1: L(4,7) = 1: L(5,7) = 1: L(6,7) = 1 : L(7,7) = 1
100 X = 4*16: Y = 4*16: F = 45
101 CLS
105 FOR S = -160 TO 149 STEP 30: R = F + (S * .1875)
106 IF R > 360 THEN R = 0
107 IF R < 0 THEN R = 359
112 ' R = R + 360 * (LF - RF)
113 XI = COS(R / 57): YI = SIN(R / 57): Xa = X: Ya = Y
114 Xa = Xa + XI: Ya = Ya + YI
115 XX = (Xa / 16) : IF XX < 1 THEN XX = 1
116 IF XX > 7 THEN XX = 7
117 YY = (Ya / 16) : IF YY < 1 THEN YY = 1
118 IF YY > 7 THEN YY = 7
124 IF L( XX, YY ) = 0 THEN GOTO 114
125 D = ABS((X - Xa) / COS(R / 57)): H = (1816 / D)
126 ' LINE ( (S + 160)/2 , (100 - H)/2 )-( (S + 169)/2, (100 + H)/2 ), 1
127 LINE ((S + 160)/3) , ((100 - H)/3), ((S + (169))/3), ((100 + H)/3)
128 ' LINE (S + 160, 100 - H)-(S + 169, 100 + H), 1, BF
129 ' LINE (S + 160, 0)-(S + 169, 99 - H), 0, BF 
130 ' LINE (S + 160, 101 + H)-(S + 169, 320), 0, BF
135 NEXT S
136 A$ = INKEY$
141 IF A$ = "d" THEN F = F+5 : GOTO 101
142 IF A$ = "f" THEN F = F-5 : GOTO 101
143 IF F < 0 THEN F = 355
144 IF F > 355 THEN F = 0
151 XX = (X + COS(F / 57)) / 16 : IF XX < 1 THEN XX = 1
152 IF XX > 7 THEN XX = 7
153 YY = (Y + SIN(F / 57)) / 16 : IF YY < 1 THEN YY = 1
154 IF YY > 7 THEN YY = 7
155 IF A$ = "s" AND (L(XX, YY) = 0) THEN X = X + COS(F / 57) : GOTO 101
190 IF A$ = "q" then STOP
195 IF A$ = "r" THEN Y = Y + SIN(F / 57) : GOTO 101
198 DELAY 10
199 GOTO 136
208 ' Original code found @ http://forum.qbasicnews.com/index.php?topic=5699.0
