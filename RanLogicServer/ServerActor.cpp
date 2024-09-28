#include "pch.h"

#include "../SigmaCore/Math/Random.h"
#include "../RanLogic/Msg/GLContrlMsg.h"
#include "../RanLogic/Skill/GLSkill.h"
#include "../RanLogic/GLogicData.h"
#include "./FieldServer/GLGaeaServer.h"
#include "./Land/GLLandMan.h"

#include "./AI/CAICompField.h"

#include "ServerActor.h"

#include "../SigmaCore/DebugInclude.h"

STARGETID g_InvalidDummyTarget;

ServerActor::ServerActor(GLGaeaServer* const pGaeaServer, const EMCROW emCrow)
: GLActor(emCrow)
, m_pGaeaServer(pGaeaServer)
, m_dwCeID(0)
{		
}


ServerActor::~ServerActor()
{	
}

void ServerActor::Reset(void)
{
	GLActor::Reset();
}

BOOL ServerActor::IsVisibleDetect( ServerActor* pAttacker ) const
{
	if ( NULL == pAttacker )
		return FALSE;

	// 자기자신이라면 무조건 찾을 수 있다;
	if ( pAttacker->GetActorID() == GetActorID() )
		return TRUE;

	// 숨어있는 상태이면 찾을 수 없다;
	if ( IsActState( EM_REQ_VISIBLENONE | EM_REQ_VISIBLEOFF ) )
		return FALSE;

	// 스킬로 숨어있는 상태가 아니라면 찾을 수 있다;
	if ( false == IsCheckedSkillFlagSpecial( EMSPECA_INVISIBLE ) )
		return TRUE;

	// 은신감지가 있을 경우 찾을 수 있다;
	if ( pAttacker->IsCheckedSkillFlagSpecial( EMSPECA_RECVISIBLE ) )
		return TRUE;

	// 그외의 경우에는 찾을 수 없다;
	return FALSE;
}

// Party;
const bool ServerActor::isValidParty(void) const
{
	return m_PartyID.isValidParty();
}
const bool ServerActor::isValidExpedition(void) const
{
	return m_PartyID.isValidExpedition();
}
const GLPartyID& ServerActor::GetPartyID(void) const
{
	return m_PartyID;
}
void ServerActor::SetPartyID(const GLPartyID& PartyID)
{
	m_PartyID = PartyID;
}
void ServerActor::ResetPartyID(void)
{
	m_PartyID.Reset();
}
// Party;



// Battle;

/*
 * 단순히 대상이 공격을 막았는지에 대한 검사만이 아닌, 유효성 검사도 함꼐 함;
*/
const bool ServerActor::CalcBlocking(ServerActor* const pTargetActor, const int nDamage, const GLSKILL* const pSkill /*= NULL*/, const DWORD dwSkillLevel /*= 0*/)
{
	if ( pTargetActor == NULL )
		return true;		

	// 2014.09.03;
	// nDamage : 일반 공격은 양수, 스킬은 음수로 들어 오고 있다;

	// Note : 스킬 정보 가져옴.
	if ( pSkill != NULL )
	{
		const SKILL::CDATA_LVL& sSKILL_DATA(pSkill->m_sAPPLY.sDATA_LVL[dwSkillLevel]);
		switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
		{
		case SKILL::EMFOR_HP:
			if ( (sSKILL_DATA.fBASIC_VAR < 0.0f) && (nDamage == 0) )
				return true;
			break;
		}

		return false;
	}

	if ( nDamage == 0 )
		return true;

	return false;
}

GLLandMan* const ServerActor::GetLandMan(void)
{
	GLLandMan* const pLand(m_pGaeaServer->GetLand(GetCurrentMap()));
	return pLand;
}
GLLandManager* const ServerActor::GetParentLandManager(void)
 {
	 GLLandManager* const pLandManager(m_pGaeaServer->GetLandManager(GetCurrentMap()));
	 return pLandManager;
}

