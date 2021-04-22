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
#include <ctype.h>
#include <setjmp.h>
#include <string.h>

//#include <intrinsics.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "type.h"
#include "bits.h"
#include "ftypes.h"
#include "asswords.h"
#include "words.h"
#include "forth.h"
#include "corewords.h"
#include "dos.h"
#include "float.h"


int32_t  iff(UserStatePtr user)
{
   return((int32_t)pop(user));
}

void tickeval(UserStatePtr user)
{
   push(user,(Cell)(&(USER_VAR(user,TICKEVAL))));
}
void csp(UserStatePtr user)
{
   push(user,(Cell)(&(USER_VAR(user,CSP))));
}

void handler(UserStatePtr user)
{
   push(user,(Cell)(&(USER_VAR(user,HANDLER))));
}

void hld(UserStatePtr user)
{
   /* user variable */
   push(user,(Cell)(&(USER_VAR(user,HLD))));
}
void base(UserStatePtr user)
{
   push(user,(Cell)(&(USER_VAR(user,BASE))));
}

void ptib(UserStatePtr user)
{
   push(user,(Cell)(&(USER_VAR(user,PTIB))));
}

void getUserBase(UserStatePtr user)
{
   PUSH(user,user->userVariables.indexedVariables);
}

void hold(UserStatePtr user)
{
   hld(user);
   at(user);
   push(user,1);
   minus(user);
   dupp(user);
   hld(user);
   bang(user);
   charbang(user);
}


void lessPound(UserStatePtr user)
{
   // make corrections for null terminated strings
   pad(user);
   hld(user);
   bang(user);
   #ifdef LONG_STRING_COUNT
   // stick a null down
   push(user,0);
   hold(user);
   #endif
}

void pound(UserStatePtr user)
{
   base(user);
   at(user);
   extract(user);
   hold(user);
}


// modify #s to remove the value
void poundS(UserStatePtr user)
{
   while(1)
   {
      pound(user);
      if(top(user) == 0) break;
   }
   drop(user);
} /* pounds */

#ifdef USE_FLOAT
void floatPoundS(UserStatePtr user)
{
   char buffer[40];
   int width,frac;
   Floater floater;
   Float32 value;
   int length;
   int i;
   int j;

   floater.int32Value = UPOP();
   value = floater.float32Value;
   frac = UPOP();
   width = UPOP();
   length = sprintf(buffer,"%*.*f",width,frac,value);
   j = length - 1;
   for(i = 0;i < length;i++)
   {
      UPUSH(buffer[j]);
      hold(user);
      j--;
   }
} /* pounds */


// this version uses the default formats
void floatPoundSFree(UserStatePtr user)
{
   char buffer[40];
   Floater floater;
   Float32 value;
   int length;
   int i;
   int j;

   floater.int32Value = UPOP();
   value = floater.float32Value;
   length = sprintf(buffer,"%f",value);
   j = length - 1;
   for(i = 0;i < length;i++)
   {
      UPUSH(buffer[j]);
      hold(user);
      j--;
   }
} /* pounds */


// this version uses the %E format
void floatPoundSE(UserStatePtr user)
{
   char buffer[40];
   Floater floater;
   Float32 value;
   int length;
   int i;
   int j;

   floater.int32Value = UPOP();
   value = floater.float32Value;
   length = sprintf(buffer,"%e",value);
   j = length - 1;
   for(i = 0;i < length;i++)
   {
      UPUSH(buffer[j]);
      hold(user);
      j--;
   }
} /* pounds */


#endif

void sign(UserStatePtr user)
{
   SignedCell val;
   val = (SignedCell)pop(user);
   if(val < 0)
   {
      push(user,(Cell)'-');
      hold(user);
   }
} /* sign */
void poundGreater(UserStatePtr user)
{
   // remove this so multiple values may be passed, user responsibility to drop them
   //     drop(user); // this drops the number being converted, or whats left of it.
   //
   hld(user);
   at(user);
   pad(user);
   over(user);
   minus(user);
   #ifdef LONG_STRING_COUNT
   // strings need to be null terminated
   #endif
} /* poundGreater */



void type(UserStatePtr user)
{
   SignedCell count;
   char *ptr;
   int i;
   count = (SignedCell)pop(user);
   #ifdef LONG_STRING_COUNT
   count--; // don't print the null
   #endif
   ptr = (char *)pop(user);
   for(i = 0;i < count;i++)
   {
      PUTCH(*ptr++);
   }
} /* type */

void count(UserStatePtr user)
{
   /* expects a pointer to a counted string */
   char *ptr;
   ptr = (char *)pop(user);
   PUSH(user,ptr + 1);
   PUSH(user,LENGTH_MASK & (*ptr));
}


// converts a cell index in an array into the byte offset
// then adds to the base that is expected
// ( base index -- base + index * cellsize )
void toByteAddress(UserStatePtr user)
{
   uint8_t *address;
   SignedCell index;   // use signed since we may want to do negative indexing

   index = (SignedCell)pop(user);
   address = (uint8_t *)pop(user);
   address += index * sizeof(Cell);
   PUSH(user,address);
}

void hex(UserStatePtr user)
{
   USER_VAR(user,BASE) = 16;
}
void decimal(UserStatePtr user)
{
   USER_VAR(user,BASE) = 10;
}

void emit(UserStatePtr user);
void space(UserStatePtr user)
{
   push(user,32);
   emit(user);
}

void toR(UserStatePtr user);
void fromR(UserStatePtr user);
void absolute(UserStatePtr user);
void sign(UserStatePtr user);

void udot(UserStatePtr user)
{
   lessPound(user);
   poundS(user);
   poundGreater(user);
   space(user);
   type(user);
}

void udotNoSpace(UserStatePtr user)
{
   lessPound(user);
   poundS(user);
   poundGreater(user);
   // space(user);
   type(user);
}

void dot(UserStatePtr user)
{
   if((USER_VAR(user,BASE)) != 10)
   {
      udot(user);
      return;
   }
   dupp(user);
   toR(user);
   absolute(user);
   lessPound(user);
   poundS(user);
   fromR(user);
   sign(user);
   poundGreater(user);
   space(user);
   type(user);
}


void dotNoSpace(UserStatePtr user)
{
   if((USER_VAR(user,BASE)) != 10)
   {
      udotNoSpace(user);
      return;
   }
   dupp(user);
   toR(user);
   absolute(user);
   lessPound(user);
   poundS(user);
   fromR(user);
   sign(user);
   poundGreater(user);
   // space(user);
   type(user);
}



