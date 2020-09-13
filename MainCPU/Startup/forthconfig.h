/*

MickeyForth (mForth) forth interpreter.
copyright 1998-2011

Charles M. Nowell Jr.
The Mouse Works, LLC

This software is not freeware or shareware.

Permission to re-use or distribute this source code is prohibited without
prior permission.
 
*/

#ifndef FORTHCONFIG_H
#define FORTHCONFIG_H


// saves some memory not to have help
#define USE_HELP
#undef  USE_HELP


// this needs to be defined to use on a cortex m3
#define M3FIX
// #undef M3FIX

// for a PC based version, probably needs some fixup if this is going to be done
#define PC
#undef PC

// Define this to use a local malloc function
// i.e. not the system malloc
#define LOCAL_MALLOC
#undef  LOCAL_MALLOC

#define USE_FLOAT
// #undef  USE_FLOAT

#define USE_BUFFER_DATA
#undef  USE_BUFFER_DATA

#define USE_SD_FILE
#undef USE_SD_FILE

#define USE_MATRIX
#undef USE_MATRIX

// define this to make it impossible to read FLASH and peripherals
#define SECURE
#undef SECURE


#define USE_OSDELAY
// #undef USE_OSDELAY

// stuff to get to the desired i/o routines
// Modify for your application
//#include "target.h"
//#include "uartconst.h"

#include "conio.h"


// how big the define space is 
#define DEFINES1_NAME_MAX 100


typedef enum
{
   ROOT_USER_ID,
}ForthContextIDType;

#endif
