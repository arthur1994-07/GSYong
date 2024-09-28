#include "pch.h"

#include "../DxTools/DxFontMan.h"
#include "./UIKeyCheck.h"
#include "./UITextControl.h"

//#include "./UILineInfoGlobal.h"

#include "./TextButtonUnity.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTextButtonUnity::CTextButtonUnity (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_bRadio( FALSE )
    , m_bFlip( false )
    , m_bDisAble( FALSE )

    , m_bButtonInfo( FALSE )

    , m_KeyCode ( NULL )
    , m_KeyCodeEx ( NULL )

    , m_pButton( NULL )
    , m_pButtonIn( NULL )
    , m_pButtonClick( NULL )
    , m_pButtonText( NULL )
    , m_pButtonTextBack( NULL )
{
}

CTextButtonUnity::~CTextButtonUnity ()
{
}

void CTextButtonUnity::CreateBaseButton ( const TCHAR* szButtonControl)
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szButtonControl );
	pControl->SetVisibleSingle(TRUE);
	RegisterControl ( pControl );
	m_pButton = pControl;
}

void CTextButtonUnity::CreateClickButton ( 
	const TCHAR* szButtonControl, 
	WORD wFlipMode)
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szButtonControl );
	pControl->SetVisibleSingle(FALSE);
	RegisterControl ( pControl );
	m_pButtonClick = pControl;
}

void CTextButtonUnity::CreateMouseInButton ( const TCHAR* szButtonControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szButtonControl );
	pControl->SetVisibleSingle(FALSE);
	RegisterControl ( pControl );
	m_pButtonIn = pControl;
}

void CTextButtonUnity::CreateTextBox ( 
    const TCHAR* szButtonControl, 
    const TCHAR* szText,
    WORD nTextAlign )
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pButtonText = new CBasicTextBox(m_pEngineDevice);
	pButtonText->CreateSub ( this, szButtonControl );
	pButtonText->SetFont ( pFont9 );
	pButtonText->SetTextAlign ( nTextAlign );	
	RegisterControl ( pButtonText );
	m_pButtonText = pButtonText;

	CString strText( szText );
	SetOneLineText(szText);
}


void CTextButtonUnity::SetOneLineText( 
										const CString& strTemp, 
										const D3DCOLOR& TextColor )
{
	m_bButtonInfo = FALSE;
	m_strText = strTemp;
	m_dwColor = TextColor;

	if( NULL == m_pButtonText )
		return;

	CString strVisibleInfo = _T( "" );

	if( FALSE == m_strText.IsEmpty() )
	{
		SIZE sizeText;
		m_pButtonText->GetFont()->GetTextExtent( m_strText.GetString(), sizeText );

		// 문자열의 길이가 버튼보다 클경우 말줄임 표시를 쓰도록 한다.
		if( m_pButtonText->GetLocalPos().sizeX <= sizeText.cx )
		{
			m_bButtonInfo = TRUE;

			int nTotalLength = m_strText.GetLength();
			int nOneLengthSize = sizeText.cx / nTotalLength;

			int nMaxLength = (int) ( m_pButtonText->GetLocalPos().sizeX / nOneLengthSize );
			nMaxLength -= 3;

			strVisibleInfo.Format( _T( "%s..." ), m_strText.Left( nMaxLength ) );
		}
		else
		{
			strVisibleInfo = m_strText;
		}
	}

	m_pButtonText->SetOneLineText( strVisibleInfo, TextColor );
}

void CTextButtonUnity::ResetButton()
{
	if( m_pButtonIn ) 
		m_pButtonIn->SetVisibleSingle( FALSE );
	if( m_pButtonClick ) 
		m_pButtonClick->SetVisibleSingle( FALSE );		
}

void CTextButtonUnity::SetFlip( bool bFlip )
{
	m_bFlip = bFlip;
}

void CTextButtonUnity::SetDisable( BOOL bDisAble )
{
	m_bDisAble = bDisAble;

	if( TRUE == bDisAble )
	{
		SetDiffuse ( D3DCOLOR_ARGB ( 100, 0xFF, 0xFF, 0xFF ) );
	}
	else
	{
		SetDiffuse ( D3DCOLOR_ARGB ( 0xFF, 0xFF, 0xFF, 0xFF ) );
	}
}


void CTextButtonUnity::Update ( 
								 int x, 
								 int y, 
								 BYTE LB, 
								 BYTE MB, 
								 BYTE RB, 
								 int nScroll, 
								 float fElapsedTime, 
								 BOOL bFirstControl )
{
	CUIGroup::Update ( 
		x, 
		y, 
		LB, 
		MB, 
		RB, 
		nScroll, 
		fElapsedTime, 
		bFirstControl );

	DWORD dwMsg = GetMessageEx ();

	// 사용불가능일 경우 마우스인 메세지만 남기고 나머지 메세지는 
	// 없애준다. 
	// 버튼인에 정보가 담겨있는 경우가 있어 마우스인 필요 ( ex: 레벨업 버튼 )
	if( TRUE == m_bDisAble )
	{
		ResetMessageEx ();

		if( CHECK_MOUSE_IN ( dwMsg ) )
			AddMessageEx( UIMSG_MOUSEIN );
		return;
	}

	//if( TRUE == m_bButtonInfo && 
	//	CHECK_MOUSE_IN ( dwMsg ) )
	//{
	//	UILineInfoGlobal::SetLineInfo( m_strText, m_dwColor, x, y );
	//}

	if( FALSE == m_bRadio )
		ResetButton();	

	if( m_pButtonClick )
		m_pButtonClick->SetVisibleSingle( m_bFlip );	

	if(TRUE == bFirstControl && 
		FALSE == IsNoMessage() )
	{
		if( m_pButtonIn && false == m_bFlip )	
			m_pButtonIn->SetVisibleSingle( CHECK_MOUSE_IN ( dwMsg ) );
		if( m_pButtonClick && false == m_bFlip) 
			m_pButtonClick->SetVisibleSingle( CHECK_MOUSE_IN_LBDOWNLIKE (dwMsg ) );	
		if( m_pButtonText )	
			m_pButtonText->SetUseOverColor( CHECK_MOUSE_IN ( dwMsg ) );

		CUIControl* pParent = GetTopParent ();
		BOOL bFocusControl = ( pParent && pParent->IsFocusControl() ) ? TRUE : FALSE;

		if ( TRUE == bFocusControl )
		{
			if ( m_KeyCode || m_KeyCodeEx )
			{
				if ( UIKeyCheck::GetInstance()->Check ( m_KeyCode, DXKEY_DOWN ) )
				{
					AddMessageEx ( UIMSG_KEY_FOCUSED );
				}

				if ( UIKeyCheck::GetInstance()->Check ( m_KeyCodeEx, DXKEY_DOWN ) )
				{
					AddMessageEx ( UIMSG_KEY_FOCUSED );
				}
			}
		}
	}
}