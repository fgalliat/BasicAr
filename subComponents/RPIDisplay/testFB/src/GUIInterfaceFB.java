// BEWARE : as uses .awt.Color / .awt.Image .. seems to need X11 started
import java.awt.Color;
import java.util.*;

import java.text.*; // SimpleDateFormat

import com.xtase.hardware.*;
import com.xtase.struct.*;

/**
 * HDMI Display on RPI for Xts-uBasic
 * 
 * @author Xtase - fgalliat @Dec-2017
 *
 *         Still missing : <br/>
 *         cursor <br/>
 *         reverse video text <br/>
 *
 */

public class GUIInterfaceFB {

  //static Bitmap offscreen = null;
  static boolean inRender = false;
  
  static boolean DO_ONLY_SINGLE_TEST = false;
  
	protected static TFTScreen dispSrv = null;

	protected static GUIInterfaceFB instance = null;

	protected static int TTY_WIDTH = 100;
	protected static int TTY_HEIGHT = 50;

	static SimpleDateFormat timeFormat = new SimpleDateFormat("hh:mm:ss");

	public static GUIInterfaceFB getInstance() {
		return instance;
	}

	public GUIInterfaceFB(TFTScreen dispSrv) {
		instance = this;

        //offscreen = Bitmap.createNew(  dispSrv.SCREEN_WIDTH, dispSrv.SCREEN_HEIGHT );
      
        System.out.println( dispSrv.SCREEN_WIDTH +"x"+ dispSrv.SCREEN_HEIGHT );
		GUIInterfaceFB.dispSrv = dispSrv;

		new TextWindow(0, 0, 0, TTY_WIDTH, TTY_HEIGHT, false, null);
		curWindow = 0; // no win (100x50 chars default main win)

      	// ralenti le deamrrage mais si pas d'autre fenetres 
        // definies -> necessaire ....
		setDirty(windows[0]);
      
   
      new Thread() {
        public void run() {
          while(true) {
            //if (screenIsDirty) { d_blit(); /*screenIsDirty = false;*/ }
            if (screenIsDirty) { 
              doRedraw(); 
              //d_blit(); 
              screenIsDirty = false;
            }
            Zzz(55);
          }
        }
      }.start();
      
	}

	// will be changeable later
	static Color FG = Color.green;
	static Color BG = Color.black;

	static int WHITE_RGB = Color.white.getRed();

  static int HERE_MODIFIER = 1;
  
  // !!!!!! needs @ least 800px height
  static int TXT_ZOOM = 2;
	static int ftHeight = (10-1)*TXT_ZOOM; // -1 is experimental !!!!
	static int ftWidth = (7 - HERE_MODIFIER) * TXT_ZOOM;

	// -------------------------------------------------
	boolean yetDrawn = false;
  
    static boolean screenIsDirty = false;

	protected synchronized void setDirty(TextWindow win) {
		if (!yetDrawn) {
			yetDrawn = true;

// BEWARE : no more cls
            //dispSrv.cls();
    //       fillRect(0,0,dispSrv.SCREEN_WIDTH,dispSrv.SCREEN_HEIGHT, BG.getRGB() );
          
            FGcolor = FG.getRGB();

			// System.out.print("YD ");
		}
      /*
		if (win.isAttached()) {
			win.render(this);
			// System.out.print("wr ");
		}
      */
      
      //d_blit();
      screenIsDirty = true;
	}

  boolean finishedRedraw = false;
  boolean askFinishRedraw = false;
  
  /*synchronized*/ void doRedraw() {
    
    if ( !finishedRedraw ) {
      askFinishRedraw = true;
    }
    
    
    finishedRedraw = false;
    TextWindow win;
    for (int i = 0; i < this.windows.length; i++) {
      win = this.windows[i];
      if (win != null && win.isAttached() && win.isDirty) {
        win.render(this);
        win.hasBeenRendered = true;
      }
      if ( askFinishRedraw ) { win.setDirty(); break; }
	}
	
	// =======================================
	//String time = timeFormat.format(new Date());
	String time = "cocou";
	d_drawString(time, (100-10)*ftWidth, 1*ftHeight, WHITE_RGB, true);
	// =======================================

    askFinishRedraw = false;
    finishedRedraw = true;
  }
  
  
	protected void setDirty() {
      /*
		TextWindow win;
		for (int i = 0; i < this.windows.length; i++) {
			win = this.windows[i];
			if (win != null && win.isAttached()) {
				setDirty(win);
			}
		}
        */
      //d_blit();
	}

