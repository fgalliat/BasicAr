package com.xtase.hardware;

import com.xtase.struct.*;

import java.io.*;
import java.util.*;
import java.awt.image.*;
import java.awt.*;
import java.awt.event.*;

public class TFTScreen_emul extends TFTScreen /*Window*/ implements Runnable {

  static Window win = new Window(initFrame()) {
    public void update(Graphics g) {
      paint(g);
    }

    public void paint(Graphics g) {
      if (misDirty) {
        mis.newPixels(0, 0, (int) SCREEN_VIRTUAL_WIDTH, (int) SCREEN_VIRTUAL_HEIGHT);
      }

      // use an ImageObserver ('win') consumes more cpu ~80%
      //g.drawImage(image, 0, 0, win);
      g.drawImage(image, 0, 0, null);
    }
  };

  // ===========================================================
  // ===========================================================

  // beware w/ these static
  private static Image image;
  private static MemoryImageSource mis;
  private static boolean misDirty = false;
  private static int pix[];

  protected void init() {
    pix = new int[(int) (SCREEN_VIRTUAL_WIDTH * SCREEN_VIRTUAL_HEIGHT)];
  }

  public void recomputeOrigins() {
    super.recomputeOrigins();

    pix = new int[(int) (SCREEN_VIRTUAL_WIDTH * SCREEN_VIRTUAL_HEIGHT)];
    mis = new MemoryImageSource((int) SCREEN_VIRTUAL_WIDTH, (int) SCREEN_VIRTUAL_HEIGHT, ColorModel.getRGBdefault(),
        pix, 0, (int) SCREEN_VIRTUAL_WIDTH);
    mis.setAnimated(true);
    // mis.setFullBufferUpdates(true);
    image = win.createImage(mis);
  }

  Thread saie;

  public void start() {
    saie = new Thread(this);
    saie.start();
  }

  public void run() {
    while (true) {
      try {
        win.repaint();
        Zzz(55L);
      } catch (Exception exception) {
        exception.printStackTrace();
      }
    }
  }

  // ===========================================================
  // ===========================================================

  static {
    //System.loadLibrary("TFTScreen");
  }

  protected int openFb(String fbDev) {
    init();
    start();
    return 0;
  }

  public void closeFb() {
  }

  public void fillDirect(int[] buff, int x, int y, int width, int height) {
    fill(buff, -1, x, y, width, height);
  }

  public void fillColor(int color, int x, int y, int width, int height) {
    for (int yy = 0; yy < height; yy++) {
      for (int xx = 0; xx < width; xx++) {
        blitPixel(x + xx, y + yy, color);
      }
    }
  }

  protected void fill(int[] buff, int index, int x, int y, int width, int height) {
    for (int yy = 0; yy < height; yy++) {
      for (int xx = 0; xx < width; xx++) {
        blitPixel(x + xx, y + yy, buff[(yy * width) + xx]);
      }
    }
  }

  protected void drawString(String text, int x, int y, int color, int zoom, boolean renderSapces) {
    drawString(text, x, y, color, zoom);
  }

  protected void drawString(String text, int x, int y, int color, int zoom) {
    FrameBuffer.dispStrColor(this, text, x, y, color, zoom);
  }

  public void blitPixel(int x, int y, int color) {
    // emulate BLACK when alpha
    // if ( color == 0 ) { color = 255<<24; }
    int addr = (y * ((int) SCREEN_VIRTUAL_WIDTH)) + x;
    if (addr < 0 || addr >= (SCREEN_VIRTUAL_WIDTH * SCREEN_VIRTUAL_HEIGHT)) {
      return;
    }
    if (pix[addr] == color) {
      return;
    }
    pix[addr] = color;
    misDirty = true;
  }

  public void FX_starField() {
    FX_Starfield.drawFX();
  }

  // ===========================================================
  // ===========================================================

  // ===========================================

  protected static TFTScreen_emul instance = null;

  public static synchronized TFTScreen getInstance() {
    if (instance == null) {
      instance = new TFTScreen_emul();
    }
    return instance;
  }

  // ===========================================

  static TextArea zone = null;

  static Frame initFrame() {
    Frame f = new Frame("");
    f.setSize(100, 100);

    f.setVisible(true); // BEWARE w/ this Hack

    zone = new TextArea();

    zone.addKeyListener(new java.awt.event.KeyListener() {
      public void keyTyped(java.awt.event.KeyEvent e) {
        //System.out.println("GPIO "+e.getKeyCode());
      }

      public void keyReleased(java.awt.event.KeyEvent e) {
      }

      public void keyPressed(java.awt.event.KeyEvent e) {
        //System.out.println("GPIO "+e.getKeyCode());
        int code = e.getKeyCode();
        // 	if ( code == KeyEvent.VK_1 ) { GPIOInput.IO22 = true; }
        // 	else if ( code == KeyEvent.VK_3 ) { GPIOInput.IO27 = true; }
        // 	else if ( code == KeyEvent.VK_2 ) { GPIOInput.IO18 = true; }

        // 	if ( code == KeyEvent.VK_UP ) { JoystickInput.UP = true; }
        // 	else if ( code == KeyEvent.VK_DOWN ) { JoystickInput.DOWN = true; }
        // 	else if ( code == KeyEvent.VK_LEFT ) { JoystickInput.LEFT = true; }
        // 	else if ( code == KeyEvent.VK_RIGHT ) { JoystickInput.RIGHT = true; }
        // 	else if ( code == KeyEvent.VK_SPACE ) { JoystickInput.FIRE = true; }

        // 	if ( code == KeyEvent.VK_ESCAPE ) { System.exit(0); }
        //  }

        //  public void keyReleased(java.awt.event.KeyEvent e) {
        // 	int code = e.getKeyCode();
        // 	if ( code == KeyEvent.VK_1 ) { GPIOInput.IO22 = false; }
        // 	else if ( code == KeyEvent.VK_3 ) { GPIOInput.IO27 = false; }
        // 	else if ( code == KeyEvent.VK_2 ) { GPIOInput.IO18 = false; }

        // 	if ( code == KeyEvent.VK_UP ) { JoystickInput.UP = false; }
        // 	else if ( code == KeyEvent.VK_DOWN ) { JoystickInput.DOWN = false; }
        // 	else if ( code == KeyEvent.VK_LEFT ) { JoystickInput.LEFT = false; }
        // 	else if ( code == KeyEvent.VK_RIGHT ) { JoystickInput.RIGHT = false; }
        // 	else if ( code == KeyEvent.VK_SPACE ) { JoystickInput.FIRE = false; }
      }
    });
    f.setVisible(true);

    f.add(zone);

    zone.requestFocus();
    return f;
  }

  protected TFTScreen_emul() {
    super();

    SCREEN_WIDTH = 800;
    SCREEN_HEIGHT = 600;

    win.setSize((int) SCREEN_WIDTH, (int) SCREEN_HEIGHT);
    //win.setSize( (int)SCREEN_VIRTUAL_WIDTH, (int)SCREEN_HEIGHT);
    win.setLocation(100, 0);
    win.setVisible(true);
  }

  public void reshape() {
    win.setSize((int) SCREEN_WIDTH, (int) SCREEN_HEIGHT);
    System.out.println("Screen is now " + SCREEN_WIDTH + "x" + SCREEN_HEIGHT);
  }

  public void close() {
    cls();
    closeFb();
  }

}