// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "../../SigmaCore/Util/SystemInfo.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../RanLogic/GLogicData.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogicServer/DxServerInstance.h"

#include "./Util/CGmCfg.h"
#include "./Util/CConsoleMessage.h"
#include "./Database/DatabaseTable.h"
#include "./Database/ADO/ADOManager.h"
//#include "./Database/ADO/ADOClass.hpp"
#include "./Database/ADO/ADOConnectionString.h"
#include "./Database/ODBC/CGMOdbcManager.h"

#include "./SubDialogs/DlgConfig.h"
#include "./SubDialogs/DlgGMLogin.h"
#include "./SubDialogs/DlgServerSelect.h"

// Main Tab
#include "./MainTabs/CharSearch.h"
#include "./MainTabs/CItemSearch.h"
#include "./MainTabs/CItemChange.h"
#include "./MainTabs/CItemStatistic.h"
#include "./MainTabs/CClubSearch.h"
#include "./MainTabs/CGMItemSearch.h"
#include "./MainTabs/CMoneyStat.h"
#include "./MainTabs/ItemCount.h"
#include "./MainTabs/CSearchDuplicationItems.h"
#include "./MainTabs/ItemTransfer.h"
#include "./DlgSkillSearch.h"

#include "GMCharEdit.h"
#include "MainFrm.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CMainFrame

// g_pFrame = NULL;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(wm_ChangeSelCombo, OnSelchangeDockCombo)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
	: m_bCfgLoad(0)
	, m_bDBConnect(FALSE)
	, m_bLogin(FALSE)
	, m_nUserLevel(0)
	, m_csCaption(_T("GMCharEdit "))
{
	m_bCfgLoad = 0;

	// TODO: add member initialization code here

	//
	// DBMan MUST be set after loading confiurations.
	//
	//m_pDbMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CMainFrame::~CMainFrame()
{
	GMTOOL::CleanUp();
	m_Framework.Destroy();
	FreeMemory();
}

LRESULT CMainFrame::OnSelchangeDockCombo(WPARAM wParam, LPARAM lParam)
{
	int nRetCode = 0;
	BOOL bAdded = FALSE;
	int nCurSel = m_comboBox.GetCurSel();

	m_nSelectedServer = nCurSel;

	if (CGmCfg::GetInstance()->m_nDBType == gmce::ADO)
	{
		m_nSvrGrp	 = m_nSelectedServer;
		
		CjConnectionStrings::ReleaseInstance();
		CjConnectionStrings *pCjConn = CjConnectionStrings::GetInstance();

		

#ifdef TIXML_USE_TICPP
		
		//m_vGROUPS.
		
		m_DbInfo = CGmCfg::GetInstance()->m_vGROUPS[m_nSelectedServer];
		

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_USERDB,
										m_DbInfo.userDB.ip,
										m_DbInfo.userDB.name,
										m_DbInfo.userDB.id,
										m_DbInfo.userDB.pw
										);

		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_GAMEDB,
										m_DbInfo.gameDB.ip,
										m_DbInfo.gameDB.name,
										m_DbInfo.gameDB.id,
										m_DbInfo.gameDB.pw
										);

		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_LOGDB,
										m_DbInfo.logDB.ip,
										m_DbInfo.logDB.name,
										m_DbInfo.logDB.id,
										m_DbInfo.logDB.pw
										);

#else
		m_DbInfo = CGmCfg::GetInstance()->m_vDBINFO[m_nSelectedServer];

		

		

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_USERDB,
										CString(m_DbInfo.UserDB.IP),
										CString(m_DbInfo.UserDB.DB),
										CString(m_DbInfo.UserDB.ID),
										CString(m_DbInfo.UserDB.PW)
										);

		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_GAMEDB,
										CString(m_DbInfo.GameDB.IP),
										CString(m_DbInfo.GameDB.DB),
										CString(m_DbInfo.GameDB.ID),
										CString(m_DbInfo.GameDB.PW)
										);

		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_LOGDB,
										CString(m_DbInfo.LogDB.IP),
										CString(m_DbInfo.LogDB.DB),
										CString(m_DbInfo.LogDB.ID),
										CString(m_DbInfo.LogDB.PW)
										);
