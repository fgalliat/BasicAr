#ifndef ARDUINO_COMPAT
#define ARDUINO_COMPAT 1

  #ifdef ARDUINO_ARCH_ESP32
    #define BUT_ESP32 1
  #endif


  #ifndef SPI_FREQUENCY
    // on mcu#2 setted to 40000000 ?
    #define SPI_FREQUENCY  20000000
  #endif


  #ifdef COMPUTER
    // for usleep
    #include <unistd.h>
    typedef unsigned char uint8_t; 
  #else
    #include <Arduino.h>
    #include <SPI.h>

    // -- SPI SCreen - values to copy in TFT_eSPI/User_Select.h
    #define TFT_MISO 19
    #define TFT_MOSI 23
    #define TFT_CLK  18
    #define TFT_CS    5  // Chip select control pin
    #define TFT_DC   15  // Data Command control pin
    // connected to EN pin
    #define TFT_RST   -1

  #define BUILTIN_LED 2
  #define BUILTIN_BTN 0

  #endif

  #define PROGMEM 

  static uint8_t _xts_pgm_read_byte(uint8_t* base, int offset) {
      // line = pgm_read_byte(font+(c*5)+i);
      return base[ offset ];
  }

  static uint8_t _xts_pgm_read_byte(const uint8_t* base, int offset) {
      return _xts_pgm_read_byte( (uint8_t*) base, offset);
  }

  #ifdef COMPUTER
    static void _delay_ms(long millis) {
        usleep( millis * 1000 );
    }

    static void _delay_us(long micros) {
        usleep( micros );
    }
  #else
    static void _delay_ms(long millis) {
        delay( millis );
    }

    static void _delay_us(long micros) {
        delayMicroseconds( micros );
    }
  #endif

#endif