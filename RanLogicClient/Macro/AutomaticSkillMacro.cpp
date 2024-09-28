
#include "pch.h"
#include "../Char/GLCharacter.h"
#include "../Input/GLInputManager.h"
#include "../GLGaeaClient.h"
#include "../Land/Faction/FactionManagerClient.h"
#include "../Land/GLLandManClient.h"
#include "../Widget/GLWidgetScript.h"

#include "./MacroDefine.h"
#include "./BaseMacro.h"
#include "./MacroManager.h"
#include "./AutomaticSkillMacro.h"

typedef std::list<MACRO::ReservedSkillID_Macro>::iterator LIST_SKILLID_MACRO_ITR;

AutomaticSkillMacro::AutomaticSkillMacro(GLGaeaClient* pGaeaClient)
: BaseMacro(pGaeaClient)
, m_sNIDLasSkill(NATIVEID_NULL())
, m_nLastUseSkillTab(0)
{
	m_MacroType = MACRO::MACRO_AUTOMATICSKILLUSE;
	// 전이 가능 매크로 등록;
	m_vecTransferableType.push_back(MACRO::MACRO_RETURNBYOPTION);
	m_vecTransferableType.push_back(MACRO::MACRO_AUTOMATICSKILLUSE);
	m_vecTransferableType.push_back(MACRO::MACRO_NEARESTTARGETTRACE);

	for ( int i = 0; i < EMSKILLQUICKTAB_SIZE; ++i )
		m_bUseSkillTab[i] = false;
}

AutomaticSkillMacro::~AutomaticSkillMacro(void)
{
}

void AutomaticSkillMacro::DoInit(void)
{
	SlotInfoUpdate();
}

void AutomaticSkillMacro::DoUpdate(float fTime, float fElapsedTime)
{
}

void AutomaticSkillMacro::DoApply(void)
{
	// 해당 조건문에 만족이 된다면 사용 슬롯 탭이 하나도 없는것이다;
	if ( m_lstReservedSkill.size() == 0 )
		return;

	const MACRO::ReservedSkillID_Macro sMacroData = m_lstReservedSkill.front();
 	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();

	if ( CheckSkill(sMacroData.sSkillID) )
	{
		if ( pCharacter->SkillActive(sMacroData.sSkillID, sMacroData.wSkillQuickSlot, true) == false )
		{
			pCharacter->ResetSelectID();
		}
		// 탭이 다를경우만 탭갱신;
		const INT nCurrentSkillTabIdx(sMacroData.wSkillQuickSlot/EMSKILLQUICKTAB_SLOTSIZE);
		if ( m_nLastUseSkillTab != nCurrentSkillTabIdx )
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_SQSGroup, "-n", nCurrentSkillTabIdx );

		// 캐싱해둘 필요가 있는게, 다음탭1번 스킬이 불발이되면;
		// 탭 전환이 안이루어질 수 있음;
		m_nLastUseSkillTab = nCurrentSkillTabIdx;
	}
	m_lstReservedSkill.pop_front();
	m_lstReservedSkill.push_back(sMacroData);
}

// GLCharacter 스킬 사용부분에 스킬 사용가능한지 체크하는 부분과 거의 동일하다;
// 합칠까?;
bool AutomaticSkillMacro::CheckSkill(const SNATIVEID& sSkillID) const
{
	GLLandManClient* const pLandMClient = m_pGaeaClient->GetLandManClient();
	if ( !pLandMClient )
		return false;

	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	EMIMPACT_SIDE		emSIDE		= pSkill->m_sBASIC.emIMPACT_SIDE;

	// 타겟 검사;
	STARGETID& sTargetID = pCharacter->GetSelectID();
	if ( !sTargetID.ISNULL() )
	{
		ClientActor*	pActor = m_pGaeaClient->GetCopyActor( sTargetID );
		if( !pActor )
			return false;

		//Faction::ManagerClient* const _factionMan = pLandMClient->GetFactionManager();
		//const Faction::EMRELATION_TYPE _relation = _factionMan->getRelationClient(pCharacter, pActor);
		//! 적에게 사용하고 선택된 타겟이 유효하지 않다면;
		if ( pSkill->IsHarm() && emSIDE == EMIMPACT_SIDE_ENEMY )// emSIDE == EMIMPACT_SIDE_ENEMY )
		{			
			if ( !pActor->IsValidBody() || (FALSE == pCharacter->IsReActionable(sTargetID, EMIMPACT_SIDE_ENEMY)) )
				sTargetID.RESET();
		}
		else // 이로운 스킬;
		{
			if ( pCharacter->IsReActionable(sTargetID, EMIMPACT_SIDE_ENEMY) )
				sTargetID.RESET();
		}
	}

	// 타겟 세팅;
	if ( sTargetID.ISNULL() )
	{
		if ( emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			//! 현재 선택된 타겟이 없지만 공격중이라면, 그 타겟을 선택한다;
			if ( pCharacter->isAction(GLAT_ATTACK) )
			{
				pCharacter->SetSelectID(pCharacter->GetActionID());
				sTargetID = pCharacter->GetActionID();
			}
			else
			{
				GLInputManager::GetInstance().ExternEvent_SelectAround();
				sTargetID = pCharacter->GetSelectID();
			} //if..else

			//  [12/5/2013 gbgim];
			// - 예약스킬의 경우 해당 적이 사라지고, 주위에 다른 행위 대상자가 없을 경우; 
			// 위의 구문으로는 처기가 되지않고 NULL값이 담긴다 그렇기 때문에 한번더 검사한다;
			if ( sTargetID.ISNULL() )
				return false;
		}
		else if ( emSIDE == EMIMPACT_SIDE_ANYBODY || emSIDE == EMIMPACT_SIDE_PARTY )				//! 만약 PC에게 사용가능한 스킬이고 타겟이 없다면 자신에게 쓴다.
		{
			sTargetID = STARGETID(CROW_PC, pCharacter->GetGaeaID(), pCharacter->GetPosition());
		}
	}
	return (EMSKILL_OK == pCharacter->SkillCheck(sSkillID, pCharacter->GetSelectID(), 1, false, true, false));
}

