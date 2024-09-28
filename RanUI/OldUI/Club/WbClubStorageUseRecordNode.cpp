#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Util/DateTime.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubStorageUseRecordNode.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CWbClubStorageUseRecordNode::CWbClubStorageUseRecordNode(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pUseRecordContent(NULL)
    , m_pUseRecordDate(NULL)
{
}

CWbClubStorageUseRecordNode::~CWbClubStorageUseRecordNode ()
{
}

void CWbClubStorageUseRecordNode::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_NODE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_NODE_CONTENT" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT| TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pUseRecordContent = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_USE_RECORD_NODE_DATE" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pUseRecordDate = pTextBox;
    }
}

void CWbClubStorageUseRecordNode::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubStorageUseRecordNode::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CWbClubStorageUseRecordNode::SetContent( std::string& strContent )
{
    if ( strContent.empty() )
        return;

    //m_pUseRecordContent->SetText( strContent.c_str(), NS_UITEXTCOLOR::WHITE );
    m_pUseRecordContent->ClearText();

    std::vector<sc::string::PARSE_RESULT> Split;
    sc::string::ParseMark(strContent, '[', ']', Split);
    int nLine = 0;
    for (size_t i=0; i<Split.size(); ++i)
    {
        D3DCOLOR TextColor = NS_UITEXTCOLOR::WHITE;
        if (Split[i].m_Mark)
            TextColor = NS_UITEXTCOLOR::GREENYELLOW;

        if (i == 0)
            nLine = m_pUseRecordContent->AddText(Split[i].m_Data.c_str(), TextColor);
        else
            m_pUseRecordContent->AddString(nLine, Split[i].m_Data.c_str(), TextColor);
    }
}

void CWbClubStorageUseRecordNode::SetDate( __time64_t LogTime )
{
    std::string strDate = sc::time::DateTimeFormat(LogTime, true, false);
    m_pUseRecordDate->SetText( strDate.c_str(), NS_UITEXTCOLOR::WHITE );
}

void CWbClubStorageUseRecordNode::ClearInfo()
{
    m_pUseRecordContent->ClearText();
    m_pUseRecordDate->ClearText();
}
