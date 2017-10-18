5 ' semicolon mandatory ?
10 PRINT "coucou"
20 A=1
30 B=A-1
35 LED A,1 : LED A-1,0
36 PRINT "-";A;"-"
40 IF BTN(0) <> 0 THEN GOTO 70
50 ' A=A+1 : IF A > 3 THEN A = 0
55 A = A+1
57 IF A > 3 THEN A = 0
58 ' why need goto ..
59 ' DELAY 300 : GOTO 60
60 GOTO 30
70 PRINT "Bye"

