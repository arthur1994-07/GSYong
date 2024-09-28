#include "stdafx.h"
#include <math.h>
#include "SSE2.h"

namespace SSE2
{
	const __m128 xmmX(const __m128& _x0)
	{
		return ::_mm_shuffle_ps(_x0, _x0, _MM_SHUFFLE(0, 0, 0, 0));
	}
	const __m128 xmmY(const __m128& _x0)
	{
		return ::_mm_shuffle_ps(_x0, _x0, _MM_SHUFFLE(1, 1, 1, 1));
	}
	const __m128 xmmZ(const __m128& _x0)
	{
		return ::_mm_shuffle_ps(_x0, _x0, _MM_SHUFFLE(2, 2, 2, 2));
	}
	const __m128 xmmW(const __m128& _x0)
	{
		return ::_mm_shuffle_ps(_x0, _x0, _MM_SHUFFLE(3, 3, 3, 3));
	}
	const __m128 xmm(const float _f0, const float _f1, const float _f2, const float _f3)
	{
		return ::_mm_set_ps(_f3, _f2, _f1, _f0);
	}
	const __m128 xmm(const float _f0, const float _f1, const float _f2)
	{
		return ::_mm_set_ps(1.0f, _f2, _f1, _f0);
	}
	const __m128 xmm(const float _f0)
	{
		return ::_mm_set1_ps(_f0);
	}

	const float normalize(__m128& _x0)
	{
		const __m128& _xVector = _x0;
		const __m128 _xSquare = _xVector * _xVector;		
		const __m128 _xLengthVector = 
			SSE2::sqrt(SSE2::xmmX(_xSquare) + SSE2::xmmY(_xSquare) + SSE2::xmmZ(_xSquare));	

		const float _fLength = _xLengthVector.m128_f32[0];
		_x0 = _fLength < 0.00001f ? _x0 * SSE2::xmm(0.0f, 0.0f, 0.0f) : _x0 / _xLengthVector;

		return _fLength < 0.00001f ? 0.0f : _fLength;
	}

	const __m128 normalizeElement(const __m128 _x0)
	{ // 각 원소 -> 양수/음수 정규화;
		return ((_x0 < 0.0f) & -2.0f) + 1.0f;
	}

	const __m128 normal(const __m128& _x0)
	{
		const __m128 _xSquare = _x0 * _x0;		
		const __m128 _xLengthVector = 
			SSE2::sqrt(SSE2::xmmX(_xSquare) + SSE2::xmmY(_xSquare) + SSE2::xmmZ(_xSquare));	

		return _xLengthVector.m128_f32[0] < 0.00001f ? _x0 * SSE2::xmm(0.0f, 0.0f, 0.0f) : _x0 / _xLengthVector;
	}

	const __m128 abs(const __m128& _x0)
	{
		const __m128& _xNegative = _x0 < 0.0f;
		const __m128& _xPositive = _x0 > 0.0f;
		return (_x0 & _xPositive) - (_x0 & _xNegative);
	}	

	const __m128 dotProduct(const __m128& _x0)
	{
		return SSE2::xmmX(_x0) + SSE2::xmmY(_x0) + SSE2::xmmZ(_x0);
	}
	const __m128 sqrt(const __m128& _x0)
	{
		return ::_mm_sqrt_ps(_x0);
	}
	const __m128 length(const __m128& _x0)
	{
		const __m128& _xVectorSquare = _x0 * _x0;
		return SSE2::sqrt(SSE2::xmmX(_xVectorSquare) + SSE2::xmmY(_xVectorSquare) + SSE2::xmmZ(_xVectorSquare));
	}
	const __m128 lengthSquare(const __m128& _x0)
	{
		const __m128& _xVectorSquare = _x0 * _x0;
		return SSE2::xmmX(_xVectorSquare) + SSE2::xmmY(_xVectorSquare) + SSE2::xmmZ(_xVectorSquare);
	}	

