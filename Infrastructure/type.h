/**
 *  
 * @brief   type
 * 
 * @author  Charles M Nowell, Jr.
 * 
 ***************************************************************
 ***************************************************************
 */

#ifndef TYPE_H
#define TYPE_H

///*************************************************************
///*************************************************************

#include <stdint.h>
#include <stdbool.h>

///*************************************************************
///*************************************************************

#define BIT_LOW   0
#define BIT_HIGH  1

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef False
#define False   (0)
#endif

#ifndef True            // a definition is required by chans code.
#define True    (0x01)
#endif

#ifndef TRUE
#define TRUE    (1u)
#endif

#ifndef FALSE
#define FALSE   (0u)
#endif

///*************************************************************
///*************************************************************

typedef bool devStatus_t;

//#define ERROR           (0xff)
#define ERROR_NONE      (0)
#define ERROR_ONE       (1)
#define ERROR_TWO       (2)
#define ERROR_THREE     (3)
#define ERROR_FOUR      (4)

///*************************************************************
///*************************************************************

typedef unsigned int   BOOL;

typedef unsigned char Byte;

///*************************************************************
///*************************************************************
//
// Start unsigned 8 bit types
//
#ifndef Word8
#define Word8 Byte
#endif
// #ifndef WORD8
// #define WORD8 Byte
// #endif
// #ifndef UCHAR
// #define UCHAR Byte
// #endif
// #ifndef uchar
// #define uchar Byte
// #endif
// #ifndef ubyte
// #define ubyte  Byte
// #endif
// #ifndef UBYTE
// #define UBYTE  Byte
// #endif
// #ifndef BYTE
// #define BYTE  Byte              // definition required by chans code
// #endif
// #ifndef byte
// #define byte  Byte
// #endif
//
// End of unsigned 8 bit types
//
typedef signed char Int8;   // pick something.
//
// Start of signed 8 bit types
//
#ifndef int8
#define int8 Int8
#endif
// #ifndef INT8
// #define INT8 Int8
// #endif
// #ifndef Char
// #define Char Int8
// #endif

///*************************************************************
///*************************************************************

//
// End of signed 8-bit types
//
typedef unsigned short Word16;
// 
// Start of unsigned 16 bit types
//

///*************************************************************
///*************************************************************

#ifndef word16          // no way to tell if signed or unsigned in source code
#define word16 Word16
#endif
// #ifndef WORD16          // no way to tell if signed or unsigned in source code
// #define WORD16 Word16
// #endif
// #ifndef WORD            // no way to tell if signed or unsigned in source code
// #define WORD   Word16
// #endif
// #ifndef UINT16
// #define UINT16 Word16
// #endif
// #ifndef uint16
// #define uint16 Word16
// #endif
// #ifndef USHORT
// #define USHORT Word16
// #endif

///*************************************************************
///*************************************************************

//
// End of unsigned 16 bit
//
typedef signed short Int16;
//
// Start of signed 16 bit
//

///*************************************************************
///*************************************************************

#ifndef int16
#define int16 Int16
#endif
// #ifndef INT16
// #define INT16 Int16
// #endif
// #ifndef SHORT
// #define SHORT Int16
// #endif

///*************************************************************
///*************************************************************

//
// End of signed 16 bit types
//
typedef unsigned int uint32;
//
// Start of 32 bit unsigned types
//

///*************************************************************
///*************************************************************

#if 1
#ifndef word32 
#define word32   uint32
#endif
// #ifndef WORD32
// #define WORD32   uint32
// #endif
// #ifndef uint32_t
// #define uint32_t   uint32
// #endif
// #ifndef UINT
// #define UINT     uint32
// #endif
// #ifndef uint
// #define uint     uint32
// #endif
// #ifndef UINT32
// #define UINT32   uint32
// #endif
// #ifndef ULONG
// #define ULONG    uint32
// #endif
// #ifndef ulong
// #define ulong    uint32
// #endif
#ifndef BOOL    // this was in the original type.h (seems to big for a bool)
#define BOOL     uint32
#endif
#endif
///*************************************************************
///*************************************************************

//
// End of unsigned 32-bit types
//
typedef signed int   Int32;
//
// Start of signed 32-bit types
//

///*************************************************************
///*************************************************************

// #ifndef LONG
// #define LONG     Int32
// #endif
// #ifndef int32
// #define int32 Int32
// #endif
// #ifndef DWORD
// #define DWORD    Int32
// #endif
// #ifndef WCHAR
// #define WCHAR    Word16
// #endif

///*************************************************************
///*************************************************************

typedef uint8_t  Selector;
typedef uint32_t UInt32;
typedef uint8_t  UInt8;

//
// End of signed 32-bit types

