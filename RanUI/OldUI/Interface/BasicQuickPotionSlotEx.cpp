#include "StdAfx.h"
#include "BasicQuickPotionSlotEx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../Item/ItemImage.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicQuickPotionSlotEx::CBasicQuickPotionSlotEx(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CBasicQuickPotionSlot(pGaeaClient, pEngineDevice)
    , m_pInterface(pInterface)
{
}

CBasicQuickPotionSlotEx::~CBasicQuickPotionSlotEx ()
{
}

void  CBasicQuickPotionSlotEx::CreateSubControl ( int nIndex )
{
	CreatePotionImage ( "QUICK_POTION_IMAGE" );
	CreateMouseOver ( "QUICK_POTION_MOUSE_OVER" );	
	m_pPotionMouseOver->SetVisibleSingle ( FALSE );

	m_pMiniText = CreateTextBox ( "BASIC_QUICK_POTION_SLOT_AMOUNT" );
	m_pMiniText->SetTextAlign ( TEXT_ALIGN_RIGHT );
	const D3DXVECTOR2 vPos ( m_pMiniText->GetLocalPos ().left, m_pMiniText->GetLocalPos ().top );
	m_pMiniText->SetLocalPos ( D3DXVECTOR2 ( vPos.x-3.0f, vPos.y ) );

	CBasicTextBox* pTextBox = NULL;
	{	
		std::string strText;	
		strText = GLInputManager::GetInstance().GetShortcutsToString( static_cast<EMSHORTCUTS>(EMSHORTCUTS_QUICKSLOT0+nIndex) );

		//	오른쪽 위 shortcut
		pTextBox = CreateTextBox ( "QUICK_POTION_SLOT_SHORTCUT_TEXT" );
		pTextBox->AddText ( strText.c_str(), NS_UITEXTCOLOR::WHITE );
		m_pTextBox = pTextBox;
	}

	SetSlotIndex ( nIndex );
}

void CBasicQuickPotionSlotEx::SetShotcutText( CString strTemp )
{
	m_pTextBox->SetText( strTemp, NS_UITEXTCOLOR::WHITE );
}


void CBasicQuickPotionSlotEx::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CBasicQuickPotionSlot::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    int nIndex = GetSlotIndex ();
	GASSERT ( 0 <= nIndex && nIndex < EMACTIONQUICK_SIZE && "범위를 벗어납니다." );

	SACTION_SLOT sSlot = m_pGaeaClient->GetCharacter()->m_sACTIONQUICK[nIndex];
	BOOL bSlotEmpty = (sSlot.sNID == NATIVEID_NULL ());
	DWORD dwMsg = GetMessageEx ();
	if ( m_pPotionMouseOver )
	{
		if ( !bSlotEmpty && CHECK_MOUSE_IN ( dwMsg ) && bFirstControl )
		{
			m_pPotionMouseOver->SetVisibleSingle ( TRUE );
		}
		else
		{
			m_pPotionMouseOver->SetVisibleSingle ( FALSE );
		}
	}
}

UIRECT	CBasicQuickPotionSlotEx::GetAbsPosPotionImage ()
{
	return m_pItemImage->GetGlobalPos ();
}

void CBasicQuickPotionSlotEx::ResetText ()
{
	if ( !m_pTextBox )
	{
		return;
	}

	std::string strText;	
	strText = GLInputManager::GetInstance().GetShortcutsToString( static_cast<EMSHORTCUTS>(EMSHORTCUTS_QUICKSLOT0+GetSlotIndex()) );
	m_pTextBox->ClearText();
	m_pTextBox->AddText ( strText.c_str(), NS_UITEXTCOLOR::WHITE );
}