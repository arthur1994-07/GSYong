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
#include "./SelectCountryWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CSelectCountryWindow::NODE_HORIZONTAL_INTERVAL_OFFSET = 20.0f;
const float CSelectCountryWindow::NODE_VERTICAL_INTERVAL_OFFSET = 10.0f;

CSelectCountryWindow::CSelectCountryWindow(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIOuterWindow( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
    , m_pTitle(NULL)
    , m_pCountryBack(NULL)
    , m_pCountryNodeTemp(NULL)
    , m_pSelectCountryNode(NULL)
    , m_pCountryNameEdit(NULL)
    , m_pComboRollSearchOver(NULL)
    , m_pSearchButton(NULL)
    , m_pSelectCountryFlag(NULL)
    , m_pSelectCountryDescText(NULL)
    , m_pSelectCountryNotify(NULL)
    , m_pOKButton(NULL)

    , m_nStartRowIndex(0)
    , m_nCountryInfoSize(0)
    , m_nSelectCountryNode(NON_SELECT_COUNTRY_NODE)
    , m_nSelectString(0)
    , m_nComboPageNum(0)
    , m_nBeforeStrLength(0)
{
    for ( int i = 0; i < MAX_NODE_ONE_PAGE; i++ )
        m_pCountryNode[ i ] = NULL;

    m_vecString.clear();
    m_listSrcString.clear();
}

CSelectCountryWindow::~CSelectCountryWindow()
{
}

void CSelectCountryWindow::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_WINDOW_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_WINDOW_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_WINDOW_BACK2", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_WINDOW_BACK_LINE2" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_WINDOW_BODY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_WINDOW_BODY_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "SELCET_COUNTRY_WINDOW_TITLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pTitle = pTextBox;
        m_pTitle->SetText( ID2GAMEEXTEXT( "SELECT_COUNTRY_WINDOW_TITLE" ), NS_UITEXTCOLOR::WHITE );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_WINDOW_COUNTRY_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_WINDOW_COUNTRY_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pCountryBack = pLinBoxSmart;
    }

    {
        //	스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, SCROLL_BAR );
        pScrollBar->CreateBaseScrollBar( "SELCET_COUNTRY_WINDOW_COUNTRY_SCROLLBAR" );
        pScrollBar->GetThumbFrame()->SetState( 1, m_nCountryInfoSize );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pCountryBack );
        pScrollBar->GetThumbFrame()->SetScrollMoveLine( 2 );
        RegisterControl( pScrollBar );
        m_pMainScrollBar = pScrollBar;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "SELCET_COUNTRY_WINDOW_NODE", UI_FLAG_DEFAULT );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pCountryNodeTemp = pControl;
    }

    {
        for ( int i = 0; i < MAX_NODE_ONE_PAGE; i++ )
        {
            CSelectCountryNode* pNode = new CSelectCountryNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pNode->CreateSub( this, "SELCET_COUNTRY_WINDOW_NODE", UI_FLAG_DEFAULT, (NODE_START+i) );
            pNode->CreateSubControl();
            RegisterControl( pNode );
            m_pCountryNode[ i ] = pNode;
        }

        SetLocateAllCountryNodes();
        ReLoadFlags();
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_WINDOW_NODE_SELECT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_WINDOW_NODE_SELECT_LINE" );
        pLinBoxSmart->SetVisibleSingle( FALSE );
        RegisterControl( pLinBoxSmart );
        m_pSelectCountryNode = pLinBoxSmart;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_SEARCH_NAME_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_SEARCH_NAME_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont10 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

        CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
        pEditBoxMan->CreateSub( this, "SELCET_COUNTRY_SEARCH_NAME_EDIT_MAN", UI_FLAG_DEFAULT, COUNTRY_NAME_MAN );
        pEditBoxMan->CreateEditBox( COUNTRY_NAME_EDIT,
            "SELCET_COUNTRY_SEARCH_NAME_EDIT", "SELCET_COUNTRY_SEARCH_NAME_CARRAT",
            TRUE, UINT_MAX, pFont10, 18 );
        RegisterControl( pEditBoxMan );
        m_pCountryNameEdit = pEditBoxMan;
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
        pRnButton->CreateSub(this, "SELCET_COUNTRY_SEARCH_NAME_BUTTON", UI_FLAG_DEFAULT, COUNTRY_NAME_SEARCH_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pSearchButton = pRnButton;
    }

    {
        CSelectCountryNode* pNode = new CSelectCountryNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pNode->CreateSub( this, "SELCET_COUNTRY_SELECT_COUNTRY_FLAG" );
        pNode->CreateSubControl();
        pNode->SetVisibleSingle( FALSE );
        RegisterControl( pNode );
        m_pSelectCountryFlag = pNode;
        if (!m_vecCountryInfo.empty())
            m_pSelectCountryFlag->SetCountryInfo(m_vecCountryInfo[0]);
    }

    {
        CD3DFontPar* pFont14Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 14, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "SELCET_COUNTRY_SELECT_COUNTRY_DESC", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont14Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pSelectCountryDescText = pTextBox;
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "SELCET_COUNTRY_SELECT_COUNTRY_NOTIFY", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pSelectCountryNotify = pTextBox;
    }

    {
        CBasicComboBoxRollOver *pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, COUNTRY_NAME_ROLL_OVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "SELCET_COUNTRY_COMBO_ROLLOVER_SEARCH", FALSE );			
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pComboRollSearchOver = pComboBoxRollOver;

        LoadSearchList();
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
        arg.text = ID2GAMEWORD( "SELECT_COUNTRY_WINDOW_OK_BUTTON", 0 );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "SELCET_COUNTRY_OK_BUTTON", UI_FLAG_DEFAULT, OK_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pOKButton = pRnButton;
    }
}

