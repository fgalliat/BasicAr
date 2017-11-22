5 ' mixing array types fails !!!!!
10 DIM a$(5) : DIM b(5)
20 FOR i=1 TO 5
30 a$(i)="Hello "+STR$(i)
35 b(i) = 3
40 PRINT ">";a$(i);"< ";b(i)
50 NEXT i