#ifdef USE_FLOAT
// Floating point to ascii converter
// Need the width and precision
//

// Numbers are printed W.P unless too big for the space
// Then they are printed W.PE+/-DD
void floatToAscii(float value, int width, int precision, char *output)
{
   int wholeDigits = width - precision - 1;
   float maxForWidth = powf(10.0,(float)wholeDigits);
   float minForFraction = powf(10.0,-(float)precision);
   float fraction;
   float whole;
   int   exponent;
   //    int   exponentDigits;
   int   wholeI;
   char buffer[40];
   int index;
   int digit;
   uint8_t doE = 0;
   int  eValue;

   if(value < 0)
   {
      *output++ = '-';
      value = -value;
   }
   if((value > maxForWidth) || (value < minForFraction))
   {
      // won't fit, go with exponential form
      //        strcpy(output,"Not yet");
      fraction = frexpf(value,&exponent);
      eValue = (int)(log10(powf(2.0,exponent)));
      value /= powf(10.0,eValue);
      doE = 1;
   }
   // number is in range
   fraction = modff(value,&whole);
   index = 0;
   wholeI = (int)value;
   if(wholeI == 0)
   {
      buffer[index++] = '0';
   }
   else
   {
      while(wholeI)
      {
         buffer[index++] = '0' + (wholeI % 10);
         wholeI /= 10;
      }
   }
   index--;
   for(;index >= 0;index--)
   {
      *output++ = buffer[index];
   }
   // now the decimal
   *output++ = '.';
   // now the fractional part
   while(precision--)
   {
      fraction *= 10;
      digit = (int)(fraction);
      *output++ = '0' + digit;
      fraction = fraction - (float)digit;
   }
   if(doE)
   {
      sprintf(output,"E%d",eValue);
   }
   else
   {
      // terminating null
      *output++ = 0;
   }
}

#endif
void toR(UserStatePtr user)
{
   rpPush(user,(SignedCell)pop(user));
} /* toR */

void fromR(UserStatePtr user)
{
   push(user,(Cell)rpPop(user));
} /* fromR */


void plusBang(UserStatePtr user)
{
   Cell *value;
   Cell increment;

   value = (Cell *)pop(user);
   increment = pop(user);
   *value += increment;
} /* plusBank */

void toIN(UserStatePtr user)
{
   push(user,(Cell)((Cell *)(user->userVariables.indexedVariables) + TO_IN));
}

/* implements the low level parse function like eforth */
/* but uses local params */
/* returns the length of the token found */
/* and its location */
SignedCell parse(char *inbuf, int16_t length, char delim,char **token,SignedCell *tokenLength)
{
   int delta = 0;
   int size = 0;
   char *buf = inbuf;

   /* find front */
   while((length > 0) && (*buf == delim) && (*buf != '\r') && (*buf != '\n'))
   {
      buf++;
      length--;
   } /* while */
   *token = buf;
   if(length <= 0)
   {
      *tokenLength = 0;
      return(0);
   }
   while((length > 0) && (*buf != delim) && (*buf != '\r') && (*buf != '\n'))
   {
      buf++;
      size++;
      length--;
   } /* while */

   *tokenLength = size;
   delta = (int16_t)(buf - inbuf + 1);
   /*   FPRINTF("%d %c %d LENGTH %d\r\n",*buf,*buf,delta,size);  */
   return(delta);
} /* parse */

/* parse functions like the eforth PARSE */
/* but uses clib funtions to get there */
/* will put a zero terminator at the end of */
/* a string to force the token search to terminate */
// Note, does not place the null termination.
// Just counts the actual number of characters between the token
void PARSE(UserStatePtr user)
{
   SignedCell delim;
   SignedCell length;
   SignedCell index;
   SignedCell tokenSize;
   SignedCell delta;

   char *ptr;
   char *token;
   delim = pop(user);



   /*FPRINTF(" in ptr %d %d\r\n",(int16_t)((Cell*)(user->userVariables.indexedVariables)[TO_IN]),(int16_t)((Cell*)(user->userVariables.indexedVariables)[PTIB])); */
   index = (SignedCell)(USER_VAR(user,TO_IN));
   ptr = user->tibBuffer + index;
   length = (SignedCell)(USER_VAR(user,PTIB)) - index;


   delta = parse(ptr,length,delim,&token,&tokenSize);
   index += delta;
   USER_VAR(user,TO_IN) = (Cell)index;
   push(user,(Cell)token);
   push(user,tokenSize);
   /* FPRINTF(" in ptr %d %d\r\n",(int16_t)((Cell*)(user->userVariables.indexedVariables)[TO_IN]),(int16_t)((Cell*)(user->userVariables.indexedVariables)[PTIB])); */
}


/* CMN : Feb 2012 */
/* modified version of parse that requires the delimiter to be on front and back */
/* of the item being parsed */
/* This is being done to allow a string operator that finds things in quotes that has */
/* spaces in front of it, so s$ "stuff" works as does   s$       "stuff" as does s$       "   stuff" */
/* so you can put spaces in literally and have spaces between the string word and the first quote */

/* implements the low level parse function like eforth */
/* but uses local params */
/* returns the length of the token found */
/* and its location */
SignedCell parseAlt(char *inbuf, int16_t length, char delim,char **token,SignedCell *tokenLength)
{
   int delta = 0;
   int size = 0;
   char *buf = inbuf;

   /* move forward until the delimiter is found */
   while((length > 0) && (*buf != delim) && (*buf != '\r') && (*buf != '\n'))
   {
      buf++;
      length--;
   } /* while */
   if(*buf != delim)
   {
      *tokenLength = 0;
      delta = (int16_t)(buf - inbuf + 1);
      return(delta);
   }
   // skip over the delimiter
   buf++;
   length--;
   *token = buf;
   if(length <= 0)
   {
      *tokenLength = 0;
      delta = (int16_t)(buf - inbuf + 1);
      return(delta);
   }
   while((length > 0) && (*buf != delim) && (*buf != '\r') && (*buf != '\n'))
   {
      buf++;
      size++;
      length--;
   } /* while */

   *tokenLength = size;
   delta = (int16_t)(buf - inbuf + 1);
   return(delta);
} /* parse */

