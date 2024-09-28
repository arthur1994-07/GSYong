#include "../pch.h"
#include "../../RanLogic/stl_Func.h"

#include "../Land/GLLandMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Party/GLPartyField.h"
#include "../Party/GLPartyManagerField.h"
#include "./GLSummonField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//	자기편에게 스킬 체크 ( 자기 위치에 영향 )
BOOL GLSummonField::SKT_SELFZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD                   wTARNUM      = sSKILL_DATA.wTARNUM;
	float                  fDetectRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (pOwner)
	{
		// Note : 주인에게 영향
		//
		D3DXVECTOR3 vOwnerPos = pOwner->GetPosition();

		if ( pLand->IsAbleAttack_Range_LayerSimpleCollision( fDetectRange, GetPosition(), vOwnerPos, 5.f ) )
		{
			m_sTARIDS[m_wTARNUM].emCrow = pOwner->GetCrow();
			m_sTARIDS[m_wTARNUM].dwID   = pOwner->GetGaeaID();
			++m_wTARNUM;
		}

		if ( wTARNUM<=m_wTARNUM )	
			return TRUE;
	}

	// Note : 자기 자신에 영향
	//
	m_sTARIDS[m_wTARNUM].emCrow = GetCrow();
	m_sTARIDS[m_wTARNUM].dwID   = GetGaeaID();
	++m_wTARNUM;

	if ( wTARNUM<=m_wTARNUM )
		return TRUE;
	
	if (!pOwner)
		return TRUE;

	// 파티의 멤버를 검색한다
	//GLPartyFieldMan* pPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
	//GLPARTY_FIELD* pMyParty = pPartyFieldMan->GetParty(m_pOwner->m_dwPartyID);    
	TARDISTMAP Targetmap;
	STARGETID sTempTargetID;

	GLPartyField* const pMyParty = m_pGaeaServer->GetParty(pOwner->GetPartyID());
	if ( pMyParty == NULL )
	{
		// 부활일때만 시체만 체크
		bool bValidBody = !sSKILL.IsRebirth();

		// Note : 주인 소환수 체크
		//
		/*if (!pOwner->m_dwSummonGUIDList.empty())
		{
			SUMMONID_ITER iter    = pOwner->m_dwSummonGUIDList.begin();
			SUMMONID_ITER iterEnd = pOwner->m_dwSummonGUIDList.end();

			for ( ;iter!=iterEnd; ++iter )
			{
				PGLSUMMONFIELD pSummon = pLand->GetSummon( (*iter) );

				if( pSummon && pSummon->GetGaeaID() != GetGaeaID() && (pSummon->IsValidBody() == bValidBody) )
				{
					D3DXVECTOR3 vMemPos = pSummon->GetPosition();
					FLOAT       fLength = D3DXVec3Length ( &D3DXVECTOR3(GetPosition()-vMemPos) );
					if ( fLength <= fDetectRange )
					{
						STARGETID sFindID ( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
						Targetmap.push_back ( STARDIST(fLength,sFindID) );
					}
				}
			}
		}*/
		if ( !pOwner->IsEmptyChildCrow() )
		{
			CROWIDLIST_ITER iter = pOwner->GetChildCrowList().begin();
			CROWIDLIST_ITER iterEnd = pOwner->GetChildCrowList().end();

			for ( ; iter!=iterEnd; ++iter )
			{
				Faction::GLActorID sSummonID = *iter;
				if ( sSummonID.actorType != CROW_SUMMON )
					continue;

				PGLSUMMONFIELD pSummon =
					pLand->GetSummon( sSummonID.actorID_Num );

				if( pSummon &&
					pSummon->GetGaeaID() != GetGaeaID() &&
					( pSummon->IsValidBody() == bValidBody ) )
				{
					D3DXVECTOR3 vMemPos = pSummon->GetPosition();
					FLOAT fLength = D3DXVec3Length(
						&D3DXVECTOR3( GetPosition() - vMemPos ) );
					if ( fLength <= fDetectRange && 
						!pLand->IsLayerSimpleCollision( GetPosition(), vMemPos, 5.f ) )
					{
						STARGETID sFindID( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
						Targetmap.push_back( STARDIST( fLength, sFindID ) );
					}
				}
			}
		}
	} 
	// if !sMyParty
	else
	{
		PartyMemberGaeaIDs arPartyMemberGaeaIDs;
		const DWORD dwMemberSize = pMyParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);

		for ( DWORD i=0; i < dwMemberSize; ++i )
		{
			DWORD dwTargetGUID = arPartyMemberGaeaIDs[i];

			sTempTargetID.emCrow = CROW_PC;
			sTempTargetID.GaeaId   = dwTargetGUID;

			ServerActor* pActor = m_pGaeaServer->GetTarget( pLand, sTempTargetID );
			if (!pActor)
				continue;

			// 부활일때만 죽은 사람 체크
			bool bValidBody = !sSKILL.IsRebirth();

			// Note : 주인 제외
			//
			if (pOwner->GetGaeaID() != dwTargetGUID && (pActor->IsValidBody() == bValidBody) )
			{
				D3DXVECTOR3 vMemPos = pActor->GetPosition();
				float       fLength = D3DXVec3Length ( &D3DXVECTOR3(GetPosition()-vMemPos) );

				if ( fLength <= fDetectRange && 
					!pLand->IsLayerSimpleCollision( GetPosition(), vMemPos, 5.f ) )
				{
					STARGETID sFindID ( pActor->GetCrow(), pActor->GetGaeaID(), vMemPos  );
					Targetmap.push_back ( STARDIST(fLength, sFindID) );
				}
			}

			// Note : 파티 소환수 체크
			//
			PGLCHAR pChar = (PGLCHAR)pActor;

			/*if ( !pChar->m_dwSummonGUIDList.empty() )
			{
				SUMMONID_ITER iter    = pChar->m_dwSummonGUIDList.begin();
				SUMMONID_ITER iterEnd = pChar->m_dwSummonGUIDList.end();

				for ( ;iter!=iterEnd; ++iter )
				{
					PGLSUMMONFIELD pSummon = pLand->GetSummon( (*iter) );

					// Note : 자기 자신 제외
					//
					if( pSummon && pSummon->GetGaeaID() != GetGaeaID() && (pSummon->IsValidBody() == bValidBody) )
					{
						D3DXVECTOR3 vMemPos = pSummon->GetPosition();
						FLOAT       fLength = D3DXVec3Length ( &D3DXVECTOR3(GetPosition()-vMemPos) );
						if ( fLength <= fDetectRange )
						{
							STARGETID sFindID ( pSummon->GETCROW(), pSummon->GetGaeaID(), vMemPos );
							Targetmap.push_back ( STARDIST(fLength,sFindID) );
						}
					}
				}
			}*/

			if ( !pChar->IsEmptyChildCrow() )
			{
				CROWIDLIST_ITER iter = pChar->GetChildCrowList().begin();
				CROWIDLIST_ITER iterEnd = pChar->GetChildCrowList().end();

				for ( ; iter!=iterEnd; ++iter )
				{
					Faction::GLActorID sSummonID = *iter;
					if ( sSummonID.actorType != CROW_SUMMON )
						continue;

					PGLSUMMONFIELD pSummon =
						pLand->GetSummon( sSummonID.actorID_Num );

					if( pSummon &&
						pSummon->GetGaeaID() != GetGaeaID() &&
						( pSummon->IsValidBody() == bValidBody ) )
					{
						D3DXVECTOR3 vMemPos = pSummon->GetPosition();
						FLOAT fLength = D3DXVec3Length(
							&D3DXVECTOR3( GetPosition() - vMemPos ) );
						if ( fLength <= fDetectRange && 
							!pLand->IsLayerSimpleCollision( GetPosition(), vMemPos, 5.f ) )
						{
							STARGETID sFindID( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
							Targetmap.push_back( STARDIST( fLength, sFindID ) );
						}
					}
				}
			}
		}
	} // if !sMyParty end
	
	//	Note : 가까운것부터 가능한 갯수많큼 뽑아내기.
	//
	SelectFromNearRange( Targetmap, m_wTARNUM, wTARNUM, static_cast<WORD>(EMTARGET_NET), m_sTARIDS );

	if ( m_wTARNUM == 0 )
	{
		return FALSE;
	}

	return TRUE;
}

