/******************************************************************************\
*	IMEEdit.cpp : ���� �����Դϴ�.
*
*	������ Microsoft �ҽ� �ڵ� ������ �Ϻ��Դϴ�. 
*	Copyright (c) Microsoft Corporation.
*	All rights reserved. 
*
*	�� �ҽ� �ڵ�� Microsoft ���� ���� ��/�Ǵ� 
*	WinHelp �������� ���� �߰������� �����Ǵ� �����Դϴ�.
*	Microsoft ���� ���α׷��� ���� �ڼ��� ������ 
*	�ش� ���� ������ �������� �����Ͻʽÿ�.
\******************************************************************************/

#include "pch.h"
#include "./IMEEdit.h"
#include "./DxInputString.h"
#include "../DxTools/DxFontMan.h"
#include "../DxTools/RENDERPARAM.h"
#include "../../SigmaCore/Log/LogMan.h"
// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
#include "../../RanLogic/GLUseFeatures.h"

/////////////////////////////////////////////////////////////////////////////
// CIMEEdit

CIMEEdit::CIMEEdit()
{
	OSVERSIONINFO osVI;

	// ���� ��ġ�� �ʱ�ȭ�մϴ�.
	m_xEndPos = m_xInsertPos = m_xCaretPos = 0;

	// ���۸� �ʱ�ȭ�մϴ�.
	SecureZeroMemory( m_szBuffer, sizeof(m_szBuffer) );
	SecureZeroMemory( m_szBufferMbs, sizeof(m_szBufferMbs) );
	SecureZeroMemory( m_szBackup, sizeof(m_szBackup) );

	// ��� ������ �ʱ�ȭ�մϴ�.
	m_bEnterKeyDown = false;		// �Է� ���� ���� �Ǵܿ� ���
	m_bKeyUse = TRUE;
	m_bWideCaret  = false;
	m_bMultiLine  = false;
	m_IsOnChar    = false;
	m_IsOnKeyDown = 0;

	m_fStat = FALSE;				// ���� ������ ǥ������ �ʽ��ϴ�.
	m_fShowInvert = FALSE;			// ���� �������� ���ڿ��� ��ȯ���� �ʽ��ϴ�.
	m_nComSize = 0;					// �������� ���ڿ� ũ��(����Ʈ)�� ���� ������ ����ϴ�.
	m_nCharSet = DEFAULT_CHARSET;		
	m_hFont = NULL;		
	m_fIsNT = FALSE;
	m_fIsXP = FALSE;

	// �ý��� ������ Ȯ���մϴ�. 
	SecureZeroMemory( &osVI, sizeof( OSVERSIONINFO ) );	// OSVERSIONINFO ������ �ʱ�ȭ�մϴ�.
	osVI.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );    

	if (GetVersionEx( &osVI ))		// OS ���� ������ �����ɴϴ�.
	{
		if( osVI.dwPlatformId == VER_PLATFORM_WIN32_NT )
		{
			m_fIsNT = TRUE;

			// �� ������ Ȯ���մϴ�.
			if(osVI.dwMinorVersion) 
				m_fIsXP = TRUE;		// Windows XP �̻��� ���
		}
	}
	else	// GetVersionEx �Լ��� �����ϸ� �ٸ� �Լ��� ����Ͽ� �ٽ� �õ��մϴ�.
	{
		if (GetVersion() < 0x80000000)
			m_fIsNT = TRUE;
	}

	m_bChangeKeyboardLayout = FALSE;
	m_hKeyboardLayout = 0;
/*
	OSVERSIONINFOEX osViEx;
	SYSTEM_INFO si;
	ZeroMemory(&osViEx, sizeof(OSVERSIONINFOEX));
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	GetVersionEx((OSVERSIONINFO*) &osViEx);
	if (osViEx.dwMajorVersion == 5 && osViEx.dwMinorVersion == 1)
	{
		sc::writeLogInfo("Window Xp");
		if( osViEx.wSuiteMask & VER_SUITE_PERSONAL )
			sc::writeLogInfo("Home Edition");
		else sc::writeLogInfo("Professional");
	}
	if ( osViEx.dwMajorVersion == 5 && osViEx.dwMinorVersion == 2 )
	{
		if( osViEx.wProductType == VER_NT_WORKSTATION &&
			si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
		{
			sc::writeLogInfo("Windows XP Professional x64 Edition");
		}
	}
	if ( VER_PLATFORM_WIN32_NT==osViEx.dwPlatformId && 
		osViEx.dwMajorVersion > 4 )
	{
		if( osViEx.dwMinorVersion == 0 )
		{
			if( osViEx.wProductType == VER_NT_WORKSTATION )
				sc::writeLogInfo("Windows Vista ");
			else sc::writeLogInfo("Windows Server 2008 ");
		}
	}
*/
}

CIMEEdit::~CIMEEdit()
{
	if ( m_bChangeKeyboardLayout == TRUE )
	{
		UnloadKeyboardLayout( m_hKeyboardLayout );
	}

	if (m_hFont)
		DeleteObject(m_hFont);
}

const char* CIMEEdit::GetString ()
{
	SecureZeroMemory( m_szBufferMbs, sizeof(m_szBufferMbs) );
	WideCharToMultiByte ( m_nCodePage, NULL, m_szBuffer, -1, m_szBufferMbs, BUFFERSIZE*2+1, NULL, NULL );

	return m_szBufferMbs;
}

void CIMEEdit::SetString ( const char* szChange )
{
	if ( szChange && strlen(szChange) )
	{
		int nLen = MultiByteToWideChar( m_nCodePage, 
			NULL, 
			szChange, 
			-1, 
			m_szBuffer, 
			BUFFERSIZE+1 );
		m_xEndPos = m_xInsertPos = m_xCaretPos = nLen-1;
	}
	else
	{
		ClearBuffer();
	}

	//if( DXInputString::m_bCaratMove )
	//{
	//	// �� �κ��� ó���ؾ� ���ѱ��̸�ŭ �Է����� �� ���װ� �߻����� �ʴ´�
	//	memcpy( m_szBackup, m_szBuffer, WCHARSIZE * (BUFFERSIZE+1) );
	//}

	if( m_fStat )
	{
		// Turn off composition flag
		m_fStat = FALSE;

		// IME �Ӽ��� IME_PROP_AT_CARET�� ������ ���� 3 ����� �����մϴ�.
		if ( m_dwProperty&IME_PROP_AT_CARET )
		{
			OnImeEndComposition(0,0);
		}
	}
}

void CIMEEdit::InsertString ( const char* szInsert )
{
	if ( szInsert && strlen(szInsert) )
	{
		wchar_t szBack[BUFFERSIZE+1];	// �Է� ���ڿ��� �����Դϴ�.
		wchar_t	szBuffer[BUFFERSIZE+1];	// �Է� ���ڿ��� �����Դϴ�.

		int nLen = MultiByteToWideChar( m_nCodePage, 
			NULL, 
			szInsert, 
			-1, 
			szBuffer, 
			BUFFERSIZE+1 );

		int size = wcslen(m_szBuffer);

		if ( size + nLen > BUFFERSIZE+1 ) return;

		//���
		StringCchCopyW( szBack,  BUFFERSIZE+1, m_szBuffer );

		StringCchCopyNW( m_szBuffer, BUFFERSIZE+1, szBack, m_xInsertPos );
		StringCchCatW( m_szBuffer, BUFFERSIZE+1, szBuffer );
		StringCchCatW( m_szBuffer, BUFFERSIZE+1, szBack+m_xInsertPos );

		m_xInsertPos = m_xCaretPos = nLen+m_xInsertPos-1;
		m_xEndPos += nLen-1;

	}

	//if( DXInputString::m_bCaratMove )
	//{
	//	// �� �κ��� ó���ؾ� ���ѱ��̸�ŭ �Է����� �� ���װ� �߻����� �ʴ´�
	//	memcpy( m_szBackup, m_szBuffer, WCHARSIZE * (BUFFERSIZE+1) );
	//}

	if( m_fStat )
	{
		// Turn off composition flag
		m_fStat = FALSE;

		// IME �Ӽ��� IME_PROP_AT_CARET�� ������ ���� 3 ����� �����մϴ�.
		if ( m_dwProperty&IME_PROP_AT_CARET )
		{
			OnImeEndComposition(0,0);
		}
	}

}

