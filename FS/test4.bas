5 ' mixing array types fails !!!!!
10 DIM a$(5)
20 FOR i=1 TO 5
30 a$(i)="Hello "+STR$(i)
50 NEXT i
55 DIM b(5)
60 FOR i=1 TO 5
75 b(i) = 3
90 NEXT i
100 FOR i=1 TO 5
120 PRINT ">";a$(i);"< ";b(i)
130 NEXT i
