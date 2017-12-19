// *****************************************************************
// Filename:  FrameBuffer.h
// Copyright: Xtase (Franck GALLIAT). All rights reserved.
// Author:    Xtase (Franck GALLIAT)
// Date:      21.05.2014
// *****************************************************************

#ifndef _FRAMEBUFF_H_
#define _FRAMEBUFF_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/input.h>
#include <linux/kd.h>


#include <jni.h>

//! Class handling the frameBuffer.
class FrameBuffer {

  public:
    FrameBuffer();
    ~FrameBuffer();

                int  openFb(const char* fbDev);
                void closeFb();

void fillAll(unsigned int rgb[]);
void fill(unsigned int rgb[], int x, int y, int width, int height);

void blit(JNIEnv *env, jintArray rgb, int x, int y, int width, int height);
void blitPixel(int x, int y, int pixel);

void dispCharColor(int code, int x, int y, int color, int zoom);
void dispStrColor(const char* str, int x, int y, int color, int zoom, bool renderSpaces);

  private:
    //int m_fd;                                //!< file descriptor
    //struct termios m_options_old;            //!< previous options for serial port
                
    int fbfd , tty ;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize; 
    char *fbp;
    int x, y, i;
    uint8_t pixel;
    bool stripe;
    long int location;

};

#endif /* _FRAMEBUFF_H_ */