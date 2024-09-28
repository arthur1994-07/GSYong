#pragma once

#include "SSE2Define.h"

class __m128_4
{
public:
	const __m128_4& operator =(const __m128_4& _xMatrix);
	const __m128_4& operator +=(const __m128_4& _xMatrix);
	const __m128_4& operator -=(const __m128_4& _xMatrix);
	const __m128_4& operator *=(const __m128_4& _xMatrix);
	const __m128_4& operator *=(const float _fScalar);
	const __m128_4& operator /=(const float _fScalar);

	const __m128_4 operator +(const __m128_4& _xMatrix) const;
	const __m128_4 operator -(const __m128_4& _xMatrix) const;
	const __m128_4 operator *(const __m128_4& _xMatrix) const;

	const __m128 operator *(const __m128& _xVector) const;

	const __m128_4 operator *(const float _fScalar) const;
	const __m128_4 operator /(const float _fScalar) const;

	__m128& operator [](const unsigned int _nIndex);
	const __m128& operator [](const unsigned int _nIndex) const;

public:
	__m128_4(void);
	__m128_4(const __m128_4& _xMatrix);
	__m128_4(const __m128& _xVector0, const __m128& _xVector1, const __m128& _xVector2, const __m128& _xVector3);
	__m128_4(
		const float _f00, const float _f01, const float _f02, const float _f03,
		const float _f10, const float _f11, const float _f12, const float _f13,
		const float _f20, const float _f21, const float _f22, const float _f23,
		const float _f30, const float _f31, const float _f32, const float _f33);

public:
	__m128 xMatrix[4];
};

namespace SSE2
{
	extern __forceinline const __m128 xmmX(const __m128& _x0); // x, x, x, x;
	extern __forceinline const __m128 xmmY(const __m128& _x0); // y, y, y, y;
	extern __forceinline const __m128 xmmZ(const __m128& _x0); // z, z, z, z;
	extern __forceinline const __m128 xmmW(const __m128& _x0); // w, w, w, w;
	extern __forceinline const __m128 xmm(const float _f0, const float _f1, const float _f2, const float _f3);
	extern __forceinline const __m128 xmm(const float _f0, const float _f1, const float _f2);	
	extern __forceinline const __m128 xmm(const float _f0); // _f0, _f0, _f0, _f0;

	extern __forceinline const float normalize(__m128& _x0);	 // _x0 -> normalize _x0;
	extern __forceinline const __m128 normal(const __m128& _x0); // ret : _x0 normalize;
	extern __forceinline const __m128 abs(const __m128& _x0);
	extern __forceinline const __m128 dotProduct(const __m128& _x0); // x+y+z, x+y+z, x+y+z, x+y+z;
	extern __forceinline const __m128 sqrt(const __m128& _x0); // squart root(x, y, z, w);
	extern __forceinline const __m128 length(const __m128& _x0); // length, length, length, length;
	extern __forceinline const __m128 lengthSquare(const __m128& _x0); // length^2, length^2, length^2, length^2;

	// _xOver 초과이면 _xOver로 잘라줌, ret : over(0xff), not over(0.0f);
	extern __forceinline const __m128 limitOver(__m128& _x0, const __m128& _xOver);

	// _xUnder 미만이면 _xUnder로 잘라줌, ret : under(0xff), not under(0.0f);
	extern __forceinline const __m128 limitUnder(__m128& _x0, const __m128& _xUnder);

	// ret : _xOver초과이거나 _xUnder 미만이면 잘라줌, ret : over(0xff), not over(0.0f);
	// 잘린 값이 over인지 under인지는 구분 못함;
	extern __forceinline const __m128 limit(__m128& _x0, const __m128& _xUnder, const __m128& _xOver);

	extern __forceinline const bool isZero(const __m128& _x0); // ret : _x0 == 0;

	extern __forceinline const __m128& rotateEulerAxisY(__m128& xTargetPosition, const __m128& xReferencePosition, const float fAngle);	

	extern __forceinline const __m128_4 identityMatrix(void);
	extern __forceinline const __m128_4 inverseWorldMatrix(const __m128_4& xSourceMatrix);	
	extern __forceinline const __m128_4 inverseViewMatrix(const __m128_4& xSourceMatrix, const __m128& xPosition);	
	extern __forceinline const __m128_4 quarternionMatrix(const __m128& xAxis, const float fAngle);
	extern __forceinline const __m128_4 rotateAxisZ(const __m128_4& xSourceMatrix, const float fAngle);

	extern __forceinline const __m128_4& identityMatrix(__m128_4& xTargetMatrix);
	extern __forceinline const __m128_4& inverseWorldMatrix(__m128_4& xTargetMatrix, const __m128_4& xSourceMatrix);
	extern __forceinline const __m128_4& inverseViewMatrix(__m128_4& xTargetMatrix, const __m128_4& xSourceMatrix, const __m128& xPosition);
	extern __forceinline const __m128_4& perspectiveProjectionMatrix(__m128_4& xTargetMatrix, const float fFOV, const float fAspect, const float fNearZ, const float fFarZ);
	extern __forceinline const __m128_4& orthoProjectionMatrix(__m128_4& xTargetMatrix, const float fWidth, const float fHeght, const float fNearZ, const float fFarZ);	
}

__forceinline extern const __m128& operator +=(__m128& _x0, const __m128& _x1);
__forceinline extern const __m128& operator -=(__m128& _x0, const __m128& _x1);
__forceinline extern const __m128& operator *=(__m128& _x0, const __m128& _x1);
__forceinline extern const __m128& operator *=(__m128& _x0, const float _f1);
__forceinline extern const __m128& operator /=(__m128& _x0, const __m128& _x1);
__forceinline extern const __m128& operator /=(__m128& _x0, const float _f1);

__forceinline extern const __m128 operator +(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator +(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator -(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator -(const __m128& _x0);
__forceinline extern const __m128 operator *(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator *(const __m128& _x0, const __m128_4& _xMatrix);
__forceinline extern const __m128 operator *(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator /(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator /(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator /(const float _f0, const __m128& _x1);
__forceinline extern const __m128 operator ^(const __m128& _x0, const __m128& _x1);

__forceinline extern const __m128 operator ==(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator ==(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator !=(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator !=(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator <=(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator <=(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator >=(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator >=(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator <(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator <(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator >(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator >(const __m128& _x0, const float _f1);

__forceinline extern const __m128 operator &(const __m128& _x0, const __m128& _x1);
__forceinline extern const __m128 operator &(const __m128& _x0, const float _f1);
__forceinline extern const __m128 operator |(const __m128& _x0, const __m128& _x1);

__forceinline extern const __m128& operator *=(__m128& _x0, const __m128_4& _xMatrix);


__forceinline extern const __m128i operator ==(const __m128i& _x0, const __m128i& _x1);
__forceinline extern const __m128i operator <(const __m128i& _x0, const __m128i& _x1);


extern void getCPUInformation(SSE2::CPUInformation& CPUInfo);
