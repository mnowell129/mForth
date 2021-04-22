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
#include <stdlib.h>
#include <math.h>



#include "type.h"


#include "forthconfig.h"
#ifdef DATABASE_IN
#include "dataelements.h"
#endif

#include "ftypes.h"
#include "asswords.h"
#include "words.h"
#include "forth.h"
#include "corewords.h"
#include "float.h"

#include <stdarg.h>
#include "snprintf99.h"


#ifdef USE_FLOAT



// floating point print
void ffdot(UserStatePtr user)
{
    Floater value;
    float   fvalue;
    int before,after;
    //    float   mantissa;
    //    int     exponent;
    //    int i;
    //    Byte sign = 0;

    value.int32Value = (int32_t)UPOP();
    fvalue = value.float32Value;
    after  = UPOP();
    before = UPOP();
    //   floatToAscii(fvalue,before,after,buffer);
    //    FPRINTF("%s",buffer);
    //    sprintf(buffer,"%f",fvalue);
    FPRINTF("%*.*f",before,after,fvalue);
}//void fdot(UserStatePtr user)

void ffdotdot(UserStatePtr user)
{
    static char buffer[80];
    char        *ptr;
    Floater value;
    float   fvalue;
    uint32_t length;
    int before,after;

    value.int32Value = (int32_t)UPOP();
    fvalue = value.float32Value;
    after  = UPOP();
    before = UPOP();
    //   floatToAscii(fvalue,before,after,buffer);
    //    FPRINTF("%s",buffer);
    //    sprintf(buffer,"%f",fvalue);
    length = rpl_vsnprintf(buffer,80,"%*.*f",before,after,fvalue);
    ptr = buffer;
    while(length--)
    {
        PUTCH(*ptr++);
    }
    // FPRINTF("%*.*f",before,after,fvalue);
}//void fdot(UserStatePtr user)


// floating point print
void edot(UserStatePtr user)
{
    static char buffer[80];
    Floater value;
    float   fvalue;
    uint32_t length;
    char        *ptr;
    value.int32Value = (int32_t)UPOP();
    fvalue = value.float32Value;

    length = rpl_vsnprintf(buffer,80,"%*.*e",8,5,fvalue);
    ptr = buffer;
    while(length--)
    {
        PUTCH(*ptr++);
    }
    // FPRINTF("%*.*e",8,5,fvalue);
}//void fdot(UserStatePtr user)

// floating point print
void fdot(UserStatePtr user)
{
    static char buffer[80];
    char        *ptr;
    uint32_t length;
    Floater value;
    float   fvalue;
    value.int32Value = (int32_t)UPOP();
    fvalue = value.float32Value;
    length = rpl_vsnprintf(buffer,80," %g",fvalue);
    ptr = buffer;
    while(length--)
    {
        PUTCH(*ptr++);
    }
}//void fdot(UserStatePtr user)

// Doesn't cause a carriage return
// that goes with some interfaces's printf.
void fdotdot(UserStatePtr user)
{
    static char buffer[80];
    char        *ptr;
    uint32_t length;
    Floater value;
    float   fvalue;
    value.int32Value = (int32_t)UPOP();
    fvalue = value.float32Value;
    length = rpl_vsnprintf(buffer,80,"%g",fvalue);
    ptr = buffer;
    while(length--)
    {
        PUTCH(*ptr++);
    }
}//void fdot(UserStatePtr user)