#endif


		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}


		CADOManager *pADOMan = CADOManager::GetInstance();

		CString csUserDB;
		CString csGameDB;
		CString csLogDB;

		pCjConn->GetConnString(gmce::ADO_IDX_USERDB, csUserDB);
		pCjConn->GetConnString(gmce::ADO_IDX_GAMEDB, csGameDB);
		pCjConn->GetConnString(gmce::ADO_IDX_LOGDB, csLogDB);

		pADOMan->SetConnStrings(csUserDB, csGameDB, csLogDB);

		//
		// DB Connection Test
		//

		CjADO UserDB;
		CjADO GameDB;
		CjADO LogDB;

		if ( !UserDB.Open(csUserDB) )
		{
			m_bDBConnect = FALSE;
			MessageBox(_T("UserDB connection failure."), _T("DB Connection failure"), MB_OK);
			return FALSE;
		}

		if ( !GameDB.Open(csGameDB) )
		{
			m_bDBConnect = FALSE;
			MessageBox(_T("GameDB connection failure."), _T("DB Connection failure"), MB_OK);
			return FALSE;
		}

		if ( !LogDB.Open(csLogDB) )
		{
			m_bDBConnect = FALSE;
			MessageBox(_T("LogDB connection failure."), _T("DB Connection failure"), MB_OK);
			return FALSE;
		}

		m_bDBConnect = TRUE;
	}
	else
	{
		nRetCode = CGMOdbcManager::GetInstance()->OpenGameDB(m_vServerList[m_nSelectedServer].strOdbcName.GetString(),
												         m_vServerList[m_nSelectedServer].strOdbcUserID.GetString(),
													     m_vServerList[m_nSelectedServer].strOdbcPassword.GetString());

		if (nRetCode == DB_ERROR)
		{
			// DB 연결 실패
			MessageBox(_T("Game DB Open Failed"), _T("ERROR"), MB_ICONEXCLAMATION);
		}

		m_nSvrGrp = m_vServerList[m_nSelectedServer].nSGNum;
	}

	return NULL;
}

