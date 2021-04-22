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
#include <stdint.h>
#include "type.h"

#include "bits.h"
#include "ftypes.h"
#include "asswords.h"
#include "words.h"
#include "corewords.h"
#include "forth.h"
#include "dos.h"

#define INLINE
#undef INLINE


#ifdef INLINE
#pragma inline
#endif
void jumpColon(UserStatePtr user)
{
    #ifdef GLOBAL_DOS
    (*(doColon))(user);
    #else
    (*(user->doColon))(user);
    #endif
}
#ifdef INLINE
#pragma inline
#endif
void jumpVariable(UserStatePtr user)
{
    #ifdef GLOBAL_DOS
    (*(doVariable))(user);
    #else
    (*(user->doVariable))(user);
    #endif
}

#ifdef INLINE
#pragma inline
#endif
void jumpConstant(UserStatePtr user)
{
    #ifdef GLOBAL_DOS
    (*(doConstant))(user);
    #else
    (*(user->doConstant))(user);
    #endif
}
#ifdef INLINE
#pragma inline
#endif
void dummy(UserStatePtr user)
{
    #ifdef GLOBAL_DOS
    (*(doConstant))(user);
    #else
    (*(user->doConstant))(user);
    #endif
}



#if 0
void jumpDoes(UserStatePtr user)
{
    #ifdef GLOBAL_DOS
    (*(doDoes))(user);
    #else
    (*(user->doDoes))(user);
    #endif
}
void jumpUser(UserStatePtr user)
{
    #ifdef GLOBAL_DOS
    (*(doUser))(user);
    #else
    (*(user->doUser))(user);
    #endif
}
#endif
