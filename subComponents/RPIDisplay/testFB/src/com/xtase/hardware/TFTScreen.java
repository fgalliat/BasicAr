package com.xtase.hardware;

import com.xtase.struct.*;

import java.io.*;
import java.util.*;
import java.awt.image.*;
import java.awt.*;

public class TFTScreen {

  static long now() { return System.currentTimeMillis(); }

  // ===========================================
    public static /*final*/ int SCREEN_WIDTH = 320;
    public static /*final*/ int SCREEN_HEIGHT = 240;
    public static /*final*/ int SCREEN_VIRTUAL_WIDTH = 800;
    public static /*final*/ int SCREEN_VIRTUAL_HEIGHT = 600;
    public static /*final*/ int SCREEN_VIRTUAL_X = 0;
    public static /*final*/ int SCREEN_VIRTUAL_Y = 0;
  
  public void recomputeOrigins() {
    SCREEN_VIRTUAL_X = ( SCREEN_WIDTH - SCREEN_VIRTUAL_WIDTH ) / 2;
    SCREEN_VIRTUAL_Y = ( SCREEN_HEIGHT - SCREEN_VIRTUAL_HEIGHT ) / 2;
  }
  // ===========================================


  // ===========================================

 //static String DEVICE = "/dev/fb1";
 static String DEVICE = "/dev/fb0";


 static {
   System.loadLibrary("TFTScreen");
 }

 protected native int  openFb(String fbDev);
 //protected native void closeFb();
  public native void closeFb();

 public native void fillDirect(int[] buff, int x, int y, int width, int height);
 public native void fillColor(int color, int x, int y, int width, int height);
  
 protected native void fill(int[] buff, int index, int x, int y, int width, int height);
 public native void resetFrame(int index);

 protected native void drawString(String text, int x, int y, int color, int zoom, boolean renderSapces);

 public native void blitPixel(int x, int y, int color);


 public native void FX_starField();



 // cf a C++ lib bug
 //public native void fillAll(int[] buff);
 protected void fillAll(int[] buff, int index) {
   fill(buff, index, 0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
 }

// -------------------------------

int[] cls = null;

public synchronized void cls() {
  if ( cls == null ) {
   cls = new int[(SCREEN_WIDTH*SCREEN_HEIGHT)];
   int black = Color.BLACK.getRGB();
   for(int i=0; i < cls.length; i++) { cls[i] = black; }
  }  
  fill(cls, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

public int[] blit(int[] rgba, int index, int x, int y, int width, int height) {
  long t0,t1;
  t0 = now();
  fill(rgba, index, x, y, width, height);
  t1 = now();
//  _("draw image = "+ (t1-t0) +" msec");
  return rgba;
}


public int[] blit(BufferedImage img, int index,int x, int y) {
  return blit(img,index,x,y,img.getWidth(), img.getHeight());
}

public int[] blit(BufferedImage img, int index,int x, int y, int width, int height) {
  // coords to be confirmed
  long t0,t1;
  t0 = now();
  int[] array1 = img.getRGB(x, y, width, height, null, (y*width)+x, width); 
  t1 = now();
//  _("grabb image = "+ (t1-t0) +" msec");

  t0 = now();
  fill(array1, index,x, y, width, height);
  t1 = now();
//  _("draw image = "+ (t1-t0) +" msec");
  return array1;
}


  // ===========================================

  protected static TFTScreen instance = null;

  public static synchronized TFTScreen getInstance() {
    if ( instance == null ) {
      instance = new TFTScreen();
    }
    return instance;
  }

  // ===========================================


    protected TFTScreen() {
  //t = new Test();
        openFb(DEVICE);

      // TMP because :
      // Should be done by the JNI as it is printed @ start
      
      // '/dev/fb0' -> 'fb0'
      String fbNum = DEVICE.substring( DEVICE.lastIndexOf("/")+1 );
      
      long t0,t1;
      t0 = System.currentTimeMillis();
      int w = -1, h = -1;
      try {
        byte[] b = new byte[32];
        FileInputStream fis = new FileInputStream("/sys/class/graphics/"+ fbNum +"/virtual_size");
        int readed = fis.read(b, 0, b.length);
        fis.close();
        String strRes = new String(b, 0, readed).trim();
        if ( strRes.endsWith( (""+(char)0x00) ) ) {
          strRes = strRes.substring(0, strRes.length()-1);
        }
        //System.out.println("(ii) "+strRes);
        w = Integer.parseInt( strRes.substring(0, strRes.indexOf(",")) );
        h = Integer.parseInt( strRes.substring(strRes.indexOf(",")+1) );
        
        SCREEN_WIDTH = w;
        SCREEN_HEIGHT = h;
        
        recomputeOrigins();
      } catch(Exception ex) {
        System.out.println("(EE) "+ex.toString());
      }
      t1 = System.currentTimeMillis();
      System.out.println((t1-t0)+"msec to read resolution... w="+w+" h="+h);
      
    }

    public void close() {
  cls();
  closeFb();
    }



    public void blit(Bitmap bmp, int x, int y) {
  blit( bmp, x, y, bmp.getWidth(), bmp.getHeight() ); 
    }

    public void blit(Bitmap bmp, int xDest, int yDest, int width, int height) {
  blit(bmp.getPixels(), bmp.getInnerIndex(), xDest, yDest, width, height);
    }

    public void blit(Bitmap bmp, int xDest, int yDest, int x, int y, int width, int height) {
  // TODO !!!!! ex font cropped char
    }

  // ===========================================
    public void blitString(String txt, int x, int y, int color) {  
      blitString (txt,x,y,color, 1);
    }
  
    // faster if no needs to render 'space' chars
    public void blitString(String txt, int x, int y, int color, boolean renderSpaces) {  
      drawString (txt,x,y,color,1, renderSpaces);
    }
      
    public void blitString(String txt, int x, int y, int color, int zoom) {
      blitString (txt,x,y,color, false, zoom);
    }

   // public void blitString(String txt, int x, int y, int color, boolean shadow) {
   //   blitString(txt, x, y, color, shadow, 1);
   // }
      
    public void blitString(String txt, int x, int y, int color, boolean shadow, int zoom) {
      if (shadow) {
        drawString (txt,x-1,y-1,Color.white.getRGB(),1, true);
        drawString (txt,x+1,y+1,Color.black.getRGB(),1, true);
      }
      drawString (txt,x,y,color,1, true);
    }

  // ===========================================


}