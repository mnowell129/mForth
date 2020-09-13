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
#ifndef ASSWORDS_H
#define ASSWORDS_H

Cell popsafe(UserStatePtr user, bool *wouldJump);
void resetStack(UserStatePtr user);


void push(UserStatePtr user,Cell value);
Cell depth(UserStatePtr user);
Cell pop(UserStatePtr user);
Cell top(UserStatePtr user);
void dupp(UserStatePtr user);
void rot(UserStatePtr user);
void swap(UserStatePtr user);
void at(UserStatePtr user);
void bang(UserStatePtr user);
void charbang(UserStatePtr user);
void _charat(UserStatePtr user);
void charat(UserStatePtr user);
void wordat(UserStatePtr user);
void wordbang(UserStatePtr user);
void or(UserStatePtr user);
void oror(UserStatePtr user);
void drop(UserStatePtr user);
void shl(UserStatePtr user);
void shr(UserStatePtr user);
void plus(UserStatePtr user);
void minus(UserStatePtr user);
void times(UserStatePtr user);
void divide(UserStatePtr user);
void divideSigned(UserStatePtr user);
void multiplySigned(UserStatePtr user);
void help(UserStatePtr user);
void dots(UserStatePtr user);
void and(UserStatePtr user);
void andand(UserStatePtr user);
void not(UserStatePtr user);
void lnot(UserStatePtr user);

#endif
