// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef PX_PHYSICS_COMMON_VECMATH_SCALAR_INLINE
#define PX_PHYSICS_COMMON_VECMATH_SCALAR_INLINE

#if COMPILE_VECTOR_INTRINSICS
#error Scalar version should not be included when using vector intrinsics.
#endif

/////////////////////////////////////////////////////////////////////
////FUNCTIONS USED ONLY FOR ASSERTS IN VECTORISED IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////


PX_FORCE_INLINE bool isValidVec3V(const Vec3V a)
{
	return a.pad == 0.f;
}

PX_FORCE_INLINE bool hasZeroElementInFloatV(const FloatV a)
{
	return (0==a.x);
}

PX_FORCE_INLINE bool hasZeroElementInVec3V(const Vec3V a)
{
	return (0==a.x || 0==a.y || 0==a.z);
}

PX_FORCE_INLINE bool hasZeroElementInVec4V(const Vec4V a)
{
	return (0==a.x || 0==a.y || 0==a.z || 0==a.w);
}

PX_FORCE_INLINE bool isFiniteVec3V(const Vec3V a)
{
	return PxIsFinite(a.x) && PxIsFinite(a.y) && PxIsFinite(a.z);
}

PX_FORCE_INLINE bool isFiniteVec4V(const Vec4V a)
{
	return PxIsFinite(a.x) && PxIsFinite(a.y) && PxIsFinite(a.z) && PxIsFinite(a.w);
}

/////////////////////////////////////////////////////////////////////
////VECTORISED FUNCTION IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////


PX_FORCE_INLINE FloatV FloatV_From_F32(const PxF32 f)			
{
	return FloatV(f);
}

PX_FORCE_INLINE Vec3V Vec3V_From_F32(const PxF32 f)			
{
	return Vec3V(f,f,f);
}

PX_FORCE_INLINE Vec4V Vec4V_From_F32(const PxF32 f)			
{
	return Vec4V(f,f,f,f);
}

PX_FORCE_INLINE BoolV BoolV_From_Bool32(const bool f)			
{
#if defined(PX_ARM)
    // SD: Android ARM builds fail if this is done with a cast. 
    // Might also fail because of something else but the select 
    // operator here seems to fix everything that failed in release builds.
	return f ? BTTTT() : BFFFF();
#else
	PxU32 i=-(PxI32)f;
	return BoolV(i,i,i,i);
#endif
}

PX_FORCE_INLINE Vec3V Vec3V_From_PxVec3_Aligned(const PxVec3& f)
{
	VECMATHAOS_ASSERT(0 == (reinterpret_cast<PxU64>(&f) & 0x0f));
	return Vec3V(f.x,f.y,f.z);
}

PX_FORCE_INLINE Vec3V Vec3V_From_PxVec3(const PxVec3& f)		
{
	return Vec3V(f.x,f.y,f.z);
}

PX_FORCE_INLINE Vec3V Vec3V_From_PxVec3_WUndefined(const PxVec3& f)		
{
	return Vec3V(f.x,f.y,f.z);
}

PX_FORCE_INLINE Vec3V Vec3V_From_Vec4V(Vec4V f)
{
	return Vec3V(f.x,f.y,f.z);
}

PX_FORCE_INLINE Vec3V Vec3V_From_Vec4V_WUndefined(const Vec4V v)
{
	return Vec3V(v.x, v.y, v.z);
}

PX_FORCE_INLINE Vec4V Vec4V_From_Vec3V(Vec3V f)
{
	return Vec4V(f.x,f.y,f.z, 0.0f);
}


PX_FORCE_INLINE Vec4V Vec4V_From_F32Array_Aligned(const PxF32* const f)	
{
	VECMATHAOS_ASSERT(0 == ((PxU64)f & 0x0f));
	return Vec4V(f[0],f[1],f[2],f[3]);
}

PX_FORCE_INLINE void F32Array_Aligned_From_Vec4V(const Vec4V a, PxF32* f)
{
	VECMATHAOS_ASSERT(0 == ((PxU64)f & 0x0f));
	*reinterpret_cast<Vec4V*>(f) = a;
}

PX_FORCE_INLINE void PxU32Array_Aligned_From_BoolV(const BoolV a, PxU32* f)
{
	VECMATHAOS_ASSERT(0 == ((PxU64)f & 0x0f));
	*reinterpret_cast<BoolV*>(f) = a;
}

PX_FORCE_INLINE Vec4V Vec4V_From_F32Array(const PxF32* const f)	
{
	return Vec4V(f[0],f[1],f[2],f[3]);
}

PX_FORCE_INLINE Vec4V Vec4V_From_PxVec3_WUndefined(const PxVec3& f)		
{
	return Vec4V(f[0],f[1],f[2],0.f); 
}

PX_FORCE_INLINE BoolV BoolV_From_Bool32Array(const bool* const f)	
{
	return BoolV(-(PxI32)f[0],-(PxI32)f[1],-(PxI32)f[2],-(PxI32)f[3]);
}


PX_FORCE_INLINE PxF32 PxF32_From_FloatV(const FloatV a)		
{
	return a.x;
}

PX_FORCE_INLINE void PxF32_From_FloatV(const FloatV a, PxF32* PX_RESTRICT f)		
{
	*f = a.x;
}

PX_FORCE_INLINE void PxVec3Aligned_From_Vec3V(const Vec3V a, PxVec3& f)
{
	f=PxVec3(a.x,a.y,a.z);
}

PX_FORCE_INLINE void PxVec3_From_Vec3V(const Vec3V a, PxVec3& f)
{
	f=PxVec3(a.x,a.y,a.z);
}

PX_FORCE_INLINE bool _VecMathTests::allElementsEqualFloatV(const FloatV a, const FloatV b)
{
	return (a.x==b.x);
	
}
PX_FORCE_INLINE bool _VecMathTests::allElementsEqualVec3V(const Vec3V a, const Vec3V b)
{
	return (a.x==b.x && a.y==b.y && a.z==b.z);
}

PX_FORCE_INLINE bool _VecMathTests::allElementsEqualVec4V(const Vec4V a, const Vec4V b)
{
	return (a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w);
}

PX_FORCE_INLINE bool _VecMathTests::allElementsEqualBoolV(const BoolV a, const BoolV b)
{
	return (a.ux==b.ux && a.uy==b.uy && a.uz==b.uz && a.uw==b.uw);
}

#define VECMATH_AOS_EPSILON (1e-3f)

PX_FORCE_INLINE bool _VecMathTests::allElementsNearEqualFloatV(const FloatV a, const FloatV b)
{
	const PxF32 cx=a.x-b.x;
	return (cx>-VECMATH_AOS_EPSILON && cx<VECMATH_AOS_EPSILON);
}

PX_FORCE_INLINE bool _VecMathTests::allElementsNearEqualVec3V(const Vec3V a, const Vec3V b)
{
	const PxF32 cx=a.x-b.x;
	const PxF32 cy=a.y-b.y;
	const PxF32 cz=a.z-b.z;
	return 
	(
		cx>-VECMATH_AOS_EPSILON && cx<VECMATH_AOS_EPSILON &&
		cy>-VECMATH_AOS_EPSILON && cy<VECMATH_AOS_EPSILON &&
		cz>-VECMATH_AOS_EPSILON && cz<VECMATH_AOS_EPSILON
	);
}

PX_FORCE_INLINE bool _VecMathTests::allElementsNearEqualVec4V(const Vec4V a, const Vec4V b)
{
	const PxF32 cx=a.x-b.x;
	const PxF32 cy=a.y-b.y;
	const PxF32 cz=a.z-b.z;
	const PxF32 cw=a.w-b.w;
	return 
	(
		cx>-VECMATH_AOS_EPSILON && cx<VECMATH_AOS_EPSILON &&
		cy>-VECMATH_AOS_EPSILON && cy<VECMATH_AOS_EPSILON &&
		cz>-VECMATH_AOS_EPSILON && cz<VECMATH_AOS_EPSILON &&
		cw>-VECMATH_AOS_EPSILON && cw<VECMATH_AOS_EPSILON
	);
}

//////////////////////////
//FLOATV
//////////////////////////

PX_FORCE_INLINE FloatV FZero()
{
	return FloatV_From_F32(0.0f);
}

PX_FORCE_INLINE FloatV FOne()
{
	return FloatV_From_F32(1.0f);
}

PX_FORCE_INLINE FloatV FHalf()
{
	return FloatV_From_F32(0.5f);
}

PX_FORCE_INLINE FloatV FEps()
{
	return FloatV_From_F32(PX_EPS_REAL);
}

PX_FORCE_INLINE FloatV FMax()
{
	return FloatV_From_F32(PX_MAX_REAL);
}

PX_FORCE_INLINE FloatV FNegMax()
{
	return FloatV_From_F32(-PX_MAX_REAL);
}

