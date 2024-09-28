#include "stdafx.h"

#include "../NpcAction.h"
#include "./NpcActionPage.h"
#include "../Edit/SheetWithTab.h"

#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Skill/GLSkill.h"

// CNpcActionPage 대화 상자입니다.

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

std::string	CNpcActionPage::GetBoolString ( BOOL bValue )
{
	const static	char szTRUE[] = "TRUE";
	const static	char szFALSE[] = "FALSE";
	
	if ( bValue )	return szTRUE;
	else			return szFALSE;
}