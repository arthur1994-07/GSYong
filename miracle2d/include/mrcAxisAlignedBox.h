/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef _mrcAxisAlignedBox_h_
#define _mrcAxisAlignedBox_h_

// Precompiler options
#include "mrcConfig.h"

#include "mrcVector2.h"
#include "mrcMatrix4.h"

#include "mrcUtil/mrcVectorOperator.h"
#include "mrcASSERT.h"

#define Real real

namespace mrc {
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/

	/** A 3D box aligned with the x/y/z axes.
	@remarks
	This class represents a simple box which is aligned with the
	axes. Internally it only stores 2 points as the extremeties of
	the box, one which is the minima of all 3 axes, and the other
	which is the maxima of all 3 axes. This class is typically used
	for an axis-aligned bounding box (AABB) for collision and
	visibility determination.
	*/
	class  AxisAlignedBox
	{
	public:
		enum Extent
		{
			EXTENT_NULL,
			EXTENT_FINITE,
			EXTENT_INFINITE
		};
	protected:

		Vector2 mMinimum;
		Vector2 mMaximum;
		Extent mExtent;
		mutable Vector2* mpCorners;

	public:
		/*
		1-----2
		/|    /|
		/ |   / |
		5-----4  |
		|  0--|--3
		| /   | /
		|/    |/
		6-----7
		*/
		typedef enum {
			LEFT_TOP= 0,
			LEFT_BOTTOM = 1,
			RIGHT_BOTTOM = 2,
			RIGHT_TOP = 3,
		} CornerEnum;
		inline AxisAlignedBox() : mMinimum(Vector2::ZERO), mMaximum(Vector2::UNIT_SCALE), mpCorners(0)
		{
			// Default to a null box 
			setMinimum( -0.5, -0.5 );
			setMaximum( 0.5, 0.5 );
			mExtent = EXTENT_NULL;
		}
		inline AxisAlignedBox(Extent e) : mMinimum(Vector2::ZERO), mMaximum(Vector2::UNIT_SCALE), mpCorners(0)
		{
			setMinimum( -0.5, -0.5 );
			setMaximum( 0.5, 0.5 );
			mExtent = e;
		}

		inline AxisAlignedBox(const AxisAlignedBox & rkBox) : mMinimum(Vector2::ZERO), mMaximum(Vector2::UNIT_SCALE), mpCorners(0)

		{
			if (rkBox.isNull())
				setNull();
			else if (rkBox.isInfinite())
				setInfinite();
			else
				setExtents( rkBox.mMinimum, rkBox.mMaximum );
		}

		inline AxisAlignedBox( const Vector2& min, const Vector2& max ) : mMinimum(Vector2::ZERO), mMaximum(Vector2::UNIT_SCALE), mpCorners(0)
		{
			setExtents( min, max );
		}

		inline AxisAlignedBox(Real mx, Real my, Real Mx, Real My ) 
			: mMinimum(Vector2::ZERO), mMaximum(Vector2::UNIT_SCALE), mpCorners(0)
		{
			setExtents( mx, my,  Mx, My );
		}

		AxisAlignedBox& operator=(const AxisAlignedBox& rhs)
		{
			// Specifically override to avoid copying mpCorners
			if (rhs.isNull())
				setNull();
			else if (rhs.isInfinite())
				setInfinite();
			else
				setExtents(rhs.mMinimum, rhs.mMaximum);

			return *this;
		}

		~AxisAlignedBox()
		{
			if (mpCorners)
				mrcDELETE [] mpCorners;
		}


		/** Gets the minimum corner of the box.
		*/
		inline const Vector2& getMinimum(void) const
		{ 
			return mMinimum; 
		}

		/** Gets a modifiable version of the minimum
		corner of the box.
		*/
		inline Vector2& getMinimum(void)
		{ 
			return mMinimum; 
		}

		/** Gets the maximum corner of the box.
		*/
		inline const Vector2& getMaximum(void) const
		{ 
			return mMaximum;
		}