/* parse functions like the eforth PARSE */
/* but uses clib funtions to get there */
/* will put a zero terminator at the end of */
/* a string to force the token search to terminate */
// Note, does not place the null termination.
// Just counts the actual number of characters between the token
void PARSEALT(UserStatePtr user)
{
   SignedCell delim;
   SignedCell length;
   SignedCell index;
   SignedCell tokenSize;
   SignedCell delta;

   char *ptr;
   char *token;
   delim = pop(user);



   /*FPRINTF(" in ptr %d %d\r\n",(int16_t)((Cell*)(user->userVariables.indexedVariables)[TO_IN]),(int16_t)((Cell*)(user->userVariables.indexedVariables)[PTIB])); */
   index = (SignedCell)(USER_VAR(user,TO_IN));
   ptr = user->tibBuffer + index;
   length = (SignedCell)(USER_VAR(user,PTIB)) - index;


   delta = parseAlt(ptr,length,delim,&token,&tokenSize);
   index += delta;
   USER_VAR(user,TO_IN) = (Cell)index;
   push(user,(Cell)token);
   push(user,tokenSize);
   /* FPRINTF(" in ptr %d %d\r\n",(int16_t)((Cell*)(user->userVariables.indexedVariables)[TO_IN]),(int16_t)((Cell*)(user->userVariables.indexedVariables)[PTIB])); */
}

void emit(UserStatePtr user)
{
   PUTCH((char)pop(user));
}

void key(UserStatePtr user)
{
   push(user,(Cell)GETCH());
}


/* ( a u a -- counted string ) */
void packDollar(UserStatePtr user)
{
   /* expects a string,count,address */
   /* makes a counted string out of the given */
   /* string at the new address */
   /* used by TOKEN and WORD */
   /* used to copy a name into the text space */
   char *address;
   int16_t length;
   char *source;
   address = (char *)pop(user);
   length = (int16_t)pop(user);

   /**
    * Allow for very long strings of data that aren't really 
    * strings and never will be. 
    * Instead of trying to copy them into the space 
    * just leave a pointer to them. 
    */
   source = (char *)pop(user);
   UPUSH(source);
   if(*source == '\'')
   {
      // someone is trying to build a binary string
      // if(USER_VAR(user,STATE) != INTERPRETING)
      // {
      //    FPRINTF("can't do dynamic in compile mode\r\n");
      //    // not in compile mode.
      //    longjmp(user->jumper,4);
      // }
      source = (char *)pop(user);
      #ifdef LONG_STRING_COUNT
      // don't make zero length strings 1 byte long of nothing
      if(length)
      {
         length++;
      }
      // special token
      address[0] = 255;
      memcpy(address + 1,source,length); /* plus 1 makes sure to copy a zero if it exists*/
      address[length] = 0; /* force a null terminated string */
      #else
      address[0] = 255;
      memcpy(address + 1,source,length + 1); /* plus 1 makes sure to copy a zero */
      address[length + 1] = 0; /* force a null terminated string */
      #endif

   }
   else
   {
      // reserve 255 for indeterminate length
      if((length < 0) || (length > 254))
      {
         longjmp(user->jumper,4);
      }
      source = (char *)pop(user);
      #ifdef LONG_STRING_COUNT
      // don't make zero length strings 1 byte long of nothing
      if(length)
      {
         length++;
      }
      address[0] = length;
      memcpy(address + 1,source,(LENGTH_MASK & length)); /* plus 1 makes sure to copy a zero if it exists*/
      address[(LENGTH_MASK & length)] = 0; /* force a null terminated string */
      #else
      address[0] = length;
      memcpy(address + 1,source,(LENGTH_MASK & length) + 1); /* plus 1 makes sure to copy a zero */
      address[(LENGTH_MASK & length) + 1] = 0; /* force a null terminated string */
      #endif
   }
   push(user,(Cell)address);
}


/* comment line */
void backSlash(UserStatePtr user)
{
   ptib(user);
   at(user);
   toIN(user);
   bang(user);
}
void leftParen(UserStatePtr user)
{
   PUSH(user,')');
   PARSE(user);
   drop(user);
   drop(user);
}

#if 1
void chaR(UserStatePtr user)
{
   if(USER_VAR(user,STATE) == INTERPRETING)
   {
      PUSH(user,' ');
      PARSE(user);
      drop(user);
      charat(user);
   }
   else
   {
      // compiling behavior
      PUSH(user,' ');
      PARSE(user);
      drop(user);
      charat(user);
      PUSH(user,doLIT);
      comma(user);
      comma(user); /* store the constant */
   }
}
#endif

/* pushes current code address */
void here(UserStatePtr user)
{
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   push(user,(uint32_t)(current->cp));
}
/* this function will adjust the code pointer (cp) */
/* or any other wordptr type object */
/* up to the the next wordptr boundary */
/* used by routines that put strings or word names in the dictionary */
/* so that the code pointer is correctly aligned */
/* length is the number of bytes to move */
/* and may be negative just for completeness */
//#define CELL_ALIGNMENT_MASK CELL_ALIGN_MASK
//#define CELL_BITS_TO_SHIFT  (CELL/2)
WordPtr* cellAlignedMove(WordPtr *wp,int16_t length);
WordPtr* cellAlignedMove(WordPtr *wp,int16_t length)
{
   if(!length)
   {
      return(wp);  /* nothing to do */
   }
   if(length < 0)
   {
      /* check to see if we are not an integer number of words */
      /* and add one word if not */
      if((-length) & CELL_ALIGNMENT_MASK)
      {
         wp -= ((-length) >> CELL_BITS_TO_SHIFT) + 1;
      }
      else
      {
         wp -= ((-length) >> CELL_BITS_TO_SHIFT);
      }
   }
   else
   {
      if(length & CELL_ALIGNMENT_MASK)
      {
         wp += (length >> CELL_BITS_TO_SHIFT) + 1;
      }
      else
      {
         wp += (length >> CELL_BITS_TO_SHIFT);
      }
   }
   return(wp);
}

/* leaves a pointer to a counted string */
/* already in the dictionary */
/* ( -- string pointer ) */
void tokeN(UserStatePtr user)
{
   /* parses a word from the input stream */
   /* copies it to the name dictionary */
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   push(user,32);
   PARSE(user);
   PUSH(user,(Cell)(current->cp));
   packDollar(user);
}

