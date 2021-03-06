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

  static int isBEndian = -1;

  static bool isBigEndian(){
    if ( isBEndian > -1 ) { return isBEndian == 1; } 

    int number = 1;
    isBEndian = (*(char*)&number != 1) ? 1 : 0;
    return isBEndian == 1;
  }


 // ===================================
 // Bytes Maipulation 

  static float getFloatFromBytes(unsigned char* memSeg, int address) {
    char bytes[sizeof(float)];

    if ( address > MEMORY_SIZE || address < 0 ) {
        DBUG_NOLN( "(EE) Tried to parse float from MEM:", address );
        return -1.0;
    }

    //DBUG_NOLN( "Parsing float from MEM : ", address );

    if( isBigEndian() ){
       for(int i=0;i<sizeof(float);i++) {
          bytes[sizeof(float)-i] = memSeg[address+i];
        //   DBUG_NOLN( (int)memSeg[address+i] );
        //   DBUG_NOLN( " " );
        }
    }
    else{
       for(int i=0;i<sizeof(float);i++) {
          bytes[i] = memSeg[address+i];
        //   DBUG_NOLN( (int)memSeg[address+i] );
        //   DBUG_NOLN( " " );
        }
    }
    //DBUG_NOLN( " -end\n" );
    float result;
    memcpy(&result, bytes, sizeof(float));
    //DBUG_NOLN( "FINISHED\n" );
    return result;
 }

 static int getSizeOfFloat() { return sizeof(float); }

 static void copyFloatToBytes(unsigned char* memSeg, int address, float f) {

    // DBUG_NOLN("copyFloatToBytes "); DBUG( address );
    if ( address > MEMORY_SIZE || address < 0 ) {
        DBUG_NOLN( "(EE) Tried to write float to MEM:", address );
        return;
    }

    int float_tlen = sizeof(float); // 4 bytes on Arduino
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

    // DBUG_NOLN("copyFloatToBytes "); DBUG( "END" );
 }

 #ifdef mem_routines

    extern unsigned char mem[];

    // ex: x = mem2float(p,mem);
    static float mem2float(unsigned char* cur, unsigned char* src) {
        int addr = &(cur[0]) - &(src[0]);
        return getFloatFromBytes(mem, addr);
    }

    // ex: x = float2mem(p,mem, 3.14);
    static void float2mem(unsigned char* cur, unsigned char* src, float value) {
        int addr = &(cur[0]) - &(src[0]);
        copyFloatToBytes(mem, addr, value);
    }

 #endif