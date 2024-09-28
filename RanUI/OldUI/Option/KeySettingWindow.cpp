#include "StdAfx.h"
#include "KeySettingWindow.h"
#include "ShotCutBox.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/RanParamClient.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"

#include "../../InnerInterface.h"
#include "../Interface/UILeftTopGroup.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


CKeySettingWindow::CKeySettingWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx( pInterface, pEngineDevice )
	, m_dwCurrentSettingType(TAP_DEFAULT)
	, m_emCurrentSettingType(EM_CONTROLTYPE_DEFAULT)
	, m_emSaveSettingType(EM_CONTROLTYPE_NULL)
	, m_pBeginEdit(NULL)
{
}

CKeySettingWindow::~CKeySettingWindow()
{
}

void CKeySettingWindow::InitData()
{
	m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT0] = EMSHORTCUTS_QUICKSLOT0;
	m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT1] = EMSHORTCUTS_QUICKSLOT1;
	m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT2] = EMSHORTCUTS_QUICKSLOT2;
	m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT3] = EMSHORTCUTS_QUICKSLOT3;
	m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT4] = EMSHORTCUTS_QUICKSLOT4;
	m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT5] = EMSHORTCUTS_QUICKSLOT5;

	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT0] = EMSHORTCUTS_SKILLSLOT0;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT1] = EMSHORTCUTS_SKILLSLOT1;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT2] = EMSHORTCUTS_SKILLSLOT2;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT3] = EMSHORTCUTS_SKILLSLOT3;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT4] = EMSHORTCUTS_SKILLSLOT4;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT5] = EMSHORTCUTS_SKILLSLOT5;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT6] = EMSHORTCUTS_SKILLSLOT6;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT7] = EMSHORTCUTS_SKILLSLOT7;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT8] = EMSHORTCUTS_SKILLSLOT8;
	m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT9] = EMSHORTCUTS_SKILLSLOT9;

	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT0]  = EMSHORTCUTS_INVENTORY;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT1]  = EMSHORTCUTS_CHARACTER;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT2]  = EMSHORTCUTS_SKILL;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT3]  = EMSHORTCUTS_PARTY;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT4]  = EMSHORTCUTS_QUEST;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT5]  = EMSHORTCUTS_CLUB;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT6]  = EMSHORTCUTS_FRIEND;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT7]  = EMSHORTCUTS_MINIMAP;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT8]  = EMSHORTCUTS_CHATMACRO;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT9]  = EMSHORTCUTS_ITEMBANK;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT10] = EMSHORTCUTS_POINTSHOP;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT11] = EMSHORTCUTS_RUNMODE;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT12] = EMSHORTCUTS_HELP;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT13] = EMSHORTCUTS_PET;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT14] = EMSHORTCUTS_PEACEMODE;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT15] = EMSHORTCUTS_FOURCEDPKMODE;
	m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT16] = EMSHORTCUTS_PRODUCT;
}

void CKeySettingWindow::Open( const EM_CONTROLTYPE emControlType )
{
	if ( GLUseFeatures::GetInstance().IsUsingNewControlType() )
	{
		if ( m_emSaveSettingType == EM_CONTROLTYPE_NULL )
		{
			m_emSaveSettingType = emControlType;
		}

		m_emCurrentSettingType = emControlType;
	}
	else
	{
		if ( m_emSaveSettingType == EM_CONTROLTYPE_NULL )
		{
			m_emSaveSettingType = EM_CONTROLTYPE_A;
		}

		m_emCurrentSettingType = EM_CONTROLTYPE_A;
	}

	switch ( m_emCurrentSettingType )
	{
	case EM_CONTROLTYPE_A: m_dwCurrentSettingType = KEYSETTING_TAB_TYPE_A; break;
	case EM_CONTROLTYPE_B: m_dwCurrentSettingType = KEYSETTING_TAB_TYPE_B; break;
	};

	TAPSELECTOR::TapSelect( m_dwCurrentSettingType );
	
	GetShotCutKey();
}

