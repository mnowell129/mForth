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
#include <stdio.h>
#include "type.h"
#include <setjmp.h>
#include "ftypes.h"
#include <ctype.h>
#include <string.h>
#include "bits.h"
#include "asswords.h"
#include "words.h"
//#include "uart.h"
#include "stdarg.h"
//#include "iostd.h"


#ifdef GLOBAL_DOS
WordPtr doColon;
WordPtr doVariable;
WordPtr doConstant;
WordPtr doUser;
WordPtr doDoes; 
WordPtr doDefine; 
WordPtr  dollarCompile;
WordPtr  dollarInterpret;
#endif

//extern NameType names[];


// char     variableSpace[VARIABLE_SPACE_SIZE];
//#pragma data_alignment=4
//#pragma location="EMAC_DMA_RAM"
#pragma location="USB_DMA_RAM"
WordPtr  codeSpace[CODE_SPACE_SIZE];
// char     nameSpace[NAME_SPACE_SIZE];
//#pragma data_alignment=4
//#pragma location="USB_DMA_RAM"
// "EMAC_DMA_RAM"
//Cell   forthStack[STACK_SIZE+RETURN_STACK_SIZE];
//WordPtr  *returnStack[RETURN_STACK_SIZE];


/* now for some basic words */

/* expects ( ca -- ) */
/* executes the word at that address */
#if 0
void execute(UserStatePtr user)
 {
   WordPtr val;
   val = (WordPtr)pop(user);
   user->userVariables.namedVariables.current = (WordPtr)val;
   (*val)(user); 

 }/* execute */
void atExecute(UserStatePtr user)
 {
   WordPtr val;
   val = (WordPtr)pop(user);
   val = *val;
   user->userVariables.namedVariables.current = (WordPtr)val;
   (*val)(user); 

 }/* atExecute */
#else

extern void exiT(UserStatePtr user);
//WordPtr indirect[2] = {NULL,exiT};

#define FUNCTION2INT(a)  ((((int)a) & (-2)))

static volatile int badCall;
// If this calls a "c" function then
// main will do the call direct
void execute(UserStatePtr user)
 {
   WordPtr val;
   //   uint32_t temp;

   val = (WordPtr)(pop(user));
   *(user->userVariables.namedVariables.rp)-- = (WordPtr*)(user->userVariables.namedVariables.ip);
   //   indirect = user->userVariables.namedVariables.ipTemp;
   user->userVariables.namedVariables.ipTemp[0] = (WordPtr)(val);
   user->userVariables.namedVariables.ipTemp[1] = (WordPtr)exiT;
   user->userVariables.namedVariables.ip = user->userVariables.namedVariables.ipTemp;
   (*(user->userVariables.namedVariables.ip)++)(user);
 }/* execute */

void atExecute(UserStatePtr user)
 {
   Cell  *loc;

   loc = (Cell*)pop(user);
   PUSH(user,*loc);
   execute(user);
   
 }/* atExecute */
#endif



 /* primitives */

/* conditional branch */
void qbranch(UserStatePtr user)
 {
    Cell val;
    //    uint32_t *temp;
    //    uint32_t jump;
    val = pop(user);
    if(val==0)
     {
         // temp = (uint32_t *)(user->userVariables.namedVariables.ip);
         //   jump = *temp;
         
         #ifdef RELATIVE
         user->userVariables.namedVariables.ip += (int)((WordPtr*)*(user->userVariables.namedVariables.ip));
         #else
         user->userVariables.namedVariables.ip = (WordPtr*)*(user->userVariables.namedVariables.ip);
         #endif
     }
    else
     {
       (user->userVariables.namedVariables.ip)++;
     }
 }/* qbranch */

/* unconditional branch */
void branch(UserStatePtr user)
 {
     #ifdef RELATIVE
     user->userVariables.namedVariables.ip += (int)((WordPtr*)*(user->userVariables.namedVariables.ip));
     #else
     user->userVariables.namedVariables.ip = (WordPtr*)*(user->userVariables.namedVariables.ip);
     #endif
 }


void rpPush(UserStatePtr user,Cell value)
 {
   *(user->userVariables.namedVariables.rp)-- = (WordPtr*)value;
 }


// note rp stack moves down in memory towards the stack pointer
// the < 1 is because once 1 item is pushed the difference better be at least one
// since the pointer is pointing to the next free value

Cell rpPop(UserStatePtr user)
 {
   if(((SignedCell)(user->userVariables.namedVariables.rpStart)- ((SignedCell)(user->userVariables.namedVariables.rp))) < 1)
    {
      longjmp(user->jumper,RETURN_STACK_EMPTY);
    }
   return((Cell)*++(user->userVariables.namedVariables.rp));
 }

Cell rpTop(UserStatePtr user)
 {
   if(((SignedCell)(user->userVariables.namedVariables.rpStart)- ((SignedCell)(user->userVariables.namedVariables.rp))) < 1)
    {
      longjmp(user->jumper,RETURN_STACK_EMPTY);
    }
   return((Cell)*((user->userVariables.namedVariables.rp)+1));
 }

