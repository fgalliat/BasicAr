5 CLS
7 '  123456789012345678901
10 ? "  -= System Menu =-"
20 ? ""
30 ? "1. Set console"
40 ? "2. Set Buzzer"
50 ? "3. Send/Recv File"
60 ? "4. Set WIFI"
90 ? "9. Reboot"
95 ? "0. Exit";
100 INPUT CH$
110 IF CH$ = "0" THEN GOTO 9000
120 IF CH$ = "9" THEN BYE
130 IF CH$ = "4" THEN GOSUB 1000
140 IF CH$ = "3" THEN GOSUB 2000
150 IF CH$ = "2" THEN GOSUB 3000
160 IF CH$ = "1" THEN GOSUB 4000
600 GOTO 5
1000 CLS : ? "   -= WIFI Menu =-" : INPUT CH$ : RETURN
2000 CLS : ? "   -= Send/Recv =- " : INPUT CH$ : RETURN
3000 CLS : ? "   -= BUZZER Menu =-" : INPUT CH$ : RETURN
4000 CLS : ? "  -= Console Menu =-" : INPUT CH$ : RETURN
9000 CLS : ? "Bye..."
