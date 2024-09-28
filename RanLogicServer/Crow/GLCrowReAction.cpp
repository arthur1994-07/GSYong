#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/stl_Func.h"

#include "./GLCrow.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Util/StlFuncServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/**
 * �� ĳ���� �˻�.
 */
STARGETID GLCrow::FindEnemyChar ()
{
	STARGETID targetID;
	targetID.RESET();

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return targetID;

	LIST_CHAR			listCharacters;

	EMCROWACT_UP emACT_UP		= GetActPattern();
	EMATT_RGTYPE emAttRgType	= GetAttackRangeType();
	int		nRange = (int) m_pCrowData->m_wViewRange;

	// [shhan][2015.09.01] rm #1631 - [��ų]�þ����� ȿ�� ���� ��û
	//
	// �� �þ� ���� GETVISIBLESIGHT() �۴ٸ� �þ߸� GETVISIBLESIGHT() ���� ����.
	// ���� Ÿ���� �� ã���� �Ѵ�.
	//
	// (GETVISIBLESIGHT()+1) �� Ȥ�ó� float * �� �Ǹ鼭 ���� ������ �� �־ 1�� ����.
	// 1 ������ �ֱ��������Ƿ� ���� �����ϰ� �ص� �������.
	//
	if ( (GETVISIBLESIGHT()+1) < nRange )
	{
		nRange = GETVISIBLESIGHT();
	}
	

	for( DWORD i = 0; i < m_dwPC_AMOUNT && i < m_vecPC_VIEW.size(); ++i )
	{
		DWORD		dwGAEAID = m_vecPC_VIEW[i];
		GLChar*		pChar	 = m_pGaeaServer->GetChar( dwGAEAID );
		if ( !pChar )
			continue;

		if( pChar->IsSafeTime()		||
			pChar->IsSafeZone()		||
		    !pChar->IsValidBody()   ||
			!pChar->IsVisibleDetect( this ) )
		{
			 continue;
		} //if

		// ���̶�� �Ǵ��ϰ� ������������ �Ǻ��Ѵ�;
		if ( EMREACTIONABLE_FALSE == IsReactionable( pChar, EMIMPACT_SIDE_ENEMY ) )
			continue;

		//	��ýÿ��� ���� �ɸ��͸� �������� ����.
		if( pChar->GETCONFTING()->IsCONFRONTING() )
			continue;

		BOOL		bTarget		= FALSE;
		float		fTarLeng	= FLT_MAX;
		if( emAttRgType == EMATT_SHORT )
		{
			//	Note : �ٰŸ� ������ ���.
			if( pChar->IsFreeARSlot() || pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) )
			{
				D3DXVECTOR3 vLengA = pChar->GetPosition() - m_vPosition;
				fTarLeng = D3DXVec3Length( &vLengA );
				if( fTarLeng <= nRange )
					bTarget = TRUE;
			} //if
		}
		else
		{
			//	Note : ��Ÿ� ������ ���.
			D3DXVECTOR3		vLengA = pChar->GetPosition() - m_vPosition;
			fTarLeng = D3DXVec3Length( &vLengA );
			if( fTarLeng <= nRange )
				bTarget = TRUE;
		} //if..else

		if( !bTarget )
			continue;

		switch( emACT_UP )
		{
		case EMCROWACT_UP_LOWLEVEL:
			if( pChar->GETLEVEL() > GETLEVEL() )
				bTarget = FALSE;
			break;
		case EMCROWACT_UP_LOWHP:
			if( pChar->GETHP() > (int)GETHP() )
				bTarget = FALSE;
			break;

		case EMCROWACT_UP_BRIGHT:
			if( pChar->GETBRIGHT() != BRIGHT_LIGHT )
				bTarget = FALSE;
			break;

		case EMCROWACT_UP_DARK:
			if( pChar->GETBRIGHT() != BRIGHT_DARK )
				bTarget = FALSE;
			break;

		case EMCROWACT_UP_BLOW:
			if( !pChar->ISSTATEBLOW() )
				bTarget = FALSE;
			break;

		case EMCROWACT_UP_ARMER:
			if( !pChar->VALID_SLOT_ITEM( pChar->GetCurRHand() ) )
				bTarget = FALSE;
			break;

		case EMCROWACT_UP_RUNNER:
			if( !pChar->IsRunning() )
				bTarget = FALSE;
			break;
		}; //switch

		if( !bTarget )
			continue;

		listCharacters.push_back( std_afunc::CHARDIST( fTarLeng, pChar ) );
	} //for

    
	/*
	for( DWORD i = 0; i < m_vecMob_VIEW.size(); i++ )
	{
		GLCrow*		pMob = pLand->GetCrow( m_vecMob_VIEW[i] );
		if( !pMob )
			continue;

		if( !pMob->IsValidBody() )
            continue;

        // ���� ���� �����ؾ��� ��� �ּ� Ǯ�� ���⿡ �ڵ带 �߰��ϸ� �˴ϴ�.;       
	}
	*/

	if( !listCharacters.empty() )
	{
		listCharacters.sort( std_afunc::CCompareCharDist() );
		const std_afunc::CHARDIST &Target = *listCharacters.begin();

		if( Target.pChar )
			return STARGETID( CROW_PC, Target.pChar->GetGaeaID(), Target.pChar->GetPosition() );
	} //if

	LIST_SUMMON			listSummons;
	// Note : ĳ���͸� �߰� ���Ѱ�� ��ȯ���� ã�´�.
	for( DWORD i = 0; i < m_dwSUMMON_AMOUNT && i < m_vecSUMMON_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecSUMMON_VIEW[i];
		GLSummonField* pSummon = pLand->GetSummon(dwGAEAID);
		if (!pSummon)
            continue;

		if (!pSummon->IsValidBody())
            continue;

		if ( pSummon->IsNonTargetable() )
			continue;

		if ( !pSummon->IsVisibleDetect( this ) )
			continue;

		// ���̶�� �Ǵ��ϰ� ������������ �Ǻ��Ѵ�;
		if ( EMREACTIONABLE_FALSE == IsReactionable( pSummon, EMIMPACT_SIDE_ENEMY ) )
			continue;

		ServerActor* pSummonOwner = m_pGaeaServer->GetActor( pLand, pSummon->m_sOwnerID );
        //GLChar* pSummonOwner = m_pGaeaServer->GetChar64(pSummon->GetOwnerDbNum(), pSummon->GetOwnerSvrGroupNum());
        if (!pSummonOwner)
            continue;

		//	��ýÿ��� ���� �������� ����.
		if (pSummonOwner->GETCONFTING()->IsCONFRONTING())
            continue;

		BOOL bTarget = FALSE;
		float fTarLeng = FLT_MAX;

		if ( pSummon->GetARoundSlot().GetAmount() >= GLCONST_SUMMON::wMaxMobTracking[ pSummon->m_emTYPE ] )
		{
			continue;
		}

		if ( emAttRgType==EMATT_SHORT )
		{
			//	Note : �ٰŸ� ������ ���.
			if ( pSummon->IsFreeARSlot() || pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) )
			{
				D3DXVECTOR3 vLengA = pSummon->GetPosition() - m_vPosition;
				fTarLeng = D3DXVec3Length(&vLengA);
				if ( fTarLeng <= nRange )
					bTarget = TRUE;
			} //if
		}
		else
		{
			//	Note : ��Ÿ� ������ ���.
			D3DXVECTOR3 vLengA = pSummon->GetPosition() - m_vPosition;
			fTarLeng = D3DXVec3Length(&vLengA);
			if( fTarLeng <= nRange )
				bTarget = TRUE;
		} //if..else

		if ( pSummonOwner->GetCrow() == CROW_PC )
		{
			GLChar* pSummonOwnerChar = static_cast< GLChar* >( pSummonOwner );
			switch( emACT_UP )
			{
			default:	
				break;

			case EMCROWACT_UP_BRIGHT:
				if ( pSummonOwnerChar->GETBRIGHT() != BRIGHT_LIGHT )
					bTarget = FALSE;
				break;

			case EMCROWACT_UP_DARK:
				if ( pSummonOwnerChar->GETBRIGHT() != BRIGHT_DARK )
					bTarget = FALSE;
				break;
			} //switch
		}

		if( !bTarget )
			continue;

		listSummons.push_back ( std_afunc::SUMMONDIST(fTarLeng,pSummon) );
	} //for

	if ( !listSummons.empty() )
	{
		listSummons.sort( std_afunc::CCompareSummonDist() );
		const std_afunc::SUMMONDIST &Target = *listSummons.begin();
		if( Target.pSummon )
			return STARGETID( CROW_SUMMON, Target.pSummon->GetGaeaID(), Target.pSummon->GetPosition() );
	} //if

	return targetID;
}