	// -------------------------------------------------

	protected static class TextWindow {
		int x, y, w, h;
		int id = -1; // [0] 1-9
		boolean borders;
		String title;
		int xModifier = 0, yModifierTop = 0, yModifierBottom = 0;
      int yMaxWritten = -1;

      static char BLANK_CHAR = ' ';
      //static char BLANK_CHAR = 0x00;
      
		int cursX = 0;
		int cursY = 0;

		char[][] content = null;

		protected boolean isDirty = false;
		protected boolean isAttached = false;
      	protected boolean hasBeenRendered = false;
      

		int ttyWidth, ttyHeight;

		protected void setDirty() {
          setDirty(true);
        }
        protected void setDirty(boolean propagate) {
			this.isDirty = true;
			// .... ????
			if (propagate) GUIInterfaceFB.getInstance().setDirty(this);
		}

		protected boolean hasTitle() {
			return title != null;
		}

		public TextWindow(int id, int x, int y, int w, int h, boolean borders, String title) {
			this.id = id;

			this.x = x;
			this.y = y;
			this.w = w;
			this.h = h;
			this.borders = borders;
          
            this.yMaxWritten = this.h;
          
			if (title != null && title.isEmpty()) {
				title = null;
			}

			this.title = title;

			if (borders) {
				xModifier = 1;

				if (hasTitle()) {
					yModifierTop = 1 + 1 + 1;
				} else {
					yModifierTop = 1;
				}
				yModifierBottom = 1;
			}

			// border shrinks the text Frame
			ttyWidth = w - (xModifier * 2);
			ttyHeight = h - (yModifierTop + yModifierBottom);
			if (ttyWidth < 0) {
				ttyWidth = 0;
			}
			if (ttyHeight < 0) {
				ttyHeight = 0;
			}
			this.content = new char[ttyHeight][ttyWidth];

			GUIInterfaceFB.getInstance().windows[id] = this;

			attach();
            focus();
          
            System.out.println( "created win id#"+id );
		}
      

        // Cf here : text is topLeft driven (not bottommLeft driven)
        int TTY_Y_MODIFIER = 1;
        int TTY_X_MODIFIER = 1;

		protected void applyLineColor(GUIInterfaceFB iface, int y) {
            int color = Color.black.getRGB();
          
			if (iface.screenlines[this.y + y] == null) {
              color = BG.getRGB();
			} else {
              color = iface.screenlines[this.y + y].BG.getRGB();
              //color = (255<<24)+(r<<16) + (g << 8) + b ;
			}
          
          // AA RR GG BB
          // color = 0xFF00FF00;
          
            d_fillRect(this.x * ftWidth, (this.y + y+TTY_Y_MODIFIER) * ftHeight, (this.w+TTY_X_MODIFIER) * ftWidth, 1 * ftHeight, color);

			if (iface.screenlines[this.y + y] == null) {
              FGcolor = FG.getRGB();
			} else {
              FGcolor = iface.screenlines[this.y + y].FG.getRGB();
			}
		}