void CMainFrame::FreeMemory()
{
	CGmCfg::GetInstance()->ReleaseInstance();
    
	
	CADOManager::ReleaseInstance();
	CGMOdbcManager::GetInstance()->ReleaseInstance();


	CConsoleMessage::GetInstance()->ReleaseInstance();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if (m_bCfgLoad == TRUE) 
	{
		if (CGmCfg::GetInstance()->m_nDBType == gmce::ADO)
		{
			//
			//In case of ADO, the connection test will be done after the selection of server-group.
			//

			m_csCaption.Append(_T("on ADO "));

			DoLoginADO();

			printf("[DEBUG] - #07\n");
		}
		else
		{
			int nRetCode;
			nRetCode = CGMOdbcManager::GetInstance()->OpenUserDB(CGmCfg::GetInstance()->GetOdbcName(),
																 CGmCfg::GetInstance()->GetUserName(),
																 CGmCfg::GetInstance()->GetUserPass());
			if (nRetCode == DB_ERROR)
			{
				m_bDBConnect = FALSE; // DB 연결 실패
				MessageBox(_T("User DB Open Failed"), _T("ERROR"), MB_ICONEXCLAMATION);			

				return -1;
			}
			else
			{
				m_bDBConnect = TRUE; // DB 연결 성공
			}

			m_csCaption.Append(_T("on ODBC "));

			DoLoginODBC();			
		}
	}
	m_csCaption.Append( _T( sc::getFileVersion().c_str() ) );

	if (m_bCfgLoad && m_bDBConnect && m_bLogin)
	{
		printf("[DEBUG] - #08\n");

		// Note : 실행파일의 경로를 찾아서 저장한다. - 반드시 CBasicWnd(CD3dApp) 이전에 수행해야 한다.
		//
		CString strAppPath;

		TCHAR szPath[MAX_PATH] = {0};
		GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
		strAppPath = szPath;

		if ( !strAppPath.IsEmpty() )
		{
			DWORD dwFind = strAppPath.ReverseFind ( '\\' );
			if ( dwFind != -1 )
			{
				m_strAppPath = strAppPath.Left ( dwFind );
				
				if ( !m_strAppPath.IsEmpty() )
				if ( m_strAppPath.GetAt(0) == '"' )
					m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );

				strcpy ( m_szAppPath, m_strAppPath.GetString() );
			}
		}
		// 리스트 컨트롤 초기화
        // InitListHead();	
		// m_bInit = TRUE;

		printf("[DEBUG] - #09\n");

		GLOGIC::SetGlogicPackFile(FALSE);
		GLOGIC::SetGlogicZipFile(TRUE);
		GLOGIC::SetEngineZipFile(TRUE);

        CGMCharEditApp* pApp = (CGMCharEditApp*) AfxGetApp();
		GMTOOL::Create(m_szAppPath, pApp->GetSubPath(), pApp->GetServiceProvider());

		CRect rect;
		int nIndex = m_wndToolBar.GetToolBarCtrl().CommandToIndex(ID_COMBO);
		m_wndToolBar.SetButtonInfo(nIndex, ID_COMBO, TBBS_SEPARATOR, 205);
		m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex, &rect);
		rect.top = 1;
		rect.bottom = rect.top + 250 /*drop height*/;

		printf("[DEBUG] - #10\n");
		
		if(!m_comboBox.Create(CBS_DROPDOWNLIST /*| CBS_SORT*/ | WS_VISIBLE |
			                  WS_TABSTOP | WS_VSCROLL, rect, &m_wndToolBar, ID_COMBO))
		{
			printf("[DEBUG] - #11-1\n");

			TRACE(_T("Failed to create combo-box\n"));
			return FALSE;
		}
		else
		{
			printf("[DEBUG] - #11-2\n");

			m_comboBox.SetWndMsgParent(this);

			if (CGmCfg::GetInstance()->m_nDBType == gmce::ADO)
			{
				CGmCfg *p = CGmCfg::GetInstance();
		
				for (gmce::iterGROUPS iter = p->m_vGROUPS.begin(); iter != p->m_vGROUPS.end(); ++iter)
				{
					m_comboBox.AddString(iter->title.c_str());
				}
				m_comboBox.SetCurSel(m_nSelectedServer);
			}
			else
			{
				for (int nServer=0; nServer < (int) m_vServerList.size(); nServer++)
				{
					m_comboBox.AddString(m_vServerList[nServer].strName.GetString());
				}
				m_comboBox.SetCurSel(m_nSelectedServer);
			}
		}
		// UpdateData(FALSE);
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	// m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	// EnableDocking(CBRS_ALIGN_ANY);
	// DockControlBar(&m_wndToolBar);


	return 0;
}


BOOL CMainFrame::DoLoginADO()
{
	int	 nRetCode = 0;
	BOOL bAdded = FALSE;


	//
	// Select the Server-Group
	//
	CDlgServerSelect dlgServer;
	dlgServer.DoModal();
    
	if (dlgServer.m_nSelectedServer != -1)
	{
		m_nSelectedServer= dlgServer.m_nSelectedServer;
		m_nSvrGrp		 = m_nSelectedServer;
		
		CjConnectionStrings *pCjConn = CjConnectionStrings::GetInstance();
		


#ifdef TIXML_USE_TICPP
		
		//m_vGROUPS.
		
		m_DbInfo = CGmCfg::GetInstance()->m_vGROUPS[m_nSelectedServer];
		

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_USERDB,
										m_DbInfo.userDB.ip,
										m_DbInfo.userDB.name,
										m_DbInfo.userDB.id,
										m_DbInfo.userDB.pw
										);

		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_GAMEDB,
										m_DbInfo.gameDB.ip,
										m_DbInfo.gameDB.name,
										m_DbInfo.gameDB.id,
										m_DbInfo.gameDB.pw
										);

		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_LOGDB,
										m_DbInfo.logDB.ip,
										m_DbInfo.logDB.name,
										m_DbInfo.logDB.id,
										m_DbInfo.logDB.pw
										);