void ServerActor::RemoveAllChildCrow()
{
	GLLandMan* const pLand =
		m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::RemoveAllChildCrow - pLand is NULL." );
		return;
	}

	for( CROWIDLIST_ITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); )
	{
		Faction::GLActorID sID = *iter;
		++iter;
		switch ( sID.actorType )
		{
		case CROW_SUMMON:
			pLand->DropOutSummon( sID.actorID_Num );
			break;
		default:
			sc::writeLogError( "RemoveAllChildCrow - Invalid Crow Type." );
			break;
		}
	}

	m_listCrowID.clear();
}

void ServerActor::KillAllChildCrow ()
{
	GLLandMan* const pLand =
		m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::KillAllChildCrow - pLand is NULL." );
		return;
	}

	for( CROWIDLIST_ITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); )
	{
		Faction::GLActorID sID = *iter;
		++iter;
		switch ( sID.actorType )
		{
		// Actor는 오직 Summon만을 가질 수 있다;
		case CROW_SUMMON:
			{
				PGLSUMMONFIELD pSummon =
					pLand->GetSummon( sID.actorID_Num );
				if ( !pSummon || !pSummon->IsValidBody() )
				{
					continue;
				}

				pSummon->Order_Die();
			}
			break;
		default:
			sc::writeLogError( "KillAllChildCrow - Invalid Crow Type." );
			break;
		}
	}

	m_listCrowID.clear();
}

void ServerActor::KillAllHireChildCrow ()
{
	GLLandMan* const pLand =
		m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::KillAllChildCrow - pLand is NULL." );
		return;
	}

	for( CROWIDLIST_ITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); )
	{
		Faction::GLActorID sID = *iter;
		//++iter;

		//bool bErase = false;
		switch ( sID.actorType )
		{
			// Actor는 오직 Summon만을 가질 수 있다;
		case CROW_SUMMON:
			{
				PGLSUMMONFIELD pSummon =
					pLand->GetSummon( sID.actorID_Num );
				if ( !pSummon || !pSummon->IsValidBody() )
				{
					++iter;
					continue;
				}

				if ( pSummon->IsHireSummon() )
				{
					pSummon->Order_Die();
					//bErase = true;
					iter = m_listCrowID.erase( iter );
				}
				else
				{
					++iter;
				}
			}
			break;
		default:
			++iter;
			sc::writeLogError( "KillAllChildCrow - Invalid Crow Type." );
			break;
		}

		/*if ( !bErase )
			++iter;*/
	}
}

void ServerActor::OwnerAttack ()
{
	AISystem::CAICompField* pAI =
		static_cast< AISystem::CAICompField* >(
		GetComponent( ComponentSystem::EMFAMILY_AI ) );
	if ( pAI )
	{
		pAI->OwnerAttack();
	}
}

void ServerActor::OwnerAttacked ( ServerActor* const pAttacker )
{
	AISystem::CAICompField* pAI =
		static_cast< AISystem::CAICompField* >(
		GetComponent( ComponentSystem::EMFAMILY_AI ) );
	if ( pAI )
	{
		pAI->OwnerAttacked( pAttacker );
	}
}

void ServerActor::SendToChildCrowAttack ()
{
	GLLandMan* const pLand = m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::SendToChildCrowAttack - pLand is NULL." );
		return;
	}

	for( CROWIDLIST_ITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); ++iter )
	{
		Faction::GLActorID sID = *iter;
		ServerActor* pTempActor =
			m_pGaeaServer->GetActor( pLand, sID );
		if ( pTempActor )
			pTempActor->OwnerAttack();
	}
}

void ServerActor::SendToChildCrowAttacked ( ServerActor* const pAttacker )
{
	GLLandMan* const pLand = m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::SendToChildCrowAttacked - pLand is NULL." );
		return;
	}

	for( CROWIDLIST_ITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); ++iter )
	{
		Faction::GLActorID sID = *iter;
		ServerActor* pTempActor =
			m_pGaeaServer->GetActor( pLand, sID );
		if ( pTempActor )
			pTempActor->OwnerAttacked( pAttacker );
	}
}

void ServerActor::ChangeAIState ( const char* szChangeName )
{
	AISystem::CAICompField* pAI =
		static_cast< AISystem::CAICompField* >(
		GetComponent( ComponentSystem::EMFAMILY_AI ) );
	if ( pAI )
	{
		pAI->ChangeState( szChangeName );
	}
}

