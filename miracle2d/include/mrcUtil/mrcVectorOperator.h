
#ifndef _mrcVectorOperator_h_
#define _mrcVectorOperator_h_

#include "../mrcConfig.h"

#include "../mrcVector2.h"
#include "../mrcVector3.h"
#include "../mrcVector4.h"
#include "../mrcMatrix4.h"

namespace mrc
{
	inline Vector2 toVector2(const Vector2& vec) { return vec; }
	inline Vector3 toVector3(const Vector2& vec) { return Vector3(vec.x,vec.y,0); }
	inline Vector4 toVector4(const Vector2& vec) { return Vector4(vec.x,vec.y,0,0); }

	inline Vector2 toVector2(const Vector3& vec) { return Vector2(vec.x,vec.y); }
	inline Vector3 toVector3(const Vector3& vec) { return vec; }
	inline Vector4 toVector4(const Vector3& vec) { return Vector4(vec.x,vec.y,vec.z,0); }

	inline Vector2 toVector2(const Vector4& vec) { return Vector2(vec.x,vec.y); }
	inline Vector3 toVector3(const Vector4& vec) { return Vector3(vec.x,vec.y,vec.z); }
	inline Vector4 toVector4(const Vector4& vec) { return vec; }


	inline bool operator==(const Vector2& vec2,const Vector3& vec3) 
	{
		return vec2.x == vec3.x && vec2.y == vec3.y;
	}

	inline bool operator!=(const Vector2& vec2,const Vector3& vec3) 
	{
		return !(vec2 == vec3);
	}


	inline bool operator==(const Vector3& vec3,const Vector2& vec2) { return vec2 == vec3; }


}





#endif