		public void render(GUIInterfaceFB iface) {
          GUIInterfaceFB.inRender = true;
			String line = "";

			if (!hasBeenRendered && borders) {
              //hasBeenRendered = true;
              
				// TODO : nicer borders
				line = "";
				for (int i = 0; i < this.w; i++) {
					line += "-";
				}
				// upper line
				applyLineColor(iface, 0);
				//iface.getGraphics().drawString(line, this.x * ftWidth, (this.y + 1) * ftHeight);
 				d_drawString(line, this.x * ftWidth, (this.y + 1) * ftHeight, FGcolor, false);

				// lower line
				applyLineColor(iface, (this.h - 1));
				//iface.getGraphics().drawString(line, this.x * ftWidth, ((this.y + this.h - 1) + 1) * ftHeight);
 				d_drawString(line, this.x * ftWidth, ((this.y + this.h - 1) + 1) * ftHeight, FGcolor, false);

				if (hasTitle()) {
					// under-title line
					applyLineColor(iface, (1 + 1));
					//iface.getGraphics().drawString(line, this.x * ftWidth, (this.y + 1 + 1 + 1) * ftHeight);
 					d_drawString(line, this.x * ftWidth, (this.y + 1 + 1 + 1) * ftHeight, FGcolor, false);

					// title line
					line = "|";
					int xx = ((this.w-2) - title.length()) / 2;
					
					// not centered by default
					//xx = 0;
					//for (int zz = 0; zz < xx; zz++) {
					//	line += BLANK_CHAR;
					//}
                  
					line += title;
					for (int zz = line.length(); zz < this.w - 1; zz++) {
						line += BLANK_CHAR;
					}
					line += "|";
					applyLineColor(iface, 1);
					//iface.getGraphics().drawString(line, this.x * ftWidth, (this.y + 1 + 1) * ftHeight);
   					d_drawString(line, this.x * ftWidth, (this.y + 1 + 1) * ftHeight, FGcolor, false);
				}
			}

			for (int yy = 0; yy < this.h - (yModifierTop + yModifierBottom); yy++) {
              
              if ( hasBeenRendered && yy > yMaxWritten ) { break; }
              
				char[] lineSeg = this.content[yy];
				line = "";
				if (borders) {
					line += "|";
				}
				for (int xx = 0; xx < this.w - (xModifier * 2); xx++) {
					char ch = lineSeg[xx];
					line += ch == 0x00 ? BLANK_CHAR : ch;
				}
				if (borders) {
					line += "|";
				}
				applyLineColor(iface, (yy + yModifierTop));
				//iface.getGraphics().drawString(line, this.x * ftWidth, (this.y + yy + yModifierTop + 1) * ftHeight);
   				d_drawString(line, this.x * ftWidth, (this.y + yy + yModifierTop + 1) * ftHeight, FGcolor, false);
			}
          
            //d_blit();
          
			isDirty = false;
          
          GUIInterfaceFB.inRender = false;
		}

		public boolean isAttached() {
			return this.isAttached;
		}

        public void attach() {
          isAttached = true;
          setDirty();
        }
      
		public void detach() {
			isAttached = false;
			setDirty();
		}

		public void cls() {
			for (int y = 0; y < this.ttyHeight; y++) {
				for (int x = 0; x < this.ttyWidth; x++) {
					// System.out.println("("+ id +") x="+x+", y="+y+" Vs w="+w+", h="+h);
					content[y][x] = BLANK_CHAR;
				}
			}
			cursX = 0;
			cursY = 0;
            //yMaxWritten = 0;
            this.yMaxWritten = this.h;
			setDirty();
		}

		public void print(String txt) {
			for (int i = 0; i < txt.length(); i++) {
				print(txt.charAt(i));
			}
			setDirty();
		}

		protected void print(char ch) {
			if (ch == '\r') {
				return;
			}
			if (ch == '\n') {
				br();
			} else {
				content[cursY][cursX] = ch;
				cursX++;
				if (cursX >= this.ttyWidth) {
					br();
				}
			}
			setDirty();
		}

		protected void br() {
			cursY++;
			cursX = 0;
          yMaxWritten = cursY;
			if (cursY >= this.ttyHeight) {
			  scrollUp();
              cursY--;
			}
		}

		protected void scrollUp() {
          for (int y = 1; y < this.ttyHeight; y++) {
            //this.content[y-1] = this.content[y];
            System.arraycopy( this.content[y], 0, this.content[y-1], 0, this.ttyWidth );
          }          
          for (int x = 0; x < this.ttyWidth; x++) {
            content[this.ttyHeight-1][x] = BLANK_CHAR;
          }
		}

		public void locate(int x2, int y2) {
			cursX = x2;
			cursY = y2;
          if ( cursY > yMaxWritten ) { yMaxWritten = cursY; }
		}

		public void unfocus() {
			// deal with cursor
		}

		public void focus() {
			// deal with cursor
		}
	}

	protected static class ColorLine {
		int y1, y2;
		Color FG, BG;

		public ColorLine(int y1, int y2, Color fg, Color bg) {
			this.y1 = y1;
			this.y2 = y1;
			this.FG = fg;
			this.BG = bg;

			for (int y = y1; y <= y2; y++) {
				GUIInterfaceFB.getInstance().screenlines[y] = this;
			}

			// TODO : fewer times
			
          	// BEWARE I rempoved this code
            GUIInterfaceFB.getInstance().screenIsDirty = true;
          	GUIInterfaceFB.getInstance().setDirty();
          /*
          for (int i = 0; i < GUIInterfaceFB.getInstance().windows.length; i++) {
			TextWindow win = GUIInterfaceFB.getInstance().windows[i];
			if (win != null && win.isAttached()) {
            	win.hasBeenRendered = false;
            	//win.isDirty = true;
                win.setDirty( !false );
            }
          }
          */
		}

