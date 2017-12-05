package com.xtase.struct;
  
import com.xtase.hardware.*;

import javax.imageio.ImageIO;

import java.io.*;
import java.util.*;
import java.awt.image.*;
import java.awt.*;

public class Bitmap {

  public static Bitmap loadImage(String path) throws Exception {
    return new Bitmap(ImageIO.read(new File(path)));
  }

  public static Bitmap createNew(int width, int height) {
      return new Bitmap(width, height);
  }
  
  public static Bitmap createFrom(Bitmap bmp, int x, int y, int width, int height) {
     Bitmap result = createNew(width, height);
     result.blitCrop( bmp, x, y, width, height );
    return result;
  }

  // ================================================================
  protected int width;
  protected int height;
  protected int[] argb;

  protected int innerIndex = -1;
  protected static int cptImg = 5; /// cf cls  ... let starts a bit after
  // ================================================================

  protected Bitmap(BufferedImage img) {
      this( img.getRGB(0,0,img.getWidth(),img.getHeight(),null,0,img.getWidth()), img.getWidth(),img.getHeight() );
  }

  protected Bitmap(int width, int height) {
    this( new int[ width * height ], width, height);
  }

  protected Bitmap(int[] argb, int width, int height) {
    this.width  = width;
    this.height = height;
    this.argb   = argb;

    innerIndex = cptImg++;
  }

  // ================================================================
  
  public void fillRect(int x, int y, int w, int h, int color) {
    for(int yy=0; yy < h; yy++) {
      for(int xx=0; xx < w; xx++) {
        argb[ ((yy+y)*w)+(xx+x) ] = color;
      }
    }
  }
  
  public void drawString(String txt, int x, int y, int color) {
    // TODO
  }
  
  // ================================================================
  
  public void clear(int color) {
    for(int i=0; i < getWidth() * getHeight(); i++) {
      argb[ i ] = color;
    }
  }
  
  public void blit(Bitmap bmp, int x, int y) {
    if ( bmp == null ) { return; }
  
    for(int yy=0; yy < bmp.getHeight(); yy++) {
      if ( yy + y > this.getHeight() ) { break; }
      for(int xx=0; xx < bmp.getWidth(); xx++) {
        if ( xx + x > this.getWidth() ) { break; }
        setRGB( x + xx, y + yy, bmp.getRGB( xx,yy ) );
      }  
    }
  }
  
  // draws @ 0,0
  public void blitCrop(Bitmap bmp, int x, int y, int width, int height) {
    if ( bmp == null ) { return; }
  
    for(int yy=0; yy < height; yy++) {
      if ( yy + 0 > this.getHeight() ) { break; }
      //if ( yy + y > height ) { break; }
      
      for(int xx=0; xx < width; xx++) {
        if ( xx + 0 > this.getWidth() ) { break; }
        //if ( xx + x > width ) { break; }
        
        setRGB( xx, yy, bmp.getRGB( x+xx,y+yy ) );
      }  
    }
  }
  
  // ================================================================

    public int getInnerIndex()  { return innerIndex; }
      
    static Object blocker = "coucou";
      
    public void resetInnerIndex()  { 
      synchronized( blocker ) {
        
        TFTScreen.getInstance().resetFrame(innerIndex);
        
        innerIndex = cptImg++;
        innerIndex %= 50;                         // \__ beware with frame rewrite....
        if ( innerIndex < 5 ) { innerIndex = 5; } // /
      }
    }

    public int getWidth()  { return width; }
    public int getHeight() { return height; }

    public int[] getPixels() {
      return argb;
    }  

    public int getRGB(int x, int y) {
      return argb[ (y*width)+x ];
    }

    public void setRGB(int x, int y, int color) {
      int alpha = color >> 24 & 0xFF;
      if ( alpha == 0 ) { return; }
      
      argb[ (y*width)+x ] = color;
    }



}