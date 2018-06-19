NEW
5 ' EXEC "LCD"
10 ERASE
20 CLS
30  ' === Init ===
40 BW = 7 : BH = 7
50 DIM BD$( BW * BH ) 
100 ' ======== Board drawing ====== 
101 ' .12345678901234567890
105 LOCATE 0,0
110 ? "|.......| GTrus    "
120 ? "|.......|     "
130 ? "|.......| [@][*][?]"
140 ? "|.......|     "
150 ? "|.......|     "
160 ? "|.......|     "
170 ? "|.......|     "
180 ? " -------      "


400 ' getCell(x,y) => BD$( (Y*BW) + X )

500 ' GM=1 -> got Chunk -> waits for turning it
505 ' GM=2 -> chunk rotation done -> waits for placing on board
510 ' GM=3 -> chunk on board -> starts scanning
515 ' GM=4 -> scan done, check if GAME-OVER -> goto GM=1

-------------------------------------

we 
  -> cf lundi midi -> 13:30
    -> SMS caro & fanfan 
    -> chez Franck -- on fait les courses
          \__ une idée pour aller avec le foie-gras ???
               --> partant pour tout

    -> on fera les courses car il n'aura pas le temps
       en rentrant....