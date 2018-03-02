/* ---------------------------------------------------------------------------
 * Basic Interpreter
 *
 * Modified by : Xtase - fgalliat @Sept2017
 * 
 * Original code : Robin Edwards 2014
 * ---------------------------------------------------------------------------
 * This BASIC is modelled on Sinclair BASIC for the ZX81 and ZX Spectrum. It
 * should be capable of running most of the examples in the manual for both
 * machines, with the exception of anything machine specific (i.e. graphics,
 * sound & system variables).
 *
 * Notes
 *  - All numbers (except line numbers) are floats internally
 *  - Multiple commands are allowed per line, seperated by :
 *  - LET is optional e.g. LET a = 6: b = 7
 *  - MOD provides the modulo operator which was missing from Sinclair BASIC.
 *     Both numbers are first rounded to ints e.g. 5 mod 2 = 1
 *  - CONT can be used to continue from a STOP. It does not continue from any
 *     other error condition.
 *  - Arrays can be any dimension. There is no single char limit to names.
 *  - Like Sinclair BASIC, DIM a(10) and LET a = 5 refer to different 'a's.
 *     One is a simple variable, the other is an array. There is no ambiguity
 *     since the one being referred to is always clear from the context.
 *  - String arrays differ from Sinclair BASIC. DIM a$(5,5) makes an array
 *     of 25 strings, which can be any length. e.g. LET a$(1,1)="long string"
 *  - functions like LEN, require brackets e.g. LEN(a$)
 *  - String manipulation functions are LEFT$, MID$, RIGHT$
 *  - RND is a nonary operator not a function i.e. RND not RND()
 *  - PRINT AT x,y ... is replaced by POSITION x,y : PRINT ...
 *  - LIST takes an optional start and end e.g. LIST 1,100 or LIST 50
 *  - INKEY$ reads the last key pressed from the keyboard, or an empty string
 *     if no key pressed. The (single key) buffer is emptied after the call.
 *     e.g. a$ = INKEY$
 *  - LOAD/SAVE load and save the current program to the EEPROM (1k limit).
 *     SAVE+ will set the auto-run flag, which loads the program automatically
 *     on boot. With a filename e.g. SAVE "test" saves to an external EEPROM.
 *  - DIR/DELETE "filename" - list and remove files from external EEPROM.
 *  - PINMODE <pin>, <mode> - sets the pin mode (0=input, 1=output, 2=pullup)
 *  - PIN <pin>, <state> - sets the pin high (non zero) or low (zero)
 *  - PINREAD(pin) returns pin value, ANALOGRD(pin) for analog pins
 * ---------------------------------------------------------------------------
 */

// TODO
// ABS, SIN, COS, EXP etc
// DATA, READ, RESTORE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#include "xts_arch.h"
#ifdef BUT_TEENSY
  //#include "xts_teensy.h"

#ifdef BUT_ESP32
 #ifdef ESP32PCKv2
    extern Esp32Pocketv2 esp32;

 #ifdef ESP32_WIFI_SUPPORT
     extern void host_outputString(char* str);
     extern int host_outputInt(long v);

     #define DBUG(a) { Serial.print(a); host_outputString(a); }
     #define DBUGi(a) { Serial.print(a); host_outputInt(a); }

     #include "Esp32WifiServer.h"
     extern Esp32WifiServer telnet;
     
     #undef DBUG
     #undef DBUGi
 #endif

 #else
   extern Esp32Oled esp32;
 #endif

#endif

  #ifndef COMPUTER
    #include "xts_teensy.h"
  #else 
    #include "computer.h"
  #endif



#else
  #include <Arduino.h>
#endif

#include "basic.h"
#include "host.h"

//#include <avr/pgmspace.h>

// Xtase
#include "mem_utils.h"

// -------- Xtase refacto -------------
char executeMode;
int curToken;
extern bool LOCAL_ECHO;
#include "xtase_fct.h"
// ------------------------------------


int sysPROGEND;
int sysSTACKSTART, sysSTACKEND;
int sysVARSTART, sysVAREND;
int sysGOSUBSTART, sysGOSUBEND;

const char string_0[]  = "OK";
const char string_1[]  = "Bad number";
const char string_2[]  = "Line too long";
const char string_3[]  = "Unexpected input";
const char string_4[]  = "Unterminated string";
const char string_5[]  = "Missing bracket";
const char string_6[]  = "Error in expr";
const char string_7[]  = "Numeric expr expected";
const char string_8[]  = "String expr expected";
const char string_9[]  = "Line number too big";
const char string_10[]  = "Out of memory";
const char string_11[]  = "Div by zero";
const char string_12[]  = "Variable not found";
const char string_13[]  = "Bad command";
const char string_14[]  = "Bad line number";
const char string_15[]  = "Break pressed";
const char string_16[]  = "NEXT without FOR";
const char string_17[]  = "STOP statement";
const char string_18[]  = "Missing THEN in IF";
const char string_19[]  = "RETURN without GOSUB";
const char string_20[]  = "Wrong array dims";
const char string_21[]  = "Bad array index";
const char string_22[]  = "Bad string index";
const char string_23[]  = "Error in VAL input";
const char string_24[]  = "Bad parameter";

// const char *errorTable[] = {
const char* errorTable[]  = {
    string_0, string_1, string_2, string_3,
    string_4, string_5, string_6, string_7,
    string_8, string_9, string_10, string_11,
    string_12, string_13, string_14, string_15,
    string_16, string_17, string_18, string_19,
    string_20, string_21, string_22, string_23,
    string_24
};

// // Token flags
// // bits 1+2 number of arguments
// #define TKN_ARGS_NUM_MASK	0x03
// // bit 3 return type (set if string)
// #define TKN_RET_TYPE_STR	0x04
// // bits 4-6 argument type (set if string)
// #define TKN_ARG1_TYPE_STR	0x08
// #define TKN_ARG2_TYPE_STR	0x10
// #define TKN_ARG3_TYPE_STR	0x20

// #define TKN_ARG_MASK		0x38
// #define TKN_ARG_SHIFT		3
// // bits 7,8 formatting
// #define TKN_FMT_POST		0x40
// #define TKN_FMT_PRE		0x80


 //const TokenTableEntry tokenTable[] = {
TokenTableEntry tokenTable[] = {
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {"(", 0}, {")",0}, {"+",0}, {"-",0},
    {"*",0}, {"/",0}, {"=",0}, {">",0},
    {"<",0}, {"<>",0}, {">=",0}, {"<=",0},
    {":",TKN_FMT_POST}, {";",0}, {",",0}, {"AND",TKN_FMT_PRE|TKN_FMT_POST},
    {"OR",TKN_FMT_PRE|TKN_FMT_POST}, {"NOT",TKN_FMT_POST}, {"PRINT",TKN_FMT_POST}, {"LET",TKN_FMT_POST},
    {"LIST",TKN_FMT_POST}, {"RUN",TKN_FMT_POST}, {"GOTO",TKN_FMT_POST}, {"REM",TKN_FMT_POST},
    {"STOP",TKN_FMT_POST}, {"INPUT",TKN_FMT_POST},  {"CONT",TKN_FMT_POST}, {"IF",TKN_FMT_POST},
    {"THEN",TKN_FMT_PRE|TKN_FMT_POST}, {"LEN",1|TKN_ARG1_TYPE_STR}, {"VAL",1|TKN_ARG1_TYPE_STR}, {"RND",0},
    {"INT",1}, {"STR$", 1|TKN_RET_TYPE_STR}, {"FOR",TKN_FMT_POST}, {"TO",TKN_FMT_PRE|TKN_FMT_POST},
    {"STEP",TKN_FMT_PRE|TKN_FMT_POST}, {"NEXT", TKN_FMT_POST}, {"MOD",TKN_FMT_PRE|TKN_FMT_POST}, {"NEW",TKN_FMT_POST},
    {"GOSUB",TKN_FMT_POST}, {"RETURN",TKN_FMT_POST}, {"DIM", TKN_FMT_POST}, {"LEFT$",2|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR},
    {"RIGHT$",2|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR}, {"MID$",3|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR}, {"CLS",TKN_FMT_POST}, {"PAUSE",TKN_FMT_POST},
    {"POSITION", TKN_FMT_POST},  {"PIN",TKN_FMT_POST}, {"PINMODE", TKN_FMT_POST}, {"INKEY$", 0},
    {"SAVE", TKN_FMT_POST}, {"LOAD", TKN_FMT_POST}, {"PINREAD",1}, {"ANALOGRD",1},
    {"DIR", TKN_FMT_POST}, {"DELETE", TKN_FMT_POST},

    // ------ Xtase routines -----------
    {"MEM",0}, {"?",TKN_FMT_POST}, {"'",TKN_FMT_POST},
    {"LOCATE",2|TKN_FMT_POST}, // Oups : there was a POSITION cmd ...
    {"LED",2}, // to switch on/off a led
    {"BEEP",2}, {"MUTE", 0},
    {"PLAY",1|TKN_ARG1_TYPE_STR},
    {"PLAYT5K",1|TKN_ARG1_TYPE_STR|TKN_FMT_POST},
    {"PLAYT53",1|TKN_ARG1_TYPE_STR|TKN_FMT_POST},

    {"BYE",TKN_FMT_POST},

    {"BTN",1}, // to read btn state

    {"ECHO",TKN_FMT_POST}, // to (un)lock local echo

    {"DELAY",TKN_FMT_POST}, // to sleep MCU

    {"CONSOLE", 0|TKN_FMT_POST}, // change current I/O console ==> TODO : add args to select devices

    {"LLIST",TKN_FMT_POST}, // to dump current/fromSD PRGM to Serial console

    {"DRAWBPP",1|TKN_ARG1_TYPE_STR|TKN_FMT_POST},

    {"SECS", 0}, // returns nb of seconds since boot time

    {"CHAIN",TKN_FMT_POST}, // to load then execute a program

    {"CIRCLE", 3|TKN_FMT_POST},
    {"LINE", 4|TKN_FMT_POST},
    {"PSET", 2|TKN_FMT_POST},    // switch ON a pixel
    {"PRESET", 2|TKN_FMT_POST},  // switch OFF a pixel

    {"STRING$", 2|TKN_RET_TYPE_STR},  // repeat x times CHR$(y)
    {"UPPER$", 1|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR},   // returns upper str
    {"SPACE$", 1|TKN_RET_TYPE_STR},  // repeat x times CHR$(32)

    {"EXEC", TKN_FMT_POST},  // execute an extended command

    {"ASC", 1|TKN_ARG1_TYPE_STR},  // returns ascii code of 1st char of string
    {"INSTR", 2|TKN_ARG1_TYPE_STR|TKN_ARG2_TYPE_STR},  // returns ascii code of 1st char of string
    {"MILLIS", 0}, // returns nb of milli-seconds since boot time

    {"DIRARRAY", TKN_FMT_POST}, // DIR -> redirected to DIR$() array variable

    {"HALT",TKN_FMT_POST}, // halt the whole System

    {"ABS", 1},  // returns ABS(x)
    {"COS", 1},  // returns COS(x) in radian
    {"SIN", 1},  // returns SIN(x) in radian

    {"SQRT", 1}, // numeric function
    {"POW",  2},

    {"RECT",  6|TKN_FMT_POST}, // rect x,y,w,h[,color[,mode]]
    {"BLITT", 1|TKN_FMT_POST}, // {0,1,2}

    {"DATAF", TKN_FMT_POST},  // DATAF "<file>","<sizeVar>","<ARRAY_VAR>"[,"<ARRAY_VAR>","<ARRAY_VAR>"...]

    {"DRAWPCT",3|TKN_ARG1_TYPE_STR|TKN_FMT_POST}, // DRAWPCT "TEST",0,0

};


