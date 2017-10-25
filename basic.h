/***************************
* Host.h
*
* Xtase - fgalliat @ Sept2017
* Original Code by Robin Edwards @2014
***************************/

#ifndef _BASIC_H
#define _BASIC_H

#include <stdint.h>

// #ifdef FS_SUPPORT_AVOIDS_CONST
//   //#define const
// #endif


// ===============================
// Token flags
// bits 1+2 number of arguments
#define TKN_ARGS_NUM_MASK	0x03
// bit 3 return type (set if string)
#define TKN_RET_TYPE_STR	0x04
// bits 4-6 argument type (set if string)
#define TKN_ARG1_TYPE_STR	0x08
#define TKN_ARG2_TYPE_STR	0x10
#define TKN_ARG3_TYPE_STR	0x20

#define TKN_ARG_MASK		0x38
#define TKN_ARG_SHIFT		3
// bits 7,8 formatting
#define TKN_FMT_POST		0x40
#define TKN_FMT_PRE		0x80
// ===============================

// Xtase Extended Commands Set
#define XTS_EXT_CMDSET 1


#define TOKEN_EOL		0
#define TOKEN_IDENT		1	// special case - identifier follows
#define TOKEN_INTEGER	        2	// special case - integer follows (line numbers only)
#define TOKEN_NUMBER	        3	// special case - number follows
#define TOKEN_STRING	        4	// special case - string follows

#define TOKEN_LBRACKET	        8
#define TOKEN_RBRACKET	        9
#define TOKEN_PLUS	    	10
#define TOKEN_MINUS		11
#define TOKEN_MULT		12
#define TOKEN_DIV		13
#define TOKEN_EQUALS	        14
#define TOKEN_GT		15
#define TOKEN_LT		16
#define TOKEN_NOT_EQ	        17
#define TOKEN_GT_EQ		18
#define TOKEN_LT_EQ		19
#define TOKEN_CMD_SEP	        20
#define TOKEN_SEMICOLON	        21
#define TOKEN_COMMA		22
#define TOKEN_AND		23	// FIRST_IDENT_TOKEN
#define TOKEN_OR		24
#define TOKEN_NOT		25
#define TOKEN_PRINT		26
#define TOKEN_LET		27
#define TOKEN_LIST		28
#define TOKEN_RUN		29
#define TOKEN_GOTO		30
#define TOKEN_REM		31
#define TOKEN_STOP		32
#define TOKEN_INPUT	        33
#define TOKEN_CONT		34
#define TOKEN_IF		35
#define TOKEN_THEN		36
#define TOKEN_LEN		37
#define TOKEN_VAL		38
#define TOKEN_RND		39
#define TOKEN_INT		40
#define TOKEN_STR		41
#define TOKEN_FOR		42
#define TOKEN_TO		43
#define TOKEN_STEP		44
#define TOKEN_NEXT		45
#define TOKEN_MOD		46
#define TOKEN_NEW		47
#define TOKEN_GOSUB		48
#define TOKEN_RETURN	        49
#define TOKEN_DIM		50
#define TOKEN_LEFT		51
#define TOKEN_RIGHT	        52
#define TOKEN_MID		53
#define TOKEN_CLS               54
#define TOKEN_PAUSE             55
#define TOKEN_POSITION          56
#define TOKEN_PIN               57
#define TOKEN_PINMODE           58
#define TOKEN_INKEY             59
#define TOKEN_SAVE              60
#define TOKEN_LOAD              61
#define TOKEN_PINREAD           62
#define TOKEN_ANALOGRD          63
#define TOKEN_DIR               64
#define TOKEN_DELETE            65