		/** Gets a modifiable version of the maximum
		corner of the box.
		*/
		inline Vector2& getMaximum(void)
		{ 
			return mMaximum;
		}


		/** Sets the minimum corner of the box.
		*/
		inline void setMinimum( const Vector2& vec )
		{
			mExtent = EXTENT_FINITE;
			mMinimum = vec;
		}

		inline void setMinimum( Real x, Real y )
		{
			mExtent = EXTENT_FINITE;
			mMinimum.x = x;
			mMinimum.y = y;
		}

		/** Changes one of the components of the minimum corner of the box
		used to resize only one dimension of the box
		*/
		inline void setMinimumX(Real x)
		{
			mMinimum.x = x;
		}

		inline void setMinimumY(Real y)
		{
			mMinimum.y = y;
		}

		/** Sets the maximum corner of the box.
		*/
		inline void setMaximum( const Vector2& vec )
		{
			mExtent = EXTENT_FINITE;
			mMaximum = vec;
		}

		inline void setMaximum( Real x, Real y )
		{
			mExtent = EXTENT_FINITE;
			mMaximum.x = x;
			mMaximum.y = y;
		}

		/** Changes one of the components of the maximum corner of the box
		used to resize only one dimension of the box
		*/
		inline void setMaximumX( Real x )
		{
			mMaximum.x = x;
		}

		inline void setMaximumY( Real y )
		{
			mMaximum.y = y;
		}

		/** Sets both minimum and maximum extents at once.
		*/
		inline void setExtents( const Vector2& min, const Vector2& max )
		{
            mrcASSERT( (min.x <= max.x && min.y <= max.y ) &&
                "The minimum corner of the box must be less than or equal to maximum corner" );

			mExtent = EXTENT_FINITE;
			mMinimum = min;
			mMaximum = max;
		}

		inline void setExtents(	Real mx, Real my,Real Mx, Real My )
		{
            mrcASSERT( (mx <= Mx && my <= My ) &&
                "The minimum corner of the box must be less than or equal to maximum corner" );

			mExtent = EXTENT_FINITE;

			mMinimum.x = mx;
			mMinimum.y = my;

			mMaximum.x = Mx;
			mMaximum.y = My;
		}

		/** Returns a pointer to an array of 8 corner points, useful for
		collision vs. non-aligned objects.
		@remarks
		If the order of these corners is important, they are as
		follows: The 4 points of the minimum Z face (note that
		because Ogre uses right-handed coordinates, the minimum Z is
		at the 'back' of the box) starting with the minimum point of
		all, then anticlockwise around this face (if you are looking
		onto the face from outside the box). Then the 4 points of the
		maximum Z face, starting with maximum point of all, then
		anticlockwise around this face (looking onto the face from
		outside the box). Like this:
		<pre>
		1-----2
		/|    /|
		/ |   / |
		5-----4  |
		|  0--|--3
		| /   | /
		|/    |/
		6-----7
		</pre>
		@remarks as this implementation uses a static member, make sure to use your own copy !
		*/
		inline const Vector2* getAllCorners(void) const
		{
			mrcASSERT( (mExtent == EXTENT_FINITE) && "Can't get corners of a null or infinite AAB" );

			// The order of these items is, using right-handed co-ordinates:
			// Minimum Z face, starting with Min(all), then anticlockwise
			//   around face (looking onto the face)
			// Maximum Z face, starting with Max(all), then anticlockwise
			//   around face (looking onto the face)
			// Only for optimization/compatibility.
			if (!mpCorners)
				mpCorners = mrcNEW Vector2[4];

			mpCorners[0] = mMinimum;
			mpCorners[1].x = mMinimum.x; mpCorners[1].y = mMaximum.y; 
			mpCorners[2].x = mMaximum.x; mpCorners[2].y = mMaximum.y; 
			mpCorners[3].x = mMaximum.x; mpCorners[3].y = mMinimum.y;           

			return mpCorners;
		}