void AutomaticSkillMacro::DoEndProcedure(void)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->ClearReservationAction();

	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData(m_sNIDLasSkill);
	if ( pSkill && pSkill->IsBuff() )
	{
		pCharacter->ResetSelectID();
	}
}

bool AutomaticSkillMacro::CheckTransferable(void)
{
	// 해당 조건문에 만족이 된다면 사용 슬롯 탭이 하나도 없는것이다;
	if ( m_lstReservedSkill.size() == 0 )
		return false;

	// 타겟이 있냐없냐 체크;
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	STARGETID& sTargetID = pCharacter->GetSelectID();

	const MACRO::ReservedSkillID_Macro sMacroData = m_lstReservedSkill.front();	
	PGLSKILL		pSkill = GLSkillMan::GetInstance().GetData ( sMacroData.sSkillID.wMainID, sMacroData.sSkillID.wSubID );
	EMIMPACT_SIDE	emSIDE = pSkill->m_sBASIC.emIMPACT_SIDE;
	//! 해당 타겟이 유효한지 검사;
	if ( !sTargetID.ISNULL() )
	{
		ClientActor*	pActor = m_pGaeaClient->GetCopyActor( sTargetID );
		//! 타겟이 유효하지않다면;
		if( !pActor )
		{
			sTargetID.RESET();
		}
		//! 적에게 사용하고 선택된 타겟이 유효하지 않다면;
		else if ( pSkill->IsHarm() && emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			if ( !pActor->IsValidBody() )
				sTargetID.RESET();
		}
	}
	
	//! 선택된 타겟이 없다면;
	if ( sTargetID.ISNULL() )
	{
		if ( emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			GLInputManager::GetInstance().ExternEvent_SelectAround();
			sTargetID = pCharacter->GetSelectID();
			
			if ( sTargetID.ISNULL() )
			{
				pCharacter->ResetSelectID();
				return false;
			}
		}
	}
	return true;
}

bool AutomaticSkillMacro::CheckEnd(void)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter->GetAction() == GLAT_IDLE && !pCharacter->IsExistReservationAction(REACT_SKILL))//&& pCharacter->IsEmptyReservationAction() == true )
		return true;

	return false;
}

void AutomaticSkillMacro::SetSkillTabEnable(WORD wSlotNumber, bool bEnable)
{
	if ( EMSKILLQUICKTAB_SIZE <= wSlotNumber )
		return;

	m_bUseSkillTab[wSlotNumber] = bEnable;

	DoInit();
}

bool AutomaticSkillMacro::IsSkillTabEnable(WORD wSlotNumber)
{
	if ( EMSKILLQUICKTAB_SIZE <= wSlotNumber )
		return false;

	return m_bUseSkillTab[wSlotNumber];
}

INT AutomaticSkillMacro::GetCheckTabCount(void)
{
	INT nCount(0), i(0);
	for ( ; i < EMSKILLQUICKTAB_SIZE; ++i )
	{
		if ( m_bUseSkillTab[i] )
			nCount += 1;
	}
	return nCount;
}

void AutomaticSkillMacro::SlotInfoUpdate(void)
{
	m_lstReservedSkill.clear();
	const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	const SNATIVEID sNative_Null = SNATIVEID(SNATIVEID::ID_NULL,SNATIVEID::ID_NULL);
	for ( WORD i=0; i<EMSKILLQUICK_SIZE; ++i )
	{
		if ( m_bUseSkillTab[i/EMSKILLQUICKTAB_SLOTSIZE] == true 
			&& pCharacter->m_sSKILLQUICK[i] != sNative_Null )
		{
			PGLSKILL		pSkill = GLSkillMan::GetInstance().GetData ( pCharacter->m_sSKILLQUICK[i] );
			// 패치시 스킬슬롯에 무효한 스킬이 등록되어있을 수 있다;
			if ( !pSkill )
				continue;

			m_lstReservedSkill.push_back(MACRO::ReservedSkillID_Macro(pCharacter->GetSkillIDQSlot(i), i));
		}
	}
}