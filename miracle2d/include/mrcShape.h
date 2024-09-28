

#ifndef _mrcRect_h_
#define _mrcRect_h_


#include "mrcConfig.h"

namespace mrc
{

	template<class T>
	struct __Point
	{
		__Point() { mX = mY = T(); }
		__Point(T x,T y) 
		{
			mX = x; mY = y;
		}	

		bool operator==(const __Point& point) const	{ return mX == point.mX && mY == point.mY; }
		bool operator!=(const __Point& point) const { return !(*this == point); }

		//////////////////////////////////////////////////////////////////////////
		T mX,mY;
	};

	struct Point : public __Point<int16>
	{
		Point() {}
		Point(int16 x,int16 y) : __Point<int16>(x,y) {}
		void saveXml( TiXmlElement* pDataEle ) const;
		void loadXml( TiXmlElement* pDataEle );
	};


	template<class T>
	struct __Rect
	{
		__Rect() { mLeft = mTop = mRight = mBottom = T(); }
		__Rect(T left,T top,T right,T bottom) 
		{
			mLeft = left;  
			mTop  = top;
			mRight = right; 
			mBottom = bottom;
		}	

		T getWidth() const 
		{
			T width = mRight - mLeft;
			mrcASSERT(width >= 0);
			return width;
		}

		T getHeight() const
		{
			T height = mBottom - mTop;
			mrcASSERT(height >= 0);
			return height;
		}

		bool operator==(const __Rect& rect) const
		{
			return mLeft == rect.mLeft && mTop == rect.mTop &&
				mRight == rect.mRight && mBottom == rect.mBottom;
		}

		bool operator!=(const __Rect& rect) const
		{
			return !(*this == rect);
		}
		//////////////////////////////////////////////////////////////////////////

		T mLeft,mTop,mRight,mBottom;
	};

	
	struct Rect : public __Rect<int16>
	{
		Rect() {}
		Rect(int16 left,int16 top,int16 right,int16 bottom) : __Rect<int16>(left,top,right,bottom) {}


		void saveXml( TiXmlElement* pDataEle ) const;
		void loadXml( TiXmlElement* pDataEle );
	};

	struct Rect_real : public __Rect<real>
	{
		Rect_real() {}
		Rect_real(real left,real top,real right,real bottom) : __Rect<real>(left,top,right,bottom) {}


		void saveXml( TiXmlElement* pDataEle ) const;
		void loadXml( TiXmlElement* pDataEle );
	};

	template<class T>
	struct __Triangle 
	{
		__Triangle() {}
		__Triangle(const T& first,const T& second,const T& third)
		{
			mPoints[0] = first;	mPoints[1] = second; mPoints[2] = third;
		}

		bool operator==(const __Triangle& tri) const 
		{
			return mPoints[0] == tri.mPoints[0] && mPoints[1] == tri.mPoints[1] &&
				mPoints[2] == tri.mPoints[2];
		}

		bool operator!=(const __Triangle& tri) const { return !(*this == tri); }

		T mPoints[3];
	};

	struct Triangle : __Triangle<Point>
	{
		Triangle() {}
		Triangle(const Point& first,const Point& second,const Point& third)	: 
			__Triangle(first,second,third) {}

		void saveXml( TiXmlElement* pDataEle ) const;
		void loadXml( TiXmlElement* pDataEle );
	};
}


#endif


