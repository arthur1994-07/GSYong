
// PatchPrimeManNewDlg.cpp : implementation file
//

#include "stdafx.h"

#include <tlhelp32.h>   // CreateToolhelp32Snapshot() �� ����ϱ� ���� �ش�����
#include <Psapi.h>
#include <Msi.h>	// MsiQueryProductState

#include "./Thread/PatchThread.h"
#include "./MinLua.h"
#include "./ServiceProviderDefine.h"
#include "./SUBPATH.h"
#include "./LogControl.h"
#include "./BaseString.h"
#include "./HttpPatch.h"
#include "./Cabinet.hpp"

#include "PatchPrimeManNew.h"
#include "PatchPrimeManNewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int VERSION = -1;
int SGAMEVER = -1;

// CPatchPrimeManNewDlg dialog

CPatchPrimeManNewDlg::CPatchPrimeManNewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchPrimeManNewDlg::IDD, pParent)
	, m_GameVersion( -1 )
	, m_PatchVersion( -1 )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pHttpPatch = new CHttpPatch();
	m_pPatch = new PatchThread( AfxGetThread()->m_nThreadID, m_pHttpPatch );
}


CPatchPrimeManNewDlg::~CPatchPrimeManNewDlg()
{
	if ( m_pPatch )
	{
		m_pPatch->end();
		SAFE_DELETE( m_pPatch );
	}

	SAFE_DELETE( m_pHttpPatch );
}

void CPatchPrimeManNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_PROGRESS1, m_DownloadProgress );
	DDX_Control( pDX, IDC_LIST_LOG, m_ListBox );
	DDX_Control( pDX, IDC_STATIC, m_stcInfo );
}

