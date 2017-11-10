10 CLS 
20 DRAWBPP "meti"
25 ? "Press Enter ..."
30 INPUT CH$
35 CLS 
40 ? "MP3 Player"
50 ? "Q - Quit"
60 ? "P - Play"
70 ? "N - Next"
80 ? "C - Pause"
90 ? "V - Volume"
100 INPUT CH$
110 CH$=UPPER$(CH$)
120 ? CH$
130 IF ch$="Q" THEN STOP 
140 IF ch$="P" THEN ? "Play...": EXEC "mp3","play",14
150 IF ch$="V" THEN ? "Volume ?": INPUT v: EXEC "mp3","vol",v
160 IF ch$="C" THEN ? "Pause": EXEC "mp3","pause"
170 IF ch$="N" THEN ? "Next ...": EXEC "mp3","next"
1000 GOTO 35
