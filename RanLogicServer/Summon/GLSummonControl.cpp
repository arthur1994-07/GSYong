#include "pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/stl_Func.h"

#include "../AI/CAICompField.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Util/StlFuncServer.h"

#include "../AI/GLFSMContainer.h"

#include "./GLSummonField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool GLSummonField::IsTargetable ( ServerActor* pTarget
								 , const EMIMPACT_SIDE& emTargetSide
								 , float& fTargetLength
								 , bool bForceAttack )
{
	GLLandMan* const pLand =
		m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::IsTargetable - pLand is NULL." );
		return false;
	}

	EMATT_RGTYPE emAttRgType = GetAttackRangeType();
	int nRange = static_cast< int >( m_pSummonCrowData->m_wViewRange );

	// Ÿ������ ���� ������ ����ΰ�?;
	const EMREACTIONABLE emReactionable =
		IsReActionable( pTarget, emTargetSide );
	switch ( emReactionable )
	{
	case EMREACTIONABLE_FALSE:
		return false;
		break;
	case EMREACTIONABLE_TRUE:
		break;
	case EMREACTIONABLE_FORCED_TRUE:
		if ( !bForceAttack )
			return false;
		break;
	}

	// Ÿ���� �� �� �ִ� �����ΰ�?;
	if ( pTarget->IsSafeTime() ||
		pTarget->IsSafeZone() ||
		!pTarget->IsValidBody() ||
		pTarget->IsNonTargetable()
		/*!pTarget->IsVisibleDetect( this )*/ )
	{
		return false;
	}

	bool bTarget = false;

	// ���� ������ ���� �з�;
	if ( emAttRgType == EMATT_SHORT )
	{
		/*if( pTarget->IsFreeARSlot() ||
			pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) )*/
		{
			D3DXVECTOR3 vLengA = pTarget->GetPosition() - m_vPosition;
			fTargetLength = D3DXVec3Length( &vLengA );
			if( fTargetLength <= nRange )
				bTarget = true;
		}
	}
	else
	{
		D3DXVECTOR3 vLengA = pTarget->GetPosition() - m_vPosition;
		fTargetLength = D3DXVec3Length( &vLengA );
		if( fTargetLength <= nRange )
			bTarget = true;
	}

	if ( bTarget )
	{
		if ( pLand->IsLayerSimpleCollision( pTarget->GetPosition(), m_vPosition, 5.f ) )
		{
			return false;
		}
	}

	return bTarget;
};

STARGETID GLSummonField::SearchTargetAction ( EMIMPACT_SIDE emTargetSide )
{
	if ( !m_pGaeaServer )
	{
		sc::writeLogError( "GLSummonField::SearchTargetAction - pServer is NULL." );
		return STARGETID();
	}

	GLLandMan* const pLand =
		m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::SearchTargetAction - pLand is NULL." );
		return STARGETID();
	}

	// ĳ���� �˻�;
	LIST_CHAR listChars;
	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<m_vecPC_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecPC_VIEW[ i ];
		GLChar* pChar = m_pGaeaServer->GetChar( dwGAEAID );
		if ( !pChar )
			continue;

		float fTarLeng	= FLT_MAX;
		if ( !IsTargetable( pChar, emTargetSide, fTarLeng, false ) )
			continue;

		listChars.push_back( std_afunc::CHARDIST( fTarLeng, pChar ) );
	}

	if( !listChars.empty() )
	{
		listChars.sort( std_afunc::CCompareCharDist() );
		const std_afunc::CHARDIST &target = *listChars.begin();

		if( target.pChar )
		{
			return STARGETID(
				CROW_PC,
				target.pChar->GetGaeaID(),
				target.pChar->GetPosition() );
		}
	}


	// ��ȯ�� �˻�;
	LIST_SUMMON listSummons;
	for( DWORD i = 0; i < m_vecSUMMON_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecSUMMON_VIEW[ i ];
		GLSummonField* pSummon = pLand->GetSummon( dwGAEAID );
		if( !pSummon )
			continue;

		float fTarLeng	= FLT_MAX;
		if ( !IsTargetable( pSummon, emTargetSide, fTarLeng, false ) )
			continue;

		listSummons.push_back( std_afunc::SUMMONDIST( fTarLeng, pSummon ) );
	}

	if ( !listSummons.empty() )
	{
		listSummons.sort( std_afunc::CCompareSummonDist() );
		const std_afunc::SUMMONDIST &target = *listSummons.begin();
		if( target.pSummon )
		{
			return STARGETID(
				CROW_SUMMON,
				target.pSummon->GetGaeaID(),
				target.pSummon->GetPosition() );
		}
	}

	// �� �˻�;
	LIST_CROW listCrows;
	for( DWORD i = 0; i < m_vecCROW_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecCROW_VIEW[ i ];
		GLCrow* pMob = pLand->GetCrow( dwGAEAID );
		if( !pMob )
			continue;

		float fTarLeng	= FLT_MAX;
		if ( !IsTargetable( pMob, emTargetSide, fTarLeng, false ) )
			continue;

		//sc::writeLogInfo ( sc::string::format( "Auto Search Target 'Mob %1%'.", pMob->GetGaeaID() ) );
		listCrows.push_back( std_afunc::CROWDIST( fTarLeng, pMob ) );
	}

	if ( !listCrows.empty() )
	{
		listCrows.sort( std_afunc::CCompareCrowDist() );
		const std_afunc::CROWDIST &target = *listCrows.begin();
		if( target.pCrow )
		{
			return STARGETID(
				CROW_MOB,
				target.pCrow->GetGaeaID(),
				target.pCrow->GetPosition() );
		}
	}

	return STARGETID();
}

