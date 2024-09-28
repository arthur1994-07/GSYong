#include "stdafx.h"
#include "UIGroupHelperMessageBox.h"

#include "./MessageBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIGroupHelperMessageBox::CUIGroupHelperMessageBox( EngineDeviceMan* pEngineDevice )
    : CUIGroupHelper(pEngineDevice)
    , m_pMessageBox(NULL)
    , bOnMouse(0)
{
}

void CUIGroupHelperMessageBox::CreateMessageBox()
{
    CMessageBox* pMessageBox = new CMessageBox( m_pEngineDevice );
    pMessageBox->CreateSub( this, "UTILE_MESSAGEBOX_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_MESSAGEBOX );
    pMessageBox->CreateSubControl();
    RegisterControl( pMessageBox );
    m_pMessageBox = pMessageBox;


}

void CUIGroupHelperMessageBox::OpenMessageBox( const std::string& strTitle, const std::string& strMessage, const CMessageBox::BOXTYPE eBoxType /*= CMessageBox::TYPE_OKCANCEL*/, unsigned int nIdentity /*= 0 */ )
{
    if( m_pMessageBox == NULL)
    {
        GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
        return;
    }

    m_pMessageBox->OpenMessageBox( strTitle, strMessage, eBoxType, nIdentity );
    
}

void CUIGroupHelperMessageBox::OpenMessageBox( const CMessageBox::BOXTITLE eBoxTitle, const std::string& strMessage, const CMessageBox::BOXTYPE eBoxType /*= CMessageBox::TYPE_OKCANCEL*/, unsigned int nIdentity /*= 0 */ )
{
    if( m_pMessageBox == NULL)
    {
        GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
        return;
    }

    m_pMessageBox->OpenMessageBox( eBoxTitle, strMessage, eBoxType, nIdentity );
}

void CUIGroupHelperMessageBox::OpenMessageBoxMultiLine( const CMessageBox::BOXTITLE eBoxTitle, const std::vector<std::string>& vecMessage, const CMessageBox::BOXTYPE eBoxType /*= CMessageBox::TYPE_OKCANCEL*/, unsigned int nIdentity /*= 0 */ )
{
    if( m_pMessageBox == NULL)
    {
        GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
        return;
    }

    m_pMessageBox->OpenMessageBoxMultiLine( eBoxTitle, vecMessage, eBoxType, nIdentity );
}

void CUIGroupHelperMessageBox::OpenMessageBoxMultiLine( const std::string& strTitle, const std::vector<std::string>& vecMessage, const CMessageBox::BOXTYPE eBoxType /*= CMessageBox::TYPE_OKCANCEL*/, unsigned int nIdentity /*= 0 */ )
{
    if( m_pMessageBox == NULL)
    {
        GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
        return;
    }

    m_pMessageBox->OpenMessageBoxMultiLine( strTitle, vecMessage, eBoxType, nIdentity );
}

void CUIGroupHelperMessageBox::SetVisibleSingle( BOOL bVisible )
{
    CUIGroupHelper::SetVisibleSingle( bVisible );

    if( bVisible == FALSE )
        if( m_pMessageBox )
            m_pMessageBox->CloseMessageBox();
}

void CUIGroupHelperMessageBox::CloseMessageBox()
{
    if( m_pMessageBox )
        m_pMessageBox->CloseMessageBox();
}

void CUIGroupHelperMessageBox::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if( m_pMessageBox == NULL)
	{
		GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
		return;
	}

	if( m_pMessageBox->IsVisible() == FALSE )
	{
		CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}
	else
	{
		if( m_pMessageBox )
		{
			m_pMessageBox->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
			DWORD dwMsg = m_pMessageBox->GetMessageEx();
			TranslateMessageBox( m_pMessageBox->GetIdentifier(), dwMsg);
		}
	}
}

void CUIGroupHelperMessageBox::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    if( m_pMessageBox == NULL)
    {
        GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
        return;
    }

    if( ControlID == ID_MESSAGEBOX )
        bOnMouse = true;
    else
        bOnMouse = false;

    if( m_pMessageBox->IsVisible() == FALSE )
    {
        TranslateMessageBox( m_pMessageBox->GetIdentifier(), dwMsg);
        CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
    }
}

BOOL CUIGroupHelperMessageBox::IsOpenMessageBox()
{
    if( m_pMessageBox == NULL)
    {
        GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
        return false;
    }

    return m_pMessageBox->IsVisible();
}

BOOL CUIGroupHelperMessageBox::IsOnMouse()
{
    return bOnMouse;
}

std::string CUIGroupHelperMessageBox::GetInputString()
{
    if( m_pMessageBox == NULL)
    {
        GASSERT( 0 && "CreateMessageBox()를 호출하십시오.");
        return "";
    }

    return m_pMessageBox->GetInputString();
}
