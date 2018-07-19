/*
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Serial.h"
#include "BridgedScreen.h"

// for Trigo fcts
//#include <tgmath.h>
#include <cmath>
#include <time.h>
*/

#include <Arduino.h>
#include "FXnativebckg.h"
#include "GenericMCU.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
extern GenericMCU mcu;

void setupBCK();
void loopBCK();

int mode = 1; // screen redraw mode -> 1: cls
float speed = 0.5f;
uint16_t color = 1; // WHITE

bool _star_inited = false;
void runNativeBackGroundStars(uint16_t starsNb,uint16_t _speed,uint16_t _mode,uint16_t _color) {
  if ( !_star_inited ) {
    setupBCK();
    _star_inited = true;
  }

  // starsNb is ignored @this time
  speed = (float)_speed / 200.0f; // in percent --> 100 => 0.5f
  mode  = _mode;
  color = _color;

  loopBCK();
}

// Arduino .cos(rad)

static bool firstRND=true;

float rnd(float d1) {
 if (firstRND) {
   firstRND = false;
  //  srand( time(NULL) );
 }

//  return (float)( (rand() % 10000) / 10000.0 * (double)d1 );
return (float)( ( random(10000) ) / 10000.0 * (double)d1 );
}



// =====================================
/*
Serial serial("/dev/ttyS1");
BridgedScreen screen( &serial );

#define CLR_PINK  8
#define CLR_GREEN 6
#define CLR_BLACK 0
#define CLR_WHITE 1


*/
// =====================================

void drawStarFieldFrame();




void setupBCK() {
  // Serial.begin(115200);
  //printf("Starting screen\n");
  //screen.cls();
}

void loopBCK() {
 //screen.blitt(0);
 if (mode ==1) mcu.getScreen()->clear();
 drawStarFieldFrame();
 //screen.blitt(2);
}


/*
int main(int argc, char* argv[] ) {
  screen.cls();
  setup();

  for (int i=0; i < 50; i++) {
    loop();
  }

  // screen.drawBPP("/BLAST.BPP", 0, 0);
  // screen.drawPCT("/CARO.PCT", 160, 90);

  // screen.drawPCTSprite("/CARO.PCT", 160, 0, 64, 64, 0, 0);
  // screen.drawPCTSprite("", 160+64+4, 0, 64, 64, 0, 0);

  // screen.println("Hello world from LCD controller !"); // BEWARE max 30 chars
  // screen.print(128);
  // screen.println("");
  // screen.print(3.14f);
  // screen.println("");

  // screen.drawRect(10,10,20,20,1,8); // PINK FLAT RECT
  // screen.drawLine(10,10,10+20,1+20,7); // DARKGREEN LINE
  // //screen.drawCircle(10,10+30,10,1,5); // LIGHTGREEN FLAT CIRCLE -- doesn't show
  // screen.drawCircle(10,10+30,10,0,5); // LIGHTGREEN FLAT CIRCLE

  // screen.drawTriangle(100,10, 100+20,10+30, 100,10+60  ,1,5); // LIGHTGREEN FLAT TRIANGLE
}
*/
// ----------------------------------------------------------------------

typedef struct {
    float x;
    float y;
    float z;
} Star;

// TO-LOOK
const int size=50;

int zMax = 5;


int width = SCREEN_WIDTH;
int height = SCREEN_HEIGHT;

bool inited = false;

static Star starfield[ size ];

/*
 have to define 2 exec modes : 1 uses CLS
 then another that just erase previous drawn star
*/

void createStar(int i);
void initStars();

void drawStarFieldFrame() {
  if ( !inited ) {
    initStars();
    inited = true;
  }

  // screen.cls();
  
  for(int i=0; i < size; i++) {
    if ( starfield[i].z <= speed ) {
      createStar(i);
    } else {
      starfield[i].z -= speed;
    }
    
    int x = (int)( (float)(width / 2) + starfield[i].x / starfield[i].z );
    int y = (int)( (float)(height / 2) + starfield[i].y / starfield[i].z );
    
    if ( x<0 || y<0 || x>=width || y>=height ) {
      createStar(i);
    } else {
      int zy = (int)starfield[i].z;
      if ( zy < 1 ) { zy = 1; }
      int zz = (zMax / zy );
      if (zz < 1) { zz=1; }
      int xx = x-(zz/2); if ( xx < 0 ) { xx=0; }
      int yy = y-(zz/2); if ( yy < 0 ) { yy=0; }
      
      //screen.drawRect( xx, yy, zz,zz, 1, 1 );
      mcu.getScreen()->drawRect( xx, yy, zz,zz, 1, 1 );
    }
  }
  
}

void initStars() {
  for(int i=0; i < size; i++) {
    createStar(i);
    starfield[i].z = rnd( zMax );
  }
}

void createStar(int i) {
  // starfield[i] = new ....
  starfield[i].x = rnd(2*width) - width;
  starfield[i].y = rnd(2*height) - height;
  starfield[i].z = zMax;
}