void fgreater(UserStatePtr user)
{
    Floater value1;
    Floater value2;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    if(value1.float32Value > value2.float32Value)
    {
        push(user,-1);
    }
    else
    {
        push(user,0);
    }
}
void fless(UserStatePtr user)
{
    Floater value1;
    Floater value2;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    if(value1.float32Value < value2.float32Value)
    {
        push(user,-1);
    }
    else
    {
        push(user,0);
    }
}
void fequal(UserStatePtr user)
{
    Floater value1;
    Floater value2;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    if(value1.float32Value == value2.float32Value)
    {
        push(user,-1);
    }
    else
    {
        push(user,0);
    }
}
void flessThanOrEqual(UserStatePtr user)
{
    Floater value1;
    Floater value2;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    if(value1.float32Value <= value2.float32Value)
    {
        push(user,-1);
    }
    else
    {
        push(user,0);
    }
}
void fgreaterThanOrEqual(UserStatePtr user)
{
    Floater value1;
    Floater value2;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    if(value1.float32Value >= value2.float32Value)
    {
        push(user,-1);
    }
    else
    {
        push(user,0);
    }
}



void fplus(UserStatePtr user)
 {
    Floater value1;
    Floater value2;
    Floater value3;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    value3.float32Value = value1.float32Value + value2.float32Value;
    push(user,value3.int32Value);
 }
void fminus(UserStatePtr user)
 {
    Floater value1;
    Floater value2;
    Floater value3;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    value3.float32Value = value1.float32Value - value2.float32Value;
    push(user,value3.int32Value);
 }
void ftimes(UserStatePtr user)
 {
    Floater value1;
    Floater value2;
    Floater value3;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    value3.float32Value = value1.float32Value * value2.float32Value;
    push(user,value3.int32Value);
 }
void fdivide(UserStatePtr user)
 {
    Floater value1;
    Floater value2;
    Floater value3;
    
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    value3.float32Value = value1.float32Value / value2.float32Value;
    push(user,value3.int32Value);
 }

/*
 { fAbs,              "FABS",NULL},
 { fNegate,           "-F",NULL},
 { fItoF,             "I>F",NULL},
 { fFToI,             "F>I",NULL},
 { fTrunc,            "FTRUNC",NULL},
 { fRound,            "FROUND",NULL},
*/
void fAbs(UserStatePtr user)
 {
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = fabs(value1.float32Value);
    push(user,value1.int32Value);
 }
void fNegate(UserStatePtr user)
 {
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = -value1.float32Value;
    push(user,value1.int32Value);
 }
void fItoF(UserStatePtr user)
 {
    Floater value1;
    int32_t   value;
    value = (int32_t)UPOP();
    value1.float32Value = (float)(value);
    push(user,value1.int32Value);
 }
void fFtoI(UserStatePtr user)
 {
    Floater value1;
    int32_t   value;
    value1.int32Value = (int32_t)UPOP();
    value = (int32_t)(value1.float32Value);
    push(user,value);
 }

void fTrunc(UserStatePtr user)
 {
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    //value1.float32Value = truncf(value1.float32Value);
    push(user,value1.int32Value);
 }

void fRound(UserStatePtr user)
 {
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    //value1.float32Value = roundf(value1.float32Value);
    push(user,value1.int32Value);
 }

void fPi(UserStatePtr user)
 {
    Floater value1;
    value1.float32Value = 4.0L*atanf(1.0);
    push(user,value1.int32Value);
 }



void fSin(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = sinf(value1.float32Value);
    push(user,value1.int32Value);
}
void fCos(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = cosf(value1.float32Value);
    push(user,value1.int32Value);
}
void fTan(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = tanf(value1.float32Value);
    push(user,value1.int32Value);
}
void farcSin(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = asinf(value1.float32Value);
    push(user,value1.int32Value);
}
void farcCos(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = acosf(value1.float32Value);
    push(user,value1.int32Value);
}
void farcTan(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = atanf(value1.float32Value);
    push(user,value1.int32Value);
}


void fPower(UserStatePtr user)
{
    Floater value1;
    Floater value2;
    Floater value3;
    value2.int32Value = (int32_t)UPOP();
    value1.int32Value = (int32_t)UPOP();
    value3.float32Value = powf(value1.float32Value,value2.float32Value);
    push(user,value3.int32Value);
}


void fSqrt(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = sqrtf(value1.float32Value);
    push(user,value1.int32Value);
}

