#include "pch.h"

#include "../network/NetLogicDefine.h"
#include "./LimitPrison.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPrisionLimitChecker::CPrisionLimitChecker()
{
}

CPrisionLimitChecker::~CPrisionLimitChecker()
{

}

bool CPrisionLimitChecker::IsPrisionMap( const SMAPNODE *pMapNode, DWORD dwUserLv )
{
	if( pMapNode == NULL || !pMapNode->IsPrisonMap() ) return false;

	if( dwUserLv >= USER_GM4 ) return false; 

	return true;
}

bool CPrisionLimitChecker::IsLimitStatus( const SMAPNODE *pMapNode, DWORD dwUserLv, EPML eLimit )
{
	if( pMapNode == NULL || !pMapNode->IsPrisonMap() ) return false;

	if( dwUserLv >= USER_GM4 ) return false;

	switch( eLimit )
	{
	case PML_CHATTING :
	case PML_WARP     :
	case PML_ITEM     :
	case PML_WAR      :
	case PML_PARTY    :
		return true;
	}

	return false;
}

CPrisionLimitChecker& CPrisionLimitChecker::GetInstance()
{
	static CPrisionLimitChecker sInstance;

	return sInstance;
}