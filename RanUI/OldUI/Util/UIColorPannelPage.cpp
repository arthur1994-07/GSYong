#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogic/GLColorTable.h"

#include "./UIColorSelectorWindow.h"
#include "./UIColorPannelPage.h"
#include "./UITextButton.h"

#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIColorPannelPage::CUIColorPannelPage( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIPage(pEngineDevice)
    , m_pInterface(pInterface)
    , m_nSelectGroup ( CUIColorPannelPage::SELECT_NULL )
    , m_nSelectPannel( CUIColorPannelPage::SELECT_NULL )
    , m_nMaxPannelCount(0)
    , m_bRestrict(FALSE)
{
}

CUIColorPannelPage::~CUIColorPannelPage ()
{
}

void CUIColorPannelPage::CreateSubControl ( const UI::SPANNELGROUPINFOARRAY& sInfoArray )
{
    DWORD dwMaxSize = (DWORD)sInfoArray.size();

    if ( dwMaxSize == 0 )
    {
        return;
    }

    for ( DWORD i=0; i<dwMaxSize; ++i )
    {
        AddPannelGroup( sInfoArray[i] );
    }

    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, "COLORPANNEL_PAGE_BASEGROUP" );
    const UIRECT& rcBaseLocal = BaseControl.GetLocalPos();

    FLOAT fExpandHeight = (rcBaseLocal.sizeY + (float)dwPANNELGROUPGAP_Y) * (float)dwMaxSize;
    FLOAT fExpandWidth  = (float)((m_nMaxPannelCount-1)*dwPANNELEXPANDSIZE_X);

    // ¹öÆ°
    {
        CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        const int nBUTTONSIZE = CBasicTextButton::SIZE14;

        CUITextButton* pTextButton = NULL;

        pTextButton = new CUITextButton(m_pEngineDevice);
        pTextButton->CreateSub ( this, "COLORPANNEL_PAGE_BUTTON_RANDOM", UI_FLAG_DEFAULT, COLORPANNEL_PAGE_BUTTON_RANDOM );
        pTextButton->CreateSubControl( "COLORSELECTOR_WINDOW_BUTTON_DEFAULT", "COLORSELECTOR_WINDOW_BUTTON_CLICK", pFont );
        pTextButton->SetOneLineText( ID2GAMEWORD("COLORSELECTOR_PANNEL_PAGE_BUTTON", 0) );
        pTextButton->AddPosition( D3DXVECTOR2(0.0f, fExpandHeight) );
        RegisterControl ( pTextButton );
        m_pButton_Random = pTextButton;

        pTextButton = new CUITextButton(m_pEngineDevice);
        pTextButton->CreateSub ( this, "COLORPANNEL_PAGE_BUTTON_OK", UI_FLAG_DEFAULT, COLORPANNEL_PAGE_BUTTON_OK );
        pTextButton->CreateSubControl( "COLORSELECTOR_WINDOW_BUTTON_DEFAULT", "COLORSELECTOR_WINDOW_BUTTON_CLICK", pFont );
        pTextButton->SetOneLineText( ID2GAMEWORD("COLORSELECTOR_PANNEL_PAGE_BUTTON", 3) );
        pTextButton->AddPosition( D3DXVECTOR2(0.0f, fExpandHeight) );
        RegisterControl ( pTextButton );
        m_pButton_Ok = pTextButton;

        pTextButton = new CUITextButton(m_pEngineDevice);
        pTextButton->CreateSub ( this, "COLORPANNEL_PAGE_BUTTON_RESET", UI_FLAG_DEFAULT, COLORPANNEL_PAGE_BUTTON_RESET );
        pTextButton->CreateSubControl( "COLORSELECTOR_WINDOW_BUTTON_DEFAULT", "COLORSELECTOR_WINDOW_BUTTON_CLICK", pFont );
        pTextButton->SetOneLineText( ID2GAMEWORD("COLORSELECTOR_PANNEL_PAGE_BUTTON", 1) );
        pTextButton->AddPosition( D3DXVECTOR2(fExpandWidth, fExpandHeight) );
        RegisterControl ( pTextButton );
        m_pButton_Reset = pTextButton;

        pTextButton = new CUITextButton(m_pEngineDevice);
        pTextButton->CreateSub ( this, "COLORPANNEL_PAGE_BUTTON_CANCEL", UI_FLAG_DEFAULT, COLORPANNEL_PAGE_BUTTON_CANCEL );
        pTextButton->CreateSubControl( "COLORSELECTOR_WINDOW_BUTTON_DEFAULT", "COLORSELECTOR_WINDOW_BUTTON_CLICK", pFont );
        pTextButton->SetOneLineText( ID2GAMEWORD("COLORSELECTOR_PANNEL_PAGE_BUTTON", 2) );
        pTextButton->AddPosition( D3DXVECTOR2(fExpandWidth, fExpandHeight) );
        RegisterControl ( pTextButton );
        m_pButton_Cancel = pTextButton;

    }

    m_rcPage = GetLocalPos();

    SelectPannel(0,0);
}

