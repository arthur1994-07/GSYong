
#include "pch.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"

#include "./MacroDefine.h"
#include "./BaseMacro.h"
#include "./ReturnByOptionMacro.h"

#include "./MacroManager.h"

ReturnByOptionMacro::ReturnByOptionMacro(GLGaeaClient* pGaeaClient)
: BaseMacro(pGaeaClient)
, m_bCheckDrug(false)
, m_bCheckLimitTime(false)
, m_emReturnType(TYPE_NONE)
{
	m_MacroType = MACRO::MACRO_RETURNBYOPTION;
	// 전이 가능 매크로 등록;
	m_vecTransferableType.push_back(MACRO::MACRO_CLOSE);
}

ReturnByOptionMacro::~ReturnByOptionMacro(void)
{
}

void ReturnByOptionMacro::DoInit(void)
{
	m_emReturnType = TYPE_NONE;
}

void ReturnByOptionMacro::DoUpdate(float fTime, float fElapsedTime)
{
}

void ReturnByOptionMacro::DoApply(void)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	const SINVENITEM* const pInvenItem = pCharacter->m_cInventory.FindDrugItem(ITEM_DRUG_CALL_SCHOOL);
	if ( pInvenItem )
	{
		DWORD dwReturnItemCount = pCharacter->m_cInventory.GetAmountDrugItem(ITEM_DRUG_CALL_SCHOOL);
		pCharacter->ReqReCall(pInvenItem->wPosX, pInvenItem->wPosY);
		switch ( m_emReturnType )
		{
		case TYPE_DRUG:
			m_pGaeaClient->PrintConsoleText(sc::string::format(CGameTextMan::GetInstance().GetGameWord("MACRO_RETURNTEXT", (int)TYPE_DRUG).GetString(), GLCONST_CHAR::nMacro_TermsofClose_Potion));
			break;
		case TYPE_TIME:
			m_pGaeaClient->PrintConsoleText(sc::string::format(CGameTextMan::GetInstance().GetGameWord("MACRO_RETURNTEXT", (int)TYPE_TIME).GetString(), GLCONST_CHAR::fMacro_TermsofClose_LimitTime));
			break;
		}

		// 학원 귀환 아이템을 이용하고 1나 이하라면 더이상 학원 귀환이 불가능 한 상태로 보고 매크로 설정을 끔.
		if( dwReturnItemCount <= 1 )
		{
			this->DoEndProcedure();
			GLMacroManager::GetInstance().SetCheckRemainMacroTime(false);
			GLMacroManager::GetInstance().SetCheckRemainDrugCount(false);

			GLWidgetScript::GetInstance().LuaCallFunc<void>( 
				NSWIDGET_SCRIPT::g_strFun_ResetReMainButtons);
		}

	}
}

void ReturnByOptionMacro::DoEndProcedure(void)
{
	m_emReturnType = TYPE_NONE;
}

bool ReturnByOptionMacro::CheckTransferable(void)
{	
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	const SINVENITEM* const pInvenItem = pCharacter->m_cInventory.FindDrugItem(ITEM_DRUG_CALL_SCHOOL);
	if ( pInvenItem == NULL )
		return false;
	
	// 아래 상수들은 defaultcharclass로 빼야된다 까먹지말자;
	if ( m_bCheckLimitTime == true )
	{								
		if ( pCharacter->m_fRemainMacroTime <= GLCONST_CHAR::fMacro_TermsofClose_LimitTime )
		{
			m_emReturnType = TYPE_TIME;
			return true;
		}
	}
	if ( m_bCheckDrug == true )
	{
		/*
		, ITEM_DRUG_HP
		, ITEM_DRUG_HP_MP		
		, ITEM_DRUG_MP_SP		
		, ITEM_DRUG_HP_MP_SP
		, ITEM_DRUG_HP_CURE		
		, ITEM_DRUG_HP_MP_SP_CURE	
		*/
		const DWORD dwDrugCount = pCharacter->m_cInventory.CountDrugItem(ITEM_DRUG_HP)
			+ pCharacter->m_cInventory.CountDrugItem(ITEM_DRUG_HP_MP)
			+ pCharacter->m_cInventory.CountDrugItem(ITEM_DRUG_HP_MP_SP)
			+ pCharacter->m_cInventory.CountDrugItem(ITEM_DRUG_HP_CURE)
			+ pCharacter->m_cInventory.CountDrugItem(ITEM_DRUG_HP_MP_SP_CURE);

		if ( dwDrugCount <= GLCONST_CHAR::nMacro_TermsofClose_Potion )
		{
			m_emReturnType = TYPE_DRUG;
			return true;
		}
	}
	return false;
}


bool ReturnByOptionMacro::CheckEnd(void)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter->GetAction() == GLAT_IDLE && pCharacter->IsEmptyReservationAction() == true )
		return true;

	return false;
}