int CIMEEdit::SetInsertPos( int xPos )
{
	if( DXInputString::m_bCaratMove == FALSE )
		return m_xEndPos;

	m_xCaretPos = xPos;

	if( m_xCaretPos > m_xEndPos )
		m_xCaretPos = m_xEndPos;

	m_xInsertPos = m_xCaretPos;

	return m_xCaretPos;
}

BEGIN_MESSAGE_MAP(CIMEEdit, CEdit)
	//{{AFX_MSG_MAP(CIMEEdit)
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HRESULT CIMEEdit::Render ( LPDIRECT3DDEVICEQ pd3dDevice, int x, int y, DWORD dwColor, CD3DFontPar *pFont )
{
	if ( !pd3dDevice )	return E_FAIL;
	if ( !pFont )		return E_FAIL;

	int nXStart(0), nXEnd(0);
	wchar_t uzTempStr[BUFFERSIZE+1]={0};
	TCHAR szTempStr[BUFFERSIZE*2+1]={0};

	pFont->DrawText ( (float)x, (float)y, dwColor, GetString() );

	//CDebugSet::ToView ( 1, 8, "[Render]%s", GetString() );

	// Draw dotted line under composition string
	if ( (m_fStat) && (m_dwProperty&IME_PROP_AT_CARET) && (m_nLanguage!=language::KOREAN) )
	{
		// Calculate start position of composition string and move pen to start position
		memcpy(uzTempStr, m_szBuffer, WCHARSIZE * m_xInsertPos);	 
		uzTempStr[m_xInsertPos] = L'\0';
		WideCharToMultiByte ( m_nCodePage, NULL, uzTempStr, -1, szTempStr, BUFFERSIZE*2+1, NULL, NULL );

		SIZE sizeExtend;
		pFont->GetTextExtent ( szTempStr, sizeExtend );
		nXStart = X_INIT + sizeExtend.cx;

		DWORD dwHeight = pFont->GetFontHeight();

		//dc.MoveTo(nXStart, Y_INIT + tm.tmHeight);

		// �������� ���ڿ��� �� ��ġ�� ����ϰ� ������ �׸��ϴ�.
		m_szComStr[m_nComSize/WCHARSIZE] = L'\0';

		if ( m_nLanguage==language::JAPANESE )
		{
			for (int i = 0; ; i++)
			{
				if ( m_dwCompCls[i]==m_nComAttrSize )	break;

				memcpy ( uzTempStr, &m_szComStr[m_dwCompCls[i]], WCHARSIZE * (m_dwCompCls[i+1] - m_dwCompCls[i]) );
				uzTempStr[m_dwCompCls[i+1]-m_dwCompCls[i]] = L'\0';
				WideCharToMultiByte ( m_nCodePage, NULL, uzTempStr, -1, szTempStr, BUFFERSIZE*2+1, NULL, NULL );

				SIZE sizeExtend;
				pFont->GetTextExtent ( szTempStr, sizeExtend );

				nXEnd = nXStart + sizeExtend.cx;

				/*if (m_bComAttr[m_dwCompCls[i]] == 0x00)
				pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));	// For input characters
				else if (m_bComAttr[m_dwCompCls[i]] == 0x01)
				pPen = new CPen(PS_SOLID, 2, RGB(0, 0, 0));	// For editable phrase
				else
				pPen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));	// For converted characters

				dc.LineTo(nXEnd - 1, Y_INIT + tm.tmHeight);
				dc.MoveTo(nXEnd + 1, Y_INIT + tm.tmHeight);*/

				nXStart = nXEnd;
			}
		}
		else
		{
			WideCharToMultiByte ( m_nCodePage, NULL, m_szComStr, -1, szTempStr, BUFFERSIZE*2+1, NULL, NULL );

			SIZE sizeExtend;
			pFont->GetTextExtent ( szTempStr, sizeExtend );

			nXEnd = nXStart + sizeExtend.cx;
			//pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));
			//dc.LineTo(nXEnd, Y_INIT + tm.tmHeight);
		}
	}

	// ���� �������� ���ڿ�(�߱��� ��ü �� �߱��� ��ü)�� �����մϴ�.
	//if (m_fShowInvert)
	//{
	//	// ���� ���ڿ��� ���� ��ġ�� ����մϴ�.
	//	memcpy(szTempStr, m_szBuffer, WCHARSIZE * m_nInvertStart);	 
	//	szTempStr[m_nInvertStart] = L'\0';
	//	cRect.top = Y_INIT;
	//	cRect.left = X_INIT + GetWidthOfString(szTempStr);

	//	// ���� ���ڿ��� �� ��ġ�� ����մϴ�.
	//	cRect.bottom = cRect.top + tm.tmHeight;
	//	memcpy(szTempStr, &m_szBuffer[m_nInvertStart], WCHARSIZE * (m_nInvertEnd - m_nInvertStart));	 
	//	szTempStr[m_nInvertEnd - m_nInvertStart] = L'\0';
	//	cRect.right = cRect.left + GetWidthOfString(szTempStr);

	//	dc.InvertRect(cRect);  
	//}

	//ShowCaret();
	//SelectObject(dc, pOldFont);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CIMEEdit message handlers

void CIMEEdit::OnPaint() 
{
	CPaintDC dc(this); //  �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

	TEXTMETRIC	tm;
	int			nXStart, nXEnd;
	CRect		cRect;
	HFONT		pOldFont = (HFONT)SelectObject(dc, m_hFont);
	CPen		*pPen, *pOldPen;
	wchar_t		szTempStr[BUFFERSIZE+1]={0};
	RECT		rcBounds;

	HideCaret();

	// ����� ������� ä��ϴ�.
	GetClientRect(&rcBounds);
	dc.FillRect(&rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

	// ������ �ؽ�Ʈ�� ǥ���մϴ�.
	TextOutW(dc, X_INIT, Y_INIT, m_szBuffer, (int)wcslen(m_szBuffer));

	dc.GetTextMetrics(&tm);

	// �������� ���ڿ� �Ʒ��� ������ �׸��ϴ�.
	if ( (m_fStat) && (m_dwProperty & IME_PROP_AT_CARET) && (m_nLanguage != language::KOREAN) )
	{
		// �������� ���ڿ��� ���� ��ġ�� ����ϰ� ���� ��ġ�� ���� �̵��մϴ�.
		memcpy(szTempStr, m_szBuffer, WCHARSIZE * m_xInsertPos);	 
		szTempStr[m_xInsertPos] = L'\0';
		nXStart = X_INIT + GetWidthOfString(szTempStr);
		dc.MoveTo(nXStart, Y_INIT + tm.tmHeight);

		// �������� ���ڿ��� �� ��ġ�� ����ϰ� ������ �׸��ϴ�.
		m_szComStr[m_nComSize / WCHARSIZE] = L'\0';

		if (m_nLanguage == language::JAPANESE)
		{
			for (int i = 0; ; i++)
			{
				if (m_dwCompCls[i] == m_nComAttrSize)
					break;
				memcpy(szTempStr, &m_szComStr[m_dwCompCls[i]], WCHARSIZE * (m_dwCompCls[i+1] - m_dwCompCls[i]));	 
				szTempStr[m_dwCompCls[i+1] - m_dwCompCls[i]] = L'\0';
				nXEnd = nXStart + GetWidthOfString(szTempStr);

				if (m_bComAttr[m_dwCompCls[i]] == 0x00)
					pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));	// �Է� ���ڿ� ���˴ϴ�.
				else if (m_bComAttr[m_dwCompCls[i]] == 0x01)
					pPen = new CPen(PS_SOLID, 2, RGB(0, 0, 0));	// ���� ������ ���� ���˴ϴ�.
				else 
					pPen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));	// ��ȯ�� ���ڿ� ���˴ϴ�.

				pOldPen = dc.SelectObject(pPen);
				dc.LineTo(nXEnd - 1, Y_INIT + tm.tmHeight);
				dc.MoveTo(nXEnd + 1, Y_INIT + tm.tmHeight);
				dc.SelectObject(pOldPen);
				delete pPen;
				nXStart = nXEnd;
			}
		}
		else
		{
			nXEnd = nXStart + GetWidthOfString(m_szComStr);
			pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));
			pOldPen = dc.SelectObject(pPen);
			dc.LineTo(nXEnd, Y_INIT + tm.tmHeight);
			dc.SelectObject(pOldPen);
			delete pPen;
		}
	}

	// ���� �������� ���ڿ�(�߱��� ��ü �� �߱��� ��ü)�� �����մϴ�.
	if (m_fShowInvert)
	{
		// ���� ���ڿ��� ���� ��ġ�� ����մϴ�.
		memcpy(szTempStr, m_szBuffer, WCHARSIZE * m_nInvertStart);	 
		szTempStr[m_nInvertStart] = L'\0';
		cRect.top = Y_INIT;
		cRect.left = X_INIT + GetWidthOfString(szTempStr);

		// ���� ���ڿ��� �� ��ġ�� ����մϴ�.
		cRect.bottom = cRect.top + tm.tmHeight;
		memcpy(szTempStr, &m_szBuffer[m_nInvertStart], WCHARSIZE * (m_nInvertEnd - m_nInvertStart));	 
		szTempStr[m_nInvertEnd - m_nInvertStart] = L'\0';
		cRect.right = cRect.left + GetWidthOfString(szTempStr);

		dc.InvertRect(cRect);  
	}

	ShowCaret();
	SelectObject(dc, pOldFont);
}

