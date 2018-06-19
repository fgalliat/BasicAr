/****************
 * Xtase - fgalliat @ Sept2017
 * stdlib compat. layer 
 ***************/



#ifndef __XTS_COMPAT_H_
#define __XTS_COMPAT_H_ 1



 char * dtostre(
    double __val,           // value
    char * __s,             // dest string 
    unsigned char __prec,   // precision
    unsigned char __flags) {    // ???
  
          sprintf( __s, "%f", __val );
          return __s;
  }

#ifndef ARDUINO_ARCH_ESP32

 char * dtostrf(
    double __val,           // value
    signed char __width,    // width (? padding ?)
    unsigned char __prec,   // precision
      char * __s) {           // dest string
          
          // TODO : width
          sprintf( __s, "%f", __val );
          return __s;
  }

#endif

#endif