		protected static Color parseColor(String str) {
			// '0' - '2'
			int r = toInt("" + str.charAt(0)) * 100;
			int g = toInt("" + str.charAt(1)) * 100;
			int b = toInt("" + str.charAt(2)) * 100;
			return new Color(r, g, b);
		}

		public static ColorLine fromStr(String str) {
			String[] tks = str.split("\\,");
			int y1 = toInt(tks[1]);
			int y2 = toInt(tks[2]);
			Color fg = parseColor(tks[3]);
			Color bg = parseColor(tks[4]);
			return new ColorLine(y1, y2, fg, bg);
		}

	}

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
    // Use a BufferedImage for window rendering then render text on FB
  
    // il y a 50 frames de 320x240
  	// static int FRAME_INDEX = 2+1;
    static int FGcolor = Color.green.getRGB();
  
    static int LOCAL_SCREEN_X = -1;
    static int LOCAL_SCREEN_Y = -1;
  
      static protected void d_fillRect(int x, int y, int w, int h, int color) {
        if ( LOCAL_SCREEN_X == -1 ) { 
          LOCAL_SCREEN_X = (TFTScreen.SCREEN_VIRTUAL_WIDTH-(TTY_WIDTH*ftWidth) )/2;
          LOCAL_SCREEN_Y = (TFTScreen.SCREEN_VIRTUAL_HEIGHT-(TTY_HEIGHT*ftHeight))/2;
        }
        
        /*
        int[] rgba = new int[ w*h ];
        for(int i=0; i < rgba.length; i++) { rgba[i] = color; }
        //dispSrv.blit(rgba, FRAME_INDEX, x, y, w, h);
        dispSrv.fillDirect(rgba, TFTScreen.SCREEN_VIRTUAL_X+LOCAL_SCREEN_X+x, TFTScreen.SCREEN_VIRTUAL_Y+LOCAL_SCREEN_Y+y, w, h);
        */
        dispSrv.fillColor(color, TFTScreen.SCREEN_VIRTUAL_X+LOCAL_SCREEN_X+x, TFTScreen.SCREEN_VIRTUAL_Y+LOCAL_SCREEN_Y+y, w, h);
        
        screenIsDirty = true;
      }
  
     static protected void d_drawString(String text, int x, int y, int color, boolean renderSpaces) {
       dispSrv.blitString(text, TFTScreen.SCREEN_VIRTUAL_X+LOCAL_SCREEN_X+x, TFTScreen.SCREEN_VIRTUAL_Y+LOCAL_SCREEN_Y+y, color, TXT_ZOOM, renderSpaces);
       screenIsDirty = true;
     }
    
     static synchronized void d_blit() {
       //if ( !GUIInterfaceFB.inRender ) {
       //  //dispSrv.resetFrame(FRAME_INDEX);
       //}
       //screenIsDirty = false;
     }
  
     // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
  
	protected TextWindow[] windows = new TextWindow[10];
	protected int curWindow = -1;

    // Cf Java 1.5
	//protected List<ColorLine> lines = new LinkedList<ColorLine>();
    protected java.util.List lines = new LinkedList();
  
	protected ColorLine[] screenlines = new ColorLine[50];

	// ################################################
	public boolean setup() {
		// +2 & +3 cf borders
		gui_createwin(1, 10 - 1, 10 - 5 - 2, 100 - 10 - 10 + 2, 50 - 10 - 10 + 3, true, "Main Window");

		gui_createwin(2, 10, 0, 100 - 10 - 10, 1, false, null);
		_gui_cmd("l,0,1,222,101", 2); // White text on purple lines 0-1
		gui_cls();
		gui_print("Xtase uBasic v1.0");

		gui_createwin(3, 10, 50 - 1, 100 - 10 - 10, 1, false, null);
		_gui_cmd("l,49,49,000,111", 2); // black text on grey line 49
		gui_cls();
		gui_print("Xtase - fgalliat @ 01/2017");

		gui_createwin(4, 10 - 1, 50 - 10 - 10 + 6 + 1, 100 - 10 - 10 + 2, 10 + 1, true, "Status");
		_gui_cmd("l,37,47,222,001", 2); // white text on blue lines 36-46

		// char window4_colour[] = {"l,35,48,222,001"}; // white text on blue lines
		// 35-48
		// char footer_colour[] = {"l,49,49,000,111"}; // black text on grey line 49

		gui_focuswin(1);
		gui_cls();

		return DO_ONLY_SINGLE_TEST;
	}

	void gui_cls() {
		cls();
	}

