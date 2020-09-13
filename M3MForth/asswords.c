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
#include "type.h"
#include <setjmp.h>
#include "ftypes.h"
#include <string.h>
#include "bits.h"
#include "asswords.h"


/***********************************************************/
/* these are primitive subroutines */
/* not called as forth words */


// this is a moving up stack
void push(UserStatePtr user,Cell value)
{
    *(user->userVariables.namedVariables.stackPtr)++ = value;
}

Cell pop(UserStatePtr user)
{
    if((user->userVariables.namedVariables.stackPtr) - (user->userVariables.namedVariables.stackStart) < 1)
    {
        longjmp(user->jumper,STACK_EMPTY);
    }
    return(*--(user->userVariables.namedVariables.stackPtr));
}

Cell popsafe(UserStatePtr user, bool *wouldJump)
{
    if((user->userVariables.namedVariables.stackPtr) - (user->userVariables.namedVariables.stackStart) < 1)
    {
        *wouldJump = true;
        return(0);
    }
    *wouldJump = false;
    return(*--(user->userVariables.namedVariables.stackPtr));
}
void resetStack(UserStatePtr user)
{
    user->userVariables.namedVariables.stackStart = user->userVariables.namedVariables.stackPtr = user->stack;
}




Cell depth(UserStatePtr user)
{
    Cell value;
    value = (Cell)((user->userVariables.namedVariables.stackPtr) - (user->userVariables.namedVariables.stackStart));
    return(value);
}//Cell depth(UserStatePtr user)


Cell top(UserStatePtr user)
{
    if((user->userVariables.namedVariables.stackPtr) - (user->userVariables.namedVariables.stackStart) < 1)
    {
        longjmp(user->jumper,STACK_EMPTY);
    }
    return(*((user->userVariables.namedVariables.stackPtr)-1));
}

/* forth low level words */
/* must adhere to interpreter spec */

void dupp(UserStatePtr user)
{
    push(user,top(user));
}


void at(UserStatePtr user)
{
    Cell *ptr;
    #ifdef SECURE
    uint32_t address;
    address = (uint32_t)pop(user);
    if((address <= ROM_END) || (address >= PERIPHERAL_START))
    {
        // trying to read flash or peripheral
        push(user,-1);
    }
    else
    {
        ptr = (Cell *)address;
        push(user,*ptr);
    }
    #else
    ptr = (Cell *)pop(user);
    push(user,*ptr);
    #endif
}/* at */
void bang(UserStatePtr user)
{
    Cell *ptr;
    ptr = (Cell*)pop(user);
    *ptr = pop(user);
}
void charbang(UserStatePtr user)
{
    Byte *ptr;
    ptr = (Byte *)pop(user);
    *ptr = (Byte)pop(user);
}
void charat(UserStatePtr user)
{
    Byte *ptr;
    #ifdef SECURE
    uint32_t address;
    address = (uint32_t)pop(user);
    if((address <= ROM_END) || (address >= PERIPHERAL_START))
    {
        // trying to read flash or peripheral
        push(user,0xff);
    }
    else
    {
        ptr = (Byte *)address;
        push(user,*ptr);
    }
    #else
    ptr = (Byte *)pop(user);
    push(user,*ptr);
    #endif
}
// unsecure version of charat
void _charat(UserStatePtr user)
{
    Byte *ptr;
    ptr = (Byte *)pop(user);
    push(user,*ptr);
}

void wordat(UserStatePtr user)
{
    Word16 *ptr;
    #ifdef SECURE
    uint32_t address;
    address = (uint32_t)pop(user);
    if((address <= ROM_END) || (address >= PERIPHERAL_START))
    {
        // trying to read flash or peripheral
        push(user,0xffff);
    }
    else
    {
        ptr = (Word16 *)address;
        push(user,*ptr);
    }
    #else
    ptr = (Word16 *)pop(user);
    push(user,*ptr);
    #endif
}
void wordbang(UserStatePtr user)
{
    Word16 *ptr;
    ptr = (Word16 *)pop(user);
    *ptr = (Word16)pop(user);
}


void dots(UserStatePtr user)
{
    Cell *tmp;
    int  i=0;
    char   *form = "[@Depth%2d]   %-20ld (0x%08lX)\r\n";
    tmp = user->userVariables.namedVariables.stackPtr;
    (*(user->userVariables.namedVariables.userPrintf))("\r\nStack (top down)\r\n");
    while(tmp > (user->userVariables.namedVariables.stackStart))
    {
        tmp--;
        (*(user->userVariables.namedVariables.userPrintf))(form,i++,*tmp,*tmp);
        //      form = "       %-20ld (%-8lX)\r\n";
    }
}

void and(UserStatePtr user)
{
    Cell value;
    value = pop(user);
    value &= pop(user);
    push(user,value);
}

void andand(UserStatePtr user)
{
    Cell value1;
    Cell value2;
    value1 = pop(user);
    value2 = pop(user);
    push(user,value1 && value2 ? -1 : 0 );
}

void or(UserStatePtr user)
{
    Cell value;
    value = pop(user);
    value |= pop(user);
    push(user,value);
}

void oror(UserStatePtr user)
{
    Cell value1;
    Cell value2;
    value1 = pop(user);
    value2 = pop(user);
    push(user,value1 || value2 ? -1 : 0 );
}

void not(UserStatePtr user)
{
    Cell value;
    value = pop(user);
    push(user,~value);
}

void lnot(UserStatePtr user)
{
    Cell value;
    value = pop(user);
    push(user,value ? 0 : -1 );
}

void drop(UserStatePtr user)
{
    pop(user);
}
void shl(UserStatePtr user)
{
    Cell value;
    Cell shval;
    shval = pop(user);
    value = pop(user);
    push(user,value<<shval);
}
void shr(UserStatePtr user)
{
    Cell value;
    Cell shval;
    shval = pop(user);
    value = pop(user);
    push(user,value>>shval);
    
}
void plus(UserStatePtr user)
{
    Cell value;
    value = pop(user);
    value += pop(user);
    push(user,value);
}
void minus(UserStatePtr user)
{
    Cell value,val2;
    value = pop(user);
    val2 = pop(user);
    push(user,val2-value);

}
void times(UserStatePtr user)
{
    Cell value,val2;
    value = pop(user);
    val2 = pop(user);
    push(user,val2*value);
}
void divide(UserStatePtr user)
{
    Cell value,val2;
    value = pop(user);
    val2 = pop(user);
    push(user,val2/value);
}

void divideSigned(UserStatePtr user)
{
    int value,val2;
    int result;
    value = (int)pop(user);
    val2 = (int)pop(user);
    result = val2/value;
    push(user,result);
}

void multiplySigned(UserStatePtr user)
{
    int value,val2;
    int result;
    value = (int)pop(user);
    val2 = (int)pop(user);
    result = val2*value;
    push(user,result);
}

void swap(UserStatePtr user)
{
    Cell val1,val2;
    val1 = pop(user);
    val2 = pop(user);
    push(user,val1);
    push(user,val2);
}

void rot(UserStatePtr user)
{
    Cell val1,val2,val3;
    val1 = pop(user);
    val2 = pop(user);
    val3 = pop(user);
    push(user,val2);
    push(user,val1);
    push(user,val3);
}