const bool GLSummonField::IsNonTargetable() const
{
	return ( m_pSummonCrowData && m_pSummonCrowData->m_bNonTargeting );
}

const DWORD GLSummonField::GetAIType () const
{
	const AISystem::CAICompField* pAI =
		static_cast< const AISystem::CAICompField* >(
		GetComponent( ComponentSystem::EMFAMILY_AI ) );

	if ( pAI )
	{
		return pAI->GetType();
	}

	return 0;
}

void GLSummonField::SetAIType ( DWORD dwType )
{
	AISystem::CAICompField* pAI =
		static_cast< AISystem::CAICompField* >(
		GetComponent( ComponentSystem::EMFAMILY_AI ) );

	if ( pAI )
	{
		pAI->SetAIType( dwType );
		pAI->Initialize();
	}
}

void GLSummonField::ApplyDelay ( float fDelayTime )
{
	m_bDelay = true;
	m_fDelayTime = fDelayTime;
}

bool GLSummonField::GetDelayState ()
{
	return m_bDelay;
}

ServerActor* GLSummonField::GetOwner ()
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::GetOwner - pLand is NULL." );
		return NULL;
	}

	ServerActor* pOwner = m_pGaeaServer->GetActor(
		pLand, m_sOwnerID );
	if ( !pOwner )
	{
		sc::writeLogError ( "GLSummonField::GetOwner - pOwner is NULL." );
		return NULL;
	}

	return pOwner;
}

bool GLSummonField::IsTarget ()
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::IsTarget - pLand is NULL." );
		return false;
	}

	// Ÿ���� ��ȿ���� ���� ���;
	if( m_TargetID.GaeaId == EMTARGET_NULL )
		return false;

	ServerActor* pTarget = m_pGaeaServer->GetTarget(
		pLand, m_TargetID );
	if ( !pTarget || !pTarget->IsValidBody() )
		return false;
/*
	// ������ Ÿ���� �ƴϴ�;
	ServerActor* pOwner = m_pGaeaServer->GetActor(
		pLand, Faction::GLActorID( CROW_PC, m_OwnerDbNum ) );
	if ( !pOwner )
	{
		sc::writeLogError ( "GLSummonField::IsTarget - pOwner is NULL." );
		return false;
	}

	if ( m_TargetID.emCrow == pOwner->GetCrow() &&
		m_TargetID.GaeaId == pOwner->GetGaeaID() )
		return false;*/

	return true;
}

bool GLSummonField::IsTargetExcludeOwner ()
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::IsTarget - pLand is NULL." );
		return false;
	}

	// Ÿ���� ��ȿ���� ���� ���;
	if( m_TargetID.GaeaId == EMTARGET_NULL )
		return false;

	ServerActor* pTarget = m_pGaeaServer->GetTarget(
		pLand, m_TargetID );
	if ( !pTarget || !pTarget->IsValidBody() )
		return false;

	// ������ Ÿ���� �ƴϴ�;
	ServerActor* pOwner = m_pGaeaServer->GetActor(
		pLand, m_sOwnerID );
	if ( !pOwner )
	{
		sc::writeLogError ( "GLSummonField::IsTarget - pOwner is NULL." );
		return false;
	}

	if ( m_TargetID.emCrow == pOwner->GetCrow() &&
		m_TargetID.GaeaId == pOwner->GetGaeaID() )
		return false;

	return true;
}

