#pragma once
/* RanLogic/FlyCamera/FlyCameraDefine.h·Î º¯°æµÊ;

#include "../../enginelib/G-Logic/GLDefine.h"

namespace FlyCameraControl{
	
	enum { 
		EMFLYCAMERA_MOVETO_SIZE = 4, EMFLYCAMERA_LOOKAT_SIZE = 4 
	};
 	extern std::string strMOVETOTYPE[EMFLYCAMERA_MOVETO_SIZE];
 	extern std::string strLOOKATTYPE[EMFLYCAMERA_LOOKAT_SIZE];

	struct FlyCameraKey	
	{
	public:
		STARGETID				sMoveToTargetID;
		int	nKey;

	public:
		FlyCameraKey() 
		{
		}
		FlyCameraKey(const FlyCameraKey& rhs)
			: sMoveToTargetID(rhs.sMoveToTargetID)
			, nKey(rhs.nKey) {}
		const FlyCameraKey& operator =(const FlyCameraKey& rhs)
		{
			sMoveToTargetID = rhs.sMoveToTargetID;
			nKey = rhs.nKey;
			return *this;
		}
		const bool operator ==(const FlyCameraKey& rhs) const
		{
			return (sMoveToTargetID == rhs.sMoveToTargetID && nKey == rhs.nKey);
		}
		const bool operator !=(const FlyCameraKey& rhs) const
		{
			return (sMoveToTargetID != rhs.sMoveToTargetID || nKey != rhs.nKey);
		}
		const bool operator <(const FlyCameraKey& rhs) const
		{
			if (nKey < rhs.nKey )
				return true;

			return false;
		}
		void Reset(void)
		{
			sMoveToTargetID.RESET();
			nKey = -1;
		}
	};

	struct FlyCameraAnchors
	{
		FlyCameraKey vPreviAnchor;
		FlyCameraKey vStartAnchor;
		FlyCameraKey vDestAnchor;
		FlyCameraKey vNextAnchor;
		void Reset(void)
		{
			vPreviAnchor.Reset();
			vStartAnchor.Reset();
			vDestAnchor.Reset();
			vNextAnchor.Reset();
		}
	};
	const float CATMULL_ROM_SPLINE(float u0, float u1, float u2, float x0, float x1, float x2, float x3);

	enum FLYCAMERA_MODETYPE{
		MODETYPE_NONE = 0,
		MODETYPE_FIXEDMODE,
		MODETYPE_FOLLOWMODE,
	};
	// msg
	enum EMMSG{
		EMFLYCAMERA_MOVE = 0,
		EMFLYCAMERA_ACTIVE,
		EMFLYCAMERA_PLAY_ANIMATION,
		EMFLYCAMERA_STOP_ANIMATION,
		EMFLYCAMERA_REWIND_ANIMATION,
		EMFLYCAMERA_SET_CURRENTKEYFRAME,
		EMFLYCAMERA_SET_FPS,
		EMFLYCAMERA_ADD_KEYFRAMESLOT,
		EMFLYCAMERA_DEL_KEYFRAMESLOT,
		EMFLYCAMERA_SET_KEYPOSITION,
		EMFLYCAMERA_SET_TOTALFRAME,
		EMFLYCAMERA_SET_FOLLOWTARGET,
		EMFLYCAMERA_AFTER_ACTION_MOVE_FIXEDMODE,
		EMFLYCAMERA_AFTER_ACTION_ROTATE_FIXEDMODE,
		EMFLYCAMERA_AFTER_ACTION_MOVE_FOLLOWMODE,
		EMFLYCAMERA_AFTER_ANIMATION_MOVE,

		EMFLYCAMERA_PRINT_MOVEINTERTIME,
		EMFLYCAMERA_PRINT_ROTATEINTERTIME,
		EMFLYCAMERA_PRINT_TARGETMOVEINTERTIME,
	};

	struct SMsg
	{
		EMMSG type;
		DWORD dwMsg;
	};
}
*/