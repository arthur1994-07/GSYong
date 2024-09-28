#include "stdafx.h"

#include "ConsoleMessage.h"

CConsoleMessage* CConsoleMessage::GetInstance()
{
	static CConsoleMessage consoleMsg;

	return &consoleMsg;
}


CConsoleMessage::CConsoleMessage()
{
	m_plistboxMsg = NULL;
}

CConsoleMessage::~CConsoleMessage()
{
}

void CConsoleMessage::PrintMsg( const TCHAR *format, ... )
{
	if ( m_plistboxMsg != NULL && format != NULL )
	{	
		TCHAR buffer[512]={0,};
		va_list args;

		va_start( args, format );
		
		_vstprintf( buffer, format, args );
		
		m_plistboxMsg->AddString( buffer );
		
		// Select the last item in the list box.
		int cnt = m_plistboxMsg->GetCount();
		if( cnt > 0 )
			m_plistboxMsg->SetCurSel(cnt - 1);
	}
	else
		MessageBox(NULL, _T("message-console or format is null."), _T("WARNING"), MB_OK);
}

void CConsoleMessage::PrintTo( CListBox *plbResult, const TCHAR *format, ... )
{
	if ( plbResult != NULL && format != NULL )
	{	
		TCHAR buffer[512]={0,};
		va_list args;

		va_start( args, format );

		_vstprintf( buffer, format, args );

		plbResult->AddString( buffer );
		
		// Select the last item in the list box.
		int cnt = plbResult->GetCount();
		if( cnt > 0 )
			plbResult->SetCurSel(cnt - 1);
	}
	else
		MessageBox(NULL, _T("result-console or format is null."), _T("WARNING"), MB_OK);
}