bool GLSummonField::IsValidOwnerDistance ()
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::IsValidOwnerDistance - pLand is NULL." );
		return true;
	}

	ServerActor* pOwner = m_pGaeaServer->GetActor(
		pLand, m_sOwnerID );
	if ( !pOwner )
	{
		sc::writeLogError ( "GLSummonField::IsValidOwnerDistance - pOwner is NULL." );
		return true;
	}

	D3DXVECTOR3 vLength = pOwner->GetPosition() - GetPosition();
	float fOwnerLength = D3DXVec3Length( &vLength );

	if ( fOwnerLength >= AISystem::GLFSMContainer::Instance()->GetOwnerRange() )
		return false;

	return true;
}

bool GLSummonField::IsValidEventSkillDistance ()
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::IsValidOwnerDistance - pLand is NULL." );
		return true;
	}

	ServerActor* pOwner = m_pGaeaServer->GetActor(
		pLand, m_sOwnerID );
	if ( !pOwner )
	{
		sc::writeLogError ( "GLSummonField::IsValidOwnerDistance - pOwner is NULL." );
		return true;
	}

	D3DXVECTOR3 vLength = pOwner->GetPosition() - GetPosition();
	float fOwnerLength = D3DXVec3Length( &vLength );

	if ( fOwnerLength >= AISystem::GLFSMContainer::Instance()->GetEventSkillRange() )
		return false;

	return true;
}

bool GLSummonField::IsRunAway ()
{
	return m_bRunAway;
}

const DWORD GLSummonField::GetActionCount () const
{
	return m_dwReactCount;
}

void GLSummonField::AutoSearchTarget ()
{
	if ( m_bRunAway )
		return;

	EMIMPACT_SIDE emTargetSide = EMIMPACT_SIDE_PARTY;

	for ( unsigned int i=0; i<m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size(); ++i )
	{
		SCROWATTACK* pTempAttackProp =
			&( m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[ i ] );
		if ( !pTempAttackProp )
			return;

		if ( pTempAttackProp->skill_id == SNATIVEID(false) )
			continue;
		else
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(
				pTempAttackProp->skill_id );
			if ( !pSkill )
				continue;

			if ( emTargetSide < pSkill->m_sBASIC.emIMPACT_SIDE )
				emTargetSide = pSkill->m_sBASIC.emIMPACT_SIDE;
		}
	}

	// Ÿ���� ���� ��� �˻���;
	switch ( emTargetSide )
	{
	case EMIMPACT_SIDE_PARTY:
		if ( !IsTarget() )
			m_TargetID = SearchTargetAction( emTargetSide );
		break;
	default:
		if ( !IsTargetExcludeOwner() )
			m_TargetID = SearchTargetAction( emTargetSide );
		break;
	}
}

STARGETID GLSummonField::AutoEventSkillSearchTarget ( UINT iIdx )
{
	STARGETID sReturnTargetID;

	if ( m_bRunAway )
		return sReturnTargetID;

	// ���� ����� �̺�Ʈ ��ų �˻�;
	CROWATTACK_VEC vecTemp = m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage );;
	if ( iIdx < 0 || iIdx >= vecTemp.size() )
	{
		sc::writeLogInfo( "Invalid EventSkill Index." );
		return sReturnTargetID;
	}

	// ����� �̺�Ʈ ��ų�� TargetSide �˻�;
	EMIMPACT_SIDE emTargetSide = EMIMPACT_SIDE_PARTY;
	SCROWATTACK* pTempAttackProp = &( vecTemp.at( iIdx ) );
	if ( !pTempAttackProp )
		return sReturnTargetID;

	if ( pTempAttackProp->skill_id == SNATIVEID(false) )
		return sReturnTargetID;
	else
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(
			pTempAttackProp->skill_id );
		if ( !pSkill )
			return sReturnTargetID;

		if ( emTargetSide < pSkill->m_sBASIC.emIMPACT_SIDE )
			emTargetSide = pSkill->m_sBASIC.emIMPACT_SIDE;
	}

	// Ÿ���� ���� ��� �˻���;
	switch ( emTargetSide )
	{
	case EMIMPACT_SIDE_PARTY:
		if ( !IsTarget() )
		{
			// Target�� ��� �ٸ� Thread���� ����� �� �ֱ� ������ ��ȯ�� ���� ����ؾ� �Ѵ�;
			// m_TargetID�� �������� ����� �� �ִٰ� �����Ѵ�;
			sReturnTargetID = SearchTargetAction( emTargetSide );
			m_TargetID = sReturnTargetID;
		}
		break;
	default:
		if ( !IsTargetExcludeOwner() )
		{
			// Target�� ��� �ٸ� Thread���� ����� �� �ֱ� ������ ��ȯ�� ���� ����ؾ� �Ѵ�;
			// m_TargetID�� �������� ����� �� �ִٰ� �����Ѵ�;
			sReturnTargetID = SearchTargetAction( emTargetSide );
			m_TargetID = sReturnTargetID;
		}
		break;
	}

	/*sc::writeLogInfo (
		sc::string::format( "Auto Search Main Target '%1% %2%'."
		, sReturnTargetID.emCrow
		, sReturnTargetID.GaeaId ) );*/

	return sReturnTargetID; 
}

