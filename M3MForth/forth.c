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
/* For use where printf and gets are available */
/* Implements a basic command set of basic functions */
/* like peek and poke */


/* Basic algorithm : 
    read a line of text
    parse into tokens seperated by white space (ALA FORTH )
    look up the tokens in the table of functions
    if token is found then execute that given function
    if not found convert the token to a number a push on a stack */

/* Preliminary words */
/* all words converted to uppercase for storage and comparison */
/* "@","!","C@","C!","W@","W!", HEX, DECIMAL, BINARY */
/*  .,H.,B.,AND,OR,NOT,DROP,<<,>>,+,-,*,/,DUP,SWAP,ROT */
#include <ctype.h>
#include <setjmp.h>
#include <string.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include <intrinsics.h>



#include "type.h"

#include "bits.h"
#include "ftypes.h"
#include "asswords.h"
#include "words.h"
#include "corewords.h"
#include "forth.h"
#include "dos.h"
#include "float.h"


// OS stuff for OSTimeDly
// can be removed
#ifdef USE_OSDELAY
   #include "freertos.h"
   #include "task.h"
#endif

/* combines text and code in a single space */

// leave this undefined
#define COMPILED_ROOT
#undef COMPILED_ROOT


#ifdef M3FIX
   #define  MAKEFUNCTIONCALL(a)  (WordPtr)(((uint32_t)a)|1);
#else
   #define MAKEFUNCTIONCALL(a)  (WordPtr)(((uint32_t)a));
#endif


// ******************************************************************************
// These are global to any context
// they just define how big the executable header is and what the alignment
// is. Alignment is 1, 2 or 4 indicating what boundary a pointer has to fall on.
// Header size is the size of the executable header rounded up to the nearest word.

/* a bit field variable */
//int32_t state;
int align;
int headerSize;

extern WordList defines1WordList;


// how to perform CREATE DOES>
// first the does> word has to leave an address that is compiled into the
// executable space of the word being defined
// and it must be the address
// What DOES> does at compile time is lays down doDoes
// with the current address passed in
// doDoes compiles in this address
// in the word being defined
// CREATE at compile time just compiles in doCreate
// which builds a word definition out of the following word
// and then quits
// CREATE : compile time, compiles docreate which just builds a word in the dictionary
//        : run time, build the next word in the dictionary
// DOES>  : compile time, compile dolit, compile here compile compileDoes
//        : run time,     take the address on the stack, add two cells to it, (skips over this call)
//                        then compile a call to do


// These are the global pointers that the executable headers used to jump
// to the primitive header words.
// Hmmm.. what would happen if we made the header the largest of all the doXXX
// words and just copied that executable in the first place, without the pointers.
// THis would save one jump.
// OOPS now I remember why you can't do this. The calls in the functions
// on the arm are relative and can't be moved. Thus the extra jump remains.





// These are the functions that are copied into the word headers as
// actual code.


// ******************************************************************************
// ******************************************************************************
// These are the interpreter header words that move the IP
// around when words are interpreted.
// ******************************************************************************
// ******************************************************************************

void doLIST(UserStatePtr user)
{
   uint32_t temp;
   /* header for colon word */
   /* needs to replace the IP */
   /* and push IP on RP */
   /* IP currently points to the calling word */
   /* which points to the word past this one */

   // rp moves down with combined stack and rp area
   //    *(user->userVariables.namedVariables.rp)-- = (WordPtr*)(user->userVariables.namedVariables.ip);
   // safer way, use function
   rpPush(user,(Cell)(user->userVariables.namedVariables.ip));
   // this line backs the ip up one, dereferences it to get to the c function we called
   // then make the ip point to the code field that is beyond the c header
   // headersize is word aligned
   #ifndef M3FIX
   user->userVariables.namedVariables.ip = (WordPtr *)((byte *)(*((WordPtr *)((user->userVariables.namedVariables.ip) - 1))) + headerSize); /* skip over the header */
   #else
   // Cortex M3 modification, ip dereferences to an odd value
   // not useful for constant and variable addresses
   temp = ~1 & ((uint32_t)(user->userVariables.namedVariables.ip[-1]));
   temp += headerSize;

   user->userVariables.namedVariables.ip = (WordPtr *)(temp);
   #endif
   /* and point IP to next token */
}

void doLIT(UserStatePtr user)
{
   push(user,(Cell)(*(user->userVariables.namedVariables.ip)++));
}

void doVAR(UserStatePtr user)
{
   //    WordPtr ptr;
   uint32_t temp;
   uint32_t *vPtr;

   //    ptr = (WordPtr)(user->userVariables.namedVariables.ip);
   //    ptr -= (WordPtr)sizeof(Cell);  // back up to the previous word
   //    ptr = (WordPtr)(((Byte *)ptr) - sizeof(Cell));  // back up to the previous word
   //    ptr = (WordPtr)*((WordPtr*)ptr); // follow it to a c function, that would be this one

   #ifdef M3FIX
   // Cortex M3 modification, ip dereferences to an odd value
   // not useful for constant and variable addresses
   temp = ~1 & ((uint32_t)(user->userVariables.namedVariables.ip[-1]));
   temp += headerSize;

   vPtr = (uint32_t *)temp;
   PUSH(user,vPtr);
   #else
   PUSH(user,((Cell *)((Byte *)(*((WordPtr *)((user->userVariables.namedVariables.ip) - 1))) + headerSize)));
   #endif
}

void doCONST(UserStatePtr user)
{
   //    WordPtr ptr;
   uint32_t temp;
   uint32_t *vPtr;

   //    ptr = (WordPtr)(user->userVariables.namedVariables.ip);
   //    ptr -= (WordPtr)sizeof(Cell);  // back up to the previous word
   //    ptr = (WordPtr)(((Byte *)ptr) - sizeof(Cell));  // back up to the previous word
   //    ptr = (WordPtr)*((WordPtr*)ptr); // follow it to a c function, that would be this one
   #ifdef M3FIX
   // Cortex M3 modification, ip dereferences to an odd value
   // not useful for constant and variable addresses
   temp = ~1 & ((uint32_t)(user->userVariables.namedVariables.ip[-1]));
   temp += headerSize;

   vPtr = (uint32_t *)temp;
   PUSH(user,*vPtr);
   #else
   push(user,*((Cell *)((Byte *)(*((WordPtr *)((user->userVariables.namedVariables.ip) - 1))) + headerSize)));
   #endif
}

void doDOES(UserStatePtr user)
{
   WordPtr *ptr;
   Cell  *cell;
   *(user->userVariables.namedVariables.rp)++ = (WordPtr *)(user->userVariables.namedVariables.ip);
   // get the address of the does tokens
   ptr = ((WordPtr *)((Byte *)(*((WordPtr *)((user->userVariables.namedVariables.ip) - 1))) + headerSize));
   // get the address of the data portion
   cell = ((Cell *)((Byte *)(*((WordPtr *)((user->userVariables.namedVariables.ip) - 1))) + headerSize + sizeof(WordPtr)));
   // push the address of the data
   PUSH(user,cell);
   // make the ip point to the tokens
   user->userVariables.namedVariables.ip = (WordPtr *)*ptr;
}

// METHOD ONE stores the value in the codespace
// method two stores the value in name in the code pointer, saving 4 bytes
// per define.
#define METHOD_ONE
#undef METHOD_ONE

// common method for a define
// take the name pointer
// and the code pointer is the
// address of the constant
void doDEFINE(UserStatePtr user)
{
   NameType   *foundName;
   //    uint32_t     value;
   foundName = (NameType *)pop(user);
   #ifdef METHOD_ONE
   PUSH(user,*(foundName->code));
   #else
   PUSH(user,(foundName->code));
   #endif
} //doDefine




/* word that ends a colon definition */
/* I.E. provides the direct threading */
void exiT(UserStatePtr user)
{

   /* does the reverse of : 
   *rp++ = ip;
   ip--;
   ip = (WordPtr*)((byte*)*ip + headerSize); 
   */
   /* first recover the ip */
   (user->userVariables.namedVariables.ip) = (WordPtr *)rpPop(user);
   /* and return */
   /* main loop will advance the IP */

}

// ******************************************************************************
// ******************************************************************************
// Primitive words section. Implements core stuff.
// ******************************************************************************
// ******************************************************************************


void bye(UserStatePtr user)
{
   FPRINTF("thanks for using me ...\r\n");
   //    exit(1);
}




// ******************************************************************************
// ******************************************************************************
// Substring, looks for a string piece within another string.
// used by grep to find commands.
// ******************************************************************************
// ******************************************************************************

int substring(char *string1, char *sub)
{
   return((int)strstr(string1,sub));
}

void strToUpper(char *in,char *out)
{
   while(*out++ = toupper(*in),*in++);
}

int substringCaseInsensitive(char *string1, char *sub)
{
   static char s1[80],s2[80];


   strToUpper(string1,s1);
   strToUpper(sub,s2);
   return((int)strstr(s1,s2));
}

// ******************************************************************************
// ******************************************************************************
// Grep. Allows user to put in a portion of a word and search the dictionary
//       for it. In case the full word name is forgotten somehow.
// ******************************************************************************
// ******************************************************************************

void grep(UserStatePtr user)
{
   int end;
   WordList *current;
   int wordListCount;
   char *match;

   tokeN(user);
   match = (char *)POP(user);
   match++;
   wordListCount = USER_VAR(user,WORDLISTS);
   wordListCount++; // include the constants list
   while(wordListCount > 0)
   {
      current = (WordList *)USER_VAR(user,DEFINES + wordListCount - 1);
      if(current != NULL)
      {
         end = current->last; // user->userVariables.namedVariables.last;
         FPRINTF("Wordlist : %s\r\n",current->name);
         while(end > 0)
         {
            if(substring(current->names[end].text + 1,match))
            {
               FPRINTF("%s ",current->names[end].text + 1);
            }
            end--;
         }
         FPRINTF("\r\n");
      } // if current is not null
      wordListCount--;
   } // while wordlists

} // grep

void grepCaseIndep(UserStatePtr user)
{
   int end;
   WordList *current;
   int wordListCount;
   char *match;
   char *delim;
   
   delim = (char *)POP(user);
   delim++;

   tokeN(user);
   match = (char *)POP(user);
   match++;
   //FPRINTF("grep match %s\r\n",match);
   wordListCount = USER_VAR(user,WORDLISTS);
   wordListCount++; // include the constants list
   while(wordListCount > 0)
   {
      current = (WordList *)USER_VAR(user,DEFINES + wordListCount - 1);
      if(current != NULL)
      {
         end = current->last; // user->userVariables.namedVariables.last;
         FPRINTF("Wordlist : %s\r\n",current->name);
         while(end > 0)
         {
            if(substringCaseInsensitive(current->names[end].text + 1,match))
            {
               FPRINTF("%s%s",current->names[end].text + 1,delim);
            }
            end--;
         }
         FPRINTF("\r\n");
      } // if current is not null
      wordListCount--;
   } // while wordlists

}

void igrep(UserStatePtr user)
{
   const char blankString[] = "\x1 ";
   UPUSH((Cell)blankString);
   grepCaseIndep(user);
} // grep

void grepcol(UserStatePtr user)
{
   const char newlineString[] = "\x2\r\n";
   UPUSH((Cell)newlineString);
   grepCaseIndep(user);
} // grep

// ******************************************************************************
// ******************************************************************************
// Words.  Prints out the names of all the words in all wordlists.
// ******************************************************************************
// ******************************************************************************
void words(UserStatePtr user)
{
   int end;
   WordList *current;
   int wordListCount;
   //    int width;

   wordListCount = USER_VAR(user,WORDLISTS);
   wordListCount++; // include the constants list
   while(wordListCount > 0)
   {
      current = (WordList *)USER_VAR(user,DEFINES + wordListCount - 1);
      if(current != NULL)
      {
         if(current == &defines1WordList)
         {
            if(USER_VAR(user,QUIET))
            {
               FPRINTF("Wordlist : %s\r\n",current->name);
               FPRINTF("Suppressed unless you type loud first ");
               FPRINTF("\r\n");
               wordListCount--;
               continue;
            }
         }
         if(current == &rootWordList)
         {
            if(USER_VAR(user,QUIET))
            {
               FPRINTF("Wordlist : %s\r\n",current->name);
               FPRINTF("quiet loud ");
               FPRINTF("\r\n");
               wordListCount--;
               continue;
            }
         }
         if(current == &coreWordList)
         {
            if(USER_VAR(user,QUIET))
            {
               FPRINTF("Wordlist : %s\r\n",current->name);
               FPRINTF("<Quiet>\r\n");
               wordListCount--;
               continue;
            }
         }
         end = current->last; // user->userVariables.namedVariables.last;
         FPRINTF("Wordlist : %s\r\n",current->name);
         //            width = 0;
         while(end > 0)
         {
            FPRINTF("%s ",current->names[end].text + 1);
            end--;
            #if 0
            width += current->names[end].text[0]; // add the length
            if(width > 60)
            {
               width = 0;
               FPRINTF("\r\n");
            }
            #endif
         }
         FPRINTF("\r\n");
      } // if current is not null
      wordListCount--;
   } // while wordlists
} // words



