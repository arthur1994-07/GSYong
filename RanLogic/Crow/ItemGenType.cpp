#include "../pch.h"

#include "../../SigmaCore/Math/Random.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"

#include "../GLogicEx.h"
#include "../Item/GLItemMan.h"
#include "./ItemGenType.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void SGENITEMSPEC::GetResult( OUT SGENRESULT& sResult )
{
    sResult.emType       = EMGENRESULTTYPE_ITEM;
    sResult.Data.sItemID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
}

void SGENITEMID::GetResult( OUT SGENRESULT& sResult )
{
    sResult.emType       = EMGENRESULTTYPE_ITEM;
    sResult.Data.sItemID = sItemID;
}

void SGENQITEMID::GetResult( OUT SGENRESULT& sResult )
{
    sResult.emType       = EMGENRESULTTYPE_ITEM;
    sResult.Data.sItemID = sQItemID;
}

void SGENMONEY::GetResult( OUT SGENRESULT& sResult )
{
    sResult.emType       = EMGENRESULTTYPE_MONEY;
    sResult.Data.dwMoney = dwMoney;
}

void SGENITEMSPECSKILLFACT::GetResult( OUT SGENRESULT& sResult )
{
    sResult.emType       = EMGENRESULTTYPE_ITEM;
    sResult.Data.sItemID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
}

const bool SGENITEMSPECSKILLFACT::IsGen( GLCHARLOGIC* pChar )
{
	if (!pChar)
		return false;

    if ( !SGENITEMBASE::IsGen( pChar ) )
    {
        return false;
    }

    WORD wSlot = 0;

    if ( pChar->IS_HAVE_SKILLFACT( sSkillID, wSlot ) )
    {
        return true;
    }

    return false;
}