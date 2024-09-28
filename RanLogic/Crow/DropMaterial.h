#ifndef _DROP_MATERIAL_H_
#define _DROP_MATERIAL_H_

#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"

struct SDROP_MATERIAL
{
    enum { CROW_GEM=0x0001, };

    SNATIVEID		sNativeID;	//	Crow 고유 ID.

    SNATIVEID		sMapID;		//	맵 ID.
    DWORD			dwCeID;		//	셀 ID.
    DWORD			dwGlobID;	//	생성 메모리 인덱스용.
    D3DXVECTOR3		vPos;		//	위치.
    D3DXVECTOR3		vDir;		//	방향.

    DWORD			dwFLAGS;	//	기타 속성.
    EMACTIONTYPE	emAction;	//	행동.
    DWORD			dwActState;	//	행동 상태.

    SDROP_MATERIAL() 
        : sMapID(NATIVEID_NULL())
        , dwCeID(0)
        , dwGlobID(0)
        , vPos(0,0,0)
        , vDir(1,0,0)
        , dwFLAGS(NULL)
        , emAction(GLAT_IDLE)
        , dwActState(0)
    {
    }
};

#endif // _DROP_MATERIAL_H_