/* **************************************************************************
 * PROGRAM FUNCTIONS
 * **************************************************************************/
void printTokens(unsigned char *p) {
    int modeREM = 0;
    while (*p != TOKEN_EOL) {
        if (*p == TOKEN_IDENT) {
            p++;
            while (*p < 0x80)
                host_outputChar(*p++);
            host_outputChar((*p++)-0x80);
        }
        else if (*p == TOKEN_NUMBER) {
            p++;
            host_outputFloat(*(float*)p);
            p+=4;
        }
        else if (*p == TOKEN_INTEGER) {
            p++;
            host_outputInt(*(long*)p);
            p+=4;
        }
        else if (*p == TOKEN_STRING) {
            p++;
            if (modeREM) {
                host_outputString((char*)p);
                p+=1 + strlen((char*)p);
            }
            else {
                host_outputChar('\"');
                while (*p) {
                    if (*p == '\"') host_outputChar('\"');
                    host_outputChar(*p++);
                }
                host_outputChar('\"');
                p++;
            }
        }
        else {
            //uint8_t fmt = pgm_read_byte_near(&tokenTable[*p].format);
            uint8_t fmt = tokenTable[*p].format;

            if (fmt & TKN_FMT_PRE)
                host_outputChar(' ');

            //host_outputString((char *)pgm_read_word(&tokenTable[*p].token));
            host_outputString((char *)tokenTable[*p].token);

            if (fmt & TKN_FMT_POST)
                host_outputChar(' ');
            if (*p==TOKEN_REM || *p==TOKEN_REM_EM)
                modeREM = 1;
            p++;
        }
    }
}

void listProg(uint16_t first, uint16_t last) {
    unsigned char *p = &mem[0];
    while (p < &mem[sysPROGEND]) {
        uint16_t lineNum = *(uint16_t*)(p+2);
        if ( (p >= &mem[sysPROGEND]) ) { host_outputString("OUT_OF_MEM.a\n"); break; }
        if ((!first || lineNum >= first) && (!last || lineNum <= last)) {
            host_outputInt(lineNum);
            host_outputChar(' ');
            printTokens(p+4);
            host_newLine();
        }
        p+= *(uint16_t *)p;
        if ( (p >= &mem[sysPROGEND]) ) { host_outputString("OUT_OF_MEM.b\n"); break; }
    }
}

unsigned char *findProgLine(uint16_t targetLineNumber) {
    unsigned char *p = &mem[0];
    while (p < &mem[sysPROGEND]) {
        uint16_t lineNum = *(uint16_t*)(p+2);
        if (lineNum >= targetLineNumber)
            break;
        p+= *(uint16_t *)p;
    }
    return p;
}

void deleteProgLine(unsigned char *p) {
    uint16_t lineLen = *(uint16_t*)p;
    sysPROGEND -= lineLen;
    memmove(p, p+lineLen, &mem[sysPROGEND] - p);
}

int doProgLine(uint16_t lineNumber, unsigned char* tokenPtr, int tokensLength)
{
    // find line of the at or immediately after the number
    unsigned char *p = findProgLine(lineNumber);
    uint16_t foundLine = 0;
    if (p < &mem[sysPROGEND])
        foundLine = *(uint16_t*)(p+2);
    // if there's a line matching this one - delete it
    if (foundLine == lineNumber)
        deleteProgLine(p);
    // now check to see if this is an empty line, if so don't insert it
    if (*tokenPtr == TOKEN_EOL)
        return 1;
    // we now need to insert the new line at p
    int bytesNeeded = 4 + tokensLength;	// length, linenum + tokens
    if (sysPROGEND + bytesNeeded > sysVARSTART)
        return 0;
    // make room if this isn't the last line
    if (foundLine)
        memmove(p + bytesNeeded, p, &mem[sysPROGEND] - p);
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *(uint16_t *)p = lineNumber; 
    p += 2;
    memcpy(p, tokenPtr, tokensLength);
    sysPROGEND += bytesNeeded;
    return 1;
}

/* **************************************************************************
 * CALCULATOR STACK FUNCTIONS
 * **************************************************************************/

// Calculator stack starts at the start of memory after the program
// and grows towards the end
// contains either floats or null-terminated strings with the length on the end

int stackPushNum(float val) {
    if (sysSTACKEND + sizeof(float) > sysVARSTART)
        return 0;	// out of memory
    unsigned char *p = &mem[sysSTACKEND];
    
    // orginal code : can cause deadlocks
    // ... Teensy 32b don't like that
    //*(float *)p = val;
    copyFloatToBytes(mem, sysSTACKEND, val);


    sysSTACKEND += sizeof(float);
    return 1;
}
float stackPopNum() {
    // DBUG("Ent.stackPopNum->", sysSTACKEND);
    sysSTACKEND -= sizeof(float);
    // DBUG("stackPopNum->", sysSTACKEND);

    if ( sysSTACKEND < 0 ) {
        Serial.println("ALARM 0x03");
        return 0.0f;
    }


    //unsigned char *p = &mem[sysSTACKEND];

    // DBUG("stackPopNum A->",(int)mem[sysSTACKEND+0]);
    // DBUG("stackPopNum B->",(int)mem[sysSTACKEND+1]);
    // DBUG("stackPopNum C->",(int)mem[sysSTACKEND+2]);
    // DBUG("stackPopNum D->",(int)mem[sysSTACKEND+3]);


    // orginal code : can cause deadlocks
    // ... Teensy 32b don't like that
    //float result = *(float *)p;
    float result = getFloatFromBytes( mem, sysSTACKEND ); // Xtase replacement

    // DBUG("Ex.stackPopNum", (int)result);
    return result;
}
int stackPushStr(char *str) {
    int len = 1 + strlen(str);
    if (sysSTACKEND + len + 2 > sysVARSTART)
        return 0;	// out of memory
    unsigned char *p = &mem[sysSTACKEND];
    strcpy((char*)p, str);
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
    return 1;
}
char *stackGetStr() {
    // returns string without popping it
    unsigned char *p = &mem[sysSTACKEND];
    int len = *(uint16_t *)(p-2);
    return (char *)(p-len-2);
}
char *stackPopStr() {
    // host_outputString("____popStr enter");
    // host_outputInt(sysSTACKEND);
    // host_outputChar('\n');

    // Xtase DBUG
    // if ( sysSTACKEND < 0 ) {
    //     host_outputString("0X0A ALARM.1 !!!");
    //     host_showBuffer();
    // }

    unsigned char *p = &mem[sysSTACKEND];
    int len = *(uint16_t *)(p-2);
    sysSTACKEND -= (len+2);

    // Xtase DBUG -- BEWARE w/ this !!!!!!
    // if ( sysSTACKEND < 0 ) {
    //     host_outputString("0X0A ALARM.2 !!!\n");
    //     // host_showBuffer();
    // }

    // host_outputString("____popStr quit");
    // host_outputInt(sysSTACKEND);
    // host_outputChar('\n');

    return (char *)&mem[sysSTACKEND];
}

void stackAdd2Strs() {
    // equivalent to popping 2 strings, concatenating them and pushing the result
    unsigned char *p = &mem[sysSTACKEND];
    int str2len = *(uint16_t *)(p-2);
    sysSTACKEND -= (str2len+2);
    char *str2 = (char*)&mem[sysSTACKEND];
    p = &mem[sysSTACKEND];
    int str1len = *(uint16_t *)(p-2);
    sysSTACKEND -= (str1len+2);
    char *str1 = (char*)&mem[sysSTACKEND];
    p = &mem[sysSTACKEND];
    // shift the second string up (overwriting the null terminator of the first string)
    memmove(str1 + str1len - 1, str2, str2len);
    // write the length and update stackend
    int newLen = str1len + str2len - 1;
    p += newLen;
    *(uint16_t *)p = newLen;
    sysSTACKEND += newLen + 2;
}

// mode 0 = LEFT$, 1 = RIGHT$
void stackLeftOrRightStr(int len, int mode) {
    // equivalent to popping the current string, doing the operation then pushing it again
    unsigned char *p = &mem[sysSTACKEND];
    int strlen = *(uint16_t *)(p-2);
    len++; // include trailing null
    if (len > strlen) len = strlen;
    if (len == strlen) return;	// nothing to do
    sysSTACKEND -= (strlen+2);
    p = &mem[sysSTACKEND];
    if (mode == 0) {
        // truncate the string on the stack
        *(p+len-1) = 0;
    }
    else {
        // copy the rightmost characters
        memmove(p, p + strlen - len, len);
    }
    // write the length and update stackend
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
}

void stackMidStr(int start, int len) {
    // equivalent to popping the current string, doing the operation then pushing it again
    unsigned char *p = &mem[sysSTACKEND];
    int strlen = *(uint16_t *)(p-2);
    len++; // include trailing null
    if (start > strlen) start = strlen;
    start--;	// basic strings start at 1
    if (start + len > strlen) len = strlen - start;
    if (len == strlen) return;	// nothing to do
    sysSTACKEND -= (strlen+2);
    p = &mem[sysSTACKEND];
    // copy the characters
    memmove(p, p + start, len-1);
    *(p+len-1) = 0;
    // write the length and update stackend
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
}

/* **************************************************************************
 * VARIABLE TABLE FUNCTIONS
 * **************************************************************************/

// Variable table starts at the end of memory and grows towards the start
// Simple variable
// table +--------+-------+-----------------+-----------------+ . . .
//  <--- | len    | type  | name            | value           |
// grows | 2bytes | 1byte | null terminated | float/string    | 
//       +--------+-------+-----------------+-----------------+ . . .
//
// Array
// +--------+-------+-----------------+----------+-------+ . . .+-------+-------------+. . 
// | len    | type  | name            | num dims | dim1  |      | dimN  | elem(1,..1) |
// | 2bytes | 1byte | null terminated | 2bytes   | 2bytes|      | 2bytes| float       |
// +--------+-------+-----------------+----------+-------+ . . .+-------+-------------+. . 

// variable type byte
#define VAR_TYPE_NUM		0x1
#define VAR_TYPE_FORNEXT	0x2
#define VAR_TYPE_NUM_ARRAY	0x4
#define VAR_TYPE_STRING		0x8
#define VAR_TYPE_STR_ARRAY	0x10

unsigned char *findVariable(char *searchName, int searchMask) {
    unsigned char *p = &mem[sysVARSTART];
    while (p < &mem[sysVAREND]) {
        int type = *(p+2);
        if (type & searchMask) {
            unsigned char *name = p+3;

            // DBUG_NOLN("Var scan : >");
            // DBUG_NOLN((char*)name);
            // DBUG("<");

            if (strcasecmp((char*)name, searchName) == 0)
                return p;
        }
        p+= *(uint16_t *)p;
    }

    // DBUG("Var not found : ");
    // DBUG(searchName);
    return NULL;
}

void deleteVariableAt(unsigned char *pos) {
    int len = *(uint16_t *)pos;
    if (pos == &mem[sysVARSTART]) {
        sysVARSTART += len;
        return;
    }
    memmove(&mem[sysVARSTART] + len, &mem[sysVARSTART], pos - &mem[sysVARSTART]);
    sysVARSTART += len;
}

// todo - consistently return errors rather than 1 or 0?