	const __m128 limitOver(__m128& _x0, const __m128& _xOver)
	{
		const __m128& _xConstValue = _x0;
		const __m128& _xGap = SSE2::abs(_xConstValue) - SSE2::abs(_xOver);

		const __m128& _xOverFlags = _xGap > 0.0f;
		_x0 = ::_mm_andnot_ps(_xOverFlags, _xConstValue) + (_xOverFlags & _xOver);

		return _xOverFlags;
	}
	const __m128 limitUnder(__m128& _x0, const __m128& _xUnder)
	{
		const __m128& _xConstValue = _x0;
		const __m128& _xGap = SSE2::abs(_xConstValue) - SSE2::abs(_xUnder);

		const __m128& _xUnderFlags = _xGap < 0.0f;
		_x0 = ::_mm_andnot_ps(_xUnderFlags, _xConstValue) + (_xUnderFlags & _xUnder);

		return _xUnderFlags;
	}
	const __m128 limit(__m128& _x0, const __m128& _xMinimum, const __m128& _xMaximum)
	{
		const __m128& _xConstValue = _x0;
		const __m128& _xUnder = _x0 < _xMinimum;
		const __m128& _xOver = _x0 > _xMaximum;

		const __m128& _x1 = ::_mm_andnot_ps((_xOver + _xUnder), _xConstValue);
		const __m128& _x2 = _xUnder & _xMinimum;
		const __m128& _x3 = _xOver & _xMaximum;
		_x0 = _x1 + _x2 + _x3;
		return _xUnder + _xOver;
	}

	const bool isZero(const __m128& _x0)
	{
		if ( _x0.m128_u64[0] != 0 )
			return false;

		if ( _x0.m128_u64[1] != 0 )
			return false;

		return true;
	}

	const __m128& rotateEulerAxisY(__m128& xTargetPosition, const __m128& xReferencePosition, const float fAngle)
	{
		const float _fCosSin[2] = {::cos(fAngle), ::sin(fAngle)};		
		const __m128& _xSourcePosition = xTargetPosition - xReferencePosition;
		const __m128& _xCosSin = SSE2::xmm(_fCosSin[0], -_fCosSin[1], _fCosSin[1], _fCosSin[0]);

		// ( x * cos, z * sin, x * -sin, z * cos )
		const __m128& _xArray1 = xmmShuffle1(_xSourcePosition, 0, 2, 0, 2) * _xCosSin;		

		// ( x * cos, x * -sin, y, 1 )
		//					+
		// ( z * sin,  z * cos, 0,  0 )
		const __m128& _xArray2 = 
			xmmShuffle2(_xArray1, _xSourcePosition, 0, 2, 1, 3) + 
			xmmShuffle2(_xArray1, SSE2::xmm(0.0f), 1, 3, 0, 0);

		// y z 스위칭 + 원래 좌표;
		return (xTargetPosition += (xReferencePosition + xmmShuffle1(_xArray2, 0, 2, 1, 3)));
	}
	const __m128_4& identityMatrix(__m128_4& xTargetMatrix)
	{
		xTargetMatrix[0] = SSE2::xmm(1.0f, 0.0f, 0.0f, 0.0f);
		xTargetMatrix[1] = SSE2::xmm(0.0f, 1.0f, 0.0f, 0.0f);
		xTargetMatrix[2] = SSE2::xmm(0.0f, 0.0f, 1.0f, 0.0f);
		xTargetMatrix[3] = SSE2::xmm(0.0f, 0.0f, 0.0f, 1.0f);

		return xTargetMatrix;
	}

	const __m128_4 identityMatrix(void)
	{
		return __m128_4(
			SSE2::xmm(1.0f, 0.0f, 0.0f, 0.0f),
			SSE2::xmm(0.0f, 1.0f, 0.0f, 0.0f),
			SSE2::xmm(0.0f, 0.0f, 1.0f, 0.0f),
			SSE2::xmm(0.0f, 0.0f, 0.0f, 1.0f));
	}

	const __m128_4 inverseWorldMatrix(const __m128_4& xSourceMatrix)
	{
		const __m128& _x0 = xmmShuffle1(xSourceMatrix[1], 3, 0, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 0, 3, 0, 3);
		const __m128& _x1 = xmmShuffle1(xSourceMatrix[1], 3, 1, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 1, 3, 1, 3);
		const __m128& _x2 = xmmShuffle1(xSourceMatrix[1], 3, 2, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 2, 3, 2, 3);

		const __m128& _xPosition = xSourceMatrix[3];		
		const __m128& _x3 = SSE2::xmm(0.0f, 0.0f, 0.0f, 1.0f) - (_x0 * SSE2::xmmX(_xPosition) + _x1 * SSE2::xmmY(_xPosition) + _x2 * SSE2::xmmZ(_xPosition));


		return __m128_4(_x0, _x1, _x2, _x3);
	}

