g++ -Wall -Wno-write-strings -DCOMPUTER -x c++ BasicAr.ino basic.cpp host.cpp host_xts.cpp computer.cpp dev_sound_pcspeaker.cpp -o basic -lcurses -lSDL2