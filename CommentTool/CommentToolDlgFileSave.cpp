#include "stdafx.h"
#include "../SigmaCore/File/StringFile.h"

#include "../RanLogic/GLCommentFile.h"

#include "CommentTool.h"
#include "CommentToolDlg.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../SigmaCore/Encrypt/Rijndael.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CCommentToolDlg::OnFileSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	CString szFilter = "Comment Setting File (*.ini)|*.ini|";
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return;

	CString strTemp;
	bool bOk = IniFileSave( strTemp );
	if( !bOk )
	{
		strTemp += _T(" ");
	}

    sc::CRijndael oRijndael;
    oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

	int nLen = oRijndael.GetEncryptStringLength( strTemp );
	char * szBuffer = new char[nLen];

	oRijndael.EncryptEx( strTemp.GetString(), szBuffer, strTemp.GetLength() );
    int nVersion = sc::CRijndael::VERSION;

	CFile file;
	file.Open( dlg.GetPathName().GetString(), CFile::modeCreate|CFile::modeWrite );	// 파일 열기
	file.Write( &nVersion, sizeof(int) );							// 버젼 쓰기
	file.Write( szBuffer, nLen );		// 파일 쓰기
	file.Close();

	delete [] szBuffer;
}

bool CCommentToolDlg::IniFileSave( CString& strFile )
{
	if( m_vecCommentData.size() == 0 )
		return false;

	for( INT i = 0; i < ( INT ) m_vecCommentData.size(); i++ )
	{
		strFile += _T( "[" );
		strFile += m_vecCommentData[ i ].strMainKey.c_str();
		strFile += _T( "]" );
		strFile += _T( "\n" );

		for( INT j = 0; j < ( INT ) m_vecCommentData[ i ].vecKeyData.size(); j++ )
		{
			strFile += m_vecCommentData[ i ].vecKeyData[ j ].strSubKey.c_str();
			strFile += _T( " " );
			strFile += _T( "=" );
			strFile += _T( " " );

			for( INT k = 0; k < ( INT ) m_vecCommentData[ i ].vecKeyData[ j ].strData.size(); k++ )
			{
				strFile += m_vecCommentData[ i ].vecKeyData[ j ].strData[ k ].c_str();
				
				if( k < ( INT ) m_vecCommentData[ i ].vecKeyData[ j ].strData.size() -1 )
				{
					strFile += _T( "," );
				}                
				else if( k == ( INT ) m_vecCommentData[ i ].vecKeyData[ j ].strData.size() -1 )
				{
					strFile += _T( "\n" );
				}
			}

			if( i < ( INT ) m_vecCommentData.size() -1 &&
				j == ( INT ) m_vecCommentData[ i ].vecKeyData.size() -1 )
			{
				strFile += _T( ";" );
				strFile += _T( "\n" );
			}
		}
	}

	return true;
}

void CCommentToolDlg::OnFileIniToLua()
{
	CGLCommentFile cComment;

	{
		CString szFilter = "Comment Setting File (*.ini)|*.ini|";
		CFileDialog dlg( TRUE, ".ini", NULL, OFN_HIDEREADONLY, szFilter, this );

		dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath();
		if( dlg.DoModal() != IDOK )
			return;

		if( !cComment.LOADFILE_INI( dlg.GetPathName().GetString(), true ) )
			return;
	}

	{
		CString szFilter = "Comment Setting File (*.lua)|*.lua|";
		CFileDialog dlg( FALSE, ".lua", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this );

		dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath();
		if( dlg.DoModal() != IDOK )
			return;

		if( !cComment.SAVEFILE_LUA( dlg.GetPathName().GetString() ) )
			return;
	}

	MessageBox( _T( "Success" ), _T( "Result" ), MB_OK );
}