int storeNumVariable(char *name, float val) {

    // DBUG("registering num var");
    // DBUG( name );

    // these can be modified in place
    int nameLen = strlen(name);
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);

    int addr = &(p[0]) - &(mem[0]);

    if (p != NULL)
    {	// replace the old value
        // (could either be VAR_TYPE_NUM or VAR_TYPE_FORNEXT)
        p += 3;	// len + type;
        p += nameLen + 1;
        
        addr += 3 + nameLen + 1;

        // Xtase mod
        // *(float *)p = val;
        copyFloatToBytes( mem, addr, val );

    }
    else
    {	// allocate a new variable
        int bytesNeeded = 3;	// len + flags
        bytesNeeded += nameLen + 1;	// name
        bytesNeeded += sizeof(float);	// val

        // DBUG("bytes req. : ", bytesNeeded);

        if (sysVARSTART - bytesNeeded < sysSTACKEND) {
            DBUG("new int var : MEM OVERFLOW");
            return 0;	// out of memory
        }
        sysVARSTART -= bytesNeeded;
        // DBUG("bytes va start : ", sysVARSTART);

        int addr = sysVARSTART;
        unsigned char *p = &mem[sysVARSTART];
        *(uint16_t *)p = bytesNeeded; 
        p += 2; addr+=2;
        *p++ = VAR_TYPE_NUM; addr++;
        strcpy((char*)p, name); 
        p += nameLen + 1; addr += nameLen + 1;

        // Xtase mod
        //*(float *)p = val;
        copyFloatToBytes( mem, addr, val );

        // DBUG("registered num var");
        // DBUG( name );
        // for(int i=0; i < bytesNeeded; i++) {
        //     DBUG_NOLN( mem[sysVARSTART+i] ); DBUG_NOLN( " " );
        // }

    }

    // DBUG_NOLN( "\n" );

    return 1;
}

int storeForNextVariable(char *name, float start, float step, float end, uint16_t lineNum, uint16_t stmtNum) {
    int nameLen = strlen(name);
    int bytesNeeded = 3;	// len + flags
    bytesNeeded += nameLen + 1;	// name
    bytesNeeded += 3 * sizeof(float);	// vals
    bytesNeeded += 2 * sizeof(uint16_t);

    // unlike simple numeric variables, these are reallocated if they already exist
    // since the existing value might be a simple variable or a for/next variable
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);
    if (p != NULL) {
        // check there will actually be room for the new value
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND)
            return 0;	// not enough memory
        deleteVariableAt(p);
    }

    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory
    sysVARSTART -= bytesNeeded;

    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = VAR_TYPE_FORNEXT;
    strcpy((char*)p, name); 
    p += nameLen + 1;

    //*(float *)p = start; 
    float2mem(p, mem, start);

    p += sizeof(float);
    //*(float *)p = step; 
    float2mem(p, mem, step);

    p += sizeof(float);
    //*(float *)p = end; 
    float2mem(p, mem, end);

    p += sizeof(float);
    *(uint16_t *)p = lineNum; 
    p += sizeof(uint16_t);
    *(uint16_t *)p = stmtNum;
    return 1;
}

int storeStrVariable(char *name, char *val) {
    int nameLen = strlen(name);
    int valLen = strlen(val);
    int bytesNeeded = 3;	// len + type
    bytesNeeded += nameLen + 1;	// name
    bytesNeeded += valLen + 1;	// val

    // strings and arrays are re-allocated if they already exist
    unsigned char *p = findVariable(name, VAR_TYPE_STRING);
    if (p != NULL) {
        // check there will actually be room for the new value
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND)
            return 0;	// not enough memory
        deleteVariableAt(p);
    }

    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory
    sysVARSTART -= bytesNeeded;

    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = VAR_TYPE_STRING;
    strcpy((char*)p, name); 
    p += nameLen + 1;
    strcpy((char*)p, val);
    return 1;
}

int createArray(char *name, int isString) {
    // dimensions and number of dimensions on the calculator stack
    int nameLen = strlen(name);
    int bytesNeeded = 3;	// len + flags
    bytesNeeded += nameLen + 1;	// name
    bytesNeeded += 2;		// num dims
    int numElements = 1;
    int i = 0;
    int numDims = (int)stackPopNum();
    // keep the current stack position, since we'll need to pop these values again
    int oldSTACKEND = sysSTACKEND;	
    for (int i=0; i<numDims; i++) {
        int dim = (int)stackPopNum();
        numElements *= dim;
    }
    bytesNeeded += 2 * numDims + (isString ? 1 : sizeof(float)) * numElements;
    // strings and arrays are re-allocated if they already exist
    unsigned char *p = findVariable(name, (isString ? VAR_TYPE_STR_ARRAY : VAR_TYPE_NUM_ARRAY));
    if (p != NULL) {
        // check there will actually be room for the new value
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND)
            return 0;	// not enough memory
        deleteVariableAt(p);
    }

    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory
    sysVARSTART -= bytesNeeded;

    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = (isString ? VAR_TYPE_STR_ARRAY : VAR_TYPE_NUM_ARRAY);
    strcpy((char*)p, name); 
    p += nameLen + 1;
    *(uint16_t *)p = numDims; 
    p += 2;
    sysSTACKEND = oldSTACKEND;
    for (int i=0; i<numDims; i++) {
        int dim = (int)stackPopNum();
        *(uint16_t *)p = dim; 
        p += 2;
    }
    memset(p, 0, numElements * (isString ? 1 : sizeof(float)));
    return 1;
}

// ======================= Xtase code ==================
// can only create 1-dim arrays @ this time
int xts_createArray(char *name, int isString, int dimension) {
    int nameLen = strlen(name);
    int bytesNeeded = 3;	// len + flags
    bytesNeeded += nameLen + 1;	// name
    bytesNeeded += 2;		// num dims
    int numElements = 1;
    int i = 0;
    int numDims = 1; // HARDCODED
    
    int oldSTACKEND = sysSTACKEND;	
    numElements = dimension;

    bytesNeeded += 2 * numDims + (isString ? 1 : sizeof(float)) * numElements;
    // strings and arrays are re-allocated if they already exist
    unsigned char *p = findVariable(name, (isString ? VAR_TYPE_STR_ARRAY : VAR_TYPE_NUM_ARRAY));
    if (p != NULL) {
        // check there will actually be room for the new value
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND)
            return 0;	// not enough memory
        deleteVariableAt(p);
    }

    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory
    sysVARSTART -= bytesNeeded;

    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = (isString ? VAR_TYPE_STR_ARRAY : VAR_TYPE_NUM_ARRAY);
    strcpy((char*)p, name); 
    p += nameLen + 1;
    *(uint16_t *)p = numDims; 
    p += 2;
    sysSTACKEND = oldSTACKEND;

    // for (int i=0; i<numDims; i++) {
    //     int dim = (int)stackPopNum();
    //     *(uint16_t *)p = dim; 
    //     p += 2;
    // }
    *(uint16_t *)p = dimension; 
    p += 2;

    memset(p, 0, numElements * (isString ? 1 : sizeof(float)));
    return 1;
}
// =====================================================

int _getArrayElemOffset(unsigned char **p, int *pOffset) {
    // check for correct dimensionality
    int numArrayDims = *(uint16_t*)*p; 
    *p+=2;
    int numDimsGiven = (int)stackPopNum();
    if (numArrayDims != numDimsGiven)
        return ERROR_WRONG_ARRAY_DIMENSIONS;
    // now lookup the element
    int offset = 0;
    int base = 1;
    for (int i=0; i<numArrayDims; i++) {
        int index = (int)stackPopNum();
        int arrayDim = *(uint16_t*)*p; 
        *p+=2;
        if (index < 1 || index > arrayDim)
            return ERROR_ARRAY_SUBSCRIPT_OUT_RANGE;
        offset += base * (index-1);
        base *= arrayDim;
    }
    *pOffset = offset;
    return 0;
}

int setNumArrayElem(char *name, float val) {
    // each index and number of dimensions on the calculator stack
    unsigned char *p = findVariable(name, VAR_TYPE_NUM_ARRAY);
    if (p == NULL)
        return ERROR_VARIABLE_NOT_FOUND;
    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) return ret;
    
    p += sizeof(float)*offset;

    // Xtase mod
    int addr = &(p[0]) - &(mem[0]);
    copyFloatToBytes( mem, addr,val );
    //*(float *)p = val;

    return ERROR_NONE;
}

int setStrArrayElem(char *name) {
    // string is top of the stack
    // each index and number of dimensions on the calculator stack

    // keep the current stack position, since we can't overwrite the value string
    int oldSTACKEND = sysSTACKEND;
    // how long is the new value?
    char *newValPtr = stackPopStr();
    int newValLen = strlen(newValPtr);

    unsigned char *p = findVariable(name, VAR_TYPE_STR_ARRAY);
    unsigned char *p1 = p;	// so we can correct the length when done
    if (p == NULL)
        return ERROR_VARIABLE_NOT_FOUND;

    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) return ret;
    
    // find the correct element by skipping over null terminators
    int i = 0;
    while (i < offset) {
        if (*p == 0) i++;
        p++;
    }
    int oldValLen = strlen((char*)p);
    int bytesNeeded = newValLen - oldValLen;
    // check if we've got enough room for the new value
    if (sysVARSTART - bytesNeeded < oldSTACKEND)
        return 0;	// out of memory
    // correct the length of the variable
    *(uint16_t*)p1 += bytesNeeded;
    memmove(&mem[sysVARSTART - bytesNeeded], &mem[sysVARSTART], p - &mem[sysVARSTART]);
    // copy in the new value
    strcpy((char*)(p - bytesNeeded), newValPtr);
    sysVARSTART -= bytesNeeded;
    return ERROR_NONE;
}

// ======================= Xtase code ==================
// can ONLY treat 1-dim arrays @ this time
int xts_getArrayElemOffset(unsigned char **p, int *pOffset, int _index ) {
    // check for correct dimensionality
    int numArrayDims = *(uint16_t*)*p; 
    *p+=2;

    //int numDimsGiven = (int)stackPopNum();
    int numDimsGiven = 1; // HARDCODED

    if (numArrayDims != numDimsGiven)
        return ERROR_WRONG_ARRAY_DIMENSIONS;
    // now lookup the element
    int offset = 0;
    int base = 1;
    for (int i=0; i<numArrayDims; i++) {

        // except that the numDim is 1
        // int index = (int)stackPopNum();
        int index = _index;

        int arrayDim = *(uint16_t*)*p; 
        *p+=2;
        if (index < 1 || index > arrayDim)
            return ERROR_ARRAY_SUBSCRIPT_OUT_RANGE;
        offset += base * (index-1);
        base *= arrayDim;
    }
    *pOffset = offset;
    return 0;
}