void CUIColorPannelPage::AddPannelGroup ( const UI::SCOLORPANNELGROUP_INFO& sInfo )
{
    if ( UI::SCOLORPANNELGROUP_INFO::MAX_PANNEL < sInfo.dwNUMPANNEL )
    {
        return;
    }

    const UIRECT& rcParentLocal  = GetLocalPos();
    const UIRECT& rcParentGlobal = GetGlobalPos();

    CUIControl BaseControl(m_pEngineDevice); 
    BaseControl.Create ( 1, "COLORPANNEL_PAGE_BASEGROUP" );
    const UIRECT& rcBaseLocal = BaseControl.GetLocalPos();

    SPANNELGROUP sPannelGroup;
    DWORD dwIDIndex = (DWORD)m_PannelGroupArray.size() * CUIColorPannelPage::MAX_PANNEL;

    FLOAT fBeginLeft = rcBaseLocal.left;
    FLOAT fBeginTop  = rcBaseLocal.top + (float)m_PannelGroupArray.size() * (rcBaseLocal.sizeY + (float)dwPANNELGROUPGAP_Y);

    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    CBasicTextBox* pStaticText = NULL;
    pStaticText = new CBasicTextBox(m_pEngineDevice);
    pStaticText->CreateSub      ( this, "COLORPANNEL_PAGE_LABEL", UI_FLAG_DEFAULT );
    pStaticText->SetFont        ( pFont );
    pStaticText->SetTextAlign   ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
    RegisterControl ( pStaticText );
    sPannelGroup.pLABEL = pStaticText;

    const UIRECT& rcLabelLocal = sPannelGroup.pLABEL->GetLocalPos();
    sPannelGroup.pLABEL->SetLocalPos(  D3DXVECTOR2(fBeginLeft + rcLabelLocal.left, fBeginTop + rcLabelLocal.top) );
    sPannelGroup.pLABEL->SetGlobalPos( D3DXVECTOR2(fBeginLeft + rcLabelLocal.left + rcParentGlobal.left, fBeginTop + rcLabelLocal.top + rcParentGlobal.top) );

    if ( sInfo.bENABLE )
    {
        pStaticText->SetOneLineText ( sInfo.strLABEL.c_str(),NS_UITEXTCOLOR::GOLD );
    }
    else
    {
        pStaticText->SetOneLineText ( sInfo.strLABEL.c_str(),NS_UITEXTCOLOR::DARKGRAY );
    }

    if ( m_nMaxPannelCount < sInfo.dwNUMPANNEL )
    {
        m_nMaxPannelCount = sInfo.dwNUMPANNEL;
    }

    for ( DWORD i=0; i<sInfo.dwNUMPANNEL; ++i )
    {
        std::string strColorKeyword = sc::string::format("%1%%2%", "COLORPANNEL_PAGE_COLOR_", i);

        CUIControl* pControl = NULL;     
        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, strColorKeyword.c_str(), UI_FLAG_DEFAULT, COLORPANNEL_PAGE_PANNEL_BEGIN + dwIDIndex+i );
        pControl->SetUseRender( TRUE );   
        pControl->SetDiffuse(0);
        RegisterControl ( pControl );
        sPannelGroup.pCOLOR[i] = pControl;

        const UIRECT& rcColorLocal = sPannelGroup.pCOLOR[i]->GetLocalPos();
        sPannelGroup.pCOLOR[i]->SetLocalPos ( D3DXVECTOR2(fBeginLeft + rcColorLocal.left, fBeginTop + rcColorLocal.top) );
        sPannelGroup.pCOLOR[i]->SetGlobalPos( D3DXVECTOR2(fBeginLeft + rcColorLocal.left + rcParentGlobal.left, fBeginTop + rcColorLocal.top + rcParentGlobal.top) );

        std::string strPannelKeyword        = sc::string::format("%1%%2%", "COLORPANNEL_PAGE_", i);
        std::string strPannelTextureKeyword = sc::string::format("%1%%2%", "COLORPANNEL_PAGE_INACTIVE_", i);

        CSwapImage* pSwapImage = NULL;
        pSwapImage = new CSwapImage(m_pEngineDevice);
        pSwapImage->CreateSub ( this, strPannelKeyword.c_str(), UI_FLAG_DEFAULT );
        pSwapImage->SetImage  ( strPannelTextureKeyword.c_str() );
        RegisterControl ( pSwapImage );
        sPannelGroup.pPANNEL[i] = pSwapImage;

        const UIRECT& rcPannelLocal = sPannelGroup.pPANNEL[i]->GetLocalPos();
        sPannelGroup.pPANNEL[i]->SetLocalPos ( D3DXVECTOR2(fBeginLeft + rcPannelLocal.left, fBeginTop + rcPannelLocal.top) );
        sPannelGroup.pPANNEL[i]->SetGlobalPos( D3DXVECTOR2(fBeginLeft + rcPannelLocal.left + rcParentGlobal.left, fBeginTop + rcPannelLocal.top + rcParentGlobal.top) );
    }

    sPannelGroup.dwNUMPANNEL = sInfo.dwNUMPANNEL;
    sPannelGroup.bENABLE     = sInfo.bENABLE;

    m_PannelGroupArray.push_back ( sPannelGroup );

    CUIControl PageControl(m_pEngineDevice); 
    PageControl.Create ( 1, "COLORSELECTOR_WINDOW_PAGE_PANNEL" );
    const UIRECT& rcPageLocal = PageControl.GetLocalPos();

    FLOAT fPageSizeX = rcPageLocal.sizeX + (float)((m_nMaxPannelCount-1)*dwPANNELEXPANDSIZE_X);
    SetLocalPos  ( UIRECT(rcParentLocal.left,  rcParentLocal.top,  fPageSizeX, rcParentLocal.sizeY  + rcBaseLocal.sizeY + (float)dwPANNELGROUPGAP_Y));
    SetGlobalPos ( UIRECT(rcParentGlobal.left, rcParentGlobal.top, fPageSizeX, rcParentGlobal.sizeY + rcBaseLocal.sizeY + (float)dwPANNELGROUPGAP_Y));
}

