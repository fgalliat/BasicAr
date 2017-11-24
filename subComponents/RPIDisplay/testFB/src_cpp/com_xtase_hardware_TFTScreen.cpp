#include <jni.h>

// in /usr/include/ & provides rand()
#include <stdlib.h>

// string.h -> memset
#include <string.h>


#include "FrameBuffer.h"
#include "com_xtase_hardware_TFTScreen.h"

 // not FrameBuffer fbDriver() if no args
 FrameBuffer fbDriver; 

/*
 * Class:     Test
 * Method:    openFb
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_xtase_hardware_TFTScreen_openFb
  (JNIEnv *env, jobject, jstring jstr) {

     const char *str = (env)->GetStringUTFChars( jstr, 0);
   // notice the avoidance of *env & 1st arg .....
   // const char *nativeString = (*env)->GetStringUTFChars(env, javaString, 0);
   // after use : (*env)->ReleaseStringUTFChars(env, javaString, nativeString);


 return (jint)fbDriver.openFb(str);
}

/*
 * Class:     Test
 * Method:    closeFb
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_closeFb
  (JNIEnv *, jobject) {
 fbDriver.closeFb();
}



// 3.84MB
unsigned int cache[50][320*240];
unsigned int cacheIdx[50];


  
JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_resetFrame
  (JNIEnv *env, jobject, jint index) {
  cacheIdx[(int)index] = 0;
}
  
/*
 * Class:     Test
 * Method:    fill
 * Signature: ([IIIII)V
 */
JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_fill
  (JNIEnv *env, jobject, jintArray bgrArray, jint index, jint x, jint y, jint width, jint height) {


  if ( bgrArray == NULL ) {
    return;
  }

  if ( cacheIdx[(int)index] == 0 ) {
    cacheIdx[(int)index] = 1;

    jsize len = env->GetArrayLength(bgrArray);
    //unsigned int rgb[len];
    jint *body = env->GetIntArrayElements(bgrArray,0);
    ///cache[(int)index] = int[320*240];
    for (jint i = 0; i < len; ++i){
      cache[(int)index][i] = body[i];
    }
  }
  fbDriver.fill(cache[(int)index], (int)x, (int) y, (int)width, (int)height);  


//  fbDriver.blit(env,bgrArray, (int)x, (int) y, (int)width, (int)height);

  //env->ReleastIntArrayElements(arr, body, 0);

(env)->DeleteLocalRef( bgrArray ); 

}
JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_fillColor
  (JNIEnv *env, jobject, jint color, jint x, jint y, jint width, jint height) {
/*
  const int sz = ((int)width*(int)height);
  int colArry[ sz ];
  //memset( colArry, color, sz); // ONLY deals w/ bytes
  for(int i=0; i < sz; i++) { colArry[i] = color; } // need int(s)

  fbDriver.fill( (unsigned int*)colArry, (int)x, (int) y, (int)width, (int)height);  
*/

  for(int yy=0; yy < height; yy++) {
    for(int xx=0; xx < width; xx++) {
      fbDriver.blitPixel( xx+x, yy+y, color );
    }
  }

}


JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_fillDirect
  (JNIEnv *env, jobject, jintArray bgrArray, jint x, jint y, jint width, jint height) {


  if ( bgrArray == NULL ) {
    return;
  }

  //jsize len = env->GetArrayLength(bgrArray);
  //jint *body = env->GetIntArrayElements(bgrArray,0);

//  fbDriver.fill( (unsigned int*)body, (int)x, (int) y, (int)width, (int)height);  
  fbDriver.blit( env, bgrArray, (int)x, (int) y, (int)width, (int)height);  


  //free(body);
  //env->ReleastIntArrayElements(arr, body, 0);

(env)->DeleteLocalRef( bgrArray ); 

}

/*
 * Class:     Test
 * Method:    drawString
 * Signature: ([IILjava/lang/String;III)V
 */
JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_drawString
  (JNIEnv *env, jobject, jstring str, jint x, jint y, jint color, jint zoom, jboolean renderSpaces) {

  if ( str == NULL ) {
    return;
  }

  const char *_str = (env)->GetStringUTFChars( str, 0);

  // TODO COlor
  fbDriver.dispStrColor(_str, x, y, (int)color, zoom, renderSpaces);

 (env)->DeleteLocalRef( str ); 

}

JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_blitPixel
  (JNIEnv *env, jobject, jint x, jint y, jint color) {
   fbDriver.blitPixel((int)x,(int)y,(int)color);
}



/*
 * Class:     Test
 * Method:    fillAll
 * Signature: ([I)V
 */
JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_fillAll
  (JNIEnv *, jobject, jintArray) {
  fbDriver.fillAll(NULL);
}

// ################################
// ############  FX  ##############
// ################################

// ## StarField FX ##

 int speedZ = 2;
 double speed = 1.5;

 int BLACKY = 0xFF000000;

 float WIDTH = 320.0;
 float HEIGHT = 240.0;

 float MI_WIDTH = WIDTH/2.0;
 float MI_HEIGHT = HEIGHT/2.0;
 // --------------
 const int nbStars = 1500;

 int coltable[256];
 double starx[nbStars];
 double stary[nbStars];
 double starz[nbStars];

 bool first = true;

float rnd() {
  return (float)(rand() % 320) / 320.0;
}

unsigned int cls_mask[320*240];

void tempCls() {
  if ( cls_mask[0] == 0 ) {
   int i;
   for(i=0; i < 320*240; i++) { cls_mask[i] = 0xFF000000; }
  }
  fbDriver.fill(cls_mask,0,0,320,240);
}




JNIEXPORT void JNICALL Java_com_xtase_hardware_TFTScreen_FX_1starField
  (JNIEnv *, jobject) {

  if ( first ) {
    for(int i = 0; i < nbStars; i++) {
      starx[i] = WIDTH * rnd() - MI_WIDTH;
      stary[i] = HEIGHT * rnd() - MI_HEIGHT;
      starz[i] = 255.0 * rnd();
    }

    for(int j = 0; j < 256; j++) {
      coltable[j] = 0xff000000 + (255 - j << 16) + (255 - j << 8) + (255 - j);
    }
    first = false;
  }

  float d  = -1.0;
  float d1 = -1.0;
  float i1 = -1.0;
  float j1 = -1.0;

    //tempCls();


  for(int j = 0; j < nbStars; j++) {

    // -- erase old stars --
    
    d = (starx[j] * 256.0) / starz[j];
    d1 = (stary[j] * 256.0) / starz[j];
    d += MI_WIDTH;
    d1 += MI_HEIGHT;
    i1 = (int)d;
    j1 = (int)d1;

    if(i1 >= 0 && i1 < (int)WIDTH-1 && j1 >= 0 && j1 < (int)HEIGHT-1) {
      for(int k = 0; k < 2; k++) {
        for(int l = 0; l < 2; l++) {
          //pix[(j1 + l) * (int)WIDTH + i1 + k] = musta;
          fbDriver.blitPixel( i1+k, (j1 + l), BLACKY );
        }
      }
    }
    

    // -- draw new stars --
    starz[j]-=speedZ;
    if(starz[j] < 1.0) {
      starz[j] = 255.0;
      starx[j] = WIDTH * (rnd()*speed) - MI_WIDTH;
      stary[j] = HEIGHT * (rnd()*speed) - MI_HEIGHT;
    }
    d = (starx[j] * 256.0) / starz[j];
    d1 = (stary[j] * 256.0) / starz[j];
    d += MI_WIDTH;
    d1 += MI_HEIGHT;
    i1 = (int)d;
    j1 = (int)d1;

    if(i1 >= 0 && i1 < (int)WIDTH-1 && j1 >= 0 && j1 < (int)HEIGHT-1) {
      for(int k = 0; k < 2; k++) {
        for(int l = 0; l < 2; l++) {
          //pix[(j1 + l) * (int)WIDTH + i1 + k] = coltable[(int)starz[j]];
          fbDriver.blitPixel( i1+k, (j1 + l), coltable[(int)starz[j]] );
        }
      }
    }

  } // for nbStars


}

// ##################