#if 0
/* parses a word from the input stream and copies to dictionary */
void word(UserStatePtr user)
{
   PARSE(user);
   here(user);
   packDollar(user);
}
#endif
/* copies a string from one place to the other */
/* but interprets the C escape sequences */
void copyAndConvert(char *destination,char *source, int16_t length);
void copyAndConvert(char *destination,char *source, int16_t length)
{
   typedef enum {SINGLE, ESCAPE, HEX}ConvertState;
   ConvertState state;
   char *temp;
   uint8_t digits = 0;
   char symbol;
   uint8_t value;

   temp = destination + 1;
   state = SINGLE;
   while(length-- > 0)
   {
      symbol = *source++;
      switch(state)
      {
         case HEX:
            if(isxdigit(symbol))
            {
               value = (value << 4) + (toupper(symbol) <= '9' ? symbol - '0' : 10 + toupper(symbol) - 'A');
               *temp = value; /* don't increment yet */
            }
            else
            {
               /* first non hex digit */
               temp++;
               digits++;
               /* do this part because next character may be an \ */
               source--; /* back up */
               length++; /* re-count character */
               state = SINGLE;
            }
            break;
         case ESCAPE :
            switch(symbol)
            {
               case 'x':
               case 'X':
                  state = HEX;
                  value = 0;
                  break;
               case 'n':
               case 'N':
                  *temp++ = '\n';
                  digits++;
                  state = SINGLE;
                  break;
               case 't':
               case 'T':
                  *temp++ = '\t';
                  digits++;
                  state = SINGLE;
                  break;
               case 'v':
               case 'V':
                  *temp++ = '\v';
                  digits++;
                  state = SINGLE;
                  break;
               case 'b':
               case 'B':
                  *temp++ = '\b';
                  digits++;
                  state = SINGLE;
                  break;
               case 'r':
               case 'R':
                  *temp++ = '\r';
                  digits++;
                  state = SINGLE;
                  break;
               case 'f':
               case 'F':
                  *temp++ = '\f';
                  digits++;
                  state = SINGLE;
                  break;
               case 'a':
               case 'A':
                  *temp++ = '\a';
                  digits++;
                  state = SINGLE;
                  break;
               default:
                  *temp++ = symbol;
                  digits++;
                  state = SINGLE;
                  break;
            }
            break;
         case SINGLE:
         default:
            if((symbol) != '\\')
            {
               digits++;
               *temp++ = symbol;
            }
            else
            {
               state = ESCAPE;
            }
            break;
      } /* switch */
   } /* while */
   /* if we left the while with a \xXX */
   /* and no last value then the state is stuck in hex */
   /* the character was written but the count and pointer not adjusted due to a no */
   /* non hex digit following */
   /* thus */
   if(state == HEX)
   {
      digits++;
      temp++;
   }

   *temp = 0; // null terminate
   #ifdef LONG_STRING_COUNT
   *destination = digits + 1;
   #else
   *destination = digits;
   #endif
} /* copy and Convert */
/* does what packdollar does but lets you use a C style string */
/* as the format so \n, etc will work */
/* note that this is called after parse() so the string is not */
/* null terminated yet */

void stringDollar(UserStatePtr user);
void stringDollar(UserStatePtr user)
{
   /* expects a string,count,address */
   /* makes a counted string out of the given */
   /* string at the new address */
   /* used by TOKEN and WORD */
   /* used to copy a name into the text space */
   char *address;
   int16_t length;
   char *source;

   address = (char *)pop(user);
   length = (int16_t)pop(user);
   source = (char *)pop(user);

   /* convert the source string with escapes */
   copyAndConvert(address,source,length);
   PUSH(user,(Cell)address);
}


#ifdef LONG_STRING_COUNT
#else
#endif

void _string(UserStatePtr user);
void _string(UserStatePtr user)
{
   int16_t length;
   uint8_t *ptr;
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   PARSE(user);
   /* advance */
   here(user);

   //    packDollar(user);
   // use this instead for "C" style strings
   stringDollar(user);
   dupp(user);
   ptr = (uint8_t *)pop(user);
   length = LENGTH_MASK & (*ptr);
   #ifdef LONG_STRING_COUNT
   current->cp = cellAlignedMove(current->cp,length + 1);
   #else
   /* the plus two accounts for a null termination on a C string */
   current->cp = cellAlignedMove(current->cp,length + 2);
   #endif
}

void fstring(UserStatePtr user);
void fstring(UserStatePtr user)
{
   PUSH(user,'"');
   _string(user);
}
/* determines if two strings are the same */
void sameQ(UserStatePtr user)
{
   char *one,*two;
   int16_t length;
   length = (int16_t)pop(user);
   one = (char *)item(user,1);
   two = (char *)item(user,0);
   //    if(length == 0)
   //    {
   //        while(1);
   //    }
   while(length)
   {
      if(*one++ != *two++)
      {
         push(user,(Cell)length);
         return;
      }
      length--; /* decremented here so as not to return 0 on */
      /* length 1 strings */
   }
   /* made it through */
   push(user,0);
}


void nameTo(UserStatePtr user)
{
   NameType *name;
   name = (NameType *)pop(user);
   PUSH(user,name->code);
} /* nameTo */

/* exchanges a name pointer with */
/* the pointer to the text string */

void nameToText(UserStatePtr user)
{
   NameType *name;
   name = (NameType *)pop(user);
   if(name != NULL)
   {
      PUSH(user,name->text);
   }
   else
   {
      PUSH(user,NULL);
   }
}
void dotId(UserStatePtr user)
{
   nameToText(user);
   dupp(user);
   if(iff(user))
   {
      count(user);
      type(user);
   }
   else
   {
      drop(user);
      FPRINTF("??");
   }
}


/* ( STRING DICTIONARY -- STRING 0 | CA TRUE ) */
void find(UserStatePtr user)
{
   /* expects a string and point to an array of names */
   NameType *names;
   char *searchFor;

   uint8_t len;
   names = (NameType *)pop(user);
   searchFor = (char *)pop(user);
   len = *searchFor++;

   //    if(len == 0)
   //    {
   //        FPRINTF("0");
   //    }
   while(names->text != NULL)
   {
      /* this statment depends on left to right evaluation */
      /* to speed the search */
      if((((names->text[0]) & LENGTH_MASK) == len))
      {
         /* found it */
         PUSH(user,&(names->text[1]));
         PUSH(user,searchFor);
         PUSH(user,len);
         sameQ(user);
         if(!pop(user))
         {
            drop(user);
            drop(user);
            PUSH(user,names->code);
            PUSH(user,names);
            return;
         }
         drop(user);
         drop(user);
      } /* if */
      names--;
   } /* while */
   push(user,(Cell)--searchFor);
   push(user,0);
} /* find */