void CUIColorPannelPage::OpenSubControl ()
{
    //Blank
}

void CUIColorPannelPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIColorPannelPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIPage::TranslateUIMessage ( ControlID, dwMsg );

    int nPannelID   = ControlID - COLORPANNEL_PAGE_PANNEL_BEGIN;
    int nGroupIndex = nPannelID/CUIColorPannelPage::MAX_PANNEL;
    int nMaxGroup   = (int)m_PannelGroupArray.size();

    if ( 0 <= nPannelID && nGroupIndex < nMaxGroup )
    {
        if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
        {
            int nPannelIndex = nPannelID%CUIColorPannelPage::MAX_PANNEL;
            SPANNELGROUP& sGroup = m_PannelGroupArray[nGroupIndex];

            if ( sGroup.bENABLE )
            {
                SelectPannel( nGroupIndex, nPannelIndex );
                RequestPush( CUIColorSelectorWindow::PAGE_REQ_PANNEL_SELECT );
            }
        }
    }
    else
    {
        switch ( ControlID )
        {
        case COLORPANNEL_PAGE_BUTTON_RANDOM:
            {
                if ( CHECK_KEYFOCUSED ( dwMsg ) || ( dwMsg&UIMSG_UITEXTBUTTON_PUSHUP ) )
                {
                    RandomAllPannelColor();
                    RequestPush( CUIColorSelectorWindow::PAGE_REQ_PANNEL_RANDOM );
                }
            }
            break;

        case COLORPANNEL_PAGE_BUTTON_RESET:
            {
                if ( CHECK_KEYFOCUSED ( dwMsg ) || ( dwMsg&UIMSG_UITEXTBUTTON_PUSHUP ) )
                {
                    RequestPush( CUIColorSelectorWindow::PAGE_REQ_PANNEL_RESET );
                }
            }
            break;

        case COLORPANNEL_PAGE_BUTTON_OK:
            {
                if ( CHECK_KEYFOCUSED ( dwMsg ) || ( dwMsg&UIMSG_UITEXTBUTTON_PUSHUP ) )
                {
                    RequestPush( CUIColorSelectorWindow::PAGE_REQ_PANNEL_OK );
                }
            }
            break;

        case COLORPANNEL_PAGE_BUTTON_CANCEL:
            {
                if ( CHECK_KEYFOCUSED ( dwMsg ) || ( dwMsg&UIMSG_UITEXTBUTTON_PUSHUP ) )
                {
                    RequestPush( CUIColorSelectorWindow::PAGE_REQ_PANNEL_CANCEL );
                }
            }
            break;
        };
    }
}