void CSelectCountryWindow::SetLocateAllCountryNodes()
{
    const UIRECT& rcTempLocalPos = m_pCountryNodeTemp->GetLocalPos();
    const UIRECT& rcTempGlobalPos = m_pCountryNodeTemp->GetGlobalPos();

    int nCurRow = -1;

    for ( int i = 0; i < MAX_NODE_ONE_PAGE; i++ )
    {
        int nCulumn = i % NUM_NODE_PER_ROW;
        if ( nCulumn == 0 )
            nCurRow++;

        D3DXVECTOR2 vPos;
        vPos.x = rcTempLocalPos.left + ( NODE_HORIZONTAL_INTERVAL_OFFSET + rcTempLocalPos.sizeX ) * nCulumn;
        vPos.y = rcTempLocalPos.top + ( NODE_VERTICAL_INTERVAL_OFFSET + rcTempLocalPos.sizeY ) * nCurRow;

        m_pCountryNode[ i ]->SetLocalPos( vPos );

        vPos.x = rcTempGlobalPos.left + ( NODE_HORIZONTAL_INTERVAL_OFFSET + rcTempGlobalPos.sizeX ) * nCulumn;
        vPos.y = rcTempGlobalPos.top + ( NODE_VERTICAL_INTERVAL_OFFSET + rcTempGlobalPos.sizeY ) * nCurRow;

        m_pCountryNode[ i ]->SetGlobalPos( vPos );
    }
}

void CSelectCountryWindow::SelectCountryNode( int nNodeIndex )
{
    if ( nNodeIndex < 0 || nNodeIndex >= MAX_NODE_ONE_PAGE )
        return;

    m_nSelectCountryNode = nNodeIndex;

    SetLocateSelectNode( m_nSelectCountryNode );

    const wb::COUNTRY_INFO& CountryInfo = m_pCountryNode[ m_nSelectCountryNode ]->GetCountyInfo();

    m_pSelectCountryFlag->SetCountryInfo( CountryInfo );
    m_pSelectCountryFlag->SetVisibleSingle( TRUE );

    ShowCountryClubCount();

    m_pSelectCountryNotify->SetText( ID2GAMEEXTEXT( "SELECT_COUNTRY_WINDOW_COUNTRY_NOTIFY" ), NS_UITEXTCOLOR::RED );
}

