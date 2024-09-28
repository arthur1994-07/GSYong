#include "StdAfx.h"
#include "ReferCharWindow.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogicClient/ReferChar/ReferChar.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/RANPARAM.h"

#include "../Char/Additional/RnCharacterAdditionalWindow.h"

#include "../Util/RnButton.h"

#include "../Product/ProductProgress.h"

#include "ReferCharWearTab.h"
#include "ReferCharSkillTab.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
namespace
{
    const char* ID2GAMEWORD_TITLE;
    const char* ID2GAMEWORD_WEAR_INFO;
    const char* ID2GAMEWORD_SKILL_INFO;
}

CReferCharWindow::CReferCharWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pInterface( pInterface )
    , m_pGaeaClient( pGaeaClient )
    , m_pCurrentTap( NULL )
    , m_pCharacterWearTap( NULL )
    , m_pCharacterSkillTap( NULL )
{
    ID2GAMEWORD_TITLE = ID2GAMEWORD("REFER_WINDOW", 0 );
    ID2GAMEWORD_WEAR_INFO = ID2GAMEWORD("REFER_WINDOW", 1 );
    ID2GAMEWORD_SKILL_INFO = ID2GAMEWORD("REFER_WINDOW", 2 );
}

void CReferCharWindow::CreateSubControl()
{
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    CreateBaseWindowLightGray( "ANOTHER_CHAR_WINDOW",  ID2GAMEWORD_TITLE );
    CreateLineBox( "ANOTHER_CHAR_REGION_WINDOW", "ANOTHER_CHAR_REGION_WINDOW_TEXTUREINFO" );

	{
		//! 추가 능력창
		m_pCharacterAdditionalAbility = new RnCharacterAdditionalWindow(m_pGaeaClient,m_pInterface,m_pEngineDevice);
		m_pCharacterAdditionalAbility->CreateSub( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pCharacterAdditionalAbility->CreateBaseWindowLightGray( "CHARACTOR_ADDITIONAL_ABILITY_WINDOW_BASE", (char*)ID2GAMEINTEXT("CHARACTER_WINDOW_INFO_TAP", 3) );
		m_pCharacterAdditionalAbility->CreateSubControl();
		m_pCharacterAdditionalAbility->SetIsRefer( true );
		m_pCharacterAdditionalAbility->SetVisibleSingle(FALSE);
		RegisterControl( m_pCharacterAdditionalAbility );

		D3DXVECTOR2 vMovableSize;
		vMovableSize.x = m_pCharacterAdditionalAbility->GetLocalPos().right;
		vMovableSize.y = GetLocalPos().sizeY;

		m_rcBasicPos = GetLocalPos();
		m_rcChangePos = m_rcBasicPos;
		m_rcChangePos.sizeX = vMovableSize.x;
		m_rcChangePos.right = m_rcChangePos.left + m_rcChangePos.sizeX;
	}

    {   // 복장정보 영역
        m_pCharacterWearTap = new CReferCharWearTab( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        m_pCharacterWearTap->CreateSub(this,"ANOTHER_CHAR_REGION_WINDOW",UI_FLAG_DEFAULT, ID_CHAR_WEAR_REGION);
        m_pCharacterWearTap->CreateSubControl();
        m_pCharacterWearTap->SetVisibleSingle( FALSE );
        RegisterControl( m_pCharacterWearTap );
    } 

    {   // 스킬정보 영역
        m_pCharacterSkillTap = new CReferCharSkillTab(m_pGaeaClient, m_pInterface, m_pEngineDevice );
        m_pCharacterSkillTap->CreateSub(this,"ANOTHER_CHAR_REGION_WINDOW",UI_FLAG_DEFAULT, ID_CHAR_SKILL_REGION);
        m_pCharacterSkillTap->CreateSubControl();
        m_pCharacterSkillTap->SetVisibleSingle( FALSE );
        RegisterControl( m_pCharacterSkillTap );
    }
    
    {   // 복장정보 버튼
        STAPBUTTON* pCharacterWearTap = new STAPBUTTON(m_pEngineDevice);
        pCharacterWearTap->CreateSub( this, "ANOTHER_CHARACTER_WEAR_TAB", UI_FLAG_DEFAULT, ID_CHAR_WEAR_TAB );
        pCharacterWearTap->CreateSubControl( ID2GAMEWORD_WEAR_INFO,
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
            "CHARACTER_WINDOW_TAP_BASE");
        RegisterControl ( pCharacterWearTap );
        RegisterTapButton( pCharacterWearTap );
    }

    {   // 스킬정보 버튼
        STAPBUTTON* pCharacterSkillTap = new STAPBUTTON(m_pEngineDevice);
        pCharacterSkillTap->CreateSub ( this, "ANOTHER_CHARACTER_SKILL_TAB", UI_FLAG_DEFAULT, ID_CHAR_SKILL_TAB );
        pCharacterSkillTap->CreateSubControl( ID2GAMEWORD_SKILL_INFO ,
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
            "CHARACTER_WINDOW_TAP_BASE");
        RegisterControl ( pCharacterSkillTap );
        RegisterTapButton( pCharacterSkillTap );
    }

    {
        // 새로고침 버튼
        RnButton* pRnButton = NULL;
        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "ANOTHER_CHARACTER_REFRESH_BUTTON_NORMAL";
        arg.mouseOverTextureKey = "ANOTHER_CHARACTER_REFRESH_BUTTON_OVER";
        arg.mouseClickTextureKey = "ANOTHER_CHARACTER_REFRESH_BUTTON_DWON";
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "ANOTHER_CHARACTER_REFRESH_BUTTON", UI_FLAG_DEFAULT, ID_CHAR_REFRESH );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
    }

    { 
        // 콤보박스 
        // 배경
        CreateLineBox( "ANOTHER_CHARACTER_COMBOBOX_DESIGN", "ANOTHER_CHARACTER_COMBOBOX_DESIGN_TEXINFO" );

        // 텍스트 영역
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "ANOTHER_CHARACTER_COMBOBOX_DESIGN_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetSensitive( true );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
        RegisterControl( pTextBox );
        m_pComboBoxText = pTextBox;

        //버튼
        RnButton* pRnButton = NULL;
        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_NORMAL";
        arg.mouseOverTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_OVER";
        arg.mouseClickTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_DOWN";
        //////////////////////////////////////////////////////////////////////////
        
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "ANOTHER_CHARACTER_COMBOBOX_BUTTON", UI_FLAG_DEFAULT, ID_CHAR_COMBOBOX );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);

        // 롤오버 페이지
        CBasicComboBoxRollOver* pComboBoxRollOver;
        pComboBoxRollOver =  new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_CHAR_COMBOBOX_ROLLOVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "ANOTHER_CHARACTER_COMBOBOX_ROLLOVER_DESIGN");
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pComboBoxRollOver = pComboBoxRollOver;
    }

    EventSelectedTap( ID_CHAR_WEAR_TAB );
}

