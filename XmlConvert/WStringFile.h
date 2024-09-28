
#pragma once

#include <vector>
#include <boost/lexical_cast.hpp>
#include <stdio.h>

class WStringFile
{
private:
	std::vector<std::string>	m_strSTRING;
	std::vector<std::wstring>	m_wstrSTRING;
	std::wstring				m_wstrORIGIN;
	INT							m_nCurrentLine;

public:
	WStringFile():m_nCurrentLine(0)		{}
	~WStringFile()		{}

public:
	const bool LoadFile( const char* szPath )
	{
		m_strSTRING.reserve( 2000 );
		std::stringstream	ss;
		std::ifstream		fin(szPath);
		ss << fin.rdbuf();
 
		std::string const &s = ss.str();
		m_wstrORIGIN.resize(s.size()/*/sizeof(wchar_t)*/);
		std::memcpy( &m_wstrORIGIN[0], s.c_str(), s.size());
		printf("\n s.size() is %d", s.size());

		char szValue[262143];
		std::memcpy( szValue, s.c_str(), s.size());
//		std::string szValue;
//		WideCharToMultiByte(CP_UTF8, 0, m_wstrORIGIN.c_str(), -1, szValue, sizeof(szValue)/sizeof(sizeof(szValue[0])), NULL,NULL );
		std::string strSEPERATOR( "\r\n" );
//		std::wstring strSEPERATOR( L"\r\n");
		boost::split( m_strSTRING, szValue, boost::is_any_of(strSEPERATOR.c_str()) );


		std::wstringstream	ws;
		std::wifstream		wfIn(szPath);
		ws << wfIn.rdbuf();
		std::wstring w = ws.str();
		std::wstring wstrSEPERATOR( L"\r\n" );
		std::vector<std::wstring> vecstrSTRING;
		boost::split(m_wstrSTRING,w, boost::is_any_of(wstrSEPERATOR.c_str()));

		return true;
		std::vector<std::string>::iterator	itr = m_strSTRING.begin();
		std::vector<std::string>::iterator	End = m_strSTRING.end();
		for ( ; itr != End; ++itr )
		{
			std::string& str = (*itr);
			unsigned char szBOM[4];
			strncpy( (char*)szBOM, str.c_str(), 4 );
			if ( szBOM[0] == 0xEF && szBOM[1] == 0xBB )
			{
				// 			char szBOMDELETE[512];
				// 			char* szOrigin = const_cast<char*>(str.c_str());
				// 			strcpy( szBOMDELETE, &szOrigin[4] );
				// 			str = szBOMDELETE;
				int dummy = 0;
				dummy = 0;
			}
			else
			{
// 				char		szCONVERT[512];
// 				strcpy( szCONVERT, const_cast<char*>(str.c_str()) );
// 				wchar_t		wszTEMP[512];
// 				MultiByteToWideChar(CP_UTF8,0
// 					, szCONVERT
// 					, static_cast<INT>(str.size())+1
// 					, wszTEMP
// 					, sizeof(wszTEMP)/sizeof(wszTEMP[0]));			
// 				WideCharToMultiByte(CP_UTF8, 0
// 					, wszTEMP
// 					, -1, szCONVERT
// 					, sizeof(wszTEMP)/sizeof(wchar_t)
// 					, NULL,NULL );				
// 				str = szCONVERT;
//				MessageBoxA( NULL, "다음 USC-2 lua데이터가 UTF-8로 변형되지않앗습니다;", "ERROR", MB_OK );
			}
		}

		//FF FE = UCS-2 (16bit) little endian
// 		INT ByteOderMARK = 0x0000FEFF;
// 		std::vector<std::string>::iterator	Itr = m_strSTRING.begin();
// 		std::vector<std::string>::iterator	End = m_strSTRING.end();
// 		for ( ; Itr != End; Itr++ )
// 		{
// 			std::string& str = (*Itr);
// 			INT iFind;
// 			if ( ( iFind = str.find(reinterpret_cast<char*>(&ByteOderMARK)) ) != std::string::npos )
// 			{
// 				char ch[500];
// 			}
// 		}

		m_nCurrentLine = 0;
		return true;
	}

	const bool GetNextLine( std::string& str, std::wstring& wstr )
	{
		if ( m_nCurrentLine >= (INT)m_wstrSTRING.size() )
			return false;

		char szData[4096] = {0};		
 		strcpy(szData, const_cast<char*>(m_strSTRING[m_nCurrentLine++].c_str()));
		wchar_t wData[4096] = {0};
//		wcsncpy( wData, const_cast<wchar_t*>(m_wstrSTRING[m_nCurrentLine].c_str()), m_wstrSTRING[m_nCurrentLine].size());
		MultiByteToWideChar(CP_UTF8, 0, szData, strlen(szData)+1,wData,sizeof(wData)/sizeof(wData[0]));			
		WideCharToMultiByte(CP_UTF8, 0, wData, -1, szData, sizeof(szData)/sizeof(szData[0]), NULL,NULL );

		str = szData;
		wstr = wData;
		m_nCurrentLine++;
//		str = m_strSTRING[m_nCurrentLine++];
		return true;
	}
};