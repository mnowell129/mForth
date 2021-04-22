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
#ifndef COREWORDS_H
#define COREWORDS_H
int32_t  iff(UserStatePtr user);
void tickeval(UserStatePtr user);
void csp(UserStatePtr user);
void handler(UserStatePtr user);
void hld(UserStatePtr user);
void base(UserStatePtr user);
void ptib(UserStatePtr user);
void getUserBase(UserStatePtr user);
void lessPound(UserStatePtr user);
void hold(UserStatePtr user);
void pound(UserStatePtr user);
void poundS(UserStatePtr user);
void floatPoundS(UserStatePtr user);
void floatPoundSFree(UserStatePtr user);
void floatPoundSE(UserStatePtr user);
void sign(UserStatePtr user);
void poundGreater(UserStatePtr user);
void type(UserStatePtr user);
void count(UserStatePtr user);
void toByteAddress(UserStatePtr user);
void hex(UserStatePtr user);
void decimal(UserStatePtr user);
void space(UserStatePtr user);
void udot(UserStatePtr user);
void dot(UserStatePtr user);
void dotNoSpace(UserStatePtr user);
void fdot(UserStatePtr user);
void toR(UserStatePtr user);
void fromR(UserStatePtr user);
void plusBang(UserStatePtr user);
void toIN(UserStatePtr user);
SignedCell parse(char *inbuf, int16_t length, char delim,char **token,SignedCell *tokenLength);
void PARSE(UserStatePtr user);
void PARSEALT(UserStatePtr user);
void emit(UserStatePtr user);
void key(UserStatePtr user);


void packDollar(UserStatePtr user);
void backSlash(UserStatePtr user);
void leftParen(UserStatePtr user);
void chaR(UserStatePtr user);
void here(UserStatePtr user);
#define CELL_ALIGNMENT_MASK CELL_ALIGN_MASK 
#define CELL_BITS_TO_SHIFT  (CELL/2)
WordPtr *cellAlignedMove(WordPtr *wp,int16_t length);
void tokeN(UserStatePtr user);
void word(UserStatePtr user);
void copyAndConvert(char *destination,char *source, int16_t length);
void stringDollar(UserStatePtr user);
void _string(UserStatePtr user);
void fstring(UserStatePtr user);
void sameQ(UserStatePtr user);
void nameTo(UserStatePtr user);
void nameToText(UserStatePtr user);
void dotId(UserStatePtr user);
void find(UserStatePtr user);
void nameQ(UserStatePtr user);
void nameQW(UserStatePtr user);



void numberQ(UserStatePtr user);
void qDup(UserStatePtr user);
void absolute(UserStatePtr user);
void negate(UserStatePtr user);
void equals(UserStatePtr user);
void spat(UserStatePtr user);
void spstore(UserStatePtr user);
void spzero(UserStatePtr user);
void rpzero(UserStatePtr user);
void rpat(UserStatePtr user);
void rat(UserStatePtr user);
void rpstore(UserStatePtr user);

void allot(UserStatePtr user);
void comma(UserStatePtr user);
void leftBracket(UserStatePtr user);
void rightBracket(UserStatePtr user);

void arrayBranch(UserStatePtr user);
void arrayAhead(UserStatePtr user);
void arrayStart(UserStatePtr user);
void arrayEnd(UserStatePtr user);
void arrayPut(UserStatePtr user);


void dollarCommaN(UserStatePtr user);
void dollarCommaNFinish(UserStatePtr user);
void colonComma(UserStatePtr user);
void variableComma(UserStatePtr user);
void createComma(UserStatePtr user);
void constantComma(UserStatePtr user);
void variable(UserStatePtr user);

void constant(UserStatePtr user);
void colon(UserStatePtr user);
void compile(UserStatePtr user);
void semiColon(UserStatePtr user);
void cell(UserStatePtr user);
void iF(UserStatePtr user);
void theN(UserStatePtr user);
void ahead(UserStatePtr user);
void elsE(UserStatePtr user);
void foR(UserStatePtr user);
void eye(UserStatePtr user);
void jay(UserStatePtr user);
void nexT(UserStatePtr user);
void dO(UserStatePtr user);
void looP(UserStatePtr user);
void literal(UserStatePtr user);


void qKey(UserStatePtr user);
void agaiN(UserStatePtr user);
void begiN(UserStatePtr user);
void whilE(UserStatePtr user);
void repeaT(UserStatePtr user);
void lasT(UserStatePtr user);

void immediate(UserStatePtr user);
void doDollar(UserStatePtr user);
void dollarQuoteBar(UserStatePtr user);
void dollarQuote(UserStatePtr user);
void dotQuoteBar(UserStatePtr user);
void dotQuote(UserStatePtr user);
void xor(UserStatePtr user);
void slashMod(UserStatePtr user);
void mod(UserStatePtr user);
void pick(UserStatePtr user);
void twoDrop(UserStatePtr user);
void twoDup(UserStatePtr user);
void nip(UserStatePtr user);

#endif