void CIMEEdit::OnSetFocus(CWnd* pOldWnd) 
{
	m_bWideCaret = false;

	CreateSolidCaret(1,FONT_HEIGHT);

	//	enter Ű �ʱ�ȭ.
	m_bEnterKeyDown = false;

	// ĳ�� ��ġ�� �����մϴ�.
	ShowCaretOnView();	

	ShowCaret();
}

void CIMEEdit::OnKillFocus(CWnd* pNewWnd) 
{
	HIMC	hIMC;

	//	enter Ű �ʱ�ȭ.
	m_bEnterKeyDown = false;
	m_IsOnChar		= false;
	m_IsOnKeyDown	= 0;

	//	�ɷ� �����
	HideCaret();

	// ����Ʈ ĳ���� �����մϴ�.
	Invalidate();

	// ���������� ������ �ʾ����� �������� ���ڿ��� �Ϸ��մϴ�.
	if ( (m_fStat) && (m_nLanguage != language::KOREAN) )
	{
		hIMC = ImmGetContext(this->m_hWnd);
		if (hIMC) 
		{
			ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
			ImmReleaseContext(this->m_hWnd,hIMC);
		}
	}
}

/*****************************************************************************\
* �Լ�: ShowCaretOnView
*
* ĳ�� ��ġ�� ����ϰ� ĳ���� �ٽ� ǥ���մϴ�.
\*****************************************************************************/
void CIMEEdit::ShowCaretOnView()
{
	wchar_t *szTmpStr = new(wchar_t[m_xCaretPos + 1]);

	HideCaret();

	// ĳ�� ��ġ�� ����ϰ� ĳ�� ��ġ�� �����մϴ�.
	memcpy(szTmpStr, m_szBuffer, WCHARSIZE * (m_xCaretPos));	 
	szTmpStr[m_xCaretPos] = L'\0';
	SetCaretPos(CPoint(X_INIT + GetWidthOfString(szTmpStr), Y_INIT));
	ShowCaret();

	// IME �Ӽ��� IME_PROP_AT_CARET�� ������ �������� â��
	// ��ġ�� �����մϴ�.
	if ( !(m_dwProperty & IME_PROP_AT_CARET) )
		SetCompositionWindowPos();

	delete [] szTmpStr;
}

/*****************************************************************************\
* �Լ�: GetWidthOfString
*
* ���ڿ� �ʺ�(�ȼ�)�� ����մϴ�. 
*
* �μ�:
*	LPCTSTR szStr - ���ڿ��Դϴ�.
\*****************************************************************************/
int CIMEEdit::GetWidthOfString(wchar_t *szStr)
{
	SIZE		Size;
	CClientDC	dc(this);
	HFONT		pOld = (HFONT)SelectObject(dc, m_hFont);

	GetTextExtentPoint32W(dc, szStr, (int)wcslen(szStr), &Size);

	SelectObject(dc, pOld);

	return Size.cx;
}

/*****************************************************************************\
* �Լ�: OnImeStartComposition
*
* WM_IME_STARTCOMPOSITION �޽����� �ް� ���������� �����մϴ�
*
* �μ�:
*	WPARAM wParam - ������� �ʽ��ϴ�.
*	LPARAM lParam - ������� �ʽ��ϴ�.
\*****************************************************************************/
void CIMEEdit::OnImeStartComposition(WPARAM, LPARAM)
{
	// ���۸� Ȯ���մϴ�. �� á���� ���۸� ����ϴ�. 
	if (m_xEndPos >= (BUFFERSIZE-1))	// ���۸� Ȯ���մϴ�.
		ClearBuffer();

	// �ѱ��� ���� ������ ��� ĳ���� �����մϴ�. 
	if (m_nLanguage == language::KOREAN)
	{
		m_bWideCaret = true;

		CreateSolidCaret(FONT_WIDTH, FONT_HEIGHT);	// ���� ĳ���� ����ϴ�.
		ShowCaret();
	}

	m_IsOnChar = true;

	// ���ڿ� ���۸� ����մϴ�. 
	memcpy(m_szBackup, m_szBuffer, WCHARSIZE * BUFFERSIZE+1);
}