VEC_SK_TAR GLCrow::DetectTarget ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind )
{
	VEC_SK_TAR vecTargetID;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return vecTargetID;

	float fTarLeng(0.0f);
	D3DXVECTOR3 vMax, vMin, vPos;

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<m_vecPC_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecPC_VIEW[i];
		GLChar *pChar = m_pGaeaServer->GetChar ( dwGAEAID );
		if ( !pChar )				continue;
		if ( pChar->IsSafeTime() )	continue;
		if ( pChar->IsSafeZone() )	continue;
		if ( pChar->GETCONFTING()->IsCONFRONTING() ) continue;
		if ( pChar->IsNonTargetable() ) continue;

		// ���̶�� �Ǵ��ϰ� ������������ �Ǻ��Ѵ�;
		if ( EMREACTIONABLE_FALSE == IsReactionable( pChar, EMIMPACT_SIDE_ENEMY ) )
			continue;
		
		vPos = pChar->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin ) )
		{
			vecTargetID.push_back( STARGETID(pChar->GetCrow(),  pChar->GetGaeaID(), pChar->GetPosition() ) );
		}
	}

	for ( DWORD i=0; i<m_dwSUMMON_AMOUNT && i<m_vecSUMMON_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecSUMMON_VIEW[i];
		GLSummonField *pSummon = pLand->GetSummon(dwGAEAID);
		if (!pSummon)
            continue;
		if ( pSummon->IsNonTargetable() )
			continue;

		// ���̶�� �Ǵ��ϰ� ������������ �Ǻ��Ѵ�;
		if ( EMREACTIONABLE_FALSE == IsReactionable( pSummon, EMIMPACT_SIDE_ENEMY ) )
			continue;

		ServerActor* pSummonOwner = m_pGaeaServer->GetActor( pLand, pSummon->m_sOwnerID );
        //GLChar* pSummonOwner = m_pGaeaServer->GetChar64(pSummon->GetOwnerDbNum(), pSummon->GetOwnerSvrGroupNum());
        if (!pSummonOwner)
            continue;
		if (pSummonOwner->IsSafeTime())
            continue;
		if (pSummonOwner->IsSafeZone())
            continue;
		if (pSummonOwner->GETCONFTING()->IsCONFRONTING())
            continue;

		vPos = pSummon->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin ) )
		{
			vecTargetID.push_back( STARGETID(pSummon->GetCrow(),  pSummon->GetGaeaID(), pSummon->GetPosition() ) );
		}
	}

	if( vecTargetID.empty() ) return vecTargetID;

	std_afunc::CCompareTargetDist Comp(D3DXVECTOR3(0.0f,0.0f,0.0f));
	std::sort ( vecTargetID.begin(), vecTargetID.end(), Comp );
	if( vecTargetID.size() > dwMaxFind )
		vecTargetID.erase( vecTargetID.begin()+dwMaxFind, vecTargetID.end() );

	return vecTargetID;
}


VEC_SK_TAR GLCrow::DetectTarget ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind )
{
	VEC_SK_TAR vecTargetID;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return vecTargetID;

	float fTarLeng(0.0f);
	D3DXVECTOR3 vMax, vMin, vPos;

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<m_vecPC_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecPC_VIEW[i];
		GLChar *pChar = m_pGaeaServer->GetChar ( dwGAEAID );
		if ( !pChar )
			continue;

		if ( pChar->IsSafeTime() )
			continue;

		if ( pChar->IsSafeZone() )
			continue;

		if ( pChar->GETCONFTING()->IsCONFRONTING() )
			continue;

		if ( pChar->IsNonTargetable() )
			continue;

		// ���̶�� �Ǵ��ϰ� ������������ �Ǻ��Ѵ�;
		if ( EMREACTIONABLE_FALSE == IsReactionable( pChar, EMIMPACT_SIDE_ENEMY ) )
			continue;
		
		D3DXVECTOR3 vLengA = pChar->GetPosition() - vDetectPos;
		fTarLeng = D3DXVec3Length(&vLengA);
		if ( fTarLeng <= nRange )
		{
			vecTargetID.push_back( STARGETID(pChar->GetCrow(),  pChar->GetGaeaID(), pChar->GetPosition() ) );
		}
	}

	for ( DWORD i=0; i<m_dwSUMMON_AMOUNT && i<m_vecSUMMON_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecSUMMON_VIEW[i];
		GLSummonField *pSummon = pLand->GetSummon(dwGAEAID);
		if (!pSummon)
            continue;
		if ( pSummon->IsNonTargetable() )
			continue;

		// ���̶�� �Ǵ��ϰ� ������������ �Ǻ��Ѵ�;
		if ( EMREACTIONABLE_FALSE == IsReactionable( pSummon, EMIMPACT_SIDE_ENEMY ) )
			continue;

		ServerActor* pSummonOwner = m_pGaeaServer->GetActor( pLand, pSummon->m_sOwnerID );
        //GLChar* pSummonOwner = m_pGaeaServer->GetChar64(pSummon->GetOwnerDbNum(), pSummon->GetOwnerSvrGroupNum());
        if (!pSummonOwner)
            continue;
		if (pSummonOwner->IsSafeTime())
            continue;
		if (pSummonOwner->IsSafeZone())
            continue;
		if (pSummonOwner->GETCONFTING()->IsCONFRONTING())
            continue;

		D3DXVECTOR3 vLengA = pSummon->GetPosition() - vDetectPos;
		fTarLeng = D3DXVec3Length(&vLengA);
		if ( fTarLeng <= nRange )
		{
			vecTargetID.push_back( STARGETID(pSummon->GetCrow(), pSummon->GetGaeaID(), pSummon->GetPosition() ) );
		}
	}

	if( vecTargetID.empty() ) return vecTargetID;

	std_afunc::CCompareTargetDist Comp(vDetectPos);
	std::sort ( vecTargetID.begin(), vecTargetID.end(), Comp );
	if( vecTargetID.size() > dwMaxFind )
		vecTargetID.erase( vecTargetID.begin()+dwMaxFind, vecTargetID.end() );

	return vecTargetID;
}