CString CKeySettingWindow::GetdwKeyToString(int dwKey)
{
	return CString( GLInputManager::GetInstance().GetDxKeyboardValueToString( static_cast<DWORD>(dwKey) ).c_str() );
}

CString CKeySettingWindow::GetDuplicationShortcutString()
{
	std::set<CString> setShotcut;
	std::set<CString>::iterator pos;
	CString strTemp;

	for( int i = 0; i < MAX_SETTING_KEY; ++i)
	{
		strTemp = m_pEditBox[i]->GetEditString();
		if(strTemp == ""){
			// 정의되지 않은 키가 있습니다.
			return CString();
		}

		pos = setShotcut.find(strTemp);

		if( pos != setShotcut.end() ){
            // strTemp가 중복 정의 되었습니다.
			return (*pos);
		}
		else
		{
			setShotcut.insert(strTemp);
		}
	}

	return CString();

}

const bool CKeySettingWindow::IsEmpty()
{
	CString strTemp;
	for( int i = 0; i < MAX_SETTING_KEY; ++i)
	{
		strTemp = m_pEditBox[i]->GetEditString();
		if(strTemp == "" || strTemp == " " )
		{
			return true;
		}
	}

	return false;
}

void CKeySettingWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if ( bVisible )
	{
		EM_CONTROLTYPE emType = GLInputManager::GetInstance().GetCurrentControlType();
		Open( emType );
	}
}

void CKeySettingWindow::SetShotCutKey()
{	
	CString strTemp;

	int i = 0;

	for( i = 0; i<QUICK_SLOT_NUM; ++i)
	{
		m_QuickSlot[i] = m_pEditBox[i]->GetEditString();

		GLInputManager::GetInstance().SetShortcutsDataFromString( m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT0+i], m_QuickSlot[i].GetString() );
		m_pInterface->SetShotcutText( i, m_QuickSlot[i] );
	}

	for ( i = 0; i<QUICK_SKILL_NUM; ++i)
	{
		m_SkillSlot[i] = m_pEditBox[i + QUICK_SLOT_NUM]->GetEditString();

		GLInputManager::GetInstance().SetShortcutsDataFromString( m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT0+i], m_SkillSlot[i].GetString() );
		m_pInterface->SetShotcutText( i+QUICK_SLOT_NUM, m_SkillSlot[i] );
	}

	for ( i = 0; i<MENU_SHOTCUT_NUM; ++i)
	{
		m_MenuShotcut[i] = m_pEditBox[i + QUICK_SLOT_NUM+QUICK_SKILL_NUM]->GetEditString();
		GLInputManager::GetInstance().SetShortcutsDataFromString( m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT0+i], m_MenuShotcut[i].GetString() );
		m_pInterface->SetShotcutText( i+QUICK_SLOT_NUM+QUICK_SKILL_NUM, m_MenuShotcut[i] );
	}	
}

