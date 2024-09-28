#include "pch.h"
#include "../../SigmaCore/Xml/2.5.2/tinyxml.h"
//#include "../../SigmaCore/Xml/2.5.3/tinyxml.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../RanLogic/RANPARAM.h"
#include "GameTextLoader.h"

#include <algorithm>
#include "../Common/StlFunctions.h"

#include "RanXMLParser.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NS_TEXT_LOADER
{
	BOOL bServer = FALSE;
	int	g_nTestMsgBox = 0;	// Test Code

//////////////////////////////////////////////////////////////////////////////////////
//	private
	BOOL Analyze_Sentense ( const CHAR * szFileName, sc::CStringFile& StrFile, CString& strLine,
							CStringArray& StrArray, STRFLAG& mapFlags )
	{
		GASSERT( szFileName );

		//	���Ἲ �˻�
		if ( StrArray.GetCount () < 2 )
		{
			CString str;
			str.Format ( "Syntax Error : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		//	ī��Ʈ
		CString strLineCount = StrArray.GetAt ( 2 );
		const int LineCount = atoi ( strLineCount.GetString() );
		const CString strCRLF = "\r\n";
		CString strBuffer;
        int i = 0;
		for ( ; i < LineCount && StrFile.GetNextLine(strLine); i++ )
		{
			strBuffer += strLine;

			//	���������� �ƴѰ��,
			//	�ڵ����� ���๮�ڸ� �ٿ��ش�.
			if ( (LineCount - 1) != i ) strBuffer += strCRLF;
		}

		if ( i != LineCount )
		{
			CString str;
			str.Format ( "It's short of data for lines you made : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		//	����Ÿ �����
		CStringArray* pNewStrArray = new CStringArray;
		pNewStrArray->Add ( strBuffer );

		//	�˸��� id��ġ�� ������ ���� �ֱ�
		CString StrMark = StrArray.GetAt ( 1 );
		mapFlags[std::string(StrMark.GetString())] = pNewStrArray;	

		return TRUE;
	}

	BOOL Analyze_Numeric ( const CHAR * szFileName, sc::CStringFile& StrFile, CString& strLine,
							CStringArray& StrArray, STRFLAG& mapFlags )
	{
		GASSERT( szFileName );

		//	���Ἲ �˻�
		if ( StrArray.GetCount () < 2 )
		{
			CString str;
			str.Format ( "Syntax Error : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		StrFile.GetNextLine(strLine);

		//	����Ÿ �����
		CStringArray* pNewStrArray = new CStringArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( "," );
		STRUTIL::StringSeparate ( strLine, *pNewStrArray );

		//	ī��Ʈ
		CString strColumnCount = StrArray.GetAt ( 2 );
		const int ColumnCount = atoi ( strColumnCount.GetString() );	

		if ( pNewStrArray->GetCount() != ColumnCount )
		{
			CString str;
			str.Format ( "It's short of data for lines you made : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			SAFE_DELETE ( pNewStrArray );
			return FALSE;
		}

		//	�˸��� id��ġ�� ������ ���� �ֱ�
		CString StrMark = StrArray.GetAt ( 1 );
		mapFlags.insert(STRFLAG_VALUE(std::string(StrMark.GetString()), pNewStrArray));

		return TRUE;
	}
								
	BOOL Analyze_Word ( const CHAR * szFileName, sc::CStringFile& StrFile, CString& strLine,
						CStringArray& StrArray, STRFLAG& mapFlags )
	{
		GASSERT( szFileName );

		//	���Ἲ �˻�
		if ( StrArray.GetCount () < 2 )
		{
			CString str;
			str.Format ( "Syntax Error : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		StrFile.GetNextLine(strLine);

		//	����Ÿ �����
		CStringArray* pNewStrArray = new CStringArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( "," );
		STRUTIL::StringSeparate ( strLine, *pNewStrArray );

		//	ī��Ʈ
		CString strColumnCount = StrArray.GetAt ( 2 );
		const int ColumnCount = atoi ( strColumnCount.GetString() );	

		if ( pNewStrArray->GetCount() != ColumnCount )
		{
			CString str;
			str.Format ( "It's short of data for lines you made : [%s]", strLine );
			MessageBox ( NULL, str, szFileName, MB_OK );
			SAFE_DELETE ( pNewStrArray );
			return FALSE;
		}

		//	�˸��� id��ġ�� ������ ���� �ֱ�
		CString StrMark = StrArray.GetAt ( 1 );
		mapFlags.insert(STRFLAG_VALUE(std::string(StrMark.GetString()), pNewStrArray));

		return TRUE;
	}

	BOOL IsValid_Syntax ( const CHAR * szFileName, CStringArray& StrArray, STRFLAG& mapFlags )
	{
		GASSERT( szFileName );

		if ( StrArray.GetSize() < 2 )
		{
			CString str;
			str.Format ( "Syntax Error : [%s]", StrArray.GetAt(0) );
			MessageBox ( NULL, str, szFileName, MB_OK );
			return FALSE;
		}

		CString StrMark = StrArray.GetAt ( 1 );

		STRFLAG_IT found;			
		found = mapFlags.find ( std::string(StrMark.GetString()) );

		if ( found == mapFlags.end() )	return TRUE;		
		else
		{
			CString str;
			str.Format ( "Duplicated Keyword : [%s]", StrArray.GetAt(1) );
			MessageBox ( NULL, str, StrArray.GetAt(0), MB_OK );
			return FALSE;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////
//	public
	BOOL LoadTextInZip( const char* szZipFileFullPath, const char* szPath, const char* szFile, const BOOL bPack, void* mapFlags, BOOL bServerExe, const CString& country )
	{
		GASSERT( szZipFileFullPath );
		GASSERT( szPath );
		GASSERT( szFile );
		bServer = bServerExe;

		if ( g_nTestMsgBox == 0 )
		{
			g_nTestMsgBox = 1;
		}

		CRanXMLParser xmlParser( country );
		if (!xmlParser.Parse_XMLInZip( mapFlags, szZipFileFullPath, szPath, szFile, bPack))
		{
			CString strErrorText;
			strErrorText.Format("[GAME TEXT LOADER IN ZIP]Xml Load Failed %s", szFile);

			if ( bServer )
			{
				sc::writeInfo(strErrorText.GetString());
			}
			else
			{
				AfxMessageBox(strErrorText, MB_OK|MB_ICONSTOP);
				g_nTestMsgBox = 2;
			}

			return FALSE;
		}

		return TRUE;
	}

	BOOL LoadText ( const char* szFileName, WORD progVersion, void* mapInFlags, const CString& country, DWORD dwLoadOP )
	{
		GASSERT( szFileName );

		bServer = dwLoadOP&NS_TEXT_LOADOP_SERVEREXE;

		if ( g_nTestMsgBox == 0 )
		{
			g_nTestMsgBox = 1;
		}

		if( dwLoadOP&NS_TEXT_LOADOP_REFACT )
		{
			CRanXMLParser xmlParser( country );
			DWORD dwParseOP = dwLoadOP&NS_TEXT_LOADOP_CONVUTF8?CRanXMLParser::RANXMLPARSER_PARSEOP_CONVUTF8:0;
			dwParseOP |= dwLoadOP&NS_TEXT_LOADOP_ALLLANG?CRanXMLParser::RANXMLPARSER_PARSEOP_ALLLANG:0;
			if( !xmlParser.Parse( mapInFlags, szFileName, dwParseOP ))
			{
				CString strErrorText;
				strErrorText.Format("[GAME TEXT LOADER]Xml Load Failed %s", szFileName);

				if ( bServer )
				{
					sc::writeInfo(strErrorText.GetString());
				}
				else
				{
					AfxMessageBox(strErrorText, MB_OK|MB_ICONSTOP);
					g_nTestMsgBox = 2;
				}

				return FALSE;
			}
		}
		else
		{
			STRFLAG* mapFlags = static_cast<STRFLAG*>( mapInFlags );
			sc::CStringFile StrFile;
			if ( !StrFile.Open ( szFileName ) )
                return FALSE;		

			STRUTIL::ClearSeparator ();
			STRUTIL::RegisterSeparator ( "\t" );
			STRUTIL::RegisterSeparator ( "," );

			WORD fileVersion = 0x0000;
			CString strLine;
			while ( StrFile.GetNextLine(strLine) )
			{
				CStringArray *pStrArray = new CStringArray;

				STRUTIL::ClearSeparator ();
				STRUTIL::RegisterSeparator ( "\t" );
				STRUTIL::RegisterSeparator ( "," );
				STRUTIL::StringSeparate ( strLine, *pStrArray );

				//	���� ���� üũ
				int nArraySize = 0;
				nArraySize = (int)pStrArray->GetSize ();
				if ( !nArraySize )
				{
					SAFE_DELETE(pStrArray);
					continue;
				}

				//	�ּ� ó�� üũ
				//	���� �����Ѵٸ�, '//'���Ŀ� �����ϴ� �κ� ��� ����
				for ( int i = 0; i < nArraySize; i++ )
				{				
					CString strCheckSlash = pStrArray->GetAt(i);
					if ( strCheckSlash.GetLength() >= 2 && strCheckSlash.GetAt(0)=='/' && strCheckSlash.GetAt(1)=='/' )
					{
						for ( int j = nArraySize - 1; i <= j; j-- )
						{
							pStrArray->RemoveAt (i);
						}
						break;
					}
				}

				//	�ٽ� ���� üũ ( �ּ�ó���Ǿ� �������ٸ�, �޸� ������ ���� )
				nArraySize = (int)pStrArray->GetSize ();
				if ( !nArraySize )
				{
					SAFE_DELETE(pStrArray);
					continue;
				}


				CString StrMark = pStrArray->GetAt(0);
				if ( StrMark == "VERSION" )
				{
					CString strVersion = pStrArray->GetAt ( 1 );
					fileVersion = (WORD) atoi ( strVersion.GetString() );
					SAFE_DELETE(pStrArray);
				}
				else
				{
					if ( IsValid_Syntax ( szFileName, *pStrArray, mapFlags[RENDERPARAM::emLangSet] ) )
					{
						//	���� ó��, ������ �ܾ� ó��, �������� �� ó��
						if( StrMark=="NUMERIC" )		Analyze_Numeric( szFileName, StrFile, strLine, *pStrArray, mapFlags[RENDERPARAM::emLangSet] );
						else if( StrMark=="WORD" )		Analyze_Word( szFileName, StrFile, strLine, *pStrArray, mapFlags[RENDERPARAM::emLangSet] );
						else if( StrMark=="SENTENSE" )	Analyze_Sentense( szFileName, StrFile, strLine, *pStrArray, mapFlags[RENDERPARAM::emLangSet] );
					}
					SAFE_DELETE(pStrArray);
				}	
			}

			if ( progVersion < fileVersion )
			{
				//	Note : ��� CStringArray* �� �����ش�.
				ClearText ( mapFlags );			
				return FALSE;
			}
		}

		return TRUE;
	}

	BOOL ClearText ( STRFLAG* mapFlags )
	{
		for ( int i = 0; i < language::LANG_NUM; ++i )
		{
			std::for_each ( mapFlags[i].begin(), mapFlags[i].end(), std_afunc::DeleteMapObject() );
			mapFlags[i].clear();
		}
		return TRUE;
	}

	const CString & GetText(const std::string& strKey, int Index, STRFLAG& mapFlags )
	{
		if ( g_nTestMsgBox == 0 )
		{
			g_nTestMsgBox = 1;
		}

		STRFLAG_IT found;
		found = mapFlags.find ( strKey );		
		if ( found == mapFlags.end() )
		{
			if( bServer )
			{
                std::string ErrorMsg(
                    sc::string::format(
                        "[GAME TEXT ERROR]Not exist text keyword : %1%", strKey));
				sc::writeLogError(ErrorMsg);
			}
			else
			{
				CString strTemp;
				strTemp.Format( "[GAME TEXT ERROR]Not Exist Text Keyword : %s", strKey.c_str() );
				AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
				g_nTestMsgBox = 2;
				//sc::writeLogError(strTemp.GetString());
			}

			static CString strEmpty("skip-empty"); // MEMO : ������ ��ȯ�Ǵ� �� ��Ʈ��
			return strEmpty;
		}	

		CStringArray *pStrArray = (*found).second;

		if ( !pStrArray || pStrArray->GetCount () <= Index )
		{
			if( bServer )
			{
                std::string ErrorMsg(
                    sc::string::format(
                        "[GAME TEXT ERROR] Overflow index range : %1% %2%", strKey, Index));
				sc::writeLogError(ErrorMsg);
			}
			else
			{
				CString strTemp;
				strTemp.Format( "[GAME TEXT ERROR]Overflow Index Range : %s %d", strKey.c_str(), Index );
				AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
				g_nTestMsgBox = 2;
 				//sc::writeLogError(strTemp.GetString());
			}

			static CString strEmpty("skip-empty"); // MEMO : ������ ��ȯ�Ǵ� �� ��Ʈ��
			return strEmpty;
		}
		const CString& str = pStrArray->GetAt ( Index );

//		return pStrArray->GetAt ( Index );
		return str;
	}

	int GetNumber(const std::string& strKey, int Index, STRFLAG& mapFlags )
	{
		if ( g_nTestMsgBox == 0 )
		{
			g_nTestMsgBox = 1;
		}

		STRFLAG_IT found;
		found = mapFlags.find ( strKey );		
		if ( found == mapFlags.end() )
		{
			if( bServer )
			{
                std::string ErrorMsg(
                    sc::string::format(
                        "[GAME TEXT ERROR] Not exist text keyword : %1%", strKey));
				sc::writeLogError(ErrorMsg);			}
			else
			{	
				CString strTemp;
				strTemp.Format( "[GAME TEXT ERROR]Not Exist Text Keyword : %s", strKey.c_str() );
				AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
				g_nTestMsgBox = 2;
			}

			return 0; // MEMO : 0�� ��ȯ�Ǵ°��� ����ǥ�ð� �ɼ� �ִ��� ��.
		}	

		CStringArray *pStrArray = (*found).second;

		if ( !pStrArray || pStrArray->GetCount () <= Index )
		{
			if( bServer )
			{
                std::string ErrorMsg(
                    sc::string::format(
                        "[GAME TEXT ERROR] Overflow index range : %1%", strKey));
				sc::writeLogError(ErrorMsg);
			}
			else
			{
				CString strTemp;
				strTemp.Format( "[GAME TEXT ERROR]Overflow Index Range : %s", strKey.c_str() );
				AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
				g_nTestMsgBox = 2;
			}

			return 0; // MEMO : 0�� ��ȯ�Ǵ°��� ����ǥ�ð� �ɼ� �ִ��� ��.
		}

		return atoi( pStrArray->GetAt( Index ).GetString() );
	}

	bool CheckText(const std::string& strKey, int Index, STRFLAG& mapFlags )
	{
		STRFLAG_IT found;
		found = mapFlags.find ( strKey );		
		if ( found == mapFlags.end() )
		{
			return false;
		}	

		CStringArray *pStrArray = (*found).second;
		if ( !pStrArray )
			return false;

		if ( pStrArray->GetCount () <= Index )
		{
			return false;
		}

		return true;
	}

	const BOOL SaveStringTableXML( const char* const szFileName, STRFLAG* mapFlags, const BOOL bAllLang )
	{
		if ( szFileName == NULL || strlen(szFileName) == 0 )
			return FALSE;

		// �����ÿ� �ʿ��� �ӽ� ����;
		TCHAR		szValue[MAX_TEXTLEN]={0};
		WCHAR		szTemp[MAX_TEXTLEN]={0};

		// ���� �� ���� �ʿ䰪;
		const INT iLangSet = static_cast<int>( RANPARAM::emProvideLangFlag );
		const INT iChildCnt = bAllLang == TRUE ? language::LANGFLAG_TOTAL_NUM : iLangSet + 1;
		const INT iStartCnt = bAllLang == TRUE ? 0 : iLangSet;

		// ����;
		TIXMLOLD::TiXmlDocument doc(szFileName);
		bool bLoadRes = doc.LoadFile( TIXMLOLD::TIXML_ENCODING_UTF8 );
		doc.Clear();
		TIXMLOLD::TiXmlDeclaration * decl = new TIXMLOLD::TiXmlDeclaration( "1.0", "utf-8", "" );
		doc.LinkEndChild(decl);
		TIXMLOLD::TiXmlElement* pRootElem = new TIXMLOLD::TiXmlElement("TEXT");
		doc.LinkEndChild(pRootElem);

		STRFLAG_IT IterTail = mapFlags[iLangSet].end(), findItr;
		for ( STRFLAG_IT itr = mapFlags[iLangSet].begin(); itr != IterTail; ++itr )
		{
			TIXMLOLD::TiXmlElement* pElem = new TIXMLOLD::TiXmlElement( "SENTENSE" );
			pElem->SetAttribute( "Ver", 1 );
			pElem->SetAttribute( "Id",  itr->first.c_str() );
			
			for ( int i = iStartCnt; i < iChildCnt; ++i )
			{
				if ( mapFlags[i].empty() )
					continue;
			
				if ( ( findItr = mapFlags[i].find(itr->first.c_str()) ) != mapFlags[i].end()
					&& findItr->second != NULL )
				{
					CStringArray* pStrArray = mapFlags[i][itr->first.c_str()];
					// CString str = pStrArray->GetAt( 0 );
					// szValue = const_cast<char*>(pStrArray->GetAt(0).GetString());
					int nStringCount;
					if ( (nStringCount = pStrArray->GetSize()) > 1 )
					{
						pElem->SetValue("WORD");
						for ( int j = 0; j < nStringCount; ++j )
						{
							TIXMLOLD::TiXmlElement* pChildElem = new TIXMLOLD::TiXmlElement( "VALUE" );
							pChildElem->SetAttribute( "Lang", language::strProvideLanguage[i] );
							pChildElem->SetAttribute("Index", j);

							_tcscpy( szValue, pStrArray->GetAt(0).GetString() );

							MultiByteToWideChar(CD3DFontPar::nCodePage[ (int)language::ProvidelangToLangflag((language::EMPROVIDE_LANGFLAG)i) ], 0, szValue, strlen(szValue), szTemp, sizeof(szTemp)/sizeof(szTemp[0]));			
							WideCharToMultiByte(CP_UTF8, 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );

							TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText("Skip");
							pXmlText->SetValue( szValue );
							pChildElem->LinkEndChild(pXmlText);
							pElem->LinkEndChild(pChildElem);
						}
					}
					else
					{		
						pElem->SetValue("SENTENSE");
						TIXMLOLD::TiXmlElement* pChildElem = new TIXMLOLD::TiXmlElement( "VALUE" );
						pChildElem->SetAttribute( "Lang", language::strProvideLanguage[i] );

						_tcscpy( szValue, pStrArray->GetAt(0).GetString() );

						MultiByteToWideChar(CD3DFontPar::nCodePage[ (int)language::ProvidelangToLangflag((language::EMPROVIDE_LANGFLAG)i) ], 0, szValue, strlen(szValue)+1, szTemp, sizeof(szTemp)/sizeof(szTemp[0]));			
						WideCharToMultiByte(CP_UTF8, 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );

						TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText("Skip");
						pXmlText->SetValue( szValue );
						pChildElem->LinkEndChild(pXmlText);
						pElem->LinkEndChild(pChildElem);
					}
				}				
			}

			pRootElem->LinkEndChild( pElem );
		}
		doc.SaveFile(szFileName);
		doc.Clear();
		return TRUE;
	}
}
