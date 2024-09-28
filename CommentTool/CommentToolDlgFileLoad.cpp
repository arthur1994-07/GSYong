#include "stdafx.h"

#include "../SigmaCore/String/basestring.h"
#include "../SigmaCore/String/charset.h"

#include "CommentTool.h"
#include "CommentToolDlg.h"

#include <strsafe.h>

#include "../EngineLib/G-Logic/GLogic.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CCommentToolDlg::OnFileLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Comment Setting File (*.ini)|*.ini|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".ini",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCommentToolDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		m_vecCommentData.clear();
		m_nMenuIndex = -1;
		m_nTypeIndex = -1;
		m_nDataIndex = -1;
		if( IniFileLoad( dlg.GetPathName().GetString(), true, true ) == FALSE )
		{
			MessageBox ( "Failure to Load File." );
			return;
		}
		Update();
	}
}

bool CCommentToolDlg::IniFileLoad(const STRING& strFile, bool bDEC, BOOL bPack)
{
	m_strFileName = strFile;

	char fname[_MAX_FNAME]={0};
	char ext[_MAX_EXT]={0};
	_splitpath( m_strFileName.c_str(), NULL, NULL, fname, ext );
	StringCchCat( fname, _MAX_FNAME, ext );

	std::auto_ptr<sc::CBaseString> pBString(
        GLOGIC::openfile_basestring(	
		    FALSE, 
            std::string(""), 
		    m_strFileName.c_str(), 
		    fname, 
		    bDEC,
		    bPack ) );

	if( !pBString.get() )
	{
		MessageBox ( "ERROR : CIniLoader::open(), File Open %s", fname );
		return false;
	}

	sc::CBaseString &cBString = *pBString;

	MAPKEY * pMapKey(NULL);
	STRING sectionback;

	CString strLine;
	SCommentData sCommentData;

	while( cBString.GetNextLine(strLine) )
	{
		if( !IsNotes( strLine ) ) // Notes : 주석처리
		{
			STRING section;

			if( !IsSection( strLine, section ) ) // Notes : 키와 데이터 처리, 처음에 무조건 섹션이 나와야 한다.
			{

				SCommentKeyData sCommentKeyData;
				GetKeyData( strLine, sCommentKeyData.strSubKey, &sCommentKeyData.strData ); 
				sCommentData.vecKeyData.push_back( sCommentKeyData );
			}
			else
			{
				STRING strTemp(section);

				if( sCommentData.strMainKey.empty() )
				{
					sCommentData.strMainKey = section;
				}

				if( !sCommentData.strMainKey.empty() && !sCommentData.vecKeyData.empty() )
				{
					m_vecCommentData.push_back( sCommentData );
					sCommentData.RESET();
					sCommentData.strMainKey = strTemp;
				}
			}
		}
	}

	if( !sCommentData.strMainKey.empty() && !sCommentData.vecKeyData.empty() )
	{
		m_vecCommentData.push_back( sCommentData );
		sCommentData.RESET();
	}

	return true;
}

bool CCommentToolDlg::IsNotes( const CString & line )
{
	int nLen = line.GetLength();

	for( int i = 0; i < nLen; ++i )
	{
		if( line[i] == _T(' ') || line[i] == _T('\t') ) continue;
		if( line[i] == _T(';') )
			return true;
		else
			return false;
	}

	return false;
}

bool CCommentToolDlg::IsSection( const CString & line, STRING & sec )
{
	int nStart(0);
	int nLen = line.GetLength();

	for( int i = 0; i < nLen; ++i )
	{
		if( line[i] == ' ' || line[i] == '\t' ) continue;
		if( line[i] != '[' ) return false;
		else
		{
			nStart = i;
			break;
		}
	}

	int nEnd = nLen - 1;

	for( int i = nEnd; i > 0; --i )
	{
		if( line[i] == _T(' ') || line[i] == '\t' ) continue;
		if( line[i] != ']' ) return false;
		else
		{
			nEnd = i;
			break;
		}
	}

	if( nStart > nEnd ) return false;

	sec = line.Mid( ++nStart, --nEnd );

	return true;
}

int CCommentToolDlg::GetKeyData( const CString & line, STRING & key, VECSTRING * pVec )
{
	CString strTemp( line );
	CString resToken;
	int curPos = 0;

	resToken = strTemp.Tokenize( "=", curPos );
	if( resToken == "" ) // Notes : 키값만 존재한다.
	{
		strTemp.Trim( _T(" \t") );
		key = strTemp;

		return 0;
	}

	int nTokenLen = resToken.GetLength();
	CString strData( strTemp.Right( strTemp.GetLength() - nTokenLen - 1) );
	strData.Trim( _T(" \t") );

	resToken.Trim( _T(" \t") );
	key = resToken;

	int nRet = split( STRING(strData), pVec );

	return nRet;
}

int CCommentToolDlg::split( const STRING & line, VECSTRING * pVec )
{
	STRING fld;
	UINT i(0),j(0);
	UINT nfield(0);

	if( (UINT)line.length() == 0 )
		return 0;

	do
	{
		if( i < (UINT)line.length() && line[i] == _T('"') )
			j = advquoted( line, fld, ++i); // Notes : 인용부호 처리 생략
		else
			j = advplain( line, fld, i );

		if( j > i )
		{
			if( nfield >= (UINT)pVec->size() )
				pVec->push_back(fld);
			else
				(*pVec)[nfield] = fld;

			nfield++;
		}

		i = j + 1;
	} while( j < (UINT)line.length() );

	return nfield;
}

// Notes : 인용부호로 둘러 쌓인 필드
int CCommentToolDlg::advquoted(const STRING & s, STRING & fld, UINT i)
{
	UINT j(0);
	fld = _T("");

	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	for( j = i; j < (unsigned int)s.length(); j++ )
	{
		if( s[j] == _T('"') && s[++j] != _T('"') )
		{
			indexCh = findfirstof( s, m_fieldsep, j );

			if( indexCh == npos ) // Notes : 구분자가 없는 경우
				indexCh = s.length();

			for( indexCh -= j; indexCh-- > 0; )
			{
				fld += s[j++];
			}

			break;
		}

		fld += s[j];
	}

	return j;
}

// Notes : 인용부호로 둘러싸이지 않은 필드
int CCommentToolDlg::advplain(const STRING & s, STRING & fld, UINT i ) 
{
	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	indexCh = findfirstof( s, m_fieldsep, i );

	if( indexCh == npos ) // Notes : 구분자가 없는 경우
		indexCh = s.length();

	fld = STRING( s, i, indexCh-i);

	return (int)indexCh;
}

int CCommentToolDlg::findfirstof( const STRING & line, const STRING & sep, UINT i )
{
	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	for( int index=i; index<(int)line.length(); )
	{
		if( CHARSET::IsLeadChar( line[index] ) )
		{
			index += 2;
			continue;
		}

		indexCh = sep.find( line[index] );
		if( indexCh != npos )
			return index;

		++index;
	}

	return npos;
}



