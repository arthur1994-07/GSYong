#ifndef _GLOGIC_EX_CLIENT_H_
#define _GLOGIC_EX_CLIENT_H_

#pragma once

#include "../EngineLib/G-Logic/GLDefine.h"

class GLGaeaClient;

D3DXVECTOR3 UpdateSkillDirection(
    GLGaeaClient* pGaeaClient,
    const D3DXVECTOR3& vMyPos,
    const D3DXVECTOR3& vMyDir,
    const SNATIVEID& sSKILLID,
    const D3DXVECTOR3& vTARPOS,
    const STARID* pTARID);


#endif // _GLOGIC_EX_CLIENT_H_