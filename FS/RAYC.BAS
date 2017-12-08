1 DIM L(7, 7): FOR Y = 0 TO 7: FOR X = 0 TO 7: READ L(X, Y): NEXT X: NEXT Y: X = 24: Y = 24: F = 45: SCREEN 1
2 FOR S = -160 TO 149 STEP 10: R = F + (S * .1875): R = R + 360 * ((R > 360) - (R < 0)): XI = COS(R / 57): YI = SIN(R / 57): X1 = X: Y1 = Y
3 X1 = X1 + XI: Y1 = Y1 + YI: IF L(X1 / 16, Y1 / 16) = 0 THEN GOTO 3
4 D = ABS((X - X1) / COS(R / 57)): H = (1816 / D): LINE (S + 160, 100 - H)-(S + 169, 100 + H), 1, BF: LINE (S + 160, 0)-(S + 169, 99 - H), 0, BF: LINE (S + 160, 101 + H)-(S + 169, 320), 0, BF
5 NEXT S: A$ = INKEY$: F = F + 5 * (A$ = ",") - 5 * (A$ = "."): F = F - 355 * (F = -5) + 360 * (F = 365): IF A$ = " " AND (L((X + COS(F / 57)) / 16, (Y + SIN(F / 57)) / 16) = 0) THEN X = X + COS(F / 57): Y = Y + SIN(F / 57)
6 GOTO 2: DATA 1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,0,1,0,1,0,1,1,0,0,0,0,1,0,1,1,0,1,1,0,1,0,1,1,0,1,1,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1
7 ' 6 lines raycaster in GW BASIC !!!!!
8 ' found @ http://forum.qbasicnews.com/index.php?topic=5699.0
9 ' needs ABS() COS() SIN()