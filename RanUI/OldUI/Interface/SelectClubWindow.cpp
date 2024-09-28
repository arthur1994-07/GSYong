#include "StdAfx.h"

#include "../../../SigmaCore/DebugSet.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "../Util/RnButton.h"
#include "./SelectCountryNode.h"
#include "./SelectClubList.h"
#include "./SelectClubWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectClubWindow::CSelectClubWindow(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIOuterWindow( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
    , m_pFlag(NULL)
    , m_pCountryInfoText(NULL)
    , m_pClubList(NULL)
    , m_pMakeClubBtn(NULL)
    , m_pClubNameEdit(NULL)
    , m_pSearchButton(NULL)
    , m_pComboRollSearchOver(NULL)
    , m_pSelectClubInfo(NULL)
    , m_pNotify(NULL)
    , m_pJoinButton(NULL)
    , m_pCloseButton(NULL)

    , m_nSelectString(0)
    , m_nComboPageNum(0)
    , m_nBeforeStrLength(0)
    , m_nSelectClubIndex(NON_SELECT_CLUB_LIST)
{
    m_vecString.clear();
    m_listSrcString.clear();
}

CSelectClubWindow::~CSelectClubWindow()
{
}

void CSelectClubWindow::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_CLUB_WINDOW_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_WINDOW_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_CLUB_WINDOW_BACK2", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_WINDOW_BACK_LINE2" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_CLUB_WINDOW_BODY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_WINDOW_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_CLUB_FLAG_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_FLAG_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CSelectCountryNode* pNode = new CSelectCountryNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pNode->CreateSub( this, "SELCET_CLUB_WINDOW_FLAG" );
        pNode->CreateSubControl();
        pNode->SetVisibleSingle( FALSE );
        RegisterControl( pNode );
        m_pFlag = pNode;
        m_pFlag->SetCountryInfo( m_pGaeaClient->wbGetData( wb::KOREA_REPUBLIC_OF ) );
    }

    {
        CD3DFontPar* pFont14Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 14, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "SELCET_CLUB_WINDOW_COUNTRY_INFO_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont14Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pCountryInfoText = pTextBox;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_CLUB_LIST_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_LIST_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CSelectClubList* pClubList = new CSelectClubList( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubList->CreateSub( this, "SELCET_CLUB_LIST_BACK" );
        pClubList->CreateSubControl();
        RegisterControl( pClubList );
        m_pClubList = pClubList;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_CLUB_INFO_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_INFO_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_CLUB_NOTIFY_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_NOTIFY_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        RnButton* pRnButton = NULL;
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont10Shadow;
        arg.text = ID2GAMEWORD( "SELECT_CLUB_WINDOW_MAKECLUB_BTN", 0 );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "SELCET_CLUB_MAKECLUB_BUTTON", UI_FLAG_DEFAULT, MAKE_CLUB_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pMakeClubBtn = pRnButton;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_CLUB_SEARCH_NAME_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_SEARCH_NAME_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont10 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

        CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
        pEditBoxMan->CreateSub( this, "SELCET_CLUB_SEARCH_NAME_EDIT_MAN", UI_FLAG_DEFAULT, CLUB_NAME_MAN );
        pEditBoxMan->CreateEditBox( CLUB_NAME_EDIT,
            "SELCET_CLUB_SEARCH_NAME_EDIT", "SELCET_CLUB_SEARCH_NAME_CARRAT",
            TRUE, UINT_MAX, pFont10, 18 );
        RegisterControl( pEditBoxMan );
        m_pClubNameEdit = pEditBoxMan;
    }

    {
        RnButton* pRnButton = NULL;
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont10Shadow;
        arg.text = ID2GAMEWORD( "SELECT_COUNTRY_WINDOW_SEARCH_BUTTON", 0 );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "SELCET_CLUB_SEARCH_NAME_BUTTON", UI_FLAG_DEFAULT, CLUB_NAME_SEARCH_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pSearchButton = pRnButton;
    }

    {
        CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, CLUB_NAME_ROLL_OVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "SELCET_CLUB_COMBO_ROLLOVER_SEARCH", FALSE );			
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pComboRollSearchOver = pComboBoxRollOver;

        LoadSearchList();
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "SELCET_CLUB_INFO_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pSelectClubInfo = pTextBox;
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "SELCET_CLUB_NOTIFY_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pNotify = pTextBox;
    }

    {
        RnButton* pRnButton = NULL;
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont10Shadow;
        arg.text = ID2GAMEWORD( "SELECT_CLUB_WINDOW_JOINCLUB_BTN", 0 );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "SELCET_CLUB_WINDOW_JOIN_BUTTON", UI_FLAG_DEFAULT, CLUB_JOIN_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pJoinButton = pRnButton;
    }

    {
        RnButton* pRnButton = NULL;
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont10Shadow;
        arg.text = ID2GAMEWORD( "SELECT_CLUB_WINDOW_CLOSE_BTN" );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "SELCET_CLUB_WINDOW_CLOSE_BUTTON", UI_FLAG_DEFAULT, CLUB_CLOSE_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pCloseButton = pRnButton;
    }
}

void CSelectClubWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIOuterWindow::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectClubWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    // Note : 윈도우 이동을 하지 않기 위해 호출을 하지 않는다.
    //CUIOuterWindow::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case CLUB_NAME_SEARCH_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pClubNameEdit->GetEditBox( CLUB_NAME_EDIT )->EndEdit();
                m_pComboRollSearchOver->SetVisibleSingle( FALSE );

                m_pClubList->SetClubListOrder( wb::search::CLUB_NAME_SEARCH, wb::search::ASC );
                SearchClub();
            }
        }
        break;

    case MAKE_CLUB_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                MakeNewClub();
            }
        }
        break;

    case CLUB_JOIN_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                JoinClub();
            }
        }
        break;

    case CLUB_CLOSE_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pInterface->UiHideGroup( GetWndID() );

                m_pInterface->ShowSelectWorldBattleClub();
            }
        }
        break;

    case CLUB_NAME_ROLL_OVER:
        {
            if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
            {				
                DWORD dwMsg = GetMessageEx();
                if ( dwMsg & UIMSG_LB_UP )
                {
                    dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
                    ResetMessageEx();
                    AddMessageEx( dwMsg );						
                }
                break;
            }

            if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
            {				
                int nIndex = m_pComboRollSearchOver->GetSelectIndex();
                if ( nIndex < 0 ) return ;
                CString strTemp = m_pComboRollSearchOver->GetSelectText( nIndex );								
                m_pClubNameEdit->SetEditString( CLUB_NAME_EDIT, strTemp );
                ComboInit();
            }
        }
        break;

    case CLUB_NAME_MAN:
    case CLUB_NAME_EDIT:
        {
            if( m_pClubNameEdit->GetEditLength( CLUB_NAME_EDIT ) != 0 )
            {
                if( DxInputDevice::GetInstance().IsUpdatKeyState() )
                {
                    if( UIKeyCheck::GetInstance()->Check( DIK_PGUP, DXKEY_DOWN )  )
                    {
                        if( m_nComboPageNum > 0 ) 
                        {
                            // 이전 페이지로 이동
                            m_nComboPageNum--;
                            m_nSelectString = 0;

                            UpdateComoboItem();

                            m_pClubNameEdit->SetEditString( CLUB_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }
                    }
                    else if( UIKeyCheck::GetInstance()->Check( DIK_PGDN, DXKEY_DOWN )  )
                    {
                        if(((m_nComboPageNum+1)*MAX_ROLLOVER_TEXT) < (int)m_vecString.size()) 
                        {
                            m_nComboPageNum++;
                            m_nSelectString = 0;

                            UpdateComoboItem();

                            m_pClubNameEdit->SetEditString( CLUB_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }

                    }
                    else if( UIKeyCheck::GetInstance()->Check( DIK_UP, DXKEY_DOWN )  )
                    {
                        if(m_nSelectString > 0) 
                        {
                            m_nSelectString--;
                            m_pClubNameEdit->SetEditString( CLUB_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }
                        else if( m_nComboPageNum > 0 ) 
                        {
                            // 이전 페이지로 이동
                            m_nComboPageNum--;
                            m_nSelectString = MAX_ROLLOVER_TEXT-1;

                            UpdateComoboItem();

                            m_pClubNameEdit->SetEditString( CLUB_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }
                    }
                    else if( UIKeyCheck::GetInstance()->Check( DIK_DOWN, DXKEY_DOWN )  )
                    {
                        if(m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT) < (int)m_vecString.size()-1) 
                        {
                            if( m_nSelectString < MAX_ROLLOVER_TEXT-1 )
                            {
                                m_nSelectString++;
                                m_pClubNameEdit->SetEditString( CLUB_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                                m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                            }
                            else
                            {
                                // 이후 페이지로 이동
                                m_nComboPageNum++;
                                m_nSelectString = 0;

                                UpdateComoboItem();

                                m_pClubNameEdit->SetEditString( CLUB_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                                m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                            }
                        }
                    }
                    else
                    {
                        std::string strCountryName = m_pClubNameEdit->GetEditString( CLUB_NAME_EDIT );

                        int nStrLength = static_cast<int>(strCountryName.length());

                        std::transform( strCountryName.begin(), strCountryName.end(), strCountryName.begin(), std::tolower );
                        if ( strcmp( m_strBeforeEdit.c_str(), strCountryName.c_str() ) )
                        {
                            if( m_nBeforeStrLength < nStrLength && m_vecString.size() != 0 )
                            {
                                std::vector<CString> vecString = m_vecString;

                                m_vecString.clear();
                                m_nSelectString = -1;
                                m_nComboPageNum = 0;

                                for( size_t i = 0; i < vecString.size(); i++ )
                                {
                                    std::string strLower = vecString[i];
                                    std::transform( strLower.begin(), strLower.end(), strLower.begin(), std::tolower );
                                    CString strTemp = strLower.c_str();
                                    if( strTemp.Find( strCountryName.c_str() ) != -1 )
                                    {
                                        m_vecString.push_back( vecString[i] );
                                    }
                                }
                            }
                            else
                            {
                                m_vecString.clear();
                                m_nSelectString = -1;
                                m_nComboPageNum = 0;

                                SRCLIST_ITER iter = m_listSrcString.begin();

                                for( ; iter != m_listSrcString.end(); ++iter )
                                {
                                    CString string = *iter;
                                    std::string strLower = string;
                                    std::transform( strLower.begin(), strLower.end(), strLower.begin(), std::tolower );
                                    CString strTemp = strLower.c_str();
                                    if( strTemp.Find( strCountryName.c_str() ) != -1 )
                                    {
                                        m_vecString.push_back( string );
                                    }
                                }

                            }
                            UpdateComoboItem();
                            if( m_vecString.size() != 0 )
                                m_nBeforeStrLength = static_cast<int>(strCountryName.length());

                            m_strBeforeEdit = strCountryName;
                        }
                    }
                }				
            }
            else
            {
                ComboInit();
            }

            if( DxInputDevice::GetInstance().IsUpdatKeyState() )
            {
                if ( m_pClubNameEdit->GetEditBox( CLUB_NAME_EDIT )->IsBegin() )
                {
                    if( UIKeyCheck::GetInstance()->Check( DIK_RETURN, DXKEY_DOWN )
                        || UIKeyCheck::GetInstance()->Check( DIK_NUMPADENTER, DXKEY_DOWN ) )
                    {
                        m_pClubNameEdit->GetEditBox( CLUB_NAME_EDIT )->EndEdit();
                        m_pComboRollSearchOver->SetVisibleSingle( FALSE );

                        SearchClub();
                    }
                }
            }

            if ( m_pClubNameEdit->GetEditBox( CLUB_NAME_EDIT )->IsBegin() )
            {
                if ( (UIMSG_LB_DOWN & dwMsg) ||
                    (UIMSG_MB_DOWN & dwMsg) ||
                    (UIMSG_RB_DOWN & dwMsg)
                    )
                {
                    m_pClubNameEdit->GetEditBox( CLUB_NAME_EDIT )->EndEdit();
                }
            }
        }
        break;
    };
}

void CSelectClubWindow::SetCountryInfo( const wb::EM_COUNTRY& emCountry )
{
    if ( m_pGaeaClient->GetServiceProvider() != SP_WORLD_BATTLE &&
        !GLUseFeatures::GetInstance().IsUsingEssentialSelectClub() )
    {
        wb::EM_COUNTRY Country;

        GetCountryByServiceProvider( Country );

        const wb::COUNTRY_INFO& CountryInfo = m_pGaeaClient->wbGetData(Country);
        m_pFlag->SetCountryInfo( CountryInfo );
        m_pFlag->SetVisibleSingle( TRUE );

        m_pCountryInfoText->ClearText();
        m_pCountryInfoText->AddText( CountryInfo.m_FullName.c_str(), NS_UITEXTCOLOR::WHITE );
        m_pCountryInfoText->AddText( CountryInfo.m_Desc.c_str(), NS_UITEXTCOLOR::WHITE );

        m_pGaeaClient->ReqClubCountryInfo( Country );

        m_pCloseButton->SetVisibleSingle( TRUE );
    }
    else
    {
        const wb::COUNTRY_INFO& CountryInfo = m_pGaeaClient->wbGetData(emCountry);
        m_pFlag->SetCountryInfo( CountryInfo );
        m_pFlag->SetVisibleSingle( TRUE );

        m_pCountryInfoText->ClearText();
        m_pCountryInfoText->AddText( CountryInfo.m_FullName.c_str(), NS_UITEXTCOLOR::WHITE );
        m_pCountryInfoText->AddText( CountryInfo.m_Desc.c_str(), NS_UITEXTCOLOR::WHITE );

        m_pGaeaClient->ReqClubCountryInfo( emCountry );

        m_pCloseButton->SetVisibleSingle( FALSE );
    }
}

void CSelectClubWindow::GetCountryByServiceProvider( wb::EM_COUNTRY& Country )
{
    switch ( m_pGaeaClient->GetServiceProvider() )
    {
    case SP_OFFICE_TEST:
    case SP_KOREA:
        Country = wb::KOREA_REPUBLIC_OF;
        break;

    case SP_TAIWAN:
        Country = wb::TAIWAN_PROVINCE_OF_CHINA;
        break;
    case SP_CHINA:
        Country = wb::CHINA;
        break;
    case SP_JAPAN:
        Country = wb::JAPAN;
        break;
    case SP_PHILIPPINES:
        Country = wb::PHILIPPINES;
        break;
    case SP_THAILAND:
        Country = wb::THAILAND;
        break;
    /*case SP_GLOBAL:
        Country = wb::UNKNOWN_COUNTRY;
        break;*/
    case SP_MALAYSIA:
    case SP_MALAYSIA_EN:
        Country = wb::MALAYSIA;
        break;
    /*case SP_GS:
        Country = wb::UNKNOWN_COUNTRY;
        break;*/
    case SP_INDONESIA:
        Country = wb::INDONESIA;
        break;
    case SP_VIETNAM:
        Country = wb::VIETNAM;
        break;
    case SP_HONGKONG:
        Country = wb::HONG_KONG;
        break;
    default:
        Country = wb::UNKNOWN_COUNTRY;
    };
}

void CSelectClubWindow::ReloadClubList()
{
    ComboInit();
    m_pClubNameEdit->SetEditString( CLUB_NAME_EDIT, "" );

    m_pClubList->SetSearchName( std::string( "" ) );

    m_pClubList->SearchClub();
    LoadSearchList();
    ResetSelectClub();
}

void CSelectClubWindow::SetClubListOrder( wb::search::EM_ORDER_TYPE Type, wb::search::EM_ORDER_ASC_DSC Order )
{
    m_pClubList->SetClubListOrder( Type, Order );
}

void CSelectClubWindow::SearchClub()
{
    std::string strSearchWord = sc::string::format( "%s", m_pClubNameEdit->GetEditString( CLUB_NAME_EDIT ) );

    m_pClubList->SetSearchName( strSearchWord );
    m_pClubList->SearchClub();
    LoadSearchList();
    ResetSelectClub();
}

void CSelectClubWindow::SelectClubList( int nIndex )
{
    m_pClubList->SelectClubList( nIndex );

    m_pSelectClubInfo->ClearText();
    m_pNotify->ClearText();

    const std::vector<wb::CLUB_INFO_BASE>& vecClubList = m_pClubList->GetClubList();
    if ( nIndex < 0 || static_cast<int>(vecClubList.size()) <= nIndex )
        return;

    m_nSelectClubIndex = nIndex;
    const wb::CLUB_INFO_BASE& ClubInfo = vecClubList[ m_nSelectClubIndex ];

    m_pSelectClubInfo->AddText( ClubInfo.m_ClubName.c_str(), NS_UITEXTCOLOR::WHITE );
    m_pSelectClubInfo->AddText( ClubInfo.m_MasterName.c_str(), NS_UITEXTCOLOR::WHITE );

    struct tm sTime;
    localtime_s( &sTime, &ClubInfo.m_OrganizeDate );
    std::string strText = sc::string::format(
        ID2GAMEEXTEXT( "SELECT_CLUB_WINDOW_SELECT_CLUB_INFO_0" ),
        sTime.tm_year+1900, sTime.tm_mon+1, sTime.tm_mday );
    m_pSelectClubInfo->AddText( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    strText = sc::string::format(
        ID2GAMEEXTEXT( "SELECT_CLUB_WINDOW_SELECT_CLUB_INFO_1" ), ClubInfo.m_MemberNum );
    m_pSelectClubInfo->AddText( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    m_pNotify->SetText( ID2GAMEEXTEXT( "SELECT_CLUB_NOTIFY" ), NS_UITEXTCOLOR::WHITE );
}

void CSelectClubWindow::LoadSearchList()
{
    m_listSrcString.clear();

    const std::vector<wb::CLUB_INFO_BASE>& vecClubList = m_pClubList->GetClubList();

    for ( int i = 0; i < static_cast<int>(vecClubList.size()); i++ )
    {
        CString strClubName;
        strClubName.Format( "%s", vecClubList[i].m_ClubName.c_str() );

        m_listSrcString.push_back( strClubName );
    }
}

void CSelectClubWindow::ResetSelectClub()
{
    m_nSelectClubIndex = NON_SELECT_CLUB_LIST;

    m_pSelectClubInfo->ClearText();
    m_pNotify->ClearText();
}

void CSelectClubWindow::ComboInit()
{
    m_pComboRollSearchOver->SetVisibleSingle(FALSE);
    m_pComboRollSearchOver->ClearText();
    m_vecString.clear();
    m_nBeforeStrLength = 0;
    m_nSelectString = -1;
    m_nComboPageNum = 0;
}

void CSelectClubWindow::UpdateComoboItem()
{
    if( m_vecString.size() != 0 )
    {
        m_pComboRollSearchOver->SetVisibleSingle(TRUE);
        m_pComboRollSearchOver->ClearText();
        size_t count = 0;
        for( size_t i = (m_nComboPageNum*MAX_ROLLOVER_TEXT); i < m_vecString.size(); i++ )
        {						
            if( m_vecString.size() > MAX_ROLLOVER_TEXT ) 
                m_pComboRollSearchOver->AddText(m_vecString[i], TRUE, TRUE );
            else				  
                m_pComboRollSearchOver->AddText(m_vecString[i], FALSE, TRUE );

            count++;
            if( count >= MAX_ROLLOVER_TEXT )
                break;
        }						
    }
    else
    {
        m_pComboRollSearchOver->SetVisibleSingle(FALSE);
        m_pComboRollSearchOver->ClearText();
    }
}

void CSelectClubWindow::JoinClub()
{
    if ( m_nSelectClubIndex == NON_SELECT_CLUB_LIST )
        return;

    const std::vector<wb::CLUB_INFO_BASE>& vecClubList = m_pClubList->GetClubList();
    if ( m_nSelectClubIndex < 0 || static_cast<int>(vecClubList.size()) <= m_nSelectClubIndex )
        return;

    const wb::CLUB_INFO_BASE& ClubInfo = vecClubList[ m_nSelectClubIndex ];

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    if ( !pGlobalStage )
        return;

    SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
    if ( !pCharInfo )
        return;

    m_pInterface->UiHideGroup( SELECT_CLUB_WINDOW );

    DWORD dwDbCharNum = pCharInfo->ChaDbNum();
    std::string strCharName = sc::string::format( "%1%", pCharInfo->m_ChaName );
    m_pGaeaClient->JoinClubLobby(ClubInfo.m_ClubDbNum, dwDbCharNum, strCharName, static_cast<EMCHARCLASS> (pCharInfo->m_emClass));
}

void CSelectClubWindow::MakeNewClub()
{
    m_pInterface->UiHideGroup( SELECT_CLUB_WINDOW );
    m_pInterface->DoModal( ID2GAMEEXTEXT("WB_INPUT_MAKE_CLUB_NAME"), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_INPUT_MAKE_CLUB_NAME );
}