//  적들에게 스킬 체크 ( 자기 위치에 영향 )
BOOL GLSummonField::SKT_SELFZONEENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	//	PC CROW SUMMON 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( GetPosition(), nApplyRange, DWORD(wTARNUM) );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));

			if ( !pActor || pActor->IsNonDamaged() )
				continue;
			
			const DWORD dwCrow = pActor->GetCrow();
			const DWORD dwGaeaID   = pActor->GetGaeaID();

			/*
			if( m_emTYPE == SUMMON_TYPE_WAY_1 ||
				m_emTYPE == SUMMON_TYPE_WAY_2 )
			{
				switch( dwCrow )
				{
				case CROW_PC  :
					{
						GLChar* pUser = dynamic_cast<GLChar*>(pActor);

						if( pUser->IsEntranceExitSkillApply() ) continue;
					}
					break;
				case CROW_MOB :
					{
						GLCrow* pMob = dynamic_cast<GLCrow*>(pActor);

						if( pMob->IsEntranceExitSkillApply() || pMob->IsBoss() || pMob->IsPosHold() ) continue;
					}
					break;
				}
			}*/

			m_sTARIDS[m_wTARNUM] = STARID(dwCrow,dwGaeaID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//  모두에게 스킬 체크 ( 자기 위치에 영향 )
BOOL GLSummonField::SKT_SELFZONANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	//	PC CROW SUMMON 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( GetPosition(), nApplyRange, DWORD(wTARNUM), FALSE );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));

			if ( !pActor || pActor->IsNonDamaged() )
				continue;

			const DWORD dwCrow = pActor->GetCrow();
			const DWORD dwGaeaID   = pActor->GetGaeaID();

			m_sTARIDS[m_wTARNUM] = STARID(dwCrow,dwGaeaID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	자기편에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
BOOL GLSummonField::SKT_TARSPECOUR_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (sTargetID.emCrow == CROW_MOB)
        return FALSE;
	if (!pOwner)
        return FALSE;

	if ( sTargetID.emCrow == CROW_PC )
	{
		// Note : 타겟이 주인이라면
		if ( sTargetID.emCrow == pOwner->GetCrow() && sTargetID.GaeaId == pOwner->GetGaeaID() )
		{
			m_sTARIDS[m_wTARNUM].emCrow = sTargetID.emCrow;
			m_sTARIDS[m_wTARNUM].dwID   = sTargetID.GaeaId;
			++m_wTARNUM;
		}
		else
		{
			//GLPartyFieldMan* pPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
			GLPartyField* const pMyParty = m_pGaeaServer->GetParty(pOwner->GetPartyID());

			if (!pMyParty || !pMyParty->isMemberByGaeaID(sTargetID.GaeaId))
                return FALSE;

			m_sTARIDS[m_wTARNUM].emCrow = sTargetID.emCrow;
			m_sTARIDS[m_wTARNUM].dwID   = sTargetID.GaeaId;
			++m_wTARNUM;
		}
	}
	else if ( sTargetID.emCrow == CROW_SUMMON )
	{
		// Note : 자기 자신이라면
		if ( GetCrow() == sTargetID.emCrow && GetGaeaID() == sTargetID.GaeaId )
		{
			m_sTARIDS[m_wTARNUM].emCrow = sTargetID.emCrow;
			m_sTARIDS[m_wTARNUM].dwID   = sTargetID.GaeaId;
			++m_wTARNUM;
		}
		else
		{
			PGLSUMMONFIELD pSummon = pLand->GetSummon(sTargetID.GaeaId);            
			if (!pSummon)
                return FALSE;


            /*GLChar* pSummonOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
            if (!pSummonOwner)
                return FALSE;

			// Note : 주인이 같다면 OK
			if (pOwner->GetCrow() == pSummonOwner->GetCrow() &&
                pOwner->GetGaeaID() == pSummonOwner->GetGaeaID())*/
			{
				m_sTARIDS[m_wTARNUM].emCrow = sTargetID.emCrow;
				m_sTARIDS[m_wTARNUM].dwID   = sTargetID.GaeaId;
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM == 0 )
	{
		return FALSE;
	}

	return TRUE;
}

//	적들에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
BOOL GLSummonField::SKT_TARSPECENEMY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	ServerActor *pTARGET = m_pGaeaServer->GetTarget(pLand, sTargetID);
	if ( !pTARGET || pTARGET->IsNonDamaged() || IsReActionable(pTARGET, sSKILL.m_sBASIC.emIMPACT_SIDE) == EMREACTIONABLE_FALSE )
        return FALSE;

	m_sTARIDS[m_wTARNUM].emCrow = sTargetID.emCrow;
	m_sTARIDS[m_wTARNUM].dwID   = sTargetID.GaeaId;
	++m_wTARNUM;
	return TRUE;
}

//	모두에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
BOOL GLSummonField::SKT_TARSPECANYBODY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	ServerActor *pTARGET = m_pGaeaServer->GetTarget(pLand, sTargetID);
	if ( !pTARGET || pTARGET->IsNonDamaged() || IsReActionable(pTARGET, sSKILL.m_sBASIC.emIMPACT_SIDE) == EMREACTIONABLE_FALSE )
        return FALSE;

	m_sTARIDS[m_wTARNUM].emCrow = sTargetID.emCrow;
	m_sTARIDS[m_wTARNUM].dwID   = sTargetID.GaeaId;
	++m_wTARNUM;
	return TRUE;
}

//	자기편에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
BOOL GLSummonField::SKT_TARSPECOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD                   wTARNUM      = sSKILL_DATA.wTARNUM;
	float                  fDetectRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	// Note : 타겟 위치 검사
	//
	D3DXVECTOR3 vERROR(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vFindPos = m_pGaeaServer->GetTargetPos(pLand, sTargetID);
	if (vFindPos == vERROR)
		return FALSE;

	// Note : 파티의 멤버를 검색한다
	//GLPartyFieldMan* pPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
	GLPartyField* const pMyParty = m_pGaeaServer->GetParty(pOwner->GetPartyID());
	TARDISTMAP       Targetmap;
	STARGETID        sTempTargetID;

	// Note : 파티가 없다면 위치 주변의 주인과 소환수를 체크한다
	//
	if ( !pMyParty )
	{
		// 부활일때만 시체만 체크
		bool bValidBody = !sSKILL.IsRebirth();

		if ( pOwner->IsValidBody() == bValidBody )
		{
			D3DXVECTOR3 vMemPos = pOwner->GetPosition();
			float       fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );

			if ( fLength <= fDetectRange && 
				!pLand->IsLayerSimpleCollision( vFindPos, vMemPos, 5.f ) )
			{
				STARGETID sFindID ( pOwner->GetCrow(), pOwner->GetGaeaID(), vMemPos  );
				Targetmap.push_back ( STARDIST(fLength, sFindID) );
			}
		}

		// Note : 주인 소환수 체크
		//
		/*if ( !pOwner->m_dwSummonGUIDList.empty() )
		{
			SUMMONID_ITER iter    = pOwner->m_dwSummonGUIDList.begin();
			SUMMONID_ITER iterEnd = pOwner->m_dwSummonGUIDList.end();

			for ( ;iter!=iterEnd; ++iter )
			{
				PGLSUMMONFIELD pSummon = pLand->GetSummon( (*iter) );

				if( pSummon && (pSummon->IsValidBody() == bValidBody) )
				{
					D3DXVECTOR3 vMemPos = pSummon->GetPosition();
					FLOAT       fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
					if ( fLength <= fDetectRange )
					{
						STARGETID sFindID ( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
						Targetmap.push_back ( STARDIST(fLength,sFindID) );
					}
				}
			}
		}*/

		if ( !pOwner->IsEmptyChildCrow() )
		{
			CROWIDLIST_ITER iter = pOwner->GetChildCrowList().begin();
			CROWIDLIST_ITER iterEnd = pOwner->GetChildCrowList().end();

			for ( ; iter!=iterEnd; ++iter )
			{
				Faction::GLActorID sSummonID = *iter;
				if ( sSummonID.actorType != CROW_SUMMON )
					continue;

				PGLSUMMONFIELD pSummon =
					pLand->GetSummon( sSummonID.actorID_Num );

				if( pSummon &&
					pSummon->GetGaeaID() != GetGaeaID() &&
					( pSummon->IsValidBody() == bValidBody ) )
				{
					D3DXVECTOR3 vMemPos = pSummon->GetPosition();
					FLOAT fLength = D3DXVec3Length(
						&D3DXVECTOR3( GetPosition() - vMemPos ) );
					if ( fLength <= fDetectRange && 
						!pLand->IsLayerSimpleCollision( GetPosition(), vMemPos, 5.f ) )
					{
						STARGETID sFindID( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
						Targetmap.push_back( STARDIST( fLength, sFindID ) );
					}
				}
			}
		}
		
	} 
	// if !sMyParty
	else
	{
		PartyMemberGaeaIDs arPartyMemberGaeaIDs;
		const DWORD dwMemberSize = pMyParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);	

		for ( DWORD i = 0; i < dwMemberSize; ++i )
		{
			DWORD dwTargetGUID = arPartyMemberGaeaIDs[i];

			sTempTargetID.emCrow = CROW_PC;
			sTempTargetID.GaeaId   = dwTargetGUID;

			ServerActor* pActor = m_pGaeaServer->GetTarget( pLand, sTempTargetID );
			if ( !pActor )
				continue;

			// 부활일때만 죽은 사람 체크
			bool bValidBody = !sSKILL.IsRebirth();

			if ( pActor->IsValidBody() == bValidBody )
			{
				D3DXVECTOR3 vMemPos = pActor->GetPosition();
				float       fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );

				if ( fLength <= fDetectRange && 
					!pLand->IsLayerSimpleCollision( vFindPos, vMemPos, 5.f ) )
				{
					STARGETID sFindID ( pActor->GetCrow(), pActor->GetGaeaID(), vMemPos  );
					Targetmap.push_back ( STARDIST(fLength, sFindID) );
				}
			}

			// Note : 파티 소환수 체크
			//
			PGLCHAR pChar = (PGLCHAR)pActor;

			/*if ( !pChar->m_dwSummonGUIDList.empty() )
			{
				SUMMONID_ITER iter    = pChar->m_dwSummonGUIDList.begin();
				SUMMONID_ITER iterEnd = pChar->m_dwSummonGUIDList.end();

				for ( ;iter!=iterEnd; ++iter )
				{
					PGLSUMMONFIELD pSummon = pLand->GetSummon( (*iter) );

					if( pSummon && (pSummon->IsValidBody() == bValidBody) )
					{
						D3DXVECTOR3 vMemPos = pSummon->GetPosition();
						FLOAT       fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
						if ( fLength <= fDetectRange )
						{
							STARGETID sFindID ( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
							Targetmap.push_back ( STARDIST(fLength,sFindID) );
						}
					}
				}
			}*/

			if ( !pChar->IsEmptyChildCrow() )
			{
				CROWIDLIST_ITER iter = pChar->GetChildCrowList().begin();
				CROWIDLIST_ITER iterEnd = pChar->GetChildCrowList().end();

				for ( ; iter!=iterEnd; ++iter )
				{
					Faction::GLActorID sSummonID = *iter;
					if ( sSummonID.actorType != CROW_SUMMON )
						continue;

					PGLSUMMONFIELD pSummon =
						pLand->GetSummon( sSummonID.actorID_Num );

					if( pSummon &&
						pSummon->GetGaeaID() != GetGaeaID() &&
						( pSummon->IsValidBody() == bValidBody ) )
					{
						D3DXVECTOR3 vMemPos = pSummon->GetPosition();
						FLOAT fLength = D3DXVec3Length(
							&D3DXVECTOR3( GetPosition() - vMemPos ) );
						if ( fLength <= fDetectRange && 
							!pLand->IsLayerSimpleCollision( GetPosition(), vMemPos, 5.f ) )
						{
							STARGETID sFindID( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
							Targetmap.push_back( STARDIST( fLength, sFindID ) );
						}
					}
				}
			}
		}
	} // if !sMyParty Else End

	//	Note : 가까운것부터 가능한 갯수많큼 뽑아내기.
	//
	SelectFromNearRange( Targetmap, m_wTARNUM, wTARNUM, static_cast<WORD>(EMTARGET_NET), m_sTARIDS );

	if ( m_wTARNUM == 0 )
		return FALSE;

	return TRUE;
}

