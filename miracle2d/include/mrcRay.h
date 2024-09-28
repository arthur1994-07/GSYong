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
#ifndef _mrcRay_h_
#define _mrcRay_h_

// Precompiler options
#include "mrcConfig.h"

#include "mrcVector2.h"
//#include "mrcPlaneBoundedVolume.h"

#define Real real

namespace mrc {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/
	/** Representation of a ray in space, i.e. a line with an origin and direction. */
    class  Ray
    {
    protected:
        Vector2 mOrigin;
        Vector2 mDirection;
    public:
        Ray():mOrigin(Vector2::ZERO), mDirection(Vector2::UNIT_X) {}
        Ray(const Vector2& origin, const Vector2& direction)
            :mOrigin(origin), mDirection(direction) {}

        /** Sets the origin of the ray. */
        void setOrigin(const Vector2& origin) {mOrigin = origin;} 
        /** Gets the origin of the ray. */
        const Vector2& getOrigin(void) const {return mOrigin;} 

        /** Sets the direction of the ray. */
        void setDirection(const Vector2& dir) {mDirection = dir;} 
        /** Gets the direction of the ray. */
        const Vector2& getDirection(void) const {return mDirection;} 

		/** Gets the position of a point t units along the ray. */
		Vector2 getPoint(Real t) const { 
			return Vector2(mOrigin + (mDirection * t));
		}
		
		/** Gets the position of a point t units along the ray. */
		Vector2 operator*(Real t) const { 
			return getPoint(t);
		}

		/** Tests whether this ray intersects the given plane. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
// 		std::pair<bool, Real> intersects(const Plane& p) const
// 		{
// 			return Math::intersects(*this, p);
// 		}
        /** Tests whether this ray intersects the given plane bounded volume. 
        @returns A pair structure where the first element indicates whether
        an intersection occurs, and if true, the second element will
        indicate the distance along the ray at which it intersects. 
        This can be converted to a point in space by calling getPoint().
        */
//         std::pair<bool, Real> intersects(const PlaneBoundedVolume& p) const
//         {
//             return Math::intersects(*this, p.planes, p.outside == Plane::POSITIVE_SIDE);
//         }
		/** Tests whether this ray intersects the given sphere. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, Real> intersects(const Sphere& s) const
		{
			return Math::intersects(*this, s);
		}
		/** Tests whether this ray intersects the given box. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, Real> intersects(const AxisAlignedBox& box) const
		{
			return Math::intersects(*this, box);
		}

    };
	/** @} */
	/** @} */

}

#undef Real

#endif