void CKeySettingWindow::CreateSubControl()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CBasicTextBox* pTextBox = NULL;
	CShotCutBox* pEditBox = NULL;
	CBasicLineBox* pBasicLineBox = NULL;
	
	InitData();

	m_pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	DWORD dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	int nAlign = TEXT_ALIGN_LEFT;

	//	배경
	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_OPTION", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxOption( "KEYSETTING_BACK_BOX" );
	RegisterControl( pBasicLineBox );

	//! 라인 박스
	{
		CBasicLineBoxSmart* pLineBox;
		pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "KEYSETTING_BACK_BOX" );
		pLineBox->CreateSubControl( "KEY_SETTING_BACK_BOX_LINEBOX_TEXINFO" );
		RegisterControl( pLineBox );
	}

	// 기본설정 버튼 KEYSETTING_DEFAULTSETTING
	CBasicTextButton* pDefaultButton = new CBasicTextButton(m_pEngineDevice);
	pDefaultButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, KEYSETTING_DEFAULTSETTING );
	pDefaultButton->CreateBaseButton( "KEYSETTING_DEFAULTSETTING", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "KEYSETTING_APPLYOK", 0 ) );
	RegisterControl( pDefaultButton );

	// 적용 버튼
	CBasicTextButton* pApplyButton = new CBasicTextButton(m_pEngineDevice);
	pApplyButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, KEYSETTING_APPLY );
	pApplyButton->CreateBaseButton( "KEYSETTINGN_APPLY_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "KEYSETTING_APPLYOK", 1 ) );
	RegisterControl( pApplyButton );

	// 확인 버튼 
	CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
	pOKButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, KEYSETTING_OK );
	pOKButton->CreateBaseButton( "KEYSETTING_OK_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "KEYSETTING_APPLYOK", 2 ) );
	RegisterControl( pOKButton );	

	// 임시 코드
	bool bUsingRenewProduct = GLUseFeatures::GetInstance().IsUsingRenewProduct();

	char strTemp[128];
	int i = 0;

	// 퀵슬롯 
	for(i = 0; i < QUICK_SLOT_NUM; ++i)
	{
		sprintf_s (strTemp, sizeof(strTemp), "KEY_SETTING_QUICKSLOT%d", i);
		pTextBox = CreateStaticControl( strTemp, m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText( ID2GAMEWORD ( "KEYSETTING_QUICKSLOT", i ) );
	}

	// 스킬 슬롯
	for (i = 0; i < QUICK_SKILL_NUM; ++i)
	{
		sprintf_s(strTemp, sizeof(strTemp), "KEY_SETTING_QUICKSKILL%d", i);
		pTextBox = CreateStaticControl( strTemp, m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText( ID2GAMEWORD( "KEYSETTING_QUICKSKILL", i ) );		
	}

	// 메뉴 슬롯
	for (i = 0; i < MENU_SHOTCUT_NUM; ++i)
	{
		sprintf_s (strTemp, sizeof(strTemp), "KEY_SETTING_MENUSHOTCUT%d", i);
		pTextBox = CreateStaticControl(strTemp, m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText( ID2GAMEWORD( "KEYSETTING_MENUSHOTCUT", i ) );

		// 임시 코드
		if( i == 16 && !bUsingRenewProduct )
			pTextBox->SetVisibleSingle( FALSE );
	}

	// EditBox배경 생성 ( 퀵슬롯 )
	for(i = 0; i < QUICK_SLOT_NUM; ++i)
	{
		sprintf_s(strTemp, sizeof(strTemp), "KEY_SETTING_QUICKEDIT_BACK%d", i);
		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( strTemp );
		RegisterControl ( pBasicLineBox );        
	}

	// EditBox배경 생성 ( 스킬슬롯 )
	for(i = 0; i < QUICK_SKILL_NUM; ++i)
	{
		sprintf_s (strTemp, sizeof(strTemp), "KEY_SETTING_SKILLEDIT_BACK%d", i);
		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( strTemp );
		RegisterControl( pBasicLineBox );        
	}

	// EditBox배경 생성 ( 메뉴숏컷 )
	for(i = 0; i < MENU_SHOTCUT_NUM; ++i)
	{
		sprintf_s(strTemp, sizeof(strTemp), "KEY_SETTING_SHOTCUTEDIT_BACK%d", i);
		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( strTemp );
		RegisterControl( pBasicLineBox );

		// 임시 코드
		if( i == 16 && !bUsingRenewProduct )
			pBasicLineBox->SetVisibleSingle( FALSE );
	}

    // EditBox 생성 ( 퀵슬롯 )
	for(i = 0; i < QUICK_SLOT_NUM; ++i)
	{
		sprintf_s( strTemp, sizeof(strTemp), "KEY_SETTING_QUICKEDIT%d", i);		
		pEditBox = new CShotCutBox(m_pEngineDevice);
		pEditBox->CreateSub( this, strTemp, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, KEY_SETTING_QUICKEDIT0+i );
		pEditBox->CreateCarrat( "KEY_SETTING_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetFont( m_pFont9 );

		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetShortcutsToString( m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT0+i] );

		m_QuickSlot[i] = strEditString.c_str();
		pEditBox->SetEditString( m_QuickSlot[i] );	
		RegisterControl( pEditBox );
		m_pEditBox[i] = pEditBox;
	}

	// EditBox 생성 ( 스킬슬롯 )
	for(i = 0; i < QUICK_SKILL_NUM; ++i)
	{
		sprintf_s(strTemp, sizeof(strTemp), "KEY_SETTING_SKILLEDIT%d", i);		
		pEditBox = new CShotCutBox(m_pEngineDevice);
		pEditBox->CreateSub( this, strTemp, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, KEY_SETTING_SKILLEDIT0+i );
		pEditBox->CreateCarrat( "KEY_SETTING_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetFont( m_pFont9 );

		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetShortcutsToString( m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT0+i] );
		
		m_SkillSlot[i] = strEditString.c_str();
		pEditBox->SetEditString( m_SkillSlot[i] );	
		RegisterControl( pEditBox );
		m_pEditBox[i+QUICK_SLOT_NUM] = pEditBox;
	}

	// EditBox배경 생성 ( 메뉴숏컷 )
	for(i = 0; i < MENU_SHOTCUT_NUM; ++i)
	{
		sprintf_s(strTemp, sizeof(strTemp), "KEY_SETTING_SHOTCUTEDIT%d", i);		
		pEditBox = new CShotCutBox(m_pEngineDevice);
		pEditBox->CreateSub( this, strTemp, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, KEY_SETTING_SHOTCUTEDIT0+i );
		pEditBox->CreateCarrat( "KEY_SETTING_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetFont( m_pFont9 );

		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetShortcutsToString( m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT0+i] );

		m_MenuShotcut[i] = strEditString.c_str();
		pEditBox->SetEditString( m_MenuShotcut[i] );	
		RegisterControl( pEditBox );
		m_pEditBox[i+QUICK_SLOT_NUM+QUICK_SKILL_NUM] = pEditBox;

		// 임시 코드
		if( i == 16 && !bUsingRenewProduct )
			pEditBox->SetVisibleSingle( FALSE );
	}



    //////////////////////////////////////////////////////////////////////////
    // 변경할 수 없는 공격 키 
    {
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub ( this, "KEY_SETTING_ATTACK" );
        pStaticText->SetFont ( m_pFont9 );
        pStaticText->SetTextAlign ( nAlign );
        pStaticText->SetOneLineText( ID2GAMEWORD( "KEY_SETTING_ATTACK", 0 ) );
        RegisterControl ( pStaticText );
    }

    {
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub ( this, "KEY_SETTING_ATTACK_KEY" );
        pStaticText->SetFont ( m_pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_CENTER_X );
        RegisterControl ( pStaticText );


        m_pAtteckKey = pStaticText;
    }

    pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
    pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pBasicLineBox->CreateBaseBoxEditBox( "KEY_SETTING_ATTACK_KEY_BACK" );
    RegisterControl( pBasicLineBox );        

    

	// bjju.controltypeB (11.12.5) 새로운 타입 추가
	//-------------------------------------------------------------------------------------//

	//! 탭 버튼
	{
		std::string strTapControlFormat("KEY_SETTING_TAP_%1%");
		UINT        nTapCount = 0;

		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		{
			std::string strTapControl;
			
			strTapControl = sc::string::format( strTapControlFormat, nTapCount++ );
			STAPBUTTON* pTapButton = new STAPBUTTON(m_pEngineDevice);
			pTapButton->CreateSub ( this, strTapControl.c_str(), UI_FLAG_DEFAULT, KEYSETTING_TAB_TYPE_A );
			pTapButton->CreateSubControl( ID2GAMEWORD("KEYSETTINGWINDOW_TAP_TYPE", 0),
				"KEY_SETTING_TAP_BASE","KEY_SETTING_TAP_TEXINFO_ACTIVE",
				"KEY_SETTING_TAP_BASE","KEY_SETTING_TAP_TEXINFO_INACTIVE",
				"KEY_SETTING_TAP_BASE");

			RegisterControl ( pTapButton );
			m_pTapButton[0] = pTapButton;
			TAPSELECTOR::RegisterTapButton( pTapButton );

			if ( GLUseFeatures::GetInstance().IsUsingNewControlType() )
			{
				strTapControl = sc::string::format( strTapControlFormat, nTapCount++ );
				pTapButton = new STAPBUTTON(m_pEngineDevice);
				pTapButton->CreateSub ( this, strTapControl.c_str(), UI_FLAG_DEFAULT, KEYSETTING_TAB_TYPE_B );
				pTapButton->CreateSubControl( ID2GAMEWORD("KEYSETTINGWINDOW_TAP_TYPE", 1),
					"KEY_SETTING_TAP_BASE","KEY_SETTING_TAP_TEXINFO_ACTIVE",
					"KEY_SETTING_TAP_BASE","KEY_SETTING_TAP_TEXINFO_INACTIVE",
					"KEY_SETTING_TAP_BASE");
				RegisterControl ( pTapButton );
				m_pTapButton[1] = pTapButton;
				TAPSELECTOR::RegisterTapButton( pTapButton );
			}
		}

		TAPSELECTOR::TapSelect( TAP_DEFAULT );
	}
}

void CKeySettingWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pBeginEdit && !m_pBeginEdit->IsBegin() )
	{
		EDIT_END(m_pBeginEdit->GetWndID() - KEY_SETTING_QUICKEDIT0);
	}
}

void CKeySettingWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	//! 탭 셀렉터
	TAPSELECTOR::EMTAPSELECTORRESULT emResult = TAPSELECTOR::EMTAPSELECTORRESULT_NONE;
	TAPSELECTOR::TapSelectorUIMessage( ControlID, dwMsg, emResult );

	if ( emResult == TAPSELECTOR::EMTAPSELECTORRESULT_SELECT )
	{
		if ( m_dwCurrentSettingType != ControlID )
		{
			switch ( ControlID )
			{
			case TAP_TYPE_A:
				{
					m_emCurrentSettingType = EM_CONTROLTYPE_A;
                    m_dwCurrentSettingType = TAP_TYPE_A;
					GetDefaultKey();
				}
				break;

			case TAP_TYPE_B:
				{
					if ( GLUseFeatures::GetInstance().IsUsingNewControlType() )
					{
						m_emCurrentSettingType = EM_CONTROLTYPE_B;
                        m_dwCurrentSettingType = TAP_TYPE_B;
						GetDefaultKey();
					}
				}
				break;
			}
		}
	}

	// EditBox
	std::string strTemp;
	if( ControlID >= KEY_SETTING_QUICKEDIT0 && ControlID <= KEY_SETTING_SHOTCUTEDIT16)
	{
		if ( CHECK_MOUSE_IN( dwMsg ) )
		{
			if ( UIMSG_LB_UP & dwMsg )
			{
				EDIT_BEGIN(ControlID - KEY_SETTING_QUICKEDIT0);				
			}
		}
		else
		{
			if ( UIMSG_LB_DOWN & dwMsg || UIMSG_LB_UP & dwMsg )
				EDIT_END(ControlID - KEY_SETTING_QUICKEDIT0);
		}
	}
	else
	{
		switch ( ControlID )
		{
		case ET_CONTROL_TITLE:
		case ET_CONTROL_TITLE_F:
			{
				if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN( dwMsg ) )
				{
					m_pInterface->SetDefaultPosInterface( KEY_SETTING_WINDOW );
				}
			}
			break;

		case KEYSETTING_DEFAULTSETTING:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					GetDefaultKey();
				}
				break;
			}
		case KEYSETTING_APPLY:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					if ( IsEmpty() )
					{
						m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_KEY_SETTING_ERROR_EMPTY"), UI::MODAL_ERROR, UI::OK, NO_ID );
					}
					else
					{
						CString strResult;
						strResult = GetDuplicationShortcutString();

						if( strResult.IsEmpty())
						{
							if ( m_emSaveSettingType != m_emCurrentSettingType )
							{
								GLInputManager::GetInstance().InitControlType( m_emCurrentSettingType );
								m_emSaveSettingType = m_emCurrentSettingType;
							}

							SetShotCutKey();
						}
						else
						{
							strTemp = sc::string::format("%s%s", strResult, ID2GAMEINTEXT("MODAL_KEY_SETTING_ERROR"));
							m_pInterface->DoModal( strTemp, UI::MODAL_ERROR, UI::OK, NO_ID );
						}
					}
				}
				break;
			}
		case KEYSETTING_OK:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					if ( IsEmpty() )
					{
						m_pInterface->DoModal( ID2GAMEINTEXT("MODAL_KEY_SETTING_ERROR_EMPTY"), UI::MODAL_ERROR, UI::OK, NO_ID );
					}
					else
					{
						CString strResult;
						strResult = GetDuplicationShortcutString();
						if(strResult.IsEmpty())
						{
							if ( m_emSaveSettingType != m_emCurrentSettingType )
							{
								GLInputManager::GetInstance().InitControlType( m_emCurrentSettingType );
								m_emSaveSettingType = m_emCurrentSettingType;
							}

							SetShotCutKey();
							m_pInterface->UiHideGroup( GetWndID() );
						}
						else
						{
							strTemp = sc::string::format("%s%s", strResult, ID2GAMEINTEXT("MODAL_KEY_SETTING_ERROR"));
							m_pInterface->DoModal( strTemp, UI::MODAL_ERROR, UI::OK, NO_ID );
						}
					}
				}
				break;
			}
		case ET_CONTROL_BUTTON:
			{
				if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					m_pInterface->UiHideGroup( GetWndID() );
				}
				break;
			}
		}
	}
	
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CKeySettingWindow::EDIT_BEGIN(int num)
{
	m_BeforeShotCut = m_pEditBox[num]->GetEditString();
	m_pBeginEdit	= m_pEditBox[num];

	if( m_pEditBox[num])
	{
		m_pEditBox[num]->EndEdit();
		m_pEditBox[num]->ClearEdit();

		m_pEditBox[num]->SetEditString("");
		m_pEditBox[num]->BeginEdit();	
	}
}

