
#include "stdafx.h"

#include "RnButton.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../EngineLib/DxTools/DxFontMan.h"

RnButton::CreateArg::CreateArg()
{
    pFont = NULL;
    dwColor = NS_UITEXTCOLOR::ENABLE;
    nTextAlign = (TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);
}

RnButton::RnButton(EngineDeviceMan* pEngineDevice) : CUIGroup(pEngineDevice)
{
	m_PressedEventTime = 0;
	m_PressedEventTimeAccumulated = 0;

	for (int i=0; i < COUNT; ++i)
	{
		m_pButtonImage[i] = NULL;
	}

	m_isPressed = false;
	m_isEnable = true;
}

void RnButton::CreateSubControl( const CreateArg& arg )
{
	GASSERT( !arg.defaultTextureKey.empty() );
	m_pButtonImage[DEFAULT] = new CBasicLineBoxSmart(m_pEngineDevice);
	m_pButtonImage[DEFAULT]->CreateSub( this, GetControlNameEx().c_str(),UI_FLAG_DEFAULT,WND_DEFAULT_ID);
	m_pButtonImage[DEFAULT]->CreateSubControl( arg.defaultTextureKey.c_str() );
	m_pButtonImage[DEFAULT]->SetLocalPos(D3DXVECTOR2(0,0));
	m_pButtonImage[DEFAULT]->SetGlobalPos(D3DXVECTOR2(0,0));
	RegisterControl( m_pButtonImage[DEFAULT] );

	std::string mouseOverTextureKey = arg.mouseOverTextureKey;
	if(mouseOverTextureKey.empty())
		mouseOverTextureKey = arg.defaultTextureKey;

	m_pButtonImage[MOUSEOVER]  = new CBasicLineBoxSmart(m_pEngineDevice);
	m_pButtonImage[MOUSEOVER]->CreateSub( this, GetControlNameEx().c_str(),UI_FLAG_DEFAULT,WND_MOUSEOVER_ID);
	m_pButtonImage[MOUSEOVER]->CreateSubControl( mouseOverTextureKey.c_str() );
	m_pButtonImage[MOUSEOVER]->SetLocalPos(D3DXVECTOR2(0,0));
	m_pButtonImage[MOUSEOVER]->SetGlobalPos(D3DXVECTOR2(0,0));

	RegisterControl( m_pButtonImage[MOUSEOVER] );

	std::string mouseClickTextureKey = arg.mouseClickTextureKey;
	if(mouseClickTextureKey.empty())
		mouseClickTextureKey = arg.defaultTextureKey;

	m_pButtonImage[MOUSECLICK] = new CBasicLineBoxSmart(m_pEngineDevice);
	m_pButtonImage[MOUSECLICK]->CreateSub( this, GetControlNameEx().c_str(),UI_FLAG_DEFAULT,WND_MOUSECLICK_ID);
	m_pButtonImage[MOUSECLICK]->CreateSubControl( mouseClickTextureKey.c_str() );
	m_pButtonImage[MOUSECLICK]->SetLocalPos(D3DXVECTOR2(0,0));
	m_pButtonImage[MOUSECLICK]->SetGlobalPos(D3DXVECTOR2(0,0));

	RegisterControl( m_pButtonImage[MOUSECLICK] );


	std::string inactiveTextureKey = arg.inactiveTextureKey;
	if(inactiveTextureKey.empty())
		inactiveTextureKey = arg.defaultTextureKey;

	m_pButtonImage[INACTIVE] = new CBasicLineBoxSmart(m_pEngineDevice);
	m_pButtonImage[INACTIVE]->CreateSub( this, GetControlNameEx().c_str(),UI_FLAG_DEFAULT,WND_INACTIVE_ID);
	m_pButtonImage[INACTIVE]->CreateSubControl( inactiveTextureKey.c_str() );
	m_pButtonImage[INACTIVE]->SetLocalPos(D3DXVECTOR2(0,0));
	m_pButtonImage[INACTIVE]->SetGlobalPos(D3DXVECTOR2(0,0));

	RegisterControl( m_pButtonImage[INACTIVE] );

	if(!arg.imageKey.empty())
	{
		m_pAboveImage = new CUIControl(m_pEngineDevice);
		m_pAboveImage->CreateSub(this,arg.imageKey.c_str());
	}

	if(!arg.text.empty())
	{
		CD3DFontPar* pFont = arg.pFont;
		if( pFont == NULL)
		{
			pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		}

		m_pText = new CBasicTextBox(m_pEngineDevice);
		m_pText->CreateSub( this, GetControlNameEx().c_str(), UI_FLAG_DEFAULT );
		m_pText->SetFont( pFont );
		m_pText->SetTextAlign( arg.nTextAlign );
		m_pText->SetText(arg.text.c_str(), arg.dwColor);
		m_pText->SetLocalPos(D3DXVECTOR2(0,0));
		m_pText->SetGlobalPos(D3DXVECTOR2(0,0));

		RegisterControl( m_pText );
	}

	SetEnable(true);
	SetState(DEFAULT);
}