void CReferCharWindow::SetFocusControl()
{
	CUIGroup::SetFocusControl();
	m_pCharacterAdditionalAbility->SetFocusControl();
}

void CReferCharWindow::ResetFocusControl()
{
	CUIGroup::ResetFocusControl();
	m_pCharacterAdditionalAbility->ResetFocusControl();
}

void CReferCharWindow::DeleteUIMeshData()
{
    if( m_pCharacterWearTap )
        m_pCharacterWearTap->DeleteUIMeshData();
}

void CReferCharWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
    TapSelectorUIMessage( ControlID, dwMsg, emResult );
    CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch( ControlID )
    {
    case ID_CHAR_COMBOBOX:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
            if( m_pComboBoxRollOver->IsVisible() == TRUE )
                m_pComboBoxRollOver->SetVisibleSingle( FALSE );
            else
                m_pComboBoxRollOver->SetVisibleSingle( TRUE );
        }
        break;

	case ID_CHAR_REFRESH:
        if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
        {
			ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
			if( pReferChar )
				pReferChar->Refresh();
        }
        break;

	case ID_CHAR_COMBOBOX_ROLLOVER:
        if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) &&  m_pComboBoxRollOver->IsVisible() == TRUE  )
        {				
            int nIndex = m_pComboBoxRollOver->GetSelectIndex();
            if( nIndex < 0 )
                return;

			m_pComboBoxRollOver->SetVisibleSingle( FALSE );

			CString strName = m_pComboBoxRollOver->GetSelectText( nIndex );
			strName.Trim();

			ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
			pReferChar->Request( m_pGaeaClient->GetConnServerGroup(), strName.GetString() );
	
            //ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();

            //if( pReferChar )
            //    pReferChar->Select( m_pGaeaClient->GetConnServerGroup(), strName.GetString() );
        }
        break;
    }
}

void CReferCharWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );
	SetVisibleReferCharacterAdditionalWindow( RANPARAM::bCharInfoAddition );

	if( bVisible )
	{
		Refresh();
	}
	else
	{
		m_pInterface->CloseSkillToolTipMultiAll();
	}
}

void CReferCharWindow::Refresh()
{
	if( !IsVisible() )
		return;

    if( m_pCharacterWearTap == NULL )
        return;

    if( m_pCharacterSkillTap == NULL )
        return;

	SetSelectedTargetName();
    SetComboboxItem();

	m_pCharacterWearTap->Refresh();
	m_pCharacterSkillTap->Refresh();

	m_pCharacterAdditionalAbility->RefreshReferChar();
}

CUIGroup* CReferCharWindow::GetTapWindow( UIGUID controlID )
{
    switch ( controlID )
    {
    case ID_CHAR_WEAR_TAB:
        return m_pCharacterWearTap;
    case ID_CHAR_SKILL_TAB:
        return m_pCharacterSkillTap;
    }

    GASSERT(false);
    return NULL;
}

void CReferCharWindow::EventSelectedTap( UIGUID controlID )
{
    if( controlID != ID_CHAR_WEAR_TAB &&  controlID != ID_CHAR_SKILL_TAB)
        return;

	if( m_pCurrentTap )
        m_pCurrentTap->SetVisibleSingle(FALSE);

    m_pCurrentTap = GetTapWindow(controlID);
    m_pCurrentTap->SetVisibleSingle(TRUE);
}

void CReferCharWindow::SetSelectedTargetName()
{
    if( m_pComboBoxText == NULL )
        return;

	m_pComboBoxText->ClearText();

	ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
    if( !pReferChar )
        return;

	GLCHARLOGIC* pTarget = pReferChar->GetCompleteData();
	if( !pTarget )
		return;

    CString strText;
    strText.Format( "%s%s", "     ", pTarget->m_szName );
    m_pComboBoxText->SetText( strText, NS_UITEXTCOLOR::WHITE );
}

void CReferCharWindow::SetComboboxItem()
{
    if( m_pComboBoxRollOver == NULL )
        return;

    ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
    if( !pReferChar )
        return;

    ReferChar::NameList vecList;
    pReferChar->GetNameList( m_pGaeaClient->GetConnServerGroup(), vecList );

    m_pComboBoxRollOver->ClearText();

    if( !vecList.empty() )
    {
        BOOST_FOREACH( const std::string& strName, vecList )
        {
            CString strText;
            strText.Format( "%s%s", " ", strName.c_str() );
            m_pComboBoxRollOver->AddText( strText );
        }
    }
}

std::string CReferCharWindow::GetSelectTextName()
{
	CString strName ="";

	int nIndex = m_pComboBoxRollOver->GetSelectIndex();
	if( nIndex < 0 )
		return strName.GetString();

	strName = m_pComboBoxRollOver->GetSelectText( nIndex );
	strName.Trim();

	return strName.GetString();

}

void CReferCharWindow::SetVisibleReferCharacterAdditionalWindow( bool bVisible )
{
	// 사이즈 변경 후 위치 새로고침
	if( bVisible )
	{
		ResizeWindow( D3DXVECTOR2( m_rcChangePos.sizeX, m_rcChangePos.sizeY ) );
		SetGlobalPos( GetGlobalPos() );
		m_pCharacterAdditionalAbility->SetVisibleSingle( true );
	}
	else if( !bVisible )
	{
		ResizeWindow( D3DXVECTOR2( m_rcBasicPos.sizeX, m_rcBasicPos.sizeY )  );
		SetGlobalPos( GetGlobalPos() );
		m_pCharacterAdditionalAbility->SetVisibleSingle( false );
	}
}