	const __m128_4& inverseWorldMatrix(__m128_4& xTargetMatrix, const __m128_4& xSourceMatrix)
	{
		const __m128& _x0 = xmmShuffle1(xSourceMatrix[1], 3, 0, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 0, 3, 0, 3);
		const __m128& _x1 = xmmShuffle1(xSourceMatrix[1], 3, 1, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 1, 3, 1, 3);
		const __m128& _x2 = xmmShuffle1(xSourceMatrix[1], 3, 2, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 2, 3, 2, 3);

		const __m128& _xPosition = xSourceMatrix[3];		
		const __m128& _x3 = SSE2::xmm(0.0f, 0.0f, 0.0f, 1.0f) - (_x0 * SSE2::xmmX(_xPosition) + _x1 * SSE2::xmmY(_xPosition) + _x2 * SSE2::xmmZ(_xPosition));

		xTargetMatrix.xMatrix[0] = _x0;
		xTargetMatrix.xMatrix[1] = _x1;
		xTargetMatrix.xMatrix[2] = _x2;
		xTargetMatrix.xMatrix[3] = _x3;

		return xTargetMatrix;
	}

	const __m128_4 inverseViewMatrix(const __m128_4& xSourceMatrix, const __m128& xPosition)
	{
		return __m128_4(
			xmmShuffle1(xSourceMatrix[1], 3, 0, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 0, 3, 0, 3),
			xmmShuffle1(xSourceMatrix[1], 3, 1, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 1, 3, 1, 3),
			xmmShuffle1(xSourceMatrix[1], 3, 2, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 2, 3, 2, 3),
			xPosition	);
	}

	const __m128_4& inverseViewMatrix(__m128_4& xTargetMatrix, const __m128_4& xSourceMatrix, const __m128& xPosition)
	{
		xTargetMatrix.xMatrix[0] = xmmShuffle1(xSourceMatrix[1], 3, 0, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 0, 3, 0, 3);
		xTargetMatrix.xMatrix[1] = xmmShuffle1(xSourceMatrix[1], 3, 1, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 1, 3, 1, 3);
		xTargetMatrix.xMatrix[2] = xmmShuffle1(xSourceMatrix[1], 3, 2, 3, 3) + xmmShuffle2(xSourceMatrix[0], xSourceMatrix[2], 2, 3, 2, 3);
		xTargetMatrix.xMatrix[3] = xPosition;

		return xTargetMatrix;		
	}

	const __m128_4 rotateAxisZ(const __m128_4& xSourceMatrix, const float fAngle)
	{
		const float _fSin = ::sin(fAngle);
		const float _fCos = ::cos(fAngle);

		const __m128 _xMatrix[3] = {
			SSE2::xmm(_fCos, _fSin, 0.0f, 0.0f),
			SSE2::xmm(-_fSin, _fCos, 0.0f, 0.0f),
			SSE2::xmm(0.0f, 0.0f, 1.0f, 0.0f)
		};

		const __m128& _xMatrix0 = xSourceMatrix[0];
		const __m128 _x0 = 
			SSE2::xmmX(_xMatrix0) * _xMatrix[0] +
			SSE2::xmmY(_xMatrix0) * _xMatrix[1] + 
			SSE2::xmmZ(_xMatrix0) * _xMatrix[2];

		const __m128& _xMatrix1 = xSourceMatrix[1];
		const __m128 _x1 =
			SSE2::xmmX(_xMatrix1) * _xMatrix[0] +
			SSE2::xmmY(_xMatrix1) * _xMatrix[1] + 
			SSE2::xmmZ(_xMatrix1) * _xMatrix[2];

		const __m128& _xMatrix2 = xSourceMatrix[2];
		const __m128& _x2 = 
			SSE2::xmmX(_xMatrix2) * _xMatrix[0] +
			SSE2::xmmY(_xMatrix2) * _xMatrix[1] + 
			SSE2::xmmZ(_xMatrix2) * _xMatrix[2];

		return __m128_4(_x0, _x1, _x2, xSourceMatrix[3]);		
	}
	const __m128_4 quarternionMatrix(const __m128& xAxis, const float fAngle)
	{
		// 0: cos, 1: -Sin, 2: Sin, 3: 0.0f;
		const float _fSin = ::sin(fAngle);
		const __m128& _xAxis = xAxis + SSE2::xmm(0.0f, 0.0f, 0.0f);
		const __m128& _xCosSin = SSE2::xmm(::cos(fAngle), -_fSin, _fSin, 0.0f);
		const __m128& _xUCos = SSE2::xmm(1.0f, 1.0f, 1.0f, 0.0f)  - xmmShuffle1(_xCosSin, 0, 0, 0, 3); 		

		return __m128_4(
			_xUCos * xmmShuffle1(_xAxis, 0, 0, 0, 3) * _xAxis + xmmShuffle1(_xAxis, 3, 2, 1, 3) * _xCosSin,
			_xUCos * xmmShuffle1(_xAxis, 0, 1, 1, 3) * xmmShuffle1(_xAxis, 1, 1, 2, 3) + xmmShuffle1(_xAxis, 2, 3, 0, 3) * xmmShuffle1(_xCosSin, 2, 0, 1, 3),
			_xUCos * _xAxis * xmmShuffle1(_xAxis, 2, 2, 2, 3) + xmmShuffle1(_xAxis, 1, 0, 3, 3) * xmmShuffle1(_xCosSin, 1, 2, 0, 3),
			SSE2::xmm(0.0f, 0.0f, 0.0f));
	}