/*****************************************************************************************/
/*****************************************************************************************/
/* Add vocabularies to this threaded version of the forth interpreter */
/* Try to adhere to ANSI stuff as closely as possible*/
/*****************************************************************************************/
/*****************************************************************************************/

// We need the following words
//
// DEFINITIONS -- Makes the first wordlist in the search order the compilation list
// FORTH-WORDLIST -- Returns the standard wordlist which is the startup compilation list and is part of initial
//                   search order
// GET-CURRENT -- Returns the current compilation wordlist
// GET-ORDER -- Returns a count and the number of wordlists in the search order
// SEARCH-WORDLIST -- Searches for a word in a given wordlist only
// SET-CURRENT -- Sets the compilation word list
// SET-ORDER -- takes a list of wordlists and a count and sets the search order, if count  = 0 no order
//              n = -1 set the default min order, default min order must include FORTH-WORDLIST and SET-ORDER
// WORDLIST  -- create a new wordlist, return the pointer
// ALSO -- Duplicate the search order first element so it searches twice
// FORTH -- replace the last wordlist with forth
// ONLY -- set search order to minimum , same as -1 SET-ORDER
// ORDER -- Shows the search order and the compilation wordlists
// PREVIOUS -- drop the top wordlist from the search order
// In addition FIND Is modified


// Need the following capability
// Take a block of ram, and make a vocabulary out of it
// Thus we need a malloc kind of thing
// That mallocs a block and


// Basic problem of allocating a wordlist.
// You get a block of ram and put a header on
// the header contains the pointer to the array of names
// which must be dynamically defined
// and a few other variables that go with the wordlist
// mostly LAST
// and the index of the last name in the wordlist array
// then the pointer to the end of the wordlist
// becomes the free space for code/data
//








// a primitive that builds up a wordlist from a block of ram
// given the parameters
// returns 1 for ok 0 for error
// buffer must meet the worst case alignment
int32_t createWordListPrimitive(UserStatePtr user,Byte *buffer,int32_t bufferSize,int32_t numberOfNames)
{
   WordList *wordList;
   if(((Cell)buffer) & CELL_ALIGN_MASK)
   {
      // buffer is not CELL aligned
      FPRINTF("Bad buffer alignment \r\n");
      return(0);
   }
   if((sizeof(WordList) + numberOfNames * sizeof(NameType)) > bufferSize)
   {
      FPRINTF("Buffer size wrong \r\n");
      return(0);
   } //if
   wordList = (WordList *)buffer;
   wordList->names = &(wordList->namesHolder[0]);
   wordList->names->code = NULL; // terminator token
   wordList->names->text = NULL; // terminator token
   wordList->last = 0;
   wordList->sizeOfBlock = bufferSize;
   wordList->numberOfNames = numberOfNames;
   wordList->codeSpaceStart = (WordPtr *)(wordList->namesHolder + numberOfNames);
   wordList->codeSpaceEnd =   (WordPtr *)(buffer + bufferSize - sizeof(Cell));
   wordList->cp = wordList->codeSpaceStart;
   #if 0
   FPRINTF("Wordlist %x names %x last %x size %d num names %d code start %x code end %x CP %x\r\n",
           wordList,
           wordList->names ,
           wordList->last ,
           wordList->sizeOfBlock ,
           wordList->numberOfNames ,
           wordList->codeSpaceStart ,
           wordList->codeSpaceEnd,
           wordList->cp
           );
   #endif
   return(1);
} // createWordList

void createWordList(UserStatePtr user)
{
   MallocType mallocFunction;
   Byte *buffer;
   int32_t blockSize;
   int32_t numberOfNames;
   int32_t flag;
   char *name;
   WordList *newWordList;
   uint32_t   oldCurrentWordlist;

   mallocFunction = (MallocType)(USER_VAR(user,MALLOC));
   numberOfNames = (int32_t)(pop(user));
   blockSize = (int32_t)(pop(user));
   buffer = (*mallocFunction)(blockSize);
   if(buffer == NULL)
   {
      FPRINTF("Can't malloc wordlist, halting\r\n");
      while(1);
      //        PUSH(user,0);
      //        return;
   }
   flag = createWordListPrimitive(user,buffer,blockSize,numberOfNames);
   if(!flag)
   {
      PUSH(user,0);
      return;
   }
   // an optimization
   // make the wordlists always
   // in the root space so you can always see them
   // This is ok as long as no one removes forth from the search set
   // save the current wordlist
   oldCurrentWordlist = USER_VAR(user,CURRENT);
   // temporarily use root as current
   USER_VAR(user,CURRENT) = (uint32_t)&rootWordList;
   tokeN(user);
   dupp(user); // save the name
   dollarCommaN(user);
   constantComma(user);
   /* now store the constant on top of stack in the word */
   PUSH(user,buffer);
   comma(user);
   name = (char *)pop(user); // get the token name off the stack
   newWordList = (WordList *)buffer;
   strcpy(newWordList->name,name + 1);
   /* note NO rightbracket because the compile is done */
   //    PUSH(user,buffer);
   dollarCommaNFinish(user);

   // now restore the current wordlist
   USER_VAR(user,CURRENT) = oldCurrentWordlist;

}

void freeWordList(UserStatePtr user)
{
   FreeType freeFunction;
   Byte     *buffer;
   buffer = (Byte *)pop(user);
   freeFunction = (FreeType)(USER_VAR(user,MALLOC));
   (*freeFunction)(buffer);
}




void definitions(UserStatePtr user)
{
   USER_VAR(user,CURRENT) = USER_VAR(user,WORDLIST0 + USER_VAR(user,WORDLISTS) - 1);
}

void forthWordList(UserStatePtr user)
{
   PUSH(user,&rootWordList);
}

void getCurrent(UserStatePtr user)
{
   PUSH(user,USER_VAR(user,CURRENT));
}

void getOrder(UserStatePtr user)
{
   int i;
   int j;
   i = USER_VAR(user,WORDLISTS);
   j = i;
   while(i)
   {
      PUSH(user,USER_VAR(user,j - i + WORDLIST0));
      i--;
   }
   PUSH(user,j);
}
void searchWordList(UserStatePtr user)
{
}
void setCurrent(UserStatePtr user)
{
   WordList *wordList;
   wordList = (WordList *)pop(user);
   USER_VAR(user,CURRENT) = (uint32_t)wordList;
}
void setOrder(UserStatePtr user)
{
}



void also(UserStatePtr user)
{
   if(USER_VAR(user,WORDLISTS) < MAX_WORDLISTS)
   {
      USER_VAR(user,WORDLIST0 + USER_VAR(user,WORDLISTS)) = USER_VAR(user,WORDLIST0 + USER_VAR(user,WORDLISTS) - 1);
      (USER_VAR(user,WORDLISTS))++;
   }
}
void forth(UserStatePtr user)
{
   if(USER_VAR(user,WORDLISTS) < MAX_WORDLISTS)
   {
      USER_VAR(user,WORDLIST0 + USER_VAR(user,WORDLISTS) - 1) = (uint32_t)&rootWordList;
   }
}
// this now works with the core wordlist in flash
// and root words.
void only(UserStatePtr user)
{
   USER_VAR(user,WORDLISTS) = 2;
   USER_VAR(user,WORDLIST0) = (uint32_t)&coreWordList;
   USER_VAR(user,WORDLIST1) = (uint32_t)&rootWordList;
}
void order(UserStatePtr user)
{
   int i;
   int j;
   WordList **wordList;
   i = USER_VAR(user,WORDLISTS);
   j = i;
   FPRINTF("Number of wordlists %d\r\n",i);
   FPRINTF("Search wordlists \r\n");
   wordList = (WordList **)&(USER_VAR(user,j - 1 + WORDLIST0));
   while(i)
   {
      FPRINTF("Wordlist address %x name %s\r\n",*wordList,(*wordList)->name);
      wordList--;
      i--;
   }
   wordList = (WordList **)&(USER_VAR(user,CURRENT));
   FPRINTF("Current compilation wordlist %s\r\n",(*wordList)->name);
}

void previous(UserStatePtr user)
{
   if(USER_VAR(user,WORDLISTS) > 0)
   {
      (USER_VAR(user,WORDLISTS))--;
   }
}

void restrictToCurrent(UserStatePtr user)
{
   USER_VAR(user,WORDLISTS) = 1;
   USER_VAR(user,WORDLIST0) = USER_VAR(user,CURRENT);
}

void restrictToCurrentNumber(UserStatePtr user)
{
   uint32_t lists;
   // uint32_t theList;
   uint32_t i;
   lists = UPOP();
   USER_VAR(user,WORDLISTS) = lists;
   for(i=0;i<lists;i++)
   {
      USER_VAR(user,WORDLIST0+i) = UPOP();
   }
   USER_VAR(user,CURRENT) = USER_VAR(user,WORDLIST0+lists-1);

}



void addToOrder(UserStatePtr user)
{
   uint32_t index;
   if(USER_VAR(user,WORDLISTS) < MAX_WORDLISTS)
   {
      index = USER_VAR(user,WORDLISTS);
      //	FPRINTF("index = %d wordlist0 = %d\n",index,WORDLIST0);
      //	FPRINTF("wordlist 0 wordlist 1 %x %x \r\n",USER_VAR(user,WORDLIST0),USER_VAR(user,WORDLIST1));
      //	FPRINTF("NUmber %d\n",USER_VAR(user,WORDLISTS));
      USER_VAR(user,WORDLIST0 + index) = pop(user);
      (USER_VAR(user,WORDLISTS))++;
      //	FPRINTF("wordlist 0 wordlist 1 %x %x \r\n",USER_VAR(user,WORDLIST0),USER_VAR(user,WORDLIST1));
      //	FPRINTF("NUmber %d\n",USER_VAR(user,WORDLISTS));
   }
}




// ******************************************************************************
// ******************************************************************************
// WordsInWordList  Prints all the words in a single wordlist. Used by SOME-WORDS.
// ******************************************************************************
// ******************************************************************************
void wordsInWordList(UserStatePtr user)
{
   int end;
   WordList *current;

   current = (WordList *)pop(user);
   end = current->last; // user->userVariables.namedVariables.last;
   FPRINTF("Wordlist : %s\r\n",current->name);
   while(end > 0)
   {
      FPRINTF("%s ",current->names[end].text + 1);
      end--;
   }
} // wordsInWordLists




void beQuiet(UserStatePtr user)
{
   USER_VAR(user,QUIET) = 1;
}
void beLoud(UserStatePtr user)
{
   USER_VAR(user,QUIET) = 0;
}




/* some useful words */


void init(UserStatePtr user)
{
   user->userVariables.namedVariables.stackStart = user->userVariables.namedVariables.stackPtr = user->stack;
   user->userVariables.namedVariables.rp = (WordPtr **)(user->stack + STACK_SIZE + RETURN_STACK_SIZE - 1);
   user->userVariables.namedVariables.rpStart = user->userVariables.namedVariables.rp;
}

// this allow an external call to clean up any stack dangles after init.
void stackInit(UserStatePtr user)
{
   user->userVariables.namedVariables.stackStart = user->userVariables.namedVariables.stackPtr = user->stack;
}