/*****************************************************************************\
* �Լ�: OnImeStartComposition
*
* WM_IME_COMPOSITION �޽��� �� �������� ���ڿ��� �޽��ϴ�.
*
* �μ�:
*	WPARAM wParam - �������� ���ڿ��� ���� ������ ���� ������ ��Ÿ���� 
*					DBCS �����Դϴ�.
*	LPARAM lParam - ���� �÷����Դϴ�.
\*****************************************************************************/
BOOL CIMEEdit::OnImeComposition(WPARAM wParam, LPARAM lParam)
{
	HIMC	hIMC;
	int		i, nTmpInc;

	hIMC = ImmGetContext(this->m_hWnd);
	if (hIMC == NULL) 
		return TRUE;

	//
	// GCS_RESULTSTR: �ܾ� �ϳ��� �ϼ����� �˸�;
	// GCS_COMPSTR: �Է� ������ ���� �ܾ� �и�����;
	//

	// ���ڿ� ���۸� �����մϴ�.
	if (m_fStat)
		memcpy(m_szBuffer, m_szBackup, WCHARSIZE * (BUFFERSIZE+1));

	if (lParam & GCS_RESULTSTR)
	{
		m_fShowInvert = FALSE;

		// ��� ���ڿ��� �����ɴϴ�.
		SecureZeroMemory( m_szComStr, sizeof(m_szComStr) );
		m_nComSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, (LPVOID)m_szComStr, WCHARSIZE * (BUFFERSIZE+1));

		if ( m_nComSize > 0)
		{
			// ���ڿ� ���ۿ� �������� ���ڿ��� �����մϴ�.
			nTmpInc = InsertCompStr();

			// ����/ĳ�� ��ġ�� �̵��ϰ� �� ��ġ�� ������ŵ�ϴ�.
			memcpy(m_szBackup, m_szBuffer, WCHARSIZE * (BUFFERSIZE+1));
			m_xEndPos += nTmpInc;
			m_xInsertPos += nTmpInc;
			m_xCaretPos = m_xInsertPos;
		}
	}
	else if (lParam & GCS_COMPSTR)
	{
		// IME �Ӽ���  IME_PROP_AT_CARET�� ������ ���� 3 ����� �����մϴ�.
		if ( !(m_dwProperty & IME_PROP_AT_CARET) )
			return FALSE;

		// �������� ���ڿ��� �����ɴϴ�.
		SecureZeroMemory( m_szComStr, sizeof(m_szComStr) );
		m_nComSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, (LPVOID)m_szComStr, WCHARSIZE * (BUFFERSIZE+1));

		if (m_nLanguage != language::KOREAN)
		{
			// �������� Ư�� �� �������� ���ڿ��� Ŀ�� ��ġ�� �����ɴϴ�.
			if (lParam & GCS_COMPATTR)
			{
				SecureZeroMemory( m_bComAttr, sizeof(m_bComAttr) );
				m_nComAttrSize = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, m_bComAttr, sizeof(m_bComAttr));
			}

			if (lParam & GCS_CURSORPOS)
				m_nComCursorPos = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0);

			if (lParam & GCS_COMPCLAUSE)
			{
				SecureZeroMemory( m_dwCompCls, sizeof(m_dwCompCls) );
				ImmGetCompositionStringW(hIMC, GCS_COMPCLAUSE, m_dwCompCls,sizeof(m_dwCompCls));
			}

			if ((m_nLanguage == language::TRADITIONAL_CHINESE) || (m_nLanguage == language::SIMPLIFIED_CHINESE && !m_fIsXP ))
			{
				m_fShowInvert = TRUE;
				nTmpInc = 0;
				for (i = m_nComCursorPos; i < (int)m_nComAttrSize; i++)
				{
					if (m_bComAttr[i] == 1)
						nTmpInc++;
					else
						break;
				}
				m_nInvertStart = m_xInsertPos + m_nComCursorPos;
				m_nInvertEnd = m_nInvertStart + nTmpInc;
			}
		}

		if (m_nComSize > 0)
		{
			// ���ڿ� ���ۿ� �������� ���ڿ��� �����մϴ�.
			nTmpInc = InsertCompStr();

			// ĳ�� ��ġ�� �̵��մϴ�.
			//switch( m_nLanguage )
			//{
			//case KOREAN:
			//	// �ѱ��� �����ڵ� ������ �β��� ĳ���� ������ ������ 
			//	// �β��� ĳ���� ������ �ȵǾ ĳ�� ��ġ�� �ϳ� �����Ѵ�.
			//	m_xCaretPos = m_xInsertPos + nTmpInc;
			//	break;
			//case JAPANESE:
			//	m_xCaretPos = m_xInsertPos + m_nComSize / WCHARSIZE;
			//	break;
			//default:
			//	m_xCaretPos = m_xInsertPos + m_nComCursorPos;
			//	break;
			//}

			// �Ʒ��� ������ ���� �ڵ�
			if (m_nLanguage != language::KOREAN)
			{
				if (m_nLanguage == language::JAPANESE)
					m_xCaretPos = m_xInsertPos + m_nComSize / WCHARSIZE;
				else
					m_xCaretPos = m_xInsertPos + m_nComCursorPos;
			}
		}
	}
	else
	{
		// �������� ���ڿ��� �����ϴ�.
		m_nComSize = 0;
	}

	ImmReleaseContext(this->m_hWnd, hIMC);

	m_IsOnChar = true;

	ShowCaretOnView();
	Invalidate();
	return TRUE;
}

/*****************************************************************************\
* �Լ�: OnImeEndComposition
*
* WM_IME_ENDCOMPOSITION �޽����� �ް� ���������� ��Ĩ�ϴ�.
*
* �μ�:
*	WPARAM wParam - ������� �ʽ��ϴ�. 
*	LPARAM lParam - ������� �ʽ��ϴ�.
\*****************************************************************************/
void CIMEEdit::OnImeEndComposition(WPARAM, LPARAM)
{
	// ���� �÷��׸� ���ϴ�.
	m_fShowInvert = FALSE;

	// �ѱ��� ���� ������ ��� ĳ���� �Ϲ� ��Ÿ�Ϸ� �����մϴ�.
	if (m_nLanguage == language::KOREAN)
	{
		m_bWideCaret = false;

		CreateSolidCaret(1,FONT_HEIGHT);
	}

	m_IsOnChar = true;

	ShowCaret();
}

/*****************************************************************************\
* �Լ�: InsertCompStr
*
* ���ڿ� ���ۿ� �������� ���ڿ��� �����մϴ�.
*
* �μ�:
*	WPARAM wParam - ������� �ʽ��ϴ�. 
*	LPARAM lParam - ������� �ʽ��ϴ�.
*
* ��ȯ ��:
*	TCHAR���� �������� ���ڿ��� ũ���Դϴ�.
\*****************************************************************************/
int CIMEEdit::InsertCompStr()
{
	HIMC	hIMC;
	int		nTmpPos, nTmpInc;
	wchar_t	szTmpStr[BUFFERSIZE+1]={0};

	// ���� ��ġ ���� ���� ���ڿ��� ����մϴ�.
	nTmpPos = m_xEndPos - m_xInsertPos;
	memcpy(szTmpStr, &m_szBackup[m_xInsertPos], WCHARSIZE * nTmpPos);
	szTmpStr[nTmpPos] = L'\0';

	// ���� �����÷θ� Ȯ���մϴ�.
	if ((m_nComSize % 2) != 0)
		m_nComSize++;
	nTmpInc = m_nComSize / WCHARSIZE;

	if ((m_xEndPos + nTmpInc) < 0)
		return 0;

	if ((m_xEndPos + nTmpInc) >= BUFFERSIZE)
	{
		if (m_xInsertPos > 0)
		{
			// ���� ��ġ ���� ���� ���ڿ��� �����մϴ�.
			memcpy(m_szBackup, szTmpStr, WCHARSIZE * (BUFFERSIZE+1));
			ClearBuffer();
			m_xEndPos = nTmpPos;
		}
		else
		{
			// �������� ���ڿ��� ����մϴ�.
			hIMC = ImmGetContext(this->m_hWnd);
			if (hIMC)
			{
				ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				ImmReleaseContext(this->m_hWnd,hIMC);
				ClearBuffer();
			}
		}
	}

	// �������� ���ڿ��� ���ڿ� ���۷� �����մϴ�.
	memcpy(&m_szBuffer[m_xInsertPos], m_szComStr, m_nComSize);

	// ���� ��ġ ���� ���� ���ڿ��� �����մϴ�.
	memcpy(&m_szBuffer[m_xInsertPos+nTmpInc], szTmpStr, WCHARSIZE * nTmpPos);
	m_szBuffer[m_xEndPos + nTmpInc] = L'\0';

	return nTmpInc;
}

void CIMEEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int		nTmpDec;
	wchar_t wChar, szTmpStr[BUFFERSIZE+1]={0};

	// ���� ���¸� Ȯ���մϴ�.
	//if (m_fStat)
	//	return;

	switch(nChar)
	{
		// Return key
		/*
		case _T('\r') :
		{
		// ���� ��ġ ���� ���� ���ڿ��� �����մϴ�.
		//memcpy(m_szBuffer, &m_szBuffer[m_xInsertPos], WCHARSIZE * (m_xEndPos - m_xInsertPos));
		//m_xEndPos = m_xEndPos - m_xInsertPos;
		//m_xInsertPos = m_xCaretPos = 0;
		//m_szBuffer[m_xEndPos] = L'\0';
		m_bEnterKeyDown = true;
		}
		break;
		*/

		// Back space key
	case _T('\b') :
		{
			// �� ���ڸ� �����մϴ�.
			if(m_xInsertPos == 0) break;
			nTmpDec = GetCombinedCharLength(m_xInsertPos-1);

			memcpy(&m_szBuffer[m_xInsertPos-nTmpDec], &m_szBuffer[m_xInsertPos], WCHARSIZE * (m_xEndPos - m_xInsertPos));
			m_xEndPos -= nTmpDec;
			m_xInsertPos -= nTmpDec;
			m_xCaretPos -= nTmpDec;
			m_szBuffer[m_xEndPos] = L'\0';
		}
		break;

	default :
		{
			bool bInputReturn = false;

			if ( nChar == _T('\r') )
			{
				m_bEnterKeyDown = true;
				bInputReturn	= true;

				if ( !m_bMultiLine )
				{
					break;
				}
			}

			if( m_bKeyUse )
			{
				// Note : ���๮�� ����
				if ( !bInputReturn && nChar < TCHAR(0x20) )
					break;
#ifdef _UNICODE
				wChar = nChar;
#else
				MultiByteToWideChar(m_nCodePage, 0, (char *)&nChar, 1, &wChar, 2);
#endif
				if ( !bInputReturn )
				{
					// ���� �����÷θ� Ȯ���մϴ�.
					if( m_xEndPos > (BUFFERSIZE-1) )	
						ClearBuffer();

					// ���ڿ� ���۷� ���ڸ� �����մϴ�.
					memcpy(szTmpStr, &m_szBuffer[m_xInsertPos], WCHARSIZE * (m_xEndPos - m_xInsertPos));
					szTmpStr[m_xEndPos - m_xInsertPos] = L'\0';
					m_szBuffer[m_xInsertPos] = wChar;
					memcpy(&m_szBuffer[m_xInsertPos+1], szTmpStr, WCHARSIZE * (m_xEndPos - m_xInsertPos));
					m_xEndPos++;
					m_xInsertPos++;
					m_xCaretPos++;
					m_szBuffer[m_xEndPos] = L'\0';

				}
				// Note : ���๮��
				else
				{
					// ���� �����÷θ� Ȯ���մϴ�.
					if( m_xEndPos > (BUFFERSIZE-2) )	
						ClearBuffer();

					memcpy(szTmpStr, &m_szBuffer[m_xInsertPos], WCHARSIZE * (m_xEndPos - m_xInsertPos));
					szTmpStr[m_xEndPos - m_xInsertPos] = L'\0';
					m_szBuffer[m_xInsertPos  ] = L'\r';
					m_szBuffer[m_xInsertPos+1] = L'\n';
					memcpy(&m_szBuffer[m_xInsertPos+2], szTmpStr, WCHARSIZE * (m_xEndPos - m_xInsertPos));
					m_xEndPos	 += 2;
					m_xInsertPos += 2;
					m_xCaretPos	 += 2;
					m_szBuffer[m_xEndPos] = L'\0';
				}
			}
		}
		break;
	}

	memcpy( m_szBackup, m_szBuffer, WCHARSIZE * (BUFFERSIZE+1) );

	m_IsOnChar = true;

	ShowCaretOnView();
	Invalidate();
}

void CIMEEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int nTmpDec;

	// IME �Ӽ��� IME_PROP_AT_CARET�� ������ �������� �߿� ���ڸ� ���� �ʽ��ϴ�.
	if ( !( m_dwProperty & IME_PROP_AT_CARET ) && m_fStat )
		return;

	if( DXInputString::m_bCaratMove == FALSE )
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
		m_IsOnKeyDown = nChar;
		return;
	}

	switch( nChar )
	{

	case VK_LEFT:	// Left arrow key
		if(m_xInsertPos == 0) break;
		m_xInsertPos -= GetCombinedCharLength( m_xInsertPos - 1 );
		m_xCaretPos = m_xInsertPos;
		ShowCaretOnView();
		break;

	case VK_RIGHT:	// Right arrow key
		if(m_xInsertPos == m_xEndPos) break;
		m_xInsertPos +=  GetCombinedCharLength(m_xInsertPos);
		m_xCaretPos =  m_xInsertPos;
		ShowCaretOnView();
		break;

	case VK_DELETE:	// Delete key
		if ((m_xInsertPos == m_xEndPos) || (m_xEndPos > BUFFERSIZE)) break;
		nTmpDec = GetCombinedCharLength(m_xInsertPos);
		memcpy(&m_szBuffer[m_xInsertPos], &m_szBuffer[m_xInsertPos+nTmpDec], WCHARSIZE * (m_xEndPos - m_xInsertPos - nTmpDec));
		m_xEndPos -= nTmpDec;
		m_szBuffer[m_xEndPos] = L'\0';
		Invalidate();

		// Note : DelŰ�� Ű �Է����� ����.
		m_IsOnChar = true;

		break;

	case VK_HOME:	// Home key
		if (m_xInsertPos == 0) break;
		m_xInsertPos = m_xCaretPos = 0;
		ShowCaretOnView();
		break;

	case VK_END:	// End key
		if (m_xInsertPos == m_xEndPos) break;
		m_xInsertPos = m_xCaretPos = m_xEndPos;
		ShowCaretOnView();
		break;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	m_IsOnKeyDown = nChar;
}