const DWORD ServerActor::GetChildSummonTypeCount ( SUMMON_TYPE emSummonType ) const
{
	GLLandMan* const pLand = m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::GetChildSummonTypeCount - pLand is NULL." );
		return 0;

	}

	DWORD dwCount = 0;
	for( CROWIDLIST_CITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); ++iter )
	{
		Faction::GLActorID sTempID = *iter;
		if ( sTempID.actorType == CROW_SUMMON )
		{
			PGLSUMMONFIELD pSummon = pLand->GetSummon( sTempID.actorID_Num );
			if ( pSummon )
			{
				SUMMON_TYPE emTempSummonType = pSummon->GETSUMMONTYPE();
				if ( emTempSummonType == emSummonType )
					dwCount++;
			}
		}
	}

	return dwCount;
}

const DWORD ServerActor::GetChildSummonAITypeCount ( DWORD dwAIType ) const
{
	GLLandMan* const pLand = m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::GetChildSummonAITypeCount - pLand is NULL." );
		return 0;

	}

	DWORD dwCount = 0;
	for( CROWIDLIST_CITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); ++iter )
	{
		Faction::GLActorID sTempID = *iter;
		if ( sTempID.actorType == CROW_SUMMON )
		{
			PGLSUMMONFIELD pSummon = pLand->GetSummon( sTempID.actorID_Num );
			if ( pSummon )
			{
				DWORD dwTempAIType = pSummon->GetAIType();
				if ( dwTempAIType == dwAIType )
					dwCount++;
			}
		}
	}

	return dwCount;
}

const DWORD ServerActor::GetChildSummonIDCount ( SNATIVEID sSummonCrowID ) const
{
	GLLandMan* const pLand = m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::GetChildSummonIDCount - pLand is NULL." );
		return 0;

	}

	DWORD dwCount = 0;
	for( CROWIDLIST_CITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); ++iter )
	{
		Faction::GLActorID sTempID = *iter;
		if ( sTempID.actorType == CROW_SUMMON )
		{
			PGLSUMMONFIELD pSummon = pLand->GetSummon( sTempID.actorID_Num );
			if ( pSummon )
			{
				SNATIVEID sTempSummonCrowID = pSummon->m_sSummonID;
				
				if ( sSummonCrowID == sTempSummonCrowID )
					dwCount++;
			}
		}
	}

	return dwCount;
}

const Faction::GLActorID ServerActor::RemoveFirstChildSummon (
	SUMMON_TYPE emSummonType, DWORD dwAIType, SNATIVEID sCrowID )
{
	GLLandMan* const pLand = m_pGaeaServer->GetLand( GetCurrentMap() );
	if ( !pLand )
	{
		sc::writeLogError( "ServerActor::RemoveFirstChildSummon - pLand is NULL." );
		return Faction::GLActorID();

	}

	for( CROWIDLIST_ITER iter = m_listCrowID.begin();
		iter != m_listCrowID.end(); ++iter )
	{
		Faction::GLActorID sTempID = *iter;
		if ( sTempID.actorType == CROW_SUMMON )
		{
			PGLSUMMONFIELD pSummon = pLand->GetSummon( sTempID.actorID_Num );
			if ( pSummon )
			{
				// 같은 ID의 첫번째를 제거 하는 경우;
				if ( sCrowID != SNATIVEID( false ) )
				{
					SNATIVEID sTempSummonCrowID = pSummon->m_sSummonID;
					if ( sCrowID == sTempSummonCrowID )
					{
						m_listCrowID.erase( iter );
						return sTempID;
					}
				}
				// 전체에서 첫번째를 제거 하는 경우;
				else
				{
					SUMMON_TYPE emTempSummonType = pSummon->GETSUMMONTYPE();
					DWORD dwTempAIType = pSummon->GetAIType();
					if ( emTempSummonType == emSummonType && dwTempAIType == dwAIType )
					{
						m_listCrowID.erase( iter );
						return sTempID;
					}
				}
			}
		}
	}

    return Faction::GLActorID();
}