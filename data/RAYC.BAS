1 ' CONSOLE 3,1,3 
2 ' Raycaster Demo - Xts-uBasic version
4 CLS
5 DIM L(8, 8)
10 L(1,1) = 1: L(2,1) = 1: L(3,1) = 1: L(4,1) = 1: L(5,1) = 1: L(6,1) = 1 : L(7,1) = 1
20 L(1,2) = 1: L(2,2) = 0: L(3,2) = 0: L(4,2) = 0: L(5,2) = 0: L(6,2) = 0 : L(7,2) = 1
30 L(1,3) = 1: L(2,3) = 0: L(3,3) = 0: L(4,3) = 0: L(5,3) = 2: L(6,3) = 0 : L(7,3) = 1
40 L(1,4) = 1: L(2,4) = 0: L(3,4) = 0: L(4,4) = 0: L(5,4) = 0: L(6,4) = 0 : L(7,4) = 1
50 L(1,5) = 1: L(2,5) = 0: L(3,5) = 2: L(4,5) = 0: L(5,5) = 0: L(6,5) = 0 : L(7,5) = 1
60 L(1,6) = 1: L(2,6) = 0: L(3,6) = 0: L(4,6) = 0: L(5,6) = 0: L(6,6) = 0 : L(7,6) = 1
70 L(1,7) = 1: L(2,7) = 1: L(3,7) = 1: L(4,7) = 1: L(5,7) = 1: L(6,7) = 1 : L(7,7) = 1
100 X = 4*16: Y = 4*16: F = 45
101 BLITT 0 : CLS
105 FOR S = -160 TO 149 STEP 30: R = F + (S * .1875)
107 RR = R / 57
112 COSR = COS(RR)
113 XI = COSR: YI = SIN(RR): Xa = X: Ya = Y
114 Xa = Xa + XI: Ya = Ya + YI : VD = 1
115 XX = (Xa / 16) : IF XX < 1 THEN XX = 1 : VD=0
116 IF XX > 7 THEN XX = 7 : VD=0
117 YY = (Ya / 16) : IF YY < 1 THEN YY = 1 : VD=0
118 IF YY > 7 THEN YY = 7 : VD=0
119 WO = L( XX, YY ) : ' wall object
120 IF WO = 0 AND VD = 1 THEN GOTO 114
121 ' Dist comp + fishbowl eye fix
122 D = ABS((X - Xa) / COSR): D = D * COS( F/57 - RR )
125 H = (1816 / D) / 3
126 SS = S / 3
127 ' LINE (SS + 75) , (32 - H), (SS + 75), (32 + H)
128 C = 2 : M = 1 : IF D > 32 THEN C = 3 : M = 1
130 IF D > 50 THEN C = 4 : M = 1
131 IF D > 60 THEN C = 1 : M = 0
132 IF WO > 1 THEN C = 1 : ' !border
134 RECT (SS + 64) , (32 - H), 10, H*2, C, M
135 NEXT S : BLITT 2
136 A$ = INKEY$
141 IF A$ = "f" OR BTN(5) = 1 THEN F = F+5 : GOTO 101
142 IF A$ = "d" OR BTN(4) = 1 THEN F = F-5 : GOTO 101
151 XX = (X + COS(F / 57)) / 16 : IF XX < 1 THEN XX = 1
152 IF XX > 7 THEN XX = 7
153 YY = (Y + SIN(F / 57)) / 16 : IF YY < 1 THEN YY = 1
154 IF YY > 7 THEN YY = 7
155 IF A$ = "s" AND (L(XX, YY) = 0) THEN X = X + COS(F / 57) : GOTO 101
190 IF A$ = "q" OR BTN(1) = 1 then STOP
195 IF A$ = "r" OR BTN(6) = 1 THEN Y = Y + SIN(F / 57) : X = X + COS(F / 57) : GOTO 101
198 DELAY 10
199 GOTO 136
208 ' Original code found @ http://forum.qbasicnews.com/index.php?topic=5699.0
210 ' tutos on http://lodev.org/cgtutor/raycasting.html
215 ' see also http://www.permadi.com/tutorial/raycast/rayc8.html