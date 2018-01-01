5 ' GTrus porting to basica
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

