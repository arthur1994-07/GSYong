// CItemSearch.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <process.h>
#include "../GMCharEdit.h"
#include "CItemSearch.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"
#include "../SubDialogs/DlgInvenAdd.h"

#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Util/CGmCfg.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../Util/CConsoleMessage.h"
#include "../Util/CLogFile.h"
#include "../../RanLogicServer/DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static unsigned int WINAPI ItemSearchThread(void* p)
{
	if (p == NULL) 
	{
		return 0;
	}
	else
	{
		CItemSearch* pDlg = (CItemSearch*) p;
		pDlg->SearchProc();
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// ����� �������� �˻��ϱ� ���ؼ� �������
static unsigned int WINAPI ItemSpecialSearchThread(void* p)
{
	if (p == NULL)
	{
		return 0;
	}
	else
	{
		CItemSearch* pThread = (CItemSearch*) p;
		pThread->SpecialSearchProc();
		return 0;
	}
}

// CItemSearch

IMPLEMENT_DYNCREATE(CItemSearch, CFormView)

CItemSearch::CItemSearch()
	: CFormView(CItemSearch::IDD)
	, m_nItemMain(0)
	, m_nItemSub(0)
	, m_nCheck(0)
	, m_bRunning(FALSE)
	, m_hThread(NULL)	
	, m_bInit(FALSE)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CItemSearch::~CItemSearch()
{
	if (m_bRunning == TRUE) // �۾���
	{
		m_bRunning = FALSE; // �۾��� �ߴܽ�Ų��.
		Sleep( 1000 ); // 1 �ʴ��
	}
}

void CItemSearch::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCharSearch);
	DDX_Text(pDX, IDC_EDIT_ITEM_MAIN, m_nItemMain);
	DDX_Text(pDX, IDC_EDIT_ITEM_SUB, m_nItemSub);
	DDX_Text(pDX, IDC_EDIT_ITEM_MAKENUM, m_guid);
	DDX_Check(pDX, IDC_CHECK, m_nCheck);
	DDX_Control(pDX, IDC_SEARCH_PROGRESS1, m_SearchProgress);
	DDX_Control(pDX, IDC_EDIT_ITEM_SEARCH_CONSOLE, m_EditSearchConsole);
	DDX_Control(pDX, IDC_EDIT_ITEM_COUNT, m_nItemCount);
}

BEGIN_MESSAGE_MAP(CItemSearch, CFormView)
	ON_BN_CLICKED(IDC_BTN_SELECT_ITEM, OnBnClickedBtnSelectItem)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BTN_ITEM_SEARCH_START, OnBnClickedBtnItemSearchStart)	
	ON_EN_MAXTEXT(IDC_EDIT_ITEM_SEARCH_CONSOLE, OnEnMaxtextEditItemSearchConsole)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_ITEM_SEARCH_STOP, OnBnClickedBtnItemSearchStop)
	ON_BN_CLICKED(IDC_BTN_ITEM_SPECIAL_SEARCH_START, OnBnClickedBtnItemSpecialSearchStart)
	ON_WM_CLOSE()			
	ON_BN_CLICKED(IDC_CHECK, OnBnClickedCheck)
END_MESSAGE_MAP()


// CItemSearch �����Դϴ�.

#ifdef _DEBUG
void CItemSearch::AssertValid() const
{
	CFormView::AssertValid();
}

void CItemSearch::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CItemSearch �޽��� ó�����Դϴ�.

void CItemSearch::OnBnClickedBtnSelectItem()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDlgInvenAdd dlgAdd;

	if (dlgAdd.DoModal() == IDOK)
    {
        if (dlgAdd.m_nSelected != -1)
        {
            SITEMCUSTOM sItem(dlgAdd.m_vItem[dlgAdd.m_nSelected].sBasicOp.GetNativeID());
            m_nItemMain     = sItem.Mid();
			m_nItemSub      = sItem.Sid();
			UpdateData(FALSE);
        }
    }	
}

void CItemSearch::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.		
	m_ListCharSearch.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	InitListHead();

	m_bInit = TRUE;
}

void CItemSearch::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (m_bRunning == TRUE) // �۾���
	{
		m_bRunning = FALSE; // �۾��� �ߴܽ�Ų��.
		Sleep( 1000 ); // 1 �ʴ��
	}
	CFormView::OnClose();
}

// ������ �˻� ����
void CItemSearch::OnBnClickedBtnItemSearchStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bRunning == TRUE) // �۾���
	{
		MessageBox(_T("Already Search is running, First Stop Search Process"));
		return;
	}
	else // �۾�����
	{
		UpdateData(TRUE);
		ItemSearchStart();	
	}	
}