PX_FORCE_INLINE FloatV IZero()
{
	const PxU32 zero = 0;
	return FloatV_From_F32((PxF32&)zero);
}

PX_FORCE_INLINE FloatV IOne()
{
	const PxU32 one = 1;
	return FloatV_From_F32((PxF32&)one);
}

PX_FORCE_INLINE FloatV ITwo()
{
	const PxU32 two = 2;
	return FloatV_From_F32((PxF32&)two);
}

PX_FORCE_INLINE FloatV IThree()
{
	const PxU32 three = 3;
	return FloatV_From_F32((PxF32&)three);
}

PX_FORCE_INLINE FloatV IFour()
{
	const PxU32 four = 4;
	return FloatV_From_F32((PxF32&)four);
}

PX_FORCE_INLINE FloatV FNeg(const FloatV f)									
{
	return FloatV(-f.x);
}

PX_FORCE_INLINE FloatV FAdd(const FloatV a, const FloatV b)					
{
	return FloatV(a.x+b.x);
}

PX_FORCE_INLINE FloatV FSub(const FloatV a, const FloatV b)				
{
	return FloatV(a.x-b.x);
}

PX_FORCE_INLINE FloatV FMul(const FloatV a, const FloatV b)					
{
	return FloatV(a.x*b.x);
}

PX_FORCE_INLINE FloatV FDiv(const FloatV a, const FloatV b)				
{
	VECMATHAOS_ASSERT(!HasZeroElementInFloatV(b));
	return FloatV(a.x/b.x);
}

PX_FORCE_INLINE FloatV FDivFast(const FloatV a, const FloatV b)			
{
	VECMATHAOS_ASSERT(!HasZeroElementInFloatV(b));
	return FloatV(a.x/b.x);
}

PX_FORCE_INLINE FloatV FRecip(const FloatV a)
{
	VECMATHAOS_ASSERT(!HasZeroElementInFloatV(a));
	return (1.0f/a.x);
}

PX_FORCE_INLINE FloatV FRecipFast(const FloatV a)
{
	VECMATHAOS_ASSERT(!HasZeroElementInFloatV(a));
	return (1.0f/a.x);
}

PX_FORCE_INLINE FloatV FRsqrt(const FloatV a)
{
	VECMATHAOS_ASSERT(!HasZeroElementInFloatV(a));
	return PxRecipSqrt(a.x);
}

PX_FORCE_INLINE FloatV FSqrt(const FloatV a)
{
	VECMATHAOS_ASSERT(!HasZeroElementInFloatV(a));
	return PxSqrt(a.x);
}

PX_FORCE_INLINE FloatV FRsqrtFast(const FloatV a)
{
	VECMATHAOS_ASSERT(!HasZeroElementInFloatV(a));
	return PxRecipSqrt(a.x);
}

PX_FORCE_INLINE FloatV FScaleAdd(const FloatV a, const FloatV b, const FloatV c)
{
	return FAdd(FMul(a,b),c);
}

PX_FORCE_INLINE FloatV FNegScaleSub(const FloatV a, const FloatV b, const FloatV c)
{
	return FSub(c,FMul(a,b));
}

PX_FORCE_INLINE FloatV FAbs(const FloatV a)			
{
	return FloatV(PxAbs(a.x));
}

PX_FORCE_INLINE FloatV FSel(const BoolV c, const FloatV a, const FloatV b)	
{
	return FloatV(c.ux ? a.x : b.x);
}  

PX_FORCE_INLINE BoolV FIsGrtr(const FloatV a, const FloatV b)
{
	return BoolV_From_Bool32(a.x>b.x);
}

PX_FORCE_INLINE BoolV FIsGrtrOrEq(const FloatV a, const FloatV b)
{
	return BoolV_From_Bool32(a.x>=b.x);
}

PX_FORCE_INLINE BoolV FIsEq(const FloatV a, const FloatV b)
{
	return BoolV_From_Bool32(a.x==b.x);
}

PX_FORCE_INLINE FloatV FMax(const FloatV a, const FloatV b)
{
	return (a.x>b.x ? FloatV(a.x) : FloatV(b.x));
}

PX_FORCE_INLINE FloatV FMin(const FloatV a, const FloatV b)
{
	return (a.x>b.x ? FloatV(b.x) : FloatV(a.x));
}

PX_FORCE_INLINE FloatV FClamp(const FloatV a, const FloatV minV, const FloatV maxV)
{
	return FMax(FMin(a,maxV),minV);
}

PX_FORCE_INLINE PxU32 FAllGrtr(const FloatV a, const FloatV b)
{
	return (a.x > b.x); 
}

PX_FORCE_INLINE PxU32 FAllGrtrOrEq(const FloatV a, const FloatV b)
{
	return (a.x >= b.x);
}
PX_FORCE_INLINE PxU32 FAllEq(const FloatV a, const FloatV b)
{
	return(a.x == b.x);
}

PX_FORCE_INLINE FloatV FRound(const FloatV a)
{
	return floor(a.x + 0.5f);
}

PX_FORCE_INLINE FloatV FSin(const FloatV a)
{
	return sinf(a.x);
}
PX_FORCE_INLINE FloatV FCos(const FloatV a)
{
	return cosf(a.x);
}

PX_FORCE_INLINE PxU32 FOutOfBounds(const FloatV a, const FloatV min, const FloatV max)
{
	return (a.x>max.x || a.x<min.x);
}

PX_FORCE_INLINE PxU32 FInBounds(const FloatV a, const FloatV min, const FloatV max)
{
	return (a.x>=min.x && a.x<=max.x);
}

PX_FORCE_INLINE PxU32 FOutOfBounds(const FloatV a, const FloatV bounds)
{
	return FOutOfBounds(a, FNeg(bounds), bounds);
}

PX_FORCE_INLINE PxU32 FInBounds(const FloatV a, const FloatV bounds)
{
	return FInBounds(a, FNeg(bounds), bounds);
}


/////////////////////
//VEC3V
/////////////////////

PX_FORCE_INLINE Vec3V V3Splat(const FloatV f) 
{
	return Vec3V(f.x,f.x,f.x);
}

PX_FORCE_INLINE Vec3V V3Merge(const FloatVArg x, const FloatVArg y, const FloatVArg z)
{
	return Vec3V(x.x,y.x,z.x);
} 

PX_FORCE_INLINE Vec3V V3UnitX()
{
	return Vec3V(1.0f,0.0f,0.0f);
}

PX_FORCE_INLINE Vec3V V3UnitY()
{
	return Vec3V(0.0f,1.0f,0.0f);
}

PX_FORCE_INLINE Vec3V V3UnitZ()
{
	return Vec3V(0.0f,0.0f,1.0f);
}

PX_FORCE_INLINE FloatV V3GetX(const Vec3V f) 
{
	return FloatV(f.x);
}

PX_FORCE_INLINE FloatV V3GetY(const Vec3V f) 
{
	return FloatV(f.y);
}

PX_FORCE_INLINE FloatV V3GetZ(const Vec3V f) 
{
	return FloatV(f.z);
}

PX_FORCE_INLINE Vec3V V3SetX(const Vec3V v, const FloatV f) 
{
	return Vec3V(f.x,v.y,v.z);
}

PX_FORCE_INLINE Vec3V V3SetY(const Vec3V v, const FloatV f) 
{
	return Vec3V(v.x,f.x,v.z);
}

PX_FORCE_INLINE Vec3V V3SetZ(const Vec3V v, const FloatV f) 
{
	return Vec3V(v.x,v.y,f.x);
}

PX_FORCE_INLINE Vec3V V3ColX(const Vec3V a, const Vec3V b, const Vec3V c)
{
	return Vec3V(a.x,b.x,c.x);
}

PX_FORCE_INLINE Vec3V V3ColY(const Vec3V a, const Vec3V b, const Vec3V c)
{
	return Vec3V(a.y,b.y,c.y);
}

PX_FORCE_INLINE Vec3V V3ColZ(const Vec3V a, const Vec3V b, const Vec3V c)
{
	return Vec3V(a.z,b.z,c.z);
}

PX_FORCE_INLINE Vec3V V3Zero()
{
	return Vec3V_From_F32(0.0f);
}

PX_FORCE_INLINE Vec3V V3One()
{
	return Vec3V_From_F32(1.0f);
}


PX_FORCE_INLINE Vec3V V3Neg(const Vec3V c)					
{
	return Vec3V(-c.x,-c.y,-c.z);
}

PX_FORCE_INLINE Vec3V V3Add(const Vec3V a, const Vec3V b)		
{
	return Vec3V(a.x+b.x,a.y+b.y,a.z+b.z);
}

PX_FORCE_INLINE Vec3V V3Sub(const Vec3V a, const Vec3V b)	
{
	return Vec3V(a.x-b.x,a.y-b.y,a.z-b.z);
}

