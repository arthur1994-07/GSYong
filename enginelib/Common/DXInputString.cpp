//-----------------------------------------------------------------------
//
//		CLASS   DXInputString
//
//		Using IME -
//		Link : "imm32.lib"
//
//		coding : 2000/07/11 ~ 12
//		by jdh
//
//		- 수정 : 2000/12/15 - 문자열 입력시 Input ScanCode 기능 제한.
//		- 수정 : 2000/12/21 - TAB, ESCAPE, RETURN 키 인식.
//                          - TAB 문자 무시.
//		- 수정 : 2002/08/08 - 싱글턴 페턴으로 수정.
//		- 수정 : 2002/08/09 - OnChar() 문제, ImmGetOpenStatus(m_hIMC)의
//							상태 값에 따라 입력 여부를 결정하게 수정.
//
//-----------------------------------------------------------------------
#include "pch.h"
#include "./DXInputString.h"
#include "./IMEEdit.h"
#include "../DxTools/DxFontMan.h"
#include "../DxTools/RENDERPARAM.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL DXInputString::m_bCaratMove = FALSE;

// DXInputString& DXInputString::GetInstance()
// {
// 	static DXInputString Instance;
// 	return Instance;
// }

DXInputString::DXInputString(EMSERVICE_PROVIDER emServiceProvider)
	: m_emServiceProvider(emServiceProvider)
    , m_bOn( FALSE )
	, m_pParentWnd( NULL )
	, m_peditIME( NULL )
{
	m_bCaratMove = TRUE;
}

DXInputString::~DXInputString()
{
}

void DXInputString::Create(CWnd *_pWnd, CRect& rtPosition)
{
	m_pParentWnd = _pWnd;

	// 영어, 한국어 제외한 다른 언어에서 스케일폼 IME 문제가 있다.
	// 기존 IME 돌게한다.
	if( language::DEFAULT != RENDERPARAM::emLangSet &&
		language::KOREAN != RENDERPARAM::emLangSet )
	{
		m_editIMEMulti.Create  ( ES_WANTRETURN|ES_MULTILINE, rtPosition, m_pParentWnd, 1201 );
		m_editIMEMulti.SetFont ( 0, NULL );
		m_editIMEMulti.SetMultiLine( true );

		m_editIME.Create ( NULL, rtPosition, m_pParentWnd, 1201 );
		m_editIME.SetFont ( 0, NULL );
	}

	m_peditIME = &m_editIME;

	//
	//m_editIME.Create ( ES_MULTILINE, rtPosition, m_pParentWnd, 1201 );
	//m_editIME.SetFont ( 0, NULL );
}

void DXInputString::Move ( CRect &rtPosition )
{
	m_editIME		.MoveWindow ( rtPosition.left, rtPosition.top, rtPosition.Width(), rtPosition.Height(), FALSE );
	m_editIMEMulti	.MoveWindow ( rtPosition.left, rtPosition.top, rtPosition.Width(), rtPosition.Height(), FALSE );
}

BOOL DXInputString::OnInput ( const EMIMETYPE emType )
{
	switch ( emType )
	{
	case EMIMETYPE_DEFAULT:		m_peditIME = &m_editIME;		break;
	case EMIMETYPE_MULTILINE:	m_peditIME = &m_editIMEMulti;	break;

	}

	m_bOn = TRUE;
	// 스케일폼 쪽으로 입력이 들어가려면 포커스 바꾸면 안된다.
	//m_peditIME->SetFocus();

	return TRUE;
}

BOOL DXInputString::OffInput ()
{
	m_bOn = FALSE;
	m_pParentWnd->SetFocus();

	Move ( CRect(0,0,0,0) );

	return TRUE;
}

BOOL DXInputString::IsOn ()
{
	CWnd *pWnd = m_pParentWnd->GetFocus();
	if ( !pWnd )
	{
		OffInput ();
		return FALSE;
	}

	if ( !m_peditIME )
	{
		OffInput ();
		return FALSE;
	}

	// InputString이 비활성화일 때 부모 윈도우에 강제로 포커스를 반환
	// WebBrowser때문에 포커스 반환없이 리턴 처리함
    return m_bOn;

/*
	if (pWnd != (CWnd*) m_peditIME)
	{
		OffInput ();
		return FALSE;
	}
	return m_bOn;
*/
}