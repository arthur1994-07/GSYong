#include "stdafx.h"

#include "SSE2.h"

const __m128_4& __m128_4::operator =(const __m128_4& _xMatrix)
{
	this->xMatrix[0] = _xMatrix[0];
	this->xMatrix[1] = _xMatrix[1];
	this->xMatrix[2] = _xMatrix[2];
	this->xMatrix[3] = _xMatrix[3];
	return *this;
}

const __m128_4& __m128_4::operator +=(const __m128_4& _xMatrix)
{
	this->xMatrix[0] += _xMatrix[0];
	this->xMatrix[1] += _xMatrix[1];
	this->xMatrix[2] += _xMatrix[2];
	this->xMatrix[3] += _xMatrix[3];
	return *this;
}
const __m128_4& __m128_4::operator -=(const __m128_4& _xMatrix)
{
	this->xMatrix[0] -= _xMatrix[0];
	this->xMatrix[1] -= _xMatrix[1];
	this->xMatrix[2] -= _xMatrix[2];
	this->xMatrix[3] -= _xMatrix[3];
	return *this;
}

const __m128_4& __m128_4::operator *=(const __m128_4& _xMatrix)
{
	const __m128& _xRow0 = this->xMatrix[0];
	this->xMatrix[0] = 
		SSE2::xmmX(_xRow0) * _xMatrix[0] + 
		SSE2::xmmY(_xRow0) * _xMatrix[1] + 
		SSE2::xmmZ(_xRow0) * _xMatrix[2];

	const __m128& _xRow1 = this->xMatrix[1];
	this->xMatrix[1] = 
		SSE2::xmmX(_xRow1) * _xMatrix[0] + 
		SSE2::xmmY(_xRow1) * _xMatrix[1] + 
		SSE2::xmmZ(_xRow1) * _xMatrix[2];

	const __m128& _xRow2 = this->xMatrix[2];	
	this->xMatrix[2] = 
		SSE2::xmmX(_xRow2) * _xMatrix[0] + 
		SSE2::xmmY(_xRow2) * _xMatrix[1] + 
		SSE2::xmmZ(_xRow2) * _xMatrix[2];

	const __m128& _xRow3 = this->xMatrix[3];
	this->xMatrix[3] = 
		SSE2::xmmX(_xRow2) * _xMatrix[0] + 
		SSE2::xmmY(_xRow2) * _xMatrix[1] + 
		SSE2::xmmZ(_xRow2) * _xMatrix[2] +
		_xMatrix[3];

	return *this;
}

const __m128_4& __m128_4::operator *=(const float _fScalar)
{
	const __m128& _xScalar = SSE2::xmm(_fScalar);
	this->xMatrix[0] *= _xScalar;
	this->xMatrix[1] *= _xScalar;
	this->xMatrix[2] *= _xScalar;

	return *this;
}
const __m128_4& __m128_4::operator /=(const float _fScalar)
{
	const __m128& _xScalar = 1.0f / SSE2::xmm(_fScalar);
	this->xMatrix[0] *= _xScalar;
	this->xMatrix[1] *= _xScalar;
	this->xMatrix[2] *= _xScalar;

	return *this;
}