	void gui_focuswin(int id) {
		focus(id);
	}

	void _gui_cmd(String str, int timeWait) {
		submit("^[" + str);
	}

	void gui_print(String str) {
		print(str);
	}

	void gui_createwin(int id, int x, int y, int w, int h, boolean border, String title) {
		createWindow(id, x, y, w, h, border, title);
	}

	// ################################################
	public void test() {

		if (false && setup()) {
			return;
		}
		reboot();

		// "^[" + "r" -> reboot

		// // +2 & +3 cf borders
		// gui_createwin(1, 10 - 1, 10 - 5 - 2, 100 - 10 - 10 + 2, 50 - 10 - 10 +
		// 3, true, "Main Window");
		//
		// gui_createwin(2, 10, 0, 100 - 10 - 10, 1, false, NULL);
		// _gui_cmd("l,0,1,222,101", 2); // White text on purple lines 0-1
		// gui_cls();
		// gui_print("Xtase uBasic v1.0");
		//
		// gui_createwin(3, 10, 50 - 1, 100 - 10 - 10, 1, false, NULL);
		// _gui_cmd("l,49,49,000,111", 2); // black text on grey line 49
		// gui_cls();
		// gui_print("Xtase - fgalliat @ 01/2017");
		//
		// gui_createwin(4, 10 - 1, 50 - 10 - 10 + 6 + 1, 100 - 10 - 10 + 2, 10 +
		// 1, true, "Status");
		// _gui_cmd("l,37,47,222,001", 2); // white text on blue lines 36-46

		// Window Creation strings (without initial escape sequence)
		// String header = "w,1,0,0,100,3,1,"; // no title
		String header = "w,1,0,0,100,3,1,Xtase uBasic v1.0";
		String window2 = "w,2,0,3,70,32,1,Raw Data Display";
		String window3 = "w,3,70,3,30,32,1,Calculated Values";
		String window4 = "w,4,0,35,100,14,1,Terminal Input";
		String footer = "w,5,0,49,100,1,0,"; // no border, no title
		// String footer = "w,5,0,49,100,1,0,Xtase - fgalliat @ 01/2017"; // no
		// border

		// Line colour strings (without initial escape sequence)
		String header_colour = "l,0,2,222,101"; // White text on purple lines 0-2
		String window4_colour = "l,35,48,222,001"; // white text on blue lines 35-48
		String footer_colour = "l,49,49,000,111"; // black text on grey line 49

		String esc = "^[";

		submit(esc + header);
		submit(esc + window2);
		submit(esc + window3);
		submit(esc + window4);
		submit(esc + footer);

		submit(esc + header_colour);
		submit(esc + window4_colour);
		submit(esc + footer_colour);

		focus(5);
		cls();
		print("Xtase - fgalliat @ 01/2017");

		focus(4);
		cls();
		print("(EE) not enought memory");

		focus(2);
		cls();
		print("10 CLS\n20 PRINT 3+2\n>");

      // TMP Cf non refreshed colored windows (ex. upper win)
      //GUIInterfaceFB.getInstance().setDirty();
      //GUIInterfaceFB.getInstance().windows[1].setDirty();
	}

	// -----------------------------

  protected String tmpCmd = null;
  protected boolean waitFor2ndEscChar = false;
  protected boolean waitForCmd = false;
  
  protected boolean yetReceivedChar = false;
  
  public void submitChar(char ch) {
    //if ( !yetReceivedChar ) {
    //  yetReceivedChar = true;
    //  reboot();
    //  cls();
    //  setDirty();
    //}
    
    
    // CMDS do ends with an '\n'
    if ( ch == '^' && tmpCmd == null ) {
      tmpCmd = "^";
      waitFor2ndEscChar = true;
    } else if (waitFor2ndEscChar && ch == '[') {
      tmpCmd += "[";
      waitFor2ndEscChar = false;
      waitForCmd = true;
    } else if (waitFor2ndEscChar && ch != '[') {
      submit( tmpCmd );
      waitFor2ndEscChar = false;
      waitForCmd = false;
      tmpCmd = null;
    } else if (waitForCmd) {
      if ( ch == '\n' ) {
        submit( tmpCmd );
        waitFor2ndEscChar = false;
        waitForCmd = false;
        tmpCmd = null;
      } else if ( ch == '\r' ) {
        return;
      } else {
        tmpCmd += ch;
      }
    } else {
      //submit(""+ch); // very slow ...
      print(ch);
    }
    
  }
  