/****************************************************************************\
* �Լ�: SetIMEMode
*
* IME ��带 �����մϴ�.
*
* �μ�:
*	HWND hWnd - �׽�Ʈ â�� �ڵ��Դϴ�.
*	DWORD dwNewConvMode - ��ȯ ��带 �����ϴ� �ɼ��Դϴ�.
*	DWORD dwNewSentMode - Sentence ��带 �����ϴ� �ɼ��Դϴ�.
*	BOOL fFlag - TRUE�̸� ���޵� �ɼ��� �����ϰ�, �׷��� ������ �ɼ��� �����մϴ�.
\**************************************************************************** */
void CIMEEdit::SetIMEMode ( HWND hWnd, DWORD dwNewConvMode, DWORD dwNewSentMode, BOOL fFlag )
{
	//	return ;

	HIMC	hImc;
	DWORD	dwConvMode, dwSentMode;
	BOOL	fRet;

	//�Է� ���ؽ�Ʈ�� �����ɴϴ�.
	hImc = ImmGetContext(hWnd);
	if (hImc)
	{
		//���� IME ���¸� �����ɴϴ�.
		ImmGetConversionStatus ( hImc, &dwConvMode, &dwSentMode );

		//���޵� �ɼ��� ����Ͽ� IME ���¸� �����մϴ�. 
		if (fFlag)
		{
			fRet = ImmSetConversionStatus ( hImc, dwConvMode | dwNewConvMode, dwSentMode | dwNewSentMode );
			//			if ( (GetLanguage()==JAPANESE) && (dwNewConvMode&IME_CMODE_NATIVE) )
			//			{
			// Note : �Ϻ��� �Է¸�忡�� �׻� �����Է� ���� �����ϱ� ���� !�� �ٿ���.
			//				ImmSetOpenStatus ( hImc, !fFlag );
			//			}
		}
		else
		{
			ImmSetConversionStatus ( hImc, dwConvMode&~dwNewConvMode, dwSentMode&~dwNewSentMode );
			//			if ( (GetLanguage()==JAPANESE) && (dwNewConvMode&IME_CMODE_NATIVE) )
			//			{
			//				ImmSetOpenStatus ( hImc, fFlag );
			//			}
		}

		//�Է� ���ؽ�Ʈ�� �����մϴ�. 
		ImmReleaseContext ( hWnd, hImc );
	}
}

BOOL CIMEEdit::IsNativeMode ()
{
	HWND hWnd = GetSafeHwnd();

	HIMC hIMC = ImmGetContext(hWnd);
	if ( hIMC )
	{
		DWORD dwComposition, dwSentence;

		ImmGetConversionStatus(hIMC, &dwComposition, &dwSentence);

		// Release input context
		ImmReleaseContext ( hWnd, hIMC );

		if ( (dwComposition&IME_CMODE_NATIVE)!=NULL ) return TRUE;
	}

	return FALSE;
}

/*****************************************************************************\
* �Լ�: SetFont
*
* IME â���� WM_INPUTLANGCHANGE �޽����� �޽��ϴ�.
* 
* �� ���ÿ����� �۲� �Ǵ� �ڵ� �������� �������� �ʰ� ��� ���ƽþ�
* �� ���� ���� �ٸ� �� ���� IME�� ����� �� �ֵ���, Ű���� ���̾ƿ�����
* ANSI-�����ڵ� ��ȯ�� ���Ǵ� �۲� �� �ڵ� �������� Ȯ���մϴ�.
* IME ���� 3�� �����ϱ� ���� �䱸 ������ �ƴմϴ�.
*
* �μ�:
*	HKL  hKeyboardLayout - �� Ű���� ���̾ƿ��� ���� ������ �����մϴ�.
*	wchar_t *szSelectedFont - �۲� �̸��Դϴ�. 
\*****************************************************************************/

typedef HRESULT (WINAPI *PTF_INSTALLAYOUTPRIP)(LPCWSTR sz, DWORD dwFlags);
;

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