volatile int junk;
void nameQ(UserStatePtr user)
{
   int wordListCount;
   WordList *searchWordList;
   void *top;

   top = (void *)(pop(user)); // get the name we want to look for
   wordListCount = USER_VAR(user,WORDLISTS);
   //    FPRINTF("%d..",wordListCount);
   while(wordListCount > 0)
   {
      if(wordListCount >= 3)
      {
         junk = 0;
      }
      else
      {
         junk = 1;
      }
      searchWordList = (WordList *)(USER_VAR(user,(WORDLIST0 + wordListCount - 1)));
      //	FPRINTF("search wordlist %x names %x %x\n",searchWordList,searchWordList->names,searchWordList->names+(searchWordList->last));
      PUSH(user,top); // put the name back on
      PUSH(user,searchWordList->names + (searchWordList->last)); // and wordlist
      find(user);
      dupp(user);
      if(iff(user))
      {
         return;
      }
      drop(user);
      drop(user);
      wordListCount--;
   }
   PUSH(user,top); // put string back on
   PUSH(user,0);   // and failed flag
}

// A slightly different nameq that returns the wordlist
// of the item if it is found
// used to avoid a duplicate search for the wordlist in
// help and forget words.
// returns 0 and the string if not found
// or returns wordlist and the actual element in the wordlist if found
void nameQW(UserStatePtr user)
{
   int wordListCount;
   WordList *searchWordList;
   void *top;

   top = (void *)(pop(user)); // get the name we want to look for
   wordListCount = USER_VAR(user,WORDLISTS);
   //    FPRINTF("%d..",wordListCount);
   while(wordListCount > 0)
   {
      searchWordList = (WordList *)(USER_VAR(user,(WORDLIST0 + wordListCount - 1)));
      //	FPRINTF("search wordlist %x names %x %x\r\n",searchWordList,searchWordList->names,searchWordList->names+(searchWordList->last));
      PUSH(user,top); // put the name back on
      PUSH(user,searchWordList->names + (searchWordList->last)); // and wordlist
      find(user);
      dupp(user);
      if(iff(user))
      {
         // it was found
         swap(user); // swap out the code pointer
         drop(user); // and drop it, leaving the name pointer
         PUSH(user,searchWordList);
         return;
      }
      drop(user);
      drop(user);
      wordListCount--;
   }
   PUSH(user,top); // put string back on
   PUSH(user,0);   // and failed flag
}


// makes floating point numbers

void floatQ(UserStatePtr user)
{
   Floater value;
   char *string;

   string = (char *)UPOP();
   string++;
   string++; // skip over the leading 'F'
   value.int32Value = 0;
   value.float32Value = atof(string);
   // now need some special operation here because
   // we don't want to convert to an integer
   UPUSH(value.int32Value);
   UPUSH(-1);
} //

// convert a string to a fixed point value
// The format is XD[D]: where D[D] is the number of digits
// used for the fractional part
// For example X23: is a 9.23 format
void fixedQ(UserStatePtr user)
{
} //

/*
Modified 2/11
To add fixed and floating point values
Integer math will work for fixed point, hence the value of fixed point.
Floating point will have to have special math operators including
trig and transcendentals.
Need special formatting routines for floating point that perhaps
pass something like F6.2 etc.
First problem is to parse the values.
Make things simple
Prefix floating points with F fixed point with X

*/
uint8_t contains(int32_t length,char *string,char letter)
{
   while(length--)
   {
      if((*string++) == letter) return(0xff);
   }
   return(0);
} //

void numberQ(UserStatePtr user)
{
   /* converts a string to an integer */
   /* expects a counted string */
   char *string;
   char *orgString;
   SignedCell sign = 0;
   int length;
   int  keyVal;
   SignedCell numberBase;
   SignedCell value = 0;



   orgString = string = (char *)pop(user);

   length = *string++;
   base(user);
   at(user);
   numberBase = (SignedCell)pop(user);
   if(toupper(*string) == 'F' && (contains(length,string,'.')))
   {
      #ifdef USE_FLOAT
      UPUSH(orgString);
      floatQ(user);
      #else
      PUSH(user,orgString);
      PUSH(user,0);
      #endif
      return;
   }
   if(*string == '0')
   {
      if((string[1] == 'x') || (string[1] == 'X'))
      {
         numberBase = 16;
         string += 2;
      }
      else
      {
         if((string[1] == 'b') || (string[1] == 'B'))
         {
            numberBase = 2;
            string += 2;
         }
      }
   }
   while((length) && (*string)) // stop on length or null termination
   {
      if(*string == '-')
      {
         sign = 1;
      }
      else
      {
         if((numberBase == 16 && isxdigit(*string)) || isdigit(*string))
         {
            keyVal = toupper(*string);
            if(keyVal > '9')
            {
               keyVal -= 7; /* adjust for hex characters */
            }
            value *= numberBase;
            value += keyVal - '0';
         } /* if */
         else
         {
            /* non digit encountered */

            PUSH(user,orgString);
            PUSH(user,0);
            return;
         }
      } /* else */
      string++;
      length--;
   } /* while */
   if(sign)
   {
      PUSH(user,-value);
   }
   else
   {
      PUSH(user,value);
   }
   PUSH(user,-1);
}





void qDup(UserStatePtr user)
{
   if(top(user))
   {
      push(user,top(user));
   }
}


void absolute(UserStatePtr user)
{
   //    PUSH(user,labs(pop(user)));
   PUSH(user,abs(pop(user)));
}

void negate(UserStatePtr user)
{
   PUSH(user,-pop(user));
}

void equals(UserStatePtr user)
{
   SignedCell a,b;
   a = (SignedCell)pop(user);
   b = (SignedCell)pop(user);
   PUSH(user,(a == b ? -1 : 0));
}
void spat(UserStatePtr user)
{
   PUSH(user,user->userVariables.namedVariables.stackPtr);
}

void spstore(UserStatePtr user)
{
   Cell val;
   val = (Cell)pop(user);
   (user->userVariables.namedVariables.stackPtr) = (Cell *)val;
}
void spzero(UserStatePtr user)
{
   PUSH(user,user->userVariables.namedVariables.stackStart);
}
void rpzero(UserStatePtr user)
{
   PUSH(user,user->userVariables.namedVariables.rpStart);
}
/* try a simple word */
void rpat(UserStatePtr user)
{
   PUSH(user,user->userVariables.namedVariables.rp);
}
void rat(UserStatePtr user)
{
   PUSH(user,*(user->userVariables.namedVariables.rp));
}
void rpstore(UserStatePtr user)
{
   user->userVariables.namedVariables.rp = (WordPtr **)pop(user);
}
void allot(UserStatePtr user)
{
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   //   FPRINTF(" cp : %x ",current->cp);
   current->cp += (int16_t)pop(user);
   //   FPRINTF(" cp : %x \r\n",current->cp);
}