void fCeil(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = ceilf(value1.float32Value);
    push(user,value1.int32Value);
}
void fFloor(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = floorf(value1.float32Value);
    push(user,value1.int32Value);
}

void fExp(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = expf(value1.float32Value);
    push(user,value1.int32Value);
}

void fLog(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = logf(value1.float32Value);
    push(user,value1.int32Value);
}


void fRadiansToDegrees(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = (value1.float32Value * 180.0) / (4.0 *atanf(1.0));
    push(user,value1.int32Value);
}

void fDegreesToRadians(UserStatePtr user)
{
    Floater value1;
    value1.int32Value = (int32_t)UPOP();
    value1.float32Value = (value1.float32Value * (4.0 *atanf(1.0)))/(180.0);
    push(user,value1.int32Value);
}


#ifdef USE_MATRIX
#include "matrixmath.h"

typedef Float32 (*MatrixPtr)[3];
typedef Float32 (*VectorPtr);
#define VPOPV() ((VectorPtr)UPOP())
#define VPOP(v) v = (VectorPtr)UPOP();
#define MPOP(m) m = (MatrixPtr)UPOP();
#define MPOPV() ((MatrixPtr)UPOP())
#define FPOP(v) v = (Float32)UPOP();

void mDotProduct(UserStatePtr user)
{
    // void DotProduct(VectorFloatType left, VectorFloatType right, float *DOTP);
    VectorPtr left; Float32*right; VectorPtr DOTP;
    
    VPOP(DOTP);
    VPOP(right);
    VPOP(left);
    DotProduct(left,right,DOTP);
}


void mDotProduct3D(UserStatePtr user)
{
    // void dotProduct3D(MatrixFloatType left, VectorFloatType right,
    //                  VectorFloatType result);
    MatrixPtr left;VectorPtr right; VectorPtr result;
    VPOP(result);
    VPOP(right);
    MPOP(left);
    dotProduct3D(left,right,result);
}


void mCrossProduct(UserStatePtr user)
{
    // void CrossProduct(VectorFloatType left, VectorFloatType right, VectorFloatType result);
    VectorPtr left; VectorPtr right; VectorPtr result;
    VPOP(result);
    VPOP(right);
    VPOP(left);
    CrossProduct(left,right,result);
}



void mVectorMultByElement(UserStatePtr user)
{
    // void FVectorMultByElement(VectorFloatType left, VectorFloatType right, VectorFloatType result);
    VectorPtr left; VectorPtr right; VectorPtr result;
    VPOP(result);
    VPOP(right);
    VPOP(left);
    FVectorMultByElement(left,right,result);
}

void mNotZero(UserStatePtr user)
{
    // Byte notZero(Float32 value);
    UPUSH(notZero((Float32)UPOP()));
}

void mNormalize(UserStatePtr user)
{
    // void Normalize(VectorFloatType vector1);
    VectorPtr vector1;
    VPOP(vector1);
    Normalize(vector1);
}
void mNormalize4D(UserStatePtr user)
{
    // void Normalize4D(Vector4DFloatType vector4d, Vector4DFloatType vector4dNormed);
    VectorPtr vector4d,vector4dNormed;
    VPOP(vector4dNormed);
    VPOP(vector4d);
    Normalize4D(vector4d,vector4dNormed);
}
void mBuildMatrix(UserStatePtr user)
{
    // void FBuildMatrix(VectorFloatType row1,
    //                  VectorFloatType row2,
    //                  VectorFloatType row3,
    //                  MatrixFloatType resultMatrix);
    VectorPtr row1,row2,row3;
    MatrixPtr resultMatrix;
    MPOP(resultMatrix);
    VPOP(row3);
    VPOP(row2);
    VPOP(row1);
    FBuildMatrix(row1,row2,row3,resultMatrix);
    
}
void mGenerateRotationMatrix(UserStatePtr user)
{
    // void GenerateRotationMatrix(MatrixFloatType R1, VectorFloatType vector1, Float32 theta);
    MatrixPtr R1;
    VectorPtr vector1;
    Float32 theta;
    FPOP(theta);
    VPOP(vector1);
    MPOP(R1);
    GenerateRotationMatrix(R1,vector1,theta);
    
}
void mMatrixVectorMultiply(UserStatePtr user)
{
    // void MatrixVectorMultiply(MatrixFloatType R1, VectorFloatType vector1);
    MatrixPtr R1;
    VectorPtr vector1;
    VPOP(vector1);
    MPOP(R1);
    MatrixVectorMultiply(R1,vector1);
}

