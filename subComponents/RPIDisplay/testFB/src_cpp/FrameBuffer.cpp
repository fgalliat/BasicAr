// *****************************************************************
// Filename:  FrameBuffer.cpp
// Copyright: Xtase (Franck GALLIAT). All rights reserved.
// Author:    Xtase (Franck GALLIAT)
// Date:      21.05.2014
// *****************************************************************

#include <stdlib.h>
#include <string.h>
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


//#include "./font_plain.h"
#include "./font_bold.h"


#include "FrameBuffer.h"


//! Class handling the frameBuffer.
FrameBuffer::FrameBuffer() {
    fbfd = 0; tty = 0;
    screensize = 0;
    //*fbp = 0;
    x = 0; y = 0; i = 0;
    pixel = 0;
    stripe = true;
    location = 0;
}

FrameBuffer::~FrameBuffer() {
 // finalize object
}

int FrameBuffer::openFb(const char* fbDev) {
    /* open framebuffer */

    /*if ((getuid ()) != 0) {
        printf ("You are not root, run this as root or sudo\n");
        //exit(1);
        return -1;
    }*/

    fbfd = open(fbDev, O_RDWR);
    if (fbfd == -1) {
        //perror("Error: cannot open framebuffer device");
        printf("Error: cannot open framebuffer device");
        //exit(1);
        return -1;
    }
    printf("The framebuffer device was opened successfully.\n");

/*    tty = open("/dev/tty1", O_RDWR);
    if(ioctl(tty, KDSETMODE, KD_GRAPHICS) == -1)
  printf("Failed to set graphics mode on tty1\n");
*/

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        //exit(2);
        return -2;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        //exit(3);
        return -3;
    }
    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    //screensize = vinfo.yres_virtual * finfo.line_length;
    
    //printf("screensize: %d\n",screensize); 


    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    //if ((int)fbp == -1) {
    if ( !fbp ) {
        perror("Error: failed to map framebuffer device to memory");
        //exit(4);
        return -4;
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

return 0;
}


void FrameBuffer::fillAll(unsigned int rgb[]) {
  fill(rgb, 0, 0, vinfo.xres, vinfo.yres);
}

//  rgb[i] = 255<<11 | 0 << 5 | 0;
void FrameBuffer::fill(unsigned int rgb[], int xOffset, int yOffset, int width, int height) {

   /* blit the screen */    
   int xx = 0, yy = 0;

   for (y = yOffset; y < yOffset+height; y++) {
     xx = 0;
     for (x = xOffset; x < xOffset+width; x++) {

  location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                   (y+vinfo.yoffset) * finfo.line_length;


  // from java color
  unsigned int pixel = rgb[ (yy*width) + xx ];
    blitPixel(x,y,pixel);
       
       xx++;
    }
    yy++;
  }

}


/** directly blit from java colors */
void FrameBuffer::blit(JNIEnv *env, jintArray rgb, int xOffset, int yOffset, int width, int height) {
  if ( rgb == NULL ) {
    return;
  }

  jsize len = env->GetArrayLength(rgb);
  jint *body = env->GetIntArrayElements(rgb,0);
  
  /* blit the screen */    
   int xx = 0, yy = 0;

   for (y = yOffset; y < yOffset+height; y++) {
     xx = 0;
     for (x = xOffset; x < xOffset+width; x++) {

    int pixel = body[ (yy*width) + xx ];    
    blitPixel(x,y,pixel);
       
       xx++;
    }
    yy++;
  }
  
 (env)->DeleteLocalRef( rgb ); 

}

// blit a JavaColor pixel
void FrameBuffer::blitPixel(int x, int y, int pixel) {
  location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                  (y+vinfo.yoffset) * finfo.line_length;


  if ( location < 0 || location >= screensize ) {
    return;
  }


    // from java color
      int a = (pixel >> 24) & 0xFF;
       if ( a == 0 ) { return; }
       int r = (pixel >> 16) & 0xFF;
       int g = (pixel >> 8) & 0xFF;
       int b = pixel & 0xFF;



       if (vinfo.bits_per_pixel == 32) {
           *(fbp + location) = b;       // Some blue
           *(fbp + location + 1) = g;   // A little green
           *(fbp + location + 2) = r;   // A lot of red
           *(fbp + location + 3) = 0;   // No transparency
          //location += 4;
       } else  { //assume 16bpp
    unsigned short int t = (r>>3)<<11 | (g>>3) << 6 | (b/8);
          *((unsigned short int*)(fbp + location)) = t;
       }
}


// ===========================================

void FrameBuffer::dispStrColor(const char* str, int x, int y, int color, int zoom, bool renderSpaces) {
  if ( str == NULL ) { return; }
  int len = strlen( str );
  
  int i=0;
  for(i=0; i < len; i++) {
     int code = (int) str[i];
     x += 6*zoom;
     if (!(code == 32 && !renderSpaces)) dispCharColor(code, x, y, color, zoom);  
  }
}

void FrameBuffer::dispCharColor(int code, int x, int y, int color, int zoom) {
  int offset = code * (fontWidth*fontHeight);
  int xx,yy;
  for(yy=0; yy < fontHeight*zoom; yy+=zoom) {
    for(xx=0; xx < fontWidth*zoom; xx+=zoom) {
      //if ( fontPLAIN_data[ offset++ ] > 0 ) {
      if ( fontBOLD_data[ offset++ ] > 0 ) {
      
        //int pixel = (255<<24)|(255<<16)|(255<<8)|(255);    
        //int pixel = (255<<24)|(0<<16)|(255<<8)|(0);
        for(int yyy=0; yyy < zoom; yyy++) {
          for(int xxx=0; xxx < zoom; xxx++) {
                 blitPixel( ((x+xx))+xxx, ((y+yy))+yyy,color);
          }
        }
        
      }
    }
  }
}
// ===========================================



void FrameBuffer::closeFb() {
   munmap(fbp, screensize);
   close(fbfd);
}

//=============================================