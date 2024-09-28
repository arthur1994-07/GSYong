#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../InnerInterface.h"
#include "./WbClubBasicInfoTab.h"
#include "./WbClubManagementTab.h"
#include "./WbClubStorageTab.h"
#include "./WbClubWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2GAMEWORD_CLUB_BASIC_INFO = "기본 정보";
    const char* ID2GAMEWORD_CLUB_ACTIVITY = "클럽 활동";
    const char* ID2GAMEWORD_CLUB_STORAGE = "클럽 창고";
    const char* ID2GAMEWORD_CLUB_MANAGEMENT = "클럽 관리";
}


CWbClubWindow::CWbClubWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pCurrentTap(NULL)
, m_pClubBasicInfoTab(NULL)
, m_pClubManagementTab(NULL)
, m_pClubStorageTab(NULL)
, m_nPage(CLUB_BASICINFO_TAB)
{
    ID2GAMEWORD_CLUB_BASIC_INFO = ID2GAMEWORD("WB_CLUB_WINDOW_TAB", 0);
    ID2GAMEWORD_CLUB_ACTIVITY = ID2GAMEWORD("WB_CLUB_WINDOW_TAB", 1);
    ID2GAMEWORD_CLUB_STORAGE = ID2GAMEWORD("WB_CLUB_WINDOW_TAB", 2);
    ID2GAMEWORD_CLUB_MANAGEMENT = ID2GAMEWORD("WB_CLUB_WINDOW_TAB", 3);
}

CWbClubWindow::~CWbClubWindow()
{
}


