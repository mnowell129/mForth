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

#ifndef FORTHCONFIG_H
#define FORTHCONFIG_H


// Define this to use a local malloc function
// i.e. not the system malloc
#define LOCAL_MALLOC
#undef  LOCAL_MALLOC

// saves some memory not to have help
#define USE_HELP
#undef  USE_HELP


// this needs to be defined to use on a cortex m3
#define M3FIX
 #undef M3FIX

// for a PC based version, probably needs some fixup if this is going to be done
#define PC
#undef PC


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
#define DEFINES1_NAME_MAX 250

#define TWO_WORD_SOLUTION
#undef TWO_WORD_SOLUTION

typedef enum
{
   ROOT_USER_ID,
}ForthContextIDType;

#endif