void GLCrow::RunNextAct()
{
	if (m_pCrowData->IsPosHold())
        return;

	if ( m_pCurAct )
	{
		const SMOBACTION &MobAction = m_pCurAct->Data;

		switch ( MobAction.emAction )
		{
		case EMACTION_MOVE:
			ToWalk();
			MoveTo ( &MobAction.vPos, false, 0 );
			break;

		case EMACTION_ACTSTAY:
			{
				SetFLAG(EMSTAY);
				m_fStayTimer = MobAction.fLife;
			}
			break;
		};

		if ( IsFLAG(EMPREORDER) )
		{
			if ( !m_pCurAct->pPrev )
                ReSetFLAG(EMPREORDER);

			if ( m_pCurAct->pPrev )
                m_pCurAct = m_pCurAct->pPrev;
			else
                m_pCurAct = m_pCurAct->pNext;
		}
		else
		{
			if ( !m_pCurAct->pNext )
                SetFLAG(EMPREORDER);

			if ( m_pCurAct->pNext )
                m_pCurAct = m_pCurAct->pNext;
			else
                m_pCurAct = m_pCurAct->pPrev;
		}
	}
	else
	{
		//	Note : ������ ��ġ���� idle ���¿� �ִ� ���� �ڱ� ��ġ�� �ٽ� ���ư���.
		//		�� �Ա��� ������ ���� ���Ƽ� �ٸ� �÷��̾�� ���ظ� �ִ� ���� ������.
		//
		D3DXVECTOR3 vDist(m_vPosition - m_vGenPos);
		float fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_CHAR::fMOB_TRACING )
		{
			if ( m_pCrowData->m_sAction.m_bRun )		ToRun();
			MoveTo ( &m_vGenPos, false, 0 );
		}
	}
}

BOOL GLCrow::CheckGroupRoyalty()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

    // ��ȹ�������� ���� �׷���� ���� ��� ���� [12/18/2013 hsshin];
    // ������ ���� ������ ���ݿ� �����Ͽ� Ÿ���� ������ �� �ִ�.;
    // ��ũ ��(����)�� ���� �׷�Ÿ���� �����ÿ��� Ÿ���� ���� �� �� ����;	
	SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap().getBaseMapID()));
	if ( pMapNode && pMapNode->IsDisableSwitchMob() == true )
		return FALSE;

	// �׷� ���Ͱ� �ƴѰ�� ����;
    if ( IsGroupMember() == FALSE )
        return FALSE;

    // �׷� ������ ���;
    if( IsGroupLeader() )
    {
		return FALSE;
    }
    
    // �׷����̸鼭 �׷��� Ÿ���� ���� ���� �׷�Ÿ�� �켱;
    if( pLand->GET_GROUPTARGET( GetGroupName() ).GaeaId != EMTARGET_NULL )
        return TRUE;

	// �׿� ��� ����;
    return FALSE;
}

BOOL GLCrow::CheckChangeGroupTarget()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	// �׷� ������ ��츸 �׷�Ÿ���� ������ �� �ִ�.;
	if( IsGroupLeader() )
	{
		ServerActor* const pActor = m_pGaeaServer->GetTarget ( pLand, pLand->GET_GROUPTARGET( GetGroupName() ) );

		// ���� �׷� Ÿ���� ���ų� �ش� Ÿ���� ������ ��쿡�� �׷�Ÿ���� ������ �� �ִ�.;
		if( pActor == NULL )
			return TRUE;

		// ������ �Ұ����� ��� �׷�Ÿ���� �����Ѵ�;
		if ( EMREACTIONABLE_FALSE == IsReactionable( pActor, EMIMPACT_SIDE_ENEMY ) )
			return TRUE;
	}
	return FALSE;
}

BOOL GLCrow::IsGroupTarget( const STARGETID& sTargetID )
{ 
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	return ( IsGroupMember() && pLand->GET_GROUPTARGET( GetGroupName() ) == sTargetID );
}

void GLCrow::GroupAttackProcess( const STARGETID& sTargetID  )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if( !IsGroupMember() )
		return;

	// �׷� ������ ��� �ش� Ÿ�ٿ� ���δ� ������ �Ѵ�
	if( IsGroupLeader() )
	{
        ServerActor* const pActor = m_pGaeaServer->GetTarget ( pLand, pLand->GET_GROUPTARGET( GetGroupName() ) );
        
        // ���� �׷� Ÿ���� ���ų� �ش� Ÿ���� ������ ���;
		SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap().getBaseMapID()));
		if( (pActor == NULL) || (pMapNode && (pMapNode->IsDisableSwitchMob() == true)) )
		{
			// �׷� Ÿ�ٿ� ����� �Ѵ�.
			pLand->SET_GROUPTARGET( GetGroupName(), sTargetID );

			const SETGROUPMEM& setGroupMember	= pLand->GET_GROUPMEMBER(GetGroupName());
			SETGROUPMEM_CITER setMember_citer	= setGroupMember.begin();
			for( ; setMember_citer != setGroupMember.end(); ++setMember_citer )
			{
				// ����� �ƴϸ� �Ѿ��.
				if( !pLand->IS_GROUPMEMBER(GetGroupName(), *setMember_citer ) )
					continue;

				// �ڽ��̸� �Ѿ��.
				if( m_dwGaeaID == *setMember_citer )
					continue;

				PGLCROW pGLCrow = pLand->GetCrow(*setMember_citer);
				if( !pGLCrow )
					continue;
				
				pGLCrow->NewTarget( sTargetID );
			}
		}
	}
	else
	{
		PGLCROW pGLLeader = pLand->GET_GROUPLEADER(GetGroupName());
		if( pGLLeader && pGLLeader->GetTargetID().GaeaId == EMTARGET_NULL )
		{
			pGLLeader->NewTarget( sTargetID );
		}
	}
}

void GLCrow::NewTarget( const STARGETID &sTargetID, BOOL bCheckMember /* = FALSE */ )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;
	
	ServerActor* pTarget = m_pGaeaServer->GetTarget ( pLand, sTargetID );
	if ( !pTarget )
		return;

	m_vOrgTarPos = m_vPosition;

	if( m_bIsPatrolStop == false )
		m_vPatrolStopPos = m_vPosition;
		
	m_bIsPatrolStop = true;
	

	//	Note : Ÿ������ ����.
	m_TargetID.emCrow = sTargetID.emCrow;
	m_TargetID.GaeaId = pTarget->GetGaeaID();
	m_TargetID.vPos = pTarget->GetPosition();

	SetFLAG(EMTARGET);

	if(  bCheckMember ) GroupAttackProcess(sTargetID);
}