void CWbClubWindow::CreateSubControl()
{
    {
        CWbClubBasicInfoTab* pClubInfoTab = new CWbClubBasicInfoTab( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubInfoTab->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_TAB", UI_FLAG_DEFAULT, CLUB_BASICINFO_TAB_WINDOW );
        pClubInfoTab->CreateSubControl();
        RegisterControl( pClubInfoTab );
        pClubInfoTab->SetVisibleSingle( FALSE );
        m_pClubBasicInfoTab = pClubInfoTab;
    }

    {
        CWbClubManagementTab* pClubManagementTab = new CWbClubManagementTab( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubManagementTab->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB", UI_FLAG_DEFAULT, CLUB_MANAGEMENT_TAB_WINDOW );
        pClubManagementTab->CreateSubControl();
        RegisterControl( pClubManagementTab );
        pClubManagementTab->SetVisibleSingle( FALSE );
        m_pClubManagementTab = pClubManagementTab;
    }

    {
        CWbClubStorageTab* pClubStorageTab = new CWbClubStorageTab( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubStorageTab->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB", UI_FLAG_DEFAULT, CLUB_STORAGE_TAB_WINDOW );
        pClubStorageTab->CreateSubControl();
        RegisterControl( pClubStorageTab );
        pClubStorageTab->SetVisibleSingle( FALSE );
        m_pClubStorageTab = pClubStorageTab;
    }

    {
        //////////////////////////////////////////////////////////////////////////
        STAPBUTTON* pClubWindowTab = new STAPBUTTON(m_pEngineDevice);
        pClubWindowTab->CreateSub( this, "WB_CLUB_WINDOW_TAB_BASIC_INFO", UI_FLAG_DEFAULT, CLUB_BASICINFO_TAB );
        pClubWindowTab->CreateSubControl( ID2GAMEWORD_CLUB_BASIC_INFO,
            "WB_CLUB_WINDOW_TAB_BASE","WB_CLUB_WINDOW_TAB_ACTIVE",
            "WB_CLUB_WINDOW_TAB_BASE","WB_CLUB_WINDOW_TAB_INACTIVE",
            "WB_CLUB_WINDOW_TAB_BASE");

        RegisterControl( pClubWindowTab );
        RegisterTapButton( pClubWindowTab );
        //////////////////////////////////////////////////////////////////////////

        //미구현 기능 숨김
//         pClubWindowTab = new STAPBUTTON(m_pEngineDevice);
//         pClubWindowTab->CreateSub( this, "WB_CLUB_WINDOW_TAB_ACTIVITY", UI_FLAG_DEFAULT, CLUB_ACTIVITY_TAB );
//         pClubWindowTab->CreateSubControl( ID2GAMEWORD_CLUB_ACTIVITY,
//             "WB_CLUB_WINDOW_TAB_BASE","WB_CLUB_WINDOW_TAB_ACTIVE",
//             "WB_CLUB_WINDOW_TAB_BASE","WB_CLUB_WINDOW_TAB_INACTIVE",
//             "WB_CLUB_WINDOW_TAB_BASE");
// 
//         RegisterControl( pClubWindowTab );
//         RegisterTapButton( pClubWindowTab );
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        pClubWindowTab = new STAPBUTTON(m_pEngineDevice);
        //pClubWindowTab->CreateSub( this, "WB_CLUB_WINDOW_TAB_STORAGE", UI_FLAG_DEFAULT, CLUB_STORAGE_TAB );
        pClubWindowTab->CreateSub( this, "WB_CLUB_WINDOW_TAB_ACTIVITY", UI_FLAG_DEFAULT, CLUB_STORAGE_TAB );
        pClubWindowTab->CreateSubControl( ID2GAMEWORD_CLUB_STORAGE,
            "WB_CLUB_WINDOW_TAB_BASE","WB_CLUB_WINDOW_TAB_ACTIVE",
            "WB_CLUB_WINDOW_TAB_BASE","WB_CLUB_WINDOW_TAB_INACTIVE",
            "WB_CLUB_WINDOW_TAB_BASE");

        RegisterControl( pClubWindowTab );
        RegisterTapButton( pClubWindowTab );
        //////////////////////////////////////////////////////////////////////////
        pClubWindowTab = new STAPBUTTON(m_pEngineDevice);
        //pClubWindowTab->CreateSub( this, "WB_CLUB_WINDOW_TAB_MANAGEMENT", UI_FLAG_DEFAULT, CLUB_MANAGEMENT_TAB );
    pClubWindowTab->CreateSub( this, "WB_CLUB_WINDOW_TAB_STORAGE", UI_FLAG_DEFAULT, CLUB_MANAGEMENT_TAB );
        pClubWindowTab->CreateSubControl( ID2GAMEWORD_CLUB_MANAGEMENT,
            "WB_CLUB_WINDOW_TAB_BASE","WB_CLUB_WINDOW_TAB_ACTIVE",
            "WB_CLUB_WINDOW_TAB_BASE","WB_CLUB_WINDOW_TAB_INACTIVE",
            "WB_CLUB_WINDOW_TAB_BASE");

        RegisterControl( pClubWindowTab );
        RegisterTapButton( pClubWindowTab );
        //////////////////////////////////////////////////////////////////////////
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "NEED_REGIST_CLUB_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pNeedRegistClubBack = pLinBoxSmart;

        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "NEED_REGIST_CLUB", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        RegisterControl( pControl );
        m_pNeedRegistClubLogo = pControl;

        CD3DFontPar* pFont15Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 15, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "NEED_REGIST_CLUB_TEXT" );
        pTextBox->SetFont( pFont15Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        std::string strText = stringformat( "%1%", ID2GAMEINTEXT("WB_CLUBWINDOW_NO_REGISTED_CLUB") );
        pTextBox->SetText( strText.c_str(),
            NS_UITEXTCOLOR::WHITE );
        m_pNeedRegistText = pTextBox;
    }
}

void CWbClubWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
    TapSelectorUIMessage( ControlID, dwMsg, emResult );
    CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CWbClubWindow::SetVisibleSingle( BOOL isVisible )
{
    if ( isVisible == TRUE )
    {
        std::string Title = stringformat( ID2GAMEWORD("CLUB_WINDOW_NAME_STATIC"), m_pGaeaClient->GetMyClub()->Name() );
        SetTitleName( Title.c_str() );

        if ( m_pGaeaClient->GetMyClub()->IsValid() )
        {
            m_pNeedRegistClubBack->SetVisibleSingle( FALSE );
            m_pNeedRegistClubLogo->SetVisibleSingle( FALSE );
            m_pNeedRegistText->SetVisibleSingle( FALSE );
        }
        else
        {
            m_pNeedRegistClubBack->SetVisibleSingle( TRUE );
            m_pNeedRegistClubLogo->SetVisibleSingle( TRUE );
            m_pNeedRegistText->SetVisibleSingle( TRUE );
        }

        EventSelectedTap( m_nPage );
    }

    CUIWindowEx::SetVisibleSingle( isVisible );
}

void CWbClubWindow::EventSelectedTap( UIGUID ControlID )
{
    if( m_pCurrentTap )
        m_pCurrentTap->SetVisibleSingle(FALSE);

    m_pCurrentTap = GetTapWindow(ControlID);

    if ( m_pCurrentTap )
        m_pCurrentTap->SetVisibleSingle(TRUE);

    m_nPage = ControlID;
}

CUIGroup* CWbClubWindow::GetTapWindow( UIGUID ControlID )
{
    switch ( ControlID )
    {
    case CLUB_BASICINFO_TAB:
        return m_pClubBasicInfoTab;

    case CLUB_ACTIVITY_TAB:
        return NULL;

    case CLUB_STORAGE_TAB:
        return m_pClubStorageTab;

    case CLUB_MANAGEMENT_TAB:
        return m_pClubManagementTab;
    };

    GASSERT( 0 && "CUIGroup* CWbClubWindow::GetTapWindow( UIGUID controlID ) - Can't find ControlID." );
    return NULL;
}

void CWbClubWindow::LoadClubMemberList()
{
    switch ( m_nPage )
    {
    case CLUB_BASICINFO_TAB:
        {
            m_pClubBasicInfoTab->ShowClubMember();
        }
        break;
    };
}

void CWbClubWindow::RefreshState()
{
    GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
    DWORD dwCharID = m_pGaeaClient->GetCharacter()->m_CharDbNum;

	if ( !pMyClub || (pMyClub->IsValid() == false) )
	{
		m_nPage = CLUB_BASICINFO_TAB;
		if ( IsVisible() )
		{
			m_pNeedRegistClubBack->SetVisibleSingle( TRUE );
			m_pNeedRegistClubLogo->SetVisibleSingle( TRUE );
			m_pNeedRegistText->SetVisibleSingle( TRUE );
		}
	}
    //	길드원이 아닌 경우
    if ( !pMyClub->IsMember( dwCharID ) )
        return;

    EventSelectedTap( m_nPage );
}

const DWORD CWbClubWindow::GetClubMemberID()
{
    return m_pClubBasicInfoTab->GetClubMemberID();
}

const DWORD CWbClubWindow::GetClubAllianceID()
{
	return m_pClubBasicInfoTab->GetClubAllianceID();
}

void CWbClubWindow::ShowGradePopup()
{
    m_pClubBasicInfoTab->ShowGradePopup();
}

void CWbClubWindow::ShowNotice()
{
    switch ( m_nPage )
    {
    case CLUB_BASICINFO_TAB:
        {
            m_pClubBasicInfoTab->ShowNotice();
        }
        break;
    };
}

void CWbClubWindow::ShowClubLog()
{
    switch ( m_nPage )
    {
    case CLUB_BASICINFO_TAB:
        {
            m_pClubBasicInfoTab->ShowClubLog();
        }
        break;

    case CLUB_STORAGE_TAB:
        {
            m_pClubStorageTab->ClubStorageUpdateLog();
        }
        break;
    };
}

void CWbClubWindow::LoadClubAllianceList()
{
    switch ( m_nPage )
    {
    case CLUB_BASICINFO_TAB:
        {
            m_pClubBasicInfoTab->LoadClubAllianceList();
        }
        break;
    };
}

void CWbClubWindow::LoadClubBattleList()
{
    switch ( m_nPage )
    {
    case CLUB_BASICINFO_TAB:
        {
            m_pClubBasicInfoTab->LoadClubBattleList();
        }
        break;
    };
}

void CWbClubWindow::ClubStorageUpdateMoney()
{
    switch ( m_nPage )
    {
    case CLUB_STORAGE_TAB:
        {
            m_pClubStorageTab->ClubStorageUpdateMoney();
        }
        break;
    };
}

void CWbClubWindow::ClubUpdateNewbieNotice()
{
    switch ( m_nPage )
    {
    case CLUB_MANAGEMENT_TAB:
        {
            m_pClubManagementTab->ClubUpdateNewbieNotice();
        }
        break;
    };
}

void CWbClubWindow::RefreshGuidanceMap()
{
    if ( m_pClubBasicInfoTab )
        m_pClubBasicInfoTab->RefreshGuidanceMap();
}

MyWbClubWindow::MyWbClubWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CWbClubWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyWbClubWindow::CreateUIWindowAndRegisterOwnership()
{
    Create( CLUB_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CreateBaseWindowBlack( "WB_CLUB_WINDOW", "Club" );
    std::string Title = stringformat( ID2GAMEWORD("CLUB_WINDOW_NAME_STATIC"), m_pGaeaClient->GetMyClub()->Name() );
    SetTitleName( Title.c_str() );
    CreateSubControl();
    SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );		
    m_pInterface->UiRegisterControl( this, true );
    m_pInterface->UiShowGroupFocus( GetWndID() );
}

void MyWbClubWindow::RefreshClubList()
{
    CWbClubWindow::LoadClubMemberList();
}

void MyWbClubWindow::RefreshGuidanceMap()
{
    CWbClubWindow::RefreshGuidanceMap();
}

void MyWbClubWindow::RefreshState()
{
    CWbClubWindow::RefreshState();
}

const DWORD MyWbClubWindow::GetClubMemberID()
{
    return CWbClubWindow::GetClubMemberID();
}

void MyWbClubWindow::ShowGradePopup()
{
    CWbClubWindow::ShowGradePopup();
}

void MyWbClubWindow::ShowClubLog()
{
    CWbClubWindow::ShowClubLog();
}

void MyWbClubWindow::SetAnnouncement()
{
    CWbClubWindow::ShowNotice();
}

const DWORD MyWbClubWindow::GetUnionMemberID()
{
	return CWbClubWindow::GetClubAllianceID();
}

void MyWbClubWindow::LoadClubAllianceList()
{
    CWbClubWindow::LoadClubAllianceList();
}

void MyWbClubWindow::LoadClubBattleList()
{
    CWbClubWindow::LoadClubBattleList();
}

void MyWbClubWindow::ClubStorageUpdateMoney()
{
    CWbClubWindow::ClubStorageUpdateMoney();
}

void MyWbClubWindow::ClubUpdateNewbieNotice()
{
    CWbClubWindow::ClubUpdateNewbieNotice();
}