	const __m128_4& perspectiveProjectionMatrix(__m128_4& xTargetMatrix, const float fFOV, const float fAspect, const float fNearZ, const float fFarZ)
	{
		const float _fHeight = 1.0f / ::tanf(fFOV * 0.5f);
		const float _fWidth = _fHeight / fAspect;
		const float _fDetermine = 1.0f / (fFarZ - fNearZ);

		xTargetMatrix[0] = SSE2::xmm(_fWidth, 0.0f, 0.0f, 0.0f);
		xTargetMatrix[1] = SSE2::xmm(0.0f, _fHeight, 0.0f, 0.0f);
		xTargetMatrix[2] = SSE2::xmm(0.0f, 0.0f, fFarZ * _fDetermine, 1.0f);
		xTargetMatrix[3] = SSE2::xmm(0.0f, 0.0f, -((fNearZ * fFarZ) * _fDetermine), 0.0f);

		return xTargetMatrix;		
	}
	const __m128_4& orthoProjectionMatrix(__m128_4& xTargetMatrix, const float fWidth, const float fHeght, const float fNearZ, const float fFarZ)
	{
		const float _fDetermine = 1.0f / (fFarZ - fNearZ);
		xTargetMatrix[0] = SSE2::xmm(2.0f / fWidth, 0.0f, 0.0f, 0.0f);
		xTargetMatrix[1] = SSE2::xmm(0.0f, 2.0f / fHeght, 0.0f, 0.0f);
		xTargetMatrix[2] = SSE2::xmm(0.0f, 0.0f, 1.0f * _fDetermine, 0.0f);
		xTargetMatrix[3] = SSE2::xmm(0.0f, 0.0f, -(fNearZ * _fDetermine), 1.0f);

		return xTargetMatrix;
	}
}