// Ư�� ������ �˻� ����
void CItemSearch::OnBnClickedBtnItemSpecialSearchStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bRunning == TRUE) // �۾���
	{
		MessageBox(_T("Already Search is running, First Stop Search Process"));
		return;
	}
	else // �۾�����
	{
		UpdateData(TRUE);
		SpecialItemSearchStart();
	}
}

// ������ �˻� �ߴ�
void CItemSearch::OnBnClickedBtnItemSearchStop()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bRunning == TRUE) // �۾���
	{
		m_bRunning = FALSE; // �۾��ߴ�				
	}
	else // �̹� �۾��ߴ���...
	{
	}	
}

// Edit �ڽ��� �ִ� ���ڼ��� ����������...
void CItemSearch::OnEnMaxtextEditItemSearchConsole()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GetDlgItem(IDC_EDIT_ITEM_SEARCH_CONSOLE)->SetWindowText(_T(""));	

}

void CItemSearch::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelected = m_ListCharSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� �׸��� ����
	{	
		if (m_bRunning == TRUE) // �۾���...
		{
			MessageBox(_T("First Stop Search Process!"));
		}
		else
		{
			CString strPosCode = m_ListCharSearch.GetItemText( nSelected, 4 );
			int PosCode = atoi( strPosCode );
			switch ( PosCode )
			{
			case INVEN_DELETED:
				{
// 					CConsoleMessage::GetInstance()->Write( "This item is INVEN_DELETED. Can't open. guid: %s", ( m_ListCharSearch.GetItemText( nSelected, 1 ) ).GetString() );
// 				
// 					CDlgInvenElementEdit dlgEdit;
// 					dlgEdit.m_sItem = m_vInvenItem[nSelected];
// 
// 					if (dlgEdit.DoModal() == IDOK)				
				}
				break;
			case INVEN_PUTON:
			case INVEN_INVEN:
			case INVEN_QUEST_PROC:
			case INVEN_QUEST_END:
				{
					CString strChaNum = m_ListCharSearch.GetItemText( nSelected, 0 );
					CString strUserNum = m_ListCharSearch.GetItemText( nSelected, 5 );
					CPropertySheetCharEdit dlg( atoi( strChaNum ), atoi( strUserNum ), _T( "Character Edit" ) );
					dlg.DoModal();
				}
				break;
			case INVEN_POST:
				{
				}
				break;
			case INVEN_LOCKER:
				{
					CString strUserNum = m_ListCharSearch.GetItemText( nSelected, 0 );
					CString strChaNum = m_ListCharSearch.GetItemText( nSelected, 5 );
					CPropertySheetCharEdit dlg( atoi( strChaNum ), atoi( strUserNum ), _T( "Character Edit" ) );
					dlg.DoModal();
				}
				break;
			case INVEN_CLUB_LOCKER:
				{
				}
				break;
			case INVEN_VEHICLE:
				{
				}
				break;
			case INVEN_PET:
				{
				}
				break;
			case INVEN_DELETED2:
				{
					CConsoleMessage::GetInstance()->Write( "This item is INVEN_DELETED2. Can't open. guid: %s", ( m_ListCharSearch.GetItemText( nSelected, 1 ) ).GetString() );
				}
				break;
			default:
				{
				}
				break;
			}
		}
	}
	*pResult = 0;
}

void CItemSearch::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_bInit)
	{
		int nWidth  = cx - 5;
		int nHeight = cy - 320;

		if (nWidth < 0)  cx = 0;
		if (nHeight < 0) cy = 0;
		
		m_EditSearchConsole.MoveWindow(0,
		                               100,
									   nWidth,
									   200);

		m_ListCharSearch.MoveWindow(0, // rect.left,
								    310, // rect.top,
								    nWidth,
								    nHeight);
	}
}

void CItemSearch::InitListHead()
{
	LVCOLUMN Col;
	
	// Num, Name, Class, Tribe, Level,	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= _T("Owner");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCharSearch.InsertColumn(0, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 300;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Guid");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCharSearch.InsertColumn(1, &Col);
    
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("TurnNum");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCharSearch.InsertColumn(2, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Pos");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCharSearch.InsertColumn(3, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("PosCode");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCharSearch.InsertColumn(4, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("OwnerCode");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCharSearch.InsertColumn(5, &Col);
}


void CItemSearch::ItemSearchStart()
{
	DWORD dwThreadID = 101;	
	
	/*
	m_hThread = ::CreateThread(NULL, 
					           0,
					           (LPTHREAD_START_ROUTINE) ItemSearchThread,
					           this,
					           0,
					           &dwThreadID);
	*/
	m_hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0,
								ItemSearchThread,
								this,
								0,
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// ������ ������ �����Ͽ����ϴ�.
        MessageBox(_T("CreateThread Search Failed"));
	}
	else
	{
		///////////////////////////////////////////////////////////////////////////
		// GM Log	
		CString strUserIP = m_pDBMan->GetLocalIP();
		CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
		int nGmNum = pFrame->m_nUserNum;
		std::strstream strGmCmd;
		strGmCmd << "Item Search " << m_nItemMain << " " << m_nItemSub;
		strGmCmd << '\0';
		m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
		strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		///////////////////////////////////////////////////////////////////////////	
	}
}

