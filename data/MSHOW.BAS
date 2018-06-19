10 CLS
20 ' *********  Media SHOW ************
30 DIM I$(10) : DIM M$(10) : DIM T(10)
40 NA = 1
45 ' T(x) = 50 -> T5K // 53 -> T53
50 I$(NA) = "METI"   : M$(NA) = "MARIO"  : T(NA) = 53 : NA=NA+1
55 I$(NA) = "LAMBO"  : M$(NA) = "DUEL"   : T(NA) = 53 : NA=NA+1
60 I$(NA) = "LOTUS"  : M$(NA) = "LOTUS3" : T(NA) = 53 : NA=NA+1
70 I$(NA) = "BLAST"  : M$(NA) = "XENON"  : T(NA) = 50 : NA=NA+1
80 I$(NA) = "CC3"    : M$(NA) = "CC3"    : T(NA) = 53 : NA=NA+1
100 NA = NA-1
120 C = 1
150 DRAWBPP I$(C)
152 IF T(C) = 50 THEN PLAYT5K M$(C)
154 IF T(C) = 53 THEN PLAYT53 M$(C)
160 B = BTN(0)
170 IF B = 1 THEN C = C-1 : IF C < 1 THEN C = NA
180 IF B = 4 THEN C = C+1 : IF C > NA THEN C = 1
190 IF B = 2 THEN GOTO 1000
200 IF B <> 0 THEN DELAY 150 : GOTO 150
210 GOTO 160
1000 ' ** end **
1010 CLS
