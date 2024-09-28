
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

	// Ĺ�ַ� ���ö����� �ٸ� ������, ���ö��� ������ �ٸ��� ��Ŀ�� ������;
	// ���� �ٱ��ʿ��ִ�, ������ ���⼭ ������ ��ġ�� x1, x2��;
	/// ������� ������ ����Ʈ �ΰ� ���̿� ������ �߰����� ��Ŀ�� �־��ִ� ���;
	/// ���� �ٸ���;
	const float CATMULL_ROM_SPLINE(float u0, float u1, float u2, float x0, float x1, float x2, float x3)
	{
		return ((-1.0f*u2 + 2.0f*u1 - 1.0f*u0 + 0.0f) * x0 +
			( 3.0f*u2 - 5.0f*u1 + 0.0f*u0 + 2.0f) * x1 +
			(-3.0f*u2 + 4.0f*u1 + 1.0f*u0 + 0.0f) * x2 +
			( 1.0f*u2 - 1.0f*u1 + 0.0f*u0 + 0.0f) * x3
			) / 2.0f;
	}
};