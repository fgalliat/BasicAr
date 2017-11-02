1 ' ERASE
10 CLS
20 ' *********  LIGHTLAND **************
30 DIM M$(10) : DIM F(10) : DIM V(10)
40 NA = 1
50 M$(NA) = "BANDIT"   : F(NA) = 6  : V(NA) = 10 : NA=NA+1
55 M$(NA) = "VOLEUR"   : F(NA) = 6  : V(NA) = 12 : NA=NA+1
60 M$(NA) = "SORCIER"  : F(NA) = 6  : V(NA) = 8  : NA=NA+1
65 M$(NA) = "GOBELIN"  : F(NA) = 8  : V(NA) = 10 : NA=NA+1
70 M$(NA) = "GREMLIN"  : F(NA) = 8  : V(NA) = 12 : NA=NA+1
75 M$(NA) = "GUERRIER" : F(NA) = 10 : V(NA) = 15 : NA=NA+1
80 M$(NA) = "FARFADET" : F(NA) = 4  : V(NA) = 6  : NA=NA+1
85 M$(NA) = "DRAGON"   : F(NA) = 11 : V(NA) = 8  : NA=NA+1
90 M$(NA) = "SERPENT"  : F(NA) = 3  : V(NA) = 5  : NA=NA+1
95 M$(NA) = "VAMPIRE"  : F(NA) = 6  : V(NA) = 12 : NA=NA+1
100 FF = 0 : VV = 0 : RE = 0 : R$ = "" : K = 0 : FL = 0
110 ' on commence ...
1300 V = 35 : F = 8
1400 NA = NA - 1
1450 ' ET = RND(10)+1
1500 ET = 1
1600 ' ********* BOUCLE PRINCIPALE DE JEU *********************************
1700 CLS
1800 PRINT "Nous sommes a l'etape ";ET;" sur les 60"
1810 ' in GW : LOCATE row,col // X07 LOCATE col,row
1900 PRINT "" : X$=STRING$(ET-1,95) : PRINT X$, "X" : LOCATE 60,2 : PRINT "_^_"
1950 LOCATE 60,3 : PRINT "[@]"
2000 IF ET < 60   THEN GOTO 2600
2100 ' ********* VICTOIRE DU JOUEUR ***************************************
2200 CLS
2300 PRINT "Vous avez acheve votre quete de la lumiere avec succes,BRAVO !!!"
2350 PLAY "ACEGAAAA"
2400 END
2500 ' ********* BOUCLE DE COMBAT *****************************************
2600 LOCATE 5,5 : PRINT "Force ===> ";F;"   Vie ===> ";V
2700 PRINT ""
2750 ' Z =VAL (RIGHT$ (TIME$ , 2)) : RANDOMIZE (Z) -- equiv. RSEED
2800 IF FL = 1 THEN GOTO 3300
2902 IF ( RND * ET  > (ET / 3)) THEN GOTO 7400
3000 PRINT "" : LOCATE 10,8 :PRINT "Voici une mauvaise rencontre ........."
3100 FL = 1
3200 RE = RND*NA +1 : FF = F(RE) : VV = V(RE)
3300 LOCATE 5,10 :PRINT M$(RE) ;"  d'une force : ";FF;"  et une vie de ";VV;"    "
3400 LOCATE 8,12 :PRINT "                                            "
3450 ' LOCATE 8,12 :INPUT "C = Combat ou F = Fuite : ",R$
3451 LOCATE 8,12 : ? "C = Combat ou F = Fuite : " : INPUT R$
3455 ' SPACE$(x) returns a string w/ x * ' ' [TODO]
3475 FOR NB = 1 TO 8: PRINT SPACE$(80-1) : NEXT NB
3480 R$ = UPPER$(R$)
3500 IF R$ = "F" THEN GOTO 6500
3550 IF R$ = "Q" THEN GOTO 9000
3600 IF R$ <> "C" THEN GOTO 3300
3700 LOCATE  14,10
3800 PRINT "Le combat s'engage , il est terrible "
4000 LOCATE 1,15
4050 PLAY "A"
4100 PRINT "Vous frappez : ";:K = (RND*F +1): PRINT "Avec une force :";K
4200 ' IF (RND (1) * 100) > 49 THEN PRINT "       BRAVO le coup a porte " ELSE PRINT "Dommage c'est rate" : GOTO 4600
4201 IF RND * 100 > 49 THEN PRINT "       BRAVO le coup a porte "
4202 IF RND * 100 <= 49 THEN  PRINT "Dommage c'est rate" : GOTO 4600
4301 IF K > F / 1.2 THEN PRINT "Tres ";
4401 IF K > F / 1.5 THEN PRINT "JOLI coup , Chevalier !!!!"
4500 VV = VV - K : IF VV <= 0 THEN GOTO 5200
4600 PRINT "" : PRINT "ATTENTION !!   Il riposte : ": K = RND (FF)+1 : PRINT "avec force ";K
4650 PLAY "C"
4700 IF RND * 100 > 49 THEN PRINT "        HAAA !!!! le coup a porte "
4701 IF RND * 100 <= 49 THEN PRINT "OUF le coup est manque " : GOTO 7900
4800 V = V - K : IF V <= 0 THEN 6000
4900 PRINT ""
5000 GOTO 2600
5100 ' ********** MORT DE L'ADVERSAIRE ************************************
5200 PRINT "" : PRINT "BRAVO , Il est mort !!!!!!"
5250 PLAY "ABC"
5300 FL = 0
5400 VA = (FF / 2)
5600 PRINT "Ce combat vous rapporte des points de vie : "; VA
5700 V = V + VA
5800 GOTO 7400
5900 ' ********** MORT DU CHEVALIER ***************************************
6000 PRINT "" : PRINT "ARRRRRGGGGGGGGGHHHHHHHHHHH..............." : ? "Je meurs !!!! "
6100 PRINT "Ce coup vous a ete fatal. Vous etes mort !!!!!!"
6200 PRINT "AMEN..  "
6250 PLAY "GFEDCBA"
6300 END
6400 ' ********** LE JOUEUR FUIT ******************************************
6500 LOCATE 5,15
6550 PRINT "" : PRINT "Ca n'est pas tres courageux .....": PRINT "Mais c'est plus prudent !"
6600 FL = 0
6700 PRINT "" : ET = ET - (RND (7) )
6800 IF ET < 1 THEN ET = 1
6900 IF (RND * 5 ) <> 3 THEN GOTO 7690
7000 PRINT "Vous vous revitalisez de 3 point de vie"
7100 V = V + 3
7200 GOTO 7690
7300 ' *********** FIN DE LA BOUCLE PRINCIPALE ****************************
7400 PRINT ""
7500 PRINT "Rien a signaler .Vous progressez sur la route a la recherche des allies  de l'ombre...."
7650 Z= (RND * 100)
7660 IF Z < 5 THEN PRINT "Vous avez trouve un elixir qui vous donne 2 point de FORCE" : F = F+1
7670 IF Z > 93 THEN PRINT "Vous avez trouve une source qui vous donne 2 point de  VIE ": V = V+1
7690 ? "Return pour suivre votre route ": INPUT R$
7700 ET = ET + 2
7800 GOTO 1700
7900 ' ******** L'ADVERSAIRE FUIT *****************************************
8000 IF (RND * 11 ) < FF  THEN GOTO 2600
8100 PRINT "Il fuit , votre courage lui fait peur !!!!!!!!"
8200 FL = 0
8300 GOTO 7690
9000 ' ******** FIN DU JEU ************************************************
9100 ? "Au revoir"
9200 END