void CUIColorPannelPage::SetVisibleSingle( BOOL bVisible )
{
    CUIPage::SetVisibleSingle( bVisible );

    if ( bVisible )
    {

    }
}


void CUIColorPannelPage::SetAttribute ( const UI::SPANNELGROUPINFOARRAY& sInInfoArray )
{
    int nGroupSize = (int)m_PannelGroupArray.size();
    int nInfoSize  = (int)sInInfoArray.size();

    if ( nGroupSize != nInfoSize )
    {
        return;
    }

    for ( int i=0; i<nInfoSize; ++i )
    {
        const UI::SCOLORPANNELGROUP_INFO& sInfo  = sInInfoArray[i];
        SPANNELGROUP&                     sGroup = m_PannelGroupArray[i];

        sGroup.bENABLE = sInfo.bENABLE;

        if ( sGroup.bENABLE )
        {
            sGroup.pLABEL->SetOneLineText( sInfo.strLABEL.c_str(), NS_UITEXTCOLOR::GOLD );
        }
        else
        {
            sGroup.pLABEL->SetOneLineText( sInfo.strLABEL.c_str(), NS_UITEXTCOLOR::DARKGRAY );
        }

        for ( DWORD j=0; j<sGroup.dwNUMPANNEL; ++j )
        {
            if ( sGroup.bENABLE )
            {
                sGroup.pCOLOR[j]->SetDiffuse( sInfo.dwCOLOR[j] );
            }
            else
            {
                sGroup.pCOLOR[j]->SetDiffuse( CUIColorPannelPage::COLOR_NULL );
            }
        }
    }
}

void CUIColorPannelPage::GetAttribute( UI::SPANNELGROUPINFOARRAY& sOutInfoArray )
{
    int nGroupSize = (int)m_PannelGroupArray.size();
    int nInfoSize  = (int)sOutInfoArray.size();

    if ( nGroupSize != nInfoSize )
    {
        return;
    }

    for ( int i=0; i<nInfoSize; ++i )
    {
        UI::SCOLORPANNELGROUP_INFO& sInfo  = sOutInfoArray[i];
        SPANNELGROUP&               sGroup = m_PannelGroupArray[i];

        sInfo.bENABLE     = sGroup.bENABLE;
        sInfo.dwNUMPANNEL = sGroup.dwNUMPANNEL;
        sInfo.strLABEL    = sGroup.pLABEL->GetText(0).GetString();

        for ( DWORD j=0; j<sGroup.dwNUMPANNEL; ++j )
        {
            if ( sGroup.bENABLE )
            {
                sInfo.dwCOLOR[j] = sGroup.pCOLOR[j]->GetDiffuse();
            }
            else
            {
                sInfo.dwCOLOR[j] = CUIColorPannelPage::COLOR_NULL;
            }
        }
    }
}

