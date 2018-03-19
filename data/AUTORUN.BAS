5 ' @ this time : esp requires a leading blank line
6 STOP
7 DRAWBPP "BLAST"
10 B=BTN(0) : K$ = INKEY$
20 IF B <> 0 THEN DELAY 300 : GOTO 100
25 IF K$ <> "" THEN STOP
30 DELAY 20
40 GOTO 10
100 CHAIN "SYSMENU"