int xts_setStrArrayElem(char *name, int index, char* value) {
    // string is top of the stack
    // each index and number of dimensions on the calculator stack

    // keep the current stack position, since we can't overwrite the value string
    int oldSTACKEND = sysSTACKEND;

    // how long is the new value?
    // char *newValPtr = stackPopStr();
    char *newValPtr = value;
    int newValLen = strlen(newValPtr);

    unsigned char *p = findVariable(name, VAR_TYPE_STR_ARRAY);
    unsigned char *p1 = p;	// so we can correct the length when done
    if (p == NULL)
        return ERROR_VARIABLE_NOT_FOUND;

    p += 3 + strlen(name) + 1;
    
    int offset;
    //int ret = _getArrayElemOffset(&p, &offset);
    int ret = xts_getArrayElemOffset(&p, &offset, index);
    if (ret) return ret;
    
    // find the correct element by skipping over null terminators
    int i = 0;
    while (i < offset) {
        if (*p == 0) i++;
        p++;
    }
    int oldValLen = strlen((char*)p);
    int bytesNeeded = newValLen - oldValLen;
    // check if we've got enough room for the new value
    if (sysVARSTART - bytesNeeded < oldSTACKEND)
        return 0;	// out of memory
    // correct the length of the variable
    *(uint16_t*)p1 += bytesNeeded;
    memmove(&mem[sysVARSTART - bytesNeeded], &mem[sysVARSTART], p - &mem[sysVARSTART]);
    // copy in the new value
    strcpy((char*)(p - bytesNeeded), newValPtr);
    sysVARSTART -= bytesNeeded;
    return ERROR_NONE;
}

//XX-EXPERIMENTAL-XX
int xts_setNumArrayElem(char *name, int index, float value) {
    // string is top of the stack
    // each index and number of dimensions on the calculator stack

    // keep the current stack position, since we can't overwrite the value string
    int oldSTACKEND = sysSTACKEND;

    // how long is the new value?
    // char *newValPtr = stackPopStr();
    //char *newValPtr = value;
    int newValLen = sizeof(float);//strlen(newValPtr);

    unsigned char *p = findVariable(name, VAR_TYPE_STR_ARRAY);
    unsigned char *p1 = p;	// so we can correct the length when done
    if (p == NULL)
        return ERROR_VARIABLE_NOT_FOUND;

    p += 3 + strlen(name) + 1;
    
    int offset;
    //int ret = _getArrayElemOffset(&p, &offset);
    int ret = xts_getArrayElemOffset(&p, &offset, index);
    if (ret) return ret;
    
    p += sizeof(float)*offset;

    // Xtase mod
    int addr = &(p[0]) - &(mem[0]);
    
    int oldValLen = sizeof(float);
    int bytesNeeded = newValLen - oldValLen;
    // check if we've got enough room for the new value
    if (sysVARSTART - bytesNeeded < oldSTACKEND)
        return 0;	// out of memory

    // correct the length of the variable
    *(uint16_t*)p1 += bytesNeeded;
    memmove(&mem[sysVARSTART - bytesNeeded], &mem[sysVARSTART], p - &mem[sysVARSTART]);
    
    // copy in the new value
    copyFloatToBytes( mem, addr,value );

    sysVARSTART -= bytesNeeded;
    return ERROR_NONE;
}

// =====================================================



float lookupNumArrayElem(char *name, int *error) {
    // each index and number of dimensions on the calculator stack
    unsigned char *p = findVariable(name, VAR_TYPE_NUM_ARRAY);
    if (p == NULL) {
        *error = ERROR_VARIABLE_NOT_FOUND;
        return 0.0f;
    }
    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) {
        *error = ret;
        return 0.0f;
    }
    p += sizeof(float)*offset;
    // Xtase mod
    int addr = &(p[0]) - &(mem[0]);
    return getFloatFromBytes( mem, addr );
    // return *(float *)p;
}

char *lookupStrArrayElem(char *name, int *error) {
    // each index and number of dimensions on the calculator stack
    unsigned char *p = findVariable(name, VAR_TYPE_STR_ARRAY);
    if (p == NULL) {
        *error = ERROR_VARIABLE_NOT_FOUND;
        return NULL;
    }
    p += 3 + strlen(name) + 1;

    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) {
        *error = ret;
        return NULL;
    }
    // find the correct element by skipping over null terminators
    int i = 0;
    while (i < offset) {
        if (*p == 0) i++;
        p++;
    }
    return (char *)p;
}

float lookupNumVariable(char *name) {
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);
    if (p == NULL) {
        return FLT_MAX;
    }
    p += 3 + strlen(name) + 1;

    // Xtase mod
    int addr = &(p[0]) - &(mem[0]);
    return getFloatFromBytes( mem, addr );
    //return *(float *)p;
}

char *lookupStrVariable(char *name) {
    unsigned char *p = findVariable(name, VAR_TYPE_STRING);
    if (p == NULL) {
        return NULL;
    }
    p += 3 + strlen(name) + 1;
    return (char *)p;
}

ForNextData lookupForNextVariable(char *name) {
    ForNextData ret;
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);
    if (p == NULL)
        ret.val = FLT_MAX;
    else if (*(p+2) != VAR_TYPE_FORNEXT)
        ret.step = FLT_MAX;
    else {
        p += 3 + strlen(name) + 1;
        ret.val = *(float *)p; 
        p += sizeof(float);
        ret.step = *(float *)p; 
        p += sizeof(float);
        ret.end = *(float *)p; 
        p += sizeof(float);
        ret.lineNumber = *(uint16_t *)p; 
        p += sizeof(uint16_t);
        ret.stmtNumber = *(uint16_t *)p;
    }
    return ret;
}

/* **************************************************************************
 * GOSUB STACK
 * **************************************************************************/
// gosub stack (if used) is after the variables
int gosubStackPush(int lineNumber,int stmtNumber) {
    int bytesNeeded = 2 * sizeof(uint16_t);
    if (sysVARSTART - bytesNeeded < sysSTACKEND)
        return 0;	// out of memory
    // shift the variable table
    memmove(&mem[sysVARSTART]-bytesNeeded, &mem[sysVARSTART], sysVAREND-sysVARSTART);
    sysVARSTART -= bytesNeeded;
    sysVAREND -= bytesNeeded;
    // push the return address
    sysGOSUBSTART = sysVAREND;
    uint16_t *p = (uint16_t*)&mem[sysGOSUBSTART];
    *p++ = (uint16_t)lineNumber;
    *p = (uint16_t)stmtNumber;
    return 1;
}

int gosubStackPop(int *lineNumber, int *stmtNumber) {
    if (sysGOSUBSTART == sysGOSUBEND)
        return 0;
    uint16_t *p = (uint16_t*)&mem[sysGOSUBSTART];
    *lineNumber = (int)*p++;
    *stmtNumber = (int)*p;
    int bytesFreed = 2 * sizeof(uint16_t);
    // shift the variable table
    memmove(&mem[sysVARSTART]+bytesFreed, &mem[sysVARSTART], sysVAREND-sysVARSTART);
    sysVARSTART += bytesFreed;
    sysVAREND += bytesFreed;
    sysGOSUBSTART = sysVAREND;
    return 1;
}

/* **************************************************************************
 * LEXER
 * **************************************************************************/

static unsigned char *tokenIn, *tokenOut;
static int tokenOutLeft;

// nextToken returns -1 for end of input, 0 for success, +ve number = error code
int nextToken()
{
    // Skip any whitespace.
    while (isspace(*tokenIn))
        tokenIn++;
    // check for end of line
    if (*tokenIn == 0) {
        *tokenOut++ = TOKEN_EOL;
        tokenOutLeft--;
        return -1;
    }
    // Number: [0-9.]+
    // TODO - handle 1e4 etc
    if (isdigit(*tokenIn) || *tokenIn == '.') {   // Number: [0-9.]+
        int gotDecimal = 0;
        char numStr[MAX_NUMBER_LEN+1];
        int numLen = 0;
        do {
            if (numLen == MAX_NUMBER_LEN) return ERROR_LEXER_BAD_NUM;
            if (*tokenIn == '.') {
                if (gotDecimal) return ERROR_LEXER_BAD_NUM;
                else gotDecimal = 1;
            }
            numStr[numLen++] = *tokenIn++;
        } 
        while (isdigit(*tokenIn) || *tokenIn == '.');

        numStr[numLen] = 0;
        if (tokenOutLeft <= 5) return ERROR_LEXER_TOO_LONG;
        tokenOutLeft -= 5;
        if (!gotDecimal) {
            long val = strtol(numStr, 0, 10);
            if (val == LONG_MAX || val == LONG_MIN)
                gotDecimal = true;
            else {
                *tokenOut++ = TOKEN_INTEGER;
                *(long*)tokenOut = (long)val;
                tokenOut += sizeof(long);
            }
        }
        if (gotDecimal)
        {
            *tokenOut++ = TOKEN_NUMBER;
            *(float*)tokenOut = (float)strtod(numStr, 0);
            tokenOut += sizeof(float);
        }
        return 0;
    }
    // identifier: [a-zA-Z][a-zA-Z0-9]*[$]
    // [?] -> PRINT // ['] -> REM
    if (isalpha(*tokenIn) || (*tokenIn) == '?' || (*tokenIn) == '\'' ) {
        char identStr[MAX_IDENT_LEN+1];
        int identLen = 0;
        identStr[identLen++] = *tokenIn++; // copy first char
        while (isalnum(*tokenIn) || *tokenIn=='$') {
            if (identLen < MAX_IDENT_LEN)
                identStr[identLen++] = *tokenIn;
            tokenIn++;
        }
        identStr[identLen] = 0;
        // check to see if this is a keyword
        for (int i = FIRST_IDENT_TOKEN; i <= LAST_IDENT_TOKEN; i++) {
            
            //if (strcasecmp(identStr, (char *)pgm_read_word(&tokenTable[i].token)) == 0) {
            if (strcasecmp(identStr, (char *)tokenTable[i].token) == 0) {
            
                if (tokenOutLeft <= 1) return ERROR_LEXER_TOO_LONG;
                tokenOutLeft--;
                *tokenOut++ = i;
                // special case for REM
                if (i == TOKEN_REM || i == TOKEN_REM_EM ) {
                    *tokenOut++ = TOKEN_STRING;
                    // skip whitespace
                    while (isspace(*tokenIn))
                        tokenIn++;
                    // copy the comment
                    while (*tokenIn) {
                        *tokenOut++ = *tokenIn++;
                    }
                    *tokenOut++ = 0;
                }
                return 0;
            }
        }
        // no matching keyword - this must be an identifier
        // $ is only allowed at the end
        char *dollarPos = strchr(identStr, '$');
        if  (dollarPos && dollarPos!= &identStr[0] + identLen - 1) return ERROR_LEXER_UNEXPECTED_INPUT;
        if (tokenOutLeft <= 1+identLen) return ERROR_LEXER_TOO_LONG;
        tokenOutLeft -= 1+identLen;
        *tokenOut++ = TOKEN_IDENT;
        strcpy((char*)tokenOut, identStr);

        // 0x80 -> end of token = 128(10)
        tokenOut[identLen-1] |= 0x80;

        tokenOut += identLen;
        return 0;
    }
    // string
    if (*tokenIn=='\"') {
        *tokenOut++ = TOKEN_STRING;
        tokenOutLeft--;
        if (tokenOutLeft <= 1) return ERROR_LEXER_TOO_LONG;
        tokenIn++;
        while (*tokenIn) {
            if (*tokenIn == '\"' && *(tokenIn+1) != '\"')
                break;
            else if (*tokenIn == '\"')
                tokenIn++;
            *tokenOut++ = *tokenIn++;
            tokenOutLeft--;
            if (tokenOutLeft <= 1) return ERROR_LEXER_TOO_LONG;
        }
        if (!*tokenIn) return ERROR_LEXER_UNTERMINATED_STRING;
        tokenIn++;
        *tokenOut++ = 0;
        tokenOutLeft--;
        return 0;
    }
    // handle non-alpha tokens e.g. =
    for (int i=LAST_NON_ALPHA_TOKEN; i>=FIRST_NON_ALPHA_TOKEN; i--) {
        // do this "backwards" so we match >= correctly, not as > then =

        //int len = strlen((char *)pgm_read_word(&tokenTable[i].token));
        int len = strlen((char *)tokenTable[i].token);

        if (strncmp((char *)tokenTable[i].token, (char*)tokenIn, len) == 0) {
        //if (strncmp((char *)pgm_read_word(&tokenTable[i].token), (char*)tokenIn, len) == 0) {
            if (tokenOutLeft <= 1) return ERROR_LEXER_TOO_LONG;
            *tokenOut++ = i;
            tokenOutLeft--;
            tokenIn += len;
            return 0;
        }
    }
    return ERROR_LEXER_UNEXPECTED_INPUT;
}

