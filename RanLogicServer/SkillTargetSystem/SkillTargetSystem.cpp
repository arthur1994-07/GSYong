#include "pch.h"
#include "../Party/GLPartyField.h"
#include "../Party/GLPartyManagerField.h"
//#include "../Land/GLLandMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Util/StlFuncServer.h"
#include "SkillTargetSystem/SkillTargetSystem.h"


BOOL SkillTargetSystem::SelectCircle( ServerActor* pActorCaster, 
										ServerActor* pActorTarget,
										const GLSKILL& sSKILL, const WORD wLEVEL, 
										const D3DXVECTOR3&	vPos,
										STARID *sTARIDS, 
										WORD& wTargetNum  )
{
	GLLandMan* const pLand(pActorCaster->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	float fApplyAngle  = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;
	D3DXVECTOR3 vTarOrgDir( 0, 0, 0 );
	D3DXVECTOR3 vTarPosition = vPos;

	wTargetNum = 0;
	

	STARGETID sTempTargetID;

	//AddTarget( pActorCaster, sTARIDS, wTargetNum );

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM <= wTargetNum )	
		return TRUE;
	
	int nApplyRange =  4 + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2.0f;

	EraseVector( m_mapTarget );

	VEC_SK_TAR* pDetectedCROW;
	
	pDetectedCROW = DetectTargetCircle( sSKILL, pActorCaster, pActorTarget, nApplyRange, vTarOrgDir, vTarPosition );

	D3DXVECTOR3 vMax, vMin, vPoss;

	if ( pDetectedCROW )
	{
		VEC_SK_ITER iter = pDetectedCROW->begin();
		VEC_SK_ITER iter_end = pDetectedCROW->end();
		for ( ; iter!=iter_end; ++iter )
		{
			if ( wTARNUM<=wTargetNum )
				break;
			if ( EMTARGET_NET<=wTargetNum )
				break;

			const DWORD dwCrow = iter->emCrow;
			const DWORD dwID = iter->GaeaId;

			D3DXVECTOR3 vTarDir = iter->vPos - pActorCaster->GetPositionActor();
			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );

			if ( IsAvailableTarget( pActorCaster, *iter, sSKILL.m_sBASIC.emIMPACT_SIDE ) &&
				( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle ) )
			{
				sTARIDS[wTargetNum] = STARID(dwCrow, dwID);
				++wTargetNum;
			}
		}
	}

	return TRUE;
} //SKT_SELFZONEOUR

void SkillTargetSystem::EraseVector( TARDISTMAP &vecTar )
{
	if( !vecTar.empty() )
		vecTar.erase ( vecTar.begin(), vecTar.end() );
}

void SkillTargetSystem::SelectFromNearRange( IN TARDISTMAP& vecTAR_TEMP, OUT WORD &nTarget, IN const WORD wTARNUM, IN const WORD wMaxTarget, OUT STARID *sTARIDS )
{
	//	Note : 가까운것부터 가능한 갯수많큼 뽑아내기.
	//
	std::sort( vecTAR_TEMP.begin(), vecTAR_TEMP.end() );
	TARDISTMAP_ITER iter = vecTAR_TEMP.begin();
	TARDISTMAP_ITER iter_end = vecTAR_TEMP.end();
	for( WORD i=nTarget; (iter!=iter_end) && (i<wMaxTarget); ++iter, ++i )
	{
		if( wTARNUM <= nTarget )
			break;

		sTARIDS[nTarget].SET ( (*iter).m_sTARGETID );
		++nTarget;
	}
}

void SkillTargetSystem::AddTarget( ServerActor* pActor, STARID* sTARIDS, WORD& wTARNUM )
{
	sTARIDS[wTARNUM].emCrow	= pActor->GetCrow();
	sTARIDS[wTARNUM].dwID	= pActor->GetGaeaID();
	++wTARNUM;
}

void SkillTargetSystem::AddTarget( const STARGETID& sTargetID, STARID* sTARIDS, WORD& wTARNUM )
{
	sTARIDS[wTARNUM].emCrow	= sTargetID.emCrow;
	sTARIDS[wTARNUM].dwID	= sTargetID.GaeaId;
	++wTARNUM;
}

