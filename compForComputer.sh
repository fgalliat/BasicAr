# g++ -Wall -Wno-write-strings -DCOMPUTER -x c++ BasicAr.ino basic.cpp host.cpp host_xts.cpp computer.cpp dev_sound_pcspeaker.cpp dev_screen_VGATEXT.cpp dev_screen_RPIGFX.cpp -o basic -lcurses -lSDL2

rm basic
g++ -Wall -Wno-write-strings -DCOMPUTER -x c++ BasicAr.ino basic.cpp host.cpp host_xts.cpp -o basic -lcurses -lSDL2 2>&1
