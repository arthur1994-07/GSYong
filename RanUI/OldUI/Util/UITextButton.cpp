#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "UITextButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUITextButton::CUITextButton(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pImage_Default(NULL)
    , m_pImage_Click(NULL)
    , m_pTextBox(NULL)
    , m_emMode(EMMODE_DEFAULT)
    , m_bDown(FALSE)
{
    // Blank
}

CUITextButton::~CUITextButton ()
{
    //Blank
}

void CUITextButton::CreateSubControl (  const char* szDefaultTexture, const char* szClickTexture, CD3DFontPar* pFont, CUITextButton::EMMODE emMode )
{	
    std::string strParentName = GetControlNameEx();

    CUIControl ParentControl(m_pEngineDevice); 
    ParentControl.Create ( 1, strParentName.c_str() );
    const UIRECT& rcParentLocal  = ParentControl.GetLocalPos();
    const UIRECT& rcParentGlobal = ParentControl.GetGlobalPos();

    // Note : 버튼 이미지
    {
        CSwapImage* pImage;
        pImage = new CSwapImage(m_pEngineDevice);
        pImage->CreateSub   ( this, strParentName.c_str(), UI_FLAG_LEFT | UI_FLAG_TOP, TEXTBUTTON_BUTTON );
        pImage->SetLocalPos ( D3DXVECTOR2(0.0f, 0.0f) );
        pImage->SetGlobalPos( D3DXVECTOR2(0.0f, 0.0f) );
        pImage->SetImage    ( szDefaultTexture );
        RegisterControl	    ( pImage );
        m_pImage_Default = pImage;

        pImage = new CSwapImage(m_pEngineDevice);
        pImage->CreateSub    ( this, strParentName.c_str(), UI_FLAG_LEFT | UI_FLAG_TOP );
        pImage->SetLocalPos  ( D3DXVECTOR2(0.0f, 0.0f) );
        pImage->SetGlobalPos ( D3DXVECTOR2(0.0f, 0.0f) );
        pImage->SetImage     ( szClickTexture );
        RegisterControl	     ( pImage );
        m_pImage_Click = pImage;
        m_pImage_Click->SetVisibleSingle( FALSE );
    }

    // Note : 텍스트
    {
        CBasicTextBox* pStaticText = NULL;

        pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub ( this, strParentName.c_str(), UI_FLAG_DEFAULT );
        pStaticText->SetLocalPos ( D3DXVECTOR2(0.0f, 0.0f) );
        pStaticText->SetGlobalPos( D3DXVECTOR2(0.0f, 0.0f) );
        pStaticText->SetFont ( pFont );
        pStaticText->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl ( pStaticText );
        m_pTextBox = pStaticText;
    }

    m_emMode = emMode;
}

void CUITextButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUITextButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

    if ( m_emMode == EMMODE_FLIP_MANUAL )
    {
        return;
    }

    switch ( m_emMode )
    {
    case EMMODE_DEFAULT:
        {
           UIMessageDefault ( ControlID, dwMsg );
        }
        break;

    case EMMODE_FLIP:
        {
            UIMessageFlip ( ControlID, dwMsg );
        }
        break;

    case EMMODE_FLIP_MANUAL:
        {
            // Empty
        }
        break;
    }
}

void CUITextButton::UIMessageDefault ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case TEXTBUTTON_BUTTON:
        {
            if ( dwMsg&UIMSG_LB_UP )
            {
                if ( m_bDown && CHECK_MOUSE_IN ( dwMsg ) )
                {
                    AddMessageEx( UIMSG_UITEXTBUTTON_PUSHUP );
                }

                Click ( FALSE );
                m_bDown = FALSE;
            }
            else if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( dwMsg&UIMSG_LB_DOWN )
                {
                    Click ( TRUE );
                    m_bDown = TRUE;
                }
                else if ( m_bDown && dwMsg&(UIMSG_LB_PRESEED|UIMSG_LB_DRAG) )
                {
                    Click ( TRUE );
                }
                else
                {
                    Click ( FALSE );
                    m_bDown = FALSE;
                }
            }
            else
            {
                Click ( FALSE );
            }
        }
        break;
    };
}

void CUITextButton::UIMessageFlip ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case TEXTBUTTON_BUTTON:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) && (dwMsg&UIMSG_LB_DOWN) )
            {
                AddMessageEx( UIMSG_UITEXTBUTTON_PUSHUP );
            }

            Toggle();
        }
        break;
    };
}

void CUITextButton::SetOneLineText ( const CString& strText, const D3DCOLOR& TextColor )
{
    CString strVisible = strText;
    m_strText = strText;

    if( FALSE == m_strText.IsEmpty() )
    {
        SIZE sizeText;
        m_pTextBox->GetFont()->GetTextExtent( m_strText.GetString(), sizeText );

        // 문자열의 길이가 버튼보다 클경우 말줄임 표시를 쓰도록 한다.
        if( m_pTextBox->GetLocalPos().sizeX <= sizeText.cx )
        {
            int nTotalLength = m_strText.GetLength();
            int nOneLengthSize = sizeText.cx / nTotalLength;

            int nMaxLength = (int) ( m_pTextBox->GetLocalPos().sizeX / nOneLengthSize );
            nMaxLength -= 3;

            strVisible.Format( _T( "%s..." ), m_strText.Left( nMaxLength ) );
        }
        else
        {
            strVisible = m_strText;
        }
    }

    m_pTextBox->SetOneLineText( strVisible, TextColor );
}

void CUITextButton::Click ( const bool bEnable )
{
    if ( bEnable )
    {
        m_pImage_Click->SetVisibleSingle( TRUE );
    }
    else
    {
        m_pImage_Click->SetVisibleSingle( FALSE );
    }
}

void CUITextButton::Toggle ()
{
    if ( m_pImage_Click->IsVisible() )
    {
        Click( FALSE );
    }
    else
    {
        Click( TRUE );
    }
}
