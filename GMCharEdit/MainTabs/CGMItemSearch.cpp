// CGMItemSearch.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include <process.h>
#include "../GMCharEdit.h"
#include "CGMItemSearch.h"


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

static unsigned int WINAPI GMItemSearchThread(void* p)
{
	if (p == NULL)
	{
		return 0;
	}
	else
	{
		CGMItemSearch* pDlg = reinterpret_cast<CGMItemSearch*> (p);
		pDlg->GMItemSearchProc();
		return 0;
	}
}

// CGMItemSearch

IMPLEMENT_DYNCREATE(CGMItemSearch, CFormView)

CGMItemSearch::CGMItemSearch()
	: CFormView(CGMItemSearch::IDD)
	, m_hThread (NULL)
	, m_bInit (FALSE)
	, m_bRunning (FALSE)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CGMItemSearch::~CGMItemSearch()
{
}

void CGMItemSearch::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEARCH_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_LIST_GMITEM_SEARCH, m_ListFind);
	DDX_Control(pDX, IDC_EDIT_GMITEM_SEARCH_CONSOLE, m_EditGmSearchConsole);
}

BEGIN_MESSAGE_MAP(CGMItemSearch, CFormView)
	ON_BN_CLICKED(IDC_BTN_GMITEM_SEARCH_START, OnBnClickedBtnGmitemSearchStart)
	ON_BN_CLICKED(IDC_BTN_GMITEM_SEARCH_STOP, OnBnClickedBtnGmitemSearchStop)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_EN_MAXTEXT(IDC_EDIT_GMITEM_SEARCH_CONSOLE, OnEnMaxtextEditGmitemSearchConsole)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GMITEM_SEARCH, OnNMDblclkListGmitemSearch)
END_MESSAGE_MAP()


// CGMItemSearch �����Դϴ�.

#ifdef _DEBUG
void CGMItemSearch::AssertValid() const
{
	CFormView::AssertValid();
}

void CGMItemSearch::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

// CGMItemSearch �޽��� ó�����Դϴ�.
void CGMItemSearch::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.	
	m_ListFind.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	InitListHead();
	m_bInit = TRUE;
}

// ������ �˻�����
void CGMItemSearch::OnBnClickedBtnGmitemSearchStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	if (m_bRunning == TRUE) // �۾���
	{
		MessageBox(_T("Already running, First Stop Process"));
		return;
	}
	else // �۾�����
	{
		UpdateData(TRUE);
		DWORD dwThreadID = 110;
		/*
		m_hThread = ::CreateThread(NULL,
							 	   0,
							 	   (LPTHREAD_START_ROUTINE) GMItemSearchThread,
								   this,
								   0,
								   &dwThreadID);
		*/
		m_hThread = (HANDLE) ::_beginthreadex(
									NULL,
									0,
							 		GMItemSearchThread,
									this,
									0,
									(unsigned int*) &dwThreadID );
		if (m_hThread == NULL)
		{
			// ������ ������ �����Ͽ����ϴ�.
			MessageBox(_T("Thread Failed"));
		}
		else
		{
			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "GM Item Search";
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
			strGmCmd.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			///////////////////////////////////////////////////////////////////////////
		}
	}
}

// ������ �˻�����
void CGMItemSearch::OnBnClickedBtnGmitemSearchStop()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bRunning) 
	{
		m_bRunning = FALSE;
	}
	else
	{
		return;
	}
}

void CGMItemSearch::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (m_bRunning) 
	{
		m_bRunning = FALSE;
	}
	else
	{
		return;
	}
	CFormView::OnClose();
}

void CGMItemSearch::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_bInit)
	{
		int nWidth  = cx - 5;
		int nHeight = cy - 320;

		if (nWidth < 0)  cx = 0;
		if (nHeight < 0) cy = 0;
		
		m_EditGmSearchConsole.MoveWindow(0,
		                                 100,
									     nWidth,
									     200);

		m_ListFind.MoveWindow(0, // rect.left,
				              310, // rect.top,
							  nWidth,
							  nHeight);
	}
}