void CIMEEdit::SetFont(HKL  hKeyboardLayout, LPCTSTR szSelectedFont)
{
	// ���� ��ġ�� ����;
	/*
	const int layoutCount = ::GetKeyboardLayoutList(0, NULL);
	HKL* pHKLList = new HKL[layoutCount];
	ZeroMemory(pHKLList, sizeof(HKL)*layoutCount);
	::GetKeyboardLayoutList(layoutCount, pHKLList);

// 	HKL hKlList[255] = {0};
// 	::GetKeyboardLayoutList(255, hKlList);

	for ( int i = 0; pHKLList[i] != NULL && i < 255; ++i )
	{
		char szImeFileName[255] = {0}, szImeDescription[255] = {0};
		::ImmGetIMEFileNameA(pHKLList[i], szImeFileName, 255);
		::ImmGetDescriptionA(pHKLList[i], szImeDescription, 255);
		sc::writeLogInfo(sc::string::format("Installed KeyboardLayout IME:%1%", szImeFileName));
		sc::writeLogInfo(sc::string::format("Installed KeyboardLayout Description:%1%", szImeDescription));
	}
	
	delete []pHKLList;
	*/

	sc::writeLogInfo("CIMEEdit::SetFont");
	if (szSelectedFont)		
	{
		// ������ �۲��� �����մϴ�. 
#ifdef _UNICODE
		StringCchCopy( m_szWFontName, 50, szSelectedFont );
		WideCharToMultiByte(m_nCodePage, 0, m_szWFontName, -1, (char *)m_szMBFontName, 50, NULL, NULL);
#else
		StringCchCopy( m_szMBFontName, 50, szSelectedFont );
		MultiByteToWideChar(m_nCodePage, 0, (char *)szSelectedFont, (int)strlen(szSelectedFont)+1, m_szWFontName, 50);
#endif
	}
	else
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			m_nLanguage = RENDERPARAM::emLangSet;
			if ( m_nLanguage == language::DEFAULT)
			{
				if (hKeyboardLayout == 0)
					hKeyboardLayout = GetKeyboardLayout(0);

				switch (LOWORD(hKeyboardLayout))
				{
				case LID_TRADITIONAL_CHINESE:	m_nLanguage = language::TRADITIONAL_CHINESE; break;
				case LID_JAPANESE:				m_nLanguage = language::JAPANESE;			 break;
				case LID_KOREAN:				m_nLanguage = language::KOREAN;				 break;
				case LID_SIMPLIFIED_CHINESE:	m_nLanguage = language::TRADITIONAL_CHINESE;	 break;
				case LID_THAILAND:				m_nLanguage = language::THAILAND;			 break;
				case LID_VIETNAM:				m_nLanguage = language::VIETNAM;			 break;
				default:						m_nLanguage = language::DEFAULT;			 break;
				}
			}

			// OS üũ;
			BOOL bOsVersionInfoEx;
			OSVERSIONINFOEX osviex;
			OSVERSIONINFO osvi;
			osviex.dwOSVersionInfoSize = 156;
			bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osviex);
			osvi.dwOSVersionInfoSize = 148;
			bOsVersionInfoEx = GetVersionEx(&osvi);

			BOOL bVistaLow = FALSE, bVista = FALSE;
			OSVERSIONINFOEX osViEx;
			SYSTEM_INFO si;
			PGNSI pGNSI;
			PGPI pGPI;

			//	ZeroMemory(&osViEx, sizeof(OSVERSIONINFOEX));
			//	ZeroMemory(&si, sizeof(SYSTEM_INFO));
			//	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osViEx);

			// GetProcAddress�� xp�̻� �������� �����ϴ°Ͱ���
			pGNSI = (PGNSI) GetProcAddress(
				GetModuleHandle(TEXT("kernel32.dll")), 
				"GetNativeSystemInfo");
			if(NULL != pGNSI)
				pGNSI(&si);
			else GetSystemInfo(&si);

			if (osviex.dwMajorVersion == 5 && osviex.dwMinorVersion == 1)
			{
				bVistaLow = TRUE;
				sc::writeLogInfo("Window Xp");
				if( osviex.wSuiteMask & VER_SUITE_PERSONAL )
					sc::writeLogInfo("Home Edition");
				else sc::writeLogInfo("Professional");
			}
			if ( osviex.dwMajorVersion == 5 && osviex.dwMinorVersion == 2 )
			{
				if( osviex.wProductType == VER_NT_WORKSTATION &&
					si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				{
					sc::writeLogInfo("Windows XP Professional x64 Edition");
					bVistaLow = TRUE;
				}
			}
			if ( VER_PLATFORM_WIN32_NT==osviex.dwPlatformId && 
				osviex.dwMajorVersion > 4 )
			{
				if( osviex.dwMinorVersion == 0 )
				{
					if( osviex.wProductType == VER_NT_WORKSTATION )
					{
						sc::writeLogInfo("Windows Vista ");
						/*bVistaLow = */bVista = TRUE;
					}
					else sc::writeLogInfo("Windows Server 2008 ");
				}
			}
			/*
			// �⺻ Ű���� ���̾ƿ� �� �о��;
			HKEY hCheckKey;
			if ( RegOpenKeyExA( HKEY_CURRENT_USER
			, "Keyboard Layout\\Preload"
			, 0
			, KEY_READ
			, &hCheckKey ) != ERROR_SUCCESS )
			{
			GASSERT( FALSE );
			RegCloseKey(hCheckKey);
			sc::writeLogInfo("Default Keyboard layout Read Error!!");
			}
			else
			{
			TCHAR    achKey[255];   // buffer for subkey name
			DWORD    cbName;							// size of name string 
			TCHAR    achClass[MAX_PATH] = TEXT("");		// buffer for class name 
			DWORD    cchClassName = MAX_PATH;			// size of class string 
			DWORD    cSubKeys=0;						// number of subkeys 
			DWORD    cbMaxSubKey;						// longest subkey size 
			DWORD    cchMaxClass;						// longest class string 
			DWORD    cValues;              // number of values for key 
			DWORD    cchMaxValue;          // longest value name 
			DWORD    cbMaxValueData;       // longest value data 
			DWORD    cbSecurityDescriptor; // size of security descriptor 
			FILETIME ftLastWriteTime;      // last write time 

			DWORD i, retCode; 

			TCHAR  achValue[255]; 
			DWORD cchValue = 255; 

			// Get the class name and the value count. 
			retCode = RegQueryInfoKey(
			hCheckKey,       // key handle 
			achClass,                // buffer for class name 
			&cchClassName,
			NULL,
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cValues,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 
			if ( cValues )
			{
			for ( int i = 0; i < cValues; ++i )
			{
			char szValueName[20], szData[255];
			DWORD dwSize = 255;
			_itoa_s(i, szValueName, 20, 10);

			if ( RegQueryValueExA(hCheckKey, szData, NULL, NULL, NULL, &dwSize) == ERROR_SUCCESS )
			{
			const LRESULT iSuccess = RegQueryValueExA(hCheckKey, szData, NULL, NULL, (BYTE*)szData, &dwSize);
			if ( iSuccess == ERROR_SUCCESS )
			{
			const int keyboard_layout = atoi( (char*)szData );
			sc::writeLogInfo(sc::string::format("Keyboard Layout Num : %1%, Value : %2%", i, szData));
			}
			}
			int iData = atoi(szData);
			sc::writeLogInfo(sc::string::format("%1%", iData));
			}
			}
			}

			RegCloseKey(hCheckKey);
			*/
			// Ű���� ���̾ƿ��� Ȯ���մϴ�.
			if (hKeyboardLayout == 0)
				hKeyboardLayout = GetKeyboardLayout(0);

			switch (LOWORD(hKeyboardLayout))
			{
			case LID_TRADITIONAL_CHINESE:	m_nLanguage = language::TRADITIONAL_CHINESE; break;
			case LID_JAPANESE:				m_nLanguage = language::JAPANESE;			 break;
			case LID_KOREAN:				m_nLanguage = language::KOREAN;				 break;
			case LID_SIMPLIFIED_CHINESE:	m_nLanguage = language::TRADITIONAL_CHINESE;	 break;
			case LID_THAILAND:				m_nLanguage = language::THAILAND;			 break;
			case LID_VIETNAM:				m_nLanguage = language::VIETNAM;			 break;
			default:						m_nLanguage = language::DEFAULT;			 break;
			}
			m_hKeyboardLayout = hKeyboardLayout;
		}		

		sc::writeLogInfo("Reg Done");


		// �ڵ� ������ �� ���� ������ �����մϴ�.
		if (m_nLanguage)
			m_nCodePage = CD3DFontPar::nCodePage[m_nLanguage];
		else
			m_nCodePage = GetACP();

		m_nCharSet = CD3DFontPar::nCharSet[m_nLanguage];

		// �⺻ �۲��� �����մϴ�.
		StringCchCopyW( m_szWFontName, 50, (wchar_t *)CD3DFontPar::szDefaultFontName[m_nLanguage] );
		WideCharToMultiByte(m_nCodePage, 0, m_szWFontName, -1, (char *)m_szMBFontName, 50, NULL, NULL);

		//  [2/4/2014 gbgim];
		// IME �Ӽ��� �����ɴϴ�.;
		// ���⼭ ���� ���ԵǸ� �Էµ��� �Է� Ŀ�� ��ġ�� �Ⱥ��δٴ��� ��� ������ ���� �� �ֽ��ϴ�;
		m_dwProperty = ImmGetProperty( m_hKeyboardLayout, IGP_PROPERTY );
	}

	// ���� �۲��� �����մϴ�.
	if (m_hFont)
		DeleteObject(m_hFont);

	// �۲��� ����ϴ�.
	if (m_fIsNT)
	{
		m_hFont = CreateFontW(FONT_WIDTH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, m_nCharSet,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, m_szWFontName);
	}
	else
	{
		m_hFont = CreateFontA(FONT_WIDTH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, m_nCharSet,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, m_szMBFontName);
	}

	// ������ �۲� �Ǵ� �⺻ �۲��� ������ ���ϸ� �⺻ �۲��� ����ϴ�.
	if (!m_hFont)
	{
		// �⺻ �۲��� �����մϴ�.
		StringCchCopyW( m_szWFontName, 50, (wchar_t*) CD3DFontPar::szDefaultFontName[language::DEFAULT] );
		WideCharToMultiByte(m_nCodePage, 0, m_szWFontName, -1, (char *)m_szMBFontName, 50, NULL, NULL);

		if (m_fIsNT)
		{
			m_hFont = CreateFontW(FONT_WIDTH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, m_nCharSet,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, m_szWFontName);
		}
		else
		{
			m_hFont = CreateFontA(FONT_WIDTH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, m_nCharSet,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, m_szMBFontName);
		}
	}

	ShowCaretOnView();
	Invalidate();

	sc::writeLogInfo("IMESetFont is Done");
}

/*****************************************************************************\
* �Լ�: GetCombinedCharLength
*
* ���յ� ������ ���̸� �����ɴϴ�. 
*
* �� ������ ���ΰ���Ʈ�� �����մϴ�. �ʿ��� ��� ���յ� ���ڸ�
* ó���ؾ� �մϴ�.  
*
* ��ȯ ��:
*	�����ڵ��� ��� 
*		���ΰ���Ʈ �����̸� 2�� ��ȯ�ϰ�, �׷��� ������ 1�� ��ȯ�մϴ�.
*	�����ڵ尡 �ƴ� ���
*		DBCS �����̸� 2�� ��ȯ�ϰ�, �׷��� ������ 1�� ��ȯ�մϴ�.
\*****************************************************************************/
int CIMEEdit::GetCombinedCharLength(int nTmpPos) 
{
	int	i, nRet = 1;

	// ���ΰ���Ʈ ���ڸ� Ȯ���մϴ�.
	for (i = 0; i <= nTmpPos; i++)
	{
		if ((0xD800 <= m_szBuffer[i]) && (m_szBuffer[i] <= 0xDBFF))
		{
			nRet = 2;
			++i;
		}
		else
		{
			// Note : ���� ������ ���
			if ( m_szBuffer[i] == L'\r' )
			{
				if ( i+1 <= nTmpPos && m_szBuffer[i+1] == L'\n' )
				{
					nRet = 2;
					++i;
					continue;
				}
			}

			nRet = 1;
		}
	}

	return nRet;
}

