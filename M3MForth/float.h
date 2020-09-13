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
#ifndef FLOAT_H
#define FLOAT_H


#ifdef USE_FLOAT

// floating point print
void fdot(UserStatePtr user);
void fdotdot(UserStatePtr user);
void edot(UserStatePtr user);
void ffdot(UserStatePtr user);
void ffdotdot(UserStatePtr user);

void fgreater(UserStatePtr user);
void fless(UserStatePtr user);
void fequal(UserStatePtr user);
void flessThanOrEqual(UserStatePtr user);
void fgreaterThanOrEqual(UserStatePtr user);



void fplus(UserStatePtr user);
void fminus(UserStatePtr user);
void ftimes(UserStatePtr user);
void fdivide(UserStatePtr user);
void fAbs(UserStatePtr user);
void fNegate(UserStatePtr user);
void fItoF(UserStatePtr user);
void fFtoI(UserStatePtr user);
void fTrunc(UserStatePtr user);
void fRound(UserStatePtr user);
void fPi(UserStatePtr user);
void fSin(UserStatePtr user);
void fCos(UserStatePtr user);
void fTan(UserStatePtr user);
void farcSin(UserStatePtr user);
void farcCos(UserStatePtr user);
void farcTan(UserStatePtr user);
void fPower(UserStatePtr user);
void fSqrt(UserStatePtr user);
void fCeil(UserStatePtr user);
void fFloor(UserStatePtr user);
void fExp(UserStatePtr user);
void fLog(UserStatePtr user);
void fRadiansToDegrees(UserStatePtr user);
void fDegreesToRadians(UserStatePtr user);

#ifdef USE_MATRIX

void mDotProduct(UserStatePtr user);
void mDotProduct3D(UserStatePtr user);
void mCrossProduct(UserStatePtr user);
void mVectorMultByElement(UserStatePtr user);
void mNotZero(UserStatePtr user);
void mNormalize(UserStatePtr user);
void mNormalize4D(UserStatePtr user);
void mBuildMatrix(UserStatePtr user);
void mGenerateRotationMatrix(UserStatePtr user);
void mMatrixVectorMultiply(UserStatePtr user);
void mMatrixVectorMultiplyResult(UserStatePtr user);
void mScalarVectorMultiply(UserStatePtr user);
void mScalarMatrixMultiply(UserStatePtr user);
void mVectorCopy(UserStatePtr user);
void mVectorsAreEqual(UserStatePtr user);
void mMatrixCopy(UserStatePtr user);
void mMatrixTranspose(UserStatePtr user);
void mVectorClear(UserStatePtr user);
void mVectorAdd(UserStatePtr user);
void mVectorSub(UserStatePtr user);
void mComputeVectorMagnitude(UserStatePtr user);
void mVectorMagnitude(UserStatePtr user);
void mComputeVectorMagnitude2D(UserStatePtr user);
void mMatrixClear(UserStatePtr user);
void mSetIdentity(UserStatePtr user);
void mMatrixMultiply(UserStatePtr user);
void mMatrixInverse(UserStatePtr user);
void mMatrixPrint(UserStatePtr user);
void mMatrixPrintDotF(UserStatePtr user);
void mVectorPrint(UserStatePtr user);
void mVectorPrintDotF(UserStatePtr user);

#endif // use matrix
#endif // use float
#endif // include file
