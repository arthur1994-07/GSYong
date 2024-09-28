
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
	// ���� ���� ��ũ�� ���;
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
	// �ش� ���ǹ��� ������ �ȴٸ� ��� ���� ���� �ϳ��� ���°��̴�;
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
		// ���� �ٸ���츸 �ǰ���;
		const INT nCurrentSkillTabIdx(sMacroData.wSkillQuickSlot/EMSKILLQUICKTAB_SLOTSIZE);
		if ( m_nLastUseSkillTab != nCurrentSkillTabIdx )
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_SQSGroup, "-n", nCurrentSkillTabIdx );

		// ĳ���ص� �ʿ䰡 �ִ°�, ������1�� ��ų�� �ҹ��̵Ǹ�;
		// �� ��ȯ�� ���̷���� �� ����;
		m_nLastUseSkillTab = nCurrentSkillTabIdx;
	}
	m_lstReservedSkill.pop_front();
	m_lstReservedSkill.push_back(sMacroData);
}

// GLCharacter ��ų ���κп� ��ų ��밡������ üũ�ϴ� �κа� ���� �����ϴ�;
// ��ĥ��?;
bool AutomaticSkillMacro::CheckSkill(const SNATIVEID& sSkillID) const
{
	GLLandManClient* const pLandMClient = m_pGaeaClient->GetLandManClient();
	if ( !pLandMClient )
		return false;

	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	EMIMPACT_SIDE		emSIDE		= pSkill->m_sBASIC.emIMPACT_SIDE;

	// Ÿ�� �˻�;
	STARGETID& sTargetID = pCharacter->GetSelectID();
	if ( !sTargetID.ISNULL() )
	{
		ClientActor*	pActor = m_pGaeaClient->GetCopyActor( sTargetID );
		if( !pActor )
			return false;

		//Faction::ManagerClient* const _factionMan = pLandMClient->GetFactionManager();
		//const Faction::EMRELATION_TYPE _relation = _factionMan->getRelationClient(pCharacter, pActor);
		//! ������ ����ϰ� ���õ� Ÿ���� ��ȿ���� �ʴٸ�;
		if ( pSkill->IsHarm() && emSIDE == EMIMPACT_SIDE_ENEMY )// emSIDE == EMIMPACT_SIDE_ENEMY )
		{			
			if ( !pActor->IsValidBody() || (FALSE == pCharacter->IsReActionable(sTargetID, EMIMPACT_SIDE_ENEMY)) )
				sTargetID.RESET();
		}
		else // �̷ο� ��ų;
		{
			if ( pCharacter->IsReActionable(sTargetID, EMIMPACT_SIDE_ENEMY) )
				sTargetID.RESET();
		}
	}

	// Ÿ�� ����;
	if ( sTargetID.ISNULL() )
	{
		if ( emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			//! ���� ���õ� Ÿ���� ������ �������̶��, �� Ÿ���� �����Ѵ�;
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
			// - ���ེų�� ��� �ش� ���� �������, ������ �ٸ� ���� ����ڰ� ���� ���; 
			// ���� �������δ� ó�Ⱑ �����ʰ� NULL���� ���� �׷��� ������ �ѹ��� �˻��Ѵ�;
			if ( sTargetID.ISNULL() )
				return false;
		}
		else if ( emSIDE == EMIMPACT_SIDE_ANYBODY || emSIDE == EMIMPACT_SIDE_PARTY )				//! ���� PC���� ��밡���� ��ų�̰� Ÿ���� ���ٸ� �ڽſ��� ����.
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
	// �ش� ���ǹ��� ������ �ȴٸ� ��� ���� ���� �ϳ��� ���°��̴�;
	if ( m_lstReservedSkill.size() == 0 )
		return false;

	// Ÿ���� �ֳľ��� üũ;
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	STARGETID& sTargetID = pCharacter->GetSelectID();

	const MACRO::ReservedSkillID_Macro sMacroData = m_lstReservedSkill.front();	
	PGLSKILL		pSkill = GLSkillMan::GetInstance().GetData ( sMacroData.sSkillID.wMainID, sMacroData.sSkillID.wSubID );
	EMIMPACT_SIDE	emSIDE = pSkill->m_sBASIC.emIMPACT_SIDE;
	//! �ش� Ÿ���� ��ȿ���� �˻�;
	if ( !sTargetID.ISNULL() )
	{
		ClientActor*	pActor = m_pGaeaClient->GetCopyActor( sTargetID );
		//! Ÿ���� ��ȿ�����ʴٸ�;
		if( !pActor )
		{
			sTargetID.RESET();
		}
		//! ������ ����ϰ� ���õ� Ÿ���� ��ȿ���� �ʴٸ�;
		else if ( pSkill->IsHarm() && emSIDE == EMIMPACT_SIDE_ENEMY )
		{
			if ( !pActor->IsValidBody() )
				sTargetID.RESET();
		}
	}
	
	//! ���õ� Ÿ���� ���ٸ�;
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
			// ��ġ�� ��ų���Կ� ��ȿ�� ��ų�� ��ϵǾ����� �� �ִ�;
			if ( !pSkill )
				continue;

			m_lstReservedSkill.push_back(MACRO::ReservedSkillID_Macro(pCharacter->GetSkillIDQSlot(i), i));
		}
	}
}