void CKeySettingWindow::EDIT_END(int num)
{
	if( m_pBeginEdit && m_pBeginEdit == m_pEditBox[num] )
	{
		CString strTemp = m_pEditBox[num]->GetEditString();
        std::string strTemp2 = stringformat( "%1%", m_pEditBox[num]->GetEditString() );

		if( strTemp == "" ||  strTemp == " " )
		{
			m_pEditBox[num]->SetEditString(m_BeforeShotCut);		
		}
        else if ( !CheckPossibleKeyString( strTemp2 ) )
        {
            std::string strEdit = strTemp.GetString();
            strEdit = stringformat( ID2GAMEINTEXT("MODAL_KEY_SETTING_DISABLE"), strTemp );
            m_pInterface->DoModal( strEdit, UI::MODAL_ERROR, UI::OK, NO_ID );

            m_pEditBox[num]->SetEditString(m_BeforeShotCut);
        }
		
		m_pEditBox[num]->EndEdit();
		m_pBeginEdit = NULL;
	}
}

bool CKeySettingWindow::CheckPossibleKeyString( const std::string& strKeyString )
{
    const std::string& strVehicle = GLInputManager::GetInstance().GetShortcutsToString( EMSHORTCUTS_VEHICLEACTIVE );
    const std::string& strQBox = GLInputManager::GetInstance().GetShortcutsToString( EMSHORTCUTS_QBOX );

    if ( !strKeyString.compare( strVehicle ) )
        return false;
    else if ( !strKeyString.compare( strQBox ) )
        return false;

    return true;
}