/* lay a word down in dictionary */
void comma(UserStatePtr user)
{
   Cell val;
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);

   val = (Cell)pop(user);
   *(current->cp)++ = (WordPtr)val;
}

void leftBracket(UserStatePtr user)
{
   #ifdef GLOBAL_DOS
   PUSH(user,dollarInterpret);
   #else
   PUSH(user,user->dollarInterpret);
   #endif
   tickeval(user);
   bang(user);
   USER_VAR(user,STATE) = INTERPRETING;
}

/* unconditional branch */
void arrayBranch(UserStatePtr user)
{
   // push the address of the array
   PUSH(user,((WordPtr *)(user->userVariables.namedVariables.ip)) + 1);
   #ifdef RELATIVE
   user->userVariables.namedVariables.ip += (int)((WordPtr *)*(user->userVariables.namedVariables.ip));
   #else
   user->userVariables.namedVariables.ip = (WordPtr *)*(user->userVariables.namedVariables.ip);
   #endif
}

/* compile a forward branch */
void arrayAhead(UserStatePtr user)
{
   if(USER_VAR(user,STATE) == INTERPRETING)
   {
      USER_VAR(user,ARRAY_INDEX) = 0;
   }
   else
   {
      PUSH(user,arrayBranch);
      comma(user);
      here(user);
      PUSH(user,0);
      comma(user);
   }
}

// Consolation to inline arrays
void arrayStart(UserStatePtr user)
{
   #ifdef GLOBAL_DOS
   PUSH(user,dollarArray);
   #else
   PUSH(user,user->dollarArray);
   #endif
   push(user,(Cell)(&(USER_VAR(user,TICKEVAL))));
   bang(user);
   if(USER_VAR(user,STATE) == INTERPRETING)
   {
      USER_VAR(user,ARRAY_INDEX) = 0;
   }
   else
   {
      // when compiling do other stuff
      // lay down the runtime array word
      // also keep up with here
      // so array end can back annotate
      // compile a forward branch, leave here on the stack
      arrayAhead(user);
   }
} //

void arrayEnd(UserStatePtr user)
{
   if(USER_VAR(user,STATE) == INTERPRETING)
   {
      // push the address of the array
      PUSH(user,USER_VAR(user,STRING_TEMP));
      // resume interpreting
      leftBracket(user);
   }
   else
   {
      // if compiling just lay down the jump to here
      theN(user);
      rightBracket(user);
   }
}

// put an element into the temporary array
void arrayPut(UserStatePtr user)
{
   if(USER_VAR(user,STATE) == INTERPRETING)
   {
      if(USER_VAR(user,ARRAY_INDEX) < USER_VAR(user,PAD_SIZE))
      {
         //        user->tempString[USER_VAR(user,ARRAY_INDEX)] = UPOP();
         user->padBuffer[USER_VAR(user,ARRAY_INDEX)] = UPOP();
         USER_VAR(user,ARRAY_INDEX)++;
      }
      else
      {
         UPOP();
      }
   }
   else
   {
      // compiling
      comma(user);
   }
}




void rightBracket(UserStatePtr user)
{
   #ifdef GLOBAL_DOS
   PUSH(user,dollarCompile);
   #else
   PUSH(user,user->dollarCompile);
   #endif
   tickeval(user);
   bang(user);
   USER_VAR(user,STATE) = COMPILING;
}

/* expects the results of TOKEN */
/* which is a counted address */
/* already in the dicationary */
/* this function just has to */
/* add the name to the name list */
/* patch the code pointer and update */
/* the textLast variable */
void dollarCommaN(UserStatePtr user)
{
   char *name;
   int length;

   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);


   dupp(user);
   charat(user);
   if(iff(user))
   {
      /* name is already in the name text space */
      /* but not in dictionary */
      name = (char *)pop(user);
      length = *name;
      //        (current->last)++; /* move up to next name in dictionary */
      // length +2 used to be for the bytecount and the null
      // now perhaps token returns the right value? and this can be dropped
      // CMN:
      /**
      @todo possibly remove +2
      **/
      #ifdef LONG_STRING_COUNT
      current->cp = cellAlignedMove(current->cp,length + 1);
      #else
      current->cp = cellAlignedMove(current->cp,length + 2);
      #endif
      // modified to add the name but not increment
      // last, so you can't find yourself in the dictionary
      current->names[current->last + 1].text = name;
      #ifdef M3FIX
      // For cortex m3 code pointers have to be odd
      current->names[current->last + 1].code = (WordPtr)(1 | ((uint32_t)(current->cp)));
      #else
      current->names[current->last + 1].code = (WordPtr)(1 | ((uint32_t)(current->cp)));
      #endif
      #ifdef USE_HELP
      current->names[current->last + 1].help = NULL;
      #endif

      current->names[current->last + 2].code = NULL;

      //      names[(user->userVariables.namedVariables.last)].text = name;
      //      names[(user->userVariables.namedVariables.last)].code = (WordPtr)(user->userVariables.namedVariables.cp);
      //      names[(user->userVariables.namedVariables.last)+1].code = NULL;   /* mark one past end as the end */

   }
   else
   {
      /* zero length name */
      FPRINTF("aborting $,n ");
      longjmp(user->jumper,DOLLARCOMMA);
   }
}


void dollarCommaNFinish(UserStatePtr user)
{
   //    char *name;
   //    int length;

   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   (current->last)++; /* move up to next name in dictionary */
}

void colonComma(UserStatePtr user)
{
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   #if TWO_WORD_SOLUTION
   *current->cp++ = (WordPtr)THREAD_MASK;
   #endif
   *current->cp++ = (WordPtr)THREAD_HEADER;
}

void variableComma(UserStatePtr user)
{
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   #if TWO_WORD_SOLUTION
   *current->cp++ = (WordPtr)THREAD_MASK;
   #endif
   *current->cp++ = (WordPtr)VAR_HEADER;
}
#if 0
void createComma(UserStatePtr user)
{
   uint8_t    *cpByte;
   WordList *current;

   current = (WordList*)(user->userVariables.indexedVariables[CURRENT]);
   cpByte = (uint8_t *)(current->cp);
   memcpy(cpByte,FUNCTION(jumpDoes),headerSize);
   current->cp = (WordPtr*)(cpByte+headerSize);
}
#endif
void constantComma(UserStatePtr user)
{
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
    #if TWO_WORD_SOLUTION
    *current->cp++ = (WordPtr)THREAD_MASK;
    #endif
   *current->cp++ = (WordPtr)CONST_HEADER;
}