void CUIColorPannelPage::SetSelectColor( const DWORD dwColor )
{
    if ( m_nSelectGroup == CUIColorPannelPage::SELECT_NULL || m_nSelectPannel == CUIColorPannelPage::SELECT_NULL )
    {
        return;
    }

    SPANNELGROUP& sGroup = m_PannelGroupArray[m_nSelectGroup];
    sGroup.pCOLOR[m_nSelectPannel]->SetDiffuse( dwColor );
}

const DWORD CUIColorPannelPage::GetSelectColor()
{
    if ( m_nSelectGroup == CUIColorPannelPage::SELECT_NULL || m_nSelectPannel == CUIColorPannelPage::SELECT_NULL )
    {
        return CUIColorPannelPage::COLOR_NULL;
    }

    SPANNELGROUP& sGroup = m_PannelGroupArray[m_nSelectGroup];
    DWORD dwColor = sGroup.pCOLOR[m_nSelectPannel]->GetDiffuse();

    return dwColor;
}


void CUIColorPannelPage::SelectPannel ( const int nGroup, const int nIndex )
{
    int nMaxGroup = (int)m_PannelGroupArray.size();

    if ( nGroup < 0 || nMaxGroup <= nGroup )
    {
        return;
    }

    if ( nIndex < 0 || CUIColorPannelPage::MAX_PANNEL <= nIndex )
    {
        return;
    }

    if ( nGroup == m_nSelectGroup && nIndex == m_nSelectPannel )
    {
        return;
    }

    if ( m_nSelectGroup != CUIColorPannelPage::SELECT_NULL && m_nSelectPannel != CUIColorPannelPage::SELECT_NULL )
    {
        int nPreGroupIndex  = m_nSelectGroup;
        int nPrePannelIndex = m_nSelectPannel;

        SPANNELGROUP& sPreGroup = m_PannelGroupArray[nPreGroupIndex];
        std::string strPrePannelTextureKeyword = sc::string::format("%1%%2%", "COLORPANNEL_PAGE_INACTIVE_", nPrePannelIndex);
        sPreGroup.pPANNEL[nPrePannelIndex]->SetImage ( strPrePannelTextureKeyword.c_str() );
    }

    SPANNELGROUP& sGroup = m_PannelGroupArray[nGroup];
    std::string strPannelTextureKeyword = sc::string::format("%1%%2%", "COLORPANNEL_PAGE_ACTIVE_", nIndex);
    sGroup.pPANNEL[nIndex]->SetImage ( strPannelTextureKeyword.c_str() );

    m_nSelectGroup  = nGroup;
    m_nSelectPannel = nIndex;
}

void CUIColorPannelPage::UnselectAllPannel ()
{
    SPANNELGROUPARRAY_ITER iter = m_PannelGroupArray.begin();

    for ( ; iter != m_PannelGroupArray.end(); ++iter )
    {
        SPANNELGROUP& sGroup = (*iter);

        for ( DWORD i=0; i<sGroup.dwNUMPANNEL; ++i )
        {
            std::string strPannelTextureKeyword = sc::string::format("%1%%2%", "COLORPANNEL_PAGE_INACTIVE_", i);
            sGroup.pPANNEL[i]->SetImage ( strPannelTextureKeyword.c_str() );
        }
    }

    m_nSelectGroup  = CUIColorPannelPage::SELECT_NULL;
    m_nSelectPannel = CUIColorPannelPage::SELECT_NULL;
}

void CUIColorPannelPage::RandomAllPannelColor()
{
    SPANNELGROUPARRAY_ITER iter = m_PannelGroupArray.begin();

    const int nColorMin = 0;
    const int nColorMax = 255;

    for ( ; iter != m_PannelGroupArray.end(); ++iter )
    {
        SPANNELGROUP& sGroup = (*iter);

        if ( sGroup.bENABLE )
        {
            for ( DWORD i=0; i<sGroup.dwNUMPANNEL; ++i )
            {
                DWORD dwColor = GetRandomColor();

                sGroup.pCOLOR[i]->SetDiffuse( dwColor );
            }
        }
    }
}