void GLSummonField::SetOwnerTarget ()
{
	if ( m_bRunAway )
		return;

	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::SetOwnerTarget - pLand is NULL." );
		return;
	}

	ServerActor* pOwner = m_pGaeaServer->GetActor(
		pLand, m_sOwnerID );
	if ( !pOwner )
	{
		sc::writeLogError ( "GLSummonField::SetOwnerTarget - pOwner is NULL." );
		return;
	}

	// ������ Ÿ���� ����Ѵ�;
	m_TargetID = pOwner->GetTargetID();
}

void GLSummonField::StartTargetAction ( DWORD dwCount )
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::AttackTarget - pLand is NULL." );
		return;
	}

	if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
	{
		// dwCount�� ������ ��� Ư�� ���׼� �������� �״´�;
		if ( dwCount <= m_dwReactCount &&
			dwCount != 0 )
		{
			Order_Die();
		}
	}

	// ����� ������ �����Ѵ�;
	if ( m_TargetID.GaeaId == EMTARGET_NULL )
		return;

	// ����� ������ Ÿ���� �ʱ�ȭ�Ѵ�;
	ServerActor *pTarget = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
	if ( !pTarget || !pTarget->IsValidBody() )
	{
		m_TargetID.RESET();
		return;
	}

	// �̹� �׼� �߽ø� �����Ѵ�;
	if ( isAction( GLAT_ATTACK, GLAT_SKILL ) )
		return;

	// ����� ���� �Ҽ����� ����̸� Ÿ���� �ʱ�ȭ�Ѵ�;
	EMIMPACT_SIDE emTargetSide = EMIMPACT_SIDE_ENEMY;
	if ( !m_bUsingEventSkill )
	{
		if ( m_dwAtypeAfterTracing == SCROWDATA::EMMAXATTACK )
		{
			m_dwAtypeAfterTracing = FindOptReactionType ( m_TargetID, true );
		}

		if ( m_dwAtypeAfterTracing >= m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size() ||
			m_dwAtypeAfterTracing < 0 )
		{
			if ( m_dwAType >= m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size() ||
				m_dwAType < 0 )
				return;

			SCROWATTACK* pTempAttackProp =
				&( m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[ m_dwAType ] );
			if ( !pTempAttackProp )
				return;

			if ( pTempAttackProp->skill_id == SNATIVEID(false) )
			{
				emTargetSide = EMIMPACT_SIDE_ENEMY;
			}
			else
			{
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(
					pTempAttackProp->skill_id );
				if ( !pSkill )
					return;

				emTargetSide = pSkill->m_sBASIC.emIMPACT_SIDE;
			}
		}
		else
		{
			SCROWATTACK* pTempAttackProp =
				&( m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[ m_dwAtypeAfterTracing ] );
			if ( !pTempAttackProp )
				return;

			if ( pTempAttackProp->skill_id == SNATIVEID(false) )
			{
				emTargetSide = EMIMPACT_SIDE_ENEMY;
			}
			else
			{
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(
					pTempAttackProp->skill_id );
				if ( !pSkill )
					return;

				emTargetSide = pSkill->m_sBASIC.emIMPACT_SIDE;

				// �̵��ӵ��� 0�ε� Ÿ���� ��ų �����ȿ� ���ٸ�;
				// ��ų�� Ÿ���� �����Ѵ�;
				if ( GetMoveVelo() <= 0.f )
				{
					if ( !pLand->IsAbleAttack_Range_LayerSimpleCollision( pSkill->m_sBASIC.wTARRANGE, pTarget->GetPosition(), GetPosition(), 5.f ) )
					{
						m_TargetID.RESET();
						//m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
						//m_wRangeTracing = 0;
						TurnAction( GLAT_IDLE );

						return;
					}
				}
			}
		}
	}
	else
	{
		// ���� ����� �̺�Ʈ ��ų �˻�;
		CROWATTACK_VEC vecTemp = m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage );;
		if ( m_dwEventSkillIdx < 0 || m_dwEventSkillIdx >= vecTemp.size() )
		{
			sc::writeLogError( "GLSummonField::StartTargetAction - Invalid EventSkill Index." );
			return;
		}

		// ����� �̺�Ʈ ��ų�� TargetSide �˻�;
		SCROWATTACK* pTempAttackProp = &( vecTemp.at( m_dwEventSkillIdx ) );
		if ( !pTempAttackProp )
			return;

		if ( pTempAttackProp->skill_id == SNATIVEID(false) )
			return;
		else
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(
				pTempAttackProp->skill_id );
			if ( !pSkill )
				return;

			if ( emTargetSide < pSkill->m_sBASIC.emIMPACT_SIDE )
				emTargetSide = pSkill->m_sBASIC.emIMPACT_SIDE;
		}
	}

	if ( IsReActionable( pTarget, emTargetSide ) == EMREACTIONABLE_FALSE )
	{
		m_TargetID.RESET();
		m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;

		return;
	}

	// ���׼����� �ƴϸ�;
	if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
	{
		switch ( SetReactionTarget( m_TargetID ) )
		{
		case EMRETURN_SETREACTION_SUCCESS:
			break;

			// ���׼� ������ �����ϸ� Ư�� ��쿡�� �����Ѵ�;
		case EMRETURN_SETREACTION_FAILED_COLLISION:
		case EMRETURN_SETREACTION_FAILED_RANGE:
			Move_TargetTracing( m_TargetID, m_wRangeTracing );
			break;

		case EMRETURN_SETREACTION_FAILED_SKILL:
			// ��Ÿ���� �����ϸ� �ش��ǥ�� �ӽ� �Ÿ� ��ŭ ����;
			Move_TargetTracing( m_TargetID, 10 );
			break;

		default:
			break;
		}
	}
}