BOOL GLCrow::SetTargeting ( const STARGETID& sTargetID )
{
	EMCROWACT_UP emCROWACT = GetActPattern();
	HRESULT      hrESCAPE  = ESCAPE ( STARGETID ( sTargetID.emCrow, sTargetID.GaeaId ), GetMaxHP() );

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	if ( EMCROWACT_UP_ESCAPE != emCROWACT || hrESCAPE!=S_OK )
	{
		// Ÿ���� ���� ���� �ʰų�;
        const bool bNonTarget = !IsFLAG(EMTARGET);

		// �׷�Ÿ���� ���� �Ǿ�� �ϰų�;
		const bool bGroupTargetChange = CheckChangeGroupTarget();

		// �׷�Ÿ�����Ӽ��� ���� ��Ȳ���� ����� �� ������ ���� ������;
        const bool bTargetChangeAble = (CheckGroupRoyalty() == false) && (m_pCrowData->IsTarShort() && (!IsFLAG(EMATTACKABLE)));

		// Ÿ���� �ٲ۴�.;
		if ( bNonTarget ||  bGroupTargetChange || bTargetChangeAble )
		{
			ServerActor* pTarget = m_pGaeaServer->GetTarget(pLand, sTargetID);

			if ( pTarget && !pTarget->IsNonTargetable() )
			{
				// Note : �����ڰ� ��ȯ�� �̰� ������ ĳ������ �� �ֺ��� ������ ã�´�
				//
				if ( sTargetID.emCrow == CROW_SUMMON &&
					((PGLSUMMONFIELD)pTarget)->m_sOwnerID.actorType == CROW_PC )
				{
					DWORD dwOwnerGUID = ((PGLSUMMONFIELD)pTarget)->m_dwOwnerGaeaID;

					VECPC_ITER Founditer = std::find(m_vecPC_VIEW.begin(), m_vecPC_VIEW.end(), dwOwnerGUID); 

					if ( Founditer != m_vecPC_VIEW.end() )
					{
						PGLCHAR pChar = m_pGaeaServer->GetChar ( dwOwnerGUID );

						if ( pChar && !pChar->IsSafeTime() && !pChar->IsSafeZone() && pChar->IsVisibleDetect(this) && pChar->IsValidBody() )
						{
							int         nRange   = (int) m_pCrowData->m_wViewRange;
							D3DXVECTOR3 vLengA   = pChar->GetPosition() - m_vPosition;
							float       fTarLeng = D3DXVec3Length(&vLengA);
							if (  fTarLeng <= nRange )
							{
								pTarget = pChar;
							}
						}
					}
				}

				STARGETID sTARID(pTarget->GetCrow(),pTarget->GetGaeaID(),pTarget->GetPosition());
				NewTarget ( sTARID, TRUE );
				return TRUE;
			}
		}
	}

	return FALSE;
}

namespace
{
	typedef std::pair<int,int> PRIOR_PAIR;
	class prior_sort
	{
	public:
		bool operator () ( const PRIOR_PAIR &lvalue, const PRIOR_PAIR &rvalue )
		{
			return lvalue.first > rvalue.first;
		}
	};

	// float g_fMINNEAR(15.0f);
	float g_fMINNEAR(21.0f);
	float g_fMAXNEAR(35.0f);
};

DWORD GLCrow::FindOptAType ( const STARGETID &sTargetID, bool branvar )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return SCROWDATA::EMMAXATTACK;

	ServerActor*	pTarget = m_pGaeaServer->GetTarget( pLand, sTargetID );
	if( !pTarget )
		return SCROWDATA::EMMAXATTACK;

	bool			bTarStateRun = (pTarget->IsRunning()==TRUE);
	D3DXVECTOR3		vDX			 = m_vPosition - pTarget->GetPosition();
	float			fLength		 = D3DXVec3Length( &vDX );

	//	Note : ������ �켱���� ����.
	bool			bprior_near	 = false;
	if ( g_fMAXNEAR>fLength && !bTarStateRun )
		bprior_near = true;

	std::vector<PRIOR_PAIR> vecPRIOR;
	vecPRIOR.clear ();

	int nIndex = IsLowerHP();
	if( nIndex >= 0 )
	{
		if( m_pCrowData->m_sAction.m_vecPatternList[ nIndex ].m_dwPatternAttackSet < m_pCrowData->m_vecCrowAttack.size() )
			return ( DWORD )m_pCrowData->m_sAction.m_vecPatternList[ nIndex ].m_dwPatternAttackSet;
	} //if
	
	DWORD	dwSize = (DWORD)m_pCrowData->m_vecCrowAttack.size();
	for( DWORD i = 0; i < dwSize; ++i )
	{
		const SCROWATTACK &sATTACK = m_pCrowData->m_vecCrowAttack[i];
		if( !sATTACK.bUsed )
			continue;

		if( m_fACTIONDELAY[i] > 0.0f )
			continue;
		
		bool bskill(false);
		bool bnear(false);
		if ( sATTACK.skill_id!=SNATIVEID(false) )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sATTACK.skill_id );
			if( !pSkill )
				continue;

			if( sATTACK.skill_tar == CROWSKTAR_OUR )
				continue;

			//	������ �ð�, ���Ǽ�ġ�� Ȯ���Ͽ� ��� �������� ����.
			EMSKILLCHECK emcheck = CHECHSKILL ( DWORD(i) );
			if( emcheck!=EMSKILL_OK && emcheck!=EMSKILL_NOTSP )
				continue;
			
			bskill = true;

			WORD wTARRANGE = GETSKILLRANGE_TAR( *pSkill );

			if ( wTARRANGE < g_fMAXNEAR )
				bnear = true;
			else
				bnear = false;
		}
		else
		{
			if ( sATTACK.emAttRgType == EMATT_SHORT )
				bnear = true;
			else
				bnear = false;
		} //if..else

		//	�켱���� ���.
		int		nprior(0);

		//	�켱������ ������ ��ġ�� �ణ �ο�. ( ���� ���� )
		if (branvar)
            nprior += int((sc::Random::RANDOM_POS()) * dwSize);
		
		//	��ų�� ��� ������.
		if (bskill)
            nprior += 11;
		
		//	���� ������ �����Ѱ� ��Ÿ��� �����Ѱ��� ���� �켱 ���� ����.
		if ( bprior_near )
		{
			//	���� �켱.
			if ( bnear )
				nprior += 10;
			else
				nprior += 5;
		}
		else
		{
			//	��Ÿ� �켱.
			if ( bnear )
				nprior += 5;
			else
				nprior += 10;
		}

		vecPRIOR.push_back ( std::make_pair(nprior,i) );
	}

	if ( vecPRIOR.empty() )
		return SCROWDATA::EMMAXATTACK;

	std::sort ( vecPRIOR.begin(), vecPRIOR.end(), prior_sort() );

	const PRIOR_PAIR &sPRIOR = *vecPRIOR.begin();

	GASSERT ( sPRIOR.second < (int)dwSize );
	if ( sPRIOR.second >= (int)dwSize )
		return SCROWDATA::EMMAXATTACK;

	return sPRIOR.second;
}



