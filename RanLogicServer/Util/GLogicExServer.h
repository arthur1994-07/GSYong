#ifndef _GLOGIC_EX_SERVER_H_
#define _GLOGIC_EX_SERVER_H_

#pragma once

#include "../Land/GLLandMan.h"

class GLGaeaServer;

namespace GLOGICEX
{
    //	����ġ ȹ�� ( ���Ž� )
    int CALCKILLEXP(GLGaeaServer* pGaeaServer, WORD wMYLEVEL, const STARGETID& cTargetID, const GLLandMan* pLandMan);

    //	����ġ ȹ�� ( Ÿ�ݽ� )
    int CALCATTACKEXP(GLGaeaServer* pGaeaServer, WORD wMYLEVEL, const STARGETID& cTargetID, const GLLandMan* pLandMan, DWORD dwDamage);
}

#endif // _GLOGIC_EX_SERVER_H_