void GLSummonField::DieAction ()
{
	Order_Die();
}

void GLSummonField::ChangeSkillPage ( UINT iPage )
{
	if ( !m_bEventSkill )
		return;

	if ( !m_pSummonCrowData )
	{
		sc::writeLogError( "GLSummonField::ChangeSkillPage - pCrowData is NULL." );
		return;
	}

	if ( iPage >= m_pSummonCrowData->m_vecCrowAttackEventPage.size() ||
		iPage >= m_pSummonCrowData->m_vecCrowAttackPage.size() ||
		iPage < 0 )
		return;

	// �ʱ�ȭ;
	m_dwAType = 0;
	m_dwAtypeAfterTracing = 0;

	// ������ ����;
	m_dwCurSkillPage = iPage;

	GLMSG::SNET_SUMMON_UPDATE_CURSKILLPAGE NetMsg(
		m_dwGaeaID, m_dwCurSkillPage );
	SendMsgViewAround( &NetMsg );
}

void GLSummonField::UseEventSkill ( UINT iIdx )
{
	if ( !m_bEventSkill )
		return;

	if ( !m_pSummonCrowData )
	{
		sc::writeLogError( "GLSummonField::UseEventSkill - pCrowData is NULL." );
		return;
	}

	// ���� ����� �̺�Ʈ ��ų �˻�;
	CROWATTACK_VEC vecTemp = m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage );
	if ( iIdx < 0 || iIdx >= vecTemp.size() )
	{
		sc::writeLogInfo( "Invalid EventSkill Index." );
		return;
	}

	m_dwEventSkillIdx = iIdx;

	// Ÿ�� �� ��ų �ʱ�ȭ;
	m_TargetID.RESET();
	//m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
	//m_wRangeTracing = 0;
	//m_pAttackProp = NULL;
	TurnAction( GLAT_IDLE );
	//SetAction( GLAT_IDLE );

	// ����Ǿ� �ϴ� EventSkill�� �´� Ÿ�� �˻�;
	m_bUsingEventSkill = true;

	STARGETID sTargetID = AutoEventSkillSearchTarget( m_dwEventSkillIdx );

	//sc::writeLogInfo( "Start Event Skill Mode." );

	// Ÿ�ٿ� ���� �׼�;
	SetReactionTarget ( sTargetID, true );
}