void variable(UserStatePtr user)
{
   tokeN(user);
   dollarCommaN(user);
   variableComma(user);
   PUSH(user,0);
   comma(user);
   dollarCommaNFinish(user);
   /* note NO rightbracket because the compile is done */
}

/*
Try and make a create does> function
first need to make a create.
Create does the first half of variable 
that is 
tokeN 
dollarCommaN 
variableComma 
then should do "overt" to make sure we move to the next name in the dictionary. 
Then the stuff following create is done, suppose that this is an alloc. 
this moves the cp pointer up. 
Now we have to do the does part of the word which is basically moving the IP 
to the does part of the defining word. 
 
What needs to happen is when the defining word runs 
it creates a new name in the space and a new code pointer in the table. 
But leave a space after the jumpVariable for another header that does jumpDoes. 
Hmm takes more thought to get this done in C vs assembly. 
 
Working backwards the final word should have a dolist header that calls the 
variable word itself and then to the does part. 
The name of the word is the variable name itself. 
This means a change in the way variable works all the time. 
Meaning that compiling a variable lays down a pointer to somewhere followed 
by exit, or just exit and doVar not only pushes the pointer to a couple of places 
forward it also nests and points to the does or exit function. 
 
So create lays all this down, and does takes the last definition in the dictionary 
and assumes it was made with create and patches the jump to the place just past does>. 
 
This makes variable a lot more expensive at runtime, probably not not worth it. 
But maybe as an experiment. 
 
Yes more thought indeed. 

*/

void constant(UserStatePtr user)
{
   tokeN(user);
   dollarCommaN(user);
   constantComma(user);
   /* now store the constant on top of stack in the word */
   comma(user);
   dollarCommaNFinish(user);

   /* note NO rightbracket because the compile is done */
}

void colon(UserStatePtr user)
{
   tokeN(user);
   dollarCommaN(user);
   colonComma(user);
   rightBracket(user);
   spat(user);
   csp(user);
   bang(user);
}

/* COMPILE says that as you are executing the word with COMPILE in it */
/* compile the following word into the word that is being compiled */
/* [COMPILE] says that as compiling the current word */
/* compile the the next word immediately even if it is immediate */
/* to reiterate COMPILE is a runtime word */
/* [COMPILE] is an compile time word */

/* example use of COMPILE is to compile the following word into a definition */
void compile(UserStatePtr user)
{
   PUSH(user,*(user->userVariables.namedVariables.ip)++);
   comma(user);
}



void semiColon(UserStatePtr user)
{
   PUSH(user,exiT);
   comma(user);
   leftBracket(user);
   spat(user);
   csp(user);
   at(user);
   equals(user);
   if(!iff(user))
   {
      FPRINTF("WARNING : mismatched control structure in definition\r\n");
   }
   // now finish off the definition by incrementing the last variable
   dollarCommaNFinish(user);
}

void cell(UserStatePtr user)
{
   push(user,CELL);
} //

/********************************************************/
/* COMPILING WORDS */
/********************************************************/
/* start with the simple */

void iF(UserStatePtr user)
{
   PUSH(user,qbranch);
   comma(user);  /* lay down the BRANCH */
   here(user);
   PUSH(user,0);  /* put an empty hole in */
   comma(user);
}

// this back annotates the jump on a if
void theN(UserStatePtr user)
{
   here(user); // dest here --
   over(user); // dest here dest -- // this makes it relative
   minus(user); // dest here-dest -- // this makes it relative
   cell(user);
   divideSigned(user); // this is done signed so backward branches work right
   swap(user); // here dest --
   //    push(user,2); // allow for the fact that the ip is already incremented
   //    plus(user);
   bang(user);
}

/* compile a forward branch */
void ahead(UserStatePtr user)
{
   PUSH(user,branch);
   comma(user);
   here(user);
   PUSH(user,0);
   comma(user);
}
void elsE(UserStatePtr user)
{
   ahead(user);  /* execute ahead , compiling a forward branch */
   swap(user);
   theN(user);
}

/* note that COMPILE >R */
/* is accomplished by PUSH(user,toR); comma(user); */

void foR(UserStatePtr user)
{
   PUSH(user,toR);
   comma(user);
   here(user);
}



void eye(UserStatePtr user)
{
   PUSH(user,rpTop(user));
}
/* for a nested do loop */
/* jay will access the index of the outer loop */
/* the return stack at this point */
/* has :
term count outer
count outer
term count inner
count inner
So : jay returns the 0,1,2 item on the return stack */

void jay(UserStatePtr user)
{
   PUSH(user,rpItem(user,2));
}

void nexT(UserStatePtr user)
{
   PUSH(user,next);
   comma(user);
   // Address on top of stack is the target of the jump
   here(user);      // dest here
   minus(user); // dest - here , negative, relative
   cell(user);
   divideSigned(user);
   comma(user);
}

void dO(UserStatePtr user)
{
   PUSH(user,swap);
   comma(user);
   PUSH(user,toR);
   comma(user);
   PUSH(user,toR);
   comma(user);
   here(user);    /* push the top of the loop */
   PUSH(user,doLOOP);  /* loop top instruction */
   comma(user);
   here(user);       /* push the resolution address for the branch out */
   PUSH(user,0);
   comma(user);      /* leave a space for the branch out */


}

/* how to implement a break ? */
/* break should remove the two indices then */
/* branch to the out address */
/* the out address is stored at the jump out location */
/* that is the top of the stack when compiling the loop */
/* so dup it and compile a constant then an AT then an IP update */
/* base upon at */
void looP(UserStatePtr user)
{
   /* first resolve the branch out of the loop */
   swap(user); /* bring to top */
   PUSH(user,plusLoop);
   comma(user);
   here(user);      // dest here
   minus(user); // dest - here , negative, relative
   cell(user);
   divideSigned(user);
   comma(user); /* target of the branch */
   /* tos now has the branch out address */
   here(user);
   over(user); // dest here dest -- // this makes it relative
   minus(user); // dest here-dest -- // this makes it relative
   cell(user);  // convert to a cell count
   divideSigned(user); // cell count
   swap(user);
   bang(user);    /* stores here in the jump out of the loop */

}

void literal(UserStatePtr user)
{
   PUSH(user,doLIT);
   comma(user);
   comma(user); /* store the constant */
}