void CKeySettingWindow::GetShotCutKey()
{	
	int i = 0;
	int nIndex = 0;
	// 퀵슬롯 단축키값 얻어오기
	for( i = 0; i< QUICK_SLOT_NUM; ++i)
	{
		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetShortcutsToString( m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT0+i] );

		m_QuickSlot[i] = strEditString.c_str();
		m_pEditBox[i]->SetEditString( m_QuickSlot[i] );	
	}

	// 스킬슬롯 단축키값 얻어오기
	for( i = 0; i< QUICK_SKILL_NUM; ++i)
	{
		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetShortcutsToString( m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT0+i] );

		m_SkillSlot[i] = strEditString.c_str();
		m_pEditBox[i+QUICK_SLOT_NUM]->SetEditString( m_SkillSlot[i] );	
	}

	// 메뉴 단축키값 얻어오기
	for(i = 0; i< MENU_SHOTCUT_NUM; ++i)
	{
		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetShortcutsToString( m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT0+i] );

		m_MenuShotcut[i] = strEditString.c_str();
		m_pEditBox[i+QUICK_SLOT_NUM+QUICK_SKILL_NUM]->SetEditString( m_MenuShotcut[i] );	
	}

    // 변경할 수 없는 공격 키
    switch( m_dwCurrentSettingType )
    {
    case TAP_TYPE_A:
        m_pAtteckKey->SetOneLineText( "Mouse(L)" );
        break;
    case TAP_TYPE_B:
        m_pAtteckKey->SetOneLineText( "A" );
        break;
    }
}