void mMatrixVectorMultiplyResult(UserStatePtr user)
{
    // void MatrixVectorMultiplyResult(MatrixFloatType left,
    //                                 VectorFloatType right,
    //                                 VectorFloatType result);
    MatrixPtr left;VectorPtr right; VectorPtr result;
    VPOP(result);
    VPOP(right);
    MPOP(left);
    MatrixVectorMultiplyResult(left,right,result);
}
void mScalarVectorMultiply(UserStatePtr user)
{
    // void ScalarVectorMultiply(Float32 left,
    //                           VectorFloatType right,
    //                           VectorFloatType result);
    Float32 left;VectorPtr right; VectorPtr result;
    VPOP(result);
    VPOP(right);
    FPOP(left);
    ScalarVectorMultiply(left,right,result);
}
void mScalarMatrixMultiply(UserStatePtr user)
{
    // void ScalarMatrixMultiply(Float32 left,
    //                          MatrixFloatType right,
    //                          MatrixFloatType result);
    Float32 left; MatrixPtr right; MatrixPtr result;
    MPOP(result);
    MPOP(right);
    FPOP(left);
    ScalarMatrixMultiply(left,right,result);
}

void mVectorCopy(UserStatePtr user)
{
    // void FVectorCopy(VectorFloatType source,VectorFloatType destination);
    VectorPtr source,destination;
    VPOP(destination);
    VPOP(source);
    FVectorCopy(source,destination);
}
void mVectorsAreEqual(UserStatePtr user)
{
    // Byte FVectorsAreEqual(VectorFloatType vector1, VectorFloatType vector2);
    VectorPtr vector1,vector2;
    VPOP(vector2);
    VPOP(vector1);
    UPUSH(FVectorsAreEqual(vector1,vector2));
    
}
void mMatrixCopy(UserStatePtr user)
{
    // void FMatrixCopy(MatrixFloatType source,MatrixFloatType destination);
    MatrixPtr source,destination;
    MPOP(destination);
    MPOP(source);
    FMatrixCopy(source,destination);
}
void mMatrixTranspose(UserStatePtr user)
{
    // void FMatrixTranspose(MatrixFloatType source,MatrixFloatType destination);
    MatrixPtr source,destination;
    MPOP(destination);
    MPOP(source);
    FMatrixTranspose(source,destination);
}
void mVectorClear(UserStatePtr user)
{
    // void FVectorClear(VectorFloatType inoutVector);
    VectorPtr inoutVector;
    VPOP(inoutVector);
    FVectorClear(inoutVector);
}
void mVectorAdd(UserStatePtr user)
{
    // void FVectorAdd(VectorFloatType left,VectorFloatType right, VectorFloatType result);
    VectorPtr left;VectorPtr right; VectorPtr result;
    VPOP(result);
    VPOP(right);
    VPOP(left);
    FVectorAdd(left,right,result);
}
void mVectorSub(UserStatePtr user)
{
    // void FVectorSub(VectorFloatType left,VectorFloatType right, VectorFloatType result);
    VectorPtr left;VectorPtr right; VectorPtr result;
    VPOP(result);
    VPOP(right);
    VPOP(left);
    FVectorSub(left,right,result);
}
void mComputeVectorMagnitude(UserStatePtr user)
{
    // Float32 computeVectorMagnitude(VectorFloatType vector1);
    Floater result;
    VectorPtr vector1;
    VPOP(vector1);
    result.float32Value = computeVectorMagnitude(vector1);
    UPUSH(result.int32Value);
}
void mVectorMagnitude(UserStatePtr user)
{
    // void VectorMagnitude(VectorFloatType vector1);
    VectorPtr vector1;
    VPOP(vector1);
    VectorMagnitude(vector1);
}
void mComputeVectorMagnitude2D(UserStatePtr user)
{
    // Float32 VectorMagnitude2D(Float32 xValue, Float32 yValue);
    Floater result;
    Floater xValue,yValue;
    yValue.int32Value = (int32_t)UPOP();
    xValue.int32Value = (int32_t)UPOP();
    result.float32Value = VectorMagnitude2D(xValue.float32Value,yValue.float32Value);;
    UPUSH(result.int32Value);
}

