// ItemDataMergerToolDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include <vector>
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/Common/SUBPATH.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/Item/GLItem.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/GLStringTable.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicServer/DxServerInstance.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "ItemDataMergerTool.h"
#include "Checkdlg.h"
#include "ItemDataMergerToolDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CItemDataMergerToolDlg 대화 상자

CItemDataMergerToolDlg::CItemDataMergerToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CItemDataMergerToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    GLogicData::GetInstance().LoadCountryFile();
    m_ServiceProvider = GLogicData::GetInstance().GetServiceProvider();
    m_pPath = new SUBPATH(m_ServiceProvider);
    m_pEngineDevice = new DxResponseMan(m_ServiceProvider, m_pPath);
}

CItemDataMergerToolDlg::~CItemDataMergerToolDlg()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;

    delete m_pPath;
    m_pPath = NULL;
}

void CItemDataMergerToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM1, m_ctrlItemTree1);
	DDX_Control(pDX, IDC_TREE_ITEM2, m_ctrlItemTree2);
}

BEGIN_MESSAGE_MAP(CItemDataMergerToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOAD1, OnBnClickedButtonLoad1)
	ON_BN_CLICKED(IDC_BUTTON_LOAD2, OnBnClickedButtonLoad2)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_MERGER, OnBnClickedButtonMerger)
	ON_BN_CLICKED(IDC_BUTTON_MERGER2, OnBnClickedButtonMerger2)
	ON_BN_CLICKED(IDC_BTN_CSV_SAVE1, OnBnClickedBtnCsvSave1)
	ON_BN_CLICKED(IDC_BTN_CSV_LOAD1, OnBnClickedBtnCsvLoad1)
	ON_BN_CLICKED(IDC_BTN_CSV_SAVE2, OnBnClickedBtnCsvSave2)
	ON_BN_CLICKED(IDC_BTN_CSV_LOAD2, OnBnClickedBtnCsvLoad2)
	ON_BN_CLICKED(IDC_BUTTON_SAVE1, OnBnClickedButtonSave1)
	ON_BN_CLICKED(IDC_BUTTON_SAVE2, OnBnClickedButtonSave2)
	ON_BN_CLICKED(IDC_BUTTON_NEW1, OnBnClickedButtonNew1)
	ON_BN_CLICKED(IDC_BUTTON_NEW2, OnBnClickedButtonNew2)
END_MESSAGE_MAP()


// CItemDataMergerToolDlg 메시지 처리기

BOOL CItemDataMergerToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
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

	m_pEngineDevice->OneTimeSceneInit(
        m_szAppPath,
        this,
        RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont );

    m_pItemManData1 = new GLItemMan;
	m_pItemManData2 = new GLItemMan;
	m_pItemManData1->OneTimeSceneInit();
	m_pItemManData2->OneTimeSceneInit();

	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CItemDataMergerToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CItemDataMergerToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CItemDataMergerToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

const char* CItemDataMergerToolDlg::GetFormatName ( SITEM* pItem )
{
	GASSERT ( pItem );

	//** !!Add New Sex!!     
	static	CString strItem;
	if ( pItem->sBasicOp.dwReqCharClass==GLCC_NONE)
	{
		strItem.Format ( "[%04d/%02d]  %s  [{%s}]",
			pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID, pItem->GetName(), "Unusable" );
	}
	else if ( pItem->sBasicOp.dwReqCharClass==GLCC_ALL_ACTOR)
	{
		strItem.Format ( "[%04d/%02d]  %s  [{%s}]",
			pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID, pItem->GetName(), "Common" );
	}
	else
	{
		strItem.Format ( "[%04d/%02d]  %s  [",
			pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID, pItem->GetName() );

		if ( pItem->sBasicOp.dwReqCharClass&GLCC_FIGHTER_M )		strItem += "FighterM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_FIGHTER_W )	strItem += "FighterW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ARMS_M )			strItem += "ArmsM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ARMS_W )		strItem += "ArmsW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ARCHER_M )		strItem += "ArcherM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ARCHER_W )		strItem += "ArcherW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_SPIRIT_M )		strItem += "SpiritM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_SPIRIT_W )		strItem += "SpiritW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_EXTREME_M )	strItem += "ExtremeM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_EXTREME_W )	strItem += "ExtremeW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_SCIENTIST_M )	strItem += "ScientistM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_SCIENTIST_W )	strItem += "ScientistW|";
        if ( pItem->sBasicOp.dwReqCharClass&GLCC_ASSASSIN_M )	strItem += "AssassinM|";
        if ( pItem->sBasicOp.dwReqCharClass&GLCC_ASSASSIN_W )	strItem += "AssassinW|";
        if ( pItem->sBasicOp.dwReqCharClass&GLCC_TRICKER_M )    	strItem += "TrickerM|";
        if ( pItem->sBasicOp.dwReqCharClass&GLCC_TRICKER_W )        strItem += "TrickerW";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ACTOR_M )    	strItem += "ActorM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ACTOR_W )        strItem += "ActorW";

		strItem += "]";
	}

	CString strNewName;
	strNewName.Format( "IN_%03d_%03d", pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID );
	pItem->sBasicOp.strName = strNewName;
	strNewName.Format( "ID_%03d_%03d", pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID );
	pItem->sBasicOp.strComment = strNewName;

	return strItem.GetString ();
}


