#ifndef _DROP_MATERIAL_H_
#define _DROP_MATERIAL_H_

#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"

struct SDROP_MATERIAL
{
    enum { CROW_GEM=0x0001, };

    SNATIVEID		sNativeID;	//	Crow ���� ID.

    SNATIVEID		sMapID;		//	�� ID.
    DWORD			dwCeID;		//	�� ID.
    DWORD			dwGlobID;	//	���� �޸� �ε�����.
    D3DXVECTOR3		vPos;		//	��ġ.
    D3DXVECTOR3		vDir;		//	����.

    DWORD			dwFLAGS;	//	��Ÿ �Ӽ�.
    EMACTIONTYPE	emAction;	//	�ൿ.
    DWORD			dwActState;	//	�ൿ ����.

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