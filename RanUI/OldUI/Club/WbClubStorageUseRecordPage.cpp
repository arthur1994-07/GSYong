#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubStorageUseRecordNode.h"
#include "./WbClubStorageUseRecordPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CWbClubStorageUseRecordPage::fRecordNodeOffset = 5.0f;

CWbClubStorageUseRecordPage::CWbClubStorageUseRecordPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pPreBtn(NULL)
    , m_pNextBtn(NULL)
    , m_Page(0)
{
    for ( int i = 0; i < NUM_RECORD_NODE; i++ )
        m_pUseRecordNode[i] = NULL;
}

CWbClubStorageUseRecordPage::~CWbClubStorageUseRecordPage ()
{
}

void CWbClubStorageUseRecordPage::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_TEXT" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_USE_RECORD", 0), NS_UITEXTCOLOR::WHITE );
    }

    {
        CWbClubStorageUseRecordNode* pStorageUseRecordNode = new CWbClubStorageUseRecordNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pStorageUseRecordNode->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_NODE" );
        pStorageUseRecordNode->CreateSubControl();
        RegisterControl( pStorageUseRecordNode );
        m_pUseRecordNode[0] = pStorageUseRecordNode;

        for ( int i = 1; i < NUM_RECORD_NODE; i++ )
        {
            CWbClubStorageUseRecordNode* pStorageUseRecordNode = new CWbClubStorageUseRecordNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pStorageUseRecordNode->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_NODE" );
            pStorageUseRecordNode->CreateSubControl();
            RegisterControl( pStorageUseRecordNode );
            m_pUseRecordNode[i] = pStorageUseRecordNode;

            D3DXVECTOR2 vPos;
            vPos.x = m_pUseRecordNode[0]->GetLocalPos().left;
            vPos.y = m_pUseRecordNode[0]->GetLocalPos().top;

            vPos.y += (m_pUseRecordNode[0]->GetLocalPos().sizeY + fRecordNodeOffset) * i;
            m_pUseRecordNode[i]->SetLocalPos( vPos );
        }
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_BTN_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont10Shadow;
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        arg.text = sc::string::format( "<< %1%", ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_USE_RECORD", 1) );
        RnButton* pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_PRE_BTN", UI_FLAG_DEFAULT, PRE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pPreBtn = pRnButton;
        m_pPreBtn->SetEnable( false );

        arg.text = sc::string::format( "%1% >>", ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_USE_RECORD", 2) );
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_NEXT_BTN", UI_FLAG_DEFAULT, NEXT_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pNextBtn = pRnButton;
        m_pNextBtn->SetEnable( false );
    }
}

void CWbClubStorageUseRecordPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubStorageUseRecordPage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case PRE_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                if ( m_Page > 0 )
                {
                    m_Page--;

                    Refresh();
                }
            }
        }
        break;

    case NEXT_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_Page++;

                std::vector<club::LOG_DATA> vecLogData;
                if ( !m_pGaeaClient->GetClubLog( club::LOG_CLUB_STORAGE, m_Page, NUM_RECORD_NODE, vecLogData ) )
                {
                    m_Page--;
                    Refresh();
                    m_pNextBtn->SetEnable( false );
                }
                else
                    Refresh();
            }
        }
        break;
    };
}

void CWbClubStorageUseRecordPage::Refresh()
{
    ClearInfo();

    ClubStorageUpdateLog();
}

void CWbClubStorageUseRecordPage::ClearInfo()
{
    for ( int i = 0; i < NUM_RECORD_NODE; i++)
    {
        m_pUseRecordNode[i]->ClearInfo();
    }

    m_pPreBtn->SetEnable( false );
    m_pNextBtn->SetEnable( false );
}


void CWbClubStorageUseRecordPage::ClubStorageUpdateLog()
{
    std::vector<club::LOG_DATA> vecLogData;
    if ( !m_pGaeaClient->GetClubLog( club::LOG_CLUB_STORAGE, m_Page, NUM_RECORD_NODE, vecLogData ) )
        return;

    int nLogDataSize = vecLogData.size();
    int i = 0;
    for ( i = 0; i < NUM_RECORD_NODE && i < nLogDataSize; i++)
    {
        const club::LOG_DATA& LogData = vecLogData[i];
        std::string strContent = LogData.m_LogText;
        m_pUseRecordNode[i]->SetContent( strContent );
        m_pUseRecordNode[i]->SetDate( LogData.m_LogDate );
    }

    if ( i < NUM_RECORD_NODE )
    {
        for ( int j = i; j < NUM_RECORD_NODE; j++ )
        {
            m_pUseRecordNode[j]->ClearInfo();
        }
    }

    m_pPreBtn->SetEnable( true );
    m_pNextBtn->SetEnable( true );
}