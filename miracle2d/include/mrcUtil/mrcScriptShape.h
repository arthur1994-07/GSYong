

#ifndef _mrcSqVector_h_
#define _mrcSqVector_h_

#include "mrcConfig.h"

#include "mrcVector2.h"
#include "mrcVector3.h"
#include "mrcShape.h"
namespace mrc
{

#if MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_SQUIRREL

	class SqVector2 : public Sqrat::SqObjectType , public Vector2
	{
	public:
		SqVector2(){}
		SqVector2(real x1,real y1){ x = x1; y = y1; }
		SqVector2(const Vector2& vec) : Vector2(vec) {}

		SqVector2 operator+(const SqVector2& vec) const { return Vector2::operator+(vec); }
		SqVector2 operator-(const SqVector2& vec) const { return Vector2::operator-(vec); }
		SqVector2 operator*(const SqVector2& vec) const { return Vector2::operator*(vec); }
		SqVector2 operator/(const SqVector2& vec) const { return Vector2::operator/(vec); }
		bool compare(const SqVector2& vec) const { return Vector2::operator==(vec);  } 
	};

	class SqVector3 : public Sqrat::SqObjectType , public Vector3
	{
	public:
		SqVector3(){}
		SqVector3(real x1,real y1,real z1){ x = x1; y = y1; z = z1; }
		SqVector3(const Vector3& vec) : Vector3(vec) {}

		SqVector3 operator+(const SqVector3& vec) const { return Vector3::operator+(vec); }
		SqVector3 operator-(const SqVector3& vec) const { return Vector3::operator-(vec); }
		SqVector3 operator*(const SqVector3& vec) const { return Vector3::operator*(vec); }
		SqVector3 operator/(const SqVector3& vec) const { return Vector3::operator/(vec); }
		bool compare(const SqVector3& vec) const { return Vector3::operator==(vec);  } 
	};

	class SqPoint : public Sqrat::SqObjectType , public Point
	{
	public:
		SqPoint() {}
		SqPoint(int16 x,int16 y) { mX = x; mY = y; }
		SqPoint(const Point& point) : Point(point) {}
	};


	class SqRect : public Sqrat::SqObjectType , public Rect
	{
	public:
		SqRect() {}
		SqRect(int16 left,int16 top,int16 right,int16 bottom)
		{
			mLeft = left;
			mTop = top;
			mRight = right;
			mBottom = bottom;
		}

		SqRect(const Rect& rect) 
		{
			mLeft = rect.mLeft;
			mTop = rect.mTop; 
			mRight = rect.mRight;
			mBottom = rect.mBottom;
		}		
	};

	class SqColorRGB : public Sqrat::SqObjectType
	{
	public:
		SqColorRGB(real colorR,real colorG,real colorB)
		{
			r = colorR; g = colorG; b = colorB; 
		}

		SqColorRGB() { r = g = b = real(1); }

		bool compare(const SqColorRGB& rgb) const { return r == rgb.r && g == rgb.g && b == rgb.b; }

		real r,g,b;
	};

#elif MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_LUA

#else

#error unknown script

#endif



}





#endif