// ----------- Xtase Tokens -------
#ifdef XTS_EXT_CMDSET
    #define TOKEN_MEM            66
    #define TOKEN_PRINT_QM       67
    #define TOKEN_REM_EM         68

    #define TOKEN_LOCATE         69
    #define TOKEN_LED            70 // 0x46

    #define TOKEN_TONE           71
    #define TOKEN_MUTE           72

    #define TOKEN_PLAY           73
    #define TOKEN_PLAYT5K        74
    #define TOKEN_PLAYT53        75

    #define TOKEN_BYE            76 // reset the MCU

    #define TOKEN_BTN            77 // read btn state

    #define TOKEN_ECHO           78 // (un)lock local echo

    #define TOKEN_DELAY          79 // make MCU sleep x ms

    #define TOKEN_CONSOLE        80 // select I/O console

    #define TOKEN_LLIST          81 // PRGM to Serial

    #define TOKEN_LASTONE TOKEN_LLIST
#else
    // non extended command set
    #define TOKEN_PRINT_QM       TOKEN_PRINT
    #define TOKEN_REM_EM         TOKEN_REM
    #define TOKEN_LASTONE TOKEN_DELETE
#endif


#define FIRST_IDENT_TOKEN 23
//#define LAST_IDENT_TOKEN 65
#define LAST_IDENT_TOKEN TOKEN_LASTONE

// =========================================

#define FIRST_NON_ALPHA_TOKEN    8
#define LAST_NON_ALPHA_TOKEN    22

#define ERROR_NONE				0
// parse errors
#define ERROR_LEXER_BAD_NUM			1
#define ERROR_LEXER_TOO_LONG			2
#define ERROR_LEXER_UNEXPECTED_INPUT	        3
#define ERROR_LEXER_UNTERMINATED_STRING         4
#define ERROR_EXPR_MISSING_BRACKET		5
#define ERROR_UNEXPECTED_TOKEN			6
#define ERROR_EXPR_EXPECTED_NUM			7
#define ERROR_EXPR_EXPECTED_STR			8
#define ERROR_LINE_NUM_TOO_BIG			9
// runtime errors
#define ERROR_OUT_OF_MEMORY			10
#define ERROR_EXPR_DIV_ZERO			11
#define ERROR_VARIABLE_NOT_FOUND		12
#define ERROR_UNEXPECTED_CMD			13
#define ERROR_BAD_LINE_NUM			14
#define ERROR_BREAK_PRESSED			15
#define ERROR_NEXT_WITHOUT_FOR			16
#define ERROR_STOP_STATEMENT			17
#define ERROR_MISSING_THEN			18
#define ERROR_RETURN_WITHOUT_GOSUB		19
#define ERROR_WRONG_ARRAY_DIMENSIONS	        20
#define ERROR_ARRAY_SUBSCRIPT_OUT_RANGE	        21
#define ERROR_STR_SUBSCRIPT_OUT_RANGE	        22
#define ERROR_IN_VAL_INPUT			23
#define ERROR_BAD_PARAMETER                     24

#define MAX_IDENT_LEN	8
#define MAX_NUMBER_LEN	10

// 8+1+3 ex. 'TOTO'+'.'+'BAS'
#define MAX_FILENAME_LEN	12

// max token per line ???
#define TOKEN_BUF_SIZE 64

// _____________>> that in xts_arch.h ???
#define MEMORY_SIZE	1024
//#define MEMORY_SIZE	8192

// max char per code lines (depends of ammount of RAM ??)
#define ASCII_CODELINE_SIZE 128
// ______________________________________



extern unsigned char mem[];
extern int sysPROGEND;
extern int sysSTACKSTART;
extern int sysSTACKEND;
extern int sysVARSTART;
extern int sysVAREND;
extern int sysGOSUBSTART;
extern int sysGOSUBEND;

extern uint16_t lineNumber;	// 0 = input buffer

typedef struct {
    float val;
    float step;
    float end;
    uint16_t lineNumber;
    uint16_t stmtNumber;
} 
ForNextData;

typedef struct {
    const char *token;
    uint8_t format;
} 
TokenTableEntry;

//extern const char *errorTable[];
// extern const char* const errorTable[];
// extern const TokenTableEntry tokenTable[];

extern const char* errorTable[];
extern TokenTableEntry tokenTable[];



void reset();
int tokenize(unsigned char *input, unsigned char *output, int outputSize);
int processInput(unsigned char *tokenBuf);

#endif

