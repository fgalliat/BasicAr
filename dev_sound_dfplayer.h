/************
 * DFPlayer mp3 module - Sound Layer for Xts-uBasic 
 * Xtase - fgalliat @ Nov. 2017
 * 
 * requires : DFRobotDFPlayerMini-1.0.1 library
 ************/

#ifndef _dev_sound_dfplayer_h_
#define _dev_sound_dfplayer_h_1

  #include "DFRobotDFPlayerMini.h"
  // #include "host.h"

  void setupSoundDFPlayer();

  // from 0 to 30
  void snd_setVolume(int vol);

  // from 1 to 2999
  void snd_playTrack(int trackNum);

  void snd_next();

  void snd_pause();


#endif