/*****************************************************************************\
* �Լ�: ClearBuffer
*
* ���ڿ� ���۸� ����ϴ�.
\*****************************************************************************/
void CIMEEdit::ClearBuffer()
{
	SecureZeroMemory( m_szBuffer, sizeof(m_szBuffer) );

	m_xEndPos = m_xInsertPos = m_xCaretPos = 0;
}


/*****************************************************************************\
* �Լ�: SetCandiDateWindowPos
*
* �ѱ���/�Ϻ��� IME�� �ĺ� â ��ġ�� �����մϴ�.
\*****************************************************************************/
void CIMEEdit::SetCandiDateWindowPos() 
{
	HIMC		hIMC;
	CClientDC	dc(this);
	TEXTMETRIC	tm;
	CANDIDATEFORM Candidate;

	hIMC = ImmGetContext(this->m_hWnd);
	if (hIMC) 
	{
		dc.GetTextMetrics(&tm);

		Candidate.dwIndex = 0;
		Candidate.dwStyle = CFS_EXCLUDE;

		ImmSetCandidateWindow(hIMC, &Candidate);
		ImmReleaseContext(this->m_hWnd,hIMC);

		// Note : �Ʒ� �ڵ�� ���� �ڵ� ����
		//dc.GetTextMetrics(&tm);

		//Candidate.dwIndex = 0;
		//Candidate.dwStyle = CFS_FORCE_POSITION;

		//if (m_nLanguage == JAPANESE) 
		//{
		//	// ���� ������ ���� �����̿� �ĺ� â ��ġ�� �����մϴ�.
		//	wchar_t *szTmpStr = new(wchar_t[m_xInsertPos + m_nComCursorPos + 1]);
		//	memcpy(szTmpStr, m_szBuffer, WCHARSIZE * (m_xInsertPos + m_nComCursorPos));	 
		//	szTmpStr[m_xInsertPos + m_nComCursorPos] = L'\0';
		//	Candidate.ptCurrentPos.x = X_INIT + GetWidthOfString(szTmpStr);
		//}
		//else
		//{
		//	// ĳ�� ��ġ �����̿� �ĺ� â ��ġ�� �����մϴ�.
		//	CPoint		point;

		//	point = GetCaretPos();
		//	Candidate.ptCurrentPos.x = point.x;
		//}
		//Candidate.ptCurrentPos.y = Y_INIT + tm.tmHeight + 1;
		//ImmSetCandidateWindow(hIMC, &Candidate);

		//ImmReleaseContext(this->m_hWnd,hIMC);
	}
}

/*****************************************************************************\
* �Լ�: SetCompositionWindowPos
*
* �߱��� ��ü IME�� �������� â ��ġ�� �����մϴ�.
\*****************************************************************************/
void CIMEEdit::SetCompositionWindowPos() 
{
	HIMC		hIMC;
	CPoint		point;
	COMPOSITIONFORM Composition;

	hIMC = ImmGetContext(this->m_hWnd);
	if (hIMC)
	{
		// ĳ�� ��ġ �����̿� �������� â ��ġ�� �����մϴ�.
		point = GetCaretPos();
		Composition.dwStyle = CFS_POINT;
		Composition.ptCurrentPos.x = point.x;
		Composition.ptCurrentPos.y = point.y;
		ImmSetCompositionWindow(hIMC, &Composition);

		ImmReleaseContext(this->m_hWnd,hIMC);
	}
}

LRESULT CIMEEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
		// �Է� �� ����Ǹ� ������ �����մϴ�.
	case WM_INPUTLANGCHANGE:
		{
			// IME ����� ���۰� Ŭ���� �Ǿ ���� �Է��� �������.
			// ���� �ڵ忡�� �ּ� ó���� �ߴ�. ������ ���ٸ� �̷��� ����Ѵ�.
			//
			//ClearBuffer(); 

			// ����� �Է� �� ���� �� �۲��� ����ϴ�.
			sc::writeLogInfo("CIMEEdit::WindowProc");
			SetFont((HKL) lParam, NULL);
			Invalidate();
			ShowCaretOnView();

			// IME ��� ���濡 ���� �޽����� �����ϴ�.
			//::PostMessage(GetParent()->m_hWnd, WM_SETINPUTLANG, 0L, 0L);
		}
		break;

		// ���������� �����մϴ�.
	case WM_IME_STARTCOMPOSITION:
		{
			// �������� �÷��׸� �մϴ�.
			m_fStat = TRUE;

			// IME �Ӽ��� IME_PROP_AT_CARET�� ������ ���� 3 ����� �����մϴ�.
			if ( !(m_dwProperty & IME_PROP_AT_CARET) )
				break;

			OnImeStartComposition(wParam, lParam);
		}
		return 0l;

		// �������� �����Դϴ�.
	case WM_IME_COMPOSITION:
		{
			if (OnImeComposition(wParam, lParam))
				return 0l;
			// CEdit::WinProc�� ȣ���Ͽ� �������� â�� ǥ���մϴ�.
		}
		break;

		// ���������� �����ϴ�.
	case WM_IME_ENDCOMPOSITION:
		{
			// �������� �÷��׸� ���ϴ�.
			m_fStat = FALSE;

			// IME �Ӽ��� IME_PROP_AT_CARET�� ������ ���� 3 ����� �����մϴ�.
			if ( !(m_dwProperty & IME_PROP_AT_CARET) )
				break;

			OnImeEndComposition(wParam, lParam);
		}
		return 0l;

	case WM_IME_NOTIFY:
		switch (wParam)
		{
			// �ĺ� â ��ġ�� �����մϴ�.
		case IMN_OPENCANDIDATE:
			if (m_nLanguage == language::JAPANESE)
				SetCandiDateWindowPos();
			break;

			//// IME ����� ������ �����Ϸ��� 
			//case IMN_SETCONVERSIONMODE:
			//case IMN_SETSENTENCEMODE:
			//	::PostMessage(GetParent()->m_hWnd, WM_RESETMODE, wParam, lParam);
			//	break;

			//	// �Ϻ��� IME�� ��ȯ�� �����Ϸ���
			//case IMN_SETOPENSTATUS:   
			//	::PostMessage(GetParent()->m_hWnd, WM_TOGGLE, wParam, lParam);
			//	break;
		}	
		break;

		//case WM_IME_SETCONTEXT:
		//	// �Ϻ��� Win9x�� IME ��� ��� �����ϰ� �� �޽����� ���� �ʿ䰡 �����ϴ�.
		//	if ((m_nLanguage ==  JAPANESE) && !m_fIsNT) 				
		//	{
		//		::PostMessage(GetParent()->m_hWnd, WM_SETMODE, wParam, lParam);
		//	}
		//	break;
	}

	return CEdit::WindowProc(message, wParam, lParam);
}

bool CIMEEdit::CheckEnterKeyDown ()
{
	bool bKEYDOWN = m_bEnterKeyDown;
	m_bEnterKeyDown = false;

	return bKEYDOWN;
}