#ifndef _GLHIDESET_H_
#define _GLHIDESET_H_

#include <xstring>
#include "GLHideSet_Define.h"

namespace GLHIDESET
{
	extern __forceinline const bool CheckHideSet(const EMHIDESET _IndexHideSet);	
	extern HRESULT LoadFile(const std::string& _FileName, const bool _bPastLoad);
	
	extern bool bHideSet[EMHIDE_SIZE];
	extern float fInvisiblePerceiveAngle;
	extern float fInvisiblePerceiveRadius;
	extern float fInvisiblePerceiveURadius;
	extern float fInvisiblePerceiveLevel;
	extern float fInvisibleUpdateTime;
};


#endif