void CSelectCountryWindow::ShowCountryClubCount()
{
    if ( !IsSelected() )
        return;

    const wb::COUNTRY_INFO& CountryInfo = m_pCountryNode[ m_nSelectCountryNode ]->GetCountyInfo();
    WORD wClubCount = m_pGaeaClient->wbGetData( CountryInfo.m_Code ).m_ClubCount;
    std::string strDesc = sc::string::format( ID2GAMEEXTEXT("SELECT_COUNTRY_WINDOW_COUNTRY_DESC"),
        CountryInfo.m_FullName, CountryInfo.m_Desc );
    m_pSelectCountryDescText->SetText( strDesc.c_str(), NS_UITEXTCOLOR::WHITE );

    strDesc = sc::string::format( ID2GAMEEXTEXT("SELECT_COUNTRY_WINDOW_COUNTRY_CLUB_COUNT"),
        wClubCount );
    m_pSelectCountryDescText->AddText( strDesc.c_str(), NS_UITEXTCOLOR::WHITE );
}

void CSelectCountryWindow::SetLocateSelectNode( int nNodeIndex )
{
    if ( nNodeIndex < 0 || nNodeIndex >= MAX_NODE_ONE_PAGE )
        return;

    const UIRECT& rcTempLocalPos = m_pCountryNode[ nNodeIndex ]->GetLocalPos();
    const UIRECT& rcTempGlobalPos = m_pCountryNode[ nNodeIndex ]->GetGlobalPos();

    m_pSelectCountryNode->SetLocalPos( rcTempLocalPos );
    m_pSelectCountryNode->SetGlobalPos( rcTempGlobalPos );

    m_pSelectCountryNode->SetVisibleSingle( TRUE );
}

void CSelectCountryWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIOuterWindow::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        CBasicScrollThumbFrame* pThumbFrame = m_pMainScrollBar->GetThumbFrame();
        float fCalTotalLine = ceil( m_vecCountryInfo.size() / static_cast<float>(NUM_NODE_PER_ROW) );
        const int nTotalLine = static_cast<int>( fCalTotalLine );
        pThumbFrame->SetState( nTotalLine, MAX_ROW_ONE_PAGE );
        if ( MAX_ROW_ONE_PAGE < nTotalLine )
        {
            const int nMovableLine = nTotalLine - MAX_ROW_ONE_PAGE;
            float fPercent = pThumbFrame->GetPercent();
            int nPos = static_cast<int>( floor(fPercent * nMovableLine) );
            if ( nPos != m_nStartRowIndex )
            {
                m_nStartRowIndex = nPos;
                ReLoadFlags();
            }
            else
                m_nStartRowIndex = nPos;
        }
    }
}

void CSelectCountryWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    // Note : 윈도우 이동을 하지 않기 위해 호출을 하지 않는다.
    //CUIOuterWindow::TranslateUIMessage( ControlID, dwMsg );

    if ( NODE_START <= ControlID && ControlID < (NODE_START+m_nCountryInfoSize) )
    {
        if ( CHECK_MOUSE_IN( dwMsg ) )
        {
            int nIndex = ControlID - NODE_START;

            const wb::COUNTRY_INFO& CountryInfo = m_pCountryNode[ nIndex ]->GetCountyInfo();

            std::string strTooltip = sc::string::format( ID2GAMEEXTEXT("SELECT_COUNTRY_WINDOW_COUNTRY_TOOLTIP"),
                CountryInfo.m_FullName, CountryInfo.m_ClubCount );

            m_pInterface->SHOW_COMMON_LINEINFO( strTooltip.c_str(), NS_UITEXTCOLOR::PALEGREEN );

            if ( dwMsg & UIMSG_LB_DUP )
            {
                DoneSelectCountry();
            }
        }

        if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
        {
            m_nSelectCountryNode = ControlID - NODE_START;
            SelectCountryNode( m_nSelectCountryNode );
        }
    }

    switch ( ControlID )
    {
    case OK_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                DoneSelectCountry();
            }
        }
        break;

    case COUNTRY_NAME_SEARCH_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pCountryNameEdit->GetEditBox( COUNTRY_NAME_EDIT )->EndEdit();
                m_pComboRollSearchOver->SetVisibleSingle( FALSE );

                SearchCountry();
            }
        }
        break;

    case COUNTRY_NAME_ROLL_OVER:
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
                m_pCountryNameEdit->SetEditString( COUNTRY_NAME_EDIT, strTemp );
                ComboInit();
            }
        }
        break;

    case COUNTRY_NAME_MAN:
    case COUNTRY_NAME_EDIT:
        {
            if( m_pCountryNameEdit->GetEditLength( COUNTRY_NAME_EDIT ) != 0 )
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

                            m_pCountryNameEdit->SetEditString( COUNTRY_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
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

                            m_pCountryNameEdit->SetEditString( COUNTRY_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }

                    }
                    else if( UIKeyCheck::GetInstance()->Check( DIK_UP, DXKEY_DOWN )  )
                    {
                        if(m_nSelectString > 0) 
                        {
                            m_nSelectString--;
                            m_pCountryNameEdit->SetEditString( COUNTRY_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                            m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                        }
                        else if( m_nComboPageNum > 0 ) 
                        {
                            // 이전 페이지로 이동
                            m_nComboPageNum--;
                            m_nSelectString = MAX_ROLLOVER_TEXT-1;

                            UpdateComoboItem();

                            m_pCountryNameEdit->SetEditString( COUNTRY_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
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
                                m_pCountryNameEdit->SetEditString( COUNTRY_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                                m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                            }
                            else
                            {
                                // 이후 페이지로 이동
                                m_nComboPageNum++;
                                m_nSelectString = 0;

                                UpdateComoboItem();

                                m_pCountryNameEdit->SetEditString( COUNTRY_NAME_EDIT, m_vecString[m_nSelectString+(m_nComboPageNum*MAX_ROLLOVER_TEXT)] );
                                m_pComboRollSearchOver->SetSelectIndex( m_nSelectString );
                            }
                        }
                    }
                    else
                    {
                        std::string strCountryName = m_pCountryNameEdit->GetEditString( COUNTRY_NAME_EDIT );

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
                if ( m_pCountryNameEdit->GetEditBox( COUNTRY_NAME_EDIT )->IsBegin() )
                {
                    if( UIKeyCheck::GetInstance()->Check( DIK_RETURN, DXKEY_DOWN )
                        || UIKeyCheck::GetInstance()->Check( DIK_NUMPADENTER, DXKEY_DOWN ) )
                    {
                        m_pCountryNameEdit->GetEditBox( COUNTRY_NAME_EDIT )->EndEdit();
                        m_pComboRollSearchOver->SetVisibleSingle( FALSE );

                        SearchCountry();
                    }
                }
            }

            if ( m_pCountryNameEdit->GetEditBox( COUNTRY_NAME_EDIT )->IsBegin() )
            {
                if ( (UIMSG_LB_DOWN & dwMsg) ||
                    (UIMSG_MB_DOWN & dwMsg) ||
                    (UIMSG_RB_DOWN & dwMsg)
                    )
                {
                    m_pCountryNameEdit->GetEditBox( COUNTRY_NAME_EDIT )->EndEdit();
                }
            }
        }
        break;
    };
}

void CSelectCountryWindow::SetVisibleSingle( BOOL isVisible )
{
    if ( isVisible == TRUE )
    {
        m_pGaeaClient->ReqClubCountryInfo();
    }

    CUIOuterWindow::SetVisibleSingle( isVisible );
}

void CSelectCountryWindow::ReLoadFlags()
{
    m_vecCountryInfo.clear();
    m_pGaeaClient->wbSearch(m_vecCountryInfo);

    int nCountryInfoIndex = static_cast<int>( NUM_NODE_PER_ROW * m_nStartRowIndex );
    m_nCountryInfoSize = static_cast<int>( m_vecCountryInfo.size() );
    if ( nCountryInfoIndex >= m_nCountryInfoSize )
        nCountryInfoIndex = 0;

    wb::EM_COUNTRY emCountry = wb::UNKNOWN_COUNTRY;
    if ( IsSelected() )
    {
        emCountry = m_pSelectCountryFlag->GetCountyInfo().m_Code;
        m_pSelectCountryNode->SetVisibleSingle( FALSE );
    }

    int nIndex = 0;
    for ( nIndex = 0; nIndex < MAX_NODE_ONE_PAGE; nIndex++, nCountryInfoIndex++ )
    {
        if ( nCountryInfoIndex >= m_nCountryInfoSize )
            break;

        m_pCountryNode[nIndex]->SetCountryInfo( m_vecCountryInfo[nCountryInfoIndex] );
        m_pCountryNode[nIndex]->SetVisibleSingle( TRUE );

        if ( emCountry == m_pCountryNode[nIndex]->GetCountyInfo().m_Code )
            SetLocateSelectNode( nIndex );
    }

    if ( nIndex < MAX_NODE_ONE_PAGE )
    {
        for ( int i = nIndex; i < MAX_NODE_ONE_PAGE; i++ )
            m_pCountryNode[ i ]->SetVisibleSingle( FALSE );
    }
}