//	적들에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
BOOL GLSummonField::SKT_TARSPECENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	D3DXVECTOR3 vERROR(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vFindPos = m_pGaeaServer->GetTargetPos(pLand, sTargetID);
	if (vFindPos == vERROR)
        return FALSE;

	//	PC CROW SUMMON 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( vFindPos, nApplyRange, DWORD(wTARNUM) );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));

			if ( !pActor || pActor->IsNonDamaged() )
				continue;

			const DWORD dwCrow = pActor->GetCrow();
			const DWORD dwGaeaID   = pActor->GetGaeaID();

			m_sTARIDS[m_wTARNUM] = STARID(dwCrow,dwGaeaID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	모두에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
BOOL GLSummonField::SKT_TARSPECANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	D3DXVECTOR3 vERROR(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vFindPos = m_pGaeaServer->GetTargetPos(pLand, sTargetID);
	if (vFindPos == vERROR)
        return FALSE;

	//	PC CROW SUMMON 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( vFindPos, nApplyRange, DWORD(wTARNUM), FALSE );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));

			if ( !pActor || pActor->IsNonDamaged() )
				continue;

			const DWORD dwCrow = pActor->GetCrow();
			const DWORD dwGaeaID   = pActor->GetGaeaID();

			m_sTARIDS[m_wTARNUM] = STARID(dwCrow,dwGaeaID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	'목표'를 향한 부채꼴 모양.
BOOL GLSummonField::SKT_TARSPECFANWIZE ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	ServerActor *pTarget = m_pGaeaServer->GetTarget(pLand, sTargetID);
	if (!pTarget)
        return FALSE;

	D3DXVECTOR3 vTarPos = pTarget->GetPosition();

	D3DXVECTOR3 vTarOrgDir = vTarPos - GetPosition();
	const D3DXVECTOR3 &vFindPos = GetPosition ();

	float fApplyAngle = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;

	//	첫 표적 넣음.
	m_sTARIDS[m_wTARNUM].emCrow = sTargetID.emCrow;
	m_sTARIDS[m_wTARNUM].dwID   = sTargetID.GaeaId;
	m_wTARNUM++;

	//	PC CROW SUMMON 검사
	int nApplyRange = int ( pTarget->GetBodyRadius() + GETBODYRADIUS() + GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( vFindPos, nApplyRange, DWORD(wTARNUM) );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if (wTARNUM <= m_wTARNUM)
                break;
			if (EMTARGET_NET <= m_wTARNUM)
                break;

			if ( (iter->emCrow == sTargetID.emCrow) && (sTargetID.GaeaId == iter->GaeaId) )
                continue;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));

			if ( !pActor || pActor->IsNonDamaged() )
				continue;

			const DWORD dwCrow = pActor->GetCrow();
			const DWORD dwID = pActor->GetGaeaID();

			m_sTARIDS[m_wTARNUM] = STARID(dwCrow, dwID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

// 적에게만 자기 위치에서 대상 위치까지. ( 화살 관통 )
BOOL GLSummonField::SKT_TARSELFTOSPEC ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wPIERCENUM = sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE();

	ServerActor *pTarget = m_pGaeaServer->GetTarget(pLand, sTargetID);
	if (!pTarget)
        return FALSE;

	D3DXVECTOR3 vTarget = sTargetID.vPos;	vTarget.y += 12.0f;
	D3DXVECTOR3 vFrom = m_vPosition;				vFrom.y += 12.0f;

	D3DXVECTOR3 vDir = vTarget - vFrom;
	D3DXVec3Normalize ( &vDir, &vDir );

	//	Note : 장거리형 스킬일 경우 GETSUM_TARRANGE() 가산.
	float fDetectRange = (float) ( pTarget->GetBodyRadius() + GETBODYRADIUS() + GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );
	vTarget = vFrom + vDir * fDetectRange;

	VEC_SK_TAR vecTargetID = DetectTarget ( vFrom, vTarget, wPIERCENUM );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wPIERCENUM<=m_wTARNUM )	break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));
			if (!pActor || pActor->IsNonDamaged())
				continue;

			const DWORD dwCrow = pActor->GetCrow();
			const DWORD dwGaeaID   = pActor->GetGaeaID();

			m_sTARIDS[m_wTARNUM] = STARID(dwCrow,dwGaeaID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	자기편에게 스킬 체크 ( 위치에 영향 )
BOOL GLSummonField::SKT_TARZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	if ( !pvTarPos )
	{
		return FALSE;
	}

	if ( !pvTarPos )
	{
		return FALSE;
	}

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD                   wTARNUM      = sSKILL_DATA.wTARNUM;
	float                  fDetectRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	// Note : 타겟 위치 검사
	//
	D3DXVECTOR3 vFindPos = *pvTarPos;

	// 파티의 멤버를 검색한다
	//GLPartyFieldMan* pPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return FALSE;

	GLPartyField* const pMyParty = m_pGaeaServer->GetParty(pOwner->GetPartyID());
	TARDISTMAP Targetmap;
	STARGETID sTempTargetID;

	// Note : 파티가 없다면 위치 주변의 주인과 소환수를 체크한다
	//
	if ( !pMyParty )
	{
		// 부활일때만 시체만 체크
		bool bValidBody = !sSKILL.IsRebirth();

		if (pOwner->IsValidBody() == bValidBody)
		{
			D3DXVECTOR3 vMemPos = pOwner->GetPosition();
			float       fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );

			if ( fLength <= fDetectRange && 
				!pLand->IsLayerSimpleCollision( vFindPos, vMemPos, 5.f ) )
			{
				STARGETID sFindID(pOwner->GetCrow(), pOwner->GetGaeaID(), vMemPos);
				Targetmap.push_back ( STARDIST(fLength, sFindID) );
			}
		}

		// Note : 주인 소환수 체크
		//
		/*if ( !pOwner->m_dwSummonGUIDList.empty() )
		{
			SUMMONID_ITER iter    = pOwner->m_dwSummonGUIDList.begin();
			SUMMONID_ITER iterEnd = pOwner->m_dwSummonGUIDList.end();

			for ( ;iter!=iterEnd; ++iter )
			{
				PGLSUMMONFIELD pSummon = pLand->GetSummon( (*iter) );

				if( pSummon && (pSummon->IsValidBody() == bValidBody) )
				{
					D3DXVECTOR3 vMemPos = pSummon->GetPosition();
					FLOAT       fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
					if ( fLength <= fDetectRange )
					{
						STARGETID sFindID ( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
						Targetmap.push_back ( STARDIST(fLength,sFindID) );
					}
				}
			}
		}*/

		if ( !pOwner->IsEmptyChildCrow() )
		{
			CROWIDLIST_ITER iter = pOwner->GetChildCrowList().begin();
			CROWIDLIST_ITER iterEnd = pOwner->GetChildCrowList().end();

			for ( ; iter!=iterEnd; ++iter )
			{
				Faction::GLActorID sSummonID = *iter;
				if ( sSummonID.actorType != CROW_SUMMON )
					continue;

				PGLSUMMONFIELD pSummon =
					pLand->GetSummon( sSummonID.actorID_Num );

				if( pSummon &&
					pSummon->GetGaeaID() != GetGaeaID() &&
					( pSummon->IsValidBody() == bValidBody ) )
				{
					D3DXVECTOR3 vMemPos = pSummon->GetPosition();
					FLOAT fLength = D3DXVec3Length(
						&D3DXVECTOR3( GetPosition() - vMemPos ) );
					if ( fLength <= fDetectRange && 
						!pLand->IsLayerSimpleCollision( GetPosition(), vMemPos, 5.f ) )
					{
						STARGETID sFindID( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
						Targetmap.push_back( STARDIST( fLength, sFindID ) );
					}
				}
			}
		}
	} 
	// if !sMyParty
	else
	{
		PartyMemberGaeaIDs arPartyMemberGaeaIDs;
		const DWORD dwMemberSize = pMyParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);

		for ( DWORD i = 0; i < dwMemberSize; ++i )
		{
			DWORD dwTargetGUID = arPartyMemberGaeaIDs[i];

			sTempTargetID.emCrow = CROW_PC;
			sTempTargetID.GaeaId   = dwTargetGUID;

			ServerActor* pActor = m_pGaeaServer->GetTarget( pLand, sTempTargetID );
			if ( !pActor )
				continue;

			// 부활일때만 죽은 사람 체크
			bool bValidBody = !sSKILL.IsRebirth();

			if ( pActor->IsValidBody() == bValidBody )
			{
				D3DXVECTOR3 vMemPos = pActor->GetPosition();
				float       fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );

				if ( fLength <= fDetectRange && 
					!pLand->IsLayerSimpleCollision( vFindPos, vMemPos, 5.f ) )
				{
					STARGETID sFindID ( pActor->GetCrow(), pActor->GetGaeaID(), vMemPos  );
					Targetmap.push_back ( STARDIST(fLength, sFindID) );
				}
			}

			// Note : 파티 소환수 체크
			//
			PGLCHAR pChar = (PGLCHAR)pActor;

			/*if ( !pChar->m_dwSummonGUIDList.empty() )
			{
				SUMMONID_ITER iter    = pChar->m_dwSummonGUIDList.begin();
				SUMMONID_ITER iterEnd = pChar->m_dwSummonGUIDList.end();

				for ( ;iter!=iterEnd; ++iter )
				{
					PGLSUMMONFIELD pSummon = pLand->GetSummon( (*iter) );

					if( pSummon && (pSummon->IsValidBody() == bValidBody) )
					{
						D3DXVECTOR3 vMemPos = pSummon->GetPosition();
						FLOAT       fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
						if ( fLength <= fDetectRange )
						{
							STARGETID sFindID ( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
							Targetmap.push_back ( STARDIST(fLength,sFindID) );
						}
					}
				}
			}*/

			if ( !pChar->IsEmptyChildCrow() )
			{
				CROWIDLIST_ITER iter = pChar->GetChildCrowList().begin();
				CROWIDLIST_ITER iterEnd = pChar->GetChildCrowList().end();

				for ( ; iter!=iterEnd; ++iter )
				{
					Faction::GLActorID sSummonID = *iter;
					if ( sSummonID.actorType != CROW_SUMMON )
						continue;

					PGLSUMMONFIELD pSummon =
						pLand->GetSummon( sSummonID.actorID_Num );

					if( pSummon &&
						pSummon->GetGaeaID() != GetGaeaID() &&
						( pSummon->IsValidBody() == bValidBody ) )
					{
						D3DXVECTOR3 vMemPos = pSummon->GetPosition();
						FLOAT fLength = D3DXVec3Length(
							&D3DXVECTOR3( GetPosition() - vMemPos ) );
						if ( fLength <= fDetectRange && 
							!pLand->IsLayerSimpleCollision( GetPosition(), vMemPos, 5.f ) )
						{
							STARGETID sFindID( pSummon->GetCrow(), pSummon->GetGaeaID(), vMemPos );
							Targetmap.push_back( STARDIST( fLength, sFindID ) );
						}
					}
				}
			}
		}
	} // if !sMyParty Else End

	//	Note : 가까운것부터 가능한 갯수많큼 뽑아내기.
	//
	SelectFromNearRange( Targetmap, m_wTARNUM, wTARNUM, static_cast<WORD>(EMTARGET_NET), m_sTARIDS );

	if ( m_wTARNUM == 0 )
		return FALSE;

	return TRUE;
}