Cell  rpItem(UserStatePtr user,SignedCell value)
{
     // this makes up for the pointer pointing to first empty
     value++;
     if(((SignedCell)(user->userVariables.namedVariables.rpStart)- ((SignedCell)(user->userVariables.namedVariables.rp))) < value)
     {
         longjmp(user->jumper,RETURN_STACK_EMPTY);
     }
     return((Cell)(user->userVariables.namedVariables.rp)[(value)]);
}

/* returns the value item from stack w/o destroying it */
/* i.e. item(0) is the same as top */
Cell  item(UserStatePtr user,SignedCell value)
 {
     // make up for the fact that the stack points to
     // next open location, not last pushed
   value++;
   if(((user->userVariables.namedVariables.stackPtr) - (user->userVariables.namedVariables.stackStart)) < value)
    {
      longjmp(user->jumper,STACK_EMPTY);
    }
   return((Cell)(user->userVariables.namedVariables.stackPtr)[-(value)]);
 }


/* low level do loop stuff */
/* bottom of a for next loop */
void next(UserStatePtr user)
 {
    SignedCell  count;
    count = rpPop(user);
    count--;
    if(count > 0)
     {
       rpPush(user,count);
       #ifdef RELATIVE
       user->userVariables.namedVariables.ip += (int)((WordPtr*)*(user->userVariables.namedVariables.ip));
       #else
       user->userVariables.namedVariables.ip = (WordPtr*)*(user->userVariables.namedVariables.ip);
       #endif
     }
    else
     {
       /* loop is clean */
       (user->userVariables.namedVariables.ip)++;  /* skip over jump address */
     }
 }


void plusLoop(UserStatePtr user)
 {
    SignedCell  count;
    count = rpPop(user);
    rpPush(user,++count);
    #ifdef RELATIVE
    user->userVariables.namedVariables.ip += (int)((WordPtr*)*(user->userVariables.namedVariables.ip));
    #else
    user->userVariables.namedVariables.ip = (WordPtr*)*(user->userVariables.namedVariables.ip);
    #endif
 }

/* TOP of a do loop */
void doLOOP(UserStatePtr user)
 {
    SignedCell  count;
    count = rpPop(user);
    if(count < rpTop(user))
     {
       rpPush(user,count);
       (user->userVariables.namedVariables.ip)++;  /* skip over jump address */
     }
    else
     {
       /* loop is clean */
       rpPop(user);
       #ifdef RELATIVE
       user->userVariables.namedVariables.ip += (int)((WordPtr*)*(user->userVariables.namedVariables.ip));
       #else
       user->userVariables.namedVariables.ip = (WordPtr*)*(user->userVariables.namedVariables.ip);
       #endif
     }
 }

/* this is implemented inefficiently */
/* to let the stack overflow words take care */
/* of underflows */
void over(UserStatePtr user)
 {
   SignedCell a,b;
   a = pop(user);
   b = top(user);
   push(user,a);
   push(user,b);
 }

void cr(UserStatePtr user)
 {
   (*(user->userVariables.namedVariables.userPuts))("\r\n");
 }
void blank(UserStatePtr user)
 {
   push(user,32);
 }

/*********************************************************************/
/* text interpreter words */
/*********************************************************************/


/* note that pad is a temporary buffer */
/* and is used for number conversion */
/* typically HERE + 80 */
/* and is used as the END of the conversion buffer */
/* to avoid a use of PAD as a base instead of end */
/* i have doubled pad and return */
/* the middle of pad for the PAD word */

void pad(UserStatePtr user) 
 {
    push(user,(Cell)((user->padBuffer)+(user->userVariables.namedVariables.padSize)));
 }
void tib(UserStatePtr user)
  {
    push(user,(Cell)((user->tibBuffer)));
  }

void digit(UserStatePtr user)
 {
   SignedCell val;
   val = pop(user);
   if(val > 9)
    {
      val+=7;
    }
   val += '0';
   push(user,val);

 }/* digit */

void extract(UserStatePtr user)
 {
   Cell val;
   Cell base;
   Cell rem;
   base = pop(user);
   val = pop(user);

   if(base != 0)
    {
      rem = val % base;
      val = val / base ;
    }
   else
    {
      longjmp(user->jumper,BASE_0);
    }
   push(user,val);
   push(user,rem);
   digit(user);
 }/* extract */
void lessThan(UserStatePtr user)
 {
   SignedCell val1,val2;
   val1 = (SignedCell)pop(user);
   val2 = (SignedCell)pop(user);
   PUSH(user, val2 < val1 ? -1 : 0);
 }
void greaterThan(UserStatePtr user)
 {
   SignedCell val1,val2;
   val1 = (SignedCell)pop(user);
   val2 = (SignedCell)pop(user);
   PUSH(user, val2 > val1 ? -1 : 0);
 }
void lessThanZero(UserStatePtr user)
 {
   SignedCell val1;
   val1 = (SignedCell)pop(user);
   PUSH(user,  val1 < 0 ? -1 : 0);
 }
void greaterThanZero(UserStatePtr user)
 {
   SignedCell val1;
   val1 = (SignedCell)pop(user);
   PUSH(user,  val1 > 0 ? -1 : 0);
 }
void equalsZero(UserStatePtr user)
 {
   SignedCell val1;
   val1 = (SignedCell)pop(user);
   PUSH(user,  val1 == 0 ? -1 : 0);
 }

