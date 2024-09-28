//	Note : �ý�Ʈ ��ƿ��Ƽ.
//
//	(2002.12.02), [Seperator] �ʱ� ����. JDH.
//
//
#include "stdafx.h"
#include <atlcomcli.h>
#include <ATLComTime.h>

#include "./charset.h"
#include "./StringUtil.h"
#include "./StringUtils.h"

#include "../DebugInclude.h"


namespace STRUTIL
{
	typedef std::vector<char>	VEC_STR;
	VEC_STR g_Separator;

    HMODULE m_hCheckStrDLL = NULL;
    BOOL (_stdcall *m_pCheckString)(CString) = NULL;

	void RegisterSeparator(const CString& strSeparator)
	{
		if (strSeparator.IsEmpty())
            return;

		if (strSeparator.GetLength() > 1)
		{
			MessageBox(NULL, "separator must one char.", strSeparator, MB_OK);
		}
        else
        {
		    g_Separator.push_back(strSeparator.GetAt(0));
		    std::sort(g_Separator.begin(), g_Separator.end());
        }
	}

	void ClearSeparator()
	{
		g_Separator.clear();
	}

	const int ERR_POS = -1;

	inline char* lower_bound ( char *pFirst, DWORD dwCount, char cFIND )
	{
		for (; 0 < dwCount; )
		{
			// divide and conquer, find half that contains answer
			DWORD dwCount2 = dwCount / 2;
			char *pMiddle = pFirst;
			pMiddle += dwCount2;

			if ( *pMiddle < cFIND )
				pFirst = ++pMiddle, dwCount -= dwCount2 + 1;
			else
				dwCount = dwCount2;
		}

		return (pFirst);
	}

	inline bool binary_search ( char *pFirst, DWORD dwCount, char cFIND )
	{
		const char *pFound = lower_bound ( pFirst, dwCount, cFIND );
		return (pFound!=(pFirst+dwCount) && !(cFIND < *pFound));
	}

	inline BOOL IsSeparator ( const char *_szBuffer, int _nLength, int &_nSepLength )
	{
		_nSepLength = ERR_POS;
		if ( _szBuffer==NULL || _szBuffer[0]==NULL )		return FALSE;

		bool bFOUND = binary_search ( &g_Separator[0], (DWORD)g_Separator.size(), _szBuffer[0] );
		if ( bFOUND )
		{
			_nSepLength = 1;
			return TRUE;
		}

		_nSepLength = 0;
		return FALSE;
	}

	BOOL StringSeparate ( CString &Str, CStringArray &StrArray, bool bEmptyInclude )
	{
		StrArray.RemoveAll ();

		CString StrTemp;
		int nLength = Str.GetLength();
		LPCTSTR szBuffer = Str.GetString();

		for ( int i=0; i<nLength; )
		{
			char cAt = szBuffer[i];

			if ( CHARSET::IsLeadChar(cAt) )
			{
				StrTemp += cAt;
				i++;

				if ( i<nLength )
				{
					StrTemp += szBuffer[i];
					i++;
				}

				continue;
			}

			int nSepLength = ERR_POS;
			if ( IsSeparator ( szBuffer+i, nLength-i, nSepLength ) )
			{
				if ( nSepLength == ERR_POS )	return FALSE;

				i += nSepLength;

				//	�и��� ��Ʈ�� �߰�.
				if ( StrTemp.GetLength() > 0 || bEmptyInclude )
				{
					StrArray.Add ( StrTemp );
					StrTemp = "";
				}
			}
			else
			{
				StrTemp += cAt;
				i++;
			}
		}

		//	�и��� ��Ʈ�� �߰�.
		if ( StrTemp.GetLength() > 0 )
		{
			StrArray.Add ( StrTemp );
		}

		return FALSE;
	}

	void StringReplace(const std::string& s, const std::string& oldsub,
		const std::string& newsub, bool replace_all,
		std::string* res) 
	{
		if (oldsub.empty()) {
			res->append(s);  // if empty, append the given string.
			return;
		}

		std::string::size_type start_pos = 0;
		std::string::size_type pos;
		do {
			pos = s.find(oldsub, start_pos);
			if (pos == std::string::npos) {
				break;
			}
			res->append(s, start_pos, pos - start_pos);
			res->append(newsub);
			start_pos = pos + oldsub.size();  // start searching again after the "old"
		} while (replace_all);
		res->append(s, start_pos, s.length() - start_pos);
	}

