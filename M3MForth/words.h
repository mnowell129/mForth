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
#ifndef WORDS_H
#define WORDS_H

//#define PUSH(u,x)   push((u),(Cell)(x))

//extern char     variableSpace[VARIABLE_SPACE_SIZE];
extern WordPtr  codeSpace[CODE_SPACE_SIZE];
//extern char     nameSpace[NAME_SPACE_SIZE];
extern Cell   forthStack[];
//extern WordPtr  *returnStack[RETURN_STACK_SIZE];

extern int headerSize;
extern int align;


Cell rpPop(UserStatePtr user);
Cell rpTop(UserStatePtr user);
Cell  item(UserStatePtr user,SignedCell value);


void dummy(UserStatePtr user);
void doLIST(UserStatePtr user);
void doLIT(UserStatePtr user);
void doVAR(UserStatePtr user);
void doCONST(UserStatePtr user);
void execute(UserStatePtr user);
void atExecute(UserStatePtr user);
void qbranch(UserStatePtr user);
void branch(UserStatePtr user);
void rpPush(UserStatePtr user,Cell value);
void next(UserStatePtr user);
void plusLoop(UserStatePtr user);
void doLOOP(UserStatePtr user);
void over(UserStatePtr user);
void cr(UserStatePtr user);
void blank(UserStatePtr user);
void pad(UserStatePtr user) ;
void tib(UserStatePtr user);
void digit(UserStatePtr user);
void extract(UserStatePtr user);
void lessThan(UserStatePtr user);
void greaterThan(UserStatePtr user);
void lessThanZero(UserStatePtr user);
void greaterThanZero(UserStatePtr user);
void equalsZero(UserStatePtr user);
Cell  rpItem(UserStatePtr user,SignedCell value);

#endif