BOOL SkillTargetSystem::SelectSkillTarget( ServerActor* pActorCaster, 
										  ServerActor* pActorTarget, 
										  const SNATIVEID& sSkillID, 
										  const WORD wSkillLevel, 
										  const D3DXVECTOR3& vPos, 
										  STARID* sTARIDS, 
										  WORD& wTargetNum  )
{

	if ( pActorCaster == NULL || pActorTarget == NULL )
		return FALSE;

	PGLSKILL	pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )							
		return FALSE;

	switch( pSkill->m_sBASIC.emIMPACT_TAR )
	{
	case TAR_SELF:
		switch ( pSkill->m_sBASIC.emIMPACT_REALM )
		{
		case REALM_KEEP_ZONE:
			return FALSE;
		case REALM_SELF:
		case REALM_ZONE:
			return SelectCircle( pActorCaster, pActorTarget,  *pSkill, wSkillLevel, vPos, sTARIDS, wTargetNum );
		}
	case TAR_SPEC:
		return SelectCircle( pActorCaster, pActorTarget,  *pSkill, wSkillLevel, vPos, sTARIDS, wTargetNum );
	case  TAR_SELF_TOSPEC:
		return SelectLine( pActorCaster, pActorTarget,  *pSkill, wSkillLevel, vPos, sTARIDS, wTargetNum );
	case TAR_ZONE:
		return SelectCircle( pActorCaster, pActorTarget,  *pSkill, wSkillLevel, vPos, sTARIDS, wTargetNum );
	case TAR_LAND:
		return SelectCircle( pActorCaster, pActorTarget,  *pSkill, wSkillLevel, vPos, sTARIDS, wTargetNum );
	}

	return FALSE;
} //SelectSkillTarget

SkillTargetSystem::~SkillTargetSystem()
{

}

SkillTargetSystem::SkillTargetSystem()
{

}

BOOL SkillTargetSystem::IsAvailableTarget( ServerActor* pActorCaster, STARGETID &sTarget, EMIMPACT_SIDE emTargetSide )
{
	BOOL bIsAvailableTarget = FALSE;
	GLLandMan* const pLand(pActorCaster->GetLandMan());
	if ( pLand == NULL )
		return FALSE;


	ServerActor* const pTargetActor = pLand->GetActor( sTarget );
	if ( pTargetActor == NULL )
		return FALSE;

	switch (pActorCaster->GetCrow())
	{
	case CROW_PC:
		{
			GLChar* pChar = pLand->GetChar( pActorCaster->GetGaeaID() );

			if ( pChar && EMREACTIONABLE_TRUE == pChar->IsReActionable( pTargetActor, emTargetSide ) )
			{
				bIsAvailableTarget = TRUE;
			}
		}
		break;

	case CROW_MOB:
		{
			bIsAvailableTarget = TRUE;
		}
		break;

	case CROW_SUMMON:
		{
			GLSummonField* pSummon = pLand->GetSummon( pActorCaster->GetGaeaID() );

			if ( pSummon == NULL )
				return FALSE;

			ServerActor* pOwner = pSummon->GetGaeaServer()->GetActor( pLand, pSummon->m_sOwnerID );
			//GLChar* pOwner = pSummon->GetGaeaServer()->GetChar64(pSummon->GetOwnerDbNum(), pSummon->GetOwnerSvrGroupNum());
			if ( pOwner->GetCrow() == CROW_PC )
			{
				GLChar* pOwnerChar = static_cast< GLChar* >( pOwner );
				if ( pOwnerChar && EMREACTIONABLE_TRUE == pOwnerChar->IsReActionable( pTargetActor, emTargetSide ) )
				{
					bIsAvailableTarget = TRUE;
				}
			}
			else
				bIsAvailableTarget = TRUE;
		}
		break;

	default:
		break;
	}

	return bIsAvailableTarget;
}

