// ItemDataMergerToolDlg.cpp : ���� ����
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

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
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

// CItemDataMergerToolDlg ��ȭ ����

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


// CItemDataMergerToolDlg �޽��� ó����

BOOL CItemDataMergerToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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

	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CItemDataMergerToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemDataMergerToolDlg*)this);


	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{

		//m_ItemManData1.FinalCleanup ();
		/*m_ItemManData1.OneTimeSceneInit();*/

		HRESULT hr;
		hr = m_pItemManData1->LoadFile(dlg.GetFileName().GetString());

		// ������ ��Ʈ�����̺��� �ε��Ѵ�. by ���
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemDataMergerToolDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		/*m_ItemManData2.FinalCleanup ();
		m_ItemManData2.OneTimeSceneInit();*/

		HRESULT hr;
		hr = m_pItemManData2->LoadFile(dlg.GetFileName().GetString());

		// ������ ��Ʈ�����̺��� �ε��Ѵ�. by ���
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_pItemManData1->FinalCleanup ();
	m_pItemManData2->FinalCleanup ();
	SAFE_DELETE( m_pItemManData1 );
	SAFE_DELETE( m_pItemManData2 );

	m_pEngineDevice->FinalCleanup ();


}

void CItemDataMergerToolDlg::OnBnClickedButtonMerger()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_vecCheckItemList.clear();

	SearchCheckItem( &m_ctrlItemTree1, m_TreeRoot1 );

	if( m_pItemManData1 == NULL || m_pItemManData2 == NULL )
	{
		MessageBox( "������ �����͸� �ε��ϼ���", "�˸�" );
		return;
	}

	if( m_vecCheckItemList.size() == 0 )
	{
		MessageBox( "�߰��� ������ �����͸� �����ϼ���", "�˸�" );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_vecCheckItemList.clear();

	SearchCheckItem( &m_ctrlItemTree2, m_TreeRoot2 );

	if( m_pItemManData1 == NULL || m_pItemManData2 == NULL )
	{
		MessageBox( "������ �����͸� �ε��ϼ���", "�˸�" );
		return;
	}

	if( m_vecCheckItemList.size() == 0 )
	{
		MessageBox( "�߰��� ������ �����͸� �����ϼ���", "�˸�" );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_pItemManData1->SaveCsvFile(this) == S_OK )
	{
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
	}
}

void CItemDataMergerToolDlg::OnBnClickedBtnCsvLoad1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_pItemManData1->LoadCsvFile(this) == S_OK )
	{
		UpdateTree(TRUE);
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CItemDataMergerToolDlg::OnBnClickedBtnCsvSave2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_pItemManData2->SaveCsvFile(this) == S_OK )
	{
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
	}
}

void CItemDataMergerToolDlg::OnBnClickedBtnCsvLoad2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_pItemManData2->LoadCsvFile(this) == S_OK )
	{
		UpdateTree(FALSE);
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CItemDataMergerToolDlg::OnBnClickedButtonSave1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";

	//	Note : ���� ���� ���̾˷α׸� ����.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";

	//	Note : ���� ���� ���̾˷α׸� ����.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pItemManData1->FinalCleanup();
	m_pItemManData1->OneTimeSceneInit();

	UpdateTree( TRUE );
}

void CItemDataMergerToolDlg::OnBnClickedButtonNew2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pItemManData2->FinalCleanup();
	m_pItemManData2->OneTimeSceneInit();

	UpdateTree( FALSE );
}
