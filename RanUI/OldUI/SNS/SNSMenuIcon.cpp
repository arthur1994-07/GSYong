
// bjju.sns

#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "./SNSMenuIcon.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSNSMenuIcon::CSNSMenuIcon(EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pImage		( NULL )
	, m_pText		( NULL )
	, m_bEnable		( TRUE )
{
	//  Blank
}

CSNSMenuIcon::~CSNSMenuIcon ()
{
	//  Blank
}

void CSNSMenuIcon::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nAlignLeft         = TEXT_ALIGN_LEFT;
	const int nAlignLeftCenterX  = TEXT_ALIGN_CENTER_X;	

	// Note : 이미지
	{
		CSwapImage* pImage;
		m_pImage = pImage = new CSwapImage(m_pEngineDevice);
		pImage->CreateSub  ( this, "SNS_WINDOW_PAGE_MENU_ICON_IMAGE", UI_FLAG_LEFT | UI_FLAG_TOP, MENU_ICON_IMAGE );
		pImage->SetDiffuse ( cCOLOR_ICON_DEFAULT_DIFFUSE );
		RegisterControl	   ( pImage );
	}

	// Note : 이름
	{
		CBasicTextBox* pText;
		m_pText = pText = new CBasicTextBox(m_pEngineDevice);
		pText->CreateSub	( this, "SNS_WINDOW_PAGE_MENU_ICON_TEXT", UI_FLAG_LEFT | UI_FLAG_BOTTOM, MENU_ICON_TEXT );
		pText->SetFont		( pFont );
		pText->SetTextAlign ( nAlignLeftCenterX );
		RegisterControl		( pText );
	}
}

void CSNSMenuIcon::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !m_bEnable )
	{
		if ( m_pImage )
		{
			m_pImage->SetDiffuse ( cCOLOR_ICON_DISABLE );
		}

		if ( m_pText )
		{
			m_pText->SetTextColor ( 0, NS_UITEXTCOLOR::DARKGRAY );
		}
	}
	else
	{
		m_pText->SetTextColor ( 0, NS_UITEXTCOLOR::WHITE );
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSNSMenuIcon::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( !m_bEnable )
	{
		CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
		return;
	}

	switch ( ControlID )
	{			
	case MENU_ICON_IMAGE:
		{
			if ( !m_pImage )
			{
				break;
			}

			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				AddMessageEx( UIMSG_SNS_MENU_ICON_PUSH );
			}

			if ( CHECK_MOUSE_IN ( dwMsg )  )
			{
				if ( dwMsg&(UIMSG_LB_DOWN | UIMSG_LB_PRESEED | UIMSG_LB_DRAG) )
				{
					m_pImage->SetDiffuse ( cCOLOR_ICON_PUSH_DIFFUSE );
				}
				else
				{
					m_pImage->SetDiffuse ( cCOLOR_ICON_OVER_DIFFUSE );
				}
			}
			else
			{
				m_pImage->SetDiffuse   ( cCOLOR_ICON_DEFAULT_DIFFUSE );
			}
		}
		break;

	default:
		AddMessageEx( dwMsg );
		break;
	}

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CSNSMenuIcon::SetImage ( const char* strTextureInfo )
{
	if ( !m_pImage )
	{
		return;
	}

	m_pImage->SetImage( strTextureInfo );
}

void CSNSMenuIcon::SetText ( const CString& strText )
{
	if ( !m_pText )
	{
		return;
	}

	m_pText->SetText( strText );
	m_pText->SetUseTextColor ( 0, TRUE );
}

void CSNSMenuIcon::SetEnable ( const BOOL bEnable )
{
	m_bEnable = bEnable;
}