void CItemSearch::SpecialItemSearchStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	DWORD dwThreadID = 102;	
	
	/*
	m_hThread = ::CreateThread(NULL, 
					           0,
					           (LPTHREAD_START_ROUTINE) ItemSpecialSearchThread,
					           this,
					           0,
					           &dwThreadID);
	*/
	m_hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0,
								ItemSpecialSearchThread,
								this,
								0,
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// ������ ������ �����Ͽ����ϴ�.
        MessageBox(_T("CreateThread Failed"));
	}
}

void CItemSearch::SearchProc()
{
	int nTotalSize = 0;
	int nCurrent   = 0;
	int nCount     = 0;
	nFindCount = 0;
	int nPercent   = 0;	
	
	m_bRunning = TRUE;
	CString strTemp;
	LV_ITEM lvItem;	

	CConsoleMessage::GetInstance()->SetControl( GetDlgItem( IDC_EDIT_ITEM_SEARCH_CONSOLE )->m_hWnd );

	// ������ �˻��� ���۵Ǹ� MakeNum ��뿩�� üũ �ڽ��� ��Ȱ��ȭ�� �����.
	GetDlgItem( IDC_CHECK )->EnableWindow( FALSE );

	// Set Range Of progress bar
	m_SearchProgress.SetRange( 0, 100 );
	
	m_ListCharSearch.DeleteAllItems();

	CConsoleMessage::GetInstance()->Write( "---- Search Start ----" );

    // ��ü ĳ���� ��ȣ/������ �����´�.
	CConsoleMessage::GetInstance()->Write( "Get All Character Number" );
	std::vector< AllCharNum > vChar;
	m_pDBMan->GetAllChar( vChar );
	CConsoleMessage::GetInstance()->Write( "Total Character Number : %d", ( int ) vChar.size() );

	CLogFile LogFile( CString( "ItemSearch" ) );
	LogFile.Write( "Item(%d/%d)", m_nItemMain, m_nItemSub );
	LogFile.Write( "Type,Owner,TurnNum,Guid" );

	std::vector< ITEM_SEARCH > result;
	if ( 1 == m_nCheck )
	{
		if ( DB_ERROR == m_pDBMan->ItemSearch( m_guid, result ) )
		{
			return;
		}
	}
	else
	{
		if ( DB_ERROR == m_pDBMan->ItemSearch( m_nItemMain, m_nItemSub, result ) )
		{
			return;
		}
	}

	int ItemSize = static_cast< int >( result.size() );
	nTotalSize = ItemSize;
	for ( int loop = 0; loop < ItemSize; loop++ )
	{
		ITEM_SEARCH& Item = result[loop];

		nCurrent++;
		nPercent = ( int )( nCurrent*100 ) / nTotalSize;
		m_SearchProgress.SetPos( nPercent );

		CString strTemp;
		strTemp.Format( "%d/%d (%d%%)", nCurrent, nTotalSize, nPercent );
		SetDlgItemText( IDC_SEARCHSTATIC1, strTemp );

		/*
		INVEN_DELETED		= 0,	//! ������ ������(�ʵ忡 �����ų� �׷� ����̴�.)
		INVEN_PUTON			= 1,	//! ���������
		INVEN_INVEN			= 2,	//! �Ϲ� �κ��丮
		INVEN_QUEST_PROC	= 3,	//! �������� Quest Item
		INVEN_QUEST_END		= 4,	//! �Ϸ�� Quest Item
		INVEN_POST			= 5,	//! �������� �������� �ִ� ������
		INVEN_LOCKER		= 6,	//! ĳ���� ��Ŀ
		INVEN_CLUB_LOCKER	= 7,	//! Ŭ�� ��Ŀ
		INVEN_VEHICLE		= 8,	//! Vehicle
		INVEN_PET			= 9,	//! Pet
		INVEN_DELETED2		= 10,	//! ������ ������(������ ������ �Ȱų� �ð��� �ٵǾ� �����ǰų� ���� ������ ����̴�.)
		*/

		CString Pos;
		int OwnerCode = -1;
		AllCharNum TempOwner;
		switch ( Item.InvenType )
		{
		case INVEN_DELETED:
			{
				Pos = "INVEN_DELETED";
			}
			break;
		case INVEN_PUTON:
			{
				Pos = "INVEN_PUTON";
				
				TempOwner.ChaNum = Item.ChaNum;
				std::vector< AllCharNum >::iterator it = std::find( vChar.begin(), vChar.end(), TempOwner );
				if ( it != vChar.end() )
				{
					OwnerCode = it->UserNum;
				}
			}
			break;
		case INVEN_INVEN:
			{
				Pos = "INVEN_INVEN";

				TempOwner.ChaNum = Item.ChaNum;
				std::vector< AllCharNum >::iterator it = std::find( vChar.begin(), vChar.end(), TempOwner );
				if ( it != vChar.end() )
				{
					OwnerCode = it->UserNum;
				}
			}
			break;
		case INVEN_QUEST_PROC:
			{
				Pos = "INVEN_QUEST_PROC";

				TempOwner.ChaNum = Item.ChaNum;
				std::vector< AllCharNum >::iterator it = std::find( vChar.begin(), vChar.end(), TempOwner );
				if ( it != vChar.end() )
				{
					OwnerCode = it->UserNum;
				}
			}
			break;
		case INVEN_QUEST_END:
			{
				Pos = "INVEN_QUEST_END";

				TempOwner.ChaNum = Item.ChaNum;
				std::vector< AllCharNum >::iterator it = std::find( vChar.begin(), vChar.end(), TempOwner );
				if ( it != vChar.end() )
				{
					OwnerCode = it->UserNum;
				}
			}
			break;
		case INVEN_POST:
			{
				Pos = "INVEN_POST";
			}
			break;
		case INVEN_LOCKER:
			{
				Pos = "INVEN_LOCKER";

				TempOwner.UserNum = Item.ChaNum;
				std::vector< AllCharNum >::iterator it = std::find( vChar.begin(), vChar.end(), TempOwner );
				if ( it != vChar.end() )
				{
					OwnerCode = it->ChaNum;
				}
			}
			break;
		case INVEN_CLUB_LOCKER:
			{
				Pos = "INVEN_CLUB_LOCKER";
			}
			break;
		case INVEN_VEHICLE:
			{
				Pos = "INVEN_VEHICLE";
			}
			break;
		case INVEN_PET:
			{
				Pos = "INVEN_PET";
			}
			break;
		case INVEN_DELETED2:
			{
				Pos = "INVEN_DELETED2";
			}
			break;
		default:
			{
				Pos = "default";
			}
			break;
		}

		std::string strGuid = sc::string::uuidToString( ( UUID )Item.guid );

		CConsoleMessage::GetInstance()->Write( "Item match type: %s owner: %d turnnum: %d guid: %s", Pos.GetString(), Item.ChaNum, Item.TurnNum, strGuid.c_str() );
		LogFile.Write( "%s,%d,%d,%s", Pos.GetString(), Item.ChaNum, Item.TurnNum, strGuid.c_str() );

		nCount = m_ListCharSearch.GetItemCount();

		::memset( &lvItem, 0, sizeof( LV_ITEM ) );
		lvItem.mask = LVIF_TEXT; // �Ӽ� ����
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
		strTemp.Format( "%d", Item.ChaNum );
		lvItem.pszText = strTemp.GetBuffer();
		m_ListCharSearch.InsertItem( &lvItem ); // ���ο� ���� ������ InsertItem�� ����Ѵ�.
		strTemp.Format( "%s", strGuid.c_str() );
		m_ListCharSearch.SetItemText( nCount, 1, strTemp);
		strTemp.Format( "%d", Item.TurnNum );
		m_ListCharSearch.SetItemText( nCount, 2, strTemp);
		m_ListCharSearch.SetItemText( nCount, 3, Pos.GetString() );
		strTemp.Format( "%d", Item.InvenType );
		m_ListCharSearch.SetItemText( nCount, 4, strTemp);
		strTemp.Format( "%d", OwnerCode );
		m_ListCharSearch.SetItemText( nCount, 5, strTemp);
		nFindCount++;
	}
	
	CConsoleMessage::GetInstance()->Write( "Find %d items", nFindCount );
	CConsoleMessage::GetInstance()->Write( "---- Search End ----" );

	CString strTemp1;
	strTemp1.Format( "%d", nFindCount);
	m_nItemCount.SetWindowText( strTemp1.GetString() );

	m_bRunning = FALSE;

	// ������ �˻��� �Ϸ�Ǹ� MakeNum ��뿩�� üũ �ڽ��� Ȱ��ȭ�� �����.
	GetDlgItem( IDC_CHECK )->EnableWindow( TRUE );
}

void CItemSearch::SpecialSearchProc()
{
}

void CItemSearch::OnBnClickedCheck()
{
	if (((CButton*)GetDlgItem(IDC_CHECK))->GetCheck())
	{
		GetDlgItem(IDC_EDIT_ITEM_MAKENUM)->EnableWindow(TRUE);
		m_nCheck = 1;
	}
	else
	{
		GetDlgItem(IDC_EDIT_ITEM_MAKENUM)->EnableWindow(FALSE);
		m_nCheck = 0;
	}	
}