#pragma once

#define	EF_NEW_USER					"1-1.New_Users.txt"
#define	EF_NEW_CHAR					"1-2.New_Characters.txt"
#define	EF_DISTINCT_USER			"1-3.Distinct_Users.txt"
#define	EF_SHOP_INCOME				"1-4.Shop_Income.txt"
#define	EF_GAME_TIME				"2-1.Game_Time.txt"
#define	EF_CONCURRENT_USRES			"2-2.Concurrent_Users.txt"
#define	EF_CHAR_LEVEL				"2-3.Character_Levels.txt"
#define	EF_IP_2_NATION				"2-4.IP_to_Nations.txt"
#define	EF_IP_2_CCODE				"3-1.IP_to_CCode.txt"
#define	EF_IP_2_CC_DETAIL			"3-2.IP_to_CCode_Detail.txt"
#define	EF_IP_2_CC_UID				"3-3.IP_to_CCode_UID.txt"
#define	EF_DAILY_GAMETIME			"4-1.Daily_GameTime.txt"
#define	EF_DAILY_GAMETIME_DETAIL	"4-2.Daily_GameTime_Detail.txt"


class CExport2File {

public:
	CExport2File(TCHAR *pFileName)
	{
        if (pFileName)
		{
			ZeroMemory(m_szFileName, sizeof(m_szFileName));
			StringCchCopy(m_szFileName, sizeof(m_szFileName), pFileName);

			m_pf = NULL;
			m_pf = _tfopen(pFileName, "w+b");

			if (m_pf)
				CConsoleMessage::GetInstance()->PrintMsg(_T("OK: %s is ready."), pFileName);
			else
				CConsoleMessage::GetInstance()->PrintMsg(_T("ERROR: %s could not be opened."), pFileName);
		}
		else
		{
			CConsoleMessage::GetInstance()->PrintMsg(_T("ERROR: Export file name is wrong."));
		}
	}
	virtual ~CExport2File()
	{
		fclose(m_pf);
		m_pf = NULL;
	}
		
	size_t WriteStrTo(const TCHAR *pszLine)
	{
		return fwrite((const void *)pszLine, sizeof(TCHAR), _tcslen(pszLine), m_pf);
	}

	BOOL ExportFromListBox(CListBox *plbResult)
	{
		if (plbResult == NULL)
			return FALSE;
		
		int cnt = plbResult->GetCount();
		CString tmp;

		for (int i=0; i<cnt; i++)
		{			
			plbResult->GetText(i, tmp);

			WriteStrTo(tmp.GetString());
			WriteStrTo(_T("\r\n"));
		}

		return TRUE;
	}

private:
	FILE *m_pf;
	TCHAR m_szFileName[256];
};