int tokenize(unsigned char *input, unsigned char *output, int outputSize)
{
    tokenIn = input;
    tokenOut = output;
    tokenOutLeft = outputSize;
    int ret;
    while (1) {
        ret = nextToken();
        if (ret) break;
    }
    return (ret > 0) ? ret : 0;
}

/* **************************************************************************
 * PARSER / INTERPRETER
 * **************************************************************************/

//static char executeMode;	// 0 = syntax check only, 1 = execute
uint16_t lineNumber, stmtNumber;
// stmt number is 0 for the first statement, then increases after each command seperator (:)
// Note that IF a=1 THEN PRINT "x": print "y" is considered to be only 2 statements
static uint16_t jumpLineNumber, jumpStmtNumber;
static uint16_t stopLineNumber, stopStmtNumber;
static char breakCurrentLine;

static unsigned char *tokenBuffer, *prevToken;
//static int curToken;
static char identVal[MAX_IDENT_LEN+1];
static char isStrIdent;
static float numVal;
static char *strVal;
static long numIntVal;

int getNextToken()
{
    prevToken = tokenBuffer;
    curToken = *tokenBuffer++;
    if (curToken == TOKEN_IDENT) {
        int i=0;
        while (*tokenBuffer < 0x80)
            identVal[i++] = *tokenBuffer++;
        identVal[i] = (*tokenBuffer++)-0x80;
        isStrIdent = (identVal[i++] == '$');
        identVal[i++] = '\0';
    }
    else if (curToken == TOKEN_NUMBER) {
        numVal = *(float*)tokenBuffer;
        tokenBuffer += sizeof(float);
    }
    else if (curToken == TOKEN_INTEGER) {
        // these are really just for line numbers
        numVal = (float)(*(long*)tokenBuffer);
        tokenBuffer += sizeof(long);
    }
    else if (curToken == TOKEN_STRING) {
        strVal = (char*)tokenBuffer;
        tokenBuffer += 1 + strlen(strVal);
    }
    return curToken;
}

// value (int) returned from parseXXXXX
#define ERROR_MASK						0x0FFF
#define TYPE_MASK						0xF000
#define TYPE_NUMBER						0x0000
#define TYPE_STRING						0x1000

#define IS_TYPE_NUM(x) ((x & TYPE_MASK) == TYPE_NUMBER)
#define IS_TYPE_STR(x) ((x & TYPE_MASK) == TYPE_STRING)

// forward declarations
int parseExpression();
int parsePrimary();
int expectNumber();

// parse a number
int parseNumberExpr()
{
    if (executeMode && !stackPushNum(numVal))
        return ERROR_OUT_OF_MEMORY;
    getNextToken(); // consume the number
    return TYPE_NUMBER;
}

// parse (x1,....xn) e.g. DIM a(10,20)
int parseSubscriptExpr() {
    // stacks x1, .. xn followed by n
    int numDims = 0;
    if (curToken != TOKEN_LBRACKET) return ERROR_EXPR_MISSING_BRACKET;
    getNextToken();
    while(1) {
        numDims++;
        int val = expectNumber();
        if (val) return val;	// error
        if (curToken == TOKEN_RBRACKET)
            break;
        else if (curToken == TOKEN_COMMA)
            getNextToken();
        else
            return ERROR_EXPR_MISSING_BRACKET;
    }
    getNextToken(); // eat )
    if (executeMode && !stackPushNum(numDims))
        return ERROR_OUT_OF_MEMORY;
    return 0;
}

// parse a function call e.g. LEN(a$)
int parseFnCallExpr() {
    int op = curToken;
    //int fnSpec = pgm_read_byte_near(&tokenTable[curToken].format);
    int fnSpec = tokenTable[curToken].format;
    getNextToken();
    // get the required arguments and types from the token table
    if (curToken != TOKEN_LBRACKET) return ERROR_EXPR_MISSING_BRACKET;
    getNextToken();

    int reqdArgs = fnSpec & TKN_ARGS_NUM_MASK;
    int argTypes = (fnSpec & TKN_ARG_MASK) >> TKN_ARG_SHIFT;
    int ret = (fnSpec & TKN_RET_TYPE_STR) ? TYPE_STRING : TYPE_NUMBER;
    for (int i=0; i<reqdArgs; i++) {
        int val = parseExpression();
        if (val & ERROR_MASK) return val;
        // check we've got the right type
        if (!(argTypes & 1) && !IS_TYPE_NUM(val))
            return ERROR_EXPR_EXPECTED_NUM;
        if ((argTypes & 1) && !IS_TYPE_STR(val))
            return ERROR_EXPR_EXPECTED_STR;
        argTypes >>= 1;
        // if this isn't the last argument, eat the ,
        if (i+1<reqdArgs) {
            if (curToken != TOKEN_COMMA)
                return ERROR_UNEXPECTED_TOKEN;
            getNextToken();
        }
    }
    // now all the arguments will be on the stack (last first)
    if (executeMode) {
        int tmp, tmp2;
        char* tmpS1;

        switch (op) {
        case TOKEN_INT:
            stackPushNum((float)floor(stackPopNum()));
            break;
        case TOKEN_STR:
            {
                char buf[16];
                if (!stackPushStr(host_floatToStr(stackPopNum(), buf)))
                    return ERROR_OUT_OF_MEMORY;
            }
            break;
        case TOKEN_LEN:
            tmp = strlen(stackPopStr());
            if (!stackPushNum(tmp)) return ERROR_OUT_OF_MEMORY;
            break;
        case TOKEN_VAL:
            {
                // tokenise str onto the stack
                int oldStackEnd = sysSTACKEND;
                unsigned char *oldTokenBuffer = prevToken;
                int val = tokenize((unsigned char*)stackGetStr(), &mem[sysSTACKEND], sysVARSTART - sysSTACKEND);
                if (val) {
                    if (val == ERROR_LEXER_TOO_LONG) return ERROR_OUT_OF_MEMORY;
                    else return ERROR_IN_VAL_INPUT;
                }
                // set tokenBuffer to point to the new set of tokens on the stack
                tokenBuffer = &mem[sysSTACKEND];
                // move stack end to the end of the new tokens
                sysSTACKEND = tokenOut - &mem[0];
                getNextToken();
                // then parseExpression
                val = parseExpression();
                if (val & ERROR_MASK) {
                    if (val == ERROR_OUT_OF_MEMORY) return val;
                    else return ERROR_IN_VAL_INPUT;
                }
                if (!IS_TYPE_NUM(val))
                    return ERROR_EXPR_EXPECTED_NUM;
                // read the result from the stack
                float f = stackPopNum();
                // pop the tokens from the stack
                sysSTACKEND = oldStackEnd;
                // and pop the original string
                stackPopStr();
                // finally, push the result and set the token buffer back
                stackPushNum(f);
                tokenBuffer = oldTokenBuffer;
                getNextToken();
            }
            break;
        case TOKEN_LEFT:
            tmp = (int)stackPopNum();
            if (tmp < 0) return ERROR_STR_SUBSCRIPT_OUT_RANGE;
            stackLeftOrRightStr(tmp, 0);
            break;
        case TOKEN_RIGHT:
            tmp = (int)stackPopNum();
            if (tmp < 0) return ERROR_STR_SUBSCRIPT_OUT_RANGE;
            stackLeftOrRightStr(tmp, 1);
            break;
        case TOKEN_MID:
            {
                tmp = (int)stackPopNum();
                int start = stackPopNum();
                if (tmp < 0 || start < 1) return ERROR_STR_SUBSCRIPT_OUT_RANGE;
                stackMidStr(start, tmp);
            }
            break;
        case TOKEN_PINREAD:
            tmp = (int)stackPopNum();
            if (!stackPushNum(host_digitalRead(tmp))) return ERROR_OUT_OF_MEMORY;
            break;
        case TOKEN_ANALOGRD:
            tmp = (int)stackPopNum();
            if (!stackPushNum(host_analogRead(tmp))) return ERROR_OUT_OF_MEMORY;
            break;

// ============ Xtase ===========
        case TOKEN_BTN:
            tmp = (int)stackPopNum();
            if (!stackPushNum(xts_buttonRead(tmp))) return ERROR_OUT_OF_MEMORY;
            break;

        case TOKEN_STR_STRING:
            tmp2 = (int)stackPopNum();  // inv. sorting
            tmp = (int)stackPopNum();
            if (!stackPushStr(xts_str_string(tmp, tmp2))) return ERROR_OUT_OF_MEMORY;
            break;

        case TOKEN_STR_INSTR:
            tmpS1 = stackPopStr();  // inv. sorting
            if (!stackPushNum(xts_str_instr(stackPopStr(), tmpS1))) return ERROR_OUT_OF_MEMORY;
            break;

        case TOKEN_STR_ASC:
            if (!stackPushNum(xts_str_asc( stackPopStr() ))) return ERROR_OUT_OF_MEMORY;
            break;

        case TOKEN_STR_UPPER:
            if (!stackPushStr(xts_str_upper( stackPopStr() ))) return ERROR_OUT_OF_MEMORY;
            break;

        case TOKEN_STR_SPACE:
            tmp2 = (int)' ';
            tmp = (int)stackPopNum();
            if (!stackPushStr(xts_str_string(tmp, tmp2))) return ERROR_OUT_OF_MEMORY;
            break;

        case TOKEN_ABS:
            if (!stackPushNum(xts_abs( stackPopNum() ))) return ERROR_OUT_OF_MEMORY;
            break;
        case TOKEN_COS:
            // in degrees
            if (!stackPushNum(xts_cos( stackPopNum() ))) return ERROR_OUT_OF_MEMORY;
            break;
        case TOKEN_SIN:
            // in degrees
            if (!stackPushNum(xts_sin( stackPopNum() ))) return ERROR_OUT_OF_MEMORY;
            break;
        case TOKEN_SQRT:
            if (!stackPushNum(xts_sqrt( stackPopNum() ))) return ERROR_OUT_OF_MEMORY;
            break;
        case TOKEN_POW:
            tmp2 = (int)stackPopNum(); // inv. order powValue
            tmp  = (int)stackPopNum(); // powNum
            if (!stackPushNum(xts_pow( tmp, tmp2 ))) return ERROR_OUT_OF_MEMORY;
            break;

// ==============================

        default:
            return ERROR_UNEXPECTED_TOKEN;
        }
    }
    if (curToken != TOKEN_RBRACKET) return ERROR_EXPR_MISSING_BRACKET;
    getNextToken();	// eat )
    return ret;
}