void CGMItemSearch::OnEnMaxtextEditGmitemSearchConsole()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GetDlgItem(IDC_EDIT_GMITEM_SEARCH_CONSOLE)->SetWindowText("");
}


void CGMItemSearch::InitListHead()
{
	LVCOLUMN Col;
	
	// Num, Name, Class, Tribe, Level,	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= _T("User Num");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListFind.InsertColumn(0, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Char Num");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListFind.InsertColumn(1, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 150;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Cha Name");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListFind.InsertColumn(2, &Col);
    
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Pos");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListFind.InsertColumn(3, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 150;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Item Name");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListFind.InsertColumn(4, &Col);
}

void CGMItemSearch::OnNMDblclkListGmitemSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelected = m_ListFind.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� �׸��� ����
	{	
		if (m_bRunning == TRUE) // �۾���...
		{
			MessageBox(_T("First Stop Process!"));
		}
		else
		{
			CString strTemp = m_ListFind.GetItemText(nSelected, 1);
            CString strTemp2 = m_ListFind.GetItemText(nSelected, 0);
			CPropertySheetCharEdit dlg(atoi(strTemp), atoi(strTemp2), _T("Character Edit"));
			dlg.DoModal();
		}
	}
	*pResult = 0;
}

void CGMItemSearch::GMItemSearchProc()
{
	int nTotalSize = 0;
	int nCurrent   = 0;
	int nCount     = 0;
	int	nFindCount = 0;
	int nPercent   = 0;		
	
	m_bRunning = TRUE;
	CString strTemp;
	LV_ITEM lvItem;	

	CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_GMITEM_SEARCH_CONSOLE)->m_hWnd);

	// Set Range Of progress bar
	m_Progress.SetRange(0, 100);
	
	m_ListFind.DeleteAllItems();	

	CConsoleMessage::GetInstance()->Write("---- Search Start ----");	

    // ��ü ĳ���� ��ȣ/������ �����´�.
	CConsoleMessage::GetInstance()->Write("Get All Character Number");
	std::vector<AllCharNum> vChar;
	m_pDBMan->GetAllChar(vChar);
	CConsoleMessage::GetInstance()->Write("Total Character Number : %d", (int) vChar.size());

	// ��ü �κ� ���ڸ� �����´�
	CConsoleMessage::GetInstance()->Write("Get All Inventory Number");
	std::vector<AllCharNum> vInven;
	m_pDBMan->GetAllInven(vInven);
	CConsoleMessage::GetInstance()->Write("Total User Inventory Number : %d", (int) vInven.size());	
    
	nTotalSize = (int) vChar.size() + (int) vInven.size();	

	CLogFile LogFile(CString("GMItemSearch"));
	LogFile.Write(_T("GM Item Search Item Main 76"));
	LogFile.Write(_T("UserNum,CharacterNum,CharacterName,ItemName"));

    ///////////////////////////////////////////////////////////////////////////	
	// ������ ���鼭 ĳ���� �ϳ��� �����Ѵ�.
	for(int i=0; i<(int) vChar.size(); i++)	
	{
		GLCHARLOGIC pChar;
		if (m_bRunning == FALSE)
		{
			break;
		}

		nCurrent++;
		nPercent = (int) (nCurrent*100)/nTotalSize;
		m_Progress.SetPos(nPercent);

		CString strTemp;
		strTemp.Format("%d/%d (%d%%)", nCurrent, nTotalSize, nPercent);
		SetDlgItemText(IDC_STATIC_GMITEM_SEARCH, strTemp);

		CConsoleMessage::GetInstance()->Write("Character %d %s", vChar[i].ChaNum, vChar[i].ChaName.GetString());
				
		if (m_pDBMan->GetCharacterInfo(vChar[i].ChaNum, vChar[i].UserNum, &pChar) == DB_ERROR)
		{			
            continue;
		}		
		
		// �������������
		int nMaxSize = EMSLOT(SLOT_TSIZE);

		for (int i2=0; i2<nMaxSize; i2++)
		{
			if ((pChar.m_PutOnItems[i2].Mid() == 76)) // GM Item
			{
				SITEM *pItemData = GLogicData::GetInstance().GetItem(pChar.m_PutOnItems[i2].GetNativeID());
				if (pItemData != NULL) 
				{
					CConsoleMessage::GetInstance()->Write(_T("Item match %d %d %s %s at Put On Item"), 
						                                  vChar[i].UserNum,
														  vChar[i].ChaNum,
														  vChar[i].ChaName.GetString(),
														  pItemData->GetName());

					LogFile.Write(_T("%d,%d,%s,%s"), 
						          vChar[i].UserNum, 
								  vChar[i].ChaNum, 
								  vChar[i].ChaName.GetString(),
								  pItemData->GetName());

					nCount = m_ListFind.GetItemCount();				
					
					::memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT; // �Ӽ� ����
					lvItem.iItem = nCount;
					lvItem.iSubItem = 0;
					strTemp.Format("%d", vChar[i].UserNum);
					lvItem.pszText = strTemp.GetBuffer();		
					m_ListFind.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.						
					strTemp.Format("%d", vChar[i].ChaNum);
					m_ListFind.SetItemText(nCount, 1, strTemp);
					m_ListFind.SetItemText(nCount, 2, vChar[i].ChaName);
					m_ListFind.SetItemText(nCount, 3, _T("Put On Item"));
					m_ListFind.SetItemText(nCount, 4, pItemData->GetName());
					nFindCount++;
				}
			}
		}

		// ĳ���� �κ�����
		EMCHARINDEX emCharIndex = CharClassToIndex ( pChar.m_emClass );

		const GLInventory::CELL_MAP* pItemMap2 = pChar.m_cInventory.GetItemList();
		GLInventory::CELL_MAP_CITER iter2 = pItemMap2->begin();
		GLInventory::CELL_MAP_CITER iter_end2 = pItemMap2->end();
		
		for ( ; iter2!=iter_end2; ++iter2 )
		{
			const SINVENITEM &sInvenItem = *(*iter2).second;			

			if (sInvenItem.sItemCustom.Mid() == 76)
			{			
				SITEM *pItemData = GLogicData::GetInstance().GetItem(sInvenItem.sItemCustom.GetNativeID());
				if (pItemData != NULL) 
				{
					CConsoleMessage::GetInstance()->Write(_T("Item match %d %d %s %s at Character Inven"), 
						                                  vChar[i].UserNum,
														  vChar[i].ChaNum,
														  vChar[i].ChaName.GetString(),
														  pItemData->GetName());

					LogFile.Write(_T("%d,%d,%s,%s"), 
						          vChar[i].UserNum, 
								  vChar[i].ChaNum, 
								  vChar[i].ChaName.GetString(),
								  pItemData->GetName());

					nCount = m_ListFind.GetItemCount();
					::memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT; // �Ӽ� ����
					lvItem.iItem = nCount;
					lvItem.iSubItem = 0;
					strTemp.Format("%d", vChar[i].UserNum);
					lvItem.pszText = strTemp.GetBuffer();		
					m_ListFind.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.						
					strTemp.Format("%d", vChar[i].ChaNum);
					m_ListFind.SetItemText(nCount, 1, strTemp);
					m_ListFind.SetItemText(nCount, 2, vChar[i].ChaName);
					m_ListFind.SetItemText(nCount, 3, _T("Char Inven"));
					m_ListFind.SetItemText(nCount, 4, pItemData->GetName());
					nFindCount++;
				}
			}
		}		
		Sleep( 1 );
	}

	///////////////////////////////////////////////////////////////////////////
	// ������ ���鼭 ������κ��� �ϳ��� �����Ѵ�.
	for(int i=0; i<(int) vInven.size(); i++)	
	{
		GLCHARLOGIC pChar;
		if (m_bRunning == FALSE)
		{
			break;
		}		
		
		nCurrent++;
		nPercent = (int) (nCurrent*100)/nTotalSize;
		m_Progress.SetPos(nPercent);
		CString strTemp;
		strTemp.Format(_T("%d/%d (%d%%)"), nCurrent, nTotalSize, nPercent);
		SetDlgItemText(IDC_STATIC_GMITEM_SEARCH, strTemp);

		CConsoleMessage::GetInstance()->Write(_T("User Inventory %d %s"), vInven[i].ChaNum, vInven[i].ChaName.GetString());

		// ����� �κ�����
		// â��� �� 5 ���� �����Ǿ� �ִ�.
		// ����� �ϳ��� ���. (0 ���� ����Ѵ�)
		// ���Ŀ� �þ ��� 0 ���� 4 ������ ��밡��
		CString ChaName;

		pChar.SetUserID( (DWORD) vInven[i].UserNum );
		pChar.m_CharDbNum = (DWORD) vInven[i].ChaNum;
        pChar.SetName(vInven[i].ChaName.GetString());
            
		if (m_pDBMan->GetUserInven(&pChar) == DB_ERROR)
		{			
			continue;
		}

		// EMSTORAGE_CHANNEL : Max

		for (int nInvenCount=0; nInvenCount < EMSTORAGE_CHANNEL; nInvenCount++)
		{
			const GLInventory::CELL_MAP* pItemMap = pChar.m_cStorage[nInvenCount].GetItemList();
			GLInventory::CELL_MAP_CITER iter = pItemMap->begin();
			GLInventory::CELL_MAP_CITER iter_end = pItemMap->end();

			for ( ; iter!=iter_end; ++iter )
			{
				const SINVENITEM &sInvenItem = *(*iter).second;				
				
				if (sInvenItem.sItemCustom.Mid() == 76)
				{
					SITEM *pItemData = GLogicData::GetInstance().GetItem(sInvenItem.sItemCustom.GetNativeID());
					if (pItemData != NULL) 
					{
						CConsoleMessage::GetInstance()->Write(_T("Item match %d %d %s %s at User Inven"),
															  vInven[i].UserNum, 
															  vInven[i].ChaNum, 
															  vInven[i].ChaName.GetString(),
															  pItemData->GetName());

						LogFile.Write(_T("%d,%d,%s,%s"),
									  vInven[i].UserNum, 
									  vInven[i].ChaNum, 
									  vInven[i].ChaName.GetString(),
									  pItemData->GetName());

						nCount = m_ListFind.GetItemCount();
						
						::memset(&lvItem, 0, sizeof(LV_ITEM));
						lvItem.mask = LVIF_TEXT; // �Ӽ� ����
						lvItem.iItem = nCount;
						lvItem.iSubItem = 0;
						strTemp.Format(_T("%d"), vInven[i].UserNum);
						lvItem.pszText = strTemp.GetBuffer();		
						m_ListFind.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.						
						strTemp.Format(_T("%d"), vInven[i].ChaNum);
						m_ListFind.SetItemText(nCount, 1, strTemp);
						m_ListFind.SetItemText(nCount, 2, vInven[i].ChaName);
						m_ListFind.SetItemText(nCount, 3, _T("User Inven"));
						m_ListFind.SetItemText(nCount, 4, pItemData->GetName());
						nFindCount++;
					}
				}
				Sleep( 1 );
			}
			Sleep( 1 );
		}
		Sleep( 1 );
	}
	CConsoleMessage::GetInstance()->Write("Find %d items", nFindCount);
	CConsoleMessage::GetInstance()->Write("---- Search End ----");
	m_bRunning = FALSE;
}