void RnButton::SetState( STATE state )
{
	for (int i=0; i < COUNT; ++i)
	{
		m_pButtonImage[i]->SetVisibleSingle(FALSE);
	}

	m_pButtonImage[state]->SetVisibleSingle(TRUE);
}

void RnButton::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

	CUIGroup::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	GASSERT( m_PressedEventTime >= 0 );

	// 	CUIGroup::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl); 이것후에 해야 적용됨
	const float LAZY_TIME = 1.0f;

	if( m_isPressed )
	{
		if( m_PressedEventTime )
		{
			m_PressedEventTimeAccumulated += fElapsedTime;
			if( m_PressedEventTimeAccumulated > LAZY_TIME)
			{
				if( m_PressedEventTime + LAZY_TIME <= m_PressedEventTimeAccumulated )
				{
					m_PressedEventTimeAccumulated -= m_PressedEventTime;
					AddMessageEx( UIMSG_RNBUTTON_EVENT_PRESSED );
				}
			}
		}
	}
	else
	{
		m_PressedEventTimeAccumulated = 0;
	}

}

void RnButton::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if( m_isEnable == false )
		return;

	switch ( ControlID )
	{
	case WND_INACTIVE_ID:
		{
			SetState(INACTIVE);			
			break;
		}
	case WND_DEFAULT_ID:
	case WND_MOUSEOVER_ID:
	case WND_MOUSECLICK_ID:
		{
			if ( dwMsg&UIMSG_LB_UP )
			{
				if ( m_isPressed && CHECK_MOUSE_IN ( dwMsg ) )
				{
					AddMessageEx( UIMSG_RNBUTTON_CLICK );
				}

				//Click ( FALSE );
				m_isPressed = false;
				SetState(DEFAULT);
			}
			else if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg&UIMSG_LB_DOWN )
				{
//					Click ( TRUE );
					m_isPressed = true;
					SetState(MOUSECLICK);
				}
				else if ( m_isPressed && (dwMsg&(UIMSG_LB_PRESEED|UIMSG_LB_DRAG)) )
				{
//					Click ( TRUE );
					SetState(MOUSECLICK);
				}
				else
				{
//					Click ( FALSE );
					m_isPressed = false;
					SetState(MOUSEOVER);
				}
			}
			else
			{
				if( m_isPressed )
					SetState(MOUSECLICK);
				else
					SetState(DEFAULT);
			}
		
			break;
		}
	}
}

void RnButton::SetEnable( bool isEnable )
{
	m_isEnable = isEnable;
	if( m_isEnable )
		SetState(DEFAULT);
	else
		SetState(INACTIVE);
}

bool RnButton::IsState( STATE state )
{
    if ( state < 0 || state >= COUNT )
        return false;

    if ( m_pButtonImage[state]->IsVisible() )
        return true;

    return false;
}

void RnButton::SetText( const CString& strText, const D3DCOLOR& TextColor )
{
	if( m_pText )
		m_pText->SetText( strText, TextColor );
}

void RnButton::SetOneLineText( const CString& strText, const D3DCOLOR& TextColor )
{
	if( m_pText )
		m_pText->SetOneLineText( strText, TextColor );
}

std::string RnButton::GetText()
{
	return LPCSTR(m_pText->GetText(0));
}