// parse an identifer e.g. a$ or a(5,3)
int parseIdentifierExpr() {
    char ident[MAX_IDENT_LEN+1];
    if (executeMode)
        strcpy(ident, identVal);
    int isStringIdentifier = isStrIdent;
    getNextToken();	// eat ident
    if (curToken == TOKEN_LBRACKET) {
        // array access
        int val = parseSubscriptExpr();
        if (val) return val;
        if (executeMode) {
            if (isStringIdentifier) {
                int error = 0;
                char *str = lookupStrArrayElem(ident, &error);
                if (error) return error;
                else if (!stackPushStr(str)) return ERROR_OUT_OF_MEMORY;
            }
            else {
                int error = 0;
                float f = lookupNumArrayElem(ident, &error);
                if (error) return error;
                else if (!stackPushNum(f)) return ERROR_OUT_OF_MEMORY;
            }
        }
    }
    else {
        // simple variable
        if (executeMode) {
            if (isStringIdentifier) {
                char *str = lookupStrVariable(ident);
                if (!str) return ERROR_VARIABLE_NOT_FOUND;
                else if (!stackPushStr(str)) return ERROR_OUT_OF_MEMORY;
            }
            else {
                float f = lookupNumVariable(ident);
                if (f == FLT_MAX) return ERROR_VARIABLE_NOT_FOUND;
                else if (!stackPushNum(f)) return ERROR_OUT_OF_MEMORY;
            }
        }
    }
    return isStringIdentifier ? TYPE_STRING : TYPE_NUMBER;
}

// parse a string e.g. "hello"
int parseStringExpr() {
    if (executeMode && !stackPushStr(strVal))
        return ERROR_OUT_OF_MEMORY;
    getNextToken(); // consume the string
    return TYPE_STRING;
}

// parse a bracketed expressed e.g. (5+3)
int parseParenExpr() {
    getNextToken();  // eat (
    int val = parseExpression();
    if (val & ERROR_MASK) return val;
    if (curToken != TOKEN_RBRACKET)
        return ERROR_EXPR_MISSING_BRACKET;
    getNextToken();  // eat )
    return val;
}

int parse_RND() {
    getNextToken();
    if (executeMode && !stackPushNum((float)rand()/(float)RAND_MAX))
        return ERROR_OUT_OF_MEMORY;
    return TYPE_NUMBER;	
}

int parse_INKEY() {
    getNextToken();
    if (executeMode) {
        char str[2];
        str[0] = host_getKey();
        str[1] = 0;
        if (!stackPushStr(str))
            return ERROR_OUT_OF_MEMORY;
    }
    return TYPE_STRING;	
}

int parseUnaryNumExp()
{
    int op = curToken;
    getNextToken();
    int val = parsePrimary();
    if (val & ERROR_MASK) return val;
    if (!IS_TYPE_NUM(val))
        return ERROR_EXPR_EXPECTED_NUM;
    switch (op) {
    case TOKEN_MINUS:
        if (executeMode) stackPushNum(stackPopNum() * -1.0f);
        return TYPE_NUMBER;
    case TOKEN_NOT:
        if (executeMode) stackPushNum(stackPopNum() ? 0.0f : 1.0f);
        return TYPE_NUMBER;
    default:
        return ERROR_UNEXPECTED_TOKEN;
    }
}

/// primary
int parsePrimary() {
    switch (curToken) {
    case TOKEN_IDENT:	
        return parseIdentifierExpr();
    case TOKEN_NUMBER:
    case TOKEN_INTEGER:
        return parseNumberExpr();
    case TOKEN_STRING:	
        return parseStringExpr();
    case TOKEN_LBRACKET:
        return parseParenExpr();

        // "psuedo-identifiers"
    case TOKEN_RND:	
        return parse_RND();
    case TOKEN_INKEY:
        return parse_INKEY();

// -- XTase
    case TOKEN_SECS:	
        return fct_getSecs();
    case TOKEN_MILLIS:	
        return fct_getMillis();
// -- XTase

        // unary ops
    case TOKEN_MINUS:
    case TOKEN_NOT:
        return parseUnaryNumExp();

        // functions
    case TOKEN_INT: 
    case TOKEN_STR: 
    case TOKEN_LEN: 
    case TOKEN_VAL:
    case TOKEN_LEFT: 
    case TOKEN_RIGHT: 
    case TOKEN_MID: 
    case TOKEN_PINREAD:
    case TOKEN_ANALOGRD:

    case TOKEN_BTN: // Xtase code
    case TOKEN_STR_STRING: // Xtase code
    case TOKEN_STR_UPPER: // Xtase code
    case TOKEN_STR_SPACE: // Xtase code
    case TOKEN_STR_ASC: // Xtase code
    case TOKEN_STR_INSTR: // Xtase code

    case TOKEN_ABS: // Xtase code
    case TOKEN_COS: // Xtase code
    case TOKEN_SIN: // Xtase code

    case TOKEN_SQRT: // Xtase code
    case TOKEN_POW:  // Xtase code

        return parseFnCallExpr();

    // ------ Xtase token ---------
    case TOKEN_MEM:
        return getMemFree();
    // ----------------------------

    default:
        return ERROR_UNEXPECTED_TOKEN;
    }
}

int getTokPrecedence() {
    if (curToken == TOKEN_AND || curToken == TOKEN_OR) return 5;
    if (curToken == TOKEN_EQUALS || curToken == TOKEN_NOT_EQ) return 10;
    if (curToken == TOKEN_LT || curToken == TOKEN_GT || curToken == TOKEN_LT_EQ || curToken == TOKEN_GT_EQ) return 20;
    if (curToken == TOKEN_MINUS || curToken == TOKEN_PLUS) return 30;
    else if (curToken == TOKEN_MULT || curToken == TOKEN_DIV || curToken == TOKEN_MOD) return 40;
    else return -1;
}

// Operator-Precedence Parsing
int parseBinOpRHS(int ExprPrec, int lhsVal) {
    // If this is a binop, find its precedence.
    while (1) {
        int TokPrec = getTokPrecedence();

        // If this is a binop that binds at least as tightly as the current binop,
        // consume it, otherwise we are done.
        if (TokPrec < ExprPrec)
            return lhsVal;

        // Okay, we know this is a binop.
        int BinOp = curToken;
        getNextToken();  // eat binop

        // Parse the primary expression after the binary operator.
        int rhsVal = parsePrimary();
        if (rhsVal & ERROR_MASK) return rhsVal;

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int NextPrec = getTokPrecedence();
        if (TokPrec < NextPrec) {
            rhsVal = parseBinOpRHS(TokPrec+1, rhsVal);
            if (rhsVal & ERROR_MASK) return rhsVal;
        }

        if (IS_TYPE_NUM(lhsVal) && IS_TYPE_NUM(rhsVal))
        {	// Number operations
            float r, l;
            if (executeMode) {
                r = stackPopNum();
                l = stackPopNum();
            }
            if (BinOp == TOKEN_PLUS) {
                if (executeMode) stackPushNum(l+r);
            }
            else if (BinOp == TOKEN_MINUS) {
                if (executeMode) stackPushNum(l-r);
            }
            else if (BinOp == TOKEN_MULT) {
                if (executeMode) stackPushNum(l*r);
            }
            else if (BinOp == TOKEN_DIV) {
                if (executeMode) {
                    if (r) stackPushNum(l/r);
                    else return ERROR_EXPR_DIV_ZERO;
                }
            }
            else if (BinOp == TOKEN_MOD) {
                if (executeMode) {
                    if ((int)r) stackPushNum((float)((int)l % (int)r));
                    else return ERROR_EXPR_DIV_ZERO;
                }
            }
            else if (BinOp == TOKEN_LT) {
                if (executeMode) stackPushNum(l < r ? 1.0f : 0.0f);
            }
            else if (BinOp == TOKEN_GT) {
                if (executeMode) stackPushNum(l > r ? 1.0f : 0.0f);
            }
            else if (BinOp == TOKEN_EQUALS) {
                if (executeMode) stackPushNum(l == r ? 1.0f : 0.0f);
            }
            else if (BinOp == TOKEN_NOT_EQ) {
                if (executeMode) stackPushNum(l != r ? 1.0f : 0.0f);
            }
            else if (BinOp == TOKEN_LT_EQ) {
                if (executeMode) stackPushNum(l <= r ? 1.0f : 0.0f);
            }
            else if (BinOp == TOKEN_GT_EQ) {
                if (executeMode) stackPushNum(l >= r ? 1.0f : 0.0f);
            }
            else if (BinOp == TOKEN_AND) {
                if (executeMode) stackPushNum(r != 0.0f ? l : 0.0f);
            }
            else if (BinOp == TOKEN_OR) {
                if (executeMode) stackPushNum(r != 0.0f ? 1 : l);
            }
            else
                return ERROR_UNEXPECTED_TOKEN;
        }
        else if (IS_TYPE_STR(lhsVal) && IS_TYPE_STR(rhsVal))
        {	// String operations
            if (BinOp == TOKEN_PLUS) {
                if (executeMode)
                    stackAdd2Strs();
            }
            else if (BinOp >= TOKEN_EQUALS && BinOp <=TOKEN_LT_EQ) {
                if (executeMode) {
                    char *r = stackPopStr();
                    char *l = stackPopStr();
                    int ret = strcmp(l,r);
                    if (BinOp == TOKEN_EQUALS && ret == 0) stackPushNum(1.0f);
                    else if (BinOp == TOKEN_NOT_EQ && ret != 0) stackPushNum(1.0f);
                    else if (BinOp == TOKEN_GT && ret > 0) stackPushNum(1.0f);
                    else if (BinOp == TOKEN_LT && ret < 0) stackPushNum(1.0f);
                    else if (BinOp == TOKEN_GT_EQ && ret >= 0) stackPushNum(1.0f);
                    else if (BinOp == TOKEN_LT_EQ && ret <= 0) stackPushNum(1.0f);
                    else stackPushNum(0.0f);
                }
                lhsVal = TYPE_NUMBER;
            }
            else
                return ERROR_UNEXPECTED_TOKEN;
        }
        else
            return ERROR_UNEXPECTED_TOKEN;
    }
}

int parseExpression()
{
    int val = parsePrimary();
    if (val & ERROR_MASK) return val;
    return parseBinOpRHS(0, val);
}

int expectNumber() {
    int val = parseExpression();
    if (val & ERROR_MASK) return val;
    if (!IS_TYPE_NUM(val))
        return ERROR_EXPR_EXPECTED_NUM;
    return 0;
}


// == Xtase ==
void doRunPrg() {
    uint16_t startLine = 1;
    // clear variables
    sysVARSTART = sysVAREND = sysGOSUBSTART = sysGOSUBEND = MEMORY_SIZE;
    jumpLineNumber = startLine;
    stopLineNumber = stopStmtNumber = 0;
}
// == Xtase ==


int parse_RUN() {
    getNextToken();
    uint16_t startLine = 1;
    if (curToken != TOKEN_EOL) {
        int val = expectNumber();
        if (val) return val;	// error
        if (executeMode) {
            startLine = (uint16_t)stackPopNum();
            if (startLine <= 0)
                return ERROR_BAD_LINE_NUM;
        }
    }
    if (executeMode) {
        // clear variables
        sysVARSTART = sysVAREND = sysGOSUBSTART = sysGOSUBEND = MEMORY_SIZE;
        jumpLineNumber = startLine;
        stopLineNumber = stopStmtNumber = 0;
    }
    return 0;
}