void CKeySettingWindow::GetDefaultKey()
{
	int i = 0;
	int nIndex = 0;
	for( i = 0; i< QUICK_SLOT_NUM; ++i)
	{
		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetDefaultShortcutsToString( m_emCurrentSettingType, m_mGUIDToShorcuts[KEY_SETTING_QUICKEDIT0+i] );

		m_QuickSlot[i] = strEditString.c_str();
		m_pEditBox[i]->SetEditString( strEditString.c_str() );	
	}

	// 스킬슬롯 단축키값 얻어오기
	for( i = 0; i< QUICK_SKILL_NUM; ++i)
	{
		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetDefaultShortcutsToString( m_emCurrentSettingType, m_mGUIDToShorcuts[KEY_SETTING_SKILLEDIT0+i] );

		m_SkillSlot[i] = strEditString.c_str();
		m_pEditBox[i+QUICK_SLOT_NUM]->SetEditString( m_SkillSlot[i] );
	}

	// 메뉴 단축키값 얻어오기
	for(i = 0; i< MENU_SHOTCUT_NUM; ++i)
	{
		std::string strEditString;
		strEditString = GLInputManager::GetInstance().GetDefaultShortcutsToString( m_emCurrentSettingType, m_mGUIDToShorcuts[KEY_SETTING_SHOTCUTEDIT0+i] );

		m_MenuShotcut[i] = strEditString.c_str();
		m_pEditBox[i+QUICK_SLOT_NUM+QUICK_SKILL_NUM]->SetEditString( m_MenuShotcut[i] );	
	}


    //////////////////////////////////////////////////////////////////////////
    // 변경 할 수 없는 키
    //     m_emCurrentSettingType = EM_CONTROLTYPE_A;
    //     m_dwCurrentSettingType = TAP_TYPE_A;

    //////////////////////////////////////////////////////////////////////////
    // 변경할 수 없는 공격 키
    switch( m_dwCurrentSettingType )
    {
    case TAP_TYPE_A:
        m_pAtteckKey->SetOneLineText( "Mouse(L)" );
        break;
    case TAP_TYPE_B:
        m_pAtteckKey->SetOneLineText( "A" );
        break;
    }
    

}

