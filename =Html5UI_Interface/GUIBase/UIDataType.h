#pragma	once

namespace GUIBase
{

// UI의 크기와 위치 등을 판단하는 구조체;
//! UIRECT
struct UIRECT
{
	float left, right, top, bottom;
	float sizeX, sizeY;

	UIRECT () :
	left ( 0.0f ), right ( 0.0f ), top ( 0.0f ), bottom ( 0.0f ),
		sizeX ( 0.0f ), sizeY ( 0.0f )
	{
	}

	UIRECT ( float _left, float _top, float _sizeX, float _sizeY )
	{
		left = _left;
		top = _top;
		sizeX = _sizeX;
		sizeY = _sizeY;
		//
		right = left + sizeX;
		bottom = top + sizeY;
	}

	const UIRECT& operator= ( const UIRECT &rcPos )
	{
		if( this == &rcPos )
			return *this;

		left = rcPos.left;
		top = rcPos.top;
		sizeX = rcPos.sizeX;
		sizeY = rcPos.sizeY;
		//
		right = left + sizeX;
		bottom = top + sizeY;

		return *this;
	}

	UIRECT operator+ ( const UIRECT &rcPos )
	{
		UIRECT rcTemp;
		rcTemp.left = left + rcPos.left;
		rcTemp.top = top + rcPos.top;
		rcTemp.sizeX = sizeX + rcPos.sizeX;
		rcTemp.sizeY = sizeY + rcPos.sizeY;

		// MEMO : 좌표 계산에 문제가 있는것 같다. +1을 해주는게 맞을거 같다.
		// 하지만 지금 이 좌표대로 CFG가 설정되어있어서 변경 불가능
		rcTemp.right = right + rcPos.right;
		rcTemp.bottom = bottom + rcPos.bottom;

		return rcTemp;
	}

	bool operator == ( const UIRECT &rcPos )
	{
		if ( left != rcPos.left )	return false; // MEMO
		if ( top  != rcPos.top  )	return false;
		if ( sizeX!= rcPos.sizeX)	return false;
		if ( sizeY!= rcPos.sizeY)	return false;

		return true;
	}

	void SetPosition( float _left, float _top, float _sizeX, float _sizeY )
	{
		left = _left;
		top = _top;
		sizeX = _sizeX;
		sizeY = _sizeY;

		// MEMO : 좌표 계산에 문제가 있는것 같다. -1을 해주는게 맞을거 같다.
		// 하지만 지금 이 좌표대로 CFG가 설정되어있어서 변경 불가능
		right = left + sizeX;
		bottom = top + sizeY;
	}

	void SetPosition( float _PosX, float _PosY )
	{
		left  = _PosX;
		top   = _PosY;

		right  = left + sizeX;
		bottom = top + sizeY;
	}
};

//! UI Point
template< class T >
struct UIPOINT
{
	T x;
	T y;
	T z;

	UIPOINT()
		: x( 0 )
		, y( 0 )
		, z( 0 ) { }

	UIPOINT( T _x, T _y, T _z )
		: x( _x )
		, y( _y )
		, z( _z ) { }

	const UIPOINT<T>& operator= ( const UIPOINT<T>& point )
	{
		if( this == &point )
			return *this;

		x = point.x;
		y = point.y;
		z = point.z;

		return *this;
	}

	UIPOINT<T> operator+ ( const UIPOINT<T>& point )
	{
		UIPOINT<T> returnPoint;
		returnPoint.x = x + point.x;
		returnPoint.y = y + point.y;
		returnPoint.z = z + point.z;

		return returnPoint;
	}

	bool operator== ( const UIPOINT<T>& point )
	{
		if ( x != point.x ) return false;
		if ( y != point.y ) return false;
		if ( z != point.z ) return false;

		return true;
	}

	void SetPoint( T _x, T _y, T _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

//! UI Size
template< class T >
struct UISIZE
{
	T sizeX;
	T sizeY;

	UISIZE()
		: sizeX( 0 )
		, sizeY( 0 ) { }

	UISIZE( T _sizeX, T _sizeY )
		: sizeX( _sizeX )
		, sizeY( _sizeY ) { }

	const UISIZE<T>& operator= ( const UISIZE<T>& size )
	{
		if( this == &size )
			return *this;

		sizeX = size.sizeX;
		sizeY = size.sizeY;

		return *this;
	}

	UISIZE<T> operator+ ( const UISIZE<T>& size )
	{
		UISIZE returnSize;
		returnSize.sizeX = sizeX + size.sizeX;
		returnSize.sizeY = sizeY + size.sizeY;

		return returnSize;
	}

	bool operator== ( const UISIZE<T>& size )
	{
		if ( sizeX != size.sizeX ) return false;
		if ( sizeY != size.sizeY ) return false;

		return true;
	}

	void SetSize( T _sizeX, T _sizeY )
	{
		sizeX = sizeX;
		sizeY = sizeY;
	}
};

typedef	int UIID;

const UIID NO_ID = 0;
const UIID FORCE_ID = 10000;

}