BOOL CItemDataMergerToolDlg::UpdateTree( BOOL bType1 )
{	
	CleanAllItem ( bType1 );

	CTreeCtrl *treeCtrl = NULL;
	HTREEITEM *TreeRoot;
	GLItemMan *ItemManData;
	if( bType1 )
	{
		treeCtrl = &m_ctrlItemTree1;
		TreeRoot = &m_TreeRoot1;
		ItemManData = m_pItemManData1;
	}else{
		treeCtrl = &m_ctrlItemTree2;
		TreeRoot = &m_TreeRoot2;
		ItemManData = m_pItemManData2;
	}


	for ( int MID = 0; MID < GLItemMan::MAX_MID; MID++ )
	{
		HTREEITEM hMainItem= NULL;
		HTREEITEM hSubItem = NULL;

		for ( int SID = 0; SID < GLItemMan::MAX_SID; SID++ )
		{        		
			SITEM* pItem = ItemManData->GetItem ( MID, SID );

			if ( pItem )
			{
				CString strItem = GetFormatName ( pItem );
				if ( !hMainItem )
				{
					hMainItem = treeCtrl->InsertItem ( strItem, *TreeRoot );
					treeCtrl->SetItemData ( hMainItem, (DWORD_PTR)pItem );

				//	TreeItemList[MID][SID] = hMainItem;
				}
				else
				{
					hSubItem = treeCtrl->InsertItem ( strItem, hMainItem );
					treeCtrl->SetItemData ( hSubItem, (DWORD_PTR)pItem );

				//	TreeItemList[MID][SID] = hSubItem;
				}				
			}
		}
	}

	return TRUE;
}

void CItemDataMergerToolDlg::CleanAllItem( BOOL bType1 )
{

	if( bType1 )
	{
		m_ctrlItemTree1.DeleteAllItems ();
		m_TreeRoot1 = m_ctrlItemTree1.InsertItem ( "Root Node" );
	}else{
		m_ctrlItemTree2.DeleteAllItems ();
		m_TreeRoot2 = m_ctrlItemTree2.InsertItem ( "Root Node" );
	}
}


void CItemDataMergerToolDlg::OnBnClickedButtonLoad1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemDataMergerToolDlg*)this);


	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{

		//m_ItemManData1.FinalCleanup ();
		/*m_ItemManData1.OneTimeSceneInit();*/

		HRESULT hr;
		hr = m_pItemManData1->LoadFile(dlg.GetFileName().GetString());

		// 아이템 스트링테이블을 로드한다. by 경대
		GLStringTable::GetInstance().CLEAR();
		if( !GLStringTable::GetInstance().LOADFILE( m_pItemManData1->_STRINGTABLE, GLStringTable::ITEM ) )
		{
			MessageBox ( "Failure to load file." );
		}


		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to load file." );
		}

	}

	UpdateTree( TRUE );
}

void CItemDataMergerToolDlg::OnBnClickedButtonLoad2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemDataMergerToolDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		/*m_ItemManData2.FinalCleanup ();
		m_ItemManData2.OneTimeSceneInit();*/

		HRESULT hr;
		hr = m_pItemManData2->LoadFile(dlg.GetFileName().GetString());

		// 아이템 스트링테이블을 로드한다. by 경대
		GLStringTable::GetInstance().CLEAR();
		if( !GLStringTable::GetInstance().LOADFILE( m_pItemManData2->_STRINGTABLE, GLStringTable::ITEM ) )
		{
			MessageBox ( "Failure to load file." );
		}


		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to load file." );
		}

	}

	UpdateTree( FALSE );
}

void CItemDataMergerToolDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pItemManData1->FinalCleanup ();
	m_pItemManData2->FinalCleanup ();
	SAFE_DELETE( m_pItemManData1 );
	SAFE_DELETE( m_pItemManData2 );

	m_pEngineDevice->FinalCleanup ();


}

