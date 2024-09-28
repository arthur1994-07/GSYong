#include "pch.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./GLogicExServer.h"

#include "../../SigmaCore/DebugInclude.h"


namespace GLOGICEX
{

    int CALCKILLEXP(GLGaeaServer* pGaeaServer, WORD wMYLEVEL, const STARGETID &cTargetID, const GLLandMan* pLandMan)
    {	
        ServerActor *pActor = pGaeaServer->GetTarget(pLandMan, cTargetID);
        if (!pActor)
            return 0;

        WORD wAttackerLev = wMYLEVEL;
        WORD wDefenserLev = pActor->GetLevel();
        DWORD dwBonusExp = pActor->GetBonusExp();
        
        // 경험치 산출	
        return GLOGICEX::GLKILLEXP(wAttackerLev, wDefenserLev, dwBonusExp);
    }

    int CALCATTACKEXP(GLGaeaServer* pGaeaServer, WORD wMYLEVEL, const STARGETID &cTargetID, const GLLandMan* pLandMan, DWORD dwDamage)
    {
        ServerActor *pActor = pGaeaServer->GetTarget(pLandMan, cTargetID);
        if (!pActor)
            return 0;

        WORD wTAR_LEVEL = pActor->GetLevel();
        DWORD dwTAR_MAXHP = pActor->GetMaxHP();
        DWORD dwBONUSEXP = pActor->GetBonusExp();

        //	Note : 경험치 산출
        return GLOGICEX::GLATTACKEXP(wMYLEVEL, wTAR_LEVEL, dwDamage, dwTAR_MAXHP, dwBONUSEXP);
    }

} // namespace GLOGICEX