#else
		m_DbInfo = CGmCfg::GetInstance()->m_vDBINFO[m_nSelectedServer];

		

		

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_USERDB,
										CString(m_DbInfo.UserDB.IP),
										CString(m_DbInfo.UserDB.DB),
										CString(m_DbInfo.UserDB.ID),
										CString(m_DbInfo.UserDB.PW)
										);

		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_GAMEDB,
										CString(m_DbInfo.GameDB.IP),
										CString(m_DbInfo.GameDB.DB),
										CString(m_DbInfo.GameDB.ID),
										CString(m_DbInfo.GameDB.PW)
										);

		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		bAdded = pCjConn->AddConnString(gmce::ADO_IDX_LOGDB,
										CString(m_DbInfo.LogDB.IP),
										CString(m_DbInfo.LogDB.DB),
										CString(m_DbInfo.LogDB.ID),
										CString(m_DbInfo.LogDB.PW)
										);
#endif


		if (!bAdded)
		{
			m_bLogin = FALSE;
			return FALSE;
		}

		CADOManager *pADOMan = CADOManager::GetInstance();

		CString csUserDB;
		CString csGameDB;
		CString csLogDB;

		pCjConn->GetConnString(gmce::ADO_IDX_USERDB, csUserDB);
		pCjConn->GetConnString(gmce::ADO_IDX_GAMEDB, csGameDB);
		pCjConn->GetConnString(gmce::ADO_IDX_LOGDB, csLogDB);

		pADOMan->SetConnStrings( csUserDB, csGameDB, csLogDB);



		//
		// DB Connection Test
		//

		CjADO UserDB;
		CjADO GameDB;
		CjADO LogDB;

		if ( !UserDB.Open(csUserDB) )
		{
			m_bDBConnect = FALSE;
			MessageBox(_T("UserDB connection failure."), _T("DB Connection failure"), MB_OK);
			return FALSE;
		}

		if ( !GameDB.Open(csGameDB) )
		{
			m_bDBConnect = FALSE;
			MessageBox(_T("GameDB connection failure."), _T("DB Connection failure"), MB_OK);
			return FALSE;
		}

		if ( !LogDB.Open(csLogDB) )
		{
			m_bDBConnect = FALSE;
			MessageBox(_T("LogDB connection failure."), _T("DB Connection failure"), MB_OK);
			return FALSE;
		}

		m_bDBConnect = TRUE;
	}
	else
	{
		m_bLogin = FALSE;
		return FALSE;
	}

	//
	//Login Dialog
	//

	CDlgGMLogin dlgLogin;


	if (dlgLogin.DoModal() == IDOK)
	{
		if (dlgLogin.m_nRetCode < 20)
		{
			printf("[DEBUG] - #06-1\n");

			m_bLogin = FALSE;
		}
		else
		{
			printf("[DEBUG] - #06-2\n");
			m_bLogin = TRUE;

			m_nUserLevel = dlgLogin.m_nRetCode;
			m_nUserNum   = m_pDbMan->GetGmUserNum();
		}
	}
	else
	{
		m_bLogin = FALSE;		
	}
	
	return m_bLogin;
}


