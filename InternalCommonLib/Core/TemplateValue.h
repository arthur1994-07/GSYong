/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 */
#pragma once

namespace ic
{
    template< class T >
    struct TValue2
    {
        T x;
        T y;

        TValue2()
            : x( 0 )
            , y( 0 )
        {
        }
        TValue2( T x_, T y_ )
            : x( x_ )
            , y( y_ )
        {
        }

        TValue2& operator += ( const TValue2& t )
        {
            x += t.x;
            y += t.y;
            return *this;
        }
        TValue2& operator -= ( const TValue2& t )
        {
            x -= t.x;
            y -= t.y;
            return *this;
        }
        TValue2& operator *= ( const TValue2& t )
        {
            x *= t.x;
            y *= t.y;
            return *this;
        }
        TValue2& operator /= ( const TValue2& t )
        {
            x /= t.x;
            y /= t.y;
            return *this;
        }

        TValue2 operator + ( const TValue2& t ) const
        {
            return TValue2( x + t.x, y + t.y );
        }
        TValue2 operator - ( const TValue2& t ) const
        {
            return TValue2( x - t.x, y - t.y );
        }
        TValue2 operator * ( const TValue2& t ) const
        {
            return TValue2( x * t.x, y * t.y );
        }
        TValue2 operator / ( const TValue2& t ) const
        {
            return TValue2( x / t.x, y / t.y );
        }

        BOOL operator == ( const TValue2& t ) const
        {
            return x == t.x && y == t.y;
        }
        BOOL operator != ( const TValue2& t ) const
        {
            return x != t.x || y != t.y;
        }
    };

    template< class T >
    struct TValue3
    {
        T x;
        T y;
        T z;

        TValue3()
            : x( 0 )
            , y( 0 )
            , z( 0 )
        {
        }
        TValue3( T x_, T y_, T z_ )
            : x( x_ )
            , y( y_ )
            , z( z_ )
        {
        }

        TValue3& operator += ( const TValue3& t )
        {
            x += t.x;
            y += t.y;
            z += t.z;
            return *this;
        }
        TValue3& operator -= ( const TValue3& t )
        {
            x -= t.x;
            y -= t.y;
            z -= t.z;
            return *this;
        }
        TValue3& operator *= ( const TValue3& t )
        {
            x *= t.x;
            y *= t.y;
            z *= t.z;
            return *this;
        }
        TValue3& operator /= ( const TValue3& t )
        {
            x /= t.x;
            y /= t.y;
            z /= t.z;
            return *this;
        }

        TValue3 operator + ( const TValue3& t ) const
        {
            return TValue3( x + t.x, y + t.y, z + t.z );
        }
        TValue3 operator - ( const TValue3& t ) const
        {
            return TValue3( x - t.x, y - t.y, z - t.z );
        }
        TValue3 operator * ( const TValue3& t ) const
        {
            return TValue3( x * t.x, y * t.y, z * t.z );
        }
        TValue3 operator / ( const TValue3& t ) const
        {
            return TValue3( x / t.x, y / t.y, z / t.z );
        }

        BOOL operator == ( const TValue3& t ) const
        {
            return x == t.x && y == t.y && z == t.z;
        }
        BOOL operator != ( const TValue3& t ) const
        {
            return x != t.x || y != t.y || z != t.z;
        }
    };

    template< class T >
    struct TValue4
    {
        T x;
        T y;
        T z;
        T w;

        TValue4()
            : x( 0 )
            , y( 0 )
            , z( 0 )
            , w( 0 )
        {
        }
        TValue4( T x_, T y_, T z_, T w_ )
            : x( x_ )
            , y( y_ )
            , z( z_ )
            , w( w_ )
        {
        }

        TValue4& operator += ( const TValue4& t )
        {
            x += t.x;
            y += t.y;
            z += t.z;
            w += t.w;
            return *this;
        }
        TValue4& operator -= ( const TValue4& t )
        {
            x -= t.x;
            y -= t.y;
            z -= t.z;
            w -= t.w;
            return *this;
        }
        TValue4& operator *= ( const TValue4& t )
        {
            x *= t.x;
            y *= t.y;
            z *= t.z;
            w *= t.w;
            return *this;
        }
        TValue4& operator /= ( const TValue4& t )
        {
            x /= t.x;
            y /= t.y;
            z /= t.z;
            w /= t.w;
            return *this;
        }

        TValue4 operator + ( const TValue4& t ) const
        {
            return TValue4( x + t.x, y + t.y, z + t.z, w + t.w );
        }
        TValue4 operator - ( const TValue4& t ) const
        {
            return TValue4( x - t.x, y - t.y, z - t.z, w - t.w );
        }
        TValue4 operator * ( const TValue4& t ) const
        {
            return TValue4( x * t.x, y * t.y, z * t.z, w * t.w );
        }
        TValue4 operator / ( const TValue4& t ) const
        {
            return TValue4( x / t.x, y / t.y, z / t.z, w / t.w );
        }

        BOOL operator == ( const TValue4& t ) const
        {
            return x == t.x && y == t.y && z == t.z && w == t.w;
        }
        BOOL operator != ( const TValue4& t ) const
        {
            return x != t.x || y != t.y || z != t.z || w != t.w;
        }
    };

    typedef TValue2< INT > Int2;
    typedef TValue3< INT > Int3;
    typedef TValue4< INT > Int4;

    typedef TValue2< FLOAT > Float2;
    typedef TValue3< FLOAT > Float3;
    typedef TValue4< FLOAT > Float4;
}