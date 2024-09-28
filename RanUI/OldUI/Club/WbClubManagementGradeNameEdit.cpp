#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubManagementGradeNode.h"
#include "./WbClubManagementGradeNameEdit.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_6 = "계급 이름 편집";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_7 = "닫기";
}

const float CWbClubManagementGradeNameEdit::fGRADE_OFFSET = 5.0f;

CWbClubManagementGradeNameEdit::CWbClubManagementGradeNameEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pBack(NULL)
    , m_pCloseButton(NULL)
{
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_6 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 6);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_7 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 7);

    for ( int i = 0; i < club::AUTHORITY_GRADE; i++ )
        m_pGradeNode[i] = NULL;
}

CWbClubManagementGradeNameEdit::~CWbClubManagementGradeNameEdit()
{
}

void CWbClubManagementGradeNameEdit::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NAME_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pBack = pLinBoxSmart;
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NAME_EDIT_TITLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_6, NS_UITEXTCOLOR::WHITE );
    }

    {
        CWbClubManagementGradeNode* pGradeNode = new CWbClubManagementGradeNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pGradeNode->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NAME_EDIT_NODE", UI_FLAG_DEFAULT, GRADE_NODE_START );
        pGradeNode->CreateSubControl();
        RegisterControl( pGradeNode );
        m_pGradeNode[ 0 ] = pGradeNode;
        m_pGradeNode[ 0 ]->SetGradeData( 0 );

        for ( int i = 1; i < club::AUTHORITY_GRADE; i++ )
        {
            CWbClubManagementGradeNode* pGradeNode = new CWbClubManagementGradeNode( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            pGradeNode->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NAME_EDIT_NODE", UI_FLAG_DEFAULT, (GRADE_NODE_START+i) );
            pGradeNode->CreateSubControl();
            RegisterControl( pGradeNode );
            m_pGradeNode[ i ] = pGradeNode;
            m_pGradeNode[ i ]->SetGradeData( i );

            D3DXVECTOR2 vPos;
            vPos.x = m_pGradeNode[0]->GetLocalPos().left;
            vPos.y = m_pGradeNode[0]->GetLocalPos().top;

            vPos.y += (m_pGradeNode[0]->GetLocalPos().sizeY + fGRADE_OFFSET) * i;
            m_pGradeNode[i]->SetLocalPos( vPos );
        }
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
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_7;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NAME_CLOSE_BTN", UI_FLAG_DEFAULT, CLOSE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pCloseButton = pRnButton;

        D3DXVECTOR2 vPos;
        vPos.x = (GetLocalPos().sizeX - m_pCloseButton->GetLocalPos().sizeX)*0.5f;
        vPos.y = m_pGradeNode[club::AUTHORITY_GRADE-1]->GetLocalPos().bottom + fGRADE_OFFSET;
        m_pCloseButton->SetLocalPos( vPos );
    }

    {
        float fSizeY = m_pCloseButton->GetLocalPos().bottom + fGRADE_OFFSET;

        const UIRECT& rcOriginLP = m_pBack->GetLocalPos();
        UIRECT rcNew = UIRECT( rcOriginLP.left, rcOriginLP.top, rcOriginLP.sizeX, fSizeY );
        m_pBack->SetLocalPos( rcNew );
    }
}

void CWbClubManagementGradeNameEdit::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubManagementGradeNameEdit::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case CLOSE_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                SetVisibleSingle( FALSE );
            }
        }
        break;
    };
}

void CWbClubManagementGradeNameEdit::SetVisibleSingle( BOOL isVisible )
{
    if ( isVisible == TRUE )
    {
        SetGradeData();
    }

    CUIGroup::SetVisibleSingle( isVisible );
}

void CWbClubManagementGradeNameEdit::SetGradeData()
{
    for ( int i = 0; i < club::AUTHORITY_GRADE; i++ )
        m_pGradeNode[ i ]->SetGradeData( i );
}
