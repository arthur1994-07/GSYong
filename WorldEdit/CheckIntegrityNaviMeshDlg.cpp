// CheckIntegrityNaviMeshDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../SigmaCore/File/SerialFile.h"

#include "WorldEdit.h"
#include "CheckIntegrityNaviMeshDlg.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/NaviMesh/NavigationMesh.h"

#include "../EngineLib/Common/SUBPATH.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCheckIntegrityNaviMeshDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCheckIntegrityNaviMeshDlg, CDialog)
CCheckIntegrityNaviMeshDlg::CCheckIntegrityNaviMeshDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckIntegrityNaviMeshDlg::IDD, pParent)
	, m_StaticCount(_T(""))
	, m_StaticIslandCount(_T(""))
{
	m_pCellTable = NULL;
	m_ppIslandID = NULL;
	m_pSameIslandCount = NULL;
	m_nIslandCount = 0;
}

CCheckIntegrityNaviMeshDlg::~CCheckIntegrityNaviMeshDlg()
{
	SAFE_DELETE_ARRAY ( m_pCellTable );

	for ( DWORD i = 0; i < m_nIslandCount; i++ )
	{
		SAFE_DELETE_ARRAY ( m_ppIslandID[i] );
	}
	SAFE_DELETE_ARRAY ( m_ppIslandID );
	SAFE_DELETE_ARRAY ( m_pSameIslandCount );
	m_nIslandCount = 0;
}

void CCheckIntegrityNaviMeshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHECKINTEGRITYLOG, m_ctrlCheckListBoxLog);
	DDX_Control(pDX, IDOK, m_ctrlButtonOK);
	DDX_Control(pDX, IDIGNORE, m_ctrlButtonIGNORE);
	DDX_Text(pDX, IDC_STATIC_CHECKINTEGRITYCOUNT, m_StaticCount);
	DDX_Text(pDX, IDC_STATIC_CHECKINTEGRITY_ISLAND_COUNT, m_StaticIslandCount);
	DDX_Control(pDX, IDC_LIST_CHECKINTEGRITYLOG2, m_ctrlCheckListBoxLog2);
}


BEGIN_MESSAGE_MAP(CCheckIntegrityNaviMeshDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST_CHECKINTEGRITYLOG, OnLbnDblclkListCheckintegritylog)
	ON_LBN_DBLCLK(IDC_LIST_CHECKINTEGRITYLOG2, OnLbnDblclkListCheckintegritylog2)
	ON_BN_CLICKED(IDIGNORE, OnBnClickedIgnore)
END_MESSAGE_MAP()


// CCheckIntegrityNaviMeshDlg 메시지 처리기입니다.

BOOL CCheckIntegrityNaviMeshDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
	DxLandMan* pDxLandMan = pView->GetLandMan();

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	TCHAR szDebugFile[MAX_PATH]={0};
	SHGetSpecialFolderPath( NULL, szDebugFile, CSIDL_PERSONAL, FALSE );
	StringCchCat( szDebugFile, MAX_PATH, pPath->DebugInfoRoot().c_str()); //SUBPATH::DEBUGINFO_ROOT );
	StringCchCat( szDebugFile, MAX_PATH, _T("CellLinkIntegrity.bin") );

	if ( !pDxLandMan->IsNaviMeshIntegrity() )
	{
		sc::SerialFile SFile;
		if ( SFile.OpenFile ( FOT_READ, szDebugFile ) )
		{
			SNaviIntegrityLog	IntegrityLog;
			DWORD	ErrorCount = 0;
			SFile >> ErrorCount;
            
			m_StaticCount.Format ( "%d", ErrorCount );
			m_pCellTable = new DWORD [ ErrorCount ][10];

			DWORD i;
			for ( i = 0; i < ErrorCount; i++ )
			{
				SFile.ReadBuffer ( &IntegrityLog, sizeof ( SNaviIntegrityLog ) );
				memcpy ( m_pCellTable[i], IntegrityLog.CellID, sizeof ( DWORD ) * 10 );
				m_ctrlCheckListBoxLog.AddString ( IntegrityLog.StrLog );
			}

			DWORD	GroupCount = 0;
			SFile >> GroupCount;

			if ( 1 < GroupCount )
			{   			
				CString	str;
				str.Format ( "%d", GroupCount );
				m_StaticIslandCount = str;

				const	DWORD	NOTUSED = -1;
				boost::shared_ptr<NavigationMesh>	spNaviMesh = pDxLandMan->GetNaviMesh();
				const	DWORD	nCellCount = spNaviMesh->TotalCells();			      
				DWORD	*pGroup = new DWORD [ nCellCount ];
				memset ( pGroup, NOTUSED, sizeof ( DWORD ) * nCellCount );
				SFile.ReadBuffer ( pGroup, sizeof ( DWORD ) * nCellCount );

				m_nIslandCount = GroupCount;
				if ( m_nIslandCount )
				{
					m_pSameIslandCount = new DWORD [ m_nIslandCount ];
					m_ppIslandID = new DWORD* [ m_nIslandCount ];

					memset ( m_pSameIslandCount, 0, sizeof ( DWORD ) * m_nIslandCount );
				}

				for ( i = 0; i < m_nIslandCount; i++ )
				{				
					for ( DWORD j = 0; j < nCellCount; j++ )
					{
						if ( i == pGroup[j] )
						{
							m_pSameIslandCount[i]++;
						}
					}
				}

				for ( i = 0; i < m_nIslandCount; i++ )
				{
					m_ppIslandID[i] = new DWORD [ m_pSameIslandCount[i] ];
				}

				DWORD	CellCount = 0;
				for ( i = 0; i < m_nIslandCount; i++ )
				{
					CellCount = 0;
					for ( DWORD j = 0; j < nCellCount; j++ )
					{
						if ( i == pGroup[j] )
						{
							m_ppIslandID[i][CellCount] = j;
							CellCount++;
						}
					}
					CString	str;
					str.Format ( "[%d] 섬이 발견되었습니다.", i );
					m_ctrlCheckListBoxLog2.AddString ( str );     
				}

				SAFE_DELETE_ARRAY ( pGroup );
			}
			else
			{
				m_StaticIslandCount.Format ( "%d", 0 );
			}

	        SFile.CloseFile ();			
		}
	}
	else
	{
		m_StaticCount.Format ( "%d", 0 );
		m_StaticIslandCount.Format ( "%d", 0 );
	}
	
	UpdateData ( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CCheckIntegrityNaviMeshDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditDoc* pDoc = (CWorldEditDoc *) pFrame->GetActiveDocument ();
        
	pDoc->m_pCheckIntegrityDlg->DestroyWindow();
	delete pDoc->m_pCheckIntegrityDlg;
	pDoc->m_pCheckIntegrityDlg = NULL;
//	OnOK();
}

void CCheckIntegrityNaviMeshDlg::OnLbnDblclkListCheckintegritylog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxLandMan	*pDxLandMan = pView->GetLandMan ();
	boost::shared_ptr<NavigationMesh> spNaviMesh = pDxLandMan->GetNaviMesh();

	int Item = m_ctrlCheckListBoxLog.GetCurSel ();

	int Count = 0;
	while ( m_pCellTable[Item][Count] != -1 )
	{
		Count++;
	}

	if ( Item != -1 )
	{
		spNaviMesh->GotoErrorPosition ( m_pCellTable[Item], Count );
	}
}

void CCheckIntegrityNaviMeshDlg::OnLbnDblclkListCheckintegritylog2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxLandMan	*pDxLandMan = pView->GetLandMan ();
	boost::shared_ptr<NavigationMesh> spNaviMesh = pDxLandMan->GetNaviMesh();

	int Item = m_ctrlCheckListBoxLog2.GetCurSel ();

	if ( Item != -1 )
	{
		spNaviMesh->GotoErrorPosition ( m_ppIslandID[Item], m_pSameIslandCount[Item] );
	}
}

void CCheckIntegrityNaviMeshDlg::OnBnClickedIgnore()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	if ( MessageBox ( "본 과정을 무시할 경우, 길찾기에 심각한 오류가 발생합니다.\n그래도 진행하신다면 확인을 눌러주십시오.", "질문", MB_ICONQUESTION | MB_YESNO ) == IDYES )
//	{
		CCheckIntegrityNaviMeshDlg::OnBnClickedOk();
//	}
}