//	적들에게 스킬 체크 ( 위치에 영향 )
BOOL GLSummonField::SKT_TARZONEENEMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	if (!pvTarPos)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_vTARPOS = *pvTarPos;
	D3DXVECTOR3 vFindPos = *pvTarPos;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	//	PC CROW SUMMON 검사
	int nApplyRange = int ( static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( vFindPos, nApplyRange, DWORD(wTARNUM) );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for ( ; iter != vecTargetID.end(); ++iter)
		{
			if (wTARNUM <= m_wTARNUM)
                break;
			if (EMTARGET_NET <= m_wTARNUM)
                break;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));
			if (!pActor || pActor->IsNonDamaged())
				continue;

			const DWORD dwCrow = pActor->GetCrow();
			const DWORD dwGaeaID   = pActor->GetGaeaID();

			m_sTARIDS[m_wTARNUM] = STARID(dwCrow,dwGaeaID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	모두에게 스킬 체크 ( 위치에 영향 )
BOOL GLSummonField::SKT_TARZONEANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if (!pOwner)
		return FALSE;

	if (!pvTarPos)
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	D3DXVECTOR3 vFindPos = *pvTarPos;

	//	PC CROW SUMMON 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( vFindPos, nApplyRange, DWORD(wTARNUM), FALSE );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if (wTARNUM<=m_wTARNUM)
                break;
			if (EMTARGET_NET<=m_wTARNUM)
                break;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));

			if (!pActor || pActor->IsNonDamaged())
				continue;

			const DWORD dwCrow = pActor->GetCrow();
			const DWORD dwID   = pActor->GetGaeaID();

			m_sTARIDS[m_wTARNUM] = STARID(dwCrow, dwID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )
		return FALSE;

	return TRUE;
}

