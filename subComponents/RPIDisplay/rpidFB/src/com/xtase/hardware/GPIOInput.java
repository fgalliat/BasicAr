package com.xtase.hardware;

import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;


public class GPIOInput {


 public static String GPIO_EXEC_PATH = "./";

  /*
  
    to work with adafruit TFT touchscreen - 4 buttons interface
  
  */


  static void gpioSet(String cmd) throws Exception {
    run(GPIO_EXEC_PATH+"gpio "+cmd);
  }

  protected static boolean IO18 = false;
  protected static boolean IO27 = false;
  protected static boolean IO22 = false;

  public static boolean isIO18() { return IO18; }
  public static boolean isIO27() { return IO27; }
  public static boolean isIO22() { return IO22; }


  public static void main(String[] args) throws Exception {
   start(args == null || args.length == 0 ? true : false);
  }

  static boolean verbose = true;
  public static void start(boolean _verbose) throws Exception {
    verbose = _verbose;

   _("reseting");
   
   /*TFTScreen.getInstance().addKeyListener( new KeyListener() {
    public void keyTyped(java.awt.event.KeyEvent e) {
    	_("GPIO "+e.getKeyCode());
    }
    
    public void keyPressed(java.awt.event.KeyEvent e) {
    	_("GPIO "+e.getKeyCode());
    }
    
    public void keyReleased(java.awt.event.KeyEvent e) {}
   } );
   */
   
   
   //run("./gpio reset");
/*
   gpioSet("export 1 in"); // #18
   gpioSet("export 2 in"); // #27
   gpioSet("export 3 in"); // #22

   gpioSet("mode 1 up"); // #18
   gpioSet("mode 2 up"); // #27
   gpioSet("mode 3 up"); // #22

   _("starting");
   
while( true ) {
   List<String> table = runResult(GPIO_EXEC_PATH+"gpio readall");
   for(String line : table) {
      if ( line.contains( "GPIO 1" ) || line.contains( "GPIO 2" ) || line.contains( "GPIO 3" ) ) {
         //_(line);
              if ( line.contains( "GPIO 1" ) ) { IO18 = line.contains("Low"); }
         else if ( line.contains( "GPIO 2" ) ) { IO27 = line.contains("Low"); }
         else if ( line.contains( "GPIO 3" ) ) { IO22 = line.contains("Low"); break; }
      }
   }

   _("#22 "+IO22+" #27 "+IO27+" #18 "+IO18); 

   _("");
Zzz(700);
}
    
//   _("stoping");
//   gpioSet("unexportall");
*/

  }
 
  static void Zzz(long time) {
    try { Thread.sleep(time); } catch(Exception ex) {}
  }

  static void _(Object o) {
    if (verbose) System.out.println(""+o);
  }

  static void run(String cmd) throws Exception {
    Runtime.getRuntime().exec(cmd);
  }

  //static List<String> runResult(String cmd) throws Exception {
  //  Process ps = Runtime.getRuntime().exec(cmd);
  //  BufferedReader reader = new BufferedReader( new InputStreamReader( ps.getInputStream() ) );
  //  String line = null;
  //  //String buff = "";
  //  List<String> buff = new ArrayList<String>();
  //  while( ( line = reader.readLine() ) != null ) {
  //    //buff += line +"\n";
  //    buff.add( line );
  //  }
  //  return buff;
  //}



}