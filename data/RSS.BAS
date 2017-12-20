10 ERASE
20 CLS
30 EXEC "WIFI","CHECK"
40 ? "OK"
45 UL$="http://feeds2.feedburner.com/LeJournalduGeek"
50 EXEC "WIFI","RSS", UL$
60 ? "OK"
65 N=RZ$(0)
68 ? N, "lines readed"
80 IF N < 2 THEN ? "ERROR" : GOTO 1000
90 CLS
100 ? SPACE$(4) , "-= " , MID$( RZ$(1+1), 1, 64 ), " =-"
102 ? "" : ? ""
105 IF N > 14 THEN N = 14
110 FOR I=2+1 TO N STEP 1
120 IF LEFT$( RZ$(I), 1 ) = "T" THEN ? "> " , MID$( RZ$(I), 2, 75 )
130 IF LEFT$( RZ$(I), 1 ) = "t" THEN ? " o " , MID$( RZ$(I), 2, 75-1 ),CHR$(10)
900 NEXT
1000 ? "Bye"