	CString GetExtName( const CString& strFileName )
	{
		CString strFileExt = strFileName;

		strFileExt = strFileExt.Right( strFileExt.GetLength() - strFileExt.ReverseFind( '.' ) );
		strFileExt.MakeLower();

		return strFileExt;
	}

	BOOL ChangeExt ( const char* _szSrc, CString &strDes, const char* szDesExt )
	{
		char szSrcTemp[MAX_PATH] = "";
		StringCchCopy( szSrcTemp, MAX_PATH, _szSrc );

//		�ش��Լ��� �ѱ��� ���� ���� ������ �����Ѵ�.
//		strDes.MakeLower(); �̰ɷ� ��ü�߽��ϴ�.
//		_strlwr ( szSrcTemp );
		char* strExtension = strrchr(szSrcTemp,'.');
		if ( !strExtension )					return FALSE;

		int nLength = (int)(strExtension - szSrcTemp);
		
		for ( int i=0; i<nLength; i++ )
		{
			strDes += szSrcTemp[i];
		}
		strDes += szDesExt;
		strDes.MakeLower();

		return TRUE;
	}

	BOOL ChangeExt ( const char* _szSrc, const char* szSrcExt, CString &strDes, const char* szDesExt )
	{
		char szSrcTemp[MAX_PATH] = "";
		StringCchCopy( szSrcTemp, MAX_PATH, _szSrc );


//		�ش��Լ��� �ѱ��� ���� ���� ������ �����Ѵ�.
//		strDes.MakeLower(); �̰ɷ� ��ü�߽��ϴ�.
//		_strlwr ( szSrcTemp );
		char* strExtension = strrchr(szSrcTemp,'.');
		if ( !strExtension )					return FALSE;
		if ( _stricmp(strExtension,szSrcExt) )	return FALSE;

		int nLength = (int)(strExtension - szSrcTemp);
		
		for ( int i=0; i<nLength; i++ )
		{
			strDes += szSrcTemp[i];
		}

		strDes += szDesExt;
		strDes.MakeLower();

		return TRUE;
	}

	VOID OutputStrCsv( std::fstream &SFile, std::string &Str, bool bMargin )
	{
		// Csv ���� �ε��� �� Separater�� �����ڰ� ���ӵǸ� �����ϱ⶧���� ���� �־��
		if( Str.empty() )
		{
			SFile << " " << ",";
			return;
		}

		// ���ڿ��� ��ǥ ','�� ���Ե� �� �ֱ⶧���� ����1 ���� ���� '`'�� ��ü��
		for( std::string::iterator ci = Str.begin(); ci != Str.end(); ++ci )
			if( ( *ci ) == ',' )
				*ci = '`';

		//  [11/12/2015 gbgim];
		// Str << ","; �� ���� ���ڿ� ����� csv���� �ε�� 
		// Str���ڿ� ���� ","�� �ϳ��� �ν��ϴ� ��찡 ����;
		// �޸����̳� �ٸ� �����⿡���� �����̹Ƿ� csv ������ �Ǵ��ϰ�;
		// �Ʒ��� ���� ó����;
		if ( bMargin )
			SFile << Str << " ,";
		else
			SFile << Str << ",";
	}

	VOID InputStrCsv( CString &Src, std::string &Dest )
	{
		Dest.clear();

		// OutputStrCsv ������ �����̸� Ŭ�����ϰ� �ƴϸ� ���� ����
		if( CString::StringLength( Src ) == 0 )
			return;
		if( Src.GetAt( 0 ) == ' ' && CString::StringLength( Src ) < 2 )
			return;

		// ���ڿ��� '`'�� ���ԵǾ� ������ ��ǥ ','�� ��ü��
		Src.Replace( '`', ',' );
		Dest = Src;
	}

	BOOL CheckString(const CString& strChk)
	{
		if (strChk.FindOneOf("~!@#$%^&*+|:?><,.;[]{}()\n\t\v\b\r\a\\\?\'\" ") != -1)
			return TRUE;
        else
		    return FALSE;
	}

	BOOL CheckVietnamString(const CString& strChk)
	{
		int size = 0;
		char chBefore = '\0';
		size = strChk.GetLength();

		char strText[MAX_PATH] = {0};
		StringCchCopy(strText, MAX_PATH, strChk.GetString());

		for (int i=0; i<size; ++i)
		{
			if (CheckCharSunjo(strText[i])  &&  !CheckCharMoum(chBefore))
				return TRUE;
			chBefore = strText[i];
		}
		return FALSE;
	}