typedef float  Float32;
typedef double Float64;
typedef signed long long int   Int64;
typedef signed int Fixed32;
typedef signed long long int Fixed64;

///*************************************************************
///*************************************************************
///
/// Some function pointer typedefs below
///
///*************************************************************
///*************************************************************

typedef void*  PVOID;
typedef void* (*THREAD_FUNC_PTR)(void*);
typedef int   (*USER_CALLBACK_FUNCPTR) (void*);    
typedef void  (*VOID_FUNC_PTR) (void);
typedef int   (*INT_FUNC_PTR) (void);  
// This is a special type name to make things obvious
typedef char const * const CountedString;

typedef const char *StringConstant;
typedef  char *String;

typedef String *Strings;

typedef CountedString *CountedStrings;

typedef Byte Ordinal;

///*************************************************************
///************************************************************* 
/// 
/// This is used to get the binary representations of values to
/// send to the database. Mostly it provides a method of passing
/// a float32 as a single precision by passing the int32 binary
/// version of it. This gets around the automatic double
/// conversion that C does automatically that screws up trying
/// to pass an object as a 32 bit value all the time.
/// 
///*************************************************************
///*************************************************************

typedef union
{
    Int32    int32Value;
    Float32  float32Value;
    String   stringValue;
    Fixed32  fixed32Value;
    Fixed64  fixed64Value;
    Float64  float64Value;
    Int32    *int32Ptr;
    UInt32   *uint32Ptr;
    UInt8    *uint8Ptr;
    Float32  *float32Ptr;
    void     *voidPtr;
}Floater;

///*************************************************************
///*************************************************************
///
/// the rest of normal type.h
///
///*************************************************************
///*************************************************************
#ifndef BIT0
#define   BIT0        0x00000001
#define   BIT1        0x00000002
#define   BIT2        0x00000004
#define   BIT3        0x00000008
#define   BIT4        0x00000010
#define   BIT5        0x00000020
#define   BIT6        0x00000040
#define   BIT7        0x00000080
#define   BIT8        0x00000100
#define   BIT9        0x00000200
#define   BIT10       0x00000400
#define   BIT11       0x00000800
#define   BIT12       0x00001000
#define   BIT13       0x00002000
#define   BIT14       0x00004000
#define   BIT15       0x00008000
#define   BIT16       0x00010000
#define   BIT17       0x00020000
#define   BIT18       0x00040000
#define   BIT19       0x00080000
#define   BIT20       0x00100000
#define   BIT21       0x00200000
#define   BIT22       0x00400000
#define   BIT23       0x00800000
#define   BIT24       0x01000000
#define   BIT25       0x02000000
#define   BIT26       0x04000000
#define   BIT27       0x08000000
#define   BIT28       0x10000000
#define   BIT29       0x20000000
#define   BIT30       0x40000000
#define   BIT31       0x80000000
#endif

///*************************************************************
///*************************************************************

#define BIT0_8        0x01
#define BIT1_8        0x02
#define BIT2_8        0x04
#define BIT3_8        0x08
#define BIT4_8        0x10
#define BIT5_8        0x20
#define BIT6_8        0x40
#define BIT7_8        0x80

#define BIT8_16        0x0100
#define BIT9_16        0x0200
#define BIT10_16       0x0400
#define BIT11_16       0x0800
#define BIT12_16       0x1000
#define BIT13_16       0x2000
#define BIT14_16       0x4000
#define BIT15_16       0x8000

#define	ALSO	|
//#define	bool		Boolean

#define SET8(a,b) a|=(b)
#define CLEAR8(a,b)  a&=~(b)

#define ALL_ONES 0xFFFFFFFF
#define WORD_SIZE 32

#define MASK(width)          ((ALL_ONES >> (WORD_SIZE-(width))))
#define FIELD(width,offset)  ((ALL_ONES >> (WORD_SIZE-(width))) << (offset))
#define LIMIT(value,width)   (value & MASK(width))
#define PUT_FIELD(result,value,width,offset)  result &= ~(FIELD(width,offset))\
        ;result |= LIMIT(value,width) << (offset)
#define GET_FIELD(variable,width,offset)    (((variable)&(FIELD(width,offset))) >> offset)

#define PUT_A_FIELD(result,value,symbol) PUT_FIELD((result),value,symbol##_FIELD_SIZE,symbol##_FIELD_OFFSET)

#ifndef  NULL 
#define NULL ((void *)0)
#endif

#define TIME  ((Int32)(OSTimeGet()))

#ifndef PRINTF_TYPE
#define PRINTF_TYPE
typedef void  (*PrintfType)(char *format,...);
#endif

///*************************************************************
///*************************************************************

#endif  /* __TYPE_H__ */