BOOL CMainFrame::DoLoginODBC()
{
	int nRetCode = 0;

	//
	//Login Dialog
	//
	CDlgGMLogin dlgLogin;
	
	if (dlgLogin.DoModal() == IDOK)	
	{
		if (dlgLogin.m_nRetCode < 20)
		{		
			m_bLogin = FALSE;					
		}
		else
		{			
			m_bLogin = TRUE;
			
			m_nUserLevel = dlgLogin.m_nRetCode;								
			m_nUserNum   = m_pDbMan->GetUserNum(dlgLogin.m_strGMID);

			
			//
			// Select the Server-Group
			//
			CDlgServerSelect dlgServer;			
			dlgServer.DoModal();


			if (dlgServer.m_nSelectedServer != -1)
			{
				m_nSelectedServer = dlgServer.m_nSelectedServer;
				m_vServerList.clear();
				m_vServerList = dlgServer.m_vServerList;
				nRetCode = CGMOdbcManager::GetInstance()->OpenGameDB(m_vServerList[m_nSelectedServer].strOdbcName.GetString(),
																     m_vServerList[m_nSelectedServer].strOdbcUserID.GetString(),
																     m_vServerList[m_nSelectedServer].strOdbcPassword.GetString());

				if (nRetCode == DB_ERROR)
				{
					// DB 연결 실패
					CString strTemp;
					/*
					strTemp = "Game DB Open Failed/" + m_vServerList[m_nSelectedServer].strOdbcName + "/" +
						      m_vServerList[m_nSelectedServer].strOdbcUserID + "/" +
							  m_vServerList[m_nSelectedServer].strOdbcPassword;
					*/
					strTemp = _T("Game DB Open Failed");
					MessageBox(strTemp, _T("ERROR"), MB_ICONEXCLAMATION);
				}
				else
				{
					m_nSvrGrp = m_vServerList[m_nSelectedServer].nSGNum;
				}
			}
		}
	}
	else
	{
		m_bLogin = FALSE;
	}

	return m_bLogin;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	
	//
	//note: PreCreateWindow() is called twice.
	//
	//if (m_bCfgLoad)
		//return TRUE;

	static BOOL bDone = FALSE;

	if (bDone)
		return TRUE;

	if (CGmCfg::GetInstance()->LoadADOInfo(true) == FALSE)	//ADO
	{
		if (CGmCfg::GetInstance()->LoadODBCInfo() == FALSE) // ODBC - backward compatibility
		{
			//
			// Request to input ODBC information with a dialog.
			//
			CDlgConfig dlgConfig;

			if (dlgConfig.DoModal() == IDOK)
			{
				m_bCfgLoad = TRUE; // cfg 파일 로드 성공

				CGmCfg::GetInstance()->LoadODBCInfo();
			}
			else
			{			
				m_bCfgLoad = FALSE;
			}
		}
		else
		{
			m_bCfgLoad = TRUE; // cfg 파일 로드 성공
		}
	}
	else
	{
		m_bCfgLoad = TRUE; // cfg 파일 로드 성공
	}
	
	if (m_bCfgLoad)
	{
		m_pDbMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
	}

	bDone = m_bCfgLoad;	

	return m_bCfgLoad;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers
void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CFrameWnd::OnClose();
}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// Base Tab
	TVisualObject *pTab1 = new TVisualObject(1,
	                                         _T(""),
										     pContext, 
										     RUNTIME_CLASS(TTabWnd),
										     TVisualObject::TOS_TABTOP);

	// Character Search Tab	
	TVisualObject *pView1 = new TVisualObject(2, 
		                                      _T("Character Search"),  
											  pContext, 
											  RUNTIME_CLASS(CCharSearch));
	pView1->SetIcon(IDR_MAINFRAME);
	pView1->SetDescription(_T("Character Search"));
	pView1->SetHotKey('1');	
	
	// Item Search Tab
	TVisualObject *pView2 = new TVisualObject(3, 
		                                      _T("Item Search"), 
											  pContext, 
											  RUNTIME_CLASS(CItemSearch));	
	pView2->SetIcon(IDR_MAINFRAME);
	pView2->SetDescription(_T("Item Search"));
	pView2->SetHotKey('2');	

	// Item Change Tab
// 	TVisualObject *pView3 = new TVisualObject(4, 
// 		                                      _T("Item Change"), 
// 											  pContext, 
// 											  RUNTIME_CLASS(CItemChange));
// 	pView3->SetIcon(IDR_MAINFRAME);
// 	pView3->SetDescription(_T("Item Change"));
// 	pView3->SetHotKey('3');	

	// Item Statistics Tab
	TVisualObject* pView4 = new TVisualObject(4,
		                                      _T("Item Statistics"),
											  pContext,
											  RUNTIME_CLASS(CItemStatistic));
	pView4->SetIcon(IDR_MAINFRAME);
	pView4->SetDescription(_T("Item Statistics"));
	pView4->SetHotKey('3');	

	// GM Item Search Tab