void CSelectCountryWindow::SearchCountry()
{
    m_pComboRollSearchOver->SetVisibleSingle( FALSE );

    std::string strSearchWord = sc::string::format( "%s", m_pCountryNameEdit->GetEditString( COUNTRY_NAME_EDIT ) );

    std::vector<wb::COUNTRY_INFO> SearchResult;

    m_pGaeaClient->wbSearch( strSearchWord, SearchResult );

    if ( SearchResult.size() != 1 ) // 결과가 1개일 때만, 찾은 국가를 선택한다.
        return;

    wb::COUNTRY_INFO& SearchCountryInfo = SearchResult[0];

    for ( int i = 0; i < m_nCountryInfoSize; i++ )
    {
        wb::COUNTRY_INFO& CountryInfo = m_vecCountryInfo[ i ];

        if ( SearchCountryInfo.m_Code == CountryInfo.m_Code )
        {
            SearchCountryNodeByInfoIndex( i );
        }
    }
}

void CSelectCountryWindow::SearchCountryNodeByInfoIndex( int nInfoIndex )
{
    m_nStartRowIndex = nInfoIndex / NUM_NODE_PER_ROW;
    int nColIndex = nInfoIndex % NUM_NODE_PER_ROW;

    const int nTotalLine = static_cast<int>( m_vecCountryInfo.size() / NUM_NODE_PER_ROW );

    if ( m_nStartRowIndex + MAX_ROW_ONE_PAGE > nTotalLine )
    {
        int nRowGap = m_nStartRowIndex - (nTotalLine - MAX_ROW_ONE_PAGE);

        nColIndex += (nRowGap*NUM_NODE_PER_ROW);

        m_nStartRowIndex = nTotalLine - MAX_ROW_ONE_PAGE;
    }

     CBasicScrollThumbFrame* pThumbFrame = m_pMainScrollBar->GetThumbFrame();
     float fPercent = static_cast<float>(m_nStartRowIndex) / (nTotalLine - MAX_ROW_ONE_PAGE);
     pThumbFrame->SetPercent( fPercent );

     ReLoadFlags();

     SelectCountryNode( nColIndex );
}

void CSelectCountryWindow::ComboInit()
{
    m_pComboRollSearchOver->SetVisibleSingle(FALSE);
    m_pComboRollSearchOver->ClearText();
    m_vecString.clear();
    m_nBeforeStrLength = 0;
    m_nSelectString = -1;
    m_nComboPageNum = 0;
}

void CSelectCountryWindow::UpdateComoboItem()
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

void CSelectCountryWindow::LoadSearchList()
{
    m_listSrcString.clear();

    for ( int i = 0; i < m_nCountryInfoSize; i++ )
    {
        CString strCountryName;
        strCountryName.Format( "%s", m_vecCountryInfo[i].m_FullName.c_str() );

        m_listSrcString.push_back( strCountryName );
    }
}

void CSelectCountryWindow::ResetAll()
{
    m_pCountryNameEdit->ClearEdit( COUNTRY_NAME_EDIT );

    m_pCountryNameEdit->Init();
    m_pCountryNameEdit->GetEditBox( COUNTRY_NAME_EDIT )->EndEdit();
    m_pCountryNameEdit->GetEditBox( COUNTRY_NAME_EDIT )->BeginEdit();

    m_nSelectString = 0;
    m_nComboPageNum = 0;
}

void CSelectCountryWindow::DoneSelectCountry()
{
    if ( !IsSelected() )
        return;

    m_pInterface->UiHideGroup( SELECT_COUNTRY_WINDOW );

    m_pInterface->ShowAfterSelectModal( m_pSelectCountryFlag->GetCountyInfo() );
}

bool CSelectCountryWindow::IsSelected()
{
    if ( m_nSelectCountryNode < 0 || m_nSelectCountryNode >= MAX_NODE_ONE_PAGE )
        return false;

    return true;
}
