#include "stdafx.h"

#include "../../InternalCommonLib/dxstdafx.h"

#include "../../EngineLib/Common/SUBPATH.h"

#include "../../MfcExLib/MinBugTrap.h"

#include "../../RanLogic/RANPARAM.h"

#include "./Frame.h"
#include "./UITexture.h"

#include "../../SigmaCore/DebugInclude.h"

CUITextureApp theApp;

BEGIN_MESSAGE_MAP( CUITextureApp, CWinApp )
END_MESSAGE_MAP()

CUITextureApp::CUITextureApp()
    : m_pFrame( NULL )
    , m_hIcon( NULL )
{
    //GLogicFile::GetInstance().LoadCountryFile();
    BUG_TRAP::BugTrapInstall( SP_OFFICE_TEST, TSTRING( _T( "UITexture" ) ) );
    m_pPath = new SUBPATH( SP_OFFICE_TEST );
}

CUITextureApp::~CUITextureApp()
{
    delete m_pPath;
    m_pPath = NULL;

    DestroyIcon( m_hIcon );
    OnDestroyApp();
}

BOOL CUITextureApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof( InitCtrls );

    // Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx( &InitCtrls );

	CWinApp::InitInstance();
    afxAmbientActCtx = FALSE;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey( _T( "VisualScript" ) );

    OnCreateApp();

    // To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	m_pFrame = new CFrame;
	if( !m_pFrame )
		return FALSE;

    m_pMainWnd = m_pFrame;

    // create and load the frame with its resources
	m_pFrame->LoadFrame(
        IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
        NULL,
		NULL );

    // set icon
    m_hIcon = LoadIcon( IDR_MAINFRAME );
    m_pFrame->SetIcon( m_hIcon, TRUE );
	m_pFrame->SetIcon( m_hIcon, FALSE );

    // call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

INT CUITextureApp::Run()
{
    MSG msg;

    while( true )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( GetMessage( &msg, NULL, 0, 0 ) == FALSE )
			{
				ExitInstance();
				return msg.wParam;
			}

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
            if( !m_pFrame->OnLoop() )
                break;
		}
	}

    return 1;
}

void CUITextureApp::OnCreateApp()
{
	TCHAR szPath[ MAX_PATH ] = { 0 };
	GetModuleFileName( ::AfxGetInstanceHandle(), szPath, MAX_PATH );
	m_strPath = szPath;

	if( !m_strPath.IsEmpty() )
	{
		DWORD dwFind = m_strPath.ReverseFind( '\\' );
		if( dwFind != -1 )
		{
			m_strPath = m_strPath.Left( dwFind );

			if( !m_strPath.IsEmpty() && 
                m_strPath.GetAt( 0 ) == '"' )
            {
				m_strPath = m_strPath.Right( m_strPath.GetLength() - 1 );
            }
		}
	}

    RANPARAM::LOAD( m_strPath, SP_OFFICE_TEST, true, false );
}

void CUITextureApp::OnDestroyApp()
{
}