const __m128& operator +=(__m128& _x0, const __m128& _x1)
{
	return _x0 = ::_mm_add_ps(_x0, _x1);
}
const __m128& operator -=(__m128& _x0, const __m128& _x1)
{
	return _x0 = ::_mm_sub_ps(_x0, _x1);
}
const __m128& operator *=(__m128& _x0, const __m128& _x1)
{
	return _x0 = ::_mm_mul_ps(_x0, _x1);
}
const __m128& operator *=(__m128& _x0, const float _f1)
{
	return _x0 = ::_mm_mul_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128& operator /=(__m128& _x0, const __m128& _x1)
{
	return _x0 = ::_mm_mul_ps(_x0, ::_mm_rcp_ps(_x1));
}
const __m128& operator /=(__m128& _x0, const float _f1)
{
	return _x0 = ::_mm_mul_ps(_x0, ::_mm_rcp_ps(::_mm_set1_ps(_f1)));
}

const __m128 operator +(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_add_ps(_x0, _x1);
}
const __m128 operator +(const __m128& _x0, const float _f1)
{
	return ::_mm_add_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator -(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_sub_ps(_x0, _x1);
}
const __m128 operator -(const __m128& _x0)
{
	return ::_mm_mul_ps(_x0, ::_mm_set_ps(1.0f, -1.0f, -1.0f, -1.0f));
}
const __m128 operator *(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_mul_ps(_x0, _x1);
}
const __m128 operator *(const __m128& _x0, const __m128_4& _xMatrix)
{

	const __m128 _0 = ::_mm_mul_ps(_xMatrix[0], SSE2::xmmX(_x0));
	const __m128 _1 = ::_mm_mul_ps(_xMatrix[1], SSE2::xmmY(_x0));
	const __m128 _2 = ::_mm_mul_ps(_xMatrix[2], SSE2::xmmZ(_x0));
	const __m128 _3 = ::_mm_mul_ps(_xMatrix[3], SSE2::xmmW(_x0));

	return ::_mm_add_ps(::_mm_add_ps(_0, _1), ::_mm_add_ps(_2, _3));
}
const __m128 operator *(const __m128& _x0, const float _f1)
{
	return ::_mm_mul_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator /(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_mul_ps(_x0, ::_mm_rcp_ps(_x1));
}
const __m128 operator /(const __m128& _x0, const float _f1)
{
	return ::_mm_mul_ps(_x0, ::_mm_rcp_ps(::_mm_set1_ps(_f1)));
}
const __m128 operator /(const float _f0, const __m128& _x1)
{
	return ::_mm_mul_ps(::_mm_set1_ps(_f0), ::_mm_rcp_ps(_x1));
}
const __m128 operator ^(const __m128& _x0, const __m128& _x1)
{
	// y1z2 - y2z1, x1z2 - x2z1, x1y2 - x2y1;
	return ::_mm_sub_ps(::_mm_mul_ps(
		::_mm_shuffle_ps(_x0, _x0, _MM_SHUFFLE(3, 0, 2, 1)),
		::_mm_shuffle_ps(_x1, _x1, _MM_SHUFFLE(3, 1, 0, 2))),
		::_mm_mul_ps(
		::_mm_shuffle_ps(_x1, _x1, _MM_SHUFFLE(3, 0, 2, 1)),
		::_mm_shuffle_ps(_x0, _x0, _MM_SHUFFLE(3, 1, 0, 2))));
}

const __m128 operator ==(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_cmpeq_ps(_x0, _x1);
}
const __m128 operator ==(const __m128& _x0, const float _f1)
{
	return ::_mm_cmpeq_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator !=(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_cmpneq_ps(_x0, _x1);
}
const __m128 operator !=(const __m128& _x0, const float _f1)
{
	return ::_mm_cmpneq_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator <=(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_cmple_ps(_x0, _x1);
}
const __m128 operator <=(const __m128& _x0, const float _f1)
{
	return ::_mm_cmple_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator >=(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_cmpge_ps(_x0, _x1);
}
const __m128 operator >=(const __m128& _x0, const float _f1)
{
	return ::_mm_cmpge_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator <(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_cmplt_ps(_x0, _x1);
}
const __m128 operator <(const __m128& _x0, const float _f1)
{
	return ::_mm_cmplt_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator >(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_cmpgt_ps(_x0, _x1);
}
const __m128 operator >(const __m128& _x0, const float _f1)
{
	return ::_mm_cmpgt_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator &(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_and_ps(_x0, _x1);
}
const __m128 operator &(const __m128& _x0, const float _f1)
{
	return ::_mm_and_ps(_x0, ::_mm_set1_ps(_f1));
}
const __m128 operator |(const __m128& _x0, const __m128& _x1)
{
	return ::_mm_or_ps(_x0, _x1);
}

const __m128& operator *=(__m128& _x0, const __m128_4& _xMatrix)
{
	const __m128& _xVector = _x0;
	const __m128& _0 = _xMatrix[0] * SSE2::xmmX(_xVector);
	const __m128& _1 = _xMatrix[1] * SSE2::xmmY(_xVector);
	const __m128& _2 = _xMatrix[2] * SSE2::xmmZ(_xVector);
	const __m128& _3 = _xMatrix[3] * SSE2::xmmW(_xVector);

	return _x0 = _0 + _1 + _2 + _3;
}


const __m128i operator ==(const __m128i& _x0, const __m128i& _x1)
{
	return ::_mm_cmpeq_epi32(_x0, _x1);
}

const __m128i operator <(const __m128i& _x0, const __m128i& _x1)
{
	return ::_mm_cmplt_epi32(_x0, _x1);
}

extern void getCPUInformation(SSE2::CPUInformation& CPUInfo)
{	
	bool bSSE = false;
	bool bSSE2 = false;
	bool b3DNOW = false;
	bool bMMX = false;

	bool bExt = false;
	bool bMMX_Ext = false;
	bool b3DNOW_Ext = false;

	bool bIntelCPU = false;

	unsigned int nCore = 1;

	char* cVendorName = CPUInfo.cVendorName;

	__asm
	{
		mov	eax, 0x0; // 생산자 이름 코드;
		cpuid;
		mov	esi, cVendorName; // 포인터 연결;

		mov	[esi], ebx;			// 처음 4바이트;
		mov	[esi + 4], edx;		// 두번째 4바이트;
		mov	[esi + 8], ecx;		// 세번째 4바이트;


		mov	eax, 0x1;			// 지원 기능 코드;
		cpuid;
		test	edx, 04000000h;	// SSE2를 지원하는가?;
		jz		__NOSSE2; // 지원하지 않는다면 __NOSSE2로 넘어간다;
		mov	bSSE2, 0x1; // 지원하면 sse2_값을 1로 변경한다;

__NOSSE2:
		test	edx, 02000000h; // SSE를 지원하는가?;
		jz		__NOSSE; // 지원하지 않는다면 __NOSSE로 넘어간다;
		mov	bSSE, 0x1; // 지원하면 sse_값을 1로 변경한다;

__NOSSE:
		test	edx, 00800000h; // MMX를 지원하는가?;
		jz		__NOMMX; // 지원하지 않느다면 __NOMMX로 넘어간다;
		mov	bMMX, 0x1; // 지원하면 mmx_값을 1로 변경한다;
__NOMMX:


		mov	eax, 80000000h; // 확장 함수 정보를 얻어온다;
		cpuid;
		cmp	eax, 80000000h; // eax값과 80000000h값의 차이를 구한다;
		jbe	__NOEXT; // 작거나 같으면 __NOEXT로 넘어간다;
		mov	bExt, 0x1; // 아니라면 ext_를 1로 변경한다;
		mov	eax, 80000001h; // 확장 프로세서 정보를 얻어온다;
		cpuid;
		test	edx, 80000000h; // 지원 정보가 있는가?;
		jz		__NOEXT; // 없다면 __NOEXT로 넘어간다;
		mov	b3DNOW, 0x1; // 있다면 threeNow_를 1로 변경한다;
__NOEXT:
	}

	bIntelCPU = ::strncmp(cVendorName, "AuthenticAMD", 12) ? true : false;

	if ( (bIntelCPU == false) && (bExt == true) )
	{ // AMD CPU인가?;
		__asm
		{
			mov	eax, 0x80000001; // 확장 함수 정보를 얻어온다;
			cpuid;
			test	edx, 0x40000000; // 3DNow 확장을 지원하는가?;
			jz		__NOEXT3DNOW; // 지원하지 않으면 __NOEXT3DNOW로 넘어간다;
			mov	b3DNOW_Ext, 0x1; // 지원하면 threeNowExt_를 1로 변경한다;
__NOEXT3DNOW:
			test	edx, 0x00400000; // MMX 확장을 지원하는가?;
			jz		__NOEXTMMX;
			mov	bMMX_Ext, 0x1;
__NOEXTMMX:
		}
	}

	if ( bIntelCPU == true )
	{
		__asm 
		{
			mov	eax, 0x04;
			mov	ecx, 0x00;
			cpuid;
			shr	eax, 26;
			and	eax, 0x3f;
			add	eax, 0x01;
			mov	nCore, eax;
		}
	}
	else
	{
		__asm
		{
			mov	eax, 0x80000008;
			cpuid;
			and	ecx, 0xff;
			add	ecx, 0x01;
			mov	nCore, eax;
		}
	}


	CPUInfo.bSSE = bSSE;
	CPUInfo.bSSE2 = bSSE2;
	CPUInfo.b3DNOW = b3DNOW;
	CPUInfo.bMMX = bMMX;

	CPUInfo.bExt = bExt;
	CPUInfo.bMMX_Ext = bMMX_Ext;
	CPUInfo.b3DNOW_Ext = b3DNOW_Ext;

	CPUInfo.nCore = nCore;

	CPUInfo.bIntelCPU = bIntelCPU;
	CPUInfo.cVendorName[12] = 0;
}