	BOOL CheckCharSunjo(const char chText)
	{
		const int nSungjo = 5;
		static const unsigned char szSungjo[nSungjo] = { 0xcc, 0xd2, 0xde, 0xec, 0xf2 }; // ����

		for ( int i = 0; i< nSungjo; ++i )
		{
			if ( chText == szSungjo[i] )
				return TRUE;
		}
		return FALSE;
	}

	BOOL CheckCharMoum(const char chText)
	{
		const int nMoum = 24;				
		/* A, a, E, e, I, i , O, o, U, u Y, y ��Ʈ�� ������ */
		static const unsigned char szMoum[nMoum] = { 0x41, 0x61, 0xc3, 0xe3, 0xc2, 0xe2, 0x45, 0x65, 0xca, 0xea,
											 0x49, 0x69, 0x4f, 0x6f, 0xd4, 0xf4, 0xd5, 0xf5, 0x55, 0x75,
											 0xdd, 0xfd, 0x59, 0x79 };

		for ( int i = 0; i< nMoum; ++i )
		{
			if ( chText == szMoum[i] )
				return TRUE;
		}
		return FALSE;
	}

	CTime CStringToCTime( CString &cs )
	{
		COleDateTime oleTime;
		CTime resultTime;

		if ( oleTime.ParseDateTime( cs ) )
		{
			SYSTEMTIME st;

			if(oleTime.GetAsSystemTime(st) )
			{
				resultTime = st;	// UTC, GMT time value
			}
			else
			{
				// ERROR HERE
				resultTime = 0;
			}
		}
		else
		{
			// ERROR HERE;
			resultTime = 0;
		}

		return resultTime;

	}

    BOOL InitThaiDLL()
    {
        if (m_hCheckStrDLL && m_pCheckString) 
            return TRUE;

	    HMODULE m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");
	    if (m_hCheckStrDLL)
	    {
		    m_pCheckString = (BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");            
            return TRUE;
	    }
        return FALSE;
    }

    BOOL ReleaseThaiDLL()
    {
        if( m_hCheckStrDLL ) 
            FreeLibrary( m_hCheckStrDLL );
        m_pCheckString = NULL;
        return TRUE;
    }

    BOOL CheckThaiString(const CString& strChk)
    {
        if (!m_pCheckString)
            return FALSE;
        return m_pCheckString(strChk);
        /*
        // �±��� OS ���� �׽�Ʈ �ؾ� ��.
        // �ѱ� OS ������ ����� �������� ����.
        // �ٺ����� �ذ�å�� strChk �� �����ڵ�� �Ѱܹ޾ƾ� ��
        std::wstring UniStr(sc::string::ansiToUnicode(strChk.GetString()));
        return sc::string::checkThaiName(UniStr);
        */
    }
} // namespace STRUTIL

void CSEPARATOR::Register ( char cSEP )
{
	m_Separator.push_back ( cSEP );
	std::sort ( m_Separator.begin(), m_Separator.end() );
}

void CSEPARATOR::Clear ()
{
	m_Separator.clear();
}

inline bool CSEPARATOR::IsSeparator ( char cVALUE )
{
	return STRUTIL::binary_search ( &m_Separator[0], (DWORD)m_Separator.size(), cVALUE );
}

bool CSEPARATOR::DoSeparate ( const std::string& strSource, std::vector<std::string> &strArray )
{
	strArray.clear();
	std::string strtemp;
	strtemp.reserve ( strSource.size() );

	//CString StrTemp;
	int nLength = (int)strSource.length();
	const char* szSOURCE = strSource.c_str();

	for ( int i=0; i<nLength; )
	{
		char cVAL = szSOURCE[i];

		if ( CHARSET::IsLeadChar(cVAL) )
		{
			strtemp += cVAL;
			++i;

			if ( i<nLength )
			{
				strtemp += szSOURCE[i];
				++i;
			}

			continue;
		}

		if ( !IsSeparator(cVAL) )
		{
			strtemp += cVAL;
			++i;
			continue;
		}

		++i;

		//	�и��� ��Ʈ�� �߰�.
		if ( !strtemp.empty() )
		{
			strArray.push_back ( strtemp );
			strtemp = "";
		}
	}

	//	�и��� ��Ʈ�� �߰�.
	if ( !strtemp.empty() )
	{
		strArray.push_back ( strtemp );
	}

	return FALSE;
}