// void CKeySettingWindow::TapEnable  ( TAPSELECTOR::BUTTON* pButton )
// {
// 	if ( pButton )
// 	{
// 		pButton->SetEnable( true );
// 	}
// }

// void CKeySettingWindow::TapDisable ( TAPSELECTOR::BUTTON* pButton )
// {
// 	if ( pButton )
// 	{
// 		pButton->SetEnable( false );
// 	}
// }

//----------------------------------------------------------------------------------------------------//
// 
// void CKeySettingWindow::STAPBUTTON::CreateSubControl ( const std::string& strLabel )
// {
// 	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
// 
// 	ACTIVE = new CBasicLineBoxSmart(m_pEngineDevice);
// 	ACTIVE->CreateSub( this, "KEY_SETTING_TAP_BASE" );
// 	ACTIVE->CreateSubControl( "KEY_SETTING_TAP_TEXINFO_ACTIVE" );
// 	RegisterControl( ACTIVE );
// 
// 	INACTIVE = new CBasicLineBoxSmart(m_pEngineDevice);
// 	INACTIVE->CreateSub( this, "KEY_SETTING_TAP_BASE" );
// 	INACTIVE->CreateSubControl( "KEY_SETTING_TAP_TEXINFO_INACTIVE" );
// 	RegisterControl( INACTIVE );
// 
// 	TEXTBOX = new CBasicTextBox(m_pEngineDevice);
// 	TEXTBOX->CreateSub( this, "KEY_SETTING_TAP_BASE" );
// 	TEXTBOX->SetFont( pFont );
// 	TEXTBOX->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
// 	TEXTBOX->SetText( strLabel.c_str() );
// 	TEXTBOX->SetUseTextColor( 0, TRUE );
// 	RegisterControl( TEXTBOX );
// }
// 
// void CKeySettingWindow::STAPBUTTON::SetEnable ( const bool bEnable )
// {
// 	ENABLE = bEnable;
// 
// 	if ( bEnable )
// 	{
// 		ACTIVE->SetVisibleSingle( TRUE );
// 		INACTIVE->SetVisibleSingle( FALSE );
// 		TEXTBOX->SetTextColor(0, NS_UITEXTCOLOR::BLACK);
// 	}
// 	else
// 	{
// 		ACTIVE->SetVisibleSingle( FALSE );
// 		INACTIVE->SetVisibleSingle( TRUE );
// 		TEXTBOX->SetTextColor(0, NS_UITEXTCOLOR::WHITE);
// 	}
// }

//----------------------------------------------------------------------------------------------------//
MyKeySettingWindow::MyKeySettingWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CKeySettingWindow( pInterface, pEngineDevice )
{
}

void MyKeySettingWindow::CreateUIWindowAndRegisterOwnership()
{
    CKeySettingWindow::Create( KEY_SETTING_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CKeySettingWindow::CreateBaseWindowLightGray( "KEY_SETTING_WINDOW", (char*)ID2GAMEWORD("KEYSETTING_WINDOW_NAME_STATIC") );
    CKeySettingWindow::CreateSubControl();
    CKeySettingWindow::SetControlNameEx( "게임키설정창" );
    CKeySettingWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
    CKeySettingWindow::m_pInterface->UiRegisterControl( this, true);
    CKeySettingWindow::m_pInterface->UiShowGroupFocus( KEY_SETTING_WINDOW );
}

UI::String MyKeySettingWindow::GetdwKeyToString( int dwKey )
{
    return UI::ToString( CKeySettingWindow::GetdwKeyToString( dwKey ) );
}

void MyKeySettingWindow::GetShotCutKey()
{
    CKeySettingWindow::GetShotCutKey();
}

//----------------------------------------------------------------------------------------------------//