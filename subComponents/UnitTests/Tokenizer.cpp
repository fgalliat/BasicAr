#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DBUG(a) { printf("(ii) %s\n", a); }
#define DBUG(a) { ; }


  char* nextSplit( char* remaining, int fullLen, char delim, bool treatEscapes ) {
    // reads remaining, fill w/ 0x00 readed chars then copy those to result; 
    int start = -1, stop = -1;
    DBUG("1");
    for(int i=0; i < fullLen; i++) {
      if ( remaining[i] == 0x00 ) { continue; }
      if ( start == -1 ) { start = i; }

      if ( remaining[i] == '\\' && i+1 < fullLen && remaining[i+1] == delim && treatEscapes ) {
        i++;
        continue;
      }

      if ( remaining[i] == delim ) {
        stop = i+1;
        break;
      }
    }
    // reached end of line ?
    if ( start == -1 ) { return NULL; }

    DBUG("2");
    if ( stop == -1 ) { stop = fullLen; }
    //printf("start=%d stop=%d \n", start, stop);

    if ( stop - start <= 0 ) { DBUG("eject 3"); return NULL; }
    else {
      DBUG("4");

      char* result = (char*)malloc( stop-start-1+1 );

      DBUG("5");
      //printf("start=%d stop=%d \n", start, stop);

      int cpt=0;
      for(int i=start; i <= stop; i++) {
        //printf(" 5.1 > %d [%c] \n", i, remaining[i]);

        if ( remaining[i] == '\\' && i+1 < fullLen && remaining[i+1] == delim && treatEscapes ) {
          // nothing
          DBUG(" 5.2");
        } else if ( remaining[i] == delim && i == stop-1 ) {
          DBUG(" 5.3");
          remaining[i] = 0x00; // erase readed chars
          break;
        } else {
          DBUG(" 5.4");
          result[ cpt++ ] = remaining[i]; 
        }
        DBUG(" 5.5");
        remaining[i] = 0x00; // erase readed chars
        DBUG(" 5.6");
      }

      DBUG("6");
      result[ cpt ] = 0x00;

      DBUG("7");

      return result;
    }

    DBUG("eject 8");

    return NULL;
  }


int main(int argc, char** argv) {

    printf("Hello World \n");

    // doesn't allow initString modification (causes segmentation faults)

    // OK
    // char* initString = (char*)"Rolph;10.1;25;T\\;o\\\"to\\;;23";

    // OK
    //char* initString = (char*)"Rolph;10.1;25;T\\;o\\\"to\\;;;23";

    // OK
    char* initString = (char*)"Rolph;10.1;25;T\\;o\\\"to\\;;;2;3;;";

    int fullLen = strlen( initString );

    char* varString = (char*)malloc( fullLen+1 );
    memcpy(varString, initString, fullLen);
    varString[fullLen] = 0;

    char delim = ';';
    bool treatEscapes = true;

    char* token;

    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);


    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    // should be null !!!!!!
    token = nextSplit( varString, fullLen, delim, treatEscapes );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    printf("ByeBye World \n");

return 0;
}

