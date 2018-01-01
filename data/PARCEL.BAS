10 ERASE
20 CLS
23 ' EXEC "WIFI","RESET"
25 ' ? "Reset OK"
26 PC$="6A06091725648"
30 EXEC "WIFI","CHECK"
40 ? "OK"
50 EXEC "WIFI","PARCEL", PC$
60 ? "OK"
65 N=RZ$(0)
68 ? N, "lines readed"
70 IF N < 4 THEN ? "ERROR" : GOTO 1000
80 CLS
100 ? "########## Colis : ", RZ$(2+1), " ##########"
110 ? ""
120 ? "Date : " , RZ$(3+1)
130 ? ""
140 ? ""
150 ? "Etat : " , RZ$(4+1)
160 ? ""
170 ? "Donnees provenant du cache ? (", RZ$(1+1) ,")"
1000 ? "Bye"
