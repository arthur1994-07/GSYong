#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD GLChar::CALCDAMAGE(
    int &rResultDAMAGE,
    const DWORD dwGaeaID,
    const STARGETID &cTargetID,
    const GLLandMan* pLandMan,
    const GLSKILL* pSkill,
    const DWORD dwskill_lev,
    const DWORD dwWeatherFlag,
    const DWORD dwDivCount)
{
    return m_pGaeaServer->CalcDamage(
        this,
        rResultDAMAGE,
        dwGaeaID,
        cTargetID,
        pLandMan,
        pSkill,
        dwskill_lev,
        dwWeatherFlag,
        dwDivCount);
}

DWORD GLChar::CALCHEAL(
    int &rResultHEAL,
    const DWORD	dwGaeaID,
    const STARGETID &sTargetID,
    const GLLandMan* pLandMan,
    const GLSKILL* pSkill,
    const DWORD dwskill_lev,
    const DWORD dwWeatherFlag,
    const DWORD dwDivCount)
{
	ServerActor *pActor = GetTarget(pLandMan, sTargetID);
	if (!pActor)
		return HEAL_TYPE_NONE;

	/// 추후 확장성을 위해 만들어 둠, 당장엔 하는 일 없음.
	return HEAL_TYPE_NONE;
}
