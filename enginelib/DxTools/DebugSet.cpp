#include "pch.h"
#include "shlobj.h"
#include <list>
#include "../../SigmaCore/Encrypt/CompByte.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Container/MList.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../DxTools/DxInputDevice.h"
#include "../Common/SUBPATH.h"
#include "DxFontMan.h"
#include "DebugSet.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace CDebugSet
{
	#define MAX_INFO		(40)
	#define MAX_CHANNEL		(8)
	#define MAX_STRLENGTH	(256)
	#define	LIST_CHANNEL	(8)
	#define	MAX_LISTPAGE	(20)	//	������ ��, 1�������� MAX_INFO ������ ����ϸ�,
									//	1364�������� �뷫... 10M���� �Դ´�.

	enum	EDEBUGSTATE
	{
		EMPTY,
        USING,
        FIXLINE
	};

	struct INFO
	{
		char			szInfo[MAX_INFO][MAX_STRLENGTH];
		EDEBUGSTATE		State[MAX_INFO];	
		float			LeftTime[MAX_INFO];
		float			YPos[MAX_INFO];
		int				nCur;

		INFO () : nCur(-1)
		{
		}

		void Init ()
		{
			memset ( szInfo, 0, sizeof(char)*MAX_INFO*MAX_STRLENGTH );	
			memset ( LeftTime, 0, sizeof(float)*MAX_INFO );

			float ypos = 55.0f;
			for ( int i = 0; i < MAX_INFO; i++, ypos += 15.f )
			{
				YPos[i] = ypos;		
				State[i] = EMPTY;
			}
		}
	};

	DWORD m_nChannel = 0;
	INFO m_Info[MAX_CHANNEL];	
	char m_szFullDir[MAX_PATH] = "";
	CD3DFontPar* m_pFont = NULL;

	sc::CMList<INFO*> m_ListInfo;
	int m_CurNode = 1;
	bool m_bLogFileFinalOpen = false;

	bool	m_bLogWrite(false);
	CTime	m_LogTime;
	
	CString m_strLogName;
	CString m_strLogFile;
	CString m_strTutorialLogFile;
	CString m_strInstLogFile;
//	CString m_strTracingLogFile;
	CString m_strHackingLogFile;
	CString m_strMoneyLogFile;
	CString m_strClubNoticeLogFile;
	CString m_strBlockLogFile;
	CString m_strClubIncomeLogFile;
	CString m_strCDMLogFile;
	CString m_strGameStateLogFile;
	CString m_strProFileLogFile;
	CString m_strProfileExLogFile;
	CString m_strProFileMsgFile;

	FILE* m_pLogFile;
	FILE* m_pTutorialLogFile;
	FILE* m_pInstLogFile;
	FILE* m_pHackingLogFile;
	FILE* m_pMoneyLogFile;
	FILE* m_pClubNoticeLogFile;
	FILE* m_pBlockLogFile;
	FILE* m_pClubIncomeLogFile;
	FILE* m_pCDMLogFile;
	FILE* m_pGameStateLogFile;
	FILE* m_pProFileLogFile;
	FILE* m_pProfileExLogFile;
	FILE* m_pProFileMsgFile;


	CString m_strSTARTTIME;
	BOOL	m_bSubjectName_ERROR = FALSE;

	struct SPOSTEXT
	{
		float		fX;
		float		fY;
		std::string	strText;

		SPOSTEXT ( float fx, float fy, std::string str ) :
			fX(fx),
			fY(fy),
			strText(str)
		{
		}
	};

	typedef std::list<SPOSTEXT>				LISTPOSTEXT;
	typedef std::list<SPOSTEXT>::iterator	LISTPOSTEXT_ITER;

	LISTPOSTEXT m_listPosText;

	//	Note	:	ȭ�鿡 �Ѹ��� �ð� ( �ʴ��� )
	//
	float DELAY_TIME = 30.0f;

	void FrameMove ( float fTime, float fETime )
	{
		//	Ű���� �Է� ä�� ����
		DxInputDevice &IDev = DxInputDevice::GetInstance();
		if ( IDev.GetKeyState(DIK_LCONTROL)&DXKEY_PRESSED )
		{
			for ( int nKey=DIK_1; nKey<=DIK_0; nKey++ )
			{
				if (IDev.GetKeyState(nKey)&DXKEY_UP )	m_nChannel = nKey - DIK_1;
			}
		}
		
		if ( m_nChannel == LIST_CHANNEL )	//	����Ʈ ������ ��Ʈ��
		{
			if ( IDev.GetKeyState(DIK_LCONTROL)&DXKEY_PRESSED )
			{
				if (IDev.GetKeyState(DIK_PGUP)&DXKEY_UP )
				{
					if ( 1 < m_CurNode )
					{
						if ( !m_ListInfo.IsEnd() )
						{
							m_ListInfo.GoPrev ();
							m_CurNode--;
						}
					}					
				}
				else if (IDev.GetKeyState(DIK_PGDN)&DXKEY_UP )
				{
					if ( m_CurNode < int(m_ListInfo.GetAmount()) )
					{
						if ( !m_ListInfo.IsEnd() )
						{
							m_ListInfo.GoNext ();
							m_CurNode++;
						}
					}					
				}
				else if (IDev.GetKeyState(DIK_HOME)&DXKEY_UP )
				{
					m_ListInfo.SetHead ();
					m_CurNode = 1;					
				}
				else if (IDev.GetKeyState(DIK_END)&DXKEY_UP )
				{
					m_ListInfo.SetTail ();
					m_CurNode = m_ListInfo.GetAmount ();					
				}
			}
		}
		else								//	���� �ð�
		{
			for ( int i=0; i<MAX_INFO; ++i )
			{
				for ( int j=0; j<MAX_CHANNEL; ++j )
				{
					if ( m_Info[j].LeftTime[i] > 0.0f )		m_Info[j].LeftTime[i] -= fETime;
					else									m_Info[j].State[i] = EMPTY;
				}
			}
		}
	}

	void Render ()
	{	
		if ( !m_pFont )	return;

		//	Ư����ġ�� �� ������ ���� �ѷ����� �ؽ�Ʈ ����.
		{
			LISTPOSTEXT_ITER iter = m_listPosText.begin();
			LISTPOSTEXT_ITER iter_end = m_listPosText.end();

			for ( ; iter!=iter_end; ++iter )
				m_pFont->DrawText ( (*iter).fX, (*iter).fY, D3DCOLOR_ARGB(255,255,255,255), const_cast<char*>((*iter).strText.c_str()) );

			m_listPosText.clear();
		}

		D3DCOLOR fontColor;
		float	XPos = 2.f;

		if ( m_nChannel == LIST_CHANNEL )	//	����Ʈ ������ ����
		{
			char szChannel[128] = "";
			StringCchPrintf( szChannel, 128, "List Reporting...  [%04d/%04d]", m_CurNode, m_ListInfo.GetAmount () );
			m_pFont->DrawText ( XPos, 40.0f, D3DCOLOR_ARGB(255,255,255,255), szChannel );

			INFO* pInfo = NULL;			
			m_ListInfo.GetCurrent ( pInfo );
			if ( pInfo )
			{
				fontColor =	D3DCOLOR_ARGB(255,255,255,255);

				for( int i = 0; i < MAX_INFO; i++ )
					m_pFont->DrawText ( XPos, pInfo->YPos[i], fontColor, pInfo->szInfo[i] );
			}
		}
		else								//	�Ϲ� ä�� ����
		{
			char szChannel[128] = "";
			StringCchPrintf( szChannel, 128, "Channel :\t%d", m_nChannel );
			m_pFont->DrawText ( XPos, 40.0f, D3DCOLOR_ARGB(255,255,255,255), szChannel );
			
			for( int i = 0; i < MAX_INFO; i++ )
			{
				INFO &info = m_Info[m_nChannel];

				//	Note	:	����� ������ �ִٸ�... EMPTY�� �ƴҶ�...
				//				EMPTY�� 0��
				if ( info.State[i] )
				{
					//	Note	:	������ ������ ��
					if ( info.State[i] == FIXLINE )
					{
						fontColor = D3DCOLOR_ARGB(255,165,235,255);
					}
					
					//	Note	:	���� �ʰ� ���� ����� ����
					//				
					else if ( info.nCur == i )
					{
						DWORD	Alpha = DWORD(255.0f * ( info.LeftTime[i] / DELAY_TIME ));
						fontColor = D3DCOLOR_ARGB(Alpha,255,199,172);
					}
					
					//	Note	:	�Ϲ� ����� ����
					//
					else
					{
						DWORD	Alpha = DWORD(255.0f * ( info.LeftTime[i] / DELAY_TIME ));
						fontColor =	D3DCOLOR_ARGB(Alpha,255,255,255);
					}

					m_pFont->DrawText ( XPos, info.YPos[i], fontColor, info.szInfo[i] );
				}
			}
		}
	}

#ifdef _ON_DEBUGSET
	void ToView ( const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf ( szBuffer, MAX_STRLENGTH, szFormat, argList );
		va_end(argList);

		//	Note	:	���� �������� ����
		m_Info[0].nCur = ( m_Info[0].nCur + 1 ) % MAX_INFO;

		//	Note	:	FullCount�� ����� �迭�� ��� �� á����,
		//				���� �������� ���� ���� �����.
		for ( int	FullCount = 0; m_Info[0].State[m_Info[0].nCur] != EMPTY; FullCount++ )
		{	
			//	Note	:	�� ������ �� ���� ��ã�� ����
			//				����� ������ ��á�ٴ� �̾߱�
			if ( FullCount == MAX_INFO )
			{
				//	Note	:	�ٽ� ���ڸ��� ���ƿ������� �ұ��ϰ�,
				//				�� �ڸ��� �������� ����� �����̸�
				//				���� �������� ����
				for ( ; m_Info[0].State[m_Info[0].nCur] == FIXLINE; )
				{
					m_Info[0].nCur = ( m_Info[0].nCur + 1 ) % MAX_INFO;                                
				}
				break;
			}
			m_Info[0].nCur = ( m_Info[0].nCur + 1 ) % MAX_INFO;
		}	
		
		//	Note	:	���� ���̱�
		char	szLine[12] = "";
		StringCchPrintf( szLine, 12, "[%02d]  ", m_Info[0].nCur );

		StringCchCopy( m_Info[0].szInfo[m_Info[0].nCur], MAX_STRLENGTH, szLine );
		StringCchCat( m_Info[0].szInfo[m_Info[0].nCur], MAX_STRLENGTH, szBuffer );

		m_Info[0].LeftTime[m_Info[0].nCur] = DELAY_TIME;
		m_Info[0].State[m_Info[0].nCur] = USING;
	}


	void ToView ( int line, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		line %= MAX_INFO;

		char	szLine[12] = "";
		StringCchPrintf( szLine, 12, "[%02d]  ", line );

		StringCchCopy ( m_Info[0].szInfo[line], MAX_STRLENGTH, szLine );
		StringCchCat( m_Info[0].szInfo[line], MAX_STRLENGTH, szBuffer );

		m_Info[0].LeftTime[line] = DELAY_TIME;
		m_Info[0].State[line] = FIXLINE;
	}

	void ToView ( int nChannel, int line, const char *szFormat, ... )
	{
		GASSERT ( nChannel<MAX_CHANNEL && "ToViewChannel() A value of channel does about below MAX_CHANNEL." );

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		line %= MAX_INFO;

		char	szLine[12] = "";
		StringCchPrintf( szLine, 12, "[%02d]  ", line );

		StringCchCopy( m_Info[nChannel].szInfo[line], MAX_STRLENGTH, szLine );
		StringCchCat( m_Info[nChannel].szInfo[line], MAX_STRLENGTH, szBuffer );

		m_Info[nChannel].LeftTime[line] = DELAY_TIME;
		m_Info[nChannel].State[line] = FIXLINE;		
	}

	void ToListView ( const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		INFO* pInfo = NULL;
		m_ListInfo.GetTail ( pInfo );

		if ( pInfo )
		{
			//	Note	:	���� �������� ����
			pInfo->nCur = pInfo->nCur + 1;			
			
			if ( pInfo->nCur == MAX_INFO )	//	��尡 �� �����
			{
				BOOL	bTail = FALSE;
				if ( m_CurNode == m_ListInfo.GetAmount () )
				{
					bTail = TRUE;
				}

				//	�������� �ִ� MAX_LISTPAGE��ŭ �������� �Ѵ�.
				//	���� ���, ���� �ͺ��� ������
				if ( MAX_LISTPAGE == m_ListInfo.GetAmount () )
				{
					//	���� ������ �̵�
					m_ListInfo.GoNext ();

					//	�Ӹ� ����
					INFO* pHeadInfo = NULL;
					m_ListInfo.GetHead ( pHeadInfo );
					SAFE_DELETE ( pHeadInfo );					
					m_ListInfo.DelHead ();

					if ( !m_ListInfo.GetAmount () )
					{
						//	���� : m_CurNode�� 0�ΰ��� ����.
						//		   ���⼭�� ���������� ���, ��� ��尡 ������ ����
						//		   �ǹ��Ѵ�.
						m_CurNode = 0;
					}
				}

				INFO* pNewInfo = new INFO;
				pNewInfo->Init ();

				m_ListInfo.AddTail ( pNewInfo );
				m_ListInfo.GetTail ( pInfo );

				//	���簡 ������ ������ �ΰ�?
				if ( bTail )
				{
					m_ListInfo.SetTail ();
					m_CurNode = m_ListInfo.GetAmount ();
				}

				pInfo->nCur = 0;				
			}
			
			//	Note	:	���� ���̱�		
			char	szLine[16] = "";
			static	int nLine = 0;
			StringCchPrintf( szLine, 16, "[%05d]  ", nLine++ );

			StringCchCopy( pInfo->szInfo[pInfo->nCur], MAX_STRLENGTH, szLine );
			StringCchCat( pInfo->szInfo[pInfo->nCur], MAX_STRLENGTH, szBuffer );

			pInfo->LeftTime[pInfo->nCur] = DELAY_TIME;
			pInfo->State[pInfo->nCur] = FIXLINE;
		}
	}

	void ToPos ( float x, float y, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		m_listPosText.push_back ( SPOSTEXT(x,y,szBuffer) );
	}
#endif //_ON_DEBUGSET

	void SetLogFileFinalOpen( bool bUse )
	{
		m_bLogFileFinalOpen = bUse;
	}

	void SetSubjectName_ERROR( BOOL bUse )
	{
		m_bSubjectName_ERROR = bUse;
	}


	void ToFile ( const char *szFileName, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);


		//	Note	:	���� ��� ���̱�
		//
		char	szFullPathFileName[MAX_PATH] = "";

		StringCchCopy( szFullPathFileName, MAX_PATH, m_szFullDir );
		StringCchCat( szFullPathFileName, MAX_PATH, szFileName );

		FILE *fp;
		fp = _fsopen ( szFullPathFileName, "at", _SH_DENYNO );
		if ( fp )
		{
			fprintf ( fp, "%s\n", szBuffer );
			fclose(fp);
		}
	}

	void ToTracingFile ( const char *szAccountName, const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH+20] = "";
		char szTempBuf[MAX_STRLENGTH] = "";
		char szFileName[MAX_STRLENGTH] = "";

		char	date[128];
		char	time[128];

		va_start(argList, szFormat);
		StringCbVPrintf(szTempBuf, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		_strdate( date );
		_strtime( time );
		
		sprintf( szBuffer, "[%s], %s", GetCurTime().c_str(), szTempBuf );
		
		std::string AppPath = sc::getAppPath();
		AppPath.append("\\log\\");

		//	Note	:	���� ��� ���̱�
		//
		char	szFullPathFileName[MAX_PATH] = "";
		CTime Time = CTime::GetCurrentTime();
		sprintf( szFileName, "%s_%02d%02d%02d_Tracing_Log.csv", szAccountName, Time.GetYear(), Time.GetMonth(), Time.GetDay());

		//StringCchCopy( szFullPathFileName, MAX_PATH, m_szFullDir );
		StringCchCopy( szFullPathFileName, MAX_PATH, AppPath.c_str() );
		StringCchCat( szFullPathFileName, MAX_PATH, szFileName );

		FILE *fp;
		fp = _fsopen ( szFullPathFileName, "at", _SH_DENYNO );
		if ( fp )
		{
			fprintf ( fp, "%s\n", szBuffer );
			fclose(fp);
		}
	}

	std::string GetCurTime ()
	{
		CTime Time = CTime::GetCurrentTime();

		CString strTime;
		strTime.Format ( "%d-%02d-%02d %02d:%02d:%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay(),
			Time.GetHour(), Time.GetMinute(), Time.GetSecond() );

		return strTime.GetString();
	}

	void ToFileWithTime ( const char* szFileName, const char* szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);


		//	Note	:	���� ��� ���̱�
		//
		char	szFullPathFileName[MAX_PATH] = "";

		StringCchCopy( szFullPathFileName, MAX_PATH, m_szFullDir );
		StringCchCat( szFullPathFileName, MAX_PATH, szFileName );

		FILE *fp;
		fp = _fsopen ( szFullPathFileName, "at", _SH_DENYNO );
		if ( fp )
		{
			std::string strTIME = GetCurTime ();
			fprintf ( fp, "[%s]%s\n", strTIME.c_str(), szBuffer );
			fclose(fp);
		}
	}

	void ToHackingFile( const char *szFormat, ... )
	{
		GASSERT(!m_strHackingLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pHackingLogFile )
		{
			m_pHackingLogFile = fopen ( m_strHackingLogFile.GetString(), "at" );
			if ( m_pHackingLogFile )
			{
				fprintf( m_pHackingLogFile, "Time, Message, Account, ID, Money/StorageMoney, TempMoney/TempStorageMoney, Money Gap\n" );
			}
			else return;			
		}

		std::string strTIME = GetCurTime();
		fprintf ( m_pHackingLogFile, "%s, %s\n", strTIME.c_str(), szBuffer );
		fflush( m_pHackingLogFile );	
	}

	void ToTutorialMapFile( const char *szFormat, ... )
	{
		GASSERT(!m_strTutorialLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pTutorialLogFile )
		{
			m_pTutorialLogFile = _fsopen ( m_strTutorialLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pTutorialLogFile )	return;			
		}


		std::string strTIME = GetCurTime();
		fprintf ( m_pTutorialLogFile, "%s, %s\n", strTIME.c_str(), szBuffer );
		fflush( m_pTutorialLogFile );	
	}

	void ToInstantMapFile( const char *szFormat, ... )
	{
		GASSERT(!m_strInstLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pInstLogFile )
		{
			m_pInstLogFile = _fsopen ( m_strInstLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pInstLogFile )	return;			
		}

		
		std::string strTIME = GetCurTime();
		fprintf ( m_pInstLogFile, "%s, %s\n", strTIME.c_str(), szBuffer );
		fflush( m_pInstLogFile );	
	}

	void ToBillionUpdateFile( const char *szFormat, ... )
	{
		GASSERT(!m_strMoneyLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pMoneyLogFile )
		{
			m_pMoneyLogFile = _fsopen ( m_strMoneyLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pMoneyLogFile )	return;			
		}

		
		std::string strTIME = GetCurTime();
		fprintf ( m_pMoneyLogFile, "[%s] %s\n", strTIME.c_str(), szBuffer );
		fflush( m_pMoneyLogFile );			
	}

	void ToClubNoticeFile( const char *szFormat, ... )
	{
		GASSERT(!m_strClubNoticeLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pClubNoticeLogFile )
		{
			m_pClubNoticeLogFile = _fsopen ( m_strClubNoticeLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pClubNoticeLogFile )	return;			
		}

		
		std::string strTIME = GetCurTime();
		fprintf ( m_pClubNoticeLogFile, "[%s] %s\n", strTIME.c_str(), szBuffer );
		fflush( m_pClubNoticeLogFile );		

	}

	// �� �α�
	void ToBlockDetectFile( const char *szFormat, ... )
	{
		GASSERT(!m_strBlockLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pBlockLogFile )
		{
			m_pBlockLogFile = _fsopen ( m_strBlockLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pBlockLogFile )	return;			
		}

		
		std::string strTIME = GetCurTime();
		fprintf ( m_pBlockLogFile, "[%s] %s\n", strTIME.c_str(), szBuffer );
		fflush( m_pBlockLogFile );			
	}
	
	// Ŭ�� ���� �α�
	void ToClubIncomeFile( const char *szFormat, ... )
	{
		GASSERT(!m_strClubIncomeLogFile.IsEmpty()&&"log file were not set.");
		
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pClubIncomeLogFile )
		{
			m_pClubIncomeLogFile = _fsopen ( m_strClubIncomeLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pClubIncomeLogFile )	return;			
		}

		
		std::string strTIME = GetCurTime();
		fprintf ( m_pClubIncomeLogFile, "[%s] %s\n", strTIME.c_str(), szBuffer );
		fflush( m_pClubIncomeLogFile );		
	}
	
	// CDM �α�
	void ToCDMLogFile( const char *szFormat, ... )
	{
		GASSERT(!m_strCDMLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pCDMLogFile )
		{
			m_pCDMLogFile = _fsopen ( m_strCDMLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pCDMLogFile )	return;			
		}

		
		std::string strTIME = GetCurTime();
		fprintf ( m_pCDMLogFile, "[%s] %s\n", strTIME.c_str(), szBuffer );		
		fflush( m_pCDMLogFile );
		
	}

	// �������ϸ� �α�
	void ToProFileLogFile( const char *szFormat, ... )
	{
		GASSERT(!m_strProFileLogFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pProFileLogFile )
		{
			m_pProFileLogFile = _fsopen ( m_strProFileLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pProFileLogFile )	return;			
		}

		
		std::string strTIME = GetCurTime();
		fprintf ( m_pProFileLogFile, "%s, %s\n", strTIME.c_str(), szBuffer );		
		fflush( m_pProFileLogFile );
		
	}

	void ToProfileExLogFile( const char *szFormat, ... )
	{
		GASSERT(!m_strProfileExLogFile.IsEmpty()&&"log file were not set.");

		std::string AppPath = sc::getAppPath();

        AppPath.append("\\log");
        //StringCchCopyA(m_szFullDir, MAX_PATH, AppPath.c_str());

		CTime cCurTime = CTime::GetCurrentTime();

		CString strTime; 
		strTime.Format ( "%04d%02d%02d%02d%02d",
						cCurTime.GetYear(),
						cCurTime.GetMonth(),
						cCurTime.GetDay(),
						cCurTime.GetHour(),
						cCurTime.GetMinute() );

		CString strFileName;
		strFileName.Format( "ProfileExLog.%s.csv", strTime );
		m_strProfileExLogFile.Format( "%s\\%s", AppPath.c_str(), strFileName );		
		

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pProFileLogFile )
		{
			m_pProfileExLogFile = _fsopen ( m_strProfileExLogFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pProfileExLogFile )	return;			

			std::string strTIME = GetCurTime();
			fprintf ( m_pProfileExLogFile, "%s, %s\n", strTIME.c_str(), szBuffer );		
			fflush( m_pProfileExLogFile );

			fclose(m_pProfileExLogFile);
		}
	}


	void ToProFileMsgFile( const char *szFormat, ... )
	{
		GASSERT(!m_strProFileMsgFile.IsEmpty()&&"log file were not set.");

		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		if( !m_pProFileMsgFile )
		{
			m_pProFileMsgFile = _fsopen ( m_strProFileMsgFile.GetString(), "at", _SH_DENYNO );
			if ( !m_pProFileMsgFile )	return;			
		}

		
		std::string strTIME = GetCurTime();
		fprintf ( m_pProFileMsgFile, "%s, %s\n", strTIME.c_str(), szBuffer );		
		fflush( m_pProFileMsgFile );

	}
    
    void ToLogFile( std::string& Message, BOOL bSaveCSV )
    {        
		GASSERT(!m_strLogFile.IsEmpty() && "log file were not set.");
		
		if (!m_pLogFile)
		{
			m_pLogFile = _fsopen(m_strLogFile.GetString(), "at", _SH_DENYNO);
			if (m_pLogFile)
			{
				if (m_bSubjectName_ERROR)
				{
					fprintf ( m_pLogFile, "-----------------------------------------------\n" );
					fprintf ( m_pLogFile, "				    ERROR REPORT\n" );
					fprintf ( m_pLogFile, "-----------------------------------------------\n" );
				}
				else
				{
					fprintf ( m_pLogFile, "-----------------------------------------------\n" );
					fprintf ( m_pLogFile, "                 ERROR REPORT\n" );
					fprintf ( m_pLogFile, "-----------------------------------------------\n" );
				}

				fprintf(m_pLogFile, "\n");
				fprintf(m_pLogFile, "Execution time : %s\n", m_strSTARTTIME.GetString());

				m_bLogWrite = true;
			}
			else
            {
                return;
            }
		}
		
		std::string strTIME(GetCurTime());
		fprintf(m_pLogFile, "[%s] %s\n", strTIME.c_str(), Message.c_str());
		fflush(m_pLogFile);

		if ( bSaveCSV )
		{
			sc::writeLogError(Message);
		}
	}

	void ToStateCheckFile ( const char *szFormat, ... )
	{
		va_list argList;
		char szBuffer[MAX_STRLENGTH] = "";

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
		va_end(argList);

		FILE *fp;

		CString strFileName = m_szFullDir;
		strFileName += "GameStateFile.csv";
		
		fp = _fsopen ( strFileName.GetString(), "at", _SH_DENYNO );
		if ( fp )
		{		
			fprintf ( fp, "%s\n", szBuffer );

			fclose(fp);
		}
	}

	void ClearFile ( const char *szFileName )
	{
		//	Note	:	���� ��� ���̱�
		//
		char	szFullPathFileName[MAX_PATH] = "";

		StringCchCopy( szFullPathFileName, MAX_PATH, m_szFullDir );
		StringCchCat( szFullPathFileName, MAX_PATH, szFileName );

		DeleteFile ( szFullPathFileName );
	}

	const char* GetPath ()	{ return m_szFullDir; }

	HRESULT	OneTimeSceneInit(const SUBPATH* pPath, bool bLogFileFinalOpen)
	{
		//FinalCleanup();

        char szPROFILE[MAX_PATH] = {0};
        SHGetSpecialFolderPath(NULL, szPROFILE, CSIDL_PERSONAL, FALSE);

		StringCchCopy(m_szFullDir, MAX_PATH, szPROFILE);
		StringCchCat(m_szFullDir, MAX_PATH, pPath->DebugInfoRoot().c_str()); // SUBPATH::DEBUGINFO_ROOT
		CreateDirectory(m_szFullDir, NULL);

        // ������ �α������� �����
        sc::file::deleteFiles(std::string(m_szFullDir));

		m_bLogFileFinalOpen = bLogFileFinalOpen;

		for (int i=0; i<MAX_CHANNEL; i++)
            m_Info[i].Init();

		m_pFont = NULL;
		
		//	����Ʈ ä�� �⺻ 1�� �ۼ�
		INFO* pNewInfo = new INFO;
		pNewInfo->Init();
		m_ListInfo.AddTail(pNewInfo);
		m_ListInfo.SetTail();

		m_strSTARTTIME = GetCurTime().c_str();

		// Use time structure to build a customized time string.
		//time_t ltime;
		//struct tm *today;

		//time(&ltime);
		//today = localtime(&ltime);

		//TCHAR szFileName[MAX_PATH] = {0};
		//// Use strftime to build a customized time string.
		//_tcsftime( szFileName, MAX_PATH, _T("log.%Y%m%d%H%M.txt"), today );

		CTime cCurTime = CTime::GetCurrentTime();

		CString strTime; 
		strTime.Format ( "%04d%02d%02d%02d%02d",
						cCurTime.GetYear(),
						cCurTime.GetMonth(),
						cCurTime.GetDay(),
						cCurTime.GetHour(),
						cCurTime.GetMinute() );

		CString strFileName;
		strFileName.Format( "log.%s.txt", strTime );
		m_strLogName = strFileName;
		m_strLogFile.Format ( "%s\\%s", m_szFullDir, m_strLogName );

		strFileName.Format( "TutorialMaplog.%s.csv", strTime );		
		m_strTutorialLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "InstMaplog.%s.csv", strTime );		
		m_strInstLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "HackingLog.%s.csv", strTime );		
		m_strHackingLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "MoneyUpdate.%s.log", strTime );
		m_strMoneyLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "ClubNotice.%s.log", strTime );
		m_strClubNoticeLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "block_detected.%s.txt", strTime );
		m_strBlockLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "club_income.%s.txt", strTime );
		m_strClubIncomeLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "club_deathmatch.%s.csv", strTime );
		m_strCDMLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "GameState.%s.csv", strTime );
		m_strGameStateLogFile.Format( "%s\\%s", m_szFullDir, strFileName );

		strFileName.Format( "ProFileLog.%s.csv", strTime );
		m_strProFileLogFile.Format( "%s\\%s", m_szFullDir, strFileName );		

		strFileName.Format( "ProfileExLog.%s.csv", strTime );
		m_strProfileExLogFile.Format( "%s\\%s", m_szFullDir, strFileName );		
		
		strFileName.Format( "ProFileMsg.%s.csv", strTime );
		m_strProFileMsgFile.Format( "%s\\%s", m_szFullDir, strFileName );		

		m_bLogWrite = false;

		m_pLogFile = NULL;
		m_pTutorialLogFile= NULL;
		m_pInstLogFile= NULL;
		m_pHackingLogFile= NULL;
		m_pMoneyLogFile= NULL;
		m_pClubNoticeLogFile= NULL;
		m_pBlockLogFile= NULL;
		m_pClubIncomeLogFile= NULL;
		m_pCDMLogFile= NULL;
		m_pGameStateLogFile= NULL;
		m_pProFileLogFile = NULL;
		m_pProfileExLogFile = NULL;
		m_pProFileMsgFile = NULL;

		return S_OK;
	}

	HRESULT	InitDeviceObjects(CD3DFontPar* pFont)
	{
		if (!pFont)
            return E_FAIL;
		
		// ��Ʈ ����
		m_pFont = pFont;

		return S_OK;
	}

	HRESULT FinalCleanup()
	{
		m_ListInfo.SetHead ();
		while ( !m_ListInfo.IsEnd () )
		{
			INFO* pInfo;
			m_ListInfo.GetCurrent ( pInfo );

			SAFE_DELETE ( pInfo );
			m_ListInfo.GoNext ();
		}
		m_ListInfo.RemoveAll ();

		CleanLogFile();

		bool a = m_bLogWrite;
		bool b = m_bLogFileFinalOpen;
		if ( m_bLogWrite && m_bLogFileFinalOpen )
		{
			CString strPROCESS;
			strPROCESS.Format ( "notepad %s", m_strLogName );

			STARTUPINFO si;
			memset ( &si, 0, sizeof ( STARTUPINFO ) );
			si.cb = sizeof ( STARTUPINFO );
			si.dwFlags = 0;

			PROCESS_INFORMATION pi;
			pi.hProcess = NULL;
			pi.hThread = NULL;
			CreateProcess ( NULL, strPROCESS.GetBuffer(), NULL, NULL, FALSE, 0, NULL, m_szFullDir, &si, &pi );
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
            strPROCESS.ReleaseBuffer();
		}

		return S_OK;
	}

	void CleanLogFile()
	{
		if ( m_pLogFile ) fclose( m_pLogFile );
		if ( m_pTutorialLogFile ) fclose( m_pTutorialLogFile );
		if ( m_pInstLogFile ) fclose( m_pInstLogFile );
		if ( m_pHackingLogFile ) fclose( m_pHackingLogFile );
		if ( m_pMoneyLogFile ) fclose( m_pMoneyLogFile );
		if ( m_pClubNoticeLogFile ) fclose( m_pClubNoticeLogFile );
		if ( m_pBlockLogFile ) fclose( m_pBlockLogFile );
		if ( m_pClubIncomeLogFile ) fclose( m_pClubIncomeLogFile );
		if ( m_pCDMLogFile ) fclose( m_pCDMLogFile );
		if ( m_pGameStateLogFile ) fclose( m_pGameStateLogFile );
		if ( m_pProFileLogFile ) fclose( m_pProFileLogFile ) ;
		if ( m_pProFileMsgFile ) fclose( m_pProFileMsgFile ) ;
		// m_pProfileExLogFile �� �������� fopen, fcolse �ϹǷ� ���� ó���� �ʿ����

		

		m_pLogFile = NULL;
		m_pTutorialLogFile = NULL;
		m_pInstLogFile = NULL;
		m_pHackingLogFile = NULL;
		m_pMoneyLogFile = NULL;
		m_pClubNoticeLogFile = NULL;
		m_pBlockLogFile = NULL;
		m_pClubIncomeLogFile = NULL;
		m_pCDMLogFile = NULL;
		m_pGameStateLogFile = NULL;
		m_pProFileLogFile = NULL;
		m_pProFileMsgFile = NULL;
		m_pProfileExLogFile = NULL;
	}

	// �ӽ� �׽�Ʈ��
	bool s_bOn = false;
	FILE* s_fp = NULL;

	void TempOnOff(bool bOn, const SUBPATH* pPath, const char* szFileName)
	{
		s_bOn = bOn;
		if( s_bOn )
		{
			if( s_fp )
			{
				fprintf( s_fp, "//=====================================================================\n\n" );
				fclose( s_fp );
				s_fp = NULL;
			}

            TCHAR szFullPath[MAX_PATH] = {0};
			SHGetSpecialFolderPath(NULL, szFullPath, CSIDL_PERSONAL, FALSE);
			StringCchCat(szFullPath, MAX_PATH, pPath->SaveRoot().c_str()); //SUBPATH::SAVE_ROOT );
			StringCchCat(szFullPath, MAX_PATH, szFileName);

			s_fp = _fsopen( szFullPath, "at", _SH_DENYNO );

			if( s_fp )
				fprintf( s_fp, "//=====================================================================\n" );
			else
				s_bOn = false;
		}
		else
		{
			if( s_fp )
			{
				fprintf( s_fp, "//=====================================================================\n\n" );
				fclose( s_fp );
				s_fp = NULL;
			}
		}
	}

	void TempToFile( const char* szFormat, ... )
	{
		if( !s_bOn || !s_fp )
			return;

		va_list argList;
		char szBuffer[ MAX_STRLENGTH ] = "";

		va_start( argList, szFormat );
		StringCbVPrintf( szBuffer, MAX_STRLENGTH, szFormat, argList );
		va_end( argList );

		fprintf( s_fp, "%s\n", szBuffer );
	}
}


namespace CLOCKCHECK
{
	//	Note : cpu Ŭ�� ������ ���� ���� ����.
	//
	DWORD dwShi, dwSlo, dwEhi, dwElo;
}