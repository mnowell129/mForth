/* MIT License

Copyright (c) 2020 mnowell129
 
Charles M. Nowell Jr.
The Mouse Works, LLC 
mickeynowell@tmwfl.com 
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
/*
MickeyForth (mForth) forth interpreter.
copyright 1998-2011
 
*/

#ifndef FTYPES_H
#define FTYPES_H

// User must include this somewhere in the compile path
// see forthconfigsample.h for a template
#include "forthconfig.h"
#include <stdint.h>

#ifndef _SETJMP
#include <setjmp.h>
#endif

#define GLOBAL_DOS
#undef  GLOBAL_DOS

#define RELATIVE
//#undef RELATIVE


#define PTR_SIZE 4
#if PTR_SIZE==2
typedef struct UserState *UserStatePtr;
typedef void  (*WordPtr)(UserStatePtr user);
typedef uint16_t *CellPtr;
typedef uint16_t  Cell;
typedef int16_t   SignedCell;
#define CELL 2
#define CELL_ALIGN_MASK 0x01
#else
typedef struct UserState *UserStatePtr;
typedef void  (*WordPtr)(UserStatePtr user);
typedef WordPtr CellPtr;
typedef int32_t  SignedCell;
typedef uint32_t  Cell;
#define CELL 4
#define CELL_ALIGN_MASK 0x03
#endif

typedef float Float32;

/* the definition of a word pointer */
/* i.e. a forth token */
//typedef struct UserState;
//typedef struct UserState *UserStatePtr;

typedef void AssemblyWord(void);
typedef void ForthWord(void);
//typedef  enum{CODE,COLON,VARIABLE,CONSTANT,USER}WordType;
#define  IMMEDIATE    BIT7_OF_8
// note ignore compile only words
// so the user must me cognizant that the word
// is compile only.
// frees up one bit for length so length of strings can change 
// from 64 to 128
// #define  COMPILE_ONLY BIT6_OF_8
#define  LENGTH_MASK 0x7f



typedef struct
{
   WordPtr   code;        /* pointer to the code for this word */
   char      *text;        /* pointer to the actual 0 terminated text */
    #ifdef USE_HELP
    char      *help;
    #endif
}NameType; 

/* for variables */
/* variable space is */
/* a chunk of ram where each holds a pointer to it */
/* user HERE to keep up with it , etc */
/* declaration words need a place to add themselves */

#define CODE_SPACE_SIZE      0x80
#define STACK_SIZE           64 // 2048
#define RETURN_STACK_SIZE    64


typedef uint8_t* (*MallocType)(int16_t sizeRequested);
typedef void  (*FreeType)(uint8_t *blockToFree);
typedef const char *(*ExternalStartup)(UserStatePtr user);
#ifndef PRINTF_TYPE
typedef void  (*PrintfType)(const char *format,...);
#endif
typedef void  (*PutSType)(const char *string);
typedef char *(*GetsType)(char *buffer);
typedef char *(*GetsVector)(UserStatePtr user);
typedef void  (*PutchType)(char key);
typedef int   (*GetchType)(void);
typedef bool  (*KeyHitType)(void);
typedef void  (*SetEchoType)(bool);