void mMatrixClear(UserStatePtr user)
{
    // void matrixClear(MatrixFloatType matrix);
    MatrixPtr matrix;
    MPOP(matrix);
    matrixClear(matrix);
}
void mSetIdentity(UserStatePtr user)
{
    // void matrixSetIdentity(MatrixFloatType matrix);
    MatrixPtr matrix;
    MPOP(matrix);
    matrixSetIdentity(matrix);
}

void mMatrixMultiply(UserStatePtr user)
{
    // void MatrixMultiply(MatrixFloatType left,MatrixFloatType right,MatrixFloatType result);
    MatrixPtr left,right,result;
    MPOP(result);
    MPOP(right);
    MPOP(left);
    MatrixMultiply(left,right,result);
}

void mMatrixInverse(UserStatePtr user)
{
    // void MatrixInverse(MatrixFloatType matrix1, MatrixFloatType matrix2)
    MatrixPtr matrix1,matrix2;
    MPOP(matrix2);
    MPOP(matrix1);
    MatrixInverse(matrix1,matrix2);
}

void mVectorPrint(UserStatePtr user)
{
    VectorPtr vector;
    int i;
    VPOP(vector);
    for(i=0;i<VECTOR_FLOAT_TYPE_SIZE;i++)
    {
        FPRINTF("%f%s",vector[i],i==VECTOR_FLOAT_TYPE_SIZE ? "\r\n" : ",");
    }
}

void mVectorPrintDotF(UserStatePtr user)
{
    VectorPtr vector;
    int i;
    int width,after;

    VPOP(vector);
    after = UPOP();
    width = UPOP();
    for(i=0;i<VECTOR_FLOAT_TYPE_SIZE;i++)
    {
        FPRINTF("%*.*f%s",width,after,vector[i],i==VECTOR_FLOAT_TYPE_SIZE ? "\r\n" : ",");
    }
}


void mMatrixPrint(UserStatePtr user)
{
    MatrixPtr matrix;
    int i,j;
    MPOP(matrix);
    for(i=0;i<MATRIX_FLOAT_TYPE_ROW_SIZE;i++)
    {
        for(j=0;j<MATRIX_FLOAT_TYPE_COL_SIZE;j++)
        {
            FPRINTF("%f%s",matrix[i][j],j==MATRIX_FLOAT_TYPE_COL_SIZE-1 ? "\r\n" : ",");
        }
    }
}

void mMatrixPrintDotF(UserStatePtr user)
{
    MatrixPtr matrix;
    int i,j;
    int width,after;
    MPOP(matrix);
    after = UPOP();
    width = UPOP();
    for(i=0;i<MATRIX_FLOAT_TYPE_ROW_SIZE;i++)
    {
        for(j=0;j<MATRIX_FLOAT_TYPE_COL_SIZE;j++)
        {
            FPRINTF("%*.*f%s",width,after,matrix[i][j],j==MATRIX_FLOAT_TYPE_COL_SIZE-1 ? "\r\n" : ",");
        }
    }
}


#endif // if include matrices


#endif // if include float

