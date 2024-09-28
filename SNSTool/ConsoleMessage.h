#pragma once




//mjeon
//This ConsoleMessage class is different from RAN server but designed for simle usage.

class CConsoleMessage {
public:
	static CConsoleMessage * GetInstance();
	static void ReleaseInstance() {;}

	//SetConsole MUST be called before using of CConsole object.
	void SetConsole(CListBox *plistboxErrorMsg)		{ m_plistboxMsg = plistboxErrorMsg; }

	void PrintMsg( const TCHAR *szFormat, ... );
	void PrintTo( CListBox *plbResult, const TCHAR *format, ... );

private:
	CConsoleMessage ();
	~CConsoleMessage();

	CListBox *m_plistboxMsg;
};
