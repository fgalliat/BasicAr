/***************
 * Mem Segment routines
 * Xtase - fgalliat @ Oct2017
***************/

// #define PROGMEM 

//   unsigned char pgm_read_byte_near(const void* mem_addr) {
//     char t = *((char *) mem_addr);
//     if ( t < 0 ) { t += 256; }
//     return (unsigned char)t;
//   }

  bool isBEndian = -1;

  bool isBigEndian(){
    if ( isBEndian > -1 ) { return isBEndian == 1; } 

    int number = 1;
    isBEndian = (*(char*)&number != 1) ? 1 : 0;
    return isBEndian == 1;
  }


 // ===================================
 // Bytes Maipulation 

  float getFloatFromBytes(unsigned char* memSeg, int address) {
    char bytes[sizeof(float)];

    if(isBigEndian){
       for(int i=0;i<sizeof(float);i++) {
          bytes[sizeof(float)-i] = memSeg[address+i];
        }
    }
    else{
       for(int i=0;i<sizeof(float);i++) {
          bytes[i] = memSeg[address+i];
        }
    }
    float result;
    memcpy(&result, bytes, sizeof(float));
    return result;
 }

 void copyFloatToBytes(unsigned char* memSeg, int address, float f) {
    int float_tlen = sizeof(float);
    char data[float_tlen];
    memcpy(data, &f, sizeof f);    // send data
    bool bigEndian = isBigEndian();
    for(int i=0; i < float_tlen; i++) {
        if ( bigEndian ) {
            //                                       v ne manque t'il pas un -1 ???
            memSeg[ address+i ] = data[float_tlen - i];
        }
        else {
            memSeg[ address+i ] = data[i];
        }
    }
 }
