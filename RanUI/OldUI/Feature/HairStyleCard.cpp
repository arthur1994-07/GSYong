#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "HairStyleCard.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CHairStyleCard::CHairStyleCard(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindowEx(pInterface, pEngineDevice)
	, m_pGaeaClient ( pGaeaClient )
	, m_pStyleName(NULL)
	, m_pId2GameText(NULL)
	, m_nHair(0)
	, m_nHairBack(0)
	, m_nMaxHair(0)
	, m_bOKButton(FALSE)
{
}

CHairStyleCard::~CHairStyleCard ()
{
}

CBasicTextBox* CHairStyleCard::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CHairStyleCard::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CHairStyleCard::CreateSubControl ()
{
	CD3DFontPar * pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );

	CUIWindowEx::CreateControl( "HAIRSTYLECARD_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "HAIRSTYLECARD_TITLE_BACK_R" );
	m_pStyleName = CreateStaticControl( "HAIRSTYLECARD_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : 스타일 변경 버튼
		CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "HAIRSTYLECARD_LEFT", UI_FLAG_DEFAULT, HAIRSTYLECARD_LEFT );
		pButton->CreateFlip( "HAIRSTYLECARD_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "HAIRSTYLECARD_RIGHT", UI_FLAG_DEFAULT, HAIRSTYLECARD_RIGHT );
		pButton->CreateFlip( "HAIRSTYLECARD_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, HAIRSTYLECARD_OK );
		pButton->CreateBaseButton( "HAIRSTYLECARD_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, HAIRSTYLECARD_CANCEL );
		pButton->CreateBaseButton( "HAIRSTYLECARD_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	// Note : 헤어 스타일 이미지;
	{	
		m_pHairStyleSw = new CSwapImage( m_pEngineDevice );
		m_pHairStyleSw->CreateSub( this, "HAIRSTYLECARD_HAIR_W", UI_FLAG_DEFAULT, NO_ID );
		RegisterControl(m_pHairStyleSw);
		m_pHairStyleSw->SetVisibleSingle( FALSE );
		m_pId2GameText = "HAIRSTYLE_NAME";
		m_nSex = SEX_W;
	}
}

void CHairStyleCard::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case HAIRSTYLECARD_LEFT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				--m_nHair;

				if ( m_nHair < 0 )
				{
					m_nHair = 0;
					break;
				}
				
				CalcTextureHair();
				m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, m_nHair ) );				
				m_pGaeaClient->GetCharacter()->HairStyleChange( m_nHair );
			}
		}
		break;

	case HAIRSTYLECARD_RIGHT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				++m_nHair;

				if ( m_nHair >= m_nMaxHair )
				{
					m_nHair = m_nMaxHair-1;
					break;
				}
				
				CalcTextureHair();
				m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, m_nHair ) );
				m_pGaeaClient->GetCharacter()->HairStyleChange( m_nHair );
			}
		}
		break;

	case HAIRSTYLECARD_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case HAIRSTYLECARD_CANCEL:
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CHairStyleCard::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		EMCHARCLASS emClass = m_pGaeaClient->GetCharacter()->m_emClass;

		if ( m_nSex != m_pGaeaClient->GetCharacter()->m_wSex )
		{
			m_nSex = m_pGaeaClient->GetCharacter()->m_wSex;
			m_pHairStyleSw->SetImage( m_nSex ? "HAIRSTYLE_HAIR_M" : "HAIRSTYLE_HAIR_W");
		}

		EMCHARINDEX emIndex = CharClassToIndex(emClass);
		const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
		m_nMaxHair = sCONST.dwHAIRNUM;

		m_nHair = m_pGaeaClient->GetCharacter()->m_wHair;;				
		m_nHairBack = m_nHair;
		
		CalcTextureHair();
		m_pStyleName->AddText( ID2GAMEINTEXT( m_pId2GameText, m_nHair ) );
		m_pHairStyleSw->SetVisibleSingle( TRUE );
		//m_pGaeaClient->GetCharacter()->HairStyleInitData();
	}
	else
	{
		if( m_bOKButton )
		{
			m_pGaeaClient->GetCharacter()->ReqInvenHairStyleChange( m_nHair );
			m_bOKButton = FALSE;
		}
		else
		{
			m_pGaeaClient->GetCharacter()->HairStyleChange( m_nHairBack );
		}

		m_pStyleName->ClearText();		
		//m_pGaeaClient->GetCharacter()->HairStyleEnd();
	}
}

void CHairStyleCard::CalcTextureHair()
{	
	UIRECT rcTexPos;

	rcTexPos = m_pHairStyleSw->GetTexturePos();
	rcTexPos.top = 0;
	rcTexPos.top += rcTexPos.sizeY * ( m_nHair / 8 );
	rcTexPos.bottom = rcTexPos.top + rcTexPos.sizeY;	
	rcTexPos.left =  rcTexPos.sizeX * ( m_nHair % 8 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pHairStyleSw->SetTexturePos( rcTexPos );

}