PX_FORCE_INLINE Vec3V V3Scale(const Vec3V a, const FloatV b)	
{
	return Vec3V(a.x*b.x,a.y*b.x,a.z*b.x);
}

PX_FORCE_INLINE Vec3V V3Mul(const Vec3V a, const Vec3V b)	
{
	return Vec3V(a.x*b.x,a.y*b.y,a.z*b.z);
}

PX_FORCE_INLINE Vec3V V3ScaleInv(const Vec3V a, const FloatV b)	
{
	const PxF32 bInv=1.0f/b.x;
	return Vec3V(a.x*bInv,a.y*bInv,a.z*bInv);
}

PX_FORCE_INLINE Vec3V V3Div(const Vec3V a, const Vec3V b)		
{
	return Vec3V(a.x/b.x,a.y/b.y,a.z/b.z);
}

PX_FORCE_INLINE Vec3V V3ScaleInvFast(const Vec3V a, const FloatV b)	
{
	const PxF32 bInv=1.0f/b.x;
	return Vec3V(a.x*bInv,a.y*bInv,a.z*bInv);
}

PX_FORCE_INLINE Vec3V V3DivFast(const Vec3V a, const Vec3V b)		
{
	return Vec3V(a.x/b.x,a.y/b.y,a.z/b.z);
}

PX_FORCE_INLINE Vec3V V3Recip(const Vec3V a)
{
	return Vec3V(1.0f/a.x,1.0f/a.y,1.0f/a.z);
}

PX_FORCE_INLINE Vec3V V3RecipFast(const Vec3V a)
{
	return Vec3V(1.0f/a.x,1.0f/a.y,1.0f/a.z);
}

PX_FORCE_INLINE Vec3V V3Rsqrt(const Vec3V a)
{
	return Vec3V(PxRecipSqrt(a.x),PxRecipSqrt(a.y),PxRecipSqrt(a.z));
}		

PX_FORCE_INLINE Vec3V V3RsqrtFast(const Vec3V a)
{
	return Vec3V(PxRecipSqrt(a.x),PxRecipSqrt(a.y),PxRecipSqrt(a.z));
}

PX_FORCE_INLINE Vec3V V3ScaleAdd(const Vec3V a, const FloatV b, const Vec3V c)
{
	return V3Add(V3Scale(a,b),c);
}

PX_FORCE_INLINE Vec3V V3NegScaleSub(const Vec3V a, const FloatV b, const Vec3V c)
{
	return V3Sub(c,V3Scale(a,b));
}

PX_FORCE_INLINE Vec3V V3MulAdd(const Vec3V a, const Vec3V b, const Vec3V c)
{
	return V3Add(V3Mul(a,b),c);
}

PX_FORCE_INLINE Vec3V V3NegMulSub(const Vec3V a, const Vec3V b, const Vec3V c)
{
	return V3Sub(c,V3Mul(a,b));
}

PX_FORCE_INLINE FloatV V3Dot(const Vec3V a, const Vec3V b)	
{
	return FloatV(a.x*b.x+a.y*b.y+a.z*b.z);
}

PX_FORCE_INLINE Vec3V V3Cross(const Vec3V a, const Vec3V b)	
{
	return Vec3V
	(
	a.y*b.z-a.z*b.y, 
	a.z*b.x-a.x*b.z,
	a.x*b.y-a.y*b.x
	);
}

PX_FORCE_INLINE FloatV V3Length(const Vec3V a)
{
	return FloatV(PxSqrt(a.x*a.x + a.y*a.y + a.z*a.z));
}

PX_FORCE_INLINE FloatV V3LengthSq(const Vec3V a)
{
	return FloatV(a.x*a.x + a.y*a.y + a.z*a.z);
}

PX_FORCE_INLINE Vec3V V3Normalize(const Vec3V a)
{
	VECMATHAOS_ASSERT(a.x!=0 || a.y!=0 || a.z!=0);
	const PxF32 lengthInv=1.0f/(PxSqrt(a.x*a.x + a.y*a.y + a.z*a.z));
	return Vec3V(a.x*lengthInv,a.y*lengthInv,a.z*lengthInv);
}

PX_FORCE_INLINE Vec3V V3NormalizeSafe(const Vec3V a)
{
	const PxF32 length=PxSqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	if(PX_EPS_REAL >= length)
	{
		return Vec3V(0.0f,0.0f,0.0f);
	}
	else
	{
		const PxF32 lengthInv=1.0f/length;
		return Vec3V(a.x*lengthInv,a.y*lengthInv,a.z*lengthInv);
	}
}

PX_FORCE_INLINE Vec3V V3NormalizeFast(const Vec3V a)
{
	VECMATHAOS_ASSERT(a.x!=0 || a.y!=0 || a.z!=0);
	const PxF32 lengthInv=1.0f/(PxSqrt(a.x*a.x + a.y*a.y + a.z*a.z));
	return Vec3V(a.x*lengthInv,a.y*lengthInv,a.z*lengthInv);
}

PX_FORCE_INLINE Vec3V V3Sel(const BoolV c, const Vec3V a, const Vec3V b)
{
	return Vec3V(c.ux ? a.x : b.x, c.uy ? a.y : b.y, c.uz ? a.z : b.z);
}  

PX_FORCE_INLINE BoolV V3IsGrtr(const Vec3V a, const Vec3V b)	
{
	return BoolV(a.x>b.x ? -1 : 0, a.y>b.y ? -1 : 0, a.z>b.z ? -1 : 0, 0);
}

PX_FORCE_INLINE BoolV V3IsGrtrOrEq(const Vec3V a, const Vec3V b)	
{
	return BoolV(a.x>=b.x ? (PxU32)-1 : 0, a.y>=b.y ? (PxU32)-1 : 0, a.z>=b.z ? (PxU32)-1 : 0, (PxU32)-1);
}

PX_FORCE_INLINE BoolV V3IsEq(const Vec3V a, const Vec3V b)
{
	return BoolV(a.x==b.x ? (PxU32)-1 : 0, a.y==b.y ? (PxU32)-1 : 0, a.z==b.z ? (PxU32)-1 : 0, (PxU32)-1);
}

PX_FORCE_INLINE Vec3V V3Max(const Vec3V a, const Vec3V b)				
{
	return Vec3V(a.x>b.x ? a.x : b.x, a.y>b.y ? a.y : b.y, a.z>b.z ? a.z : b.z);
}

PX_FORCE_INLINE Vec3V V3Min(const Vec3V a, const Vec3V b)
{
	return Vec3V(a.x<b.x ? a.x : b.x, a.y<b.y ? a.y : b.y, a.z<b.z ? a.z : b.z);
}

//Extract the maximum value from a
PX_FORCE_INLINE FloatV V3ExtractMax(const Vec3V a)
{
	const PxF32 t0 = (a.x >= a.y) ? a.x : a.y;
	return t0 >= a.z ? t0 : a.z;
}

//Extract the maximum value from a
PX_FORCE_INLINE FloatV V3ExtractMin(const Vec3V a)
{
	const PxF32 t0 = (a.x <= a.y) ? a.x : a.y;
	return t0 <= a.z ? t0 : a.z;
}

//return (a >= 0.0f) ? 1.0f : -1.0f;
PX_FORCE_INLINE Vec3V V3Sign(const Vec3V a)				
{
	return Vec3V((a.x >= 0.f ? 1.f : -1.f), (a.y >= 0.f ? 1.f : -1.f), (a.z >= 0.f ? 1.f : -1.f));	
}

PX_FORCE_INLINE Vec3V V3Clamp(const Vec3V a, const Vec3V minV, const Vec3V maxV)
{
	return V3Max(V3Min(a,maxV),minV);
}

PX_FORCE_INLINE Vec3V V3Abs(const Vec3V a)
{
	return V3Max(a,V3Neg(a));
}

PX_FORCE_INLINE PxU32 V3AllGrtr(const Vec3V a, const Vec3V b)
{
	return ((a.x > b.x) & (a.y > b.y) & (a.z > b.z)) ? 1 : 0;
}

PX_FORCE_INLINE PxU32 V3AllGrtrOrEq(const Vec3V a, const Vec3V b)
{
	return ((a.x >= b.x) & (a.y >= b.y) & (a.z >= b.z)) ? 1 : 0;
}

PX_FORCE_INLINE PxU32 V3AllEq(const Vec3V a, const Vec3V b)
{
	return ((a.x == b.x) & (a.y == b.y) & (a.z == b.z)) ? 1 : 0;
}

PX_FORCE_INLINE Vec3V V3Round(const Vec3V a)
{
	return Vec3V(floor(a.x + 0.5f), floor(a.y + 0.5f), floor(a.z + 0.5f));
}

PX_FORCE_INLINE Vec3V V3Sin(const Vec3V a)
{
	return Vec3V(sinf(a.x), sinf(a.y), sinf(a.z));
}
PX_FORCE_INLINE Vec3V V3Cos(const Vec3V a)
{
	return Vec3V(cosf(a.x), cosf(a.y), cosf(a.z));
}

PX_FORCE_INLINE Vec3V V3PermYZZ(const Vec3V a)
{
	return Vec3V(a.y,a.z,a.z);
}

PX_FORCE_INLINE Vec3V V3PermXYX(const Vec3V a)
{
	return Vec3V(a.x,a.y,a.x);
}

PX_FORCE_INLINE Vec3V V3PermYZX(const Vec3V a)
{
	return Vec3V(a.y,a.z,a.x);
}

PX_FORCE_INLINE Vec3V V3PermZXY(const Vec3V a)
{
	return Vec3V(a.z,a.x,a.y);
}

PX_FORCE_INLINE Vec3V V3PermZZY(const Vec3V a)
{
	return Vec3V(a.z,a.z,a.y);
}

PX_FORCE_INLINE Vec3V V3PermYXX(const Vec3V a)
{
	return Vec3V(a.y,a.x,a.x);
}

PX_FORCE_INLINE Vec3V V3Perm_Zero_1Z_0Y(const Vec3V v0, const Vec3V v1)
{
	return Vec3V(0.0f, v1.z, v0.y);
}

PX_FORCE_INLINE Vec3V V3Perm_0Z_Zero_1X(const Vec3V v0, const Vec3V v1)
{
	return Vec3V(v0.z, 0.0f, v1.x);
}

PX_FORCE_INLINE Vec3V V3Perm_1Y_0X_Zero(const Vec3V v0, const Vec3V v1)
{
	return Vec3V(v1.y, v0.x, 0.0f);
}

PX_FORCE_INLINE FloatV V3SumElems(const Vec3V a)
{
	return FloatV(a.x + a.y + a.z);
}

PX_FORCE_INLINE PxU32 V3OutOfBounds(const Vec3V a, const Vec3V min, const Vec3V max)
{
	return (a.x>max.x || a.y>max.y || a.z>max.z ||
		    a.x<min.x || a.y<min.y || a.z<min.z); 
}

PX_FORCE_INLINE PxU32 V3InBounds(const Vec3V a, const Vec3V min, const Vec3V max)
{
	return (a.x<=max.x && a.y<=max.y && a.z<=max.z && 
		    a.x>=min.x && a.y>=min.y && a.z>=min.z); 
}

PX_FORCE_INLINE PxU32 V3OutOfBounds(const Vec3V a, const Vec3V bounds)
{
	return V3OutOfBounds(a, V3Neg(bounds), bounds);
}

PX_FORCE_INLINE PxU32 V3InBounds(const Vec3V a, const Vec3V bounds)
{
	return V3InBounds(a, V3Neg(bounds), bounds);
}




/////////////////////////
//VEC4V
/////////////////////////

PX_FORCE_INLINE Vec4V V4Splat(const FloatV f) 
{
	return Vec4V(f.x,f.x,f.x,f.x);
}

PX_FORCE_INLINE Vec4V V4Merge(const FloatV* const floatVArray) 
{
	return Vec4V(floatVArray[0].x,floatVArray[1].x,floatVArray[2].x,floatVArray[3].x);
} 

PX_FORCE_INLINE Vec4V V4Merge(const FloatVArg x,const FloatVArg y, const FloatVArg z, const FloatVArg w) 
{
	return Vec4V(x.x,y.x,z.x,w.x);
} 

PX_FORCE_INLINE Vec4V V4MergeW(const Vec4VArg x, const Vec4VArg y, const Vec4VArg z, const Vec4VArg w)
{
	return Vec4V(x.w, y.w, z.w, w.w);
}

PX_FORCE_INLINE Vec4V V4MergeZ(const Vec4VArg x, const Vec4VArg y, const Vec4VArg z, const Vec4VArg w)
{
	return Vec4V(x.z, y.z, z.z, w.z);
}

PX_FORCE_INLINE Vec4V V4MergeY(const Vec4VArg x, const Vec4VArg y, const Vec4VArg z, const Vec4VArg w)
{
	return Vec4V(x.y, y.y, z.y, w.y);
}

PX_FORCE_INLINE Vec4V V4MergeX(const Vec4VArg x, const Vec4VArg y, const Vec4VArg z, const Vec4VArg w)
{
	return Vec4V(x.x, y.x, z.x, w.x);
}

PX_FORCE_INLINE Vec4V V4UnpackXY(const Vec4VArg a, const Vec4VArg b)
{
	return Vec4V(a.x, b.x, a.y, b.y);
}

PX_FORCE_INLINE Vec4V V4UnpackZW(const Vec4VArg a, const Vec4VArg b)
{
	return Vec4V(a.z, b.z, a.w, b.w);
}


PX_FORCE_INLINE Vec4V V4UnitX()
{
	return Vec4V(1.0f,0.0f,0.0f,0.0f);  
}

PX_FORCE_INLINE Vec4V V4UnitY()
{
	return Vec4V(0.0f,1.0f,0.0f,0.0f);  
}

PX_FORCE_INLINE Vec4V V4UnitZ()
{
	return Vec4V(0.0f,0.0f,1.0f,0.0f);  
}

PX_FORCE_INLINE Vec4V V4UnitW()
{
	return Vec4V(0.0f,0.0f,0.0f,1.0f);  
}

PX_FORCE_INLINE FloatV V4GetX(const Vec4V f) 
{
	return FloatV(f.x);
}

PX_FORCE_INLINE FloatV V4GetY(const Vec4V f) 
{
	return FloatV(f.y);
}

PX_FORCE_INLINE FloatV V4GetZ(const Vec4V f) 
{
	return FloatV(f.z);
}

PX_FORCE_INLINE FloatV V4GetW(const Vec4V f) 
{
	return FloatV(f.w);
}

PX_FORCE_INLINE Vec4V V4SetX(const Vec4V v, const FloatV f) 
{
	return Vec4V(f.x,v.y,v.z,v.w);
}

PX_FORCE_INLINE Vec4V V4SetY(const Vec4V v, const FloatV f) 
{
	return Vec4V(v.x,f.x,v.z,v.w);
}

PX_FORCE_INLINE Vec4V V4SetZ(const Vec4V v, const FloatV f) 
{
	return Vec4V(v.x,v.y,f.x,v.w);
}

PX_FORCE_INLINE Vec4V V4SetW(const Vec4V v, const FloatV f) 
{
	return Vec4V(v.x,v.y,v.z,f.x);
}

PX_FORCE_INLINE Vec4V V4SetW(const Vec3V v, const FloatV f) 
{
	return Vec4V(v.x,v.y,v.z,f.x);
}

PX_FORCE_INLINE Vec4V V4Zero()
{
	return Vec4V_From_F32(0.0f);
}

PX_FORCE_INLINE Vec4V V4One()
{
	return Vec4V_From_F32(1.0f);
}

PX_FORCE_INLINE Vec4V V4Eps()
{
	return Vec4V_From_F32(PX_EPS_REAL);
}

PX_FORCE_INLINE Vec4V V4Neg(const Vec4V c)					
{
	return Vec4V(-c.x,-c.y,-c.z,-c.w); 
}

PX_FORCE_INLINE Vec4V V4Add(const Vec4V a, const Vec4V b)		
{
	return Vec4V(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);
} 

PX_FORCE_INLINE Vec4V V4Sub(const Vec4V a, const Vec4V b)	
{
	return Vec4V(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);
}

PX_FORCE_INLINE Vec4V V4Scale(const Vec4V a, const FloatV b)	
{
	return Vec4V(a.x*b.x,a.y*b.x,a.z*b.x,a.w*b.x);
}

PX_FORCE_INLINE Vec4V V4Mul(const Vec4V a, const Vec4V b)	
{
	return Vec4V(a.x*b.x,a.y*b.y,a.z*b.z,a.w*b.w);
}

PX_FORCE_INLINE Vec4V V4ScaleInv(const Vec4V a, const FloatV b)
{
	const PxF32 bInv=1.0f/b.x;
	return Vec4V(a.x*bInv,a.y*bInv,a.z*bInv,a.w*bInv);
}

PX_FORCE_INLINE Vec4V V4Div(const Vec4V a, const Vec4V b)		
{
	VECMATHAOS_ASSERT(b.x!=0 && b.y!=0 && b.z!=0 && b.w!=0);
	return Vec4V(a.x/b.x,a.y/b.y,a.z/b.z,a.w/b.w);
}

PX_FORCE_INLINE Vec4V V4ScaleInvFast(const Vec4V a, const FloatV b)
{
	const PxF32 bInv=1.0f/b.x;
	return Vec4V(a.x*bInv,a.y*bInv,a.z*bInv,a.w*bInv);
}

PX_FORCE_INLINE Vec4V V4DivFast(const Vec4V a, const Vec4V b)		
{
	return Vec4V(a.x/b.x,a.y/b.y,a.z/b.z,a.w/b.w);
}

PX_FORCE_INLINE Vec4V V4Recip(const Vec4V a)
{
	return Vec4V(1.0f/a.x,1.0f/a.y,1.0f/a.z,1.0f/a.w);
}

PX_FORCE_INLINE Vec4V V4RecipFast(const Vec4V a)
{
	return Vec4V(1.0f/a.x,1.0f/a.y,1.0f/a.z,1.0f/a.w);
}

PX_FORCE_INLINE Vec4V V4Rsqrt(const Vec4V a)
{
	return Vec4V(PxRecipSqrt(a.x),PxRecipSqrt(a.y),PxRecipSqrt(a.z),PxRecipSqrt(a.w));
}		

PX_FORCE_INLINE Vec4V V4RsqrtFast(const Vec4V a)
{
	return Vec4V(PxRecipSqrt(a.x),PxRecipSqrt(a.y),PxRecipSqrt(a.z),PxRecipSqrt(a.w));
}

PX_FORCE_INLINE Vec4V V4ScaleAdd(const Vec4V a, const FloatV b, const Vec4V c)
{
	return V4Add(V4Scale(a,b),c);
}

PX_FORCE_INLINE Vec4V V4NegScaleSub(const Vec4V a, const FloatV b, const Vec4V c)
{
	return V4Sub(c,V4Scale(a,b));
}

PX_FORCE_INLINE Vec4V V4MulAdd(const Vec4V a, const Vec4V b, const Vec4V c)
{
	return V4Add(V4Mul(a,b),c);
}

PX_FORCE_INLINE Vec4V V4NegMulSub(const Vec4V a, const Vec4V b, const Vec4V c)
{
	return V4Sub(c,V4Mul(a,b));
}

PX_FORCE_INLINE FloatV V4Dot(const Vec4V a, const Vec4V b)	
{
	return FloatV(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w);
}

PX_FORCE_INLINE FloatV V4Length(const Vec4V a)
{
	return FloatV(PxSqrt(a.x*a.x + a.y*a.y +a.z*a.z + a.w*a.w));
}

PX_FORCE_INLINE FloatV V4LengthSq(const Vec4V a)
{
	return V4Dot(a,a);
}

PX_FORCE_INLINE Vec4V V4Normalize(const Vec4V a)
{
	VECMATHAOS_ASSERT(0!=a.x || 0!=a.y || 0!=a.z || 0!=a.w);
	const FloatV length=FloatV(V4Length(a));
	return V4ScaleInv(a,length);
}

PX_FORCE_INLINE Vec4V V4NormalizeSafe(const Vec4V a)
{
	const FloatV length=FloatV(V4Length(a));
	if(PX_EPS_REAL>=length.x)
	{
		return Vec4V(0.0f,0.0f,0.0f,0.0f);
	}
	else
	{
		return V4ScaleInv(a,length);
	}
}
PX_FORCE_INLINE Vec4V V4NormalizeFast(const Vec4V a)
{
	VECMATHAOS_ASSERT(0!=a.x || 0!=a.y || 0!=a.z || 0!=a.w);
	const FloatV length=FloatV(V4Length(a));
	return V4ScaleInv(a,length);
}

PX_FORCE_INLINE Vec4V V4Sel(const BoolV c, const Vec4V a, const Vec4V b)	
{
	return Vec4V(c.ux ? a.x : b.x, c.uy ? a.y : b.y, c.uz ? a.z : b.z, c.uw ? a.w : b.w);
}

PX_FORCE_INLINE BoolV V4IsGrtr(const Vec4V a, const Vec4V b)			
{
	return BoolV(a.x>b.x ? -1 : 0, a.y>b.y ? -1 : 0, a.z>b.z ? -1 : 0, a.w>b.w ? -1 : 0);
};

PX_FORCE_INLINE BoolV V4IsGrtrOrEq(const Vec4V a, const Vec4V b)
{
	return BoolV(a.x>=b.x ? -1 : 0, a.y>=b.y ? -1 : 0, a.z>=b.z ? -1 : 0, a.w>=b.w ? -1 : 0);
}

PX_FORCE_INLINE BoolV V4IsEq(const Vec4V a, const Vec4V b)
{
	return BoolV(a.x==b.x ? -1 : 0, a.y==b.y ? -1 : 0, a.z==b.z ? -1 : 0, a.w==b.w ? -1 : 0);
}

PX_FORCE_INLINE Vec4V V4Max(const Vec4V a, const Vec4V b)	
{
	return Vec4V(a.x>b.x ? a.x : b.x, a.y>b.y ? a.y : b.y, a.z>b.z ? a.z : b.z, a.w>b.w ? a.w : b.w);
}

PX_FORCE_INLINE Vec4V V4Min(const Vec4V a, const Vec4V b)		
{
	return Vec4V(a.x<b.x ? a.x : b.x, a.y<b.y ? a.y : b.y, a.z<b.z ? a.z : b.z, a.w<b.w ? a.w : b.w);
}

//Extract the maximum value from a
PX_FORCE_INLINE FloatV V4ExtractMax(const Vec4V a)
{
	const PxF32 t0 = (a.x >= a.y) ? a.x : a.y;
	const PxF32 t1 = (a.z >= a.w) ? a.x : a.w;
	return t0 >= t1 ? t0 : t1;
}

//Extract the maximum value from a
PX_FORCE_INLINE FloatV V4ExtractMin(const Vec4V a)
{
	const PxF32 t0 = (a.x <= a.y) ? a.x : a.y;
	const PxF32 t1 = (a.z <= a.w) ? a.x : a.w;
	return t0 <= t1 ? t0 : t1;
}

PX_FORCE_INLINE Vec4V V4Clamp(const Vec4V a, const Vec4V minV, const Vec4V maxV)
{
	return V4Max(V4Min(a,maxV),minV);
}

PX_FORCE_INLINE PxU32 V4AllGrtr(const Vec4V a, const Vec4V b)
{
	return ((a.x > b.x) & (a.y > b.y) & (a.z > b.z) & (a.w > b.w)) ? 1 : 0;
}

PX_FORCE_INLINE PxU32 V4AllGrtrOrEq(const Vec4V a, const Vec4V b)
{
	return ((a.x >= b.x) & (a.y >= b.y) & (a.z >= b.z) & (a.w >= b.w)) ? 1 : 0;
}

PX_FORCE_INLINE PxU32 V4AllEq(const Vec4V a, const Vec4V b)
{
	return ((a.x == b.x) & (a.y == b.y) & (a.z == b.z) & (a.w == b.w)) ? 1 : 0;
}

PX_FORCE_INLINE Vec4V V4Round(const Vec4V a)
{
	return Vec4V(floor(a.x + 0.5f), floor(a.y + 0.5f), floor(a.z + 0.5f), floor(a.w + 0.5f));
}

PX_FORCE_INLINE Vec4V V4Sin(const Vec4V a)
{
	return Vec4V(sinf(a.x), sinf(a.y), sinf(a.z), sinf(a.w));
}
PX_FORCE_INLINE Vec4V V4Cos(const Vec4V a)
{
	return Vec4V(cosf(a.x), cosf(a.y), cosf(a.z), cosf(a.w));
}

PX_FORCE_INLINE BoolV BFFFF() 
{
	return BoolV(0, 0, 0, 0); 
}									
PX_FORCE_INLINE BoolV BFFFT() 
{
	return BoolV(0, 0, 0, (PxU32)-1); 
}							
PX_FORCE_INLINE BoolV BFFTF() 
{
	return BoolV(0, 0, (PxU32)-1, 0); 
}
PX_FORCE_INLINE BoolV BFFTT() 
{
	return BoolV(0, 0, (PxU32)-1, (PxU32)-1); 
}
PX_FORCE_INLINE BoolV BFTFF() 
{
	return BoolV(0, (PxU32)-1, 0, 0); 
}
PX_FORCE_INLINE BoolV BFTFT() 
{
	return BoolV(0, (PxU32)-1, 0, (PxU32)-1); 
}				
PX_FORCE_INLINE BoolV BFTTF()
{
	return BoolV(0, (PxU32)-1, (PxU32)-1, 0); 
}
PX_FORCE_INLINE BoolV BFTTT()
{
	return BoolV(0, (PxU32)-1, (PxU32)-1, (PxU32)-1); 
}
PX_FORCE_INLINE BoolV BTFFF()
{
	return BoolV((PxU32)-1, 0, 0, 0); 
}
PX_FORCE_INLINE BoolV BTFFT()
{
	return BoolV((PxU32)-1, 0, 0, (PxU32)-1); 
}
PX_FORCE_INLINE BoolV BTFTF()
{
	return BoolV ((PxU32)-1, 0, (PxU32)-1, 0); 
}
PX_FORCE_INLINE BoolV BTFTT() 
{
	return BoolV((PxU32)-1, 0, (PxU32)-1, (PxU32)-1); 
}	
PX_FORCE_INLINE BoolV BTTFF()
{
	return BoolV((PxU32)-1, (PxU32)-1, 0, 0); 
}
PX_FORCE_INLINE BoolV BTTFT()
{
	return BoolV((PxU32)-1, (PxU32)-1, 0, (PxU32)-1); 
}
PX_FORCE_INLINE BoolV BTTTF()
{
	return BoolV((PxU32)-1, (PxU32)-1, (PxU32)-1, 0); 
}
PX_FORCE_INLINE BoolV BTTTT() 
{
	return BoolV((PxU32)-1, (PxU32)-1, (PxU32)-1, (PxU32)-1); 
}

PX_FORCE_INLINE BoolV BXMask() {return BTFFF();}
PX_FORCE_INLINE BoolV BYMask() {return BFTFF();}
PX_FORCE_INLINE BoolV BZMask() {return BFFTF();}
PX_FORCE_INLINE BoolV BWMask() {return BFFFT();}

PX_FORCE_INLINE BoolV BGetX(const BoolV a)
{
	return BoolV(a.ux, a.ux, a.ux, a.ux);
}

PX_FORCE_INLINE BoolV BGetY(const BoolV a)
{
	return BoolV(a.uy, a.uy, a.uy, a.uy);
}

PX_FORCE_INLINE BoolV BGetZ(const BoolV a)
{
	return BoolV(a.uz, a.uz, a.uz, a.uz);
}

PX_FORCE_INLINE BoolV BGetW(const BoolV a)
{
	return BoolV(a.uw, a.uw, a.uw, a.uw);
}

PX_FORCE_INLINE BoolV BAnd(const BoolV a, const BoolV b)	
{
	return BoolV(a.ux && b.ux ? (PxU32)-1 : 0,  a.uy && b.uy ? (PxU32)-1 : 0, a.uz && b.uz ? (PxU32)-1 : 0, a.uw && b.uw ? (PxU32)-1 : 0);
}

PX_FORCE_INLINE BoolV BAndNot(const BoolV a, const BoolV b)	
{
	return BoolV(a.ux & ~b.ux, a.uy & ~b.uy, a.uz & ~b.uz, a.uw & ~b.uw);
}

PX_FORCE_INLINE BoolV BNot(const BoolV a)
{
	return BoolV(~a.ux, ~a.uy, ~a.uz, ~a.uw);
}

PX_FORCE_INLINE BoolV BOr(const BoolV a, const BoolV b)	
{
	return BoolV(a.ux || b.ux ? (PxU32)-1 : 0,  a.uy || b.uy ? (PxU32)-1 : 0, a.uz || b.uz ? (PxU32)-1 : 0, a.uw || b.uw ? (PxU32)-1 : 0);
}

PX_FORCE_INLINE PxU32 BAllEq(const BoolV a, const BoolV b)
{
	return (a.ux==b.ux && a.uy==b.uy && a.uz==b.uz && a.uw==b.uw ? 1 : 0);
}

PX_FORCE_INLINE BoolV BAllTrue4(const BoolV a)
{
	return (a.ux & a.uy & a.uz & a.uw) ? BoolV(1, 1, 1, 1) : BoolV(0, 0, 0, 0);
}

PX_FORCE_INLINE BoolV BAnyTrue4(const BoolV a)
{
	return (a.ux | a.uy | a.uz | a.uw) ? BoolV(1, 1, 1, 1) : BoolV(0, 0, 0, 0);
}

PX_FORCE_INLINE BoolV BAllTrue3(const BoolV a)
{
	return (a.ux & a.uy & a.uz) ? BoolV(1, 1, 1, 1) : BoolV(0, 0, 0, 0);
}

PX_FORCE_INLINE BoolV BAnyTrue3(const BoolV a)
{
	return (a.ux | a.uy | a.uz) ? BoolV(1, 1, 1, 1) : BoolV(0, 0, 0, 0);
}



//////////////////////////////////
//MAT33V
//////////////////////////////////

PX_FORCE_INLINE Vec3V M33MulV3(const Mat33V& a, const Vec3V b) 
{
	return Vec3V
	(
	a.col0.x*b.x + a.col1.x*b.y + a.col2.x*b.z,
	a.col0.y*b.x + a.col1.y*b.y + a.col2.y*b.z,
	a.col0.z*b.x + a.col1.z*b.y + a.col2.z*b.z
	);
}

PX_FORCE_INLINE Vec3V M33TrnspsMulV3(const Mat33V& a, const Vec3V b)
{
	return Vec3V
	(
	a.col0.x*b.x + a.col0.y*b.y + a.col0.z*b.z,
	a.col1.x*b.x + a.col1.y*b.y + a.col1.z*b.z,
	a.col2.x*b.x + a.col2.y*b.y + a.col2.z*b.z
	);
}

PX_FORCE_INLINE Vec3V M33MulV3AddV3(const Mat33V& A, const Vec3V b, const Vec3V c)
{
	const FloatV x=V3GetX(b); 
	const FloatV y=V3GetY(b); 
	const FloatV z=V3GetZ(b); 
	Vec3V result = V3MulAdd(A.col0, x, c);
	result = V3MulAdd(A.col1, y, result);
	return V3MulAdd(A.col2, z, result);
}


PX_FORCE_INLINE Mat33V M33MulM33(const Mat33V& a, const Mat33V& b)
{
	return Mat33V(M33MulV3(a,b.col0),M33MulV3(a,b.col1),M33MulV3(a,b.col2));
}

PX_FORCE_INLINE Mat33V M33Add(const Mat33V& a, const Mat33V& b)
{
	return Mat33V(V3Add(a.col0,b.col0),V3Add(a.col1,b.col1),V3Add(a.col2,b.col2));
}

PX_FORCE_INLINE Mat33V M33Sub(const Mat33V& a, const Mat33V& b)
{
	return Mat33V(V3Sub(a.col0,b.col0),V3Sub(a.col1,b.col1),V3Sub(a.col2,b.col2));
}


PX_FORCE_INLINE Mat33V M33Neg(const Mat33V& a)
{
	return Mat33V(V3Neg(a.col0),V3Neg(a.col1),V3Neg(a.col2));
}

PX_FORCE_INLINE Mat33V M33Abs(const Mat33V& a)
{
	return Mat33V(V3Abs(a.col0),V3Abs(a.col1),V3Abs(a.col2));
}


PX_FORCE_INLINE Mat33V Mat33V_From_PxMat33(const PxMat33 &m)
{
	return Mat33V(Vec3V_From_PxVec3(m.column0), 
				  Vec3V_From_PxVec3(m.column1), 
				  Vec3V_From_PxVec3(m.column2));
}

PX_FORCE_INLINE void PxMat33_From_Mat33V(const Mat33V &m, PxMat33 &out)
{
	PX_ASSERT((size_t(&out)&15)==0);
	PxVec3_From_Vec3V(m.col0, out.column0);
	PxVec3_From_Vec3V(m.col1, out.column1);
	PxVec3_From_Vec3V(m.col2, out.column2);
}



PX_FORCE_INLINE Mat33V M33Trnsps(const Mat33V& a)
{
	return Mat33V(Vec3V(a.col0.x,a.col1.x,a.col2.x),Vec3V(a.col0.y,a.col1.y,a.col2.y),Vec3V(a.col0.z,a.col1.z,a.col2.z));
}


PX_FORCE_INLINE Mat33V M33Identity()
{
	return Mat33V
	(
	V3UnitX(),
	V3UnitY(),
	V3UnitZ()
	);
}


//////////////////////////////////
//MAT34V
//////////////////////////////////

PX_FORCE_INLINE Vec3V M34MulV3(const Mat34V& a, const Vec3V b) 
{
	return Vec3V
	(
	a.col0.x*b.x + a.col1.x*b.y + a.col2.x*b.z + a.col3.x,
	a.col0.y*b.x + a.col1.y*b.y + a.col2.y*b.z + a.col3.y,
	a.col0.z*b.x + a.col1.z*b.y + a.col2.z*b.z + a.col3.z
	);
}

PX_FORCE_INLINE Vec3V M34Mul33V3(const Mat34V& a, const Vec3V b)
{
	return Vec3V
	(
	a.col0.x*b.x + a.col1.x*b.y + a.col2.x*b.z,
	a.col0.y*b.x + a.col1.y*b.y + a.col2.y*b.z,
	a.col0.z*b.x + a.col1.z*b.y + a.col2.z*b.z
	);
}

PX_FORCE_INLINE Vec3V M34TrnspsMul33V3(const Mat34V& a, const Vec3V b)
{
	return Vec3V
	(
	a.col0.x*b.x + a.col0.y*b.y + a.col0.z*b.z,
	a.col1.x*b.x + a.col1.y*b.y + a.col1.z*b.z,
	a.col2.x*b.x + a.col2.y*b.y + a.col2.z*b.z
	);
}

PX_FORCE_INLINE Mat34V M34MulM34(const Mat34V& a, const Mat34V& b)
{
	return Mat34V(M34Mul33V3(a,b.col0),M34Mul33V3(a,b.col1),M34Mul33V3(a,b.col2),M34MulV3(a,b.col3));
}

PX_FORCE_INLINE Mat33V M34Mul33V3(const Mat34V& a, const Mat33V& b)
{
	return Mat33V(M34Mul33V3(a,b.col0),M34Mul33V3(a,b.col1),M34Mul33V3(a,b.col2));
}

PX_FORCE_INLINE Mat33V M34Mul33MM34(const Mat34V& a, const Mat34V& b)
{
	return Mat33V(M34Mul33V3(a,b.col0),M34Mul33V3(a,b.col1),M34Mul33V3(a,b.col2));
}

PX_FORCE_INLINE Mat34V M34Add(const Mat34V& a, const Mat34V& b)
{
	return Mat34V(V3Add(a.col0,b.col0),V3Add(a.col1,b.col1),V3Add(a.col2,b.col2),V3Add(a.col3,b.col3));
}

PX_FORCE_INLINE Mat33V M34Trnsps33(const Mat34V& a)
{
	return Mat33V(Vec3V(a.col0.x,a.col1.x,a.col2.x),Vec3V(a.col0.y,a.col1.y,a.col2.y),Vec3V(a.col0.z,a.col1.z,a.col2.z));
}


//////////////////////////////////
//MAT44V
//////////////////////////////////

PX_FORCE_INLINE Vec4V M44MulV4(const Mat44V& a, const Vec4V b) 
{
	return Vec4V 
	(
	a.col0.x*b.x + a.col1.x*b.y + a.col2.x*b.z + a.col3.x*b.w,
	a.col0.y*b.x + a.col1.y*b.y + a.col2.y*b.z + a.col3.y*b.w,
	a.col0.z*b.x + a.col1.z*b.y + a.col2.z*b.z + a.col3.z*b.w,
	a.col0.w*b.x + a.col1.w*b.y + a.col2.w*b.z + a.col3.w*b.w
	);
}

PX_FORCE_INLINE Vec4V M44TrnspsMulV4(const Mat44V& a, const Vec4V b) 
{
	return Vec4V
	(
	a.col0.x*b.x + a.col0.y*b.y + a.col0.z*b.z + a.col0.w*b.w,
	a.col1.x*b.x + a.col1.y*b.y + a.col1.z*b.z + a.col1.w*b.w,
	a.col2.x*b.x + a.col2.y*b.y + a.col2.z*b.z + a.col2.w*b.w,
	a.col3.x*b.x + a.col3.y*b.y + a.col3.z*b.z + a.col3.w*b.w
	);
}

PX_FORCE_INLINE Mat44V M44MulM44(const Mat44V& a, const Mat44V& b)
{
	return Mat44V(M44MulV4(a,b.col0),M44MulV4(a,b.col1),M44MulV4(a,b.col2),M44MulV4(a,b.col3));
}

PX_FORCE_INLINE Mat44V M44Add(const Mat44V& a, const Mat44V& b)
{
	return Mat44V(V4Add(a.col0,b.col0),V4Add(a.col1,b.col1),V4Add(a.col2,b.col2),V4Add(a.col3,b.col3));
}

PX_FORCE_INLINE Mat44V M44Inverse(const Mat44V& a)
{
	PxF32 tmp[12]; 
	PxF32 dst[16];
	PxF32 det; 

	const PxF32 src[16] = 
	{
		a.col0.x, a.col0.y, a.col0.z, a.col0.w,
		a.col1.x, a.col1.y, a.col1.z, a.col1.w,
		a.col2.x, a.col2.y, a.col2.z, a.col2.w,
		a.col3.x, a.col3.y, a.col3.z, a.col3.w
	};

	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];

	dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
	dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
	dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
	dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
	dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
	dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
	dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
	dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
	dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];
	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10] = src[0]*src[5];
	tmp[11] = src[1]*src[4];

	dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
	dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
	dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
	dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
	dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
	dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
	dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
	dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
	dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
	dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
	dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
	dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
	dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
	dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
	dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
	dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

	det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];

	det = 1.0f/det;
	for(PxU32 j=0;j<16;j++)
	{
		dst[j] *= det;
	}

	return Mat44V
	(
		Vec4V(dst[0],dst[4],dst[8],dst[12]),
		Vec4V(dst[1],dst[5],dst[9],dst[13]),
		Vec4V(dst[2],dst[6],dst[10],dst[14]),
		Vec4V(dst[3],dst[7],dst[11],dst[15])
	);
}

PX_FORCE_INLINE Mat44V M44Trnsps(const Mat44V& a)
{
	return Mat44V
	(
	Vec4V(a.col0.x,a.col1.x,a.col2.x,a.col3.x),
	Vec4V(a.col0.y,a.col1.y,a.col2.y,a.col3.y),
	Vec4V(a.col0.z,a.col1.z,a.col2.z,a.col3.z),
	Vec4V(a.col0.w,a.col1.w,a.col2.w,a.col3.w)
	);
}

PX_FORCE_INLINE Vec4V Vec4V_From_XYZW(const PxF32& x, const PxF32& y, const PxF32& z, const PxF32& w)
{
	return Vec4V(x, y, z, w);
}


PX_FORCE_INLINE VecU16V V4U32PK(VecU32V a, VecU32V b)
{
	return VecU16V(
		PxU16(PxClamp<PxU32>((a).u32[0], 0, 0xFFFF)),
		PxU16(PxClamp<PxU32>((a).u32[1], 0, 0xFFFF)),
		PxU16(PxClamp<PxU32>((a).u32[2], 0, 0xFFFF)),
		PxU16(PxClamp<PxU32>((a).u32[3], 0, 0xFFFF)),
		PxU16(PxClamp<PxU32>((b).u32[0], 0, 0xFFFF)),
		PxU16(PxClamp<PxU32>((b).u32[1], 0, 0xFFFF)),
		PxU16(PxClamp<PxU32>((b).u32[2], 0, 0xFFFF)),
		PxU16(PxClamp<PxU32>((b).u32[3], 0, 0xFFFF)));
}

PX_FORCE_INLINE VecU32V V4U32or(VecU32V a, VecU32V b)
{
	return VecU32V((a).u32[0]|(b).u32[0], (a).u32[1]|(b).u32[1], (a).u32[2]|(b).u32[2], (a).u32[3]|(b).u32[3]);
}

PX_FORCE_INLINE VecU32V V4U32and(VecU32V a, VecU32V b)
{
	return VecU32V((a).u32[0]&(b).u32[0], (a).u32[1]&(b).u32[1], (a).u32[2]&(b).u32[2], (a).u32[3]&(b).u32[3]);
}

PX_FORCE_INLINE VecU32V V4U32Andc(VecU32V a, VecU32V b)
{
	return VecU32V((a).u32[0]&~(b).u32[0], (a).u32[1]&~(b).u32[1], (a).u32[2]&~(b).u32[2], (a).u32[3]&~(b).u32[3]);
}

PX_FORCE_INLINE VecU16V V4U16Or(VecU16V a, VecU16V b)
{
	return VecU16V(
		(a).u16[0]|(b).u16[0], (a).u16[1]|(b).u16[1], (a).u16[2]|(b).u16[2], (a).u16[3]|(b).u16[3],
		(a).u16[4]|(b).u16[4], (a).u16[5]|(b).u16[5], (a).u16[6]|(b).u16[6], (a).u16[7]|(b).u16[7]);
}

PX_FORCE_INLINE VecU16V V4U16And(VecU16V a, VecU16V b)
{
	return VecU16V(
		(a).u16[0]&(b).u16[0], (a).u16[1]&(b).u16[1], (a).u16[2]&(b).u16[2], (a).u16[3]&(b).u16[3],
		(a).u16[4]&(b).u16[4], (a).u16[5]&(b).u16[5], (a).u16[6]&(b).u16[6], (a).u16[7]&(b).u16[7]);
}

PX_FORCE_INLINE VecU16V V4U16Andc(VecU16V a, VecU16V b)
{
	return VecU16V(
		(a).u16[0]&~(b).u16[0], (a).u16[1]&~(b).u16[1], (a).u16[2]&~(b).u16[2], (a).u16[3]&~(b).u16[3],
		(a).u16[4]&~(b).u16[4], (a).u16[5]&~(b).u16[5], (a).u16[6]&~(b).u16[6], (a).u16[7]&~(b).u16[7]);
}

template<int a> PX_FORCE_INLINE VecI32V V4ISplat()
{
	return VecI32V(a, a, a, a);
}

PX_FORCE_INLINE void V4U16StoreAligned(VecU16V val, VecU16V* address)
{
	*address = val;
}

PX_FORCE_INLINE void V4U32StoreAligned(VecU32V val, VecU32V* address)
{
	*address = val;
}

PX_FORCE_INLINE Vec4V V4LoadAligned(Vec4V* addr)
{
	return *addr;
}

PX_FORCE_INLINE Vec4V V4LoadUnaligned(Vec4V* addr)
{
	return *addr;
}

PX_FORCE_INLINE Vec4V V4Andc(const Vec4V a, const VecU32V b)
{
	VecU32V result32(a);
	result32 = V4U32Andc(result32, b);
	return Vec4V(result32);
}

PX_FORCE_INLINE VecU32V V4IsGrtrV32u(const Vec4V a, const Vec4V b)
{
	return VecU32V(
		a.x > b.x ? 0xFFFFffff : 0,
		a.y > b.y ? 0xFFFFffff : 0,
		a.z > b.z ? 0xFFFFffff : 0,
		a.w > b.w ? 0xFFFFffff : 0);
}

PX_FORCE_INLINE VecU16V V4U16LoadAligned(VecU16V* addr)
{
	return *addr;
}

PX_FORCE_INLINE VecU16V V4U16LoadUnaligned(VecU16V* addr)
{
	return *addr;
}

PX_FORCE_INLINE VecU16V V4U16CompareGt(VecU16V a, VecU16V b)
{
	return VecU16V(
		(a).u16[0]>(b).u16[0], (a).u16[1]>(b).u16[1], (a).u16[2]>(b).u16[2], (a).u16[3]>(b).u16[3],
		(a).u16[4]>(b).u16[4], (a).u16[5]>(b).u16[5], (a).u16[6]>(b).u16[6], (a).u16[7]>(b).u16[7]);
}

PX_FORCE_INLINE Vec4V Vec4V_From_VecU32V(VecU32V a)
{
	return Vec4V(PxF32((a).u32[0]), PxF32((a).u32[1]), PxF32((a).u32[2]), PxF32((a).u32[3]));
}

template<int index> PX_FORCE_INLINE VecU32V V4U32SplatElement(VecU32V a)
{
	return VecU32V((a).u32[index], (a).u32[index], (a).u32[index], (a).u32[index]);
}

template<int index> PX_FORCE_INLINE VecU32V V4U32SplatElement(BoolV a)
{
	const PxU32 u = (&a.ux)[index];
	return VecU32V(u, u, u, u);
}

template<int index> PX_FORCE_INLINE Vec4V V4SplatElement(Vec4V a)
{
	float* data = (float*)&a;
	return Vec4V(data[index], data[index], data[index], data[index]);
}

template<int index> PX_FORCE_INLINE VecU16V V4U16SplatElement(VecU16V a)
{
	return VecU16V(
		(a).u16[index], (a).u16[index], (a).u16[index], (a).u16[index],
		(a).u16[index], (a).u16[index], (a).u16[index], (a).u16[index]);
}

template<int imm> PX_FORCE_INLINE VecI16V V4I16SplatImmediate()
{
	return VecI16V(imm, imm, imm, imm, imm, imm, imm, imm);
}

PX_FORCE_INLINE VecU16V V4U16SubtractModulo(VecU16V a, VecU16V b)
{
	return VecU16V(
		(a).u16[0] - (b).u16[0], (a).u16[1] - (b).u16[1], (a).u16[2] - (b).u16[2], (a).u16[3] - (b).u16[3],
		(a).u16[4] - (b).u16[4], (a).u16[5] - (b).u16[5], (a).u16[6] - (b).u16[6], (a).u16[7] - (b).u16[7]);
}

PX_FORCE_INLINE VecU16V V4U16AddModulo(VecU16V a, VecU16V b)
{
	return VecU16V(
		(a).u16[0] + (b).u16[0], (a).u16[1] + (b).u16[1], (a).u16[2] + (b).u16[2], (a).u16[3] + (b).u16[3],
		(a).u16[4] + (b).u16[4], (a).u16[5] + (b).u16[5], (a).u16[6] + (b).u16[6], (a).u16[7] + (b).u16[7]);
}

PX_FORCE_INLINE VecU32V V4U16GetLo16(VecU16V a)
{
	return VecU32V((a).u16[0], (a).u16[2], (a).u16[4], (a).u16[6]);
}

PX_FORCE_INLINE VecU32V V4U16GetHi16(VecU16V a)
{
	return VecU32V((a).u16[1], (a).u16[3], (a).u16[5], (a).u16[7]);
}

PX_FORCE_INLINE VecU32V VecU32V_From_XYZW(PxU32 x, PxU32 y, PxU32 z, PxU32 w)
{
	return VecU32V(x, y, z, w);
}

PX_FORCE_INLINE Vec4V V4Abs(const Vec4V a)
{
	return V4Max(a,V4Neg(a));
}

PX_FORCE_INLINE Vec4V V4Ceil(const Vec4V a)
{
	return Vec4V(PxCeil(a.x), PxCeil(a.y), PxCeil(a.z), PxCeil(a.w));
}

PX_FORCE_INLINE Vec4V V4Floor(const Vec4V a)
{
	return Vec4V(PxFloor(a.x), PxFloor(a.y), PxFloor(a.z), PxFloor(a.w));
}

PX_FORCE_INLINE VecU32V V4ConvertToU32VSaturate(const Vec4V a, PxU32 power)
{
	PX_ASSERT(power == 0 && "Non-zero power not supported in convertToU32VSaturate");
	PX_FORCE_PARAMETER_REFERENCE(power); // prevent warning in release builds
	PxF32 ffffFFFFasFloat = PxF32(0xFFFF0000);
	return VecU32V(
		PxU32(PxClamp<PxF32>((a).x, 0.0f, ffffFFFFasFloat)),
		PxU32(PxClamp<PxF32>((a).y, 0.0f, ffffFFFFasFloat)),
		PxU32(PxClamp<PxF32>((a).z, 0.0f, ffffFFFFasFloat)),
		PxU32(PxClamp<PxF32>((a).w, 0.0f, ffffFFFFasFloat)));
}

PX_FORCE_INLINE BoolV V4IsEqU32(const VecU32V a, const VecU32V b)
{
	return VecU32V(a.u32[0]==b.u32[0] ? -1 : 0, a.u32[1]==b.u32[1] ? -1 : 0, a.u32[2]==b.u32[2] ? -1 : 0, a.u32[3]==b.u32[3] ? -1 : 0);
}

PX_FORCE_INLINE VecI32V VecI32V_From_I32(const PxI32 i)
{
	return VecI32V(i, i, i, i);
}

PX_FORCE_INLINE VecI32V VecI32V_From_I32Array(const PxI32* i)
{
	return VecI32V(i[0], i[1], i[2], i[3]);
}

PX_FORCE_INLINE VecI32V VecI32V_From_I32Array_Aligned(const PxI32* i)
{
	return VecI32V(i[0], i[1], i[2], i[3]);
}

PX_FORCE_INLINE VecI32V VecI32V_Add(const VecI32VArg a, const VecI32VArg b)
{
	return VecI32V(a.i32[0] + b.i32[0], a.i32[1] + b.i32[1], a.i32[2] + b.i32[2], a.i32[3] + b.i32[3]);
}

PX_FORCE_INLINE VecI32V VecI32V_Sub(const VecI32VArg a, const VecI32VArg b)
{
	return VecI32V(a.i32[0] - b.i32[0], a.i32[1] - b.i32[1], a.i32[2] - b.i32[2], a.i32[3] - b.i32[3]);
}

PX_FORCE_INLINE BoolV VecI32V_IsGrtr(const VecI32VArg a, const VecI32VArg b)
{
	return BoolV(a.i32[0] > b.i32[0], a.i32[1] > b.i32[1], a.i32[2] > b.i32[2], a.i32[3] > b.i32[3]);
}

PX_FORCE_INLINE BoolV VecI32V_IsEq(const VecI32VArg a, const VecI32VArg b)
{
	return BoolV(a.i32[0] == b.i32[0], a.i32[1] == b.i32[1], a.i32[2] == b.i32[2], a.i32[3] == b.i32[3]);
}

PX_FORCE_INLINE VecI32V VecI32V_Zero()
{
	return VecI32V(0,0,0,0);
}

PX_FORCE_INLINE VecI32V VecI32V_Merge(const VecI32VArg a, const VecI32VArg b, const VecI32VArg c, const VecI32VArg d)
{
	return VecI32V(a.i32[0], b.i32[0], c.i32[0], d.i32[0]);
}


#endif //PX_PHYSICS_COMMON_VECMATH_SCALAR_INLINE