	public void submit(String expr) {
      //System.out.println(">>"+expr+"<<");
      
		if (expr.startsWith("^[")) {
			expr = expr.substring(2);
			if (expr.equals("r")) {
				reboot();
			} else if (expr.equals("halt")) {
				try { Runtime.getRuntime().exec("./haltScript.sh"); }
				catch(Exception ex) {
					ex.printStackTrace();
				}
			} else if (expr.equals("e")) {
				cls();
			} else if (expr.startsWith("p,")) {
				String[] tks = expr.split("\\,");
				locate(toInt(tks[1]), toInt(tks[2]));
			} else if (expr.startsWith("f,")) {
				String[] tks = expr.split("\\,");
				focus(toInt(tks[1]));
			} else if (expr.startsWith("l,")) {
				String[] tks = expr.split("\\,");
				// "l,49,49,000,111"; // black text on grey line 49
				// bckLine(toInt(tks[1]), toInt(tks[2]), tks[3], tks[4]);
				lines.add(ColorLine.fromStr("^[" + expr));
			} else if (expr.startsWith("w,")) {
                // BEWARE : that syntax is wrong !!!! 
				// "w,3,70,3,30,32,1,Calculated Values";
				String[] tks = expr.split("\\,");
				String title = tks.length > 7 && tks[7].trim().length() > 0 ? tks[7] : null;
				createWindow(toInt(tks[1]), toInt(tks[2]), toInt(tks[3]), toInt(tks[4]), toInt(tks[5]), toInt(tks[6]) == 1, title);
			} else if (expr.startsWith("w")) {
				// "w3,70,3,30,32,1,Calculated Values";
                // dirty strap
                expr = "?,"+expr.substring(1);
				String[] tks = expr.split("\\,");
				String title = tks.length > 7 && tks[7].trim().length() > 0 ? tks[7] : null;
				createWindow(toInt(tks[1]), toInt(tks[2]), toInt(tks[3]), toInt(tks[4]), toInt(tks[5]), toInt(tks[6]) == 1, title);
			}
			// STILL missing : cursor function
		} else {
			print(expr);
		}
	}

	// -----------------------------
	static void _(Object o) {
		System.out.println("VGAT:" + o);
	}

	// -----------------------------

	public void reboot() {
		_("I reboot ....");

		// we dont erase win[0] !!
		for (int i = 1; i < windows.length; i++) {
			if (windows[i] != null) {
				windows[i].detach();
				windows[i] = null;
			}
		}
		curWindow = 0;
		windows[0].cls();
		//windows[0].detach();
        windows[0].attach();

		lines.clear();
		for (int i = 0; i < screenlines.length; i++) {
			screenlines[i] = null;
		}

		_("I rebooted ....");
        setDirty();
	}

	public void cls() {
      if ( windows[curWindow] == null ) { curWindow=0; }
	  windows[curWindow].cls();
	}

	public void print(String txt) {
      if ( windows[curWindow] == null ) { curWindow=0; }
      windows[curWindow].print(txt);
	}
  
  	public void print(char ch) {
      if ( windows[curWindow] == null ) { curWindow=0; }
      windows[curWindow].print(ch);
	}

	public void locate(int x, int y) {
      if ( windows[curWindow] == null ) { curWindow=0; }
      windows[curWindow].locate(x, y);
	}

	// 1 to 9
	public void focus(int id) {
		windows[curWindow].unfocus();
		if (id < 1 || id > 9) {
			return;
		}
		curWindow = id;
        if ( windows[curWindow] != null ) {
          windows[curWindow].focus();
        } else {
          System.out.println("BEWARE win#"+ curWindow +" is null");
        }
	}

	public void createWindow(int id, int x, int y, int w, int h, boolean borders, String title) {
		if (id < 1 || id > 9) {
			return;
		}
		if (windows[id] != null) {
			// as the real board does !!!!
			return;
		}

		if (curWindow == 0) {
			// remove to toogle NOWIN Vs WINS
			windows[curWindow].detach();
		}
		windows[curWindow].unfocus();

		// windows[id] = new TextWindow(x, y, w, h, borders, title);
		new TextWindow(id, x, y, w, h, borders, title);
		curWindow = id;

		// does auto focus ?????
		windows[id].focus();
	}

	static int toInt(String str) {
		try {
			return Integer.parseInt(str);
		} catch (Exception ex) {
			return -1;
		}
	}

 	static void Zzz(long time) {
	  try { Thread.sleep(time); } catch(Exception ex) {}
	}

  
}