void qKey(UserStatePtr user)
{
   PUSH(user,KEYHIT() ? -1 : 0);
}


void agaiN(UserStatePtr user)
{
   PUSH(user,branch);
   comma(user);
   here(user);      // dest here
   minus(user); // dest - here , negative, relative
   cell(user);
   divideSigned(user);
   //    push(user,-2); // allow for the fact that the ip is already incremented
   //    minus(user);
   comma(user); // patch the jump
}
void begiN(UserStatePtr user)
{
   here(user);
}
void whilE(UserStatePtr user)
{
   iF(user);
   swap(user);
}
void repeaT(UserStatePtr user)
{
   agaiN(user);
   here(user); // dest here
   over(user); // dest here dest -- // this makes it relative
   minus(user); // dest here-dest -- // this makes it relative
   cell(user);
   divideSigned(user);
   //    push(user,2); // allow for the fact that the ip is already incremented
   //    minus(user);
   swap(user);  // here-dest dest --
   bang(user);
}

void lasT(UserStatePtr user)
{
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   /* returns the pointer to the last name in the dictionary */
   PUSH(user,&(current->names[current->last]));
}


void immediate(UserStatePtr user)
{
   lasT(user);
   nameToText(user);
   dupp(user);
   charat(user);
   PUSH(user,IMMEDIATE);
   or(user);
   swap(user);
   charbang(user);
}

/*********************************************************************/
/* STRING words */
/*********************************************************************/
/* runtime for string output */
/* run time for $," */
/* this routine gets called via *ip++ */
/* so that the ip is pointing to the string address */
// see notes about ABORT" in forth.c
void doDollar(UserStatePtr user)
{
   WordPtr *temp;
   uint8_t    length;
   fromR(user);  /* return address of routine calling dodollar */
   dupp(user);   /* now we have two copies of the string pointer */
   /* back in the word that called us */
   temp = (WordPtr *)pop(user);  /* this is now the pointer to the string */
   /* that needs to be advanced */
   length = LENGTH_MASK & (*((uint8_t *)temp));
   #ifdef LONG_STRING_COUNT
   temp = cellAlignedMove(temp,length + 1);
   #else
   temp = cellAlignedMove(temp,length + 2);
   #endif

   PUSH(user,temp);
   toR(user);    /* put the modified IP on */
   /* leaving the address of the string on */

   // for single region the pointer is already pointing to the string
   //
}
/* RUNTIME ROUTINE FOR $" */
void dollarQuoteBar(UserStatePtr user)
{
   uint8_t length;
   PUSH(user,user->userVariables.namedVariables.ip);
   length = *((uint8_t *)(user->userVariables.namedVariables.ip));
   #ifdef LONG_STRING_COUNT
   /* plus 2 is for length byte trailing 0, already included in count*/
   user->userVariables.namedVariables.ip = (WordPtr volatile *)cellAlignedMove((WordPtr*)(user->userVariables.namedVariables.ip),length+1);
   #else
   /* plus 2 is for length byte and trailing 0 */
   user->userVariables.namedVariables.ip = (WordPtr volatile *)cellAlignedMove((WordPtr*)(user->userVariables.namedVariables.ip),length+2);
   #endif
}

// this is the runtime for array[ when it is compile in
// advances the ip to beyond the length of the elements
// that have been layed down
void arrayBar(UserStatePtr user)
{
   uint8_t length;
   PUSH(user,user->userVariables.namedVariables.ip);
   length = *((uint8_t *)(user->userVariables.namedVariables.ip));
   #ifdef LONG_STRING_COUNT
   /* plus 2 is for length byte trailing 0, already included in count*/
   user->userVariables.namedVariables.ip = (WordPtr volatile *)cellAlignedMove((WordPtr*)(user->userVariables.namedVariables.ip),length+1);
   #else
   /* plus 2 is for length byte and trailing 0 */
   user->userVariables.namedVariables.ip = (WordPtr volatile *)cellAlignedMove((WordPtr*)(user->userVariables.namedVariables.ip),length+2);
   #endif
}

void dotQuoteBar(UserStatePtr user)
{
   uint8_t length;
   PUSH(user,user->userVariables.namedVariables.ip);  /* this is the address of the counted string */
   count(user);
   type(user);
   length = *((uint8_t *)(user->userVariables.namedVariables.ip));
   #ifdef LONG_STRING_COUNT
   user->userVariables.namedVariables.ip = (WordPtr volatile *)cellAlignedMove((WordPtr*)(user->userVariables.namedVariables.ip),length+1);  /* plus 2 is for length and trailing 0 */
   #else
   user->userVariables.namedVariables.ip = (WordPtr volatile *)cellAlignedMove((WordPtr*)(user->userVariables.namedVariables.ip),length+2);  /* plus 2 is for length and trailing 0 */
   #endif
}

/* compile time for ." */
void dotQuote(UserStatePtr user)
{
   PUSH(user,dotQuoteBar);
   comma(user);
   fstring(user);
   // string returns the address of the string at compile time
   // for the single region version we don't need the address
   // because it is inline
   // for the text elsewhere version
   // we need to lay down the address for runtime to pick up
   drop(user);
}
/* compile time for $" */
void dollarQuote(UserStatePtr user)
{
   PUSH(user,dollarQuoteBar);
   comma(user);
   fstring(user);
   // string returns the address of the string at compile time
   // for the single region version we don't need the address
   // because it is inline
   // for the text elsewhere version
   // we need to lay down the address for runtime to pick up
   drop(user);
}


void xor (UserStatePtr user)
{
      Cell a,b;
   a = (Cell)pop(user);
   b = (Cell)pop(user);
   PUSH(user,a^b);
}

void slashMod(UserStatePtr user)
{
   Cell a,b;
   b = (Cell)pop(user);
   a = (Cell)pop(user);
   PUSH(user,a / b);
   PUSH(user,a % b);
}
void mod(UserStatePtr user)
{
   Cell a,b;
   b = (Cell)pop(user);
   a = (Cell)pop(user);
   PUSH(user,a % b);
}

void pick(UserStatePtr user)
{
   int16_t itemNumber;
   itemNumber = pop(user);
   PUSH(user,item(user,itemNumber));
}
void twoDrop(UserStatePtr user)
{
   pop(user);
   pop(user);
}

void twoDup(UserStatePtr user)
{
   PUSH(user,item(user,1));
   PUSH(user,item(user,1));
}

void nip(UserStatePtr user)
{
   Cell t;
   t = pop(user);
   pop(user);
   PUSH(user,t);
}