void GLCrow::ReAction ( const STARGETID &sTargetID, float fElapsedTime )
{
	DWORD dwFoundAType = FindOptAType ( sTargetID, false );
	if ( dwFoundAType >= m_pCrowData->m_vecCrowAttack.size() )
        return;

	m_dwAType = dwFoundAType;
	m_pAttackProp = &(m_pCrowData->m_vecCrowAttack[m_dwAType]);

	if ( m_pAttackProp->skill_id==SNATIVEID(false) )
	{
		NewTarget ( sTargetID );
		m_emReAction = EMREACT_ATTACK;		
	}
	else
	{
		NewTarget ( sTargetID );
		SETACTIVESKILL ( m_pAttackProp->skill_id, m_pAttackProp->skill_lev );
		m_emReAction = EMREACT_SKILL;
	}
}

void GLCrow::ResetTarget()
{
	// Ÿ������ ��ȿȭ�Ѵ�.
	ReSetFLAG(EMTARGET);
	m_TargetID = STARGETID(CROW_PC,GAEAID_NULL);
}

void GLCrow::ResetAction ()
{
	m_emReAction = EMREACT_NONE;

	ResetTarget();
	ReSetFLAG(EMTRACING);
}

bool GLCrow::FindLongTracePos ( const D3DXVECTOR3 &vCur, const D3DXVECTOR3 &vTar, const float fMin, const float fMax, D3DXVECTOR3 &vTracePos )
{
	GASSERT(fMin<=fMax);

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	D3DXVECTOR3 vDir = vCur - vTar;	// ( tar to cur direction )
	D3DXVECTOR3 vNDir;
	D3DXVec3Normalize(&vNDir, &vDir);
	float fLength = D3DXVec3Length(&vDir);

	float fTestTrace(fMax);
	if ( fLength < fMin )
	{
		fTestTrace = fMin * 1.2f;
		if ( fTestTrace >= fMax )	fTestTrace = fMin;
	}
	else
	{
		fTestTrace = fMax * 0.85f;
		if ( fTestTrace <= fMin )	fTestTrace = fMax;
	}

	D3DXMATRIX matRotY;
	const int nMaxCircular = 9;
	const float fCircular[nMaxCircular] = { 0, 5, -5, 10, -10, 15, -15, 20, -20 };
	const D3DXVECTOR3 vMarkPos = vNDir * fTestTrace;

	boost::shared_ptr<NavigationMesh> spNaviMesh = pLand->GetNavi ();

	D3DXVECTOR3 vTarCol(vTar.x,vTar.y+15.0f,vTar.z);
	for ( int i=0; i<nMaxCircular; ++i )
	{
		float fRadian = fCircular[i] / fTestTrace;
		D3DXMatrixRotationY ( &matRotY, fRadian );
		
		D3DXVECTOR3 vTestPos;
		D3DXVec3TransformCoord ( &vTestPos, &vMarkPos, &matRotY );
		vTestPos = vTar + vTestPos;

		//	��ȿ�� ��ġ���� ����.
		BOOL bNaviCo(FALSE);
		DWORD dwCoID(0);
		D3DXVECTOR3 vNaviCo(0,0,0);
		D3DXVECTOR3 vPos1(vTestPos.x,vTestPos.y+5.0f,vTestPos.z);
		D3DXVECTOR3 vPos2(vTestPos.x,vTestPos.y-5.0f,vTestPos.z);

		if ( spNaviMesh )
		{
			spNaviMesh->IsCollision ( vPos1, vPos2, vNaviCo, dwCoID, bNaviCo );
			if ( !bNaviCo )
				continue;
		}

		D3DXVECTOR3 vSuitable(vTestPos.x,vTestPos.y+15.0f,vTestPos.z);
		BOOL bCollision = pLand->IsCollision( vSuitable, vTarCol, EMCC_CULL_NONE );
		if ( bCollision )
			continue;

		vTracePos = vTestPos;
		return true;
	}

	vTracePos = vTar;

	return true;
}