typedef enum
   {
      FORTH_IP,
      RP,
      SP,
      SP0,
      RP0,
      IPTEMP0,
      IPTEMP1,
      IPTEMP2,
      IPTEMP3,
      FPRINTF,
      PUTS,
      PUTCH,
      GETCH,
      KEYHIT,
      GETS,
      GETS_VECTOR,
      SET_ECHO_FUNCTION,
      MALLOC,
      FREE,
      // end of named variables
      STATE,
      CSP,
      CURRENT,
      HANDLER,
      TICKEVAL,
      BASE,
      PTIB,
      PPAD,
      TO_IN,
      HLD,
      QUIET,
      /* SEARCHABLE,*/ /* this is a bit mask that determines whether or not
                     a wordlist is searchable when compiling
                     maybe a future feature */
      WORDLISTS,
      DEFINES, // this must be here so that the cfasearch routine can back over it
      WORDLIST0, // this for core words
      WORDLIST1, // this for the few compiled core words
      WORDLIST2, // this for user's specific words
      WORDLIST3,
      WORDLIST4,
      WORDLIST5,
      USER_ID,  // identifies which user this is Root (serial), mib, host, hdp
      START_STRING,
      //      FILEGETS,
      FILEHANDLE,
   FQUEUE,
      QUEUEARRAY0,
      QUEUEARRAY1,
      QUEUEARRAY2,
      QUEUEARRAY3,
   FSEMAPHORE,    // semaphore that our thread can wait on, cast to pointer of local type
      SEMACALLBACK,  // pointer to function that meets local prototype to call back to signal the semaphore
      SUBSCRIBEPTR,
      OUTPUTPTR,     // pointer to a function that outputs variables on this interface
      RECORD,       // hold our current file record number
      LASTRECORD,   // last record to read if we don't hit EOF first
      OLD_GETS0,
      OLD_GETS1,
      OLD_GETS2,
      ECHO,
      ECHOZERO,
      READY_FOR_INPUT,  /* a pointer to a function, that if not null, is called in dotPrompt */
      /* indicates that the last query input has been processed */
      /* used by block devices so that at the end of a bunch of output the block device */
      /* can send the accumulated output */
      ARRAY_INDEX,
      TIB_SIZE,
      PAD_SIZE,
      ABORT,
      BUFFERHEAD,
      BUFFERTAIL,
      STRING_TEMP  // a buffer for run time strings storage, temporary
  
   }NamedUserVariables;

#define MAX_WORDLISTS  (WORDLIST5-WORDLIST0+1)
//     SP0,RP0,UP0,TICKKEY,TICKEMIT,TICKEXPECT,TICKTAP,
//   TICKECHO,TICKPROMPT,BASE,TMP,SPAN,TO_IN,PTIB,
//   TIB,CSP,TICKEVAL,TICKNUMBER,HLD,HANDLER,CP,NP,LAST}UserVars;


#define CONSTRUCT0(a)          (0xFFFFFFFE | a)
#define CONSTRUCT1(a)          (a)
#define FUNCTION_MASK         (1)
#define THREAD_MASK           CONSTRUCT0(1)
#define THREAD_HEADER         CONSTRUCT1(1)
#define VAR_HEADER            CONSTRUCT1(3)
#define CONST_HEADER          CONSTRUCT1(5)
#define DEFINE_HEADER         CONSTRUCT1(7)
#if 0
#define THREAD_MASK           CONSTRUCT0(1)
#define THREAD_HEADER         CONSTRUCT1(0)
#define VAR_HEADER            CONSTRUCT1(1)
#define CONST_HEADER          CONSTRUCT1(2)
#define DEFINE_HEADER         CONSTRUCT1(3)
#endif
#define SUBTYPE_MASK          0x07


typedef struct NamedVariables
{
    WordPtr              volatile * ip;  /* instruction pointer */
    WordPtr              ** volatile rp;  /* return pointer */
    Cell                 * volatile stackPtr;
    Cell                 *stackStart;
    WordPtr              **rpStart; //  = returnStack;
    WordPtr              ipTemp[4];  /* word to ip to point to initially */
    PrintfType           userPrintf;
    PutSType             userPuts;
    PutchType            userPutch;
    GetchType            userGetch;
    KeyHitType           userKeyHit;
    GetsType             userGets;
    GetsVector           getsVector;
    SetEchoType          setEcho;
    MallocType           forthMallocPtr;
    FreeType             forthFreePtr;
    uint32_t               state;
    uint32_t               csp;
    uint32_t               current;
    uint32_t               handler;
    uint32_t               tickeval;
    uint32_t               base;
    uint32_t               ptib;
    uint32_t               ppad;
    uint32_t               to_in;
    uint32_t               hld;
    uint32_t               quiet;
    uint32_t               wordlists;
    uint32_t               defines;
    uint32_t               wordlist0;
    uint32_t               wordlist1;
    uint32_t               wordlist2;
    uint32_t               wordlist3;
    uint32_t               wordlist4;
    uint32_t               wordlist5;
    uint32_t               user_id;
    uint32_t               start_string;
    uint32_t               filehandle;
    uint32_t               queue;
    uint32_t               queuearray0;
    uint32_t               queuearray1;
    uint32_t               queuearray2;
    uint32_t               queuearray3;
    uint32_t               semaphore;
    uint32_t               semaCallback;
    uint32_t               subscribePtr;
    uint32_t               outputPtr;
    uint32_t               record;
    uint32_t               lastrecord;
    uint32_t               old_gets0;
    uint32_t               old_gets1;
    uint32_t               old_gets2;
    uint32_t               echo;
    uint32_t               echoZero;
    uint32_t               readyForInput;
    uint32_t               arrayIndex;
    uint32_t               tibSize;
    uint32_t               padSize;
    uint32_t               abort;
    uint32_t               bufferHead; // linked list of buffers for this context
    uint32_t               bufferTail; 
    uint32_t               string_temp[1];
}
NamedVariables;