const DWORD CUIColorPannelPage::GetRandomColor()
{
    DWORD dwColor = 0;

    if ( m_bRestrict )
    {
        DWORD dwRestrictSize = (DWORD)GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT.size();        
        dwColor = GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT[ rand()%dwRestrictSize ];
    }
    else
    {
        dwColor = D3DCOLOR_ARGB ( 255, (rand()%0x100), (rand()%0x100), (rand()%0x100) );
    }

    return dwColor;
}

void CUIColorPannelPage::SetRestrict ( const BOOL bRestrict )
{
    m_bRestrict = bRestrict;
}

/*
void CUIColorPannelPage::SetColor ( const COLORARRAY& InColor )
{
    int nPannelSize = (int)InColor.size();

    if ( nPannelSize == 0 )
    {
        return;
    }

    SPANNELGROUPARRAY_ITER iter   = m_PannelGroupArray.begin();
    int                    nIndex = 0;

    for ( ; iter != m_PannelGroupArray.end(); ++iter )
    {
        SPANNELGROUP& sGroup = (*iter);

        if ( !sGroup.bENABLE )
        {
            for ( DWORD i=0; i<sGroup.dwNUMPANNEL; ++i )
            {
                sGroup.pCOLOR[i]->SetDiffuse( CUIColorPannelPage::COLOR_NULL );
            }

            nIndex += sGroup.dwNUMPANNEL;
            continue;
        }

        for ( DWORD i=0; i<sGroup.dwNUMPANNEL; ++i )
        {
            sGroup.pCOLOR[i]->SetDiffuse( InColor[nIndex++] );

            if ( nPannelSize <= nIndex )
            {
                return;
            }
        }
    }
}

void CUIColorPannelPage::GetColor ( COLORARRAY& OutColor )
{
    int nPannelSize = GetNumColor();

    if ( nPannelSize == 0 )
    {
        return;
    }

    if ( OutColor.size() != nPannelSize )
    {
        OutColor.resize(nPannelSize);
    }

    SPANNELGROUPARRAY_ITER iter   = m_PannelGroupArray.begin();
    int                    nIndex = 0;

    for ( ; iter != m_PannelGroupArray.end(); ++iter )
    {
        SPANNELGROUP& sGroup = (*iter);

        if ( !sGroup.bENABLE )
        {
            for ( DWORD i=0; i<sGroup.dwNUMPANNEL; ++i )
            {
                OutColor[nIndex++] = 0;
            }

            continue;
        }

        for ( DWORD i=0; i<sGroup.dwNUMPANNEL; ++i )
        {
            OutColor[nIndex++] = sGroup.pCOLOR[i]->GetDiffuse();
        }
    }
}

const int CUIColorPannelPage::GetNumColor()
{
    SPANNELGROUPARRAY_ITER iter   = m_PannelGroupArray.begin();
    int                    nIndex = 0;

    for ( ; iter != m_PannelGroupArray.end(); ++iter )
    {
        SPANNELGROUP& sGroup = (*iter);
        nIndex += sGroup.dwNUMPANNEL;
    }

    return nIndex;
}

const int CUIColorPannelPage::GetSelectColorIndex()
{
    if ( m_nSelectPannel == CUIColorPannelPage::SELECT_NULL )
    {
        return CUIColorPannelPage::SELECT_NULL;
    }

    SPANNELGROUPARRAY_ITER iter        = m_PannelGroupArray.begin();
    int                    nCurIndex   = 0;
    int                    nColorIndex = 0;

    for ( ; iter != m_PannelGroupArray.end(); ++iter )
    {
        SPANNELGROUP& sGroup = (*iter);

        if ( nCurIndex + CUIColorPannelPage::MAX_PANNEL < m_nSelectPannel )
        {
            nColorIndex += sGroup.dwNUMPANNEL;
            nCurIndex   += CUIColorPannelPage::MAX_PANNEL;
        }
        else
        {
            return nColorIndex + m_nSelectPannel - nCurIndex;
        }
    }

    return nColorIndex;
}
*/