// 	TVisualObject* pView5 = new TVisualObject(6,
// 		                                      _T("GM Item Search"),
// 											  pContext,
// 											  RUNTIME_CLASS(CGMItemSearch));
// 	pView5->SetIcon(IDR_MAINFRAME);
// 	pView5->SetDescription(_T("GM Item Search"));
// 	pView5->SetHotKey('5');	

	// Money Statistics Tab	
	TVisualObject* pView6 = new TVisualObject(5,
		                                      _T("Money Statistics"),
											  pContext,
											  RUNTIME_CLASS(CMoneyStat));
	pView6->SetIcon(IDR_MAINFRAME);
	pView6->SetDescription(_T("Money Statistics"));
	pView6->SetHotKey('4');	
	
/*
	// Club Search Tab	
	
	TVisualObject *pView7 = new TVisualObject(8, 
		                                      _T("Club Search"),  
											  pContext, 
											  RUNTIME_CLASS(CItemCount));
	pView6->SetIcon(IDR_MAINFRAME);
	pView6->SetDescription(_T("Club Search"));
	pView6->SetHotKey('7');
*/    

	// Item Count Tab
// 	TVisualObject* pView7 = new TVisualObject(
//         8,
//         _T("Item Count"),
//         pContext,
//         RUNTIME_CLASS(CItemCount));
// 	pView7->SetIcon(IDR_MAINFRAME);
// 	pView7->SetDescription(_T("Item Count"));
// 	pView7->SetHotKey('7');	

    // Skill search Tab
    TVisualObject* pView8 = new TVisualObject(
        6,
        _T("Skill Search"),
        pContext,
        RUNTIME_CLASS(DlgSkillSearch));
    pView8->SetIcon(IDR_MAINFRAME);
    pView8->SetDescription(_T("Skill Search"));
    pView8->SetHotKey('5');	

    // Search Duplication Items.
//     TVisualObject *pView9 = new TVisualObject(10,
//         _T("Search Duplication Items"),
//         pContext,
//         RUNTIME_CLASS(CSearchDuplicationItems));
// 
//     pView9->SetIcon(IDR_MAINFRAME);
//     pView9->SetDescription(_T("Search Duplication Items"));
//     pView9->SetHotKey('9');

	TVisualObject *pView10 = new TVisualObject( 7,
		_T("Item Transfer"),
		pContext,
		RUNTIME_CLASS( CItemTransfer ) );

	pView10->SetIcon( IDR_MAINFRAME );
	pView10->SetDescription( _T( "Item Transfer" ) );
	pView10->SetHotKey( '6' );

    //

	// Add All Tab
	m_Framework.Add(pTab1);
	m_Framework.Add(pTab1, pView1);
	m_Framework.Add(pTab1, pView2);
	//m_Framework.Add(pTab1, pView3);
	m_Framework.Add(pTab1, pView4);
	//m_Framework.Add(pTab1, pView5);
	m_Framework.Add(pTab1, pView6);
	//m_Framework.Add(pTab1, pView7);
    m_Framework.Add(pTab1, pView8);
    //m_Framework.Add(pTab1, pView9);
	m_Framework.Add( pTab1, pView10 );

	// Fram Create	
	m_Framework.Create(this);
	TVisualFrameworkIterator it(m_Framework);
	while (!it.End())
	{
		TVisualObject *pObject = it.Get();
		it++;
	}
	return TRUE;
	// return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
    

	SetWindowText(m_csCaption.GetBuffer());

	//CFrameWnd::OnUpdateFrameTitle(bAddToTitle);
}

void CMainFrame::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	GLogicData::GetInstance().ClearData();

	CFrameWnd::PostNcDestroy();
}