void CItemDataMergerToolDlg::OnBnClickedButtonMerger()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_vecCheckItemList.clear();

	SearchCheckItem( &m_ctrlItemTree1, m_TreeRoot1 );

	if( m_pItemManData1 == NULL || m_pItemManData2 == NULL )
	{
		MessageBox( "아이템 데이터를 로드하세요", "알림" );
		return;
	}

	if( m_vecCheckItemList.size() == 0 )
	{
		MessageBox( "추가할 아이템 데이터를 선택하세요", "알림" );
		return;
	}

	CCheckDlg	CheckDlg(m_vecCheckItemList, m_pItemManData1, m_pItemManData2 );
	if( CheckDlg.DoModal() == IDOK )
	{
		SITEM *pBaseItem = NULL;
		SITEM TargetItem;
		for( int i = 0; i < CheckDlg.m_vecIDList.size(); i++ )
		{
			SIDLIST idList = CheckDlg.m_vecIDList[i];
			pBaseItem	= m_pItemManData1->GetItem(idList.sCheckID);
			if( pBaseItem == NULL ) continue;

			TargetItem = *pBaseItem;

			TargetItem.sBasicOp.sNativeID = idList.sApplyID;
			GetFormatName(&TargetItem);
			m_pItemManData2->InsertItem( idList.sApplyID, (PITEMNODE)&TargetItem, TRUE );
		}

		UpdateTree( FALSE );
	}
}


void CItemDataMergerToolDlg::OnBnClickedButtonMerger2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_vecCheckItemList.clear();

	SearchCheckItem( &m_ctrlItemTree2, m_TreeRoot2 );

	if( m_pItemManData1 == NULL || m_pItemManData2 == NULL )
	{
		MessageBox( "아이템 데이터를 로드하세요", "알림" );
		return;
	}

	if( m_vecCheckItemList.size() == 0 )
	{
		MessageBox( "추가할 아이템 데이터를 선택하세요", "알림" );
		return;
	}

	CCheckDlg	CheckDlg(m_vecCheckItemList, m_pItemManData2, m_pItemManData1 );
	if( CheckDlg.DoModal() == IDOK )
	{
		SITEM *pBaseItem = NULL;
		SITEM TargetItem;
		for( int i = 0; i < CheckDlg.m_vecIDList.size(); i++ )
		{
			SIDLIST idList = CheckDlg.m_vecIDList[i];
			pBaseItem	= m_pItemManData2->GetItem(idList.sCheckID);
			if( pBaseItem == NULL ) continue;

			TargetItem = *pBaseItem;

			TargetItem.sBasicOp.sNativeID = idList.sApplyID;
			GetFormatName(&TargetItem);
			m_pItemManData1->InsertItem( idList.sApplyID, (PITEMNODE)&TargetItem, TRUE );
		}

		UpdateTree( TRUE );
	}
}

void CItemDataMergerToolDlg::SearchCheckItem( CTreeCtrl *pTreeCtrl, HTREEITEM hRootItem )
{
	HTREEITEM hChild = pTreeCtrl->GetChildItem(hRootItem);
	while( hChild )
	{
		if( pTreeCtrl->GetCheck(hChild) )
		{
			PITEMNODE pItemNode = (PITEMNODE)pTreeCtrl->GetItemData(hChild);
			m_vecCheckItemList.push_back(pItemNode->m_sItem.sBasicOp.sNativeID.dwID);
		}

		if( pTreeCtrl->GetChildItem(hRootItem) ) 
			SearchCheckItem( pTreeCtrl, hChild );

		hChild = pTreeCtrl->GetNextItem(hChild, TVGN_NEXT);
	}
}


void CItemDataMergerToolDlg::OnBnClickedBtnCsvSave1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_pItemManData1->SaveCsvFile(this) == S_OK )
	{
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
	}
}

void CItemDataMergerToolDlg::OnBnClickedBtnCsvLoad1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_pItemManData1->LoadCsvFile(this) == S_OK )
	{
		UpdateTree(TRUE);
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CItemDataMergerToolDlg::OnBnClickedBtnCsvSave2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_pItemManData2->SaveCsvFile(this) == S_OK )
	{
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
	}
}

void CItemDataMergerToolDlg::OnBnClickedBtnCsvLoad2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_pItemManData2->LoadCsvFile(this) == S_OK )
	{
		UpdateTree(FALSE);
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CItemDataMergerToolDlg::OnBnClickedButtonSave1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemDataMergerToolDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = m_pItemManData1->SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File" );
			return;
		}		

		return;
	}
}

void CItemDataMergerToolDlg::OnBnClickedButtonSave2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemDataMergerToolDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = m_pItemManData2->SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File" );
			return;
		}
	

		return;
	}
}

void CItemDataMergerToolDlg::OnBnClickedButtonNew1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pItemManData1->FinalCleanup();
	m_pItemManData1->OneTimeSceneInit();

	UpdateTree( TRUE );
}

void CItemDataMergerToolDlg::OnBnClickedButtonNew2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pItemManData2->FinalCleanup();
	m_pItemManData2->OneTimeSceneInit();

	UpdateTree( FALSE );
}