BOOL SkillTargetSystem::SelectLine( ServerActor* pActorCaster, ServerActor* pActorTarget, const GLSKILL& sSKILL, const WORD wLEVEL, const D3DXVECTOR3& vPos, STARID *sTARIDS, WORD& wTargetNum )
{
	GLLandMan* const pLand(pActorCaster->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wPIERCENUM = sSKILL_DATA.wPIERCENUM;

	float fApplyAngle  = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;
	D3DXVECTOR3 vTarOrgDir( 0, 0, 0 );
	D3DXVECTOR3 vTarPosition( 0, 0, 0);

	wTargetNum = 0;


	STARGETID sTempTargetID;

	//AddTarget( pActorCaster, sTARIDS, wTargetNum );

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM <= wTargetNum )	
		return TRUE;

	int nApplyRange =  4 + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2.0f;

	D3DXVECTOR3			vTarget		= pActorTarget->GetPositionActor();	
	D3DXVECTOR3			vFrom		= pActorCaster->GetPositionActor();


	GLCrow *pMob = pLand->GetCrow(pActorTarget->GetGaeaID());
	GLChar *pChar = pLand->GetChar(pActorCaster->GetGaeaID());

	if ( !pMob || !pChar )
		return FALSE;

	vTarget.y	+= 20 * 0.75f;	
	vFrom.y		+= 20 * 0.75f;

	D3DXVECTOR3			vDir		= vTarget - vFrom;

	D3DXVec3Normalize( &vDir, &vDir );

	//	Note : 장거리형 스킬일 경우 GETSUM_TARRANGE() 가산.
	float				fAttackRange = (float)( pMob->GetBodyRadius() + pChar->GETBODYRADIUS() + pChar->GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );

	nApplyRange += pChar->GETBODYRADIUS() * wPIERCENUM;

	vTarget = vFrom + vDir * fAttackRange;

	EraseVector( m_mapTarget );

	VEC_SK_TAR* pDetectedCROW;

	pDetectedCROW = DetectTargetCircle( sSKILL, pActorCaster, pActorTarget, nApplyRange, vTarOrgDir, vTarPosition );

	D3DXVECTOR3 vMax, vMin, vPoss;

	if ( pDetectedCROW )
	{
		VEC_SK_ITER iter = pDetectedCROW->begin();
		VEC_SK_ITER iter_end = pDetectedCROW->end();
		for ( ; iter!=iter_end; ++iter )
		{
			if ( wPIERCENUM<=wTargetNum )
				break;
			if ( EMTARGET_NET<=wTargetNum )
				break;

			const DWORD dwCrow = iter->emCrow;
			const DWORD dwID = iter->GaeaId;

			GLCrow *pMob = pLand->GetCrow(dwID);
			if (!pMob)
				continue;

			vPoss = pMob->GetPosition();
			vMax = vPoss + D3DXVECTOR3(15,20,15);
			vMin = vPoss + D3DXVECTOR3(-15,0,-15);


			if ( IsAvailableTarget( pActorCaster, *iter, sSKILL.m_sBASIC.emIMPACT_SIDE ) &&
				COLLISION::IsCollisionLineToAABB ( vFrom, vTarget, vMax, vMin ) )
			{
				sTARIDS[wTargetNum] = STARID(dwCrow, dwID);
				++wTargetNum;
			}
		}
	}

	return TRUE;
}

