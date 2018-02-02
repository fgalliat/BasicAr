1 ' blank line ....
5 CLS
7 OK = 0
10 IF BTN(1) = 1 THEN ? "[1] "; : OK=1
12 IF BTN(2) = 1 THEN ? "[2] "; : OK=1
14 IF BTN(3) = 1 THEN ? "[3] "; : OK=1
16 IF BTN(4) = 1 THEN ? "[Left] "; : OK=1
18 IF BTN(5) = 1 THEN ? "[Right] "; : OK=1
20 IF BTN(6) = 1 THEN ? "[Up] "; : OK=1
22 IF BTN(7) = 1 THEN ? "[Down] "; : OK=1
24 IF BTN(8) = 1 THEN ? "[T-A] "; : OK=1 : exec "mp3","play",1
26 IF BTN(9) = 1 THEN ? "[T-B] "; : OK=1 : exec "mp3","pause"
28 IF BTN(10) = 1 THEN ? "[MP3] "; : OK=1
100 IF OK = 1 THEN ? ""
200 DELAY 150
300 GOTO 7