int parse_GOTO() {
// DBUG("Entering GOTO");

    getNextToken();
// DBUG("GT. 1");
    int val = expectNumber();
// DBUG("GT. 2->", val);
    if (val) return val;	// error
// DBUG("GT. 3->", val);
    if (executeMode) {
// DBUG("GT. 4->", val);
        uint16_t startLine = (uint16_t)stackPopNum();
// DBUG("GT. 5->", startLine);

        if (startLine <= 0) {
// DBUG("Exiting GOTO : BAD LN");
            return ERROR_BAD_LINE_NUM;
        }
        jumpLineNumber = startLine;
    }

// DBUG("Exiting GOTO ->",jumpLineNumber);

    return 0;
}

int parse_PAUSE() {
    getNextToken();
    int val = expectNumber();
    if (val) return val;	// error
    if (executeMode) {
        long ms = (long)stackPopNum();
        if (ms < 0)
            return ERROR_BAD_PARAMETER;
        host_sleep(ms);
    }
    return 0;
}

int parse_LIST() {
    getNextToken();
    uint16_t first = 0, last = 0;
    if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = expectNumber();
        if (val) return val;	// error
        if (executeMode)
            first = (uint16_t)stackPopNum();
    }
    if (curToken == TOKEN_COMMA) {
        getNextToken();
        int val = expectNumber();
        if (val) return val;	// error
        if (executeMode)
            last = (uint16_t)stackPopNum();
    }
    if (executeMode) {
        listProg(first,last);
        host_showBuffer();
    }
    return 0;
}

int parse_PRINT() {
    getNextToken();
    // zero + expressions seperated by semicolons
    int newLine = 1;
    while (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = parseExpression();
        if (val & ERROR_MASK) return val;
        if (executeMode) {
            if (IS_TYPE_NUM(val))
                host_outputFloat(stackPopNum());
            else
                host_outputString(stackPopStr());
            newLine = 1;
        }
        if (curToken == TOKEN_SEMICOLON) {
            newLine = 0;
            getNextToken();
        }
    }
    if (executeMode) {
        if (newLine)
            host_newLine();
        host_showBuffer();
    }

    //host_outputString("END print"); host_newLine(); host_showBuffer();


    return 0;
}

// parse a stmt that takes two int parameters 
// e.g. POSITION 3,2
int parseTwoIntCmd() {
    int op = curToken;
    getNextToken();
    int val = expectNumber();
    if (val) return val;	// error
    if (curToken != TOKEN_COMMA)
        return ERROR_UNEXPECTED_TOKEN;
    getNextToken();
    val = expectNumber();
    if (val) return val;	// error
    if (executeMode) {
        int second = (int)stackPopNum();
        int first = (int)stackPopNum();
        switch(op) {
        case TOKEN_POSITION: 
            host_moveCursor(first,second); 
            break;
        case TOKEN_PIN: 
            host_digitalWrite(first,second); 
            break;
        case TOKEN_PINMODE: 
            host_pinMode(first,second); 
            break;
        }
    }
    return 0;
}

// this handles both LET a$="hello" and INPUT a$ type assignments
int parseAssignment(bool inputStmt) {
    char ident[MAX_IDENT_LEN+1];
    int val;
    if (curToken != TOKEN_IDENT) return ERROR_UNEXPECTED_TOKEN;

// DBUG_NOLN("assign : "); DBUG(identVal);

//    if (executeMode)
        strcpy(ident, identVal);


// DBUG_NOLN("copied name : "); DBUG(ident);
        

    int isStringIdentifier = isStrIdent;
    int isArray = 0;
    getNextToken();	// eat ident
    if (curToken == TOKEN_LBRACKET) {
        // array element being set
        val = parseSubscriptExpr();
        if (val) return val;
        isArray = 1;
    }
    if (inputStmt) {
        // from INPUT statement
        if (executeMode) {
            char *inputStr = host_readLine();
            if (isStringIdentifier) {
                if (!stackPushStr(inputStr)) return ERROR_OUT_OF_MEMORY;
            }
            else {
                float f = (float)strtod(inputStr, 0);
                if (!stackPushNum(f)) return ERROR_OUT_OF_MEMORY;
            }
            host_newLine();
            host_showBuffer();
        }
        val = isStringIdentifier ? TYPE_STRING : TYPE_NUMBER;
    }
    else {
        // from LET statement
        if (curToken != TOKEN_EQUALS) return ERROR_UNEXPECTED_TOKEN;
        getNextToken(); // eat =
        val = parseExpression();
        if (val & ERROR_MASK) return val;
    }
    // type checking and actual assignment
    if (!isStringIdentifier)
    {	// numeric variable
        if (!IS_TYPE_NUM(val)) return ERROR_EXPR_EXPECTED_NUM;
        if (executeMode) {
            if (isArray) {
                val = setNumArrayElem(ident, stackPopNum());
                if (val) return val;
            }
            else {

                // DBUG_NOLN("feeding var : "); DBUG(ident);
                // // else : value isn't trmit
                // char* id2 = (char*)malloc( strlen(ident) );
                // strcpy(id2, ident);
                // if (!storeNumVariable( id2, stackPopNum())) return ERROR_OUT_OF_MEMORY;
                // free(id2);

                if (!storeNumVariable( ident, stackPopNum())) return ERROR_OUT_OF_MEMORY;
            }
        }
    }
    else
    {	// string variable
        if (!IS_TYPE_STR(val)) return ERROR_EXPR_EXPECTED_STR;
        if (executeMode) {
            if (isArray) {
                // annoyingly, we've got the string at the top of the stack
                // (from parseExpression) and the array index stuff (from
                // parseSubscriptExpr) underneath.
                val = setStrArrayElem(ident);
                if (val) return val;
            }
            else {
                if (!storeStrVariable(ident, stackGetStr())) return ERROR_OUT_OF_MEMORY;
                stackPopStr();
            }
        }
    }
    return 0;
}

int parse_IF() {
    getNextToken();	// eat if
    int val = expectNumber();
    if (val) return val;	// error
    if (curToken != TOKEN_THEN)
        return ERROR_MISSING_THEN;
    getNextToken();
    if (executeMode && stackPopNum() == 0.0f) {
        // condition not met
        breakCurrentLine = 1;
        return 0;
    }
    else return 0;
}

int parse_FOR() {
    char ident[MAX_IDENT_LEN+1];
    float start, end, step = 1.0f;
    getNextToken();	// eat for
    if (curToken != TOKEN_IDENT || isStrIdent) return ERROR_UNEXPECTED_TOKEN;
    if (executeMode)
        strcpy(ident, identVal);
    getNextToken();	// eat ident
    if (curToken != TOKEN_EQUALS) return ERROR_UNEXPECTED_TOKEN;
    getNextToken(); // eat =
    // parse START
    int val = expectNumber();
    if (val) return val;	// error
    if (executeMode)
        start = stackPopNum();
    // parse TO
    if (curToken != TOKEN_TO) return ERROR_UNEXPECTED_TOKEN;
    getNextToken(); // eat TO
    // parse END
    val = expectNumber();
    if (val) return val;	// error
    if (executeMode)
        end = stackPopNum();
    // parse optional STEP
    if (curToken == TOKEN_STEP) {
        getNextToken(); // eat STEP
        val = expectNumber();
        if (val) return val;	// error
        if (executeMode)
            step = stackPopNum();
    }
    if (executeMode) {
        if (!storeForNextVariable(ident, start, step, end, lineNumber, stmtNumber)) return ERROR_OUT_OF_MEMORY;
    }
    return 0;
}

int parse_NEXT() {
    getNextToken();	// eat next
    if (curToken != TOKEN_IDENT || isStrIdent) return ERROR_UNEXPECTED_TOKEN;
    if (executeMode) {
        ForNextData data = lookupForNextVariable(identVal);
        if (data.val == FLT_MAX) return ERROR_VARIABLE_NOT_FOUND;
        else if (data.step == FLT_MAX) return ERROR_NEXT_WITHOUT_FOR;
        // update and store the count variable
        data.val += data.step;
        storeNumVariable(identVal, data.val);
        // loop?
        if ((data.step >= 0 && data.val <= data.end) || (data.step < 0 && data.val >= data.end)) {
            jumpLineNumber = data.lineNumber;
            jumpStmtNumber = data.stmtNumber+1;
        }
    }
    getNextToken();	// eat ident
    return 0;
}

int parse_GOSUB() {
    getNextToken();	// eat gosub
    int val = expectNumber();
    if (val) return val;	// error
    if (executeMode) {
        uint16_t startLine = (uint16_t)stackPopNum();
        if (startLine <= 0)
            return ERROR_BAD_LINE_NUM;
        jumpLineNumber = startLine;
        if (!gosubStackPush(lineNumber,stmtNumber))
            return ERROR_OUT_OF_MEMORY;
    }
    return 0;
}

// LOAD or LOAD "x"
// SAVE, SAVE+ or SAVE "x"
// DELETE "x"
int parseLoadSaveCmd() {
    int op = curToken;
    char autoexec = 0, gotFileName = 0;
    getNextToken();
    if (op == TOKEN_SAVE && curToken == TOKEN_PLUS) {
        getNextToken();
        autoexec = 1;
    }
    else if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = parseExpression();
        if (val & ERROR_MASK) return val;
        if (!IS_TYPE_STR(val))
            return ERROR_EXPR_EXPECTED_STR;
        gotFileName = 1;
    }

    if (executeMode) {
        if (gotFileName) {
#if EXTERNAL_EEPROM
            char fileName[MAX_IDENT_LEN+1];
            if (strlen(stackGetStr()) > MAX_IDENT_LEN)
                return ERROR_BAD_PARAMETER;
            strcpy(fileName, stackPopStr());
            if (op == TOKEN_SAVE) {
                if (!host_saveExtEEPROM(fileName))
                    return ERROR_OUT_OF_MEMORY;
            }
            else if (op == TOKEN_LOAD) {
                reset();
                if (!host_loadExtEEPROM(fileName))
                    return ERROR_BAD_PARAMETER;
            }
            else if (op == TOKEN_DELETE) {
                if (!host_removeExtEEPROM(fileName))
                    return ERROR_BAD_PARAMETER;
            }
#else 

            if (op == TOKEN_LOAD) {
                char fileName[MAX_FILENAME_LEN+1];
                if (strlen(stackGetStr()) > MAX_FILENAME_LEN)
                    return ERROR_BAD_PARAMETER;
                strcpy(fileName, stackPopStr());

                reset();
                if (! xts_loadBas( fileName ) )
                    return ERROR_BAD_PARAMETER;
            }

            else if (op == TOKEN_CHAIN) {
                char fileName[MAX_FILENAME_LEN+1];
                if (strlen(stackGetStr()) > MAX_FILENAME_LEN)
                    return ERROR_BAD_PARAMETER;
                strcpy(fileName, stackPopStr());

                reset();
                if (! xts_chain( fileName ) )
                    return ERROR_BAD_PARAMETER;
            }

            else if (op == TOKEN_SAVE) {
                char fileName[MAX_FILENAME_LEN+1];
                if (strlen(stackGetStr()) > MAX_FILENAME_LEN)
                    return ERROR_BAD_PARAMETER;
                strcpy(fileName, stackPopStr());

                if (! xts_saveBas( fileName ) )
                    return ERROR_BAD_PARAMETER;
            }

            else if (op == TOKEN_DELETE) {
                char fileName[MAX_FILENAME_LEN+1];
                if (strlen(stackGetStr()) > MAX_FILENAME_LEN)
                    return ERROR_BAD_PARAMETER;
                strcpy(fileName, stackPopStr());

                if (! xts_delBas( fileName ) )
                    return ERROR_BAD_PARAMETER;
            }

#endif
        }
        else {
            if (op == TOKEN_SAVE)
#ifdef FS_SUPPORT
  return ERROR_BAD_PARAMETER;
#else
                host_saveProgram(autoexec);
#endif
            else if (op == TOKEN_LOAD) {
#ifdef FS_SUPPORT
  return ERROR_BAD_PARAMETER;
#else
                reset();
                host_loadProgram();
#endif
            }
            else
                return ERROR_UNEXPECTED_CMD;
        }
    }
    return 0;
}