#ifdef SECURE
void dumP(UserStatePtr user)
{
   Byte *ptr;
   int   count;
   int i,j,k;

   count = (int)pop(user);
   ptr = (Byte *)pop(user);
   j = count / 16;
   if((j * 16) !=  count)
   {
      j++;
   }
   for(i = 0;i < j;i++)
   {
      FPRINTF("\r\n %p:",ptr);
      if(((uint32_t)ptr > ROM_END) && ((uint32_t)ptr < PERIPHERAL_START))
      {
         for(k = 0;k < 16;k++)
         {
            FPRINTF(" %02X",ptr[k]);
         }
         FPRINTF("  ");

         for(k = 0;k < 16;k++)
         {
            FPRINTF("%c",(isprint(*ptr) ? *ptr : '.'));
            ptr++;
         } //
      } //
      else
      {
         for(k = 0;k < 16;k++)
         {
            FPRINTF(" %02X",0xff);
         }
         FPRINTF("  ");

         for(k = 0;k < 16;k++)
         {
            FPRINTF(".");
            ptr++;
         } //
      } //else
   } //for
   FPRINTF("\r\n");
} //
#else // if secure
void dumP(UserStatePtr user)
{
   Byte *ptr;
   int   count;
   int i,j,k;

   count = (int)pop(user);
   ptr = (Byte *)pop(user);
   j = count / 16;
   if((j * 16) !=  count)
   {
      j++;
   }
   for(i = 0;i < j;i++)
   {
      FPRINTF("\r\n %p:",ptr);
      for(k = 0;k < 16;k++)
      {
         FPRINTF(" %02X",ptr[k]);
      }
      FPRINTF("  ");

      for(k = 0;k < 16;k++)
      {
         FPRINTF("%c",(isprint(*ptr) ? *ptr : '.'));
         ptr++;
      } //
   } //for
   FPRINTF("\r\n");
} //
#endif // else not secure

void defineQ(UserStatePtr user)
{
   WordList *searchWordList;
   searchWordList = (WordList *)(USER_VAR(user,(DEFINES)));
   //	FPRINTF("search wordlist %x names %x %x\n",searchWordList,searchWordList->names,searchWordList->names+(searchWordList->last));
   if(searchWordList == NULL)
   {
      PUSH(user,0);   // fail
      return;
   }
   PUSH(user,searchWordList->names + (searchWordList->last)); // and wordlist
   find(user);
}

// looks for a word in both the names and the constants
// so does nameq and defineq
void defineQW(UserStatePtr user)
{
   WordList *searchWordList;
   searchWordList = (WordList *)(USER_VAR(user,(DEFINES)));
   //	FPRINTF("search wordlist %x names %x %x\r\n",searchWordList,searchWordList->names,searchWordList->names+(searchWordList->last));
   if(searchWordList == NULL)
   {
      PUSH(user,0);   // fail
      return;
   }
   PUSH(user,searchWordList->names + (searchWordList->last)); // and wordlist
   find(user);
   dupp(user);
   if(iff(user))
   {
      swap(user); // swap out the code pointer
      drop(user); // and drop it, leaving the name pointer
      PUSH(user,searchWordList);
      return;
   }
}

// looks for a word in both the names and the constants
// so does nameq and defineq
void bothQ(UserStatePtr user)
{
   Cell  temp;
   dupp(user); // double up the word being looked for
   temp = (Cell)pop(user); // save the pointer
   defineQ(user); // and pass back what defineQ might find
   dupp(user); // check the answer
   if(iff(user))
   {
      // find returned a true
      return;
   }
   // find failed
   // drop its results
   drop(user);
   drop(user);
   // push the pointer back on
   PUSH(user,temp);
   nameQ(user); // look for it in names
//    defineQ(user); // and pass back what defineQ might find
}

// does what nameQW does but also searches the DEFINE wordlist
void bothQW(UserStatePtr user)
{
   Cell  temp;
   dupp(user); // double up the word being looked for
   temp = (Cell)pop(user); // save the pointer
   defineQW(user); // and pass back what defineQ might find
   dupp(user); // check the answer
   if(iff(user))
   {
      // find returned a true
      return;
   }
   // find failed
   // drop its results
   drop(user);
   drop(user);
   // push the pointer back on
   PUSH(user,temp);
   nameQW(user); // look for it in names
}

// this leaves the
void interpretQuote(UserStatePtr user)
{
   PUSH(user,'"');
   PARSE(user);
   tempStringBuffer(user);
   //     packDollar(user);
   stringDollar(user);
}


void compileQuote(UserStatePtr user)
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

void quote(UserStatePtr user)
{
   if(USER_VAR(user,STATE) == INTERPRETING)
   {
      interpretQuote(user);
   }
   else
   {
      compileQuote(user);
   }
}

// expects a count and a string on the top of the stack
// the count is that of the actual number of characters
void compileString(UserStatePtr user)
{
   Int16 length;
   Byte *ptr;
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);

   // first compile the runtime
   UPUSH(dollarQuoteBar);
   comma(user);
   //    compile,dollarQuoteBar,

   // copy the string from the temporary buffer
   // converting as it goes.
   here(user);
   stringDollar(user);
   //    dupp(user);
   ptr = (uint8_t *)UPOP();
   length = LENGTH_MASK & (*ptr);
   // now advance the code pointer
   #ifdef LONG_STRING_COUNT
   current->cp = cellAlignedMove(current->cp,length + 1);
   #else
   /* the plus two accounts for a null termination on a C string */
   current->cp = cellAlignedMove(current->cp,length + 2);
   #endif
}

// Tries to make a string out of where we are pointing in the input buffer
// if compiling, copy it to the current word definition with the runtime code for
// string.
// If interpreting pack it into the tmp string.
// If it can't be made into a string, push it back on with a 0
// just like defineQW
// This does not allow for null strings. i.e. length 1 that is just the terminating 0
void stringQ(UserStatePtr user)
{
   char *string;
   char *orgString;
   SignedCell length;
   SignedCell tailLength;
   SignedCell index;

   char *ptr;
   char *tail;
   char *temp;

   orgString = string = (char *)pop(user);


   // at this point we have a token to point to.
   // Tokens are null terminated, the count includes the null terminator
   length = *string++;

   // first if it doesn't open with a quote, fugettaboutit
   if((string[0] != '"'))
   {
      PUSH(user,orgString);
      PUSH(user,0);
      return;
   }
   // length sanity check
   // must have length 3  which is " then a char then a "
   if(length < 4)
   {
      PUSH(user,orgString);
      PUSH(user,0);
      return;
   }

   // get acces to temp buffer
   temp = (char *)(USER_VAR(user,STRING_TEMP));
   // now see if there were no embedded spaces thus the token is a string itself.
   if((string[0] == '"') && ((string[length - 2] == '"')))
   {
      // move to the temporary buffer so the usage is constant
      temp[0] = length - 2; // remove the quotes, leave room for null
      memcpy(temp + 1,string + 1,length - 3); // move the actual characters
      temp[1 + length - 3] = 0; // null terminate
      UPUSH(temp + 1);
      UPUSH(length - 3); // return the actual character count, not including null
      return;
   }
   // okay there is no closing quote
   // length is now two bytes less, remove the quote and null from length
   length -= 2;
   // look through the rest of the input line for a "
   index = (SignedCell)(USER_VAR(user,TO_IN));
   ptr = user->tibBuffer + index;
   tail = strchr(ptr,'"');
   if(tail)
   {
      tailLength = (SignedCell)(tail - ptr);  // get length without the quote
                                              // copy the first part of the string to temp buffer, leave room for
                                              // count
      memcpy(temp + 1,string + 1,length);
      // copy the remainder of the string from the tib
      // note that there had to be a space before the remainder of the string
      // else the parser would not have identified the first half token of
      // "lkadfj
      // so back up one character and pick up the space, adjust the count as well
      ptr--;
      tailLength++;
      memcpy(temp + length + 1,ptr,tailLength);
      temp[1 + length + tailLength] = 0; // null terminate
      temp[0] = length + tailLength + 1; // put count in
      UPUSH(temp + 1); // push the pointer to the buffer
      UPUSH(length + tailLength);
      // now update the index in the tib
      index += tailLength + 1; // move beyond the trailing quote
      USER_VAR(user,TO_IN) = index;
   }
   else
   {
      PUSH(user,orgString);
      PUSH(user,0);
   }
} //


/**
 * Counts the number of bytes between 
 * ' and ' in an ascii hex string
 * 
 * @author nowelc2 (5/17/2017)
 * 
 * @param user 
 * 
 * @return SignedCell 
 */
SignedCell  countBytes(char *string)
{
   int32_t  length;
   length = 0;
   if(*string != '\'')
   {
      return(0);
   }
   string++;
   while((*string != '\'') && (*string != 0))
   {
      length++;
      string++;
   }
   // correct for odd length
   if(length & 1)
   {
      length++;
   }
   return(length / 2);
}


// parse a whole line starting with the word start:
void record(UserStatePtr user)
{
   PUSH(user,'\r');
   PARSE(user);
   tempStringBuffer(user);
   packDollar(user);
   //    stringDollar(user);
}

// void array(UserStatePtr user)


void pushGets(UserStatePtr user)
{
   uint32_t newGets;
   newGets = (uint32_t)UPOP();
   USER_VAR(user,OLD_GETS2) = USER_VAR(user,OLD_GETS1);
   USER_VAR(user,OLD_GETS1) = USER_VAR(user,OLD_GETS0);
   USER_VAR(user,OLD_GETS0) = USER_VAR(user,GETS_VECTOR);
   USER_VAR(user,GETS_VECTOR) = (uint32_t)newGets;
   USER_VAR(user,ECHO) = 0;
}

void popGets(UserStatePtr user)
{
   USER_VAR(user,GETS_VECTOR) = USER_VAR(user,OLD_GETS0);
   USER_VAR(user,OLD_GETS0) = USER_VAR(user,OLD_GETS1);
   USER_VAR(user,OLD_GETS1) = USER_VAR(user,OLD_GETS2);

}


void memgets(UserStatePtr user)
{
   char *startPointer;
   char *buffer = (char *)(user->tibBuffer);
   startPointer = (char *)(USER_VAR(user,START_STRING));
   while(1)
   {
      if(*startPointer == 0x04)
      {
         *buffer = 0;
         USER_VAR(user,START_STRING) = 0;
         popGets(user);
         return;
      }
      // PUTCH(*buffer);
      *buffer = *startPointer;
      if(*startPointer == '\n')
      {
         //  PUTCH('\n');
         startPointer++;
         if(*startPointer == 0x04)
         {
            USER_VAR(user,START_STRING) = 0;
            popGets(user);
         }
         *buffer = 0;
         USER_VAR(user,START_STRING) = (uint32_t)startPointer;
         return;
      } //if
      buffer++;
      startPointer++;
   } //while
} //memgets


// calls the nominal vector
void plainGets(UserStatePtr user)
{
   USER_VAR(user,ECHO) = USER_VAR(user,ECHOZERO);
   //    doEcho = 1;
   GETS((char *)(user->tibBuffer));
}

void trimToNewline(Byte *buffer, int32_t length)
{
   while(length-- && (*buffer != '\r') && (*buffer != '\n'))
   {
      buffer++;
   }
   *buffer = 0;
}


// Note this is called automatically
// by inserting a "CLOSE" in the last position in the EEPROM data
void closeFile(UserStatePtr user)
{
   USER_VAR(user,ECHO) = USER_VAR(user,ECHOZERO);
   //    doEcho = 1;
   // restore input to proper place
   popGets(user);
}



/*
DONE : fix this , this won't work for real because it doesn't allow a nested revector.
    Because if memgets is running then the old_gets is plainGets and then
    if it does an open then old_gets becomes filegets and plaingets never
    gets restored. Investigate pushing the old_gets on the RP stack but
    not sure how this will work.
*/

void getS(UserStatePtr user)
{
   GETSVECTOR(user);
}

void query(UserStatePtr user)
{

   getS(user);
   //    FPRINTF("back from gots strlen %d\n",strlen(user->tibBuffer));
   //    return;
   PUSH(user,strlen((user->tibBuffer)));
   ptib(user);
   bang(user);
   PUSH(user,0);
   toIN(user);
   bang(user);
} /* query */


