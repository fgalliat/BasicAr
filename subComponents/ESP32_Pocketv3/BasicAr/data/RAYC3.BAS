1 ' C
3 SCREEN 2
4 CLS : SCREEN 0 
5 DIM HA(11) : DIM MA(11) : DIM CA(11) : ' DIM L(8,8)
6 '    1234567812345678123456781234567812345678123456781234567812345678
80 L$="1111111110000001100000011000000110000001100000011000200111111111"
100 X=4*16: Y=4*16: F=45
101 BLITT 0: ' CLS 
104 I = 1 : LOCATE 0,0
105 FOR S=-160 TO 149 STEP 30: R=F+(S*0.187500)
107 RR=R/57
112 COSR=COS(RR)
113 XI=COSR: YI=SIN(RR): Xa=X: Ya=Y : cp=1
114 Xa=Xa+XI: Ya=Ya+YI: VD=1 : cp=cp+1
115 XX=(Xa/16): IF XX<1 THEN XX=1: VD=0
116 IF XX>7 THEN XX=7: VD=0
117 YY=(Ya/16): IF YY<1 THEN YY=1: VD=0
118 IF YY>7 THEN YY=7: VD=0
119 WO=VAL(MID$(L$,((INT(YY-1)*8)+INT(XX-1))+1,1))
120 IF WO=0 AND VD=1 AND cp < 7 THEN GOTO 114
122 D=ABS((X-Xa)/COSR): D=D*COS(F/57-RR)
123 D=D*10 : ' ? D
125 H=((100-D)*0.32): H=MIN(H,32): H=MAX(H,0)
126 SS=S/3
127 IF (h<5) THEN ? d; " "; xx; " "; yy ; " "; cp
132 C=2: M=1: IF D>32 THEN C=2: M=1
134 IF D>40 THEN C=3: M=1
136 IF D>50 THEN C=4: M=0
138 IF WO>1 THEN C=1: ' !
200 HA(I)=H : MA(I)=M : CA(I)=C : I=I+1
235 NEXT S: BLITT 2
270 FOR I=1 TO 11
275 H=HA(I) : C=CA(I) : M=MA(I)
280 RECT (I*10),0,10,64,0,1
290 RECT (I*10),(32-H),10,H*2,C,M
300 NEXT I
436 A$=INKEY$
441 IF A$="f" OR BTN(5)=1 THEN F=F+5: GOTO 101
442 IF A$="d" OR BTN(4)=1 THEN F=F-5: GOTO 101
451 XX=(X+COS(F/57))/16: IF XX<1 THEN XX=1
452 IF XX>7 THEN XX=7
453 YY=(Y+SIN(F/57))/16: IF YY<1 THEN YY=1
454 IF YY>7 THEN YY=7
455 ' IF A$="s" AND (L(XX,YY)=0) THEN X=X+COS(F/57): GOTO 101
490 IF A$="q" OR BTN(1)=1 THEN STOP 
495 IF A$="r" OR BTN(6)=1 THEN Y=Y+SIN(F/57)*2: X=X+COS(F/57)*2: GOTO 101
498 DELAY 10
499 GOTO 436
508 ' O
510 ' t
515 ' s