bool GLCrow::DoShortTracing ( const WORD wTARRANGE, const float fElapsedTime )
{
	//	Note : Ÿ���� ��ġ ������ ������.
	//		(����) �ݵ�� Ÿ���� �����Ѵٴ� �����Ͽ� ����.
	//
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;
    
	ServerActor *pTarget = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
	if ( !pTarget )		
		return false;

	GLARoundSlot* pARoundSlot = pTarget->GETSLOT ();
	
	if( pARoundSlot == NULL )
		return false;

	//GASSERT ( pARoundSlot && "glchar ���� ���� ��ġ ������ ������ �ֽ��ϴ�." );

	WORD wTarBodyRadius(0);
	D3DXVECTOR3 vTarPos(0,0,0);
	D3DXVECTOR3 vTarDist(0,0,0);
	float fDist(0.0f);

	vTarPos = pTarget->GetPosition();
	wTarBodyRadius = pTarget->GetBodyRadius();

	vTarDist = m_vPosition-vTarPos;
	fDist = D3DXVec3Length ( &vTarDist );

	
	//	Note : �Ÿ��� ���� Ÿ�� ���� �˻�.
	//
	if ( fDist < GLCONST_CHAR::fMOB_TRACING )
	{
		SetFLAG(EMTARGET);
	}
	else
	{
		BOOL bGroupTarget = IsGroupTarget( m_TargetID );
		if( pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) || bGroupTarget )
		{
			SetFLAG(EMTARGET);
		}
		else
		{
			pARoundSlot->ResetTargetOptSlot ( m_dwGaeaID, m_dwAttPosSlot );
			m_dwAttPosSlot = GLARoundSlot::SLOT_SIZE;

            ResetTarget();
		}		
	}

	//	Note : Ÿ���� ��ġ�� ���.
	if ( !IsFLAG(EMTARGET) )
	{
		ReSetFLAG(EMTRACING);
		return false;
	}

	//	Note : ��� ��� ���� �Ÿ��� �ִ��� �˻�.
	//
	m_fStayTimer -= fElapsedTime;

	WORD wActionAble = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE + 3;
	WORD wActionAbleMin = static_cast<WORD> ( (wTarBodyRadius+GETBODYRADIUS())*0.4f );
	if (fDist<wActionAble && fDist>wActionAbleMin )
	{
		SetFLAG(EMATTACKABLE);
	  	m_fStayTimer = 0.0f;
	}
	else
	{
		ReSetFLAG(EMATTACKABLE);
	}

	//	Note : ��ġ ������ ���� ���.
	if ( m_pCrowData->IsPosHold() )
	{
		if ( fDist<wActionAble )
		{
			m_fStayTimer = 0.0f;
			SetFLAG(EMATTACKABLE);
			return true;
		}

		return false;
	}

	//	Note : '������' ���¸� �������� �˻�.
	//
	if ( IsFLAG(EMTRACING) )
	{
		WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;
		WORD wTracingRange = WORD ( wActionRange * 1.2f );

		// [shhan][2015.08.20] �̰� ������ �ȴ�. 
		//						�ǵ�� ���� ��� ��Ŀ� ������ ���� �� �־� ���Ҹ� �ǵ��� ���ϰڴ�.
		//						������ m_TargetID.vPos - m_vPosition; �̰� �Ǿ߰���.
		D3DXVECTOR3 vEndDist = m_TargetID.vPos-vTarPos;
		float fEndDist = D3DXVec3Length ( &vEndDist );

		//	�̵��� �����ų� ���� ��ġ�� Ÿ����ġ�� �ʹ� �������� ��� "������ ����"�� ������.
		//
		if ( !isAction(GLAT_MOVE) || fEndDist>wTracingRange )
		{
			ReSetFLAG(EMTRACING);	//	'������' ���� ����.
		}
		else
		{
			//	Ÿ���� ���� ������ Ÿ�� �ֺ������� ���� ���� ���Ժ��� ���� ������ ������ �ִ��� ����.
			//
			if ( !pTarget->isAction(GLAT_MOVE) )
			{
				D3DXVECTOR3 vNewTracePos;
				DWORD dwAttPosSlot = pARoundSlot->TestOptimumSlot ( this, wActionRange, pLand->GetNavi(), vNewTracePos );
				if ( GLARoundSlot::SLOT_SIZE!=dwAttPosSlot )
				{
					BOOL bValidSlot = pARoundSlot->ValidOptimumSlot ( pLand->GetNavi(), vNewTracePos );
					if ( bValidSlot )
					{
						D3DXVECTOR3 vOldTraceDist = m_TargetID.vPos - m_vPosition;
						float fOldTraceDist = D3DXVec3LengthSq ( &vOldTraceDist );

						// [shhan][2015.08.20] �ּ��� �Ǿ� �ִ� �� ó�� ������ �س����� �Ÿ��� �� ���� ��� ������ ���� �׺���̼� �۾��� ���� �Ѵ�.
						//
						//D3DXVECTOR3 vNewTraceDist = m_TargetID.vPos - vNewTracePos;
						D3DXVECTOR3 vNewTraceDist = vNewTracePos - m_vPosition;
						float fNewTraceDist = D3DXVec3LengthSq ( &vNewTraceDist );

						//	�ٸ� ���� ������ ������.
						//
						if ( fNewTraceDist < fOldTraceDist )
						{
							m_dwAttPosSlot = dwAttPosSlot;
							pARoundSlot->SetTargetOptSlot ( m_dwGaeaID, dwAttPosSlot );

							//	Note : '�߰�' ����.
							if ( m_pCrowData->m_sAction.m_bRun )
                                ToRun();
							MoveTo ( &vNewTracePos, false, 0 );

							//	���� '�߰�' �ð� ���� ����.
							m_fStayTimer = sc::Random::RANDOM_POS() * 0.3f;
						}
					}
				}
			}
		}
	}

	//	Note : '����'���°� �ƴ� ��� �߰� ���θ� ������.
	//
	if ( !IsFLAG(EMTRACING) )
	{
		//	Note : '�߰�' ���� ����.
		BOOL bToTrace = FALSE;
		if ( !IsFLAG(EMATTACKABLE) && m_fStayTimer<=0.0f )
		{
			bToTrace = TRUE;
		}

		if( fDist < GetBodyRadius() && m_pCrowData->m_bOverlapAttack )
		{
			bToTrace = FALSE;
			SetFLAG(EMATTACKABLE);
			m_fStayTimer = 0.0f;
		}
		
		//	Note : '�߰�' �ϱ� ���ؼ� �̵�.
		//
		if ( bToTrace )
		{
			WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;

			BOOL bGroupTarget = IsGroupTarget( m_TargetID );
			if ( pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) || bGroupTarget )
			{
				//WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;

				float fMin = (float) ( wTarBodyRadius + GETBODYRADIUS() );
				float fMax = (float) ( wTarBodyRadius + GETBODYRADIUS() + wTARRANGE );

				D3DXVECTOR3 vTracePos;
				bool btacepos = FindLongTracePos ( m_vPosition, vTarPos, fMin, fMax, vTracePos );
				if ( btacepos )
				{
					//	Note : '�߰�' ����.
					SetFLAG(EMTRACING);

					if ( m_pCrowData->m_sAction.m_bRun )
                        ToRun();
					MoveTo ( &vTracePos, false, 0 );

					//	���� '�߰�' �ð� ���� ����.
					m_fStayTimer = sc::Random::RANDOM_POS() * 0.3f;
				}
                else
                {
                    ResetTarget();
					ReSetFLAG(EMTRACING);
				}				
			}
            else
            {
				D3DXVECTOR3 vTracePos;
				pARoundSlot->ResetTargetOptSlot ( m_dwGaeaID, m_dwAttPosSlot );

				m_dwAttPosSlot = pARoundSlot->ReqOptimumSlot ( this, wActionRange, pLand->GetNavi(), vTracePos );
				
				if ( GLARoundSlot::SLOT_SIZE!=m_dwAttPosSlot )
				{
					//	Note : '�߰�' ����.
					SetFLAG(EMTRACING);
					
					if ( m_pCrowData->m_sAction.m_bRun )
                        ToRun();
					MoveTo ( &vTracePos, false, 0 );

					//	���� '�߰�' �ð� ���� ����.
					m_fStayTimer = sc::Random::RANDOM_POS() * 0.3f;
				}
				else
				{
                    ResetTarget();
					ReSetFLAG(EMTRACING);
				}
			}
		}
	}

	return true;
}