int parseSimpleCmd() {
    int op = curToken;
    getNextToken();	// eat op
    if (executeMode) {
        switch (op) {
            case TOKEN_NEW:
                reset();
                breakCurrentLine = 1;
                break;
            case TOKEN_STOP:
                stopLineNumber = lineNumber;
                stopStmtNumber = stmtNumber;
                return ERROR_STOP_STATEMENT;
            case TOKEN_CONT:
                if (stopLineNumber) {
                    jumpLineNumber = stopLineNumber;
                    jumpStmtNumber = stopStmtNumber+1;
                }
                break;
            case TOKEN_RETURN:
            {
                int returnLineNumber, returnStmtNumber;
                if (!gosubStackPop(&returnLineNumber, &returnStmtNumber))
                    return ERROR_RETURN_WITHOUT_GOSUB;
                jumpLineNumber = returnLineNumber;
                jumpStmtNumber = returnStmtNumber+1;
                break;
            }
            case TOKEN_CLS:
                host_cls();
                host_showBuffer();
                break;
//             case TOKEN_DIR:
//                 xts_fs_dir();
// #if EXTERNAL_EEPROM
//                 host_directoryExtEEPROM();
// #endif
//                 break;

            case TOKEN_BYE:
                xts_mcu_reset();
                break;

            case TOKEN_HALT:
                xts_mcu_halt();
                break;
        }
    }
    return 0;
}

int parse_DIM() {
    char ident[MAX_IDENT_LEN+1];
    getNextToken();	// eat DIM
    if (curToken != TOKEN_IDENT) return ERROR_UNEXPECTED_TOKEN;
    if (executeMode)
        strcpy(ident, identVal);
    int isStringIdentifier = isStrIdent;
    getNextToken();	// eat ident
    int val = parseSubscriptExpr();
    if (val) return val;
    if (executeMode && !createArray(ident, isStringIdentifier ? 1 : 0))
        return ERROR_OUT_OF_MEMORY;
    return 0;
}

static int targetStmtNumber;
int parseStmts()
{
    int ret = 0;
    breakCurrentLine = 0;
    jumpLineNumber = 0;
    jumpStmtNumber = 0;

    // if (executeMode) { DBUG("Begin of Statement"); }

    while (ret == 0) {

        // if (executeMode) { 
        //     // DBUG("token:", curToken); 
        //     if ( curToken >= FIRST_IDENT_TOKEN && curToken <= LAST_IDENT_TOKEN ) {
        //         DBUG( tokenTable[ curToken ].token );
        //     }
        // }

        if (curToken == TOKEN_EOL) {
            // if (executeMode) { DBUG("EOL:quit"); }
            break;
        }
        
        if (executeMode) {
            sysSTACKEND = sysSTACKSTART = sysPROGEND;	// clear calculator stack
        }
    
        int needCmdSep = 1;

        switch (curToken) {
            case TOKEN_PRINT: ret = parse_PRINT(); break;
            case TOKEN_LET: getNextToken(); ret = parseAssignment(false); break;
            case TOKEN_IDENT: ret = parseAssignment(false); break;
            case TOKEN_INPUT: getNextToken(); ret = parseAssignment(true); break;
            case TOKEN_LIST: ret = parse_LIST(); break;
            case TOKEN_RUN: ret = parse_RUN(); break;
            case TOKEN_GOTO: ret = parse_GOTO(); break;
            case TOKEN_REM: getNextToken(); getNextToken(); break;
            case TOKEN_IF: ret = parse_IF(); needCmdSep = 0; break;
            case TOKEN_FOR: ret = parse_FOR(); break;
            case TOKEN_NEXT: ret = parse_NEXT(); break;
            case TOKEN_GOSUB: ret = parse_GOSUB(); break;
            case TOKEN_DIM: ret = parse_DIM(); break;
            case TOKEN_PAUSE: ret = parse_PAUSE(); break;
            
            case TOKEN_LOAD:
            case TOKEN_CHAIN:
            case TOKEN_SAVE:
            case TOKEN_DELETE:
                ret = parseLoadSaveCmd();
                break;
            
            case TOKEN_POSITION:
            case TOKEN_PIN:
            case TOKEN_PINMODE:
                ret = parseTwoIntCmd(); 
                break;
                
            case TOKEN_NEW:
            case TOKEN_STOP:
            case TOKEN_CONT:
            case TOKEN_RETURN:
            case TOKEN_CLS:
            //case TOKEN_DIR:

            case TOKEN_BYE: // Xtase code
            case TOKEN_HALT: // Xtase code

                ret = parseSimpleCmd();
                break;
                
            // ======== Xtase cmds =============
            case TOKEN_PRINT_QM: ret = parse_PRINT(); break;
            case TOKEN_REM_EM: getNextToken(); getNextToken(); break;

            case TOKEN_BEEP: ret = xts_tone(); break;
            case TOKEN_MUTE: ret = xts_mute(); break;

            case TOKEN_LED:    ret = xts_led(); break;
            case TOKEN_LOCATE: ret = xts_locate(); break;

            case TOKEN_ECHO: ret = xts_echo(); break;

            case TOKEN_DELAY: ret = xts_delay(); break;

            case TOKEN_PLAY: ret = xts_play(); break;

            case TOKEN_PLAYT5K: ret = xts_playT5K(); break;
            case TOKEN_PLAYT53: ret = xts_playT53(); break;

            case TOKEN_CONSOLE: ret = xts_console(); break;

            case TOKEN_LLIST: ret = xts_llist(); break;

            case TOKEN_DRAWBPP: ret = xts_dispBPP(); break;
            case TOKEN_DRAWPCT: ret = xts_dispPCT(); break;

            case TOKEN_CIRCLE : ret = xts_dispCircle(); break;
            case TOKEN_LINE   : ret = xts_dispLine(); break;
            case TOKEN_PSET   : ret = xts_pset(); break;
            case TOKEN_PRESET : ret = xts_preset(); break;
 
            case TOKEN_DIR: ret = xts_fs_dir(); break;
            case TOKEN_DIRARRAY: ret = xts_fs_dir(true); break;

            case TOKEN_EXT_EXEC: ret = xts_exec_cmd(); break;
            case TOKEN_DATAF: ret = xts_dataf_cmd(); break;

            case TOKEN_BLITT : ret = xts_blittMode(); break;
            case TOKEN_RECT  : ret = xts_dispRect(); break;

            // ======== Xtase cmds ============= 


            default: 
                ret = ERROR_UNEXPECTED_CMD;
        }

        // if (executeMode) { DBUG("ret.1:", ret); }

        // if error, or the execution line has been changed, exit here
        if (ret || breakCurrentLine || jumpLineNumber || jumpStmtNumber)
            break;

        // if (executeMode) { DBUG("ret.2:", ret); }

        // it should either be the end of the line now, and (generally) a command seperator
        // before the next command
        if (curToken != TOKEN_EOL) {
            if (needCmdSep) {
                if (curToken != TOKEN_CMD_SEP) ret = ERROR_UNEXPECTED_CMD;
                else {
                    getNextToken();
                    // don't allow a trailing :
                    if (curToken == TOKEN_EOL) ret = ERROR_UNEXPECTED_CMD;
                }
            }
        }
        // increase the statement number
        stmtNumber++;

        // if (executeMode) { DBUG("stmt.1:", stmtNumber); }

        // if we're just scanning to find a target statement, check
        if (stmtNumber == targetStmtNumber)
            break;
    }

    // if (executeMode) { DBUG("End of Statement");    DBUG(ret); }

    return ret;
}

int processInput(unsigned char *tokenBuf) {
    // first token can be TOKEN_INTEGER for line number - stored as a float in numVal
    // store as WORD line number (max 65535)
    tokenBuffer = tokenBuf;
    getNextToken();
    // check for a line number at the start
    uint16_t gotLineNumber = 0;
    unsigned char *lineStartPtr = 0;
    if (curToken == TOKEN_INTEGER) {
        long val = (long)numVal;
        if (val <=65535) {
            gotLineNumber = (uint16_t)val;
            lineStartPtr = tokenBuffer;
            getNextToken();
        }
        else
            return ERROR_LINE_NUM_TOO_BIG;
    }

    executeMode = 0;
    targetStmtNumber = 0;
    int ret = parseStmts();	// syntax check
    if (ret != ERROR_NONE)
        return ret;

    if (gotLineNumber) {
        if (!doProgLine(gotLineNumber, lineStartPtr, tokenBuffer - lineStartPtr))
            ret = ERROR_OUT_OF_MEMORY;
    }
    else {
        // we start off executing from the input buffer
        tokenBuffer = tokenBuf;
        executeMode = 1;
        lineNumber = 0;	// buffer
        unsigned char *p;

        while (1) {
            getNextToken();

            stmtNumber = 0;
            // skip any statements? (e.g. for/next)
            if (targetStmtNumber) {
                executeMode = 0; 
                parseStmts(); 
                executeMode = 1;
                targetStmtNumber = 0;
            }
            // now execute
            ret = parseStmts();
            if (ret != ERROR_NONE)
                break;

            // are we processing the input buffer?
            if (!lineNumber && !jumpLineNumber && !jumpStmtNumber)
                break;	// if no control flow jumps, then just exit

            // are we RETURNing to the input buffer?
            if (lineNumber && !jumpLineNumber && jumpStmtNumber)
                lineNumber = 0;

            if (!lineNumber && !jumpLineNumber && jumpStmtNumber) {
                // we're executing the buffer, and need to jump stmt (e.g. for/next)
                tokenBuffer = tokenBuf;
            }
            else {
                // we're executing the program
                if (jumpLineNumber || jumpStmtNumber) {
                    // line/statement number was changed e.g. goto
                    p = findProgLine(jumpLineNumber);
                }
                else {
                    // line number didn't change, so just move one to the next one
                    p+= *(uint16_t *)p;
                }
                // end of program?
                if (p == &mem[sysPROGEND])
                    break;	// end of program

                lineNumber = *(uint16_t*)(p+2);
                tokenBuffer = p+4;
                // if the target for a jump is missing (e.g. line deleted) and we're on the next line
                // reset the stmt number to 0
                if (jumpLineNumber && jumpStmtNumber && lineNumber > jumpLineNumber)
                    jumpStmtNumber = 0;
            }
            if (jumpStmtNumber)
                targetStmtNumber = jumpStmtNumber;

            if (host_ESCPressed())
            { 
                ret = ERROR_BREAK_PRESSED; 
                break; 
            }
        }
    }
    return ret;
}

void reset() {
    // program at the start of memory
    sysPROGEND = 0;
    // stack is at the end of the program area
    sysSTACKSTART = sysSTACKEND = sysPROGEND;
    // variables/gosub stack at the end of memory
    sysVARSTART = sysVAREND = sysGOSUBSTART = sysGOSUBEND = MEMORY_SIZE;
    memset(&mem[0], 0, MEMORY_SIZE);

    stopLineNumber = 0;
    stopStmtNumber = 0;
    lineNumber = 0;
}

