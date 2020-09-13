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
#ifndef FORTH_H
#define FORTH_H

#ifndef FTYPES_H
#include "ftypes.h"
#endif



#define COMPILING    BIT1_OF_32
#define INTERPRETING BIT0_OF_32



extern NameType names[];
extern NameType rootNames[];
extern const NameType coreNames[];
extern WordPtr doColon;
extern WordPtr doVariable;
extern WordPtr doConstant;
extern WordPtr doUser;
extern WordPtr doDoes; /* maybe */

/* a bit field variable */
//int32_t state;
extern int align;
extern int headerSize;
extern WordPtr  dollarCompile;
extern WordPtr  dollarInterpret;

extern UserStatePtr user1;


#define FPRINTF(...) (*(user->userVariables.namedVariables.userPrintf))(__VA_ARGS__)
#define PUTCH(a) (*(user->userVariables.namedVariables.userPutch))(a)
// use this when the function takes a max length variable
//#define GETS(a) (*(user->userVariables.namedVariables.userGets))(a,TIB_SIZE)
#define GETS(a) (*(user->userVariables.namedVariables.userGets))(a)
#define GETSVECTOR(a) (*(user->userVariables.namedVariables.getsVector))(a)
#define GETCH() (*(user->userVariables.namedVariables.userGetch))()
#define KEYHIT() (*(user->userVariables.namedVariables.userKeyHit))()
// PUSH(user,(*(user->userVariables.namedVariables.userKeyHit))());



//#define POP  pop(user)
#define POP_INT (int)pop(user)
#define POP_BYTE (Byte)pop(user)
#define POP_WORD (Word16)pop(user)


// Another though is how to do help
// A simple way is to allocate a help string
// That comes from the code space
// That is packed behind the name 
// And is always 80 characters.
// Then a put-help word is provided for a string
// using the inline string capability s"

typedef struct WordList
{
    WordPtr            *cp;          /* code pointer */
    NameType           *names;
    int                 last;         /* and index into the names space */
    int                 numberOfNames;
    int                 sizeOfBlock;
    char               name[10];
    WordPtr            *codeSpaceStart;
    WordPtr            *codeSpaceEnd;
    NameType            namesHolder[1];   // this makes a place holder for the names
}WordList;


void jumpColon(UserStatePtr user);
void jumpVariable(UserStatePtr user);
void jumpConstant(UserStatePtr user);
void jumpUser(UserStatePtr user);
void jumpDoes(UserStatePtr user);
void dummy(UserStatePtr user);
void doLIST(UserStatePtr user);
void doLIT(UserStatePtr user);
void doVAR(UserStatePtr user);
void doCONST(UserStatePtr user);
void doUSER(UserStatePtr user);
void doDOES(UserStatePtr user);
void doDEFINE(UserStatePtr user);
void bye(UserStatePtr user);
void exiT(UserStatePtr user);






extern WordList rootWordList;
extern WordList coreWordList;
int getLast(UserStatePtr user);

void initRoot(void);
void initUser1(const char *initStartString);

void tokeN(UserStatePtr user);
void dollarCommaN(UserStatePtr user);
void constantComma(UserStatePtr user);
void comma(UserStatePtr user);

extern Byte forthQuiet;

void beQuiet(UserStatePtr user);
void beLoud(UserStatePtr user);
int substring(char *string1, char *sub);
void grep(UserStatePtr user);
void igrep(UserStatePtr user);
void words(UserStatePtr user);
void wordsInWordList(UserStatePtr user);
void init(UserStatePtr user);

SignedCell  iff(UserStatePtr user);
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
void sign(UserStatePtr user);
void poundGreater(UserStatePtr user);

void type(UserStatePtr user);
void count(UserStatePtr user);
void toByteAddress(UserStatePtr user);
void hex(UserStatePtr user);
void decimal(UserStatePtr user);
void emit(UserStatePtr user);
void toR(UserStatePtr user);
void fromR(UserStatePtr user);
void absolute(UserStatePtr user);
void sign(UserStatePtr user);

void udot(UserStatePtr user);
void dot(UserStatePtr user);
void toR(UserStatePtr user);
void fromR(UserStatePtr user);
void plusBang(UserStatePtr user);
void toIN(UserStatePtr user);
SignedCell parse(char *inbuf, Int16 length, char delim,char **token,SignedCell *tokenLength);
void PARSE(UserStatePtr user);
void emit(UserStatePtr user);

void key(UserStatePtr user);
void dumP(UserStatePtr user);
void packDollar(UserStatePtr user);
void backSlash(UserStatePtr user);
void leftParen(UserStatePtr user);
void chaR(UserStatePtr user);
void here(UserStatePtr user);
WordPtr *cellAlignedMove(WordPtr *wp,Int16 length);
void tokeN(UserStatePtr user);
void word(UserStatePtr user);
void copyAndConvert(char *destination,char *source, Int16 length);
void stringDollar(UserStatePtr user);
void _string(UserStatePtr user);
void fstring(UserStatePtr user);
void sameQ(UserStatePtr user);
void nameTo(UserStatePtr user);
void nameToText(UserStatePtr user);
void dotId(UserStatePtr user);
void find(UserStatePtr user);
void nameQ(UserStatePtr user);
void defineQ(UserStatePtr user);
void bothQ(UserStatePtr user);

void getS(UserStatePtr user);
void query(UserStatePtr user);
void dotPrompt(UserStatePtr user);
void forthVerboseMode(FCONTEXT);
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
extern WordPtr  dollarCompile;
extern WordPtr  dollarInterpret;