bool GLCrow::DoLongTracing ( const WORD wTARRANGE, const float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	//	Note : Ÿ���� ��ġ ������ ������.
	//		(����) �ݵ�� Ÿ���� �����Ѵٴ� �����Ͽ� ����.
	//
	ServerActor *pTarget = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
	if ( !pTarget )		
		return false;

	WORD wTarBodyRadius(0);
	D3DXVECTOR3 vTarPos(0,0,0);
	D3DXVECTOR3 vTarDist(0,0,0);
	float fDist(0.0f);

	vTarPos = pTarget->GetPosition();
	wTarBodyRadius = pTarget->GetBodyRadius();

	vTarDist = m_vPosition-vTarPos;
	fDist = D3DXVec3Length ( &vTarDist );

	//	Note : �Ÿ��� ���� Ÿ�� ���� �˻�.
	//

	
	if ( fDist < GLCONST_CHAR::fMOB_TRACING )
	{
		SetFLAG(EMTARGET);
	}
	else
	{
		BOOL bGroupTarget = IsGroupTarget( m_TargetID );
		if( pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) || bGroupTarget )
		{
			SetFLAG(EMTARGET);
		}else{
			ResetTarget();
		}
	}

	//	Note : Ÿ���� ��ġ�� ���.
	if ( !IsFLAG(EMTARGET) )
	{
		ReSetFLAG(EMTRACING);
		return false;
	}

	//	Note : ��� ��� ���� �Ÿ��� �ִ��� �˻�.
	//
	m_fStayTimer -= fElapsedTime;

	WORD wActionAble = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE + 3;
	WORD wActionAbleMin = static_cast<WORD> ( (wTarBodyRadius+GETBODYRADIUS()) ); //*0.4f
	if ( fDist<wActionAble && fDist>wActionAbleMin )
	{
		SetFLAG(EMATTACKABLE);
	  	m_fStayTimer = 0.0f;
	}
	else
	{
		ReSetFLAG(EMATTACKABLE);
	}

	//	Note : ��ġ ������ ���� ���.
	if ( m_pCrowData->IsPosHold() )
	{
		if ( fDist<wActionAble )
		{
			m_fStayTimer = 0.0f;
			SetFLAG(EMATTACKABLE);
			return true;
		}

		return false;
	}

	//	Note : '������' ���¸� �������� �˻�.
	//
	if ( IsFLAG(EMTRACING) )
	{
		WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;
		WORD wTracingRange = WORD ( wActionRange * 1.2f );

		D3DXVECTOR3 vEndDist = m_TargetID.vPos-vTarPos;
		float fEndDist = D3DXVec3Length ( &vEndDist );

		//	�̵��� �����ų� ���� ��ġ�� Ÿ����ġ�� �ʹ� �������� ��� "������ ����"�� ������.
		//
		if ( !isAction(GLAT_MOVE) || fEndDist>wTracingRange || fEndDist<wActionAbleMin )
		{
			ReSetFLAG(EMTRACING);	//	'������' ���� ����.
		}
	}

	//	Note : '����'���°� �ƴ� ��� �߰� ���θ� ������.
	//
	if ( !IsFLAG(EMTRACING) )
	{
		//	Note : '����' ���� ����.
		BOOL bToTrace = FALSE;
		if ( !IsFLAG(EMATTACKABLE) && m_fStayTimer<=0.0f )
		{
			bToTrace = TRUE;
		}

		if( fDist < GetBodyRadius() && m_pCrowData->m_bOverlapAttack )
		{
			bToTrace = FALSE;
			SetFLAG(EMATTACKABLE);
			m_fStayTimer = 0.0f;
		}
		
		//	Note : '�߰�' �ϱ� ���ؼ� �̵�.
		//
		if ( bToTrace )
		{
			WORD wActionRange = wTarBodyRadius + GETBODYRADIUS() + wTARRANGE;

			float fMin = (float) ( wTarBodyRadius + GETBODYRADIUS() );
			float fMax = (float) ( wTarBodyRadius + GETBODYRADIUS() + wTARRANGE );

			D3DXVECTOR3 vTracePos;
			bool btacepos = FindLongTracePos ( m_vPosition, vTarPos, fMin, fMax, vTracePos );
			if ( !btacepos )
			{
				ResetTarget();
				ReSetFLAG(EMTRACING);
				return false;
			}

			//	Note : '�߰�' ����.
			SetFLAG(EMTRACING);
			if ( m_pCrowData->m_sAction.m_bRun )
				ToRun();

			MoveTo ( &vTracePos, false, 0 );
			//	���� '�߰�' �ð� ���� ����.
			m_fStayTimer = sc::Random::RANDOM_POS() * 0.3f;
		}
	}

	return true;
}

//static int nSkillReActionError = 0;

bool GLCrow::SkillReAction ( float fTime, float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	if ( !VALIDSKILL() )
		return false;

	DWORD dwFoundAType = FindOptAType ( m_TargetID, false );
	if ( dwFoundAType != m_dwAType )
	{
		ReAction ( m_TargetID );
		return false;
	}

	//	��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )
		return false;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[m_wACTIVESKILL_LVL];
	EMIMPACT_TARGET emIMPACT_TAR = pSkill->m_sBASIC.emIMPACT_TAR;
	
	WORD wTARRANGE = GETSKILLRANGE_TAR( *pSkill );

	//	Note : tracing
	//
	bool bShortTrace(false);
	if ( wTARRANGE < g_fMINNEAR )	bShortTrace = true;

	bool btrace(false);
	if ( bShortTrace )
	{
		btrace = DoShortTracing ( wTARRANGE, fElapsedTime );
	}
	else
	{
		btrace = DoLongTracing ( wTARRANGE, fElapsedTime );
	}

	if ( !btrace )
	{
		ResetAction ();
		return false;
	}

	//	Note : ��ų ��� ���� ����.
	//
	BOOL bactionable(FALSE);
	if ( bShortTrace )		bactionable = IsFLAG(EMATTACKABLE) && !IsFLAG(EMTRACING);
	else					bactionable = IsFLAG(EMATTACKABLE);

	if ( bactionable )
	{
		//	Note : �þ߰� Ȯ���Ǵ°�?
		//
		ServerActor *pTarget = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
		if ( !pTarget )
		{
			ResetAction ();
			return false;
		}
		
		D3DXVECTOR3 vFinderPos(m_vPosition.x,m_vPosition.y+15.0f,m_vPosition.z);
		D3DXVECTOR3 vTargetPos(pTarget->GetPosition());
		vTargetPos.y += 15.0f;
		
		BOOL bCollision = pLand->IsCollision( vFinderPos, vTargetPos, EMCC_CULL_NONE );
		if ( bCollision )
		{
			ResetAction ();
			return false;
		}

		PreSkillProc ();

		m_emReAction = EMREACT_NONE;
	}

	return true;
}

bool GLCrow::AttackReAction(float fTime, float fElapsedTime)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	if (!VALIDATTACK())
        return false;

	DWORD dwFoundAType = FindOptAType(m_TargetID, false);
	if (dwFoundAType != m_dwAType)
	{
		ReAction(m_TargetID);
		return false;
	}

	WORD wTARRANGE = GETATTACKRANGE();

	// tracing
	bool bShortTrace(false);
	if (m_pAttackProp && m_pAttackProp->emAttRgType == EMATT_SHORT)
        bShortTrace = true;

	bool btrace(false);
	if (bShortTrace)
        btrace = DoShortTracing(wTARRANGE, fElapsedTime);
	else
        btrace = DoLongTracing(wTARRANGE, fElapsedTime);

	if (!btrace)
	{
		ResetAction();
		return false;
	}

	// ���� ���� ����
	BOOL bactionable(FALSE);
	if (bShortTrace)
        bactionable = IsFLAG(EMATTACKABLE) && !IsFLAG(EMTRACING);
	else
        bactionable = IsFLAG(EMATTACKABLE);

	if (bactionable)
	{
		// �þ߰� Ȯ���Ǵ°�?
		ServerActor* pTarget = m_pGaeaServer->GetTarget(pLand, m_TargetID);
		if (!pTarget)
		{
			ResetAction();
			return false;
		}
		
		D3DXVECTOR3 vFinderPos(m_vPosition.x,m_vPosition.y+15.0f,m_vPosition.z);
		D3DXVECTOR3 vTargetPos(pTarget->GetPosition());
		vTargetPos.y += 15.0f;
		
		BOOL bCollision = pLand->IsCollision( vFinderPos, vTargetPos, EMCC_CULL_NONE );
		if (bCollision)
		{
			ResetAction();
			return false;
		}

		TurnAction(GLAT_ATTACK);

		m_emReAction = EMREACT_NONE;
	}

	return true;
}

