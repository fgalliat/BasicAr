/****************
 *  Teensy PROGMEM managment layer
 * 
 * XTase - fgalliat @Sept 2017
 * **************/

#ifndef _XTS_TEENSY_H_
#define _XTS_TEENSY_H_ 1

 // define of nothing ....
 #define PROGMEM

//  unsigned char pgm_read_byte_near(const void* mem_addr) {
//     char t = *((char *) mem_addr);
//     if ( t < 0 ) { t += 256; }
//     mem_addr++;
//     return (unsigned char)t;
//  }

// unsigned char pgm_read_byte(const void* mem_addr) {
//     char t = *((char *) mem_addr);
//     if ( t < 0 ) { t += 256; }
//     return (unsigned char)t;
// }

// #define pgm_read_byte(x) (*(x))

// #include "basic.h"

// TokenTableEntry* pgm_read_word(const void* mem_addr) {
//     return (TokenTableEntry*)mem_addr;
// }


// #define pgm_read_byte(addr)     (*reinterpret_cast<const uint8_t*>(addr))
// #define pgm_read_word(addr) (*reinterpret_cast<const uint16_t*>(addr))
// #define pgm_read_byte_near(addr) pgm_read_byte(addr)

#endif