void dummyQuery(UserStatePtr user)
{
   // getS(user);
   //    FPRINTF("back from gots strlen %d\n",strlen(user->tibBuffer));
   //    return;
   PUSH(user,strlen((user->tibBuffer)));
   ptib(user);
   bang(user);
   PUSH(user,0);
   toIN(user);
   bang(user);
} /* query */


typedef void (*SignalReadyFunction)(UserStatePtr user);
// this is called after query had run and a command line has been finished
// If a vector has been installed, this calls it to let block devices
// know that the line has finished.
void dotPrompt(UserStatePtr user)
{
   SignalReadyFunction signalReady;

   if((USER_VAR(user,ECHO)))
   {
      if(USER_VAR(user,STATE) == INTERPRETING)
      {
         FPRINTF(" \r\nok>");
      }
      else
      {
         FPRINTF("\r\n");
      }
   } //
   if(USER_VAR(user,READY_FOR_INPUT))
   {
      signalReady = (SignalReadyFunction)USER_VAR(user,READY_FOR_INPUT);
      (*signalReady)(user);
   }
}



/* some other basic primitives, organized around */
/* compiling words */
/**********************************************************************/
/*   fundamental words data structure */
/**********************************************************************/





/* a compiled versio of if */



void createDefine(UserStatePtr user)
{
   Cell temp;
   #ifndef METHOD_ONE
   Cell value;
   WordList *current;
   #endif

   if(USER_VAR(user,DEFINES) == 0)
   {
      FPRINTF("No define wordlist!! ");
      longjmp(user->jumper,13);
   }
   temp  = USER_VAR(user,CURRENT);
   USER_VAR(user,CURRENT) = USER_VAR(user,DEFINES);
   tokeN(user);
   dollarCommaN(user);
   #ifdef METHOD_ONE
   // now poke the constant down that was specified
   comma(user);
   #else
   // put the actual value in the code pointer
   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   value = UPOP();
   // modified once dollarcommaN was modified to avoid
   // finding yourself in the dictionary
   current->names[current->last + 1].code = (WordPtr)value;
   #endif
   /* note NO rightbracket because the compile is done */
   dollarCommaNFinish(user);
   USER_VAR(user,CURRENT) = temp;
}


void doesGreater(UserStatePtr user)
{
   Byte *ptr;
   WordList *current;

   // expects the address on the top of the stack to patch
   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   // needs to find the latest word
   // and take the address of here and put the address in
   ptr = (Byte *)(current->names[current->last].code);
   ptr += headerSize;
   PUSH(user,user->userVariables.namedVariables.ip);
   PUSH(user,ptr);
   bang(user);

   // now exit the current word
   // instead of executing the does stuff
   exiT(user);

}


/* DOES NOT WORK BECAUSE IT IS COMPILED IN AN IF */
/* THUS HIDING THE TRUE TARGET OF THE BRANCH OUT */
/* SOLUTION IS A USER VARIABLE TO KEEP UP WITH THE */
/* BOTTOM OF THE LOOP ADDRESS */

/* runtime portion */
void doBREAK(UserStatePtr user)
{
   /* breaks a do loop */
   /* NOT A FOR */
   WordPtr *target;
   target = (WordPtr *)*(user->userVariables.namedVariables.ip);
   target = (WordPtr *)*target;
   (user->userVariables.namedVariables.ip) = (WordPtr *)*target;
}

/* compile time portion */
void doBreak(UserStatePtr user)
{
   SignedCell vector;
   /* compile code to drop parms */
   PUSH(user,fromR);
   comma(user);
   PUSH(user,fromR);
   comma(user);
   PUSH(user,drop);
   comma(user);
   PUSH(user,drop);
   comma(user);
   PUSH(user,doBREAK);
   comma(user);      /* lay down the branch */
   vector = (Cell)top(user); /* tos is exit address */
   PUSH(user,vector);
   comma(user);         /* puts the address of the exit address in the word */
   /* to where ?*/

}



NameType* cfaSearch(UserStatePtr user,WordPtr searchFor,WordList **whichWordList,int32_t *index);
Int16 rootLast;
void forget(UserStatePtr user)
{
   //    WordPtr w;
   SignedCell   index;
   NameType *name;
   WordList *whichWordList;
   tokeN(user);
   bothQW(user); // searches in both words and constants
   whichWordList = (WordList *)pop(user);
   if(whichWordList != NULL)
   {
      name = (NameType *)pop(user);
      //     name = cfaSearch(user,w,&whichWordList,&index);
      if(name != NULL)
      {
         index = ((Byte *)name - (Byte *)whichWordList->names) / sizeof(NameType);
         if(whichWordList == &rootWordList)
         {
            if(index <= rootLast)
            {
               FPRINTF("Can't forget.. word is in core dictionary\r\n");
               return;
            } // if
         } // if
         if(whichWordList == &coreWordList)
         {
            FPRINTF("Can't forget.. word is in core dictionary\r\n");
            return;
         } //if
           //	  FPRINTF("index %d text %x whichWordList %x \n",index,whichWordList->names[index].text,whichWordList);
           //	  FPRINTF("current CP %x\n",whichWordList->cp);
           // truncate the name from the wordlist
         whichWordList->cp = (WordPtr *)(whichWordList->names[index].text);
         whichWordList->last = index - 1;
         whichWordList->names[index].text = NULL;
         whichWordList->names[index].code = NULL;
      }
   }
   else
   {
      // FPRINTF("Can't find it \r\n");
      pop(user);
   }

} /* FORGET */


#ifdef USE_HELP
// expects the token and the help string on the text
// the token is a counted string
// help string is plain text
void addHelpFromC(UserStatePtr user)
{
   SignedCell   index;
   NameType *name;
   WordList *whichWordList;
   char *helpString;
   helpString = (char *)UPOP();
   //    tokeN(user);
   // expects a counted string
   bothQW(user); // searches in both words and constants
   whichWordList = (WordList *)pop(user);
   if(whichWordList != NULL)
   {
      name = (NameType *)pop(user);
      //     name = cfaSearch(user,w,&whichWordList,&index);
      if(name != NULL)
      {
         index = ((Byte *)name - (Byte *)whichWordList->names) / sizeof(NameType);
         if(whichWordList == &coreWordList)
         {
            FPRINTF("Can't add to core\r\n");
            return;
         } //if
         whichWordList->names[index].help = helpString;
      }
   }
   else
   {
      if((USER_VAR(user,ECHO)))
      {
         //FPRINTF("Can't find it \r\n");
      }
      pop(user);
   }

} /* addHelp */
void dotHelp(UserStatePtr user)
{
   NameType *name;
   WordList *whichWordList;
   tokeN(user);
   bothQW(user); // searches in both words and constants
   whichWordList = (WordList *)pop(user);
   if(whichWordList != NULL)
   {
      name = (NameType *)pop(user);
      if(name != NULL)
      {
         if(name->help != NULL)
         {
            FPRINTF("\r\n%s\r\n",name->help);
         }
      }
   }
   else
   {
      FPRINTF("Can't find it \r\n");
      pop(user);
   }
} /* FORGET */
#endif

void forthVerboseMode(FCONTEXT)
{
   uint32_t value;
   value = UPOP();
   USER_VAR(user1,ECHO) = value;
   USER_VAR(user1,ECHOZERO) = value;

}


/*
void code(UserStatePtr user)
{
    tokeN(user);
    dollarCommaN(user);
    spat(user);
    csp(user);
    bang(user);
}

void endCode(UserStatePtr user)
{
    spat(user);
    csp(user);
    at(user);
    equals(user);
    if(!iff(user))
    {
        FPRINTF("WARNING : mismatched control structure in definition\r\n");
    }
}
*/

/* >NAME */
void toName(UserStatePtr user)
{
   int end;
   WordPtr searchFor;
   WordList *current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);

   end = (current->last);

   searchFor = (WordPtr)pop(user);

   while(end > 0)
   {
      /* this statment depends on left to right evaluation */
      /* to speed the search */
      if(current->names[end].code == searchFor)
      {
         PUSH(user,current->names + end);
         return;
      } /* if */
      end--;
   } /* while */
   PUSH(user,NULL);
} /* TOnAME */


void huh(UserStatePtr user)
{
   FPRINTF(" ERROR : Don't understand this??\r\n");
}

void tempStringBuffer(UserStatePtr user)
{
   PUSH(user,USER_VAR(user,STRING_TEMP));
}

void doDelay(UserStatePtr user)
{
   int32_t delay;
   delay = (int32_t)POP(user);
   if(delay)
   {
      #ifdef USE_OSDELAY
      vTaskDelay(delay);
      #endif
   } //if
} //doDelay

void copy(UserStatePtr user)
{
   Byte *source;
   Byte *destination;
   uint32_t count;

   count = (uint32_t)UPOP();
   source = (Byte *)UPOP();
   destination = (Byte *)UPOP();

   memcpy(destination,source,count);
}
void fill(UserStatePtr user)
{
   Byte *destination;
   uint32_t count;
   int32_t  value;

   count = (uint32_t)UPOP();
   value = (uint32_t)UPOP();
   destination = (Byte *)UPOP();

   memset(destination,value,count);
}


void subscribe(FCONTEXT)
{
   void(* subscribeFunction)(FCONTEXT);

   if(user->userVariables.namedVariables.subscribePtr)
   {
      subscribeFunction = (void(*)(FCONTEXT))(user->userVariables.namedVariables.subscribePtr);
      (*(subscribeFunction))(user);
   }
}



/**********************************************************************/
/*   word initialization */
/**********************************************************************/



void addExternalWords(UserStatePtr user);


/* basic words */
/* in the name space */
/* all assembly words */
const NameType coreNames[] =
{
   #include "coredefs.h"
};


#define LOCATION   "RAM"
#define LOCATION1  "RAM"
#define LOCATION2  "RAM"
#define LOCATIOND  "RAM"

// make this a little bigger so
// all the additional wordlists that are
// alocated when additional threads start have
// an anchor.
#define ROOT_NAME_MAX  24
//#pragma location=LOCATION2
NameType rootNames[ROOT_NAME_MAX] =
{
   {NULL,NULL},
   {NULL,NULL}
};



//#pragma location=LOCATIOND
NameType defines1Names[DEFINES1_NAME_MAX] =
{
   {NULL,NULL},
   {NULL,NULL}
};

// space for definition names
//
// What goes in here are the actual values and the names
// of defines that are compiled in forth.
// Names added with addDefinesCounted string don't
// have the actual text stored here
#ifdef METHOD_ONE
// need enough space for the define value and the names
   #define DEFINES1_CODE_SPACE_SIZE  (DEFINES1_NAME_MAX * 4)
#else
// here just enough for the names of forth defined words
   #define DEFINES1_CODE_SPACE_SIZE  (20)
#endif

// bytes for the values
//#pragma location=LOCATION2
WordPtr  defines1CodeSpace[DEFINES1_CODE_SPACE_SIZE];


//#pragma location=LOCATION1
UserState _user1;
UserStatePtr user1 = &(_user1);



//#pragma location=LOCATION1
WordList rootWordList;
//#pragma location=LOCATION1
WordList coreWordList;
//WordList user1WordList;
//#pragma location=LOCATION1
WordList defines1WordList;




void getLastInWordList(UserStatePtr user)
{
   int i = 1; /* skip over the known beginning null */
   int j;
   NameType* names;
   names = (NameType *)(pop(user));
   while(names[i].code != NULL)
   {
      j = i;
      i++;
   }
   push(user,j);
   //    return(j);
}

void initCoreWordList(UserStatePtr user)
{
   // this is all constants
   init(user);
   coreWordList.cp = (WordPtr *)NULL;
   coreWordList.names = (NameType *)&(coreNames[0]);
   PUSH(user,coreNames);

   // find the last word in the list
   //
   getLastInWordList(user);
   coreWordList.last = (int)(pop(user));
   coreWordList.numberOfNames = sizeof(coreNames) / sizeof(NameType); // size of core array
   coreWordList.codeSpaceStart = (WordPtr *)NULL;
   coreWordList.codeSpaceEnd = (WordPtr *)NULL;
   coreWordList.sizeOfBlock = 10000; // don't care for core , just big to avoid an error check
   strcpy(coreWordList.name,"Core");
}
void initRootWordList(UserStatePtr user)
{
   rootWordList.cp = (WordPtr *)codeSpace;
   rootWordList.names = (NameType *)&(rootNames[0]);

   rootWordList.last = 0;
   rootWordList.numberOfNames = ROOT_NAME_MAX; // size of root array
   rootWordList.codeSpaceStart = (WordPtr *)codeSpace;
   rootWordList.codeSpaceEnd = (WordPtr *)(codeSpace + CODE_SPACE_SIZE);
   rootWordList.sizeOfBlock = 10000; // don't care for root , just big to avoid an error check
   strcpy(rootWordList.name,"Forth");
}