VEC_SK_TAR* SkillTargetSystem::DetectTargetCircle( const GLSKILL& sSKILL, 
												  ServerActor* pActorCaster, 
												  ServerActor* pActorTarget, 
												  int nApplyRange, 
												  D3DXVECTOR3 &vTarOrgDir,
												  D3DXVECTOR3 &vTarPosition  )
{
	GLLandMan* const pLand(pActorCaster->GetLandMan());
	if ( pLand == NULL )
		return NULL;

	switch ( pActorCaster->GetCrow() )
	{
	case CROW_PC:
		{
			if (sSKILL.m_sBASIC.emIMPACT_TAR == TAR_SELF )
			{
				return pLand->Detect_CROW (CROW_EX_MOB, pActorCaster->GetPositionActor(), nApplyRange );
			}

			if ( sSKILL.m_sBASIC.emIMPACT_TAR == TAR_SPEC )
			{
				if ( sSKILL.m_sBASIC.emIMPACT_REALM == REALM_FANWIZE )
				{
					vTarPosition = pActorTarget->GetPositionActor();
					vTarOrgDir = vTarPosition - pActorCaster->GetPositionActor();
					return  pLand->Detect_CROW (CROW_EX_MOB, pActorCaster->GetPositionActor(), nApplyRange );
				}
				else
				{
					return pLand->Detect_CROW (CROW_EX_MOB, pActorTarget->GetPositionActor(), nApplyRange );
				}
			}

			if ( sSKILL.m_sBASIC.emIMPACT_TAR == TAR_ZONE )
			{
				return pLand->Detect_CROW (CROW_EX_MOB, vTarPosition, nApplyRange );
			}

			return pLand->Detect_CROW (CROW_EX_MOB, pActorCaster->GetPositionActor(), nApplyRange );
		}

	case CROW_MOB:
		{
			GLCrow* pMob = pLand->GetCrow( pActorCaster->GetGaeaID() );

			if ( pMob )
			{
				if ( !m_vecDetectID.empty() )
					m_vecDetectID.erase ( m_vecDetectID.begin(), m_vecDetectID.end() );

				if (sSKILL.m_sBASIC.emIMPACT_TAR == TAR_SELF )
				{
					m_vecDetectID = pMob->DetectTarget( pActorCaster->GetPositionActor(), nApplyRange, DWORD(100) );
					return &m_vecDetectID;
				}

				if ( sSKILL.m_sBASIC.emIMPACT_TAR == TAR_SPEC )
				{
					if ( sSKILL.m_sBASIC.emIMPACT_REALM == REALM_FANWIZE )
					{
						vTarPosition = pActorTarget->GetPositionActor();
						vTarOrgDir = vTarPosition - pActorCaster->GetPositionActor();
						m_vecDetectID =   pMob->DetectTarget( pActorCaster->GetPositionActor(), nApplyRange, DWORD(100) );
						return &m_vecDetectID;
					}
					else
					{
						m_vecDetectID =  pMob->DetectTarget( pActorTarget->GetPositionActor(), nApplyRange, DWORD(100) );
						return &m_vecDetectID;
					}
				}

				if ( sSKILL.m_sBASIC.emIMPACT_TAR == TAR_ZONE )
				{
					m_vecDetectID =  pMob->DetectTarget( vTarPosition, nApplyRange, DWORD(100) );
					return &m_vecDetectID;
				}

				m_vecDetectID =  pMob->DetectTarget( pActorCaster->GetPositionActor(), nApplyRange, DWORD(100) );
				return &m_vecDetectID;
			}
			return NULL;
		}

	case CROW_SUMMON:
		{
			GLSummonField* pSummon = pLand->GetSummon(pActorCaster->GetGaeaID() );

			if ( pSummon )
			{
				if ( !m_vecDetectID.empty() )
					m_vecDetectID.erase ( m_vecDetectID.begin(), m_vecDetectID.end() );

				if (sSKILL.m_sBASIC.emIMPACT_TAR == TAR_SELF )
				{
					m_vecDetectID = pSummon->DetectTarget( pActorCaster->GetPositionActor(), nApplyRange, DWORD(100) );
					return &m_vecDetectID;
				}

				if ( sSKILL.m_sBASIC.emIMPACT_TAR == TAR_SPEC )
				{
					if ( sSKILL.m_sBASIC.emIMPACT_REALM == REALM_FANWIZE )
					{
						vTarPosition = pActorTarget->GetPositionActor();
						vTarOrgDir = vTarPosition - pActorCaster->GetPositionActor();
						m_vecDetectID =   pSummon->DetectTarget( pActorCaster->GetPositionActor(), nApplyRange, DWORD(100) );
						return &m_vecDetectID;
					}
					else
					{
						m_vecDetectID =  pSummon->DetectTarget( pActorTarget->GetPositionActor(), nApplyRange, DWORD(100) );
						return &m_vecDetectID;
					}
				}

				if ( sSKILL.m_sBASIC.emIMPACT_TAR == TAR_ZONE )
				{
					m_vecDetectID =  pSummon->DetectTarget( vTarPosition, nApplyRange, DWORD(100) );
					return &m_vecDetectID;
				}

				m_vecDetectID =  pSummon->DetectTarget( pActorCaster->GetPositionActor(), nApplyRange, DWORD(100) );
				return &m_vecDetectID;
			}
			return NULL;
		}
	}
	return NULL;
}