		/** gets the position of one of the corners
		*/
		Vector2 getCorner(CornerEnum cornerToGet) const
		{
			switch(cornerToGet)
			{
			case LEFT_TOP:
				return mMinimum;
			case LEFT_BOTTOM:
				return Vector2(mMinimum.x, mMaximum.y);
			case RIGHT_BOTTOM:
				return Vector2(mMaximum.x, mMaximum.y);
			case RIGHT_TOP:
				return Vector2(mMaximum.x, mMinimum.y);
			}

			mrcASSERT(false);
			return Vector2();
		}

		
		/** Merges the passed in box into the current box. The result is the
		box which encompasses both.
		*/
		void merge( const AxisAlignedBox& rhs )
		{
			// Do nothing if rhs null, or this is infinite
			if ((rhs.mExtent == EXTENT_NULL) || (mExtent == EXTENT_INFINITE))
			{
				return;
			}
			// Otherwise if rhs is infinite, make this infinite, too
			else if (rhs.mExtent == EXTENT_INFINITE)
			{
				mExtent = EXTENT_INFINITE;
			}
			// Otherwise if current null, just take rhs
			else if (mExtent == EXTENT_NULL)
			{
				setExtents(rhs.mMinimum, rhs.mMaximum);
			}
			// Otherwise merge
			else
			{
				Vector2 min = mMinimum;
				Vector2 max = mMaximum;
				max.makeCeil(rhs.mMaximum);
				min.makeFloor(rhs.mMinimum);

				setExtents(min, max);
			}

		}

		/** Extends the box to encompass the specified point (if needed).
		*/
		inline void merge( const Vector2& point )
		{
			switch (mExtent)
			{
			case EXTENT_NULL: // if null, use this point
				setExtents(point, point);
				return;

			case EXTENT_FINITE:
				mMaximum.makeCeil(point);
				mMinimum.makeFloor(point);
				return;

			case EXTENT_INFINITE: // if infinite, makes no difference
				return;
			}

			mrcASSERT( false && "Never reached" );
		}

		/** Transforms the box according to the matrix supplied.
		@remarks
		By calling this method you get the axis-aligned box which
		surrounds the transformed version of this box. Therefore each
		corner of the box is transformed by the matrix, then the
		extents are mapped back onto the axes to produce another
		AABB. Useful when you have a local AABB for an object which
		is then transformed.
		*/
		inline void transform( const Matrix4& matrix )
		{
			// Do nothing if current null or infinite
			if( mExtent != EXTENT_FINITE )
				return;

			Vector2 oldMin, oldMax;

			// Getting the old values so that we can use the existing merge method.
			oldMin = mMinimum;
			oldMax = mMaximum;

			// reset
			setNull();

			// We sequentially compute the corners in the following order :
			// 0, 6, 5, 1, 2, 4 ,7 , 3
			// This sequence allows us to only change one member at a time to get at all corners.

			// For each one, we transform it using the matrix
			// Which gives the resulting point and merge the resulting point.

			// First corner 
			// min min 
			Vector2 currentCorner = oldMin;
			merge( matrix * currentCorner );

			// min max 
			currentCorner.y = oldMax.y;
			merge( matrix * currentCorner );

			// max max 
			currentCorner.x = oldMax.x;
			merge( matrix * currentCorner );

			// max min
			currentCorner.y = oldMin.y;
			merge( matrix * currentCorner );

		}

