package com.xtase.hardware;

public class FrameBuffer {

 public static void dispStrColor(TFTScreen screen, String str, int x, int y, int color, int zoom) {
   if ( str == null ) { return; }
   int len = str.length();
   
   int i=0;
   for(i=0; i < len; i++) {
    int code = (int) str.charAt(i);
    x += 6*zoom;
    dispCharColor(screen, code, x, y, color, zoom);  
   }
 }

 public static void dispCharColor(TFTScreen screen, int code, int x, int y, int color, int zoom) {
  int offset = code * (font_bold.fontWidth*font_bold.fontHeight);
  int xx,yy;
  for(yy=0; yy < font_bold.fontHeight*zoom; yy+=zoom) {
    for(xx=0; xx < font_bold.fontWidth*zoom; xx+=zoom) {
      //if ( fontPLAIN_data[ offset++ ] > 0 ) {
      if ( font_bold.fontBOLD_data[ offset++ ] > 0 ) {
      
        for(int yyy=0; yyy < zoom; yyy++) {
          for(int xxx=0; xxx < zoom; xxx++) {
            //TFTScreen.getInstance().blitPixel( ((x+xx))+xxx, ((y+yy))+yyy, color);
            screen.blitPixel( ((x+xx))+xxx, ((y+yy))+yyy, color);
          }
        }
        
      }
    }
  }
 }


}