BEGIN_MESSAGE_MAP(CPatchPrimeManNewDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPatchPrimeManNewDlg message handlers

BOOL CPatchPrimeManNewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	SetWindowText( "Launcher Patch" );
	
	if ( VERSION < 0 ) // -1
	{
		SendMessage ( WM_CLOSE );
		return TRUE;
	}
	
	m_GameVersion = SGAMEVER;
	m_PatchVersion = VERSION;

	//	�����Ȳ ����
	DWORD* pCurPos = NULL;
	DWORD* pCurEnd = NULL;
	NS_LOG_CONTROL::GetProcessCurPositionAddr ( &pCurPos, &pCurEnd );
	m_pHttpPatch->SetProgressValue ( pCurPos, pCurEnd );

	// load country file
	if ( !LoadCountryFile() )
	{
		::AfxMessageBox( _T( "Country file load error" ) );
		ListAddString( "error LoadCountryFile." );
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return FALSE;
	}

	m_pPath = new SUBPATH( ENGINE::emServiceProvider );

	// ���� / ���¶��� ���� ���� �� ���� ������ ���ٸ� ����
	autopatch::SetBasicPath( m_pPath );

	// load param.ini
	if ( !LoadParamFile() )
	{
		::AfxMessageBox( _T( "Param file load error" ) );
		ListAddString( "error LoadParamFile." );
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return FALSE;
	}

	// internet option ����
	SetInternetOption();

	m_DownloadProgress.SetRange( 0, 10000 );
	m_DownloadProgress.SetPos( 0 );
	UpdateData( 1 );	

	// patchlist.lua ������ ���� �޾ƾ� ������ ������ �� �� �ִ�.
	// ������ �޴´�.
	ForceDownloadFile( autopatch::strPatchListCabFileName );

	// load patch list file
	if ( !LoadPatchListFile() )
	{
		::AfxMessageBox( _T( "Patchlist file load error" ) );
		ListAddString( "error LoadPatchListFile." );
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return FALSE;
	}

	// Ŭ���̾�Ʈ ��ó��ġ���ϸ���Ʈ �ε�
	LoadClientLauncherPatchList();

	// ���� ���ϸ���Ʈ �ٿ�ε�
	ForceDownloadFile( autopatch::strServerCabFileList );

	if ( !LoadServerFileList() )
	{
		ListAddString( "error LoadServerFileList." );
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return FALSE;
	}

	if ( !LoadClientFileList() )
	{
		ListAddString( "error LoadClientFileList." );
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return FALSE;
	}

	// ��ó ���μ��� ���̱�
	KillLauncher();

	for ( size_t loop = 0; loop < m_vecPatchFile.size(); loop++ )
	{
		AddDownloadFile( m_vecPatchFile[loop].strFileName, m_vecPatchFile[loop].strMD5, m_vecPatchFile[loop].strPath, m_vecPatchFile[loop].NeedVersionUpdate );
	}

	SetTimer( E_START_PATCH, 1000, NULL );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPatchPrimeManNewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPatchPrimeManNewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPatchPrimeManNewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	UpdateProgressControls();

	switch ( nIDEvent )
	{
	case E_START_PATCH:
		{
			if ( m_pPatch->ErrorOccurred() )
			{
				KillTimer ( E_START_PATCH );
				EndPatch( true );
				PostMessage( WM_QUIT );
			}

			if ( m_pPatch->Empty() )
			{
				KillTimer ( E_START_PATCH );
				EndPatch();
				PostMessage( WM_QUIT );
			}
		}
		break;
	case E_FORCE_EXIT:
		{
			KillTimer ( E_START_PATCH );
			EndPatch( true );
			PostMessage( WM_QUIT );
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CPatchPrimeManNewDlg::LoadCountryFile()
{
	lua::lua_init();
	if ( !lua::lua_doFile( autopatch::GetCountryFilePath() ) )
	{
		ListAddString( "country.luc file missing." );
		return FALSE;
	}

	// Service Provider
	LuaPlus::LuaObject ProviderIter = lua::lua_getGlobalFromName( "ServiceProvider" );
	for ( LuaPlus::LuaTableIterator it( ProviderIter ); it; it.Next() )
	{
		std::string strKey( ( const char* ) it.GetKey().GetString() );
		if ( strKey == "Country" )
		{
			int ServiceProvider = it.GetValue().GetInteger();
			ENGINE::emServiceProvider = EMSERVICE_PROVIDER( ServiceProvider );
		}
	}
	return TRUE;
}

BOOL CPatchPrimeManNewDlg::LoadPatchListFile()
{
	CString strTemp = autopatch::GetPatchListFilePath().c_str();
	strTemp = strTemp.Left( strTemp.ReverseFind ( '.' ) );

	lua::lua_init();
	if ( !lua::lua_doFile( strTemp ) )
	{
		ListAddString( "patchlist.lua file missing." );
		return FALSE;
	}

	// file_name
	LuaPlus::LuaObject file_name = lua::lua_getGlobalFromName( "file_name" );
	for ( LuaPlus::LuaTableIterator it( file_name ); it; it.Next() )
	{
		std::string strKey( ( const char* ) it.GetKey().GetString() );
		if ( strKey == "visual_studio_redistributable_package" )
		{
			std::wstring strFileW( ( const wchar_t* )it.GetValue().GetWString() );
			autopatch::strVcredistCabFileName = autopatch::unicodeToAnsi( strFileW );
		}
		else if ( strKey == "launcher" )
		{
			std::wstring strFileW( ( const wchar_t* )it.GetValue().GetWString() );
			autopatch::strLauncherCabFileName = autopatch::unicodeToAnsi( strFileW );
		}
	}

	// download_list
	LuaPlus::LuaObject download_list = lua::lua_getGlobalFromName( "download_list" );
	for ( LuaPlus::LuaTableIterator it( download_list ); it; it.Next() )
	{
		for ( LuaPlus::LuaTableIterator it2( it.GetValue() ); it2; it2.Next() )
		{
			std::wstring strFileW( ( const wchar_t* )it2.GetValue().GetWString() );
			std::string strFileA = autopatch::unicodeToAnsi( strFileW );
			it2.Next();

			std::wstring strMD5W( ( const wchar_t* )it2.GetValue().GetWString() );
			std::string strMD5A = autopatch::unicodeToAnsi( strMD5W );
			it2.Next();

			std::wstring strPathW( ( const wchar_t* )it2.GetValue().GetWString() );
			std::string strPathA = autopatch::unicodeToAnsi( strPathW );
			it2.Next();

			DOWNLOAD_INFO Download;
			Download.strMD5 = strMD5A;
			Download.strFileName = strFileA;
			Download.strPath = strPathA;

			Download.NeedVersionUpdate = it2.GetValue().GetBoolean();

			m_vecPatchFile.push_back( Download );
		}
	}

	// delete_list
	LuaPlus::LuaObject delete_list = lua::lua_getGlobalFromName( "delete_list" );
	for ( LuaPlus::LuaTableIterator it( delete_list ); it; it.Next() )
	{
		std::wstring strFileW( ( const wchar_t* )it.GetValue().GetWString() );
		std::string strFileA = autopatch::unicodeToAnsi( strFileW );
		m_vecDeleteFile.push_back( strFileA );
	}

	return TRUE;
}

BOOL CPatchPrimeManNewDlg::LoadParamFile()
{
	return autopatch::LOAD_PARAM();
}

BOOL CPatchPrimeManNewDlg::VersionUp()
{
	CString str;
	str.Format ( "%s%s", autopatch::strRanOnlinePath.c_str(), autopatch::strVersionFileName.c_str() );
	SetFileAttributes( str.GetString(), FILE_ATTRIBUTE_NORMAL );	

	FILE* cfp = _fsopen( str.GetString(), "rb+", _SH_DENYNO );	
	if ( !cfp )
	{
		ListAddString( "error VersionUp if ( !cfp )." );
		return FALSE;
	}

	if ( 1 != fwrite( &m_PatchVersion, sizeof ( int ), 1, cfp ) )
	{
		ListAddString( "error VersionUp fwrite." );
		fclose( cfp );
		return FALSE;
	}

	fclose( cfp );
	return TRUE;
}

BOOL CPatchPrimeManNewDlg::StartPatch()
{
	return TRUE;
}

BOOL CPatchPrimeManNewDlg::EndPatch( bool bError /*= false */ )
{
	// ������ �ϴ� ���� ����
	for ( size_t loop = 0; loop < m_vecPatchFile.size(); loop++ )
	{
		std::string strFileName( autopatch::strRanOnlineSavePath );
		strFileName += m_vecPatchFile[loop].strFileName;
		autopatch::DELETEFILE( strFileName.c_str() );
	}

	// �������Ͽ� �ִ� ���ʿ��� ���� ����
	DeleteNeedlessFile();

	// patchlist.lua.cab ����
	CString strCabFile;
	strCabFile.Format( "%s%s", autopatch::strRanOnlineSavePath.c_str(), autopatch::strPatchListCabFileName.c_str() );
	autopatch::DELETEFILE( strCabFile.GetString() );

	// patchlist.lua ����
	strCabFile = autopatch::strPatchListCabFileName.c_str();
	strCabFile = strCabFile.Left( strCabFile.ReverseFind ( '.' ) );
	CString strLuaFile;
	strLuaFile.Format( "%s%s", autopatch::strRanOnlineSavePath.c_str(), strCabFile.GetString() );
	autopatch::DELETEFILE( strLuaFile.GetString() );

	// filelist.bin.cab ����
	strCabFile.Format( "%s%s", autopatch::strRanOnlineSavePath.c_str(), autopatch::strServerCabFileList.c_str() );
	autopatch::DELETEFILE( strCabFile.GetString() );

	// filelist.bin ����
	strCabFile = autopatch::strServerCabFileList.c_str();
	strCabFile = strCabFile.Left( strCabFile.ReverseFind ( '.' ) );
	CString strBinFile;
	strBinFile.Format( "%s%s", autopatch::strRanOnlineSavePath.c_str(), strCabFile.GetString() );
	autopatch::DELETEFILE( strBinFile.GetString() );

	if ( bError )
	{
		return FALSE;
	}

	// Ŭ���̾�Ʈ ���ϸ���Ʈ ������Ʈ
	FileListUpdate();

	LauncherFileListUpdate();

	// ���� ���
	VersionUp();

	// ��ó ����
	CreateLauncherProcess();

	return TRUE;
}

void CPatchPrimeManNewDlg::EndPatchThread()
{
	if ( m_pPatch )
	{
		m_pPatch->end();
	}
}

void CPatchPrimeManNewDlg::UpdateProgressControls()
{
	if ( 0 == NS_LOG_CONTROL::nCurEnd )
	{
		return;
	}
	
	DWORD percent = ( ( float ) NS_LOG_CONTROL::nCurPos / NS_LOG_CONTROL::nCurEnd ) * 10000;
	m_DownloadProgress.SetPos( percent );

	CString strProcess;
	strProcess.Format( "%d / %d", NS_LOG_CONTROL::nCurPos, NS_LOG_CONTROL::nCurEnd );
	m_stcInfo.SetWindowText( strProcess );
}

void CPatchPrimeManNewDlg::AddDownloadFile( const std::string& strFileName, const std::string& strMD5, const std::string& strPath, bool NeedVersionUpdate )
{
	// �츮�� ��ġ�Ϸ��� �������Ű�� Ȯ��
	// 2008 sp1 mfc
	// Microsoft Visual C++ 2008 Redistributable - x86 9.0.30729.6161
	if ( strFileName == autopatch::strVcredistCabFileName )
	{
		CString csProduct = "{9BE518E6-ECC6-35A9-88E4-87755C07200F}";
		INSTALLSTATE t = MsiQueryProductState( csProduct );
		if ( INSTALLSTATE_DEFAULT == t )
		{
			ListAddString( "Visual C++ 2008 Redistributable - x86 9.0.30729.6161 already installed." );
			return;
		}
	}

	SFILENODE FileNode;
	FileNode.SetFileName( strFileName.c_str() );
	FileNode.SetSubPath( strPath.c_str() );
	FileNode.SetMD5( strMD5.c_str() );

	CPatchSetNode* pNode = m_ClientLauncherPatchTree.Find( &FileNode );
	if ( pNode )
	{
		std::string tempMD5( FileNode.szMD5 );
		if ( strMD5 == tempMD5 )
		{
			if ( strFileName != autopatch::strLauncherCabFileName )
			{
				CString strMessage;
				strMessage.Format( "%s already exist.", strFileName.c_str() );
				ListAddString( strMessage );
				return;
			}
		}
	}

	PatchFile File;
	File.FileName = strFileName;
	File.MD5 = strMD5;
	File.FilePath = strPath;
	File.GameVersion = m_GameVersion;
	File.NeedVersionUpdate = NeedVersionUpdate;

	m_pPatch->AddJob( File );
}

void CPatchPrimeManNewDlg::ListAddString( const CString& strLog )
{
	CString strLogTemp = strLog;
	if ( strLogTemp[strLogTemp.GetLength()-1] == '\n' )
	{
		strLogTemp = strLogTemp.Left ( strLogTemp.GetLength()-1 );
		if ( strLogTemp[strLogTemp.GetLength()-1] == '\r' )
		{
			strLogTemp = strLogTemp.Left ( strLogTemp.GetLength()-1 );
		}
	}

	int nIndex = m_ListBox.AddString( strLogTemp );
	m_ListBox.SetCurSel( nIndex );
}

void CPatchPrimeManNewDlg::ListAddString( UINT nIDS )
{
	CString strLogTemp;
	strLogTemp.LoadString( nIDS );

	int nIndex = m_ListBox.AddString ( strLogTemp );
	m_ListBox.SetCurSel ( nIndex );
}

BOOL CPatchPrimeManNewDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch ( pMsg->message )
	{
	case WM_LISTADDSTRING:
		{
			CString* pstr = ( CString* )pMsg->lParam;
			ListAddString( *pstr  );
			delete pstr;
			pstr = NULL;
			return TRUE;
		}
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CPatchPrimeManNewDlg::SetInternetOption()
{
	DWORD dwData = 0;
	DWORD dwSize = sizeof( dwData );
	
	InternetQueryOption( NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &dwData, &dwSize );
	dwData = 10;
	InternetSetOption( NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &dwData, sizeof( dwData ) );

	InternetQueryOption( NULL, INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER, &dwData, &dwSize);
	dwData = 10;
	InternetSetOption( NULL, INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER, &dwData, sizeof( dwData ) );
}

BOOL CPatchPrimeManNewDlg::CreateLauncherProcess()
{
	// ���� ����� �Ķ��Ÿ ���� ( �� �α��� �� �� ���Ľ� �ʿ� )
	CWinApp *pApp = AfxGetApp();
	if ( !pApp )
		return FALSE;

	CString StrCmdLine = pApp->m_lpCmdLine;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "/" );
	STRUTIL::RegisterSeparator( " " );

	CStringArray strCmdArray;
	STRUTIL::StringSeparate( StrCmdLine, strCmdArray );

	CString strVALUE, strKEY, strToken;
	int nPos( -1 );

	// 0,1�� ���������� �������
	for( int i = 2; i < strCmdArray.GetCount(); ++i )
	{
		strToken = strCmdArray.GetAt(i);
		strVALUE += " " + strToken;
	}

	CString strLauncherFileName = autopatch::strLauncherCabFileName.c_str();
	strLauncherFileName = strLauncherFileName.Left( strLauncherFileName.ReverseFind ( '.' ) );
	CString strTemp;
	strTemp.Format( "\"%s%s\"", autopatch::strRanOnlinePath.c_str(), strLauncherFileName.GetString() );
	
	if ( !ShellExecute( NULL , "open", strTemp.GetString() ,strVALUE.GetString(), NULL, SW_SHOW ) )
	{
		ListAddString( "error ShellExecute." );
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CPatchPrimeManNewDlg::ForceDownloadFile( const std::string& strFileName )
{
	if ( strFileName.empty() )
	{
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return;
	}

	std::string strDownloadPath( autopatch::strRanOnlineSavePath );
	strDownloadPath += strFileName;

	std::string strSubPath( "/" );
	switch ( ENGINE::emServiceProvider )
	{
	case SP_OFFICE_TEST:
	case SP_KOREA:
	case SP_KOREA_TEST:
	case SP_GS:
		break;
	default:
		{
			if ( m_GameVersion != -1 )
			{
				CString strFolder;
				strFolder.Format( "%04d", m_GameVersion );
				strSubPath += strFolder.GetString();
				strSubPath += "/";
			}
		}
		break;
	}

	strSubPath += strFileName;

	int nHttpIndex = 0;
	for ( int i = 0; i < autopatch::MAX_HTTP; ++i ) 
	{
		std::string strHttpAddressTemp = autopatch::HttpAddressTable[i];
		if ( !strHttpAddressTemp.empty() )
		{
			nHttpIndex = i;
			break;
		}
	}

	std::string strHttpAddress = autopatch::HttpAddressTable[nHttpIndex];
	if ( strHttpAddress.empty() )
	{
		return;
	}

	std::string strRealHttpAddress( "http://" );
	strRealHttpAddress += strHttpAddress;

	if ( NET_ERROR == m_pHttpPatch->SetBaseURL ( strRealHttpAddress.c_str() ) )
	{
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return;
	}

	{
		CString strMessage;
		strMessage.Format( "%s.", strFileName.c_str() );
		ListAddString( strMessage );
	}

	if ( NET_ERROR == m_pHttpPatch->GetFile( strSubPath.c_str(), strDownloadPath.c_str() ) )
	{
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return;
	}

	DWORD RECEIVED, TOTALSIZE;
	NS_LOG_CONTROL::GetProcessCurPosition ( &RECEIVED, &TOTALSIZE );

	if ( RECEIVED != TOTALSIZE )
	{
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return;
	}

	std::wstring strDownloadPathW = autopatch::ansiToUnicode( strDownloadPath );
	std::wstring strTargetPathW = autopatch::ansiToUnicode( autopatch::strRanOnlineSavePath );

	Cabinet::CExtract i_ExtrFile;
	if ( !i_ExtrFile.CreateFDIContext() )
	{
		CString strMessage;
		strMessage.Format( "error CreateFDIContext %s.", strFileName.c_str() );
		ListAddString( strMessage );
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return;
	}

	if ( !i_ExtrFile.ExtractFileW( strDownloadPathW.c_str(), strTargetPathW.c_str() ) )
	{
		CString strMessage;
		strMessage.Format( "error ExtractFileW %s.", strFileName.c_str() );
		ListAddString( strMessage );
		SetTimer( E_FORCE_EXIT, 1000, NULL );
		return;
	}

	i_ExtrFile.CleanUp();

	// download end message
	{
		CString strMessage;
		strMessage.Format( "%s download ok.", strFileName.c_str() );
		ListAddString( strMessage );
	}
}

void CPatchPrimeManNewDlg::DeleteNeedlessFile()
{
	for ( size_t loop = 0; loop < m_vecDeleteFile.size(); loop++ )
	{
		std::string strFileName( autopatch::strRanOnlinePath );
		strFileName += m_vecDeleteFile[loop];
		autopatch::DELETEFILE( strFileName.c_str() );
	}
}

HANDLE CPatchPrimeManNewDlg::GetProcessList( LPCTSTR szFilename )
{
	HANDLE hProcessSnapshot;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;

	hProcessSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, 0 );

	if ( hProcessSnapshot == INVALID_HANDLE_VALUE ) 
		return INVALID_HANDLE_VALUE;

	pe32.dwSize = sizeof( PROCESSENTRY32 );

	Process32First( hProcessSnapshot, &pe32 );

	do
	{
		hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
		if ( hProcess != NULL )
		{
			if ( _tcscmp( pe32.szExeFile, szFilename ) == 0 )
			{
				return hProcess;
			}
		}

	} while ( Process32Next( hProcessSnapshot, &pe32 ) );

	return INVALID_HANDLE_VALUE;
}

BOOL CPatchPrimeManNewDlg::SafeTerminateProcess( HANDLE hProcess, UINT uExitCode )
{
	DWORD dwTID, dwCode, dwErr = 0;
	HANDLE hProcessDup = INVALID_HANDLE_VALUE;
	HANDLE hRT = NULL;
	HINSTANCE hKernel = GetModuleHandle( "Kernel32" );

	BOOL bSuccess = FALSE;
	BOOL bDup = DuplicateHandle( GetCurrentProcess(), 
		hProcess, 
		GetCurrentProcess(), 
		&hProcessDup, 
		PROCESS_ALL_ACCESS, 
		FALSE, 
		0);
	if ( GetExitCodeProcess(( bDup ) ? hProcessDup : hProcess, &dwCode ) 
		&& ( dwCode == STILL_ACTIVE ) )
	{
		FARPROC pfnExitProc;
		pfnExitProc = GetProcAddress( hKernel, "ExitProcess" );
		hRT = CreateRemoteThread( ( bDup ) ? hProcessDup : hProcess, 
			NULL, 
			0, 
			( LPTHREAD_START_ROUTINE )pfnExitProc,
			( PVOID )uExitCode, 0, &dwTID );
		if ( hRT == NULL ) dwErr = GetLastError();
	}
	else 
	{
		dwErr = ERROR_PROCESS_ABORTED;
	}
	if ( hRT )
	{
		WaitForSingleObject( ( bDup ) ? hProcessDup : hProcess, INFINITE );
		CloseHandle( hRT );
		bSuccess = TRUE;
	}
	if ( bDup )
		CloseHandle( hProcessDup );
	if ( !bSuccess )
		SetLastError( dwErr );

	return bSuccess;
}

void CPatchPrimeManNewDlg::KillLauncher()
{
	CString strLauncherFileName = autopatch::strLauncherCabFileName.c_str();
	strLauncherFileName = strLauncherFileName.Left( strLauncherFileName.ReverseFind ( '.' ) );

	HANDLE hProcess;
	hProcess = GetProcessList( strLauncherFileName );
	if ( hProcess == INVALID_HANDLE_VALUE )
	{
		return;
	}

	SafeTerminateProcess( hProcess, 1 );
}

BOOL CPatchPrimeManNewDlg::LoadServerFileList()
{
	CString str;
	str.Format( "%s%s", autopatch::strRanOnlineSavePath.c_str(), autopatch::strServerFileList.c_str() );

	FILE* fp = _fsopen( str.GetString(), "rb", _SH_DENYNO );
	if( !fp )
	{
		::AfxMessageBox( _T( "ServerFilelist _fsopen error" ) );
		return FALSE;
	}

	SFILENODE FileInfo;

	// ù��° ���� ����
	if( 1 != fread( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		::AfxMessageBox( _T( "ServerFilelist fread error" ) );
		fclose ( fp );
		return FALSE;
	}

	// ���� �ɼ� ����
	INT nVerOffset(0);

	switch( FileInfo.Ver )
	{
	case 1: nVerOffset = VER_1_OFFSET; break;
	case 2: nVerOffset = VER_2_OFFSET; break;
	case 3: nVerOffset = VER_3_OFFSET; break; 
	default:
		::AfxMessageBox( _T( "ServerFilelist offset is different!" ) );
		return FALSE;
	}

	// ���� ������ �ɼ� ��ŭ ���� �������� �ڷ� �̵��Ѵ�.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if ( fseek( fp, lOffset, SEEK_CUR ) )
	{
		::AfxMessageBox( _T( "ServerFilelist fseek error" ) );
		fclose( fp );
		return FALSE;
	}

	// �ι�° ���� ����
	if( 1 != fread( &FileInfo, nVerOffset, 1, fp ) )
	{
		::AfxMessageBox( _T( "ServerFilelist fread(after offset) error" ) );
		fclose ( fp );
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE NewFile;
		if( 1 != fread( &NewFile, nVerOffset, 1, fp ) )
		{
			::AfxMessageBox( _T( "ServerFilelist loop fread error" ) );
			fclose( fp );
			return FALSE;
		}

		m_ServerFileTree.Add( &NewFile );

// 		CString strMessage;
// 		strMessage.Format( "filelist %s %d %s", NewFile.FileName, NewFile.Ver, NewFile.SubPath );
// 		ListAddString( strMessage );
	}

	fclose( fp );

	return TRUE;
}

BOOL CPatchPrimeManNewDlg::LoadClientFileList()
{
	CString str;
	str.Format( "%s%s", autopatch::strRanOnlinePath.c_str(), autopatch::strClientFileList.c_str() );

	FILE* fp = _fsopen( str.GetString(), "rb", _SH_DENYNO );
	if( !fp )
	{
		::AfxMessageBox( _T( "ClientFilelist _fsopen error" ) );
		return FALSE;
	}

	SFILENODE FileInfo;

	if( 1 != fread( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		::AfxMessageBox( _T( "ClientFilelist fread error" ) );
		fclose ( fp );
		return FALSE;
	}

	// ���� �ɼ� ����
	INT nVerOffset(0);

	switch( FileInfo.Ver )
	{
	case 1: nVerOffset = VER_1_OFFSET; break;
	case 2: nVerOffset = VER_2_OFFSET; break;
	case 3: nVerOffset = VER_3_OFFSET; break;
	default:
		::AfxMessageBox( _T( "ClientFilelist offset is different!" ) );
		return FALSE;
	}

	// ���� ������ �ɼ� ��ŭ ���� �������� �ڷ� �̵��Ѵ�.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if( fseek( fp, lOffset, SEEK_CUR) )
	{
		::AfxMessageBox( _T( "ClientFilelist fseek error" ) );
		fclose( fp );
		return FALSE;
	}

	if( 1 != fread( &FileInfo, nVerOffset, 1, fp ) )
	{
		::AfxMessageBox( _T( "ClientFilelist fread(after offset) error" ) );
		fclose( fp );
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE NewFile;
		if( 1 != fread( &NewFile, nVerOffset, 1, fp ) )
		{
			::AfxMessageBox( _T( "ClientFilelist loop fread error" ) );
			fclose( fp );
			return FALSE;
		}

		m_ClientFileTree.Add( &NewFile );

// 		CString strMessage;
// 		strMessage.Format( "filelist(client) %s %d %s", NewFile.FileName, NewFile.Ver, NewFile.SubPath );
// 		ListAddString( strMessage );
	}

	fclose( fp );

	return TRUE;
}

BOOL CPatchPrimeManNewDlg::LoadClientLauncherPatchList()
{
	CString str;
	str.Format( "%s%s", autopatch::strRanOnlinePath.c_str(), autopatch::strClientLauncherPatchList.c_str() );

	FILE* fp = _fsopen( str.GetString(), "rb", _SH_DENYNO );
	if( !fp )
	{
		/*::AfxMessageBox( _T( "ClientLauncherPatchlist _fsopen error" ) );*/
		return FALSE;
	}

	SFILENODE FileInfo;

	if( 1 != fread( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		::AfxMessageBox( _T( "ClientLauncherPatchlist fread error" ) );
		fclose ( fp );
		return FALSE;
	}

	// ���� �ɼ� ����
	INT nVerOffset(0);

	switch( FileInfo.Ver )
	{
	case 1: nVerOffset = VER_1_OFFSET; break;
	case 2: nVerOffset = VER_2_OFFSET; break;
	case 3: nVerOffset = VER_3_OFFSET; break;
	default:
		::AfxMessageBox( _T( "ClientLauncherPatchlist offset is different!" ) );
		return FALSE;
	}

	// ���� ������ �ɼ� ��ŭ ���� �������� �ڷ� �̵��Ѵ�.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if( fseek( fp, lOffset, SEEK_CUR) )
	{
		::AfxMessageBox( _T( "ClientLauncherPatchlist fseek error" ) );
		fclose( fp );
		return FALSE;
	}

	if( 1 != fread( &FileInfo, nVerOffset, 1, fp ) )
	{
		::AfxMessageBox( _T( "ClientLauncherPatchlist fread(after offset) error" ) );
		fclose( fp );
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE NewFile;
		if( 1 != fread( &NewFile, nVerOffset, 1, fp ) )
		{
			::AfxMessageBox( _T( "ClientLauncherPatchlist loop fread error" ) );
			fclose( fp );
			return FALSE;
		}

		m_ClientLauncherPatchTree.Add( &NewFile );

// 		CString strMessage;
// 		strMessage.Format( "filelist(launcher) %s %d %s %s", NewFile.FileName, NewFile.Ver, NewFile.SubPath, NewFile.szMD5 );
// 		ListAddString( strMessage );
	}

	fclose( fp );

	return TRUE;
}

BOOL CPatchPrimeManNewDlg::FileListUpdate()
{
	// ��ġ�������� ���� ������ Ŭ���̾�Ʈ ���ϸ���Ʈ ������Ʈ�Ѵ�.
	for ( size_t loop = 0; loop < PatchThread::m_ExtractedFile.size(); loop++ )
	{
		SFILENODE* pExtractedFile = PatchThread::m_ExtractedFile[loop];
		if ( !pExtractedFile )
		{
			continue;
		}
		
		// �� ���� ���� �߻��ϸ� �ȵȴ�. 
		// ��ó��ġ�� �÷��� �ִ� �������Ͼȿ� �ִ� ������ ���� ��ġ����Ʈ ���Ͽ� ���� ��Ȳ�̴�.
		// ��ó��ġ���� ���� ���ϸ���Ʈ�� �ش� ������ �־�� �Ѵ�.
		CPatchSetNode* pServerFileNode = m_ServerFileTree.Find( pExtractedFile );
		if ( !pServerFileNode )
		{
			::MessageBox ( NULL, pExtractedFile->FileName, "Critical error FileListUpdate", MB_ICONEXCLAMATION|MB_OK );
			continue;
		}

		SFILENODE* pServerFile = pServerFileNode->GetFile();
		if ( !pServerFile )
		{
			::MessageBox ( NULL, pExtractedFile->FileName, "Critical error FileListUpdate pServerFile", MB_ICONEXCLAMATION|MB_OK );
			continue;
		}

		CPatchSetNode* pClientFileNode = m_ClientFileTree.Find( pExtractedFile );

		// ��ġ ������ ������ ������ �߻��ؼ� ������ ���� ��쿡 ����ؼ� Ŭ���̾�Ʈ ������ 1�� �����صд�.
		if ( pClientFileNode )
		{
			SFILENODE* pClientFile = pClientFileNode->GetFile();
			pClientFile->Ver = 1;
		}
		else
		{
			SFILENODE NewClientFile;
			NewClientFile.Ver = 1;
			NewClientFile.SetData( pServerFile );
			m_ClientFileTree.Add( &NewClientFile );
		}
		/*
		if ( pClientFileNode )
		{
			SFILENODE* pClientFile = pClientFileNode->GetFile();
			if ( pClientFile && pClientFile->Ver < pServerFile->Ver )
			{
				pClientFile->Ver = pServerFile->Ver;
			}
		}
		else
		{
			SFILENODE NewClientFile;
			NewClientFile.SetData( pServerFile );
			m_ClientFileTree.Add( &NewClientFile );
		}
		*/
	}

	// Ŭ���̾�Ʈ ���ϸ���Ʈ ������Ʈ
	CString str;
	str.Format( "%s%s", autopatch::strRanOnlinePath.c_str(), autopatch::strClientFileList.c_str() );

	FILE* fp = _fsopen ( str.GetString(), "wb", _SH_DENYNO );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

#ifdef CHINAPARAM
	FileInfo.Ver = 2;
#else
	FileInfo.Ver = 3;
#endif

	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose( fp );
		return FALSE;
	}

	FileInfo.Ver = static_cast< int >( m_ClientFileTree.Size() );
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	CPatchSet::SETPATCH_ITER iter = m_ClientFileTree.m_setPatch.begin();
	CPatchSet::SETPATCH_ITER iter_end = m_ClientFileTree.m_setPatch.end();

	for ( ; iter != iter_end; ++iter )
	{
		CPatchSetNode* pNode = *iter;
		if ( !pNode )
		{
			continue;
		}

		SFILENODE* pNewFile = pNode->GetFile();
		if ( !pNewFile )
		{
			continue;
		}

		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose( fp );
			return FALSE;
		}
	}

	fclose ( fp );

	return TRUE;
}

BOOL CPatchPrimeManNewDlg::LauncherFileListUpdate()
{
	m_ClientLauncherPatchTree.Clear();

	for ( size_t loop = 0; loop < m_vecPatchFile.size(); loop++ )
	{
		DOWNLOAD_INFO& DownloadFile = m_vecPatchFile[loop];
		std::string strFileName = PathFindFileName( DownloadFile.strFileName.c_str() );
		SFILENODE FileNode;
		FileNode.SetFileName( strFileName.c_str() );
		FileNode.SetSubPath( DownloadFile.strPath.c_str() );
		FileNode.SetMD5( DownloadFile.strMD5.c_str() );

		m_ClientLauncherPatchTree.Add( &FileNode );
	}

	// Ŭ���̾�Ʈ ���ϸ���Ʈ ������Ʈ
	CString str;
	str.Format( "%s%s", autopatch::strRanOnlinePath.c_str(), autopatch::strClientLauncherPatchList.c_str() );

	FILE* fp = _fsopen ( str.GetString(), "wb", _SH_DENYNO );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

#ifdef CHINAPARAM
	FileInfo.Ver = 2;
#else
	FileInfo.Ver = 3;
#endif

	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose( fp );
		return FALSE;
	}

	FileInfo.Ver = static_cast< int >( m_ClientLauncherPatchTree.Size() );
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	CPatchSet::SETPATCH_ITER iter = m_ClientLauncherPatchTree.m_setPatch.begin();
	CPatchSet::SETPATCH_ITER iter_end = m_ClientLauncherPatchTree.m_setPatch.end();

	for ( ; iter != iter_end; ++iter )
	{
		CPatchSetNode* pNode = *iter;
		if ( !pNode )
		{
			continue;
		}

		SFILENODE* pNewFile = pNode->GetFile();
		if ( !pNewFile )
		{
			continue;
		}

		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose( fp );
			return FALSE;
		}
	}

	fclose ( fp );

	return TRUE;
}