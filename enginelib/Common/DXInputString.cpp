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
//		- ���� : 2000/12/15 - ���ڿ� �Է½� Input ScanCode ��� ����.
//		- ���� : 2000/12/21 - TAB, ESCAPE, RETURN Ű �ν�.
//                          - TAB ���� ����.
//		- ���� : 2002/08/08 - �̱��� �������� ����.
//		- ���� : 2002/08/09 - OnChar() ����, ImmGetOpenStatus(m_hIMC)��
//							���� ���� ���� �Է� ���θ� �����ϰ� ����.
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

	// ����, �ѱ��� ������ �ٸ� ���� �������� IME ������ �ִ�.
	// ���� IME �����Ѵ�.
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
	// �������� ������ �Է��� ������ ��Ŀ�� �ٲٸ� �ȵȴ�.
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

	// InputString�� ��Ȱ��ȭ�� �� �θ� �����쿡 ������ ��Ŀ���� ��ȯ
	// WebBrowser������ ��Ŀ�� ��ȯ���� ���� ó����
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