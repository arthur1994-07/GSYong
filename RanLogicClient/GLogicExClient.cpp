#include "pch.h"

#include "./ClientActor.h"
#include "./GLogicExClient.h"
#include "./GLGaeaClient.h"

#include "../SigmaCore/DebugInclude.h"


D3DXVECTOR3 UpdateSkillDirection( GLGaeaClient*			pGaeaClient,
								  const D3DXVECTOR3&	vMyPos,
								  const D3DXVECTOR3&	vMyDir,
								  const SNATIVEID&		sSKILLID,
								  const D3DXVECTOR3&	vTARPOS,
								  const STARID*			pTARID )
{
    D3DXVECTOR3		vResultDir = vMyDir;
	PGLSKILL		pSkillData = GLSkillMan::GetInstance().GetData( sSKILLID.wMainID, sSKILLID.wSubID );
	if( !pSkillData )
		return vResultDir;

	//	Note : 방향 회전이 필요할 경우 방향을 튼다.
	//
	EMIMPACT_TARGET	emTARGET = pSkillData->m_sBASIC.emIMPACT_TAR;
	EMIMPACT_REALM	emREALM	 = pSkillData->m_sBASIC.emIMPACT_REALM;
	if( emTARGET == TAR_SPEC || emTARGET == TAR_SELF_TOSPEC )
	{
		if( pTARID[0].dwID != GAEAID_NULL )
		{
			STARGETID sTargetID(pTARID[0].emCrow, pTARID[0].dwID);
			ClientActor* pTarget = pGaeaClient->GetCopyActor ( sTargetID );
			if( pTarget )
				vResultDir = pTarget->GetPosition() - vMyPos;
		}
	}
	else if( emTARGET == TAR_ZONE || emTARGET == TAR_LAND )
	{
		vResultDir = vTARPOS - vMyPos;
	}
	else if( emTARGET == TAR_SELF )
	{
		if( emREALM == REALM_ZONE )
		{
			if( pTARID[0].dwID != GAEAID_NULL )
			{
				STARGETID sTargetID(pTARID[0].emCrow, pTARID[0].dwID);
				ClientActor* pTarget = pGaeaClient->GetCopyActor ( sTargetID );
				if( pTarget )
				{
					vResultDir = pTarget->GetPosition() - vMyPos;
					if( vResultDir == D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
						vResultDir = vMyDir;
				} //if
			} //if
		} //if
	} //if..else if..

	D3DXVec3Normalize( &vResultDir, &vResultDir );
    return vResultDir;
}

