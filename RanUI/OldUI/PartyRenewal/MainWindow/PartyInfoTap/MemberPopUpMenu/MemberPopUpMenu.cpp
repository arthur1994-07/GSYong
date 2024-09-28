#include "StdAfx.h"

#include "../../../../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "MemberPopUpMenu.h"

MemberPopUpMenu::MemberPopUpMenu(EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
{
}
MemberPopUpMenu::~MemberPopUpMenu(void)
{
}

void MemberPopUpMenu::CreateSubControl(void)
{
	// 보류; ( 현재 공용 팝업창 제작 중);
// 	m_pPopUpMenu = new CBasicComboBoxRollOver(m_pEngineDevice);
// 	m_pPopUpMenu->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, MEMBER_MENU_POPUP );
// 	m_pPopUpMenu->CreateBaseComboBoxRollOver( "PARTY_INFO_MEMBER_POPUP_MENU" );			
// 	m_pPopUpMenu->SetVisibleSingle( FALSE );
// 	RegisterControl( m_pPopUpMenu );
// 	m_pPopUpMenu->ClearText();
// 	for ( int _i = 0; _i < MEMBER_MENU_NSIZE; _i++ )
// 		m_pPopUpMenu->AddText( (char*)ID2GAMEWORD("PARTY_RENEWAL_POPUP_MENU_TEXT", _i) );
}

void MemberPopUpMenu::SetVisibleSingle(BOOL bVisible)
{
	CUIGroupHelper::SetVisibleSingle(bVisible);	
//	m_pPopUpMenu->SetVisibleSingle(bVisible);
}