void GLCrow::RunSchedule(float fTime, float fElapsedTime)
{
	GASSERT(m_pCrowData);

	AISystem::CAICompField* pAI =
		static_cast< AISystem::CAICompField* >(
		GetComponent( ComponentSystem::EMFAMILY_AI ) );
	if ( pAI )
	{
		pAI->FrameMove( fElapsedTime );
	}

	if ( isAction(GLAT_IDLE) )
	{
		if ( IsFLAG(EMSTAY) )
		{
			m_fStayTimer -= fElapsedTime;
			if ( m_fStayTimer<0.0f )
                ReSetFLAG(EMSTAY);
		}
		else if ( !IsFLAG(EMTARGET) )
            RunNextAct ();		// ���ο� �׼�.
	}

	if ( IsFLAG(EMTARGET) )
	{
		if ( !isAction(GLAT_ATTACK) && !isAction(GLAT_SKILL) )
		{
			switch ( m_emReAction )
			{
			case EMREACT_NONE:
				ReAction ( m_TargetID, fElapsedTime );
				break;

			case EMREACT_ATTACK:
				AttackReAction ( fTime, fElapsedTime );
				break;

			case EMREACT_SKILL:
				SkillReAction ( fTime, fElapsedTime );
				break;
			};
		}
	}
	else if ( m_pCrowData->m_emCrow == CROW_MOB && VALIDACTION() )
	{
		EMCROWACT_UP emACT_UP = GetActPattern();

		if ( (emACT_UP!=EMCROWACT_UP_IDLE) && (emACT_UP!=EMCROWACT_UP_ESCAPE) )
		{
			m_fTargetTimer -= fElapsedTime;
			if ( m_fTargetTimer<0.0f )
			{
				m_fTargetTimer = 0.1f;	// ���� �˻� ����.
				
				//	Note : �˻� ����.
				STARGETID targetID = FindEnemyChar ();
				if ( targetID.GaeaId != EMTARGET_NULL )
				{
//					STARGETID sTARID(CROW_PC,pTarget->m_dwGaeaID,pTarget->GetPosition());
					ReAction(targetID);
				}
			}
		}
	}
}

BOOL GLCrow::IsViewInActor( const STARGETID& sID )
{
	GLLandMan* const pLand(GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	if( sID.ISNULL() ) return FALSE;

	int	nRange = (int) m_pCrowData->m_wViewRange;

	if ( (GETVISIBLESIGHT()+1) < nRange )
	{
		nRange = GETVISIBLESIGHT();
	}
	
	for( DWORD i = 0; sID.emCrow == CROW_PC && i < m_dwPC_AMOUNT && i < m_vecPC_VIEW.size(); ++i )
	{
		DWORD		dwGAEAID = m_vecPC_VIEW[i];
		GLChar*		pChar	 = m_pGaeaServer->GetChar( dwGAEAID );
		if ( !pChar )
			continue;

		if( pChar->IsSafeTime()	|| pChar->IsSafeZone()	|| !pChar->IsValidBody() || !pChar->IsVisibleDetect( this ) )	{
			 continue;
		}

		if( sID.GaeaId == dwGAEAID ) return TRUE;
	} 
    
	for( DWORD i = 0; sID.emCrow == CROW_MOB &&  i < m_vecMob_VIEW.size(); i++ )
	{
		DWORD		dwGAEAID = m_vecMob_VIEW[i];
		GLCrow*		pMob = pLand->GetCrow( m_vecMob_VIEW[i] );
		if( !pMob )
			continue;

		if( !pMob->IsValidBody() )
            continue;

        if( sID.GaeaId == dwGAEAID ) return TRUE;
	}


	// Note : ĳ���͸� �߰� ���Ѱ�� ��ȯ���� ã�´�.
	for( DWORD i = 0; i < sID.emCrow == CROW_SUMMON && m_dwSUMMON_AMOUNT && i < m_vecSUMMON_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecSUMMON_VIEW[i];
		GLSummonField* pSummon = pLand->GetSummon(dwGAEAID);

		if (!pSummon)
            continue;

		if (!pSummon->IsValidBody())
            continue;

		if ( pSummon->IsNonTargetable() )
			continue;

		if ( !pSummon->IsVisibleDetect( this ) )
			continue;

		if( sID.GaeaId == dwGAEAID ) return TRUE;

	}

	return FALSE;
}


const EMREACTIONABLE GLCrow::IsReactionable ( ServerActor* _pTarget, const EMIMPACT_SIDE _emTargetSide )
{
	GLLandMan* const pLand = GetLandMan();
	if ( NULL == pLand )
		return EMREACTIONABLE_FALSE;

	if ( NULL == _pTarget )
		return EMREACTIONABLE_FALSE;

	if ( _pTarget->IsNonTargetable() )
		return EMREACTIONABLE_FALSE;

	if ( FALSE == _pTarget->IsVisibleDetect( this ) )
		return EMREACTIONABLE_FALSE;

	const Faction::ManagerField* const pFactionMan = pLand->GetFactionManager();
	if ( pFactionMan )
		return IsReactionableFaction( _pTarget,_emTargetSide );
	else
		return IsReactionableNonFaction( _pTarget, _emTargetSide );
}


const EMREACTIONABLE GLCrow::IsReactionableFaction ( ServerActor* _pTarget, const EMIMPACT_SIDE _emTargetSide )
{
	GLLandMan* const pLand = GetLandMan();
	if ( NULL == pLand )
		return EMREACTIONABLE_FALSE;

	if ( NULL == _pTarget )
		return EMREACTIONABLE_FALSE;

	// Ÿ���� ��ȯ����� ��� Ÿ�ٰ˻縦 ��ȯ���� �������� �ٲ۴�;
	if ( CROW_SUMMON == _pTarget->GetCrow() )
	{
		PGLSUMMONFIELD pSummon = pLand->GetSummon( _pTarget->GetGaeaID() );
		if ( NULL == pSummon )
			return EMREACTIONABLE_FALSE;

		_pTarget = GetTarget( pLand,
			STARGETID(
			pSummon->m_sOwnerID.actorType,
			pSummon->m_dwOwnerGaeaID ) );
		if ( NULL == _pTarget )
			return EMREACTIONABLE_FALSE;
	}

    Faction::ManagerField* const pFactionMan = pLand->GetFactionManager();
    if ( NULL == pFactionMan )
        return EMREACTIONABLE_FALSE;

    const Faction::EMRELATION_TYPE emRelation = pFactionMan->getRelationField( this, _pTarget );
    if ( emRelation == Faction::EMRELATION_ERROR )
        return IsReactionableNonFaction( _pTarget, _emTargetSide );

    switch ( _emTargetSide )
    {
    case EMIMPACT_SIDE_ENEMY:
		{
			if ( emRelation == Faction::EMRELATION_ENEMY )
			{
				return EMREACTIONABLE_TRUE;
			}
		}
		break;

    case EMIMPACT_SIDE_PARTY:
	case EMIMPACT_SIDE_OUR:
        {
			if ( emRelation == Faction::EMRELATION_ALLY )
				return EMREACTIONABLE_TRUE;
        }
        break;

    case EMIMPACT_SIDE_ANYBODY:
        {
			return EMREACTIONABLE_TRUE;
        }
        break;

    default:
        break;
    }

    return EMREACTIONABLE_FALSE;
}


const EMREACTIONABLE GLCrow::IsReactionableNonFaction ( ServerActor* _pTarget, const EMIMPACT_SIDE _emTargetSide )
{
	GLLandMan* const pLand = GetLandMan();
	if ( NULL == pLand )
		return EMREACTIONABLE_FALSE;

	if ( NULL == _pTarget )
		return EMREACTIONABLE_FALSE;

	// ������ ������� �ʴ°��� ������ TRUE �̴�;
	return EMREACTIONABLE_TRUE;
}