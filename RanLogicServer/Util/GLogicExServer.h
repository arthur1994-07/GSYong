#ifndef _GLOGIC_EX_SERVER_H_
#define _GLOGIC_EX_SERVER_H_

#pragma once

#include "../Land/GLLandMan.h"

class GLGaeaServer;

namespace GLOGICEX
{
    //	°æÇèÄ¡ È¹µæ ( Á¦°Å½Ã )
    int CALCKILLEXP(GLGaeaServer* pGaeaServer, WORD wMYLEVEL, const STARGETID& cTargetID, const GLLandMan* pLandMan);

    //	°æÇèÄ¡ È¹µæ ( Å¸°Ý½Ã )
    int CALCATTACKEXP(GLGaeaServer* pGaeaServer, WORD wMYLEVEL, const STARGETID& cTargetID, const GLLandMan* pLandMan, DWORD dwDamage);
}

#endif // _GLOGIC_EX_SERVER_H_