const __m128_4 __m128_4::operator +(const __m128_4& _xMatrix) const
{
	return __m128_4(
		this->xMatrix[0] + _xMatrix[0],
		this->xMatrix[1] + _xMatrix[1],
		this->xMatrix[2] + _xMatrix[2],
		this->xMatrix[3] + _xMatrix[3]);
}
const __m128_4 __m128_4::operator -(const __m128_4& _xMatrix) const
{
	return __m128_4(
		this->xMatrix[0] - _xMatrix[0],
		this->xMatrix[1] - _xMatrix[1],
		this->xMatrix[2] - _xMatrix[2],
		this->xMatrix[3] - _xMatrix[3]);
}
const __m128_4 __m128_4::operator *(const __m128_4& _xMatrix) const
{
	const __m128& _xRow0 = this->xMatrix[0];
	const __m128& _xRow1 = this->xMatrix[1];		
	const __m128& _xRow2 = this->xMatrix[2];
	const __m128& _xRow3 = this->xMatrix[3];

	return __m128_4(
		SSE2::xmmX(_xRow0) * _xMatrix[0] + SSE2::xmmY(_xRow0) * _xMatrix[1] + SSE2::xmmZ(_xRow0) * _xMatrix[2],
		SSE2::xmmX(_xRow1) * _xMatrix[0] + SSE2::xmmY(_xRow1) * _xMatrix[1] + SSE2::xmmZ(_xRow1) * _xMatrix[2],
		SSE2::xmmX(_xRow2) * _xMatrix[0] + SSE2::xmmY(_xRow2) * _xMatrix[1] + SSE2::xmmZ(_xRow2) * _xMatrix[2],
		SSE2::xmmX(_xRow3) * _xMatrix[0] + SSE2::xmmY(_xRow3) * _xMatrix[1] + SSE2::xmmZ(_xRow3) * _xMatrix[2] + _xMatrix[3]
	);
}

const __m128 __m128_4::operator *(const __m128& _xVector) const
{
	const __m128& _x0 = this->xMatrix[0] * _xVector;
	const __m128& _x1 = this->xMatrix[1] * _xVector;
	const __m128& _x2 = this->xMatrix[2] * _xVector;
	const __m128& _x3 = this->xMatrix[3] * _xVector;

	return _x0 + _x1 + _x2 + _x3;
}

const __m128_4 __m128_4::operator *(const float _fScalar) const
{
	const __m128& _xScalar = SSE2::xmm(_fScalar);
	return __m128_4(
		this->xMatrix[0] * _xScalar,
		this->xMatrix[1] * _xScalar,
		this->xMatrix[2] * _xScalar,
		this->xMatrix[3]);
}
const __m128_4 __m128_4::operator /(const float _fScalar) const
{
	const __m128& _xScalar = 1.0f / SSE2::xmm(_fScalar);
	return __m128_4(
		this->xMatrix[0] * _xScalar,
		this->xMatrix[1] * _xScalar,
		this->xMatrix[2] * _xScalar,
		this->xMatrix[3]);
}

__m128& __m128_4::operator [](const unsigned int _nIndex)
{
	return this->xMatrix[_nIndex];
}
const __m128& __m128_4::operator [](const unsigned int _nIndex) const
{
	return this->xMatrix[_nIndex];
}

__m128_4::__m128_4(void)
{
}
__m128_4::__m128_4(const __m128_4& _xMatrix)
{
	this->xMatrix[0] = _xMatrix[0];
	this->xMatrix[1] = _xMatrix[1];
	this->xMatrix[2] = _xMatrix[2];
	this->xMatrix[3] = _xMatrix[3];
}
__m128_4::__m128_4(const __m128& _xVector0, const __m128& _xVector1, const __m128& _xVector2, const __m128& _xVector3)
{
	this->xMatrix[0] = _xVector0;
	this->xMatrix[1] = _xVector1;
	this->xMatrix[2] = _xVector2;
	this->xMatrix[3] = _xVector3;
}
__m128_4::__m128_4(
				   const float _f00, const float _f01, const float _f02, const float _f03,
				   const float _f10, const float _f11, const float _f12, const float _f13,
				   const float _f20, const float _f21, const float _f22, const float _f23,
				   const float _f30, const float _f31, const float _f32, const float _f33)
{
	this->xMatrix[0] = ::_mm_set_ps(_f03, _f02, _f01, _f00);
	this->xMatrix[1] = ::_mm_set_ps(_f13, _f12, _f11, _f10);
	this->xMatrix[2] = ::_mm_set_ps(_f23, _f22, _f21, _f20);
	this->xMatrix[3] = ::_mm_set_ps(_f33, _f32, _f31, _f30);
}
