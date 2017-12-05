package com.xtase.hardware;

public class FX_Starfield {

	private static int musta = 0xFF000000;
  private static int coltable[];
  private static double starx[];
  private static double stary[];
  private static double starz[];

  static double WIDTH = 320D;
  static double HEIGHT = 240D;

  static double MI_WIDTH = WIDTH/2D;
  static double MI_HEIGHT = HEIGHT/2D;

  static int nbStars = 1500;
  static boolean first = true;

	public static synchronized void drawFX() {
		if ( first ) {
			starx = new double[nbStars];
      stary = new double[nbStars];
      starz = new double[nbStars];
      for(int i = 0; i < nbStars; i++) {
        starx[i] = WIDTH * Math.random() - MI_WIDTH;
        stary[i] = HEIGHT * Math.random() - MI_HEIGHT;
        starz[i] = 255D * Math.random();
      }

      coltable = new int[256];
      for(int j = 0; j < 256; j++) {
        coltable[j] = 0xff000000 + (255 - j << 16) + (255 - j << 8) + (255 - j);
      }
      first = false;
		}

		TFTScreen tft = TFTScreen.getInstance();

    for(int j = 0; j < nbStars; j++) {
      // -- erase old stars --
      double d = (starx[j] * 256D) / starz[j];
      double d1 = (stary[j] * 256D) / starz[j];
      d += MI_WIDTH;
      d1 += MI_HEIGHT;
      int i1 = (int)d;
      int j1 = (int)d1;
      if(i1 >= 0 && i1 < (int)WIDTH-1 && j1 >= 0 && j1 < (int)HEIGHT-1) {
        for(int k = 0; k < 2; k++) {
          for(int l = 0; l < 2; l++) {
            tft.blitPixel(i1 + k, (j1 + l)  , musta );
          }
        }
      }

      // -- draw new stars --
	    starz[j]--;
      if(starz[j] < 1.0D) {
         starz[j] = 255D;
         starx[j] = WIDTH * Math.random() - MI_WIDTH;
         stary[j] = HEIGHT * Math.random() - MI_HEIGHT;
      }
      d = (starx[j] * 256D) / starz[j];
      d1 = (stary[j] * 256D) / starz[j];
      d += MI_WIDTH;
      d1 += MI_HEIGHT;
      i1 = (int)d;
      j1 = (int)d1;
      if(i1 >= 0 && i1 < (int)WIDTH-1 && j1 >= 0 && j1 < (int)HEIGHT-1) {
        for(int k = 0; k < 2; k++) {
          for(int l = 0; l < 2; l++) {
            //pix[(j1 + l) * (int)WIDTH + i1 + k] = coltable[(int)starz[j]];
            tft.blitPixel(i1 + k, (j1 + l)  ,  coltable[(int)starz[j]] );
          }
        }
      }
    }

	}


}