void initDefines1WordList(UserStatePtr user)
{
   // this doesn't get used for defines
   defines1WordList.cp = (WordPtr *)defines1CodeSpace;
   defines1WordList.names = (NameType *)&(defines1Names[0]);

   defines1WordList.last = 0;
   defines1WordList.numberOfNames = DEFINES1_NAME_MAX; // size of defines1 array
   defines1WordList.codeSpaceStart = (WordPtr *)defines1CodeSpace;
   defines1WordList.codeSpaceEnd = (WordPtr *)(defines1CodeSpace + DEFINES1_CODE_SPACE_SIZE);
   defines1WordList.sizeOfBlock = 10000; // don't care for defines1 , just big to avoid an error check
   strcpy(defines1WordList.name,"Defines");
}

void addDefines1ToWordlist(UserStatePtr user)
{

   WordList* current;
   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);

   addName(current,"defines",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(current->cp);
   current->names[current->last + 1].code = NULL;
   constantComma(user);        /* lay down the header */
   PUSH(user,&defines1WordList);
   comma(user);

   addName(current,"Forth",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(current->cp);
   current->names[current->last + 1].code = NULL;
   constantComma(user);        /* lay down the header */
   PUSH(user,&rootWordList);
   comma(user);

   addName(current,"Core",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(current->cp);
   current->names[current->last + 1].code = NULL;
   constantComma(user);        /* lay down the header */
   PUSH(user,&coreWordList);
   comma(user);

}

#ifdef LOCAL_MALLOC
Byte* localMalloc(Int16 sizeRequested);
void localFree(Byte* buffer);
#endif

void initUser1(const char* initStartString)
{
   user1->userVariables.indexedVariables[WORDLIST0] = (Cell)(&coreWordList);
   user1->userVariables.indexedVariables[WORDLIST1] = (Cell)(&rootWordList);
   user1->userVariables.indexedVariables[DEFINES] = (Cell)(&defines1WordList);
   user1->userVariables.indexedVariables[WORDLISTS] = 2; // just core and root
   user1->userVariables.indexedVariables[CURRENT] = (Cell)(&rootWordList);
   user1->userVariables.indexedVariables[STRING_TEMP] = (Cell)(user1->padBuffer);
   user1->userVariables.indexedVariables[START_STRING] = (Cell)(initStartString);
//    user1->userVariables.indexedVariables[START_STRING] = (Cell)(0);
   #ifdef LOCAL_MALLOC
   USER_VAR(user1,MALLOC) = (Cell)localMalloc;
   USER_VAR(user1,FREE) = (Cell)localFree;
   #else
   USER_VAR(user1,MALLOC) = (Cell)malloc;
   USER_VAR(user1,FREE) = (Cell)free;
   #endif
   USER_VAR(user1,ECHO) = 1;
   USER_VAR(user1,ECHOZERO) = 1;
   USER_VAR(user1,READY_FOR_INPUT) = 0;

   USER_VAR(user1,QUIET) = 0;

   USER_VAR(user1,USER_ID) = 0;



   user1->userVariables.namedVariables.userPrintf = forthPrintf;
   user1->userVariables.namedVariables.userPuts =  (PutSType)puts;
   user1->userVariables.namedVariables.userPutch = (PutchType)putch;
   user1->userVariables.namedVariables.userGetch = (GetchType)getch;
   user1->userVariables.namedVariables.userKeyHit = kbhit;
   user1->userVariables.namedVariables.userGets = (GetsType)gets;
   user1->userVariables.namedVariables.getsVector = (GetsVector)plainGets;

   USER_VAR(user1,OLD_GETS0) = USER_VAR(user1,GETS_VECTOR);
   USER_VAR(user1,OLD_GETS1) = USER_VAR(user1,GETS_VECTOR);
   USER_VAR(user1,OLD_GETS2) = USER_VAR(user1,GETS_VECTOR);

   user1->userVariables.namedVariables.user_id = 0; // root user is 0.

}


void radioSilentSetup(UserStatePtr user)
{
   USER_VAR(user,ECHO) = 0;
   USER_VAR(user,ECHOZERO) = 0;
   // this is a function call.
   USER_VAR(user,READY_FOR_INPUT) = 0;
   USER_VAR(user,QUIET) = 0;
}

void cloneRootUser(UserStatePtr user,const char* initStartString, Byte userid)
{
   int i;
   // copy all the existing wordlists.
   user->userVariables.indexedVariables[WORDLISTS] = user1->userVariables.indexedVariables[WORDLISTS];
   for(i = 0;i < user->userVariables.indexedVariables[WORDLISTS];i++)
   {
      user->userVariables.indexedVariables[WORDLIST0 + i] = user1->userVariables.indexedVariables[WORDLIST0 + i];
   }
   user->userVariables.indexedVariables[DEFINES] = user1->userVariables.indexedVariables[DEFINES];


   user->userVariables.indexedVariables[CURRENT] = user1->userVariables.indexedVariables[CURRENT];
   user->userVariables.indexedVariables[STRING_TEMP] = (Cell)(user->padBuffer);
   user->userVariables.indexedVariables[START_STRING] = (Cell)(initStartString);

   #ifdef LOCAL_MALLOC
   USER_VAR(user,MALLOC) = (Cell)localMalloc;
   USER_VAR(user,FREE) = (Cell)localFree;
   #else
   USER_VAR(user,MALLOC) = (Cell)malloc;
   USER_VAR(user,FREE) = (Cell)free;
   #endif
   USER_VAR(user,ECHO) = 1;
   USER_VAR(user,ECHOZERO) = 1;
   USER_VAR(user,READY_FOR_INPUT) = 0;

   USER_VAR(user,QUIET) = 0;

   USER_VAR(user,USER_ID) = userid;


   user->userVariables.namedVariables.getsVector = (GetsVector)plainGets;
   USER_VAR(user,OLD_GETS0) = USER_VAR(user,GETS_VECTOR);
   USER_VAR(user,OLD_GETS1) = USER_VAR(user,GETS_VECTOR);
   USER_VAR(user,OLD_GETS2) = USER_VAR(user,GETS_VECTOR);

   user->userVariables.namedVariables.user_id = userid;

   user->doColon = user1->doColon;
   user->doVariable = user1->doVariable;
   user->doConstant = user1->doConstant;
   user->dollarCompile = user1->dollarCompile;
   user->dollarInterpret = user1->dollarInterpret;


   user->userVariables.namedVariables.userPrintf =     user1->userVariables.namedVariables.userPrintf;
//    user->userVariables.namedVariables.maskPrintf =     user1->userVariables.namedVariables.maskPrintf;
   user->userVariables.namedVariables.userPuts   =     user1->userVariables.namedVariables.userPuts;
   user->userVariables.namedVariables.userPutch  =     user1->userVariables.namedVariables.userPutch;
   user->userVariables.namedVariables.userGetch  =     user1->userVariables.namedVariables.userGetch;
   user->userVariables.namedVariables.userKeyHit =     user1->userVariables.namedVariables.userKeyHit;
   user->userVariables.namedVariables.userGets   =     user1->userVariables.namedVariables.userGets;
   // user->userVariables.namedVariables.getsVector =     user1->userVariables.namedVariables.getsVector;



}



void initAdditionalUser(UserStatePtr user,const char* initStartString, Byte userid)
{
   user->userVariables.indexedVariables[WORDLIST0] = (Cell)(&coreWordList);
   user->userVariables.indexedVariables[WORDLIST1] = (Cell)(&rootWordList);
   user->userVariables.indexedVariables[DEFINES] = (Cell)(&defines1WordList);
   user->userVariables.indexedVariables[WORDLISTS] = 2; // just core and root
   user->userVariables.indexedVariables[CURRENT] = (Cell)(&rootWordList);
   user->userVariables.indexedVariables[STRING_TEMP] = (Cell)(user->padBuffer);
   user->userVariables.indexedVariables[START_STRING] = (Cell)(initStartString);
//    user->userVariables.indexedVariables[START_STRING] = (Cell)(0);
   #ifdef LOCAL_MALLOC
   USER_VAR(user,MALLOC) = (Cell)localMalloc;
   USER_VAR(user,FREE) = (Cell)localFree;
   #else
   USER_VAR(user,MALLOC) = (Cell)malloc;
   USER_VAR(user,FREE) = (Cell)free;
   #endif
   USER_VAR(user,ECHO) = 1;
   USER_VAR(user,ECHOZERO) = 1;
   USER_VAR(user,READY_FOR_INPUT) = 0;

   USER_VAR(user,QUIET) = 0;

   USER_VAR(user,USER_ID) = 0;
   
   user->userVariables.namedVariables.getsVector = (GetsVector)plainGets;
   USER_VAR(user,OLD_GETS0) = USER_VAR(user,GETS_VECTOR);
   USER_VAR(user,OLD_GETS1) = USER_VAR(user,GETS_VECTOR);
   USER_VAR(user,OLD_GETS2) = USER_VAR(user,GETS_VECTOR);

   user->userVariables.namedVariables.user_id = userid;

   user->doColon = user1->doColon;
   user->doVariable = user1->doVariable;
   user->doConstant = user1->doConstant;
   user->dollarCompile = user1->dollarCompile;
   user->dollarInterpret = user1->dollarInterpret;

}


// Little word to print out the status
void dictStatus(UserStatePtr user)
{
   WordList* current;
   int wordListCount;
   int32_t spaceSize;
   int32_t spaceUsed;
   int32_t stackDepth;
   int32_t returnStackDepth;


   // first go through all the wordlists and see what's up
   // include the code space status
   wordListCount = USER_VAR(user,WORDLISTS);
   wordListCount++; // include the constants list
   FPRINTF("\r\nForth status\r\n");
   while(wordListCount > 0)
   {
      current = (WordList *)USER_VAR(user,DEFINES + wordListCount - 1);
      if(current != NULL)
      {
         FPRINTF("Wordlist : %s\r\n",current->name);
         FPRINTF("           List size %d  Current Usage %d\r\n",
                 current->numberOfNames,
                 current->last);
         spaceSize = (int32_t)(current->codeSpaceEnd) - (int32_t)(current->codeSpaceStart);
         spaceSize /= CELL;
         spaceUsed = (int32_t)(current->cp) - (int32_t)(current->codeSpaceStart);
         spaceUsed /= CELL;
         FPRINTF("           Code space size %d (in Cells) Usage %d (in Cells)\r\n",spaceSize,spaceUsed);
      } // if current is not null
      wordListCount--;
   } // while wordlists

   // OOPS
   // ALREADY taken care of by virtue of +1 on the wordlist count
   // and the fact that the defines wordlist is the first one before core
   // printf out definition status if available
   #if 0
   if(USER_VAR(user,DEFINES) != 0)
   {
      current = (WordList*)USER_VAR(user,DEFINES);
      if(current != NULL)
      {
         FPRINTF("Wordlist : %s\r\n",current->name);
         FPRINTF("List size %d  Current Usage %d\r\n",
                 current->numberOfNames,
                 current->last);
         spaceSize = (int32_t)(current->codeSpaceEnd) - (int32_t)(current->codeSpaceStart);
         spaceSize /= CELL;
         spaceUsed = (int32_t)(current->cp) - (int32_t)(current->codeSpaceStart);
         spaceUsed /= CELL;
         FPRINTF("Code space size %d (in Cells) Usage %d (in Cells)\r\n",spaceSize,spaceUsed);
      } // if current is not null
   } //
   #endif


   // and the stack and return stack status.
   stackDepth = (int32_t)(USER_VAR(user,SP)) - (int32_t)(USER_VAR(user,SP0));
   stackDepth /= CELL;
   returnStackDepth = (int32_t)(USER_VAR(user,RP0)) - (int32_t)(USER_VAR(user,RP));
   returnStackDepth /= CELL;
   FPRINTF("Stack Status : \r\n");
   FPRINTF("               Stack depth %d (Cells) Return Stack Depth %d (cells)\r\n",stackDepth,returnStackDepth);
}

/********************************************************************************************************/
/********************************************************************************************************/
/* This is initialization words to build the strings above into the dictionary */
/* This is done this way so the catch throw mechanism can be in forth */
/* and work properly */
/********************************************************************************************************/
/********************************************************************************************************/
// Add a name to the dictionary
void addName(WordList* wordList,char* name,Byte lex)
{
   int length;
   wordList->last++;
   wordList->names[wordList->last].text = (char *)(wordList->cp);
   #ifdef USE_HELP
   wordList->names[wordList->last].help = NULL;
   #endif

   #ifdef LONG_STRING_COUNT
   length = strlen(name) + 1;
   memcpy((Byte *)(wordList->cp) + 1,(void const *)name,length);  // 0 included
   *((Byte *)(wordList->cp)) = lex | length;
   wordList->cp = cellAlignedMove(wordList->cp,length + 1); /* allow for the zero and count*/
   #else
   length = strlen(name);
   memcpy((Byte *)(wordList->cp) + 1,(void const *)name,length + 1);  /* copies the 0 */
   *((Byte *)(wordList->cp)) = lex | length;
   wordList->cp = cellAlignedMove(wordList->cp,length + 2); /* allow for the zero and count*/
   #endif

   wordList->names[wordList->last].code = NULL;   // no code pointer yet
}

// Quick initialization
// of the core interpreter words.
// Use arrays with the header in the array.
// And hand input the pointers.

// this is in pointer sized objects
// NOTE : This size apparently changes on the ARM from
// one compiler rev to the next and how far away things are, i.e. code
// size.
// If things don't behave increase this number
// this is in pointer sized objects
#define HEADER_SIZE  4
#if HEADER_SIZE==2
   #define HEADER_SPACE 0,0
#endif
#if HEADER_SIZE==4
   #define HEADER_SPACE 0,0,0,0
#endif
#if  HEADER_SIZE==7
   #define HEADER_SPACE 0,0,0,0,0,0,0
#endif
#if  HEADER_SIZE==8
   #define HEADER_SPACE 0,0,0,0,0,0,0,0
#endif
#if  HEADER_SIZE==9
   #define HEADER_SPACE 0,0,0,0,0,0,0,0,0
#endif
#if  HEADER_SIZE==10
   #define HEADER_SPACE 0,0,0,0,0,0,0,0,0,0
#endif
#if  HEADER_SIZE==12
   #define HEADER_SPACE 0,0,0,0,0,0,0,0,0,0,0,0
#endif

void noOp(UserStatePtr user)
{
}



//#pragma location=LOCATION
WordPtr catchWord[] =
{
   HEADER_SPACE,
   spat,toR,handler,at,toR,rpat,handler,bang,execute,fromR,handler,bang,fromR,drop,doLIT,0,exiT
};

//#pragma location=LOCATION
WordPtr throwWord[] =
{
   HEADER_SPACE,
   handler,at,rpstore,fromR,handler,bang,fromR,swap,toR,spstore,drop,fromR,exiT
};

//#pragma location=LOCATION
WordPtr _abortQuoteWord[] =
{
   HEADER_SPACE,
   qbranch,(WordPtr)5,    doDollar,(WordPtr)throwWord,    branch,(WordPtr)3,
   doDollar,    drop,    exiT
};
//#pragma location=LOCATION
WordPtr abortQuoteWord[] =
{
   HEADER_SPACE,
   doLIT,(WordPtr)_abortQuoteWord,comma,fstring,drop,exiT
};
// Fatal M3 architecture problem
// the 3 attached to the qbranch below gets turned into
// 5 because function pointers have to be odd
// so the cast makes it odd
// This is the wrong amount of branching because
// this is the number of cells to move forward.
// To fix this we have to make all the function pointers
// even, and make sure execute and the main interpreter
// set the low order bit before calling.
// We shall see if this works when we make our own if then else stuff
//
// Possible solution find all the even jumps and make them odd,
// then pad a 0 before the jump destination
// Problem, backward jumps ...!!
// Need a noop function
// Plus any compiled jumps
//#pragma location=LOCATION
WordPtr dollarCompileWord[] =
{
   HEADER_SPACE,
   /* nameQ returns
   either the string pointer and a null if not found
   or the pointer to the word.
   If the pointer is non null duplicate it then either run it if
   immediate, or compile it. If the result was null, just
   leave the pointer to the token on the stack and look for it
   in the defines.
   */
   nameQ,nameToText,qDup,qbranch,(WordPtr)0x0D,_charat,
   doLIT,(WordPtr)IMMEDIATE,and,qbranch,(WordPtr)5,
   execute,exiT,branch,(WordPtr)3,comma,exiT,
   #ifdef METHOD_ONE
   defineQ,qbranch,(WordPtr)6,doDEFINE,compile,doLIT,comma,exiT,
   #else
   defineQW,qbranch,(WordPtr)6,doDEFINE,compile,doLIT,comma,exiT,
   #endif


   // if define can't find it it might be a string or number
   // first check to see if a string
   // allow strings to be tokens
   /* stringQ returns either the string and a null
   or a length and a pointer to the string.
   for a string lay down the word to push the string value
   */
   stringQ,qDup,qbranch,(WordPtr)3,compileString,exiT,
   numberQ,equalsZero,qbranch,(WordPtr)4,(WordPtr)throwWord,
   branch,(WordPtr)4,compile,doLIT,comma,exiT
};


//#pragma location=LOCATION
WordPtr dollarInterpretWord[] =
{
   HEADER_SPACE,
   nameQ,qDup,qbranch,(WordPtr)4,drop,execute,exiT,
   #ifdef METHOD_ONE
   defineQ,qbranch,(WordPtr)3,doDEFINE,exiT,
   #else
   defineQW,qbranch,(WordPtr)3,doDEFINE,exiT,
   #endif

   // try and make an array in a temporary buffer
   // if we can we have a pointer to the temporary RTOS buffer
   // and we just leave that on the stack

   // otherwise try to make as string out of it
   // call the string word, which builds the string in temp memory
   // we get a pointer and a count, if the count is 0 pointer is
   // to the begining token
   // do nothing, leave the pointer to the string on the top of the stack
   // after backing up to the count value
   // note the string has just been copied
   // but converted with escape sequences
   // do that in place
   // make a copy of pointer to string
   stringQ,qDup,qbranch,(WordPtr)4,tempStringBuffer,stringDollar,exiT,
   // if we can't make a string out if it
   // otherwise try to make a number out of it
   numberQ,qbranch,(WordPtr)2,exiT,(WordPtr)throwWord
};

void compilingQ(UserStatePtr user)
{
   if(USER_VAR(user,STATE) == COMPILING)
   {
      PUSH(user,-1);
   }
   else
   {
      PUSH(user,0);
   }
}


WordPtr dollarArrayWord[] =
{
   HEADER_SPACE,
   compilingQ,qbranch,(WordPtr)14,
   // this is the compiling branch
   nameQ,qDup,qbranch,(WordPtr)4,drop,execute,exiT,
   // otherwise make a number out of it and put in the array
   numberQ,qbranch,(WordPtr)3,comma,exiT,(WordPtr)throwWord,

   // this is the interpreting branch
   // first try to execute a word if possible
   // this allows the ]ARRAY word to be executed
   nameQ,qDup,qbranch,(WordPtr)4,drop,execute,exiT,
   // otherwise make a number out of it and put in the array
   numberQ,qbranch,(WordPtr)3,arrayPut,exiT,(WordPtr)throwWord
};

void jungle(FCONTEXT)
{
}

//#pragma location=LOCATION
WordPtr tickWord[] =
{
   HEADER_SPACE,
//    tokeN,nameQ,equalsZero,qbranch,(WordPtr)6,defineQ,qbranch,(WordPtr)2,exiT,(WordPtr)throwWord,exiT
   tokeN,nameQ,equalsZero,qbranch,(WordPtr)8,defineQ,qbranch,(WordPtr)2,exiT,
   drop,doLIT,(WordPtr)0x00,exiT
};
//#pragma location=LOCATION
WordPtr evalWord[] =
{
   HEADER_SPACE,
   tokeN,dupp,_charat,qbranch,(WordPtr)6,
   tickeval,at,execute,branch,(WordPtr)-9,drop,dotPrompt,exiT
};


//#pragma location=LOCATION
WordPtr quitWord[] =
{
   HEADER_SPACE,
   query,doLIT,(WordPtr)evalWord,(WordPtr)catchWord,qDup,qbranch,(WordPtr)10,
   count,type,spzero,spstore,huh,cr,leftBracket,branch,(WordPtr)1,branch,(WordPtr)-17,exiT
};


// WordPtr quitWord[] =
// {
//    HEADER_SPACE,
//    query,doLIT,(WordPtr)evalWord,(WordPtr)catchWord,qDup,qbranch,(WordPtr)10,
//    count,type,spzero,spstore,huh,cr,leftBracket,branch,(WordPtr)1,branch,(WordPtr)-17,exiT
// };


void abortSingle(UserStatePtr user)
{
   USER_VAR(user,ABORT) = 1;
}

void exitSingle(UserStatePtr user)
{
   USER_VAR(user,ABORT) = 2;
}


WordPtr singleInterpretWord[] =
{
   HEADER_SPACE,
   dummyQuery,doLIT,(WordPtr)evalWord,(WordPtr)catchWord,qDup,qbranch,(WordPtr)8,
   count,type,spzero,spstore,abortSingle,cr,leftBracket,exitSingle,exiT
};


// immediate word
//#pragma location=LOCATION
WordPtr bracketCOMPILEWord[] =
{
   HEADER_SPACE,
   (WordPtr)tickWord,comma,exiT
};

// immediate word
// [']
//#pragma location=LOCATION
WordPtr bracketTickWord[] =
{
   HEADER_SPACE,
   (WordPtr)tickWord,literal,exiT
};



int oneOfThoseWords(void* ptr)
{
   if(ptr == bracketTickWord)
   {
      return(1);
   }
   if(ptr == bracketCOMPILEWord)
   {
      return(1);
   }
   if(ptr == quitWord)
   {
      return(1);
   }
   if(ptr == singleInterpretWord)
   {
      return(1);
   }

   if(ptr == evalWord)
   {
      return(1);
   }
   if(ptr == tickWord)
   {
      return(1);
   }
   if(ptr == dollarInterpretWord)
   {
      return(1);
   }
   if(ptr == dollarCompileWord)
   {
      return(1);
   }

   if(ptr == abortQuoteWord)
   {
      return(1);
   }
   if(ptr == _abortQuoteWord)
   {
      return(1);
   }
   if(ptr == throwWord)
   {
      return(1);
   }
   if(ptr == catchWord)
   {
      return(1);
   }
   if(ptr == dollarArrayWord)
   {
      return(1);
   }

   return(0);

}

void makeFunction(void* buffer,int numwords)
{
   uint32_t* array = (uint32_t *)buffer;
   int i;
   for(i = HEADER_SIZE;i < numwords;i++)
   {
      if(oneOfThoseWords((void *)(array[i])))
      {
         array[i] |= 1;
      }
   }
}


#ifdef  M3FIX
   #define FIXUP(a)      memcpy((Byte *)(a),FUNCTION(jumpColon),headerSize); \
    makeFunction((void *)(a),sizeof(a)/sizeof(WordPtr));
#else
   #define FIXUP(a)      memcpy((Byte *)(a),FUNCTION(jumpColon),headerSize);
#endif


void fixup(void)
{
   FIXUP(catchWord);
   FIXUP(throwWord);
   FIXUP(_abortQuoteWord);
   FIXUP(abortQuoteWord);
   FIXUP(dollarCompileWord);
   FIXUP(dollarInterpretWord);
   FIXUP(tickWord);
   FIXUP(evalWord);
   FIXUP(quitWord);
   FIXUP(singleInterpretWord);
   FIXUP(bracketCOMPILEWord);
   FIXUP(bracketTickWord);
   FIXUP(dollarArrayWord);
} //



void initArrayWords(UserStatePtr user)
{
   WordList* current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);

   current->cp = (WordPtr *)codeSpace;

   init(user);

   //    current->last++;
   addName(current,"catch",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(catchWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"throw",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(throwWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"abort\x22",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(_abortQuoteWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"ABORT\x22",IMMEDIATE); // make immediate
   current->names[current->last].code = MAKEFUNCTIONCALL(abortQuoteWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"$COMPILE",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(dollarCompileWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"$INTERPRET",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(dollarInterpretWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"$ARRAY",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(dollarArrayWord);
   current->names[current->last + 1].code = NULL;
   #ifndef SECURE
   // prevent ticking a word, therefore finding its execution address
   // prevents overwriting a location with assembly language and
   // using it as a jump to location
   // also prevents walking words that have execute in them
   // to find the address of execute.
   addName(current,"`",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(tickWord);
   current->names[current->last + 1].code = NULL;
   #endif
   addName(current,"EVAL",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(evalWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"QUIT",0);
   current->names[current->last].code = MAKEFUNCTIONCALL(quitWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"[COMPILE]",IMMEDIATE); // make immediate
   current->names[current->last].code = MAKEFUNCTIONCALL(bracketCOMPILEWord);
   current->names[current->last + 1].code = NULL;
   addName(current,"SINGLE",0); // make immediate
   current->names[current->last].code = MAKEFUNCTIONCALL(singleInterpretWord);
   current->names[current->last + 1].code = NULL;
   #ifndef SECURE
   // prevent ticking a word, therefore finding its execution address
   // prevents overwriting a location with assembly language and
   // using it as a jump to location
   // also prevents walking words that have execute in them
   // to find the address of execute.
   addName(current,"[']",IMMEDIATE); // make immediate
   current->names[current->last].code = MAKEFUNCTIONCALL(bracketTickWord);
   current->names[current->last + 1].code = NULL;
   #endif
}

Byte addWord(UserStatePtr user,WordPtr function,char* name)
{
   //    NameType *names;
   WordList* wordList;

   wordList = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   addName(wordList,name,0);
   //     wordList = (WordListPtr)(userState[CURRENT]);
   //    names = wordList->names;
   //    names[WORDLIST_LAST].CFA.code = function;
   wordList->names[wordList->last].code = function;   /* mark one past end as the end */
   wordList->names[wordList->last + 1].code = NULL;
   return(0);
} //addword


Byte addWordWithHelp(UserStatePtr user,WordPtr function,char* name,char* helpString)
{
   //    NameType *names;
   WordList* wordList;

   wordList = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   addName(wordList,name,0);
   //     wordList = (WordListPtr)(userState[CURRENT]);
   //    names = wordList->names;
   //    names[WORDLIST_LAST].CFA.code = function;
   wordList->names[wordList->last].code = function;   /* mark one past end as the end */
   wordList->names[wordList->last + 1].code = NULL;
   #ifdef USE_HELP
   wordList->names[wordList->last].help = helpString;
   #endif
   return(0);
} //addword

// expects a pointer to a forth word function
// and a counted string, where bytecount includes the 0 as part of the length
Byte addWordCountedString(UserStatePtr user,WordPtr function,char* name)
{
   //    NameType *names;
   WordList* wordList;

   wordList = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   wordList->last++;
   wordList->names[wordList->last].text = name;
   wordList->names[wordList->last].code = function;
   #ifdef USE_HELP
   wordList->names[wordList->last].help = NULL;
   #endif
   wordList->names[wordList->last + 1].code = NULL;
   return(0);
} //addword


Byte addWordCountedStringWithHelp(UserStatePtr user,WordPtr function,char* name,char* helpString)
{
   //    NameType *names;
   WordList* wordList;

   wordList = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   wordList->last++;
   wordList->names[wordList->last].text = name;
   wordList->names[wordList->last].code = function;
   #ifdef USE_HELP
   wordList->names[wordList->last].help = helpString;
   #endif
   wordList->names[wordList->last + 1].code = NULL;
   return(0);
} //addword

Byte addWordCountedStringWithHelpCountedString(UserStatePtr user,WordPtr function,char* name,char* helpString)
{
   //    NameType *names;
   WordList* wordList;

   wordList = (WordList *)(user->userVariables.indexedVariables[CURRENT]);
   wordList->last++;
   wordList->names[wordList->last].text = name;
   wordList->names[wordList->last].code = function;
   #ifdef USE_HELP
   wordList->names[wordList->last].help = ++helpString;
   #endif
   wordList->names[wordList->last + 1].code = NULL;
   return(0);
} //addword


#if 1
Byte addDefineCountedString(UserStatePtr user,Cell value,CountedString name)
{
   WordList* wordList;
   if(USER_VAR(user,DEFINES) == 0)
   {
      printf("No define wordlist!! ");
      return(0);
   }

   wordList = (WordList *)(user->userVariables.indexedVariables[DEFINES]);
   wordList->last++;
   if(wordList->last >= DEFINES1_NAME_MAX)
   {
      printf("Compile constant error: Halting\r\n");
      printf("Not enough define space\r\n");
      while(1);
   }
   wordList->names[wordList->last].text = (char *)name;
   #ifdef METHOD_ONE
   wordList->names[wordList->last].code = (WordPtr)(wordList->cp);
   *(wordList->cp)++ = (WordPtr)value;
   #else
   wordList->names[wordList->last].code = (WordPtr)(value);
   #endif
   return(0);
} //addword
Byte addDefineCountedStringWithHelpCountedString(UserStatePtr user,Cell value,CountedString name,char* helpString)
{

   WordList* wordList;
   if(USER_VAR(user,DEFINES) == 0)
   {
      printf("No define wordlist!! ");
      return(0);
   }

   wordList = (WordList *)(user->userVariables.indexedVariables[DEFINES]);
   wordList->last++;
   if(wordList->last >= DEFINES1_NAME_MAX)
   {
      printf("Compile constant error: Halting\r\n");
      printf("Not enough define space\r\n");
      while(1);
   }
   wordList->names[wordList->last].text = (char *)name;
   #ifdef USE_HELP
   wordList->names[wordList->last].help = ++helpString;
   #endif
   #ifdef METHOD_ONE
   wordList->names[wordList->last].code = (WordPtr)(wordList->cp);
   *(wordList->cp)++ = (WordPtr)value;
   #else
   wordList->names[wordList->last].code = (WordPtr)(value);
   #endif
   return(0);
}

Byte addDefineCountedStringWithHelp(UserStatePtr user,Cell value,CountedString name,char* helpString)
{
   WordList* wordList;
   if(USER_VAR(user,DEFINES) == 0)
   {
      printf("No define wordlist!! ");
      return(0);
   }

   wordList = (WordList *)(user->userVariables.indexedVariables[DEFINES]);
   wordList->last++;
   if(wordList->last >= DEFINES1_NAME_MAX)
   {
      printf("Compile constant error: Halting\r\n");
      printf("Not enough define space\r\n");
      while(1);
   }
   wordList->names[wordList->last].text = (char *)name;
   #ifdef USE_HELP
   wordList->names[wordList->last].help = helpString;
   #endif
   #ifdef METHOD_ONE
   wordList->names[wordList->last].code = (WordPtr)(wordList->cp);
   *(wordList->cp)++ = (WordPtr)value;
   #else
   wordList->names[wordList->last].code = (WordPtr)(value);
   #endif
   return(0);
} //addword

#endif


void addExternalWords(UserStatePtr user);




#ifdef INCLUDE_CONSTANTS

void makeConstant(const char* const name,const SignedCell value)
{
   WordList* current;
   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);

   addName(current,(char *)name,0);
   current->names[current->last].code = (WordPtr)(current->cp);
   constantComma(user);        /* lay down the header */
   PUSH(user,value);
   comma(user);
}

const UserConstants userConstantsInit[] =
{
   {"_IP",FORTH_IP},
   {"_RP",RP},
   {"_SP",SP},
   {"_LAST",LAST},
   {"_SP0",SP0},
   {"_RP0",RP0},
   {"_STATE",STATE},
   {"_CSP",CSP},
   {"_CURRENT",CURRENT},
   {"_HANDLER",HANDLER},
   {"_TICKEVAL",TICKEVAL},
   {"_BASE",BASE},
   {"_TICKECHO",TICKECHO},
   {"_TICKKEY",TICKKEY},
   {"_TICKEMIT",TICKEMIT},
   {"_PTIB",PTIB},
   {"_PPAD",PPAD},
   {"_TO_IN",TO_IN},
   {"_HLD",HLD},
   {"_WORDLISTS",WORDLISTS},
   {"_WORDLIST0",WORDLIST0},
   {"_WORDLIST1",WORDLIST1},
   {"_WORDLIST2",WORDLIST2},
   {"_WORDLIST3",WORDLIST3},
   {"_DEFINES",DEFINES},
   {"_MALLOC",MALLOC},
   {"_FREE",FREE},
   {NULL,0}
};

void initConstants(void)
{
   //    int i;
   //    i = 0;
   UserConstants* constants;

   constants = (UserConstants *)userConstantsInit;
   while(constants->name != NULL)
   {
      makeConstant(constants->name,constants->value);
      constants++;
   }
}

#endif
/**********************************************************************/

/**********************************************************************/


/* creation of the name space */
/* and initialization of the pre-existing name space */
/* what we need is an array of names */
/* maybe a simplification */
/* names are the following : */

/* thus the name space is the the following */
/* an array of NameType with pointers to names that are constants */
/* when compiled. For words defined at run time you add to the end */
/* of the name space. I.E. LAST points to then end of the array */
/* and to the free pool of the name space ram */
/* when a token is created it is copied to the end of the name space */
/* and a new table entry is filled out */



void strUpr(char* buffer);
void strUpr(char* string)
{
   while(*string)
   {
      *string = toupper(*string);
      string++;
   }
}


int search(UserStatePtr user,char* buffer)
{
   char local[128];
   int len;
   int end;
   WordList* current;

   current = (WordList *)(user->userVariables.indexedVariables[CURRENT]);

   strcpy(local,buffer);
   strUpr(local);
   len = strlen(local);

   end = (current->last);

   while(end >= 0)
   {
      /* this statment depends on left to right evaluation */
      /* to speed the search */
      if((((current->names[end].text[0]) & LENGTH_MASK) == len) && (strcmp(local,current->names[end].text + 1) == 0))
      {
         return(end);
      } /* if */
      end--;
   } /* while */
   return(-1);
} /* search */


void rpDump(UserStatePtr user)
{
   WordPtr** rpTemp;
   WordPtr** rpStop;
   rpTemp = (user->userVariables.namedVariables.rp) - 1;
   rpStop = (WordPtr **)(user->userVariables.namedVariables.rpStart);
   while(rpTemp >= rpStop)
   {
      FPRINTF(" %p %p\r\n",rpTemp,*rpTemp);
      rpTemp--;
   }

}

NameType* cfaSearch(UserStatePtr user,WordPtr searchFor,WordList** whichWordList,int32_t* whichIndex)
{
   int32_t end;
   int wordListCount;
   WordList* searchWordList;
   NameType* name;

   wordListCount = USER_VAR(user,WORDLISTS);
   wordListCount++; // make sure and search the constants
   while(wordListCount)
   {
      searchWordList = (WordList *)(USER_VAR(user,(DEFINES + wordListCount - 1)));
      if(searchWordList != NULL)
      {
         end = (searchWordList->last);
         name = searchWordList->names + end;
         while(end > 0)
         {
            /* this statment depends on left to right evaluation */
            /* to speed the search */
            #ifdef M3FIX
            if(((~1) & ((uint32_t)(name->code))) == (((uint32_t)searchFor) & (~1)))
            #else
            if((((uint32_t)(name->code))) == (((uint32_t)searchFor)))
            #endif
               {
                  *whichIndex = end;
                  *whichWordList = searchWordList;
                  return(name);
               } /* if */
            end--;
            name--;
         } /* while */
      } // if
      wordListCount--;
   } //while wordlists to do
   return(NULL);
} /* search */

void display(UserStatePtr user,WordPtr* tokens,int count)
{
   NameType* name;
   WordList* whichWordList;
   SignedCell    index;
   int      i;


   for(i = 0;i < count;i++)
   {

      name = cfaSearch(user,*tokens,&whichWordList,&index);
      FPRINTF("\r\n %X:",tokens);
      if(name)
      {
         FPRINTF(" %X %s",*tokens,(name->text) + 1);
      }
      else
      {

         Cell value = (Cell)(*tokens);
         #if PTR_SIZE==4
         FPRINTF(" %02X.%02X.%02X.%02X ....[%c%c%c%c]",(Byte)(value >> 24),(Byte)(value >> 16),(Byte)(value >> 8),(Byte)value,
                 (isprint((Byte)(value)) ? (Byte)(value) : ' '),
                 (isprint((Byte)(value >> 8)) ? (Byte)(value >> 8) : ' '),
                 (isprint((Byte)(value >> 16)) ? (Byte)(value >> 16) : ' '),
                 (isprint((Byte)(value >> 24)) ? (Byte)(value >> 24) : ' '));
         #else
         FPRINTF(" %02X.%02X ....[%c%c]",(Byte)(value >> 8),(Byte)value,
                 (isprint((Byte)(value)) ? (Byte)(value) : ' '),
                 (isprint((Byte)(value >> 8)) ? (Byte)(value >> 8) : ' '));
         #endif

      }
      tokens++;
   } //for
} // display
/* expects a ca */
void see(UserStatePtr user)
{
   uint8_t* p;
   WordPtr* tokens;
   WordPtr* tokensBase;
   //    NameType *name;
   //    int  i;
   //    NameType *name;

   tokeN(user);
   nameQ(user);
   p = (uint8_t *)pop(user);
   if(p == NULL)
   {
      FPRINTF("not found..\r\n");
      pop(user);
      return;
   }
   #if 0
   name = (NameType*)pop(user);
   p = (byte *)name->code;
   #else
   p = (uint8_t *)pop(user);
   #endif
   //    p = (byte *)pop(user);
   /*
   for(i=0;i<headerSize;i++)
   {
   FPRINTF("%p %02x ",p,*p);
   if(((i+1) & 7) == 8)FPRINTF("\n");
   p++;
   }
   */
   #ifdef M3FIX
   p--; // back up the +1 bit for m3
   #endif
   p += headerSize;
   tokensBase = tokens = (WordPtr *)p;
   display(user,tokens,20);
   tokens += 20;
   while(1)
   {
      FPRINTF("\r\n");
      switch(GETCH())
      {
         case 'q':
         case 'Q':
            return;
         case 'H':
         case 'h':
            tokens = tokensBase;
            display(user,tokens,20);
            break;
         case 'U':
         case 'u':
            tokens -= 40;
            if((int32_t)tokens < (int32_t)tokensBase)
            {
               tokens = tokensBase;
            }
            display(user,tokens,20);
            tokens += 20;
            break;
         case 0x0d :
            display(user,tokens,1);
            tokens++;
            break;
         case 'P':
         case 'p':
            tokens -= 2;
            display(user,tokens,1);
            tokens++;
            break;
         case 'D':
         case 'd':
         case ' ':
            display(user,tokens,20);
            tokens += 20;
            break;
         default :
            break;

      } // switch
   } //while
}

#define NUMBER_ERROR_STRINGS 15
const char* const errorStrings[] =
{
   "Stack empty",
   "Return Stack Empty",
   "Failed trying to print",
   "Pack dollar string length",
   "$, error"
};


// this initializes the core interpreter before any user contexts run
void initCoreForth(const char* initStartString)
{
   unsigned int errorCode;
   UserStatePtr user;


   //    printf("Forth...starting\n");
   /****************************************************/
   /* initialization, builds all the root words        */
   //   (user->userVariables.namedVariables.last) = getLast(user);
   //   (user->userVariables.namedVariables.textLast) = nameSpace;
   align = sizeof(WordPtr);   /* figures out a pointers alignment */
   /* align will be either 2 4 or 8 bytes */
   headerSize = (int)jumpVariable - (int)(jumpColon);
   if(((headerSize / align) * align) != headerSize)
   {
      /* header is not a multiple of a pointer so adjust it */
      headerSize = ((headerSize / align) + 1) * align;
   }

   // printf("Header size %d\r\n",headerSize);
   if((HEADER_SIZE * sizeof(Cell)) != headerSize)
   {
      printf("Header sizes don't match! %d %d\r\r\n",HEADER_SIZE * sizeof(Cell),headerSize);
      while(1);
   }
   //    while(1);


   fixup();

   // this logic initializes the core forth in context #1
   // other users don't do as much
   user = &(_user1);

   init(user);
   initCoreWordList(user);
   initRootWordList(user);
   initDefines1WordList(user);

   initUser1(initStartString);

   #ifdef GLOBAL_DOS
   user->doColon = doColon = doLIST;
   user->doVariable = doVariable = doVAR;
   user->doConstant = doConstant = doCONST;
   #else
   user->doColon = doLIST;
   user->doVariable = doVAR;
   user->doConstant = doCONST;
   #endif
   //    user->doUser = doUSER;
   //    user->doDoes = doDoes = doDOES;
   //    user->doDefine = doDefine = doDEFINE;


   // FPRINTF("mForth 0.0\r\n");
   errorCode = setjmp(user->jumper);
   if(errorCode == 0)
   {
      initArrayWords(user);
      // make sure the defines wordlist shows up as a word so you can use it
      // for SOME-WORDS
      addDefines1ToWordlist(user);
      #ifdef COMPILED_ROOT
      initWords(user);
      #endif
      #ifdef INCLUDE_CONSTANTS
      initConstants();
      #endif
      rootLast = rootWordList.last; // user->userVariables.namedVariables.last);
   }
   else
   {
      FPRINTF("Word initialization error at startup, cannot proceed \r\n");
      return;
      //        exit(1);
   }
}

#ifdef LONG_STRING_COUNT
//                             01234567890
   #define INTERPRETSTRING "\xB" "$INTERPRET"
   #define ARRAYSTRING     "\x7" "$ARRAY"
   #define COMPILESTRING   "\x9" "$COMPILE"
   #define QUITSTRING      "\x5" "QUIT"
#else
//                             0123456789
   #define INTERPRETSTRING "\xA" "$INTERPRET"
   #define COMPILESTRING   "\x8" "$COMPILE"
   #define ARRAYSTRING     "\x6" "$ARRAY"
   #define QUITSTRING      "\x4" "QUIT"
#endif


/**
@todo
perform is broken.
Needs the same fixes applied to execute and @execute
**/

void perform(UserStatePtr user)
{
   WordPtr val;
   WordPtr* temp;
   WordPtr volatile* ipSave;
   WordPtr  indirect[2];

   val = (WordPtr)pop(user);
   ipSave = user->userVariables.namedVariables.ip;
   indirect[0] = (WordPtr)val;

   // Find the exit for THIS word, not just any exit
   temp = (WordPtr *)(((Byte *)val) + headerSize);
   while(*temp != exiT)
   {
      temp++;
   }

   user->userVariables.namedVariables.ip = &(indirect[0]);
   while(user->userVariables.namedVariables.ip != temp)
   {
      (*(user->userVariables.namedVariables.ip)++)(user);
   }
   // now do exit ourselves
   // this just cleans up the return stack that calling dolist pushed
   (user->userVariables.namedVariables.ip) = (WordPtr *)rpPop(user);
   // now recover the saved ip
   user->userVariables.namedVariables.ip = ipSave;
} /* execute */

static volatile int badCall;

/**
 * Add a local semaphore to be used for pending on events. 
 * Called from C 
 * These aren't used by forth, just placeholders for the 
 * particular context. 
 * 
 * @author nowelc2 (5/12/2017)
 * 
 * @param user 
 * @param user 
 */
void addSemaphoreLogic(UserStatePtr user,uint32_t semaphore,uint32_t callBack)
{
   user->userVariables.namedVariables.semaphore = semaphore;
   user->userVariables.namedVariables.semaCallback = callBack;
}
void semaphoreAt(FCONTEXT)
{
   UPUSH(user->userVariables.namedVariables.semaphore);
}

void runForth(UserStatePtr user)
{
   unsigned int errorCode;
   WordPtr  ipTemp[4];  /* word to ip to point to initially */


   // make sure this is pointed correctly
   user->userVariables.indexedVariables[STRING_TEMP] = (Cell)(user->padBuffer);


   // Need stack setup to push potentially
   init(user);
   if(USER_VAR(user,START_STRING))
   {
      USER_VAR(user,ECHO) = 0;
      USER_VAR(user,RECORD) = 0;
      PUSH(user,memgets);
      pushGets(user);
   }
   else
   {
      USER_VAR(user,ECHO) = USER_VAR(user,ECHOZERO);
   }
   USER_VAR(user,ECHO) = 0;

   while(1)
   {
      errorCode = setjmp(user->jumper);
      if(errorCode == 0)
      {

         init(user);
         (user->userVariables.namedVariables.ip) = ipTemp;
         USER_VAR(user,BASE) = 10;
         // Old init method
         //    errorCode = search(user,"$INTERPRET");
         //    USER_VAR(user,TICKEVAL) = (Cell)(rootNames[errorCode].code);
         push(user,(int32_t)INTERPRETSTRING);
         nameQ(user);
         if(iff(user))
         {
            USER_VAR(user,TICKEVAL) = (Cell)(pop(user));
            #ifdef M3FIX
            USER_VAR(user,TICKEVAL) = USER_VAR(user,TICKEVAL) | 1;
            #else
            USER_VAR(user,TICKEVAL) = USER_VAR(user,TICKEVAL);
            #endif
         }
         else
         {
            printf("Can't init user vars\r\n");
            while(1);
         }
         push(user,(int32_t)ARRAYSTRING);
         nameQ(user);
         if(iff(user))
         {
            uint32_t temp;
            #ifdef GLOBAL_DOS
            user->dollarArray = dollarArray = (WordPtr)(pop(user));
            #else

            temp = pop(user);
               #ifdef M3FIX
            temp |= 1;
               #endif
            user->dollarArray = (WordPtr)(temp);
            #endif
         }
         else
         {
            printf("Can't init user vars\r\n");
            while(1);
         }

         // Old init method
         //errorCode = search(user,"$COMPILE");
         //dollarCompile = rootNames[errorCode].code;
         push(user,(int32_t)COMPILESTRING);
         nameQ(user);
         if(iff(user))
         {
            uint32_t temp;
            #ifdef GLOBAL_DOS
            user->dollarCompile = dollarCompile = (WordPtr)(pop(user));
            #else

            temp = pop(user);
               #ifdef M3FIX
            temp |= 1;
               #endif
            user->dollarCompile = (WordPtr)(temp);
            #endif
         }
         else
         {
            printf("Can't init user vars\r\n");
            while(1);
         }

         #ifdef GLOBAL_DOS
         user->dollarInterpret = dollarInterpret = (WordPtr)(USER_VAR(user,TICKEVAL));
         #else
         user->dollarInterpret = (WordPtr)(USER_VAR(user,TICKEVAL));
         //            user->dollarInterpret |= 1;
         #endif
         USER_VAR(user,STATE) = INTERPRETING;

         // Leave in if the above code doesn't define a definitions list
         // USER_VAR(user,DEFINES) = 0;

         // Old init method
         //errorCode = search(user,"QUIT");
         //*(user->userVariables.namedVariables.ip) = rootNames[errorCode].code;
         //            jumpColon(user);
         push(user,(int32_t)QUITSTRING);
         nameQ(user);
         if(iff(user))
         {
            *(user->userVariables.namedVariables.ip) = (WordPtr)(pop(user));
         }
         else
         {
            printf("Can't init user vars\r\n");
            while(1);
         }


         // Do this here so it can add startup words
         // perform all startup words
         //	   externalStartup(user);
         //	   opeN(

         //            while(1);
         // Out of date, when errocode was used to load the interpreter values
         //     if(errorCode > 0)

         {

            while(1)
            {

               (*(user->userVariables.namedVariables.ip)++)(user);

            }
         }
      } /* if */
      else
      {
         // while(1);
         /* else errorcode from bad condition */
         if(errorCode < NUMBER_ERROR_STRINGS)
         {
            FPRINTF("ERROR:%02d [%s]\r\n",errorCode,errorStrings[errorCode - 1]);
         }
         else
         {
            FPRINTF("ERROR:%02d [UNKNOWN]\r\n",errorCode);
         }
      }
   }
}