void leftBracket(UserStatePtr user);
void rightBracket(UserStatePtr user);
void dollarCommaN(UserStatePtr user);
void colonComma(UserStatePtr user);
void variableComma(UserStatePtr user);
void createComma(UserStatePtr user);
void constantComma(UserStatePtr user);
void variable(UserStatePtr user);
void createDefine(UserStatePtr user);
void create(UserStatePtr user);
void doesGreater(UserStatePtr user);
void constant(UserStatePtr user);
void colon(UserStatePtr user);
void compile(UserStatePtr user);
extern int release;

void bracketCompile(UserStatePtr user);
void semiColon(UserStatePtr user);
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
void doBREAK(UserStatePtr user);
void doBreak(UserStatePtr user);
void qKey(UserStatePtr user);
void agaiN(UserStatePtr user);
void begiN(UserStatePtr user);
void whilE(UserStatePtr user);
void repeaT(UserStatePtr user);
void lasT(UserStatePtr user);
NameType  *cfaSearch(UserStatePtr user,WordPtr searchFor,WordList **whichWordList,SignedCell *index);
extern Int16 rootLast;
void forget(UserStatePtr user);
void immediate(UserStatePtr user);
void doDollar(UserStatePtr user);
void dollarQuoteBar(UserStatePtr user);
void dotQuoteBar(UserStatePtr user);
void dotQuote(UserStatePtr user);
void xor(UserStatePtr user);
void slashMod(UserStatePtr user);
void mod(UserStatePtr user);
void pick(UserStatePtr user);
void twoDrop(UserStatePtr user);
void twoDup(UserStatePtr user);
void nip(UserStatePtr user);
void code(UserStatePtr user);
void endCode(UserStatePtr user);
void toName(UserStatePtr user);
void huh(UserStatePtr user);
void see(UserStatePtr user);
void iLiteral(UserStatePtr user);
void iString(UserStatePtr user);

void beQuiet(UserStatePtr user);
void beLoud(UserStatePtr user);

void dictStatus(UserStatePtr user);


void doDelay(UserStatePtr user);
void copy(UserStatePtr user);
void fill(UserStatePtr user);
void words(UserStatePtr user);
extern NameType rootNames[];
int getLast(UserStatePtr user);
extern const char  * const (baseWords[]);
void addName(WordList *wordList,char *name,Byte lex);
// the external call word
Byte addWord(UserStatePtr user,WordPtr function,char *name);
Byte addWordWithHelp(UserStatePtr user,WordPtr function,char *name,char *helpString);
Byte addWordCountedStringWithHelp(UserStatePtr user,WordPtr function,char *name,char *helpString);
Byte addWordCountedStringWithHelpCountedString(UserStatePtr user,WordPtr function,char *name,char *helpString);
Byte addWordCountedString(UserStatePtr user,WordPtr function,char *name);
Byte addDefineCountedString(UserStatePtr user,Cell value,CountedString name);
Byte addDefineCountedStringWithHelp(UserStatePtr user,Cell value,CountedString name,char *helpString);
Byte addDefineCountedStringWithHelpCountedString(UserStatePtr user,Cell value,CountedString name,char *helpString);

void addExternalWords(UserStatePtr user);
void addExternalDBWords(FCONTEXT);
void addHelpFromC(UserStatePtr user);
void dotHelp(UserStatePtr user);
void tempStringBuffer(UserStatePtr user);


void iString(UserStatePtr user);
void iLiteral(UserStatePtr user);
void makeConstant(const char * const name,const SignedCell value);
void initWords(UserStatePtr user);
typedef struct
{
    const char * const name;
    const SignedCell        value;
}UserConstants;

extern const UserConstants userConstantsInit[];
void initConstants(void);
void strUpr(char *buffer);
int search(UserStatePtr user,char *buffer);
void rpDump(UserStatePtr user);
NameType *cfaSearch(UserStatePtr user,WordPtr searchFor,WordList **whichWordList,SignedCell *whichIndex);
void display(UserStatePtr user,WordPtr *tokens,int count);
void see(UserStatePtr user);
extern ExternalStartup externalStartups[];
void addOpenString(char *string);
void externalStartup(UserStatePtr user);
extern const char * const(errorStrings[]);
void runForth(UserStatePtr user);
bool initInterpret(UserStatePtr user,WordPtr  *ipTemp);
bool interpret(UserStatePtr user,WordPtr  *ipTemp);
void dummyQuery(UserStatePtr user);


void initCoreForth(const char *initStartString);
void startOtherThreads(UserStatePtr user);
void perform(UserStatePtr user);
void cloneRootUser(UserStatePtr user,const char *initStartString, Byte userid);
void radioSilentSetup(UserStatePtr user);

void semaphoreAt(FCONTEXT);
void addSemaphoreLogic(UserStatePtr user,uint32_t semaphore,uint32_t callBack);



void trimToNewline(Byte *buffer, int32_t length);

void closeFile(UserStatePtr user);
void openFile(UserStatePtr user);
void forthFileGets(UserStatePtr user);
void plainGets(UserStatePtr user);

void pushGets(UserStatePtr user);
void popGets(UserStatePtr user);
void stackInit(UserStatePtr user);


// against my usual policy
// but just wanted to provide a single point of include
#include "words.h"
#include "asswords.h"
#include "corewords.h"


#endif
