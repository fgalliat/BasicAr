import com.xtase.hardware.*;
import com.xtase.struct.*;

import java.awt.*;
import java.util.*;

import java.io.*;

public class TestGUI {
	
  //static int DEF_SER_PORT_SPEED = 9600;
  static int DEF_SER_PORT_SPEED = 115200;
  static String DEF_SER_PORT = "/dev/ttyS0";
	
	// =============================================================
	  static long now() { return System.currentTimeMillis(); }
	  static void _(Object o) { System.out.println(""+o); }
	// =============================================================
	
  static TFTScreen t = null;
	static GPIOInput gpioIn = new GPIOInput();
	
  	
  
  static void halt() {
    try { t.closeFb(); } catch(Exception ex) {}
    try { serialPort.close(); } catch(Exception ex) {}
    // MappableKeyboard.closeKb();
  }
  
  static boolean pollKbd = false;
  
  static SerialPortManager serialPort = null;
  static boolean pollSerial = false;
  
  //static VGATInterfaceFB vgat = null;
  static GUIInterfaceFB vgat = null;
	
	public static void main(String[] args) throws Exception {
	   /* // =================================
	   // needs sudo
	   gpioIn.GPIO_EXEC_PATH = "/usr/bin/";
	   new Thread() { public void run() { 
	     try { gpioIn.start(false); } catch(Exception ex) { ex.printStackTrace(); }
	   } }.start();
	   // ================================= */

      
      // HERE HERE HERE HERE HERE HERE HERE HERE HERE 
   //t = TFTScreen.getInstance();
   t = TFTScreen_emul.getInstance();
   // HERE HERE HERE HERE HERE HERE HERE HERE HERE 

      Runtime.getRuntime().addShutdownHook(new Thread() {
        public void run() {
          halt();
        }
      });
      
      
      // TFTScreen.SCREEN_VIRTUAL_WIDTH = 800;
      // TFTScreen.SCREEN_VIRTUAL_HEIGHT = 600;
      TFTScreen.SCREEN_VIRTUAL_WIDTH = GUIInterfaceFB.ftWidth*100;
      TFTScreen.SCREEN_VIRTUAL_HEIGHT = GUIInterfaceFB.ftHeight*50;
      TFTScreen.SCREEN_WIDTH = TFTScreen.SCREEN_VIRTUAL_WIDTH;
      TFTScreen.SCREEN_HEIGHT = TFTScreen.SCREEN_VIRTUAL_HEIGHT;
      t.reshape();
      t.recomputeOrigins();
  /*
  int[] array = new int[(TFTScreen.SCREEN_VIRTUAL_WIDTH*TFTScreen.SCREEN_VIRTUAL_HEIGHT)];
  for(int i=0; i < TFTScreen.SCREEN_VIRTUAL_WIDTH*TFTScreen.SCREEN_VIRTUAL_HEIGHT; i++) {
    array[i] = Color.cyan.getRGB();
  }
  t.blit(array, 2, TFTScreen.SCREEN_VIRTUAL_X,TFTScreen.SCREEN_VIRTUAL_Y, TFTScreen.SCREEN_VIRTUAL_WIDTH, TFTScreen.SCREEN_VIRTUAL_HEIGHT);
  */  
  t.fillColor(Color.cyan.getRGB(), TFTScreen.SCREEN_VIRTUAL_X,TFTScreen.SCREEN_VIRTUAL_Y, TFTScreen.SCREEN_VIRTUAL_WIDTH, TFTScreen.SCREEN_VIRTUAL_HEIGHT);
  
  long t0,t1;
      
      
      if (true) {
        vgat = new GUIInterfaceFB( t );
        vgat.cls();
        Zzz(200);
        //vgat.setup();

        // FAKED Serial Port.... =====================
        // echo "comment ca va ?" > /tmp/vgat 
        // echo -e "^[e" > /tmp/vgat  -- to cls system
        // echo -e "\\u3" > /tmp/vgat  -- to close system (but w/ only 1 '\')

        String pipeFileName = "rpid_out"; // here its the input !!!!
        File pipeFile = new File("/tmp/", pipeFileName);
        pipeFile.delete();
        if ( !pipeFile.exists() ) {
          int ok = Runtime.getRuntime().exec("mkfifo "+pipeFile.getAbsolutePath() ).waitFor();
          if ( ok != 0 ) {
            System.out.println("Error creating pipe file !!!!");
            System.exit(1);
          }
        }
        
        System.out.println("OK w/ PipeFile");
        InputStream serial = new FileInputStream( pipeFile );

        final char EXIT_CHAR = 0x04;

        while( true ) {
          char ch = 0x00;
          while ( serial.available() > 0 ) {
            ch = (char)serial.read();

//System.out.println((int)ch+ " "+ (char)ch);

            if ( ch == EXIT_CHAR ) { break; } // Ctrl-C code
            vgat.submitChar( ch );
          }
          if ( ch == EXIT_CHAR ) { break; } // Ctrl-C code
          Zzz(100);
        }

        serial.close();
        System.out.println("OK Bye");
        pipeFile.delete();
        System.exit(0);

        // ===============================================



        // vgat.test();
        
        // needed for VGAT to redisplay when relaunch from main TTY
        Zzz(200);
        
        // if ( args.length > 0 ) {
        //   pollKbd = true;
        //   MappableKeyboard kbdSupport = new MappableKeyboard() {
            
        //     boolean mainWin = true;
            
        //     public void keyEvent(MappableKeyboard.Key event) {
        //       // ~ as HobyTronics Serial USB HOST board does
        //         if ( event.pressed ) {
        //           if ( event.keyCode > -1 ) {
        //             String txt = event.text;
        //             if ( event.shifted && event.shiftedText != null ) { txt = event.shiftedText; }
        //             if (event.printable) {
        //               VGATInterfaceFB.getInstance().print( txt );
        //               //System.out.print( txt+"." ); 
        //             }
        //             else {
        //               if ( event.keyCode == 32 ) { VGATInterfaceFB.getInstance().print( " " ); }
        //               else if ( event.keyCode == 13 ) { VGATInterfaceFB.getInstance().print( "\n" ); }
        //               else if ( event.keyCode == 9 ) { mainWin = !mainWin; VGATInterfaceFB.getInstance().focus( mainWin ? 2 : 4 ); }
        //               //else if ( event.keyCode == MappableKeyboard.Key.KEY_BREAK ) { halt(); try { Runtime.getRuntime().exec("killall X"); } catch(Exception ex) {} }
        //               else if ( event.keyCode == MappableKeyboard.Key.KEY_ESC ) { halt(); try { Runtime.getRuntime().exec("killall X"); } catch(Exception ex) {} }
        //               else if ( event.keyCode == MappableKeyboard.Key.KEY_F1+0 ) { try { Raycast.main( new String[0] ); } catch(Exception ex) { System.out.println("Failed to launch Raycaster"); ex.printStackTrace(); } }
        //               else if ( event.keyCode == MappableKeyboard.Key.KEY_F1+1 ) { try { TestBPP.main( new String[0] ); } catch(Exception ex) { System.out.println("Failed to launch Raycaster"); ex.printStackTrace(); } }
        //               else { System.out.print( ">> "+txt+" <<\n" ); }
        //             }
        //           }
        //         }
        //     }
        //   };
          
        //   MappableKeyboard.MUTE_LOAD_KEYMAP = true;
        //   MappableKeyboard.Keyboard kbd = null;
        //   try {
        //     kbd = kbdSupport.new HTUsbHostKeyboard(0, "/vm_mnt/Java/testINPUT/map/pc105_us.keys.txt");
        //      // needed for Keyboard to re-listen when relaunch from main TTY
        //     Zzz(100);
        //     kbd.startPollKeys();
        //   } catch(Exception ex) {
        //     System.err.println( "[KB-EE] "+ex.toString() );
        //   }
          
        //   //Zzz(500);
        //   pollSerial = true;
          
          
        //   serialPort = new SerialPortManager(DEF_SER_PORT, DEF_SER_PORT_SPEED);
        //   serialPort.open();
          
        //   // remove when will be linked as HTUSBBOARD kbd
        //   serialPort.printSerialLine("Hello, I'm an RPI Zero W");
          
        //   new Thread() { public void run() {
        //     while( true ) {
        //       try {
				// //String line = serialPort.readSerialLine(true);
        //         //if ( line == null ) { continue; }
        //         //vgat.submit( line );
        //         int ch = serialPort.readChar(5);
        //         // ????
        //         if ( ch == -1 ) { continue; }
        //         VGATInterfaceFB.getInstance().submitChar( (char)ch );
        //       } catch(Exception ex) {
        //         System.out.println("(EE:0x01) "+ex.toString());
        //         ex.printStackTrace();
        //       }
        //     }
        //     //Zzz(500);
        //   	//serialPort.close();
        //   } }.start();
          
          
          
        // } // if args

      } // if VGAT
      
      
  /*
  Bitmap img3 = null;
  drawString("img 2 loading", 60, 190, txtColor);
  img3 = Bitmap.loadImage("./image3.jpg" );
  System.out.println("img 2 loaded");
  */

// ===============================

Random rnd = new Random();

/*
		Raspberry raspi = LocalDrivenRaspberry.getInstance();

		new Thread() { public void run() {
			BufferedReader in = new BufferedReader( new InputStreamReader( System.in ) );
			try { in.readLine(); } catch(Exception ex) {}
			t.close();
			System.exit(0);
		} }.start();

		new Thread() { public void run() {
		  while(true) {

		  if ( gpioIn.isIO18() ) {
			t.close();
			System.exit(0);
		  }

		  else if ( gpioIn.isIO22() ) {
		  	if ( screenNum > 0 ) { screenNum--; }
		  }

		  else if ( gpioIn.isIO27() ) {
		  	if ( screenNum < 2 ) { screenNum++; }
		  }


		  Zzz(500);

		 }
		} }.start();
*/

		/*
		while(true) {



			//System.out.print(".");

			Map<String, String> ips = EthUtils.getIps();
			float cpu = raspi.getCpu();
			long mem = raspi.getMem();
			long swap = raspi.getSwap();
			String eth0 = ips.get("eth0");
			String wlan0 = ips.get("wlan0");
			String essid = wlan0 != null ? EthUtils.getESSID("wlan0") : " --- ";
			float temp = raspi.getTemp();


			int bck = rnd.nextInt(3*1000)/1000;

			bck = 2;


			if ( bck == 2 ) {
			  	t.blit(img3, 0,0);
			} 

			if ( screenNum == 0 ) {
			  drawString("<<{ UPPER Screen }>>", 40, 5, txtColor);
			  Zzz(1000);
			  continue;
			} else if ( screenNum == 2 ) {
			  drawString("<<{ LOWER Screen }>>", 40, 5, txtColor);
			  Zzz(1000);
			  continue;
			} 


			    drawString("<<{ General Status }>>", 40, 5, txtColor);

			    int row = 40;
			    drawString("CPU  : "+ cpu +" %", 20, row, txtColor);
			    row += 15;
			    drawString("RAM  : "+ mem +" MB", 20, row, txtColor);
			    row += 15;
			    drawString("Swap : "+ swap +" MB", 20, row, txtColor);
			    row += 15;
			    drawString("Temp : "+ temp +" C", 20, row, txtColor);

			    
			    row += 35;
			    drawString("eth0  : "+ eth0 , 20, row, txtColor);
			    row += 15;
			    drawString("wlan0 : "+ wlan0 , 20, row, txtColor);
			    row += 15;
			    drawString("ESSID : "+ essid , 20, row, txtColor);

			    row += 35;
			    drawString("time : "+ new Date().toString() , 20, row, txtColor);
			    row += 15;
			    drawString("time : "+ System.currentTimeMillis() , 20, row, txtColor);

			for(int i=0; i < 5; i++) {
			  if ( screenNum != 1 ) { break; }
			  Zzz(1000);
			}

		}
		*/
//      }
  //    if (!pollKbd) { System.in.read(); }
}	
	static void drawString(String msg, int x, int y, int color) {
		t.blitString(msg , x, y, color, true);
	}


	static void Zzz(long time) {
	  try { Thread.sleep(time); } catch(Exception ex) {}
	}
	
	
}