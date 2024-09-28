#include "StdAfx.h"
#include "FaceStyleCard.h"

#include "../../../EngineLib/DeviceMan.h"
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

CFaceStyleCard::CFaceStyleCard ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx(pInterface, pEngineDevice)
	, m_pGaeaClient ( pGaeaClient )
	, m_pStyleName(NULL)
	, m_pId2GameText(NULL)
	, m_nFace(0)
	, m_nFaceBack(0)
	, m_nMaxFace(0)
	, m_nSex(0)
	, m_bOKButton(FALSE)
	, m_pFaceStyleSw(NULL)
{
}

CFaceStyleCard::~CFaceStyleCard ()
{
}

CBasicTextBox* CFaceStyleCard::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CFaceStyleCard::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CFaceStyleCard::CreateSubControl ()
{
	CD3DFontPar * pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );

	CUIWindowEx::CreateControl( "FACESTYLECARD_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "FACESTYLECARD_TITLE_BACK_R" );
	m_pStyleName = CreateStaticControl( "FACESTYLECARD_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : 스타일 변경 버튼
		CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "FACESTYLECARD_LEFT", UI_FLAG_DEFAULT, FACESTYLECARD_LEFT );
		pButton->CreateFlip( "FACESTYLECARD_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "FACESTYLECARD_RIGHT", UI_FLAG_DEFAULT, FACESTYLECARD_RIGHT );
		pButton->CreateFlip( "FACESTYLECARD_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, FACESTYLECARD_OK );
		pButton->CreateBaseButton( "FACESTYLECARD_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, FACESTYLECARD_CANCEL );
		pButton->CreateBaseButton( "FACESTYLECARD_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	// 얼굴 스타일 이미지;
	// 이 UI가 생성될때에는 캐릭터 정보가 로드되지않아 성별정보를 알 수없다;
	// 그런데 성별별로 따로 텍스처가 존재한다 m_wSex가 0이면 여자 1이면 남자이다;
	m_pFaceStyleSw = new CSwapImage(m_pEngineDevice);
	m_pFaceStyleSw->CreateSub ( this, "FACESTYLECARD_FACE_W", UI_FLAG_DEFAULT, NO_ID );
	RegisterControl ( m_pFaceStyleSw );
	m_pFaceStyleSw->SetVisibleSingle( FALSE );
	m_pId2GameText = "FACESTYLE_NAME";
	m_nSex = SEX_W;
}

void CFaceStyleCard::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case FACESTYLECARD_LEFT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				--m_nFace ;
				if ( m_nFace < 0 )
				{
					m_nFace = 0;
					break;
				}				

				// 텍스쳐 위치 바꿈
                CalcTexture();
				
				m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, m_nFace ) );

				// Code : 게임 화면상에 캐릭터의 헤어 스타일을 바꾼다.
				m_pGaeaClient->GetCharacter()->FaceStyleChange( m_nFace );
			}
		}
		break;

	case FACESTYLECARD_RIGHT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				++m_nFace ;
				if ( m_nFace >= m_nMaxFace )
				{
					m_nFace = m_nMaxFace-1;
					break;
				}			

				// 텍스쳐 위치 바꿈
                CalcTexture();
				
				m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText, m_nFace ) );

				// Code : 게임 화면상에 캐릭터의 헤어 스타일을 바꾼다.
				m_pGaeaClient->GetCharacter()->FaceStyleChange( m_nFace );
			}
		}
		break;

	case FACESTYLECARD_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case FACESTYLECARD_CANCEL:
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

void CFaceStyleCard::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		EMCHARCLASS emClass = m_pGaeaClient->GetCharacter()->m_emClass;

		EMCHARINDEX emIndex = CharClassToIndex(emClass);
		const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
		// 얼굴
		m_nMaxFace = sCONST.dwHEADNUM;

		if ( m_nSex != m_pGaeaClient->GetCharacter()->m_wSex )
		{
			m_nSex = m_pGaeaClient->GetCharacter()->m_wSex;
			m_pFaceStyleSw->SetImage( m_nSex ? "FACESTYLECARD_FACE_M" : "FACESTYLECARD_FACE_W" );
		}

		m_nFace = m_pGaeaClient->GetCharacter()->m_wFace;
		m_nFaceBack = m_nFace;
		m_pStyleName->AddText( ID2GAMEINTEXT( m_pId2GameText, m_nFace ) );
		
		CalcTexture();
		m_pFaceStyleSw->SetVisibleSingle( TRUE );

		//m_pGaeaClient->GetCharacter()->FaceStyleInitData();
	}
	else
	{
		if( m_bOKButton )
		{
			m_pGaeaClient->GetCharacter()->ReqInvenFaceStyleChange( m_nFace );
			m_bOKButton = FALSE;
		}
		else
		{
			m_pGaeaClient->GetCharacter()->FaceStyleChange( m_nFaceBack );
		}

		m_pStyleName->ClearText();
		m_pFaceStyleSw->SetVisibleSingle( FALSE );
		
		//m_pGaeaClient->GetCharacter()->FaceStyleEnd();
	}
}


void CFaceStyleCard::CalcTexture()
{
	UIRECT rcTexPos;

	rcTexPos = m_pFaceStyleSw->GetTexturePos();
	rcTexPos.top = 0;
	rcTexPos.top += rcTexPos.sizeY * ( m_nFace / 8 );
	rcTexPos.bottom = rcTexPos.top + rcTexPos.sizeY;	
	rcTexPos.left =  rcTexPos.sizeX * ( m_nFace % 8 );
	rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;	
	m_pFaceStyleSw->SetTexturePos( rcTexPos );
}