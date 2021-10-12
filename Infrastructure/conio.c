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
#include "conio.h"
#include "snprintf99.h"

#define UART0_BUFFER_SIZE  256
static char buffer0[UART0_BUFFER_SIZE];
void _printf0(char *format,va_list list)
{
   int length;
   _rpl_vsnprintf(&length,buffer0,UART0_BUFFER_SIZE,format,list);
   uart0Send((Byte *)buffer0,length);
}

void printf0(char *format,...)
{
   va_list list;
   va_start(list,format);
   _printf0(format,list);
   va_end(list);
}

void forthPrintf0(char *format,...)
{
   va_list list;

   va_start(list,format);
   _printf0(format,list);
   va_end(list);
}