BOOL GLSummonField::UpdateSkillTarget ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const SNATIVEID &sCheckSkill, const WORD wLevel )
{
	m_wTARNUM = 0;

	//	Note : 스킬 정보 가져옴.
	//
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sCheckSkill.wMainID, sCheckSkill.wSubID );
	if ( !pSkill )
		return FALSE;

	switch ( pSkill->m_sBASIC.emIMPACT_TAR )
	{
	case TAR_SELF:
		switch ( pSkill->m_sBASIC.emIMPACT_REALM )
		{
		case REALM_KEEP_ZONE:	return FALSE;

		case REALM_SELF:
			m_sTARIDS[m_wTARNUM].emCrow = GETCROW();
			m_sTARIDS[m_wTARNUM].dwID = GetGaeaID();
			++m_wTARNUM;
			return TRUE;

		case REALM_ZONE:
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case EMIMPACT_SIDE_PARTY:
				return SKT_SELFZONEOUR ( sTargetID, pvTarPos, *pSkill, wLevel );
			case EMIMPACT_SIDE_ENEMY:
				return SKT_SELFZONEENEMY ( sTargetID, pvTarPos, *pSkill, wLevel );
			case EMIMPACT_SIDE_ANYBODY:
				return SKT_SELFZONANYBODY ( sTargetID, pvTarPos, *pSkill, wLevel );
			};
			return FALSE;
		};
		return FALSE;

	case TAR_SPEC:
		{
			if ( sTargetID.GaeaId == EMTARGET_NULL )
				return FALSE;

			switch ( pSkill->m_sBASIC.emIMPACT_REALM )
			{
			case REALM_KEEP_ZONE:	return FALSE;

			case REALM_SELF:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY:
					return SKT_TARSPECOUR_SELF ( sTargetID, pvTarPos, *pSkill, wLevel );
				case EMIMPACT_SIDE_ENEMY:
					return SKT_TARSPECENEMY_SELF ( sTargetID, pvTarPos, *pSkill, wLevel );
				case EMIMPACT_SIDE_ANYBODY:
					return SKT_TARSPECANYBODY_SELF ( sTargetID, pvTarPos, *pSkill, wLevel );
				};
				return TRUE;

			case REALM_ZONE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY:
					return SKT_TARSPECOUR ( sTargetID, pvTarPos, *pSkill, wLevel );
				case EMIMPACT_SIDE_ENEMY:	
					return SKT_TARSPECENEMY ( sTargetID, pvTarPos, *pSkill, wLevel );
				case EMIMPACT_SIDE_ANYBODY:
					return SKT_TARSPECANYBODY ( sTargetID, pvTarPos, *pSkill, wLevel );
				};
				return FALSE;

			case REALM_FANWIZE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY:
					return FALSE;
				case EMIMPACT_SIDE_ENEMY:
					return SKT_TARSPECFANWIZE ( sTargetID, pvTarPos, *pSkill, wLevel );
				case EMIMPACT_SIDE_ANYBODY:
					return FALSE;
				};
				return FALSE;
			};
		}
		break;

	case TAR_SELF_TOSPEC:
		switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case EMIMPACT_SIDE_PARTY:
			return FALSE;
		case EMIMPACT_SIDE_ENEMY:
			return SKT_TARSELFTOSPEC ( sTargetID, pvTarPos, *pSkill, wLevel );
		case EMIMPACT_SIDE_ANYBODY:
			return FALSE;
		};
		return FALSE;

	case TAR_ZONE:
		if ( !pvTarPos )
			return FALSE;
		switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case EMIMPACT_SIDE_PARTY:
			return SKT_TARZONEOUR ( sTargetID, pvTarPos, *pSkill, wLevel );
		case EMIMPACT_SIDE_ENEMY:
			return SKT_TARZONEENEMY ( sTargetID, pvTarPos, *pSkill, wLevel );
		case EMIMPACT_SIDE_ANYBODY:
			return SKT_TARZONEANYBODY ( sTargetID, pvTarPos, *pSkill, wLevel );
		};
		return FALSE;
	};

	return FALSE;
}