void GLSummonField::RunAwayAction ( bool bSpeedUp )
{
	// �̵��ӵ��� ���� ��� �����Ѵ�;
	if ( GetMoveVelo() <= 0.f )
		return;

	m_bRunAway = true;

	// Ÿ�� �ʱ�ȭ;
	m_TargetID.RESET();
	//m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
	//m_wRangeTracing = 0;
	TurnAction( GLAT_IDLE );

	// ������ ���� ��������;
	TracingOwner( bSpeedUp );
}

void GLSummonField::NormalAction ()
{
	m_bRunAway = false;
}

void GLSummonField::TracingOwner ( bool bSpeedUp )
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::TracingOwner - pLand is NULL." );
		return;
	}

	ServerActor* pOwner = m_pGaeaServer->GetActor(
		pLand, m_sOwnerID );
	if ( !pOwner )
	{
		sc::writeLogError ( "GLSummonField::TracingOwner - pOwner is NULL." );
		return;
	}

	// �̹� ������ ���� ���̶��;
	/*if ( m_TargetID.emCrow == pOwner->GetCrow() &&
		m_TargetID.GaeaId == pOwner->GetGaeaID() )
		return;*/

	// Ÿ���� ������;
	if ( !IsTargetExcludeOwner() ) 
	{
		// ���׼����� �ƴϸ�;
		if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
		{
			// ���� ����;
			m_TargetID.emCrow = pOwner->GetCrow();
			m_TargetID.GaeaId = pOwner->GetGaeaID();
			Move_TargetTracing(
				m_TargetID,
				static_cast< WORD >( m_fOwnerDistance ),
				true,
				bSpeedUp );
		}
	}
}

void GLSummonField::TracingTarget( bool bSpeedUp )
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::TracingTarget - pLand is NULL." );
		return;
	}

	// Ÿ���� ��ȿ���� ���� ���;
	if( m_TargetID.GaeaId == EMTARGET_NULL )
		return;

	ServerActor* pTarget = m_pGaeaServer->GetTarget(
		pLand, m_TargetID );
	if ( !pTarget || !pTarget->IsValidBody() )
		return;

	// ���׼����� �ƴϸ�;
	if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
	{
		Move_TargetTracing(
			m_TargetID,
			static_cast< WORD >( 0 ),
			true,
			bSpeedUp );
	}
	
}

void GLSummonField::SyncPositionTarget()
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLSummonField::SyncPositionTarget - pLand is NULL." );
		return;
	}

	// Ÿ���� ��ȿ���� ���� ���;
	if( m_TargetID.GaeaId == EMTARGET_NULL )
		return;

	ServerActor* pTarget = m_pGaeaServer->GetTarget(
		pLand, m_TargetID );
	if ( !pTarget || !pTarget->IsValidBody() )
		return;

	// ���׼����� �ƴϸ�;
	if ( !isAction( GLAT_ATTACK, GLAT_SKILL ) )
	{
		ReSetActState( EM_ACT_ALL );
		SetActState( EM_ACT_RUN );

		Move( pTarget->GetPosition(), true, false );
	}
}

void GLSummonField::ViewSkillTargetEffect ( SNATIVEID sSkillID )
{
	if ( sSkillID == SNATIVEID( false ) )
		return;

	// ����Ʈ ��� ( EventSkill�� ����� �� Effect�� ����Ѵ� );
	GLMSG::SNET_SUMMON_EFFECT NetMsg(
		m_dwGaeaID );
	NetMsg.sSkillID = sSkillID;
	SendMsgViewAround( &NetMsg );
}

void GLSummonField::Update_ApplySkillEffect ( EMSPEC_ADDON emSpecAddon, bool bState )
{
	// Summon �� �����Ų��;
	m_bSkillFactsSpecial[ emSpecAddon ] = bState;

	// �ֺ��� �˸���;
	GLMSG::SNET_APPLY_SKILLEFFECT_FC sNetMsg(
		GetActorID(), emSpecAddon, bState );
	SendMsgViewAround( &sNetMsg );
}