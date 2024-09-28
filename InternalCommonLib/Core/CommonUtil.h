/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3
 */
#pragma once

namespace ic
{
    inline BOOL boolToBOOL( bool bValue )
    {
        return bValue ? TRUE : FALSE;
    }

    inline bool BOOLTobool( BOOL bValue )
    {
        return bValue ? true : false;
    }

    template< class T >
    inline T Clamp( const T X, const T Min, const T Max )
    {
	    return X < Min ? Min : X < Max ? X : Max;
    }

    // P - end points
    // T - tangent directions at end points
    // Alpha - distance along spline
    template< class T, class U >
    T CubicInterp( const T& P0, const T& T0, const T& P1, const T& T1, const U& A )
    {
	    FLOAT A3 = powf( A, 3.0f );
	    FLOAT A2 = powf( A, 2.0f );

	    return T( ( ( ( 2 * A3 ) - ( 3 * A2 ) + 1 ) * P0 ) +
            ( ( A3 - ( 2 * A2 ) + A ) * T0 ) +
            ( ( A3 - A2 ) * T1 ) +
            ( ( ( -2 * A3 ) + ( 3 * A2 ) ) * P1 ) );
    }
}