		/** Transforms the box according to the affine matrix supplied.
		@remarks
		By calling this method you get the axis-aligned box which
		surrounds the transformed version of this box. Therefore each
		corner of the box is transformed by the matrix, then the
		extents are mapped back onto the axes to produce another
		AABB. Useful when you have a local AABB for an object which
		is then transformed.
		@note
		The matrix must be an affine matrix. @see Matrix4::isAffine.
		*/
// 		void transformAffine(const Matrix4& m)
// 		{
// 			mrcASSERT(m.isAffine());
// 
// 			// Do nothing if current null or infinite
// 			if ( mExtent != EXTENT_FINITE )
// 				return;
// 
// 			Vector2 centre = getCenter();
// 			Vector2 halfSize = getHalfSize();
// 
// 			Vector2 newCentre = m.transformAffine(centre);
// 			Vector2 newHalfSize(
// 				Math::Abs(m[0][0]) * halfSize.x + Math::Abs(m[0][1]) * halfSize.y + Math::Abs(m[0][2]) * halfSize.z, 
// 				Math::Abs(m[1][0]) * halfSize.x + Math::Abs(m[1][1]) * halfSize.y + Math::Abs(m[1][2]) * halfSize.z,
// 				Math::Abs(m[2][0]) * halfSize.x + Math::Abs(m[2][1]) * halfSize.y + Math::Abs(m[2][2]) * halfSize.z);
// 
// 			setExtents(newCentre - newHalfSize, newCentre + newHalfSize);
// 		}

		/** Sets the box to a 'null' value i.e. not a box.
		*/
		inline void setNull()
		{
			mExtent = EXTENT_NULL;
		}

		/** Returns true if the box is null i.e. empty.
		*/
		inline bool isNull(void) const
		{
			return (mExtent == EXTENT_NULL);
		}

		/** Returns true if the box is finite.
		*/
		bool isFinite(void) const
		{
			return (mExtent == EXTENT_FINITE);
		}

		/** Sets the box to 'infinite'
		*/
		inline void setInfinite()
		{
			mExtent = EXTENT_INFINITE;
		}

		/** Returns true if the box is infinite.
		*/
		bool isInfinite(void) const
		{
			return (mExtent == EXTENT_INFINITE);
		}

		/** Returns whether or not this box intersects another. */
		inline bool intersects(const AxisAlignedBox& b2) const
		{
			// Early-fail for nulls
			if (this->isNull() || b2.isNull())
				return false;

			// Early-success for infinites
			if (this->isInfinite() || b2.isInfinite())
				return true;

			// Use up to 6 separating planes
			if (mMaximum.x < b2.mMinimum.x)
				return false;
			if (mMaximum.y < b2.mMinimum.y)
				return false;

			if (mMinimum.x > b2.mMaximum.x)
				return false;
			if (mMinimum.y > b2.mMaximum.y)
				return false;

			// otherwise, must be intersecting
			return true;

		}

		/// Calculate the area of intersection of this box and another
		inline AxisAlignedBox intersection(const AxisAlignedBox& b2) const
		{
            if (this->isNull() || b2.isNull())
			{
				return AxisAlignedBox();
			}
			else if (this->isInfinite())
			{
				return b2;
			}
			else if (b2.isInfinite())
			{
				return *this;
			}

			Vector2 intMin = mMinimum;
            Vector2 intMax = mMaximum;

            intMin.makeCeil(b2.getMinimum());
            intMax.makeFloor(b2.getMaximum());

            // Check intersection isn't null
            if (intMin.x < intMax.x && intMin.y < intMax.y )
            {
                return AxisAlignedBox(intMin, intMax);
            }

            return AxisAlignedBox();
		}

		/// Calculate the volume of this box
		Real volume(void) const
		{
			switch (mExtent)
			{
			case EXTENT_NULL:
				return 0.0f;

			case EXTENT_FINITE:
				{
					Vector2 diff = mMaximum - mMinimum;
					return diff.x * diff.y;
				}

			case EXTENT_INFINITE:
				return Math::POS_INFINITY;

			default: // shut up compiler
				mrcASSERT( false && "Never reached" );
				return 0.0f;
			}
		}

		/** Scales the AABB by the vector given. */
		inline void scale(const Vector2& s)
		{
			// Do nothing if current null or infinite
			if (mExtent != EXTENT_FINITE)
				return;

			// NB assumes centered on origin
			Vector2 min = mMinimum * s;
			Vector2 max = mMaximum * s;
			setExtents(min, max);
		}

		/** Tests whether this box intersects a sphere. */
		bool intersects(const Sphere& s) const
		{
			return Math::intersects(s, *this); 
		}
		/** Tests whether this box intersects a plane. */
// 		bool intersects(const Plane& p) const
// 		{
// 			return Math::intersects(p, *this);
// 		}
		/** Tests whether the vector point is within this box. */
		bool intersects(const Vector2& v) const
		{
			switch (mExtent)
			{
			case EXTENT_NULL:
				return false;

			case EXTENT_FINITE:
				return(v.x >= mMinimum.x  &&  v.x <= mMaximum.x  && 
					v.y >= mMinimum.y  &&  v.y <= mMaximum.y );

			case EXTENT_INFINITE:
				return true;

			default: // shut up compiler
				mrcASSERT( false && "Never reached" );
				return false;
			}
		}
		/// Gets the centre of the box
		Vector2 getCenter(void) const
		{
			mrcASSERT( (mExtent == EXTENT_FINITE) && "Can't get center of a null or infinite AAB" );
			return Vector2(	(mMaximum.x + mMinimum.x) * 0.5f,(mMaximum.y + mMinimum.y) * 0.5f);
		}
		/// Gets the size of the box
		Vector2 getSize(void) const
		{
			switch (mExtent)
			{
			case EXTENT_NULL:
				return Vector2::ZERO;

			case EXTENT_FINITE:
				return mMaximum - mMinimum;

			case EXTENT_INFINITE:
				return Vector2(
					Math::POS_INFINITY,
					Math::POS_INFINITY );

			default: // shut up compiler
				mrcASSERT( false && "Never reached" );
				return Vector2::ZERO;
			}
		}
		/// Gets the half-size of the box
		Vector2 getHalfSize(void) const
		{
			switch (mExtent)
			{
			case EXTENT_NULL:
				return Vector2::ZERO;

			case EXTENT_FINITE:
				return (mMaximum - mMinimum) * 0.5;

			case EXTENT_INFINITE:
				return Vector2(
					Math::POS_INFINITY,
					Math::POS_INFINITY );

			default: // shut up compiler
				mrcASSERT( false && "Never reached" );
				return Vector2::ZERO;
			}
		}

        /** Tests whether the given point contained by this box.
        */
        bool contains(const Vector2& v) const
        {
            if (isNull())
                return false;
            if (isInfinite())
                return true;

            return mMinimum.x <= v.x && v.x <= mMaximum.x &&
                   mMinimum.y <= v.y && v.y <= mMaximum.y;
        }

        /** Tests whether another box contained by this box.
        */
        bool contains(const AxisAlignedBox& other) const
        {
            if (other.isNull() || this->isInfinite())
                return true;

            if (this->isNull() || other.isInfinite())
                return false;

            return this->mMinimum.x <= other.mMinimum.x &&
                   this->mMinimum.y <= other.mMinimum.y &&
                   other.mMaximum.x <= this->mMaximum.x &&
                   other.mMaximum.y <= this->mMaximum.y;
        }

        /** Tests 2 boxes for equality.
        */
        bool operator== (const AxisAlignedBox& rhs) const
        {
            if (this->mExtent != rhs.mExtent)
                return false;

            if (!this->isFinite())
                return true;

            return this->mMinimum == rhs.mMinimum &&
                   this->mMaximum == rhs.mMaximum;
        }

        /** Tests 2 boxes for inequality.
        */
        bool operator!= (const AxisAlignedBox& rhs) const
        {
            return !(*this == rhs);
        }

		// special values
		static const AxisAlignedBox BOX_NULL;
		static const AxisAlignedBox BOX_INFINITE;


	};

	/** @} */
	/** @} */
} // namespace Ogre

#undef Real

#endif