// The tib is very large to allow for a full
// 256 byte message from the acoustic
// That may be one big strings
// modified to 128 to save space.
// Pad is just used to format numbers
// also 80 is unreasonable
// reduce to 40
// Make the temp buffer bigger to allow for interpreted arrays
// Perhaps later fold the pad and tempstring together
// to save space
#define TIB_SIZE (1024+80)
//#define PAD_SIZE 81
// #define PAD_SIZE (1024/4)  // cells = 128 bytes
//#define TEMP_SIZE 512  // room for 160 characters or 40 int32's

typedef struct UserState
{
    jmp_buf     jumper;
    // char        tibBuffer[TIB_SIZE];
    char        *tibBuffer;
    // make the padbuffer do double duty
    // as both pad, tempstring and arrays
    // Cell        padBuffer[PAD_SIZE];
    Cell        *padBuffer;
    //    uint32_t      tempString[TEMP_SIZE];
    Cell        stack[STACK_SIZE+RETURN_STACK_SIZE];
    WordPtr     doColon;
    WordPtr     doVariable;
    WordPtr     doConstant;
    //    WordPtr doUser;
    //    WordPtr doDoes; /* maybe */
    //    WordPtr doDefine; /* maybe */
    WordPtr  dollarCompile;
    WordPtr  dollarInterpret;
    WordPtr  dollarArray;
   union {
        NamedVariables   namedVariables;
        Cell           indexedVariables[STRING_TEMP+1]; // just needs to be bigger than the
        // number of named variables
   }
   userVariables;
}
UserState;

//typedef void  (*WordPtr)(UserStatePtr user);


typedef struct
{
   UserState           userState;
   WordPtr             initFunction;
   ExternalStartup     externalStartup;
}
Context;


//#define NUM_USERS 4 // some small value right now

//extern Context   users[NUM_USERS];
#define ROOT_USER  0 // Uart
#define USER_ONE   1 // MIB
#define USER_TWO   2 // Host
#define USER_THREE 3 // UDP

// NOTE : u MUST be a scalar pointer variable to userState
#define USER_VAR(u,v)   (u->userVariables.indexedVariables[v])

#define PUSH(u,x)   push((u),(Cell)(x))
#define POP(u)      pop((u))

#define UPUSH(x)      push(user,(Cell)(x))
#define UPOP()        pop(user)
#define UDEPTH()      depth(user)

// define some error codes for
// longjump to send with stack problems
//
#define STACK_EMPTY          1
#define RETURN_STACK_EMPTY   2
#define BASE_0               3
#define PACKDOLLAR           4
#define DOLLARCOMMA          5

uint8_t addWord(UserStatePtr user,WordPtr function,char *name);


#define FCONTEXT  UserStatePtr user


#define LONG_STRING_COUNT
//#undef LONG_STRING_COUNT


#ifdef M3FIX
// fixup for cortex m3
   #define FUNCTION(a)  ((uint8_t *)(((int)a) & (-2)))
#else
   #define FUNCTION(a)  ((uint8_t *)a)
#endif


#define ROM_END   0x0003FFFF
#define PERIPHERAL_START 0x2009C000

#endif
