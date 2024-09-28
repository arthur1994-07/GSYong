#include "StdAfx.h"
#include "GenderChangeWindow.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../InnerInterface.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CGenderChangeWindow::CGenderChangeWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx( pInterface, pEngineDevice )
	, m_pGaeaClient ( pGaeaClient )
	, m_pFaceStyleName(NULL)
	, m_pHairStyleName (NULL)
	, m_pFaceStyleSw (NULL)
	, m_pHairStyleSw (NULL)
	, m_pHairId2GameText (NULL)
	, m_pFaceId2GameText (NULL)
	, m_nHair(0)
	, m_nHairBack(0)
	, m_nMaxHair(0)
	, m_nFace(0)
	, m_nFaceBack(0)
	, m_nMaxFace(0)
	, m_nSex(0)
	, m_bReverse(FALSE)
	, m_bOKButton(FALSE)
{
}

CGenderChangeWindow::~CGenderChangeWindow ()
{
}

CBasicTextBox* CGenderChangeWindow::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CGenderChangeWindow::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CGenderChangeWindow::CreateSubControl ()
{
	CD3DFontPar * pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );

	CUIWindowEx::CreateControl( "FACESTYLE_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "FACESTYLE_TITLE_BACK_R" );
	m_pFaceStyleName = CreateStaticControl( "FACESTYLE_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	CUIWindowEx::CreateControl( "HAIRSTYLE_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "HAIRSTYLE_TITLE_BACK_R" );
	m_pHairStyleName = CreateStaticControl( "HAIRSTYLE_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : 스타일 변경 버튼
		CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "FACESTYLE_LEFT", UI_FLAG_DEFAULT, FACESTYLE_LEFT );
		pButton->CreateFlip( "FACESTYLE_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "FACESTYLE_RIGHT", UI_FLAG_DEFAULT, FACESTYLE_RIGHT );
		pButton->CreateFlip( "FACESTYLE_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "HAIRSTYLE_LEFT", UI_FLAG_DEFAULT, HAIRSTYLE_LEFT );
		pButton->CreateFlip( "HAIRSTYLE_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "HAIRSTYLE_RIGHT", UI_FLAG_DEFAULT, HAIRSTYLE_RIGHT );
		pButton->CreateFlip( "HAIRSTYLE_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, GENDER_CHANGE_OK );
		pButton->CreateBaseButton( "GENDER_CHANGE_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, GENDER_CHANGE_CANCEL );
		pButton->CreateBaseButton( "GENDER_CHANGE_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	// 헤어, 얼굴 스타일 이미지;
	// 이 UI가 생성될때에는 캐릭터 정보가 로드되지않아 성별정보를 알 수없다;
	// 그런데 성별별로 따로 텍스처가 존재한다 m_wSex가 0이면 여자 1이면 남자이다;
	// 기본 남자로 설정, 남자면 여자 헤어, 얼굴이 나와야한다;
	m_pFaceStyleSw = new CSwapImage(m_pEngineDevice);
	m_pFaceStyleSw->CreateSub ( this, "FACESTYLE_FACE_W", UI_FLAG_DEFAULT, NO_ID );
	RegisterControl ( m_pFaceStyleSw );
	m_pFaceStyleSw->SetVisibleSingle( FALSE );
	m_pFaceId2GameText = "FACESTYLE_NAME";

	m_pHairStyleSw = new CSwapImage(m_pEngineDevice);
	m_pHairStyleSw->CreateSub ( this, "HAIRSTYLE_HAIR_W", UI_FLAG_DEFAULT, NO_ID );
	RegisterControl ( m_pHairStyleSw );
	m_pHairStyleSw->SetVisibleSingle( FALSE );
	m_pHairId2GameText = "HAIRSTYLE_NAME";
	m_nSex = SEX_M;
}

void CGenderChangeWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{

	case FACESTYLE_LEFT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				--m_nFace;
				if ( m_nFace < 0 )
				{
					m_nFace = 0;
					break;
				}

				CalcTextureFace();
				m_pFaceStyleSw->SetVisibleSingle( TRUE );

				m_pFaceStyleName->SetText( ID2GAMEINTEXT( m_pFaceId2GameText, m_nFace ) );
			}
		}
		break;

	case FACESTYLE_RIGHT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				++m_nFace;
				if ( m_nFace >= m_nMaxFace )
				{
					m_nFace = m_nMaxFace-1;
					break;
				}

				CalcTextureFace();
				m_pFaceStyleSw->SetVisibleSingle( TRUE );

				m_pFaceStyleName->SetText( ID2GAMEINTEXT( m_pFaceId2GameText, m_nFace ) );
			}
		}
		break;

	case HAIRSTYLE_LEFT:
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
				m_pHairStyleSw->SetVisibleSingle( TRUE );
				m_pHairStyleName->SetText( ID2GAMEINTEXT( m_pHairId2GameText, m_nHair ) );
				
			}
		}
		break;

	case HAIRSTYLE_RIGHT:
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
				m_pHairStyleSw->SetVisibleSingle( TRUE );
				m_pHairStyleName->SetText( ID2GAMEINTEXT( m_pHairId2GameText, m_nHair ) );
			}
		}
		break;

	case GENDER_CHANGE_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case GENDER_CHANGE_CANCEL:
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

void CGenderChangeWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		if ( m_nSex != m_pGaeaClient->GetCharacter()->m_wSex )
		{
			m_nSex = m_pGaeaClient->GetCharacter()->m_wSex;
			// 현재 남자라면 여자 얼굴, 헤어가 나와야함;
			if ( m_nSex == SEX_M )
			{
				m_pFaceStyleSw->SetImage("FACESTYLE_FACE_W");
				m_pHairStyleSw->SetImage("HAIRSTYLE_HAIR_W");
			}
			else// if ( m_nSex == SEX_W )
			{
				m_pFaceStyleSw->SetImage("FACESTYLE_FACE_M");
				m_pHairStyleSw->SetImage("HAIRSTYLE_HAIR_M");
			}
		}
		EMCHARCLASS emClass = m_pGaeaClient->GetCharacter()->m_emClass;

		// 여기서 UI에 보여주기 위해 임시 성전환;
		m_nSex = ( m_nSex == SEX_W) ? SEX_M : SEX_W;
		emClass = CharClassToSex( emClass );

		EMCHARINDEX emIndex = CharClassToIndex(emClass);
		const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
		m_nMaxFace = sCONST.dwHEADNUM;
		m_nMaxHair = sCONST.dwHAIRNUM_SELECT;

		m_nFace = 0;
		m_nHair = 0;				

		CalcTextureFace();
		CalcTextureHair();

		m_pFaceStyleName->AddText( ID2GAMEINTEXT( m_pFaceId2GameText, m_nFace ) );
		m_pFaceStyleSw->SetVisibleSingle( TRUE );

		m_pHairStyleName->AddText( ID2GAMEINTEXT( m_pHairId2GameText, m_nHair ) );
		m_pHairStyleSw->SetVisibleSingle( TRUE );
		
	}
	else
	{
		if( m_bOKButton )
		{
			m_pGaeaClient->GetCharacter()->ReqInvenGenderChange( m_nFace, m_nHair );
			m_bOKButton = FALSE;
		}

		m_pFaceStyleName->ClearText();
		m_pHairStyleName->ClearText();

	}
}


void CGenderChangeWindow::CalcTextureFace()
{
	UIRECT rcTexPos;

	rcTexPos = m_pFaceStyleSw->GetTexturePos();
	rcTexPos.top = rcTexPos.sizeY * ( m_nFace / 8 );
	rcTexPos.bottom = rcTexPos.top + rcTexPos.sizeY;	
	rcTexPos.left =  rcTexPos.sizeX * ( m_nFace % 8 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pFaceStyleSw->SetTexturePos( rcTexPos );
}


void CGenderChangeWindow::CalcTextureHair()
{
	UIRECT rcTexPos;

	rcTexPos = m_pHairStyleSw->GetTexturePos();
	rcTexPos.top = rcTexPos.sizeY * ( m_nHair / 8 );
	rcTexPos.bottom = rcTexPos.top + rcTexPos.sizeY;	
	rcTexPos.left =  rcTexPos.sizeX * ( m_nHair % 8 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pHairStyleSw->SetTexturePos( rcTexPos );

}