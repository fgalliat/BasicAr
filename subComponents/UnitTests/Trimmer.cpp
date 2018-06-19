#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DBUG(a) { printf("(ii) %s\n", a); }
#define DBUG(a) { ; }

// remember to free() result ....
char* str_trim(char* str) {
  if ( str == NULL ) { return NULL; }
  //int start = 0, stop = strlen( str )-1; // REMOVED '-1'
  int start = 0, stop = strlen( str );

  for(int i=0; i <= stop; i++) {
    if ( str[ i ] > 32 ) {
      start = i;
      break;
    }
  }

  printf("start=%d / stop=%d\n", start, stop);

  if ( start == stop ) { char* empty = (char*)malloc(0+1); empty[0]=0x00; return empty; }

  for(int i=stop; i >= 0; i--) {
    if ( str[ i ] > 32 ) {
      stop = i;
      break;
    }
  }

  // printf("start=%d stop=%d \n", start, stop);

  char* result = (char*)malloc( (stop-start) +1);
  int cpt=0;
  for(int i=start; i <= stop; i++) {
    result[cpt++] = str[i];
  }
  result[ cpt ] = 0x00;

  return result;
}


int main(int argc, char** argv) {

    printf("Hello World \n");

    // OK
    //char* initString = (char*)" Hello World\n\t ";
    //char* initString = (char*)"   \n  \t   Hello World";
    //char* initString = (char*)"Hello World";
    //char* initString = (char*)"";
    //char* initString = (char*)"Hello World  ";
    //char* initString = (char*)"   Hello World";
    //char* initString = (char*)"3\n";
    char* initString = (char*)"3";

    char* token;
    token = str_trim( initString );
    if ( token == NULL ) { token = (char*)"NULL"; }
    printf(" >%s< \n", token);

    printf("ByeBye World \n");

return 0;
}