void GLSummonField::SelectFromNearRange( IN TARDISTMAP &vecTAR_TEMP, 
										 OUT WORD &nTarget, 
										 IN const WORD wTARNUM, 
										 IN const WORD wMaxTarget, 
										 OUT STARID *sTARIDS  )
{
	//	Note : 가까운것부터 가능한 갯수많큼 뽑아내기.
	std::sort ( vecTAR_TEMP.begin(), vecTAR_TEMP.end(), std_afunc::CCompareStarDist() );
	TARDISTMAP_ITER iter     = vecTAR_TEMP.begin();
	TARDISTMAP_ITER iter_end = vecTAR_TEMP.end();
	for ( WORD i=nTarget; (iter!=iter_end) && (i<wMaxTarget); ++iter, ++i )
	{
		if ( wTARNUM<=nTarget )		break;
		if ( wMaxTarget<=nTarget )	break;

		sTARIDS[nTarget].SET ( (*iter).m_sTARGETID );
		++nTarget;
	}

}

BOOL GLSummonField::SelfZoneInCrow ( STARGETID &sTargetID, const PGLSKILL pSkill, const WORD wLEVEL )
{
	sTargetID = STARID(CROW_MOB, EMTARGET_NULL);

	GLLandMan* const pLand(GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );

	if (!pOwner) return FALSE;
	if( pSkill == NULL ) return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	BOOL bEnemy = FALSE;
	switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
	{
	case EMIMPACT_SIDE_PARTY:
	case EMIMPACT_SIDE_ANYBODY:
		bEnemy = FALSE;
		break;

	case EMIMPACT_SIDE_ENEMY:
		bEnemy = TRUE;
		break;
	}
	
	//	PC CROW SUMMON 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( GetPosition(), nApplyRange, DWORD(wTARNUM), bEnemy );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();
		for( ; iter != vecTargetID.end(); ++iter )
		{
			//if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			ServerActor* pActor = m_pGaeaServer->GetTarget(pLand, (*iter));

			if ( !pActor || pActor->IsNonDamaged() )
				continue;

			const DWORD dwCrow   = pActor->GetCrow();
			const DWORD dwGaeaID = pActor->GetGaeaID();

			if(  dwCrow != CROW_MOB && dwCrow != CROW_PC ) continue;

			/*
			switch( dwCrow )
			{
			case CROW_PC  :
				{
					GLChar* pUser = dynamic_cast<GLChar*>(pActor);

					if( pUser->IsEntranceExitSkillApply() ) continue;
				}
				break;
			case CROW_MOB :
				{
					GLCrow* pMob = dynamic_cast<GLCrow*>(pActor);

					if( pMob->IsEntranceExitSkillApply() || pMob->IsBoss() || pMob->IsPosHold() ) continue;
				}
				break;
			}*/
			
			m_wTARNUM = 0;
			// 하나만 찾으면 된다.
			sTargetID = STARID(dwCrow,dwGaeaID);

			return TRUE;
		}
	}
	
	return FALSE;
}