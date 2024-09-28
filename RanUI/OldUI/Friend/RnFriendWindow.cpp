#include "StdAfx.h"
#include "RnFriendWindow.h"

#include "RnFriendTab.h"
#include "RnBlockTab.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogic/Friend/FriendDefine.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Friend/GLFriendClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2GAMEWORD_TITLE;
    const char* ID2GAMEWORD_FRIEND;
    const char* ID2GAMEWORD_BLOCK;
}

RnFriendWindow::RnFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx( pInterface, pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
    , m_pCurrentPage( NULL )
    , m_pFriendPage( NULL )
    , m_pBlockPage( NULL )
{
    ID2GAMEWORD_TITLE   = ID2GAMEWORD("RNFRIEND_WINDOW", 0);
    ID2GAMEWORD_FRIEND  = ID2GAMEWORD("RNFRIEND_WINDOW", 1);
    ID2GAMEWORD_BLOCK   = ID2GAMEWORD("RNFRIEND_WINDOW", 2);
}

void RnFriendWindow::CreateSubControl()
{
    //CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
    CreateBaseWindowLightGray( "RNFRIEND_WINDOW",  ID2GAMEWORD_TITLE );
  
    CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(GetEngineDeviceMan());
    pLineBox->CreateSub( GetUIGroupThis(), "RNFRIEND_WINDOW_WHITE" );
    pLineBox->CreateSubControl( "ANOTHER_CHAR_REGION_WINDOW_TEXTUREINFO" );
    RegisterControl( pLineBox );

    {   // 친구 페이지
        RnFriendTab* pFriendPage = new RnFriendTab( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pFriendPage->CreateSub(this,"RNFRIEND_FRIEND_TAB",UI_FLAG_DEFAULT, ID_FRIEND_PAGE);
        pFriendPage->CreateSubControl();
        pFriendPage->SetVisibleSingle( FALSE );
        RegisterControl( pFriendPage );
        m_pFriendPage = pFriendPage;
    } 

    {   // 차단 페이지
        RnBlockTab* pBlockPage = new RnBlockTab(m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pBlockPage->CreateSub(this,"RNFRIEND_BLOCK_TAB",UI_FLAG_DEFAULT, ID_BLOCK_PAGE);
        pBlockPage->CreateSubControl();
        pBlockPage->SetVisibleSingle( FALSE );
        RegisterControl( pBlockPage );
        m_pBlockPage = pBlockPage;
    }

    {   // 친구 탭 버튼
        STAPBUTTON* pFriend = new STAPBUTTON(m_pEngineDevice);
        pFriend->CreateSub( this, "RNFRIEND_FRIEND_BUTTON", UI_FLAG_DEFAULT, ID_FRIEND_TAB );
        pFriend->CreateSubControl( ID2GAMEWORD_FRIEND,
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
            "CHARACTER_WINDOW_TAP_BASE");
        RegisterControl ( pFriend );
        RegisterTapButton( pFriend );
    }

    {   // 차단 탭 버튼
        STAPBUTTON* pBlock = new STAPBUTTON(m_pEngineDevice);
        pBlock->CreateSub ( this, "RNFRIEND_BLOCK_BUTTON", UI_FLAG_DEFAULT, ID_BLOCK_TAB );
        pBlock->CreateSubControl( ID2GAMEWORD_BLOCK ,
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
            "CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
            "CHARACTER_WINDOW_TAP_BASE");
        RegisterControl ( pBlock );
        RegisterTapButton( pBlock );
    }

    EventSelectedTap( ID_FRIEND_TAB );
}

void RnFriendWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
    TapSelectorUIMessage( ControlID, dwMsg, emResult );
    CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

}

void RnFriendWindow::SetVisibleSingle( BOOL bVisible )
{
    if( m_pCurrentPage != NULL )
        m_pCurrentPage->SetVisibleSingle( bVisible );

    CUIWindowEx::SetVisibleSingle( bVisible );
}

void RnFriendWindow::SetWhisperName( const std::string& strCharName )
{
    m_pFriendPage->SetWhisperName( strCharName.c_str() );
}

void RnFriendWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

CUIGroup* RnFriendWindow::GetTapWindow( UIGUID controlID )
{
    switch ( controlID )
    {
    case ID_FRIEND_TAB:
        return m_pFriendPage;
    case ID_BLOCK_TAB:
        return m_pBlockPage;
    }

    return NULL;
}

void RnFriendWindow::EventSelectedTap( UIGUID controlID )
{
    if( controlID != ID_FRIEND_TAB && controlID != ID_BLOCK_TAB )
        return;
       
    CUIGroup* pUI = GetTapWindow( controlID );
    
    if( pUI != NULL )
    {
        if( m_pCurrentPage == NULL )
            m_pCurrentPage = pUI;
        else
            m_pCurrentPage->SetVisibleSingle( FALSE );

        m_pCurrentPage = pUI;
        m_pCurrentPage->SetVisibleSingle( TRUE );
        m_pCurrentPage->Refresh();       
    }

}

void RnFriendWindow::Refresh()
{
    if( m_pCurrentPage == NULL )
        return;

    m_pCurrentPage->Refresh();
}


void MyRnFriendWindow::CreateUIWindowAndRegisterOwnership()
{
    RnFriendWindow::Create( FRIEND_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    RnFriendWindow::CreateSubControl();
    RnFriendWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    RnFriendWindow::m_pInterface->UiRegisterControl( this, true );
    RnFriendWindow::m_pInterface->UiShowGroupFocus( FRIEND_WINDOW );
}

void MyRnFriendWindow::AddFriendName_To_EditBox( const UI::String& strName )
{
    RnFriendWindow::SetWhisperName( strName.c_str() );
}

void MyRnFriendWindow::AddFriend( const UI::String& strName )
{
    m_pGaeaClient->GetFriendClient()->ReqFriendAdd( strName.c_str() );
    m_pCurrentPage->Refresh();
}

void MyRnFriendWindow::LoadFriendList()
{
    m_pCurrentPage->Refresh();
}

UI::String MyRnFriendWindow::GetFriendName() const
{
    return "";
}

UI::String MyRnFriendWindow::GetSelectBlockFriendName() const
{
    return "";
}

void MyRnFriendWindow::EndEditFriendList()
{
    int i = 0;
}

std::tr1::shared_ptr<SFRIEND> MyRnFriendWindow::GetFriendSMSInfo()
{
    return std::tr1::shared_ptr<SFRIEND>();
}

void MyRnFriendWindow::AddFriendBlock( const std::string& strName, bool bBlock )
{
    m_pGaeaClient->GetFriendClient()->ReqFriendBlock( strName.c_str(), bBlock );
    m_pCurrentPage->Refresh();
}
