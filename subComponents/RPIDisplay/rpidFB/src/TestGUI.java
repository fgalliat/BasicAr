import com.xtase.hardware.*;
import com.xtase.struct.*;

import java.awt.*;
import java.util.*;

import java.io.*;

public class TestGUI {

  // static int DEF_SER_PORT_SPEED = 9600;
  static int DEF_SER_PORT_SPEED = 115200;
  static String DEF_SER_PORT = "/dev/ttyS0";

  // false -> Desktop Emul
  // true  -> RPI FB & Serial port
  static boolean MODE_RPI = false;

  // =============================================================
  static long now() {
    return System.currentTimeMillis();
  }

  static void _(Object o) {
    System.out.println("" + o);
  }

  static void Zzz(long time) {
    try {
      Thread.sleep(time);
    } catch (Exception ex) {
    }
  }
  // =============================================================

  static TFTScreen t = null;
  // static GPIOInput gpioIn = new GPIOInput();

  static void halt() {
    try {
      t.closeFb();
    } catch (Exception ex) {
    }
    try {
      serialPort.close();
    } catch (Exception ex) {
    }
    // MappableKeyboard.closeKb();
  }

  static void drawString(String msg, int x, int y, int color) {
    t.blitString(msg, x, y, color, true);
  }

  static boolean pollKbd = false;

  static SerialPortManager serialPort = null;
  static boolean pollSerial = false;

  static GUIInterfaceFB vgat = null;

  public static void main(String[] args) throws Exception {

    MODE_RPI = args != null && args.length >= 1 && args[0].equalsIgnoreCase("rpi");
    System.out.println("MODE RPI > " + MODE_RPI);

    /* // =================================
    // needs sudo
    gpioIn.GPIO_EXEC_PATH = "/usr/bin/";
    new Thread() { public void run() { 
      try { gpioIn.start(false); } catch(Exception ex) { ex.printStackTrace(); }
    } }.start();
    // ================================= */

    if (MODE_RPI) {
      t = TFTScreen.getInstance();
      if ( TFTScreen.SCREEN_HEIGHT < GUIInterfaceFB.ftHeight * GUIInterfaceFB.TTY_HEIGHT ) {
        GUIInterfaceFB.TXT_ZOOM = 1;
      }
    } else {
      t = TFTScreen_emul.getInstance();
    }

    Runtime.getRuntime().addShutdownHook(new Thread() {
      public void run() {
        halt();
      }
    });

    // TFTScreen.SCREEN_VIRTUAL_WIDTH = 800;
    // TFTScreen.SCREEN_VIRTUAL_HEIGHT = 600;
    TFTScreen.SCREEN_VIRTUAL_WIDTH = GUIInterfaceFB.ftWidth * 100;
    TFTScreen.SCREEN_VIRTUAL_HEIGHT = GUIInterfaceFB.ftHeight * 50;
    if (!MODE_RPI) {
      TFTScreen.SCREEN_WIDTH = TFTScreen.SCREEN_VIRTUAL_WIDTH;
      TFTScreen.SCREEN_HEIGHT = TFTScreen.SCREEN_VIRTUAL_HEIGHT;
    }
    t.reshape();
    t.recomputeOrigins();

    t.fillColor(Color.cyan.getRGB(), TFTScreen.SCREEN_VIRTUAL_X, TFTScreen.SCREEN_VIRTUAL_Y,
        TFTScreen.SCREEN_VIRTUAL_WIDTH, TFTScreen.SCREEN_VIRTUAL_HEIGHT);

    long t0, t1;

    if (true) {
      vgat = new GUIInterfaceFB(t);
      vgat.cls();
      Zzz(200);
      //vgat.setup();

      if (MODE_RPI) {
        serialPort = new SerialPortManager(DEF_SER_PORT, DEF_SER_PORT_SPEED);
        serialPort.open();

        boolean shutdown = false;

        while (true) {
          int ci = -1;
          char ch = 0x00;
          while ((ci = serialPort.readChar(3)) >= 0) {
            ch = (char) ci;
            shutdown = vgat.submitChar(ch);
            if (shutdown) {
              break;
            }
          }
          if (shutdown) {
            break;
          }
          TFTScreen.Zzz(100);
        }
        System.out.println("OK Bye");

      } else {

        // FAKED Serial Port.... =====================
        // echo "comment ca va ?" > /tmp/vgat 
        // echo -e "^[e" > /tmp/vgat  -- to cls system
        // echo -e "\\u3" > /tmp/vgat  -- to close system (but w/ only 1 '\')

        String pipeFileName = "rpid_out"; // here its the input !!!!
        File pipeFile = new File("/tmp/", pipeFileName);
        pipeFile.delete();
        if (!pipeFile.exists()) {
          int ok = Runtime.getRuntime().exec("mkfifo " + pipeFile.getAbsolutePath()).waitFor();
          if (ok != 0) {
            System.out.println("Error creating pipe file !!!!");
            System.exit(1);
          }
        }

        System.out.println("OK w/ PipeFile");
        InputStream serial = new FileInputStream(pipeFile);

        final char EXIT_CHAR = 0x04;
        boolean shutdown = false;
        while (true) {
          char ch = 0x00;
          while (serial.available() > 0) {
            ch = (char) serial.read();
            shutdown = vgat.submitChar(ch);
            if (shutdown) {
              break;
            }
          }
          if (shutdown) {
            break;
          }
          TFTScreen.Zzz(100);
        }

        serial.close();
        pipeFile.delete();
        System.out.println("OK Bye");

      }

      halt();
      System.exit(0);

      // ===============================================

    } // if VGAT

    /*
    Bitmap img3 = null;
    drawString("img 2 loading", 60, 190, txtColor);
    img3 = Bitmap.loadImage("./image3.jpg" );
    System.out.println("img 2 loaded");
    */

    // ===============================

  }

}