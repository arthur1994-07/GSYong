
#include "pch.h"

#include "../../EngineLib/G-Logic/GLDefine.h"

#include "./FlyCameraDefine.h"

namespace FlyCameraControl{
	std::string strMOVETOTYPE[EMFLYCAMERA_MOVETO_SIZE] = {
		"NONE",
		"SETPOSITION",
		"MOVETO",
		"MOVETO_TARGET"
	};

	std::string strLOOKATTYPE[EMFLYCAMERA_LOOKAT_SIZE] = {
		"NONE",
		"SETPOSITION",
		"MOVEMENT",
		"MOVETO_TARGET"
	};

	// 캣멀롬 스플라인은 다른 베지어, 스플라인 보간과 다르게 앵커가 움직일;
	// 선분 바깥쪽에있다, 실제로 여기서 움직일 위치는 x1, x2다;
	/// 베지어는 움직일 포인트 두개 사이에 보간될 중간지점 앵커를 넣어주는 방식;
	/// 과는 다르다;
	const float CATMULL_ROM_SPLINE(float u0, float u1, float u2, float x0, float x1, float x2, float x3)
	{
		return ((-1.0f*u2 + 2.0f*u1 - 1.0f*u0 + 0.0f) * x0 +
			( 3.0f*u2 - 5.0f*u1 + 0.0f*u0 + 2.0f) * x1 +
			(-3.0f*u2 + 4.0f*u1 + 1.0f*u0 + 0.0f) * x2 +
			( 1.0f*u2 - 1.0f*u1 + 0.0f*u0 + 0.0f) * x3
			) / 2.0f;
	}
};