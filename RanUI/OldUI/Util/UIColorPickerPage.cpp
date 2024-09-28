#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../InnerInterface.h"

#include "./UIColorSelectorWindow.h"
#include "./UIColorPickerPage.h"
#include "./UIColorPickerGroup.h"
#include "./UITextButton.h"

//#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIColorPickerPage::CUIColorPickerPage( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIPage(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pPicker(NULL)
    , m_pLabel(NULL)
{
}

CUIColorPickerPage::~CUIColorPickerPage ()
{
}

void CUIColorPickerPage::CreateSubControl ()
{
    // 픽커
    {
        CUIColorPickerGroup* pControl = new CUIColorPickerGroup(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_PAGE_GROUP", UI_FLAG_DEFAULT, COLORPICKER_PAGE_GROUP );
        pControl->CreateSubControl();
        RegisterControl ( pControl );
        m_pPicker = pControl;
    }

    // 텍스트
    {
        CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pStaticText = NULL;
        
        pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub ( this, "COLORPICKER_PAGE_STATICTEXT_SELECTCOLOR", UI_FLAG_DEFAULT );
        pStaticText->SetFont ( pFont );
        pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
        pStaticText->SetOneLineText( ID2GAMEINTEXT("COLORSELECTOR_PICKER_PAGE_LABEL") );
        RegisterControl ( pStaticText );
        m_pLabel = pStaticText;
    }

    // 버튼
    {
        CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 12, _DEFAULT_FONT_FLAG );

        CUITextButton* pTextButton = NULL;

        pTextButton = new CUITextButton(m_pEngineDevice);
        pTextButton->CreateSub ( this, "COLORPICKER_PAGE_BUTTON_CANCEL", UI_FLAG_DEFAULT, COLORPICKER_PAGE_BUTTON_CANCEL );
        pTextButton->CreateSubControl( "COLORSELECTOR_WINDOW_BUTTON_DEFAULT", "COLORSELECTOR_WINDOW_BUTTON_CLICK", pFont );
        pTextButton->SetOneLineText( ID2GAMEWORD("COLORSELECTOR_PICKER_PAGE_BUTTON", 0) );
        RegisterControl ( pTextButton );
        m_pButton_Cancel = pTextButton;

        pTextButton = new CUITextButton(m_pEngineDevice);
        pTextButton->CreateSub ( this, "COLORPICKER_PAGE_BUTTON_OK", UI_FLAG_DEFAULT, COLORPICKER_PAGE_BUTTON_OK );
        pTextButton->CreateSubControl( "COLORSELECTOR_WINDOW_BUTTON_DEFAULT", "COLORSELECTOR_WINDOW_BUTTON_CLICK", pFont );
        pTextButton->SetOneLineText( ID2GAMEWORD("COLORSELECTOR_PICKER_PAGE_BUTTON", 1)  );
        RegisterControl ( pTextButton );
        m_pButton_Ok = pTextButton;
    }
}

void CUIColorPickerPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIColorPickerPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIPage::TranslateUIMessage ( ControlID, dwMsg );

    BOOL bUpdate = FALSE;

    switch ( ControlID )
    {
 
    case COLORPICKER_PAGE_GROUP:
        {
            if ( dwMsg & UIMSG_PICKER_UPDATE )
            {
                RequestPush( CUIColorSelectorWindow::PAGE_REQ_PICKER_UPDATE );
            }
        }
        break;

    case COLORPICKER_PAGE_BUTTON_CANCEL:
        {
            if ( CHECK_KEYFOCUSED ( dwMsg ) || dwMsg&UIMSG_UITEXTBUTTON_PUSHUP )
            {
                RequestPush( CUIColorSelectorWindow::PAGE_REQ_PICKER_SELECT_CANCEL );
            }
        }
        break;

    case COLORPICKER_PAGE_BUTTON_OK:
        {
            if ( CHECK_KEYFOCUSED ( dwMsg ) || dwMsg&UIMSG_UITEXTBUTTON_PUSHUP )
            {
                RequestPush( CUIColorSelectorWindow::PAGE_REQ_PICKER_SELECT_OK );
            }
        }
        break;
    };

    /*
    if ( bUpdate )
    {
        m_FuncUpdate( m_dwColor );
    }
    */
}

void CUIColorPickerPage::SetVisibleSingle( BOOL bVisible )
{
    CUIPage::SetVisibleSingle( bVisible );
}

void CUIColorPickerPage::SetRestrict( const BOOL bRestrict )
{
    m_pPicker->SetRestrict( bRestrict );

    if ( bRestrict )
    {
        m_pLabel->SetOneLineText( ID2GAMEINTEXT("COLORSELECTOR_PICKER_PAGE_LABEL_RESTRICT") );
        m_pLabel->SetTextColor( 0, dwRESTRICTCOLOR );
    }
    else
    {
        m_pLabel->SetOneLineText( ID2GAMEINTEXT("COLORSELECTOR_PICKER_PAGE_LABEL") );
        m_pLabel->SetTextColor( 0, dwDEFAULTCOLOR );
    }
}

void CUIColorPickerPage::SetColor( const DWORD dwColor )
{
    m_pPicker->SetColor( dwColor );
}

const DWORD CUIColorPickerPage::GetColor() 
{ 
    return m_pPicker->GetColor(); 
}
