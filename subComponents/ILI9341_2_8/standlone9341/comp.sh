export runnable=ilimain
rm $runnable
# -lcurses -lSDL2 2>&1
g++ -Wall -Wno-write-strings -DCOMPUTER -x c++ standlone9341.ino ili9341.cpp ili9341gfx.cpp -o $runnable 2>&1