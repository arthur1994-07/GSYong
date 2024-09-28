// CItemChange.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <process.h>
#include <set>

#include "../SigmaCore/Math/SeqUniqueGuid.h"
#include "../SigmaCore/Util/EBTime.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Item/GLItemCustom.h"

#include "../GMCharEdit.h"
#include "../MainFrm.h"
#include "../../RanLogicServer/DxServerInstance.h"

#include "../Util/CGmCfg.h"

#include "CItemChange.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CItemChange
static unsigned int WINAPI ItemChangeThread(void* p)
{
	if (p == NULL) 
	{	
		return 0;
	}
	else
	{
		CItemChange* pTemp = reinterpret_cast<CItemChange*> (p);
		pTemp->ChangeProc();
		return 0;
	}
}

namespace
{
	void Step2ChangeProc(BOOL& bRunning, GLItemLimit* pItemLimit);
};

static DWORD WINAPI ItemStep2ChangeThread(void* p)
{
	if (p == NULL)
	{
		return 0;
	}
	else
	{
		CItemChange* pDlg = reinterpret_cast<CItemChange*> (p);
		Step2ChangeProc(pDlg->m_bRunning, pDlg->m_pItemLimit);
		return 0;
	}
}

IMPLEMENT_DYNCREATE(CItemChange, CFormView)

CItemChange::CItemChange()
	: CFormView(CItemChange::IDD)
	, m_bRunning(FALSE)
	, m_hThread(NULL)	
	, m_sFromMain(0)
	, m_sFromSub(0)
	, m_sToMain(0)
	, m_sToSub(0)
	, m_bInit(FALSE)    
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
    m_pItemLimit = new GLItemLimit(NULL, NULL, NULL);
}

CItemChange::~CItemChange()
{
	if (m_bRunning == TRUE) // �۾���
	{
		m_bRunning = FALSE; // �۾��� �ߴܽ�Ų��.
		Sleep( 1000 ); // 1 �ʴ��
	}
    SAFE_DELETE(m_pItemLimit);
}

void CItemChange::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FROM_MAIN, m_sFromMain);
	DDX_Text(pDX, IDC_EDIT_FROM_SUB, m_sFromSub);
	DDX_Text(pDX, IDC_EDIT_TO_MAIN, m_sToMain);
	DDX_Text(pDX, IDC_EDIT_TO_SUB, m_sToSub);
	DDX_Control(pDX, IDC_PROGRESS1, m_ChangeProgress);
	DDX_Control(pDX, IDC_EDIT_ITEM_CHANGE_CONSOLE, m_EditItemChangeConsole);
}

BEGIN_MESSAGE_MAP(CItemChange, CFormView)
	ON_EN_MAXTEXT(IDC_EDIT_ITEM_CHANGE_CONSOLE, OnEnMaxtextEditItemChangeConsole)
	ON_BN_CLICKED(IDC_BTN_CHANGE_STOP, OnBnClickedBtnChangeStop)
	ON_BN_CLICKED(IDC_BTN_CHANGE_START, OnBnClickedBtnChangeStart)
	ON_BN_CLICKED(IDC_BTN_CHANGE_START2, OnBnClickedBtnChangeStart2)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_SELECT_ITEM2, OnBnClickedBtnSelectItem2)
	ON_BN_CLICKED(IDC_BTN_SELECT_ITEM, OnBnClickedBtnSelectItem)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CItemChange �����Դϴ�.

#ifdef _DEBUG
void CItemChange::AssertValid() const
{
	CFormView::AssertValid();
}

void CItemChange::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

// CItemChange �޽��� ó�����Դϴ�.
void CItemChange::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	// CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_ITEM_CHANGE_CONSOLE)->m_hWnd);

	m_bInit = TRUE;
}

void CItemChange::OnEnMaxtextEditItemChangeConsole()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GetDlgItem(IDC_EDIT_ITEM_CHANGE_CONSOLE)->SetWindowText("");
}

void CItemChange::ItemChangeStart()
{
	DWORD dwThreadID = 101;	
	/*
	m_hThread = ::CreateThread(NULL, 
					           0, 
					           (LPTHREAD_START_ROUTINE) ItemChangeThread, 
					           this,
					           0,
					           &dwThreadID);
	*/
	m_hThread = (HANDLE) ::_beginthreadex(
								NULL,
								0,
								ItemChangeThread,
								this,
								0,
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// ������ ������ �����Ͽ����ϴ�.
        MessageBox(_T("Change Failed, Can't start thread"));
	}
	else
	{
		///////////////////////////////////////////////////////////////////////////
		// GM Log
		CString strUserIP = m_pDBMan->GetLocalIP();
		CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
		int nGmNum = pFrame->m_nUserNum;
		std::strstream strGmCmd;
		strGmCmd << "Item Change " << m_sFromMain << " " << m_sFromSub ;
		strGmCmd << " To " << m_sToMain << " " << m_sToSub;
		strGmCmd << '\0';
		m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
		strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		///////////////////////////////////////////////////////////////////////////
	}
}

void CItemChange::OnBnClickedBtnChangeStop()
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

void CItemChange::OnBnClickedBtnChangeStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// �������� üũ
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_MASTER)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	UpdateData(TRUE);

	if (m_bRunning == TRUE) // �۾���
	{
		MessageBox(_T("Already Change is running, First Stop Change Process"));
		return;
	}
	else // �۾�����
	{
		ItemChangeStart();
	}
}

void CItemChange::OnBnClickedBtnChangeStart2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// Note : ������,�κ� ���� �������� ���� ������ ��ȯ.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_MASTER)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_ITEM_CHANGE_CONSOLE)->m_hWnd);

	DWORD dwThreadID = 101;	
	
	//m_hThread = ::CreateThread(NULL, 
	//				0, 
	//				(LPTHREAD_START_ROUTINE) ItemStep2ChangeThread, 
	//				this, 
	//				0, 
	//				&dwThreadID);
	if (m_hThread == NULL)
	{
		// ������ ������ �����Ͽ����ϴ�.
        MessageBox("Change Failed, Can't start thread");
	}
}


void CItemChange::ChangeProc()
{	
	int nTotalSize = 0;
	int nCurrent   = 0;
	int	nFindCount = 0;
	int nPercent   = 0;

	CString strTemp;
	
	if (m_bRunning == TRUE) 
	{
		return; // �۾����̸� ����
	}
	else // �۾����� �ƴϸ� �۾������� ����
	{
		m_bRunning = TRUE;
	}

	SINVENITEM* pInvenItem = NULL;	
	
	CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_ITEM_CHANGE_CONSOLE)->m_hWnd);

	m_ChangeProgress.SetRange(0, 100);

	// ��ü ĳ���� ��ȣ/������ �����´�.
	std::vector<AllCharNum> vChar;
	m_pDBMan->GetAllChar(vChar);

	// ��ü �κ� ���ڸ� �����´�
	std::vector<AllCharNum> vInven;
	m_pDBMan->GetAllInven(vInven);
    
	// ��ü �۾��ؾ��� ����
	// ĳ���� ���� + ��Ŀ����
	nTotalSize = (int) vChar.size() + (int) vInven.size();
	
	// Get New Item
	SITEM *pNewItemData = GLogicData::GetInstance().GetItem (m_sToMain, m_sToSub);
	if ( !pNewItemData )
	{
		MessageBox("Invalid Item, Check Item Number");
		return;
	}

	CLogFile LogFile(CString("ItemChange"));
	LogFile.Write("UserNum,CharacterNum,CharacterName");
	
	// ������ ���鼭 ĳ���� �ϳ��� �����Ѵ�.
	for(int i=0; i<(int) vChar.size(); i++)
	{
		GLCHARLOGIC pChar;
		if (m_bRunning == FALSE)
		{
			break;
		}
		CConsoleMessage::GetInstance()->Write("%d %s", vChar[i].ChaNum, vChar[i].ChaName.GetString());
		
		nCurrent++;
		nPercent = (int) (nCurrent*100)/nTotalSize;
		m_ChangeProgress.SetPos(nPercent);

		CString strTemp;
		strTemp.Format("%d/%d (%d%%)", nCurrent, nTotalSize, nPercent);
		SetDlgItemText(IDC_CHANGESTATIC1, strTemp);

		if (m_pDBMan->GetCharacterInfo(vChar[i].ChaNum, vChar[i].UserNum, &pChar) == DB_ERROR)
		{
            continue;			
		}
		if (m_pDBMan->CheckInven(vChar[i].UserNum) == FALSE)
		{
			m_pDBMan->MakeUserInven(vChar[i].UserNum);
		}			
		m_pDBMan->GetUserInven(&pChar);
		
		/////////////////////////////////////////////////////////////////////////////
		// �������������
		int nMaxSize = EMSLOT(SLOT_TSIZE);
		for (int i2=0; i2<nMaxSize; i2++)
		{
			if ((pChar.m_PutOnItems[i2].Mid() == m_sFromMain) &&
				(pChar.m_PutOnItems[i2].Sid() == m_sFromSub))
			{
				CConsoleMessage::GetInstance()->Write("Item Found %d %d %s at Put On Item", vChar[i].UserNum, vChar[i].ChaNum, vChar[i].ChaName.GetString());


				SITEMCUSTOM sItem( pNewItemData->sBasicOp.GetNativeID() );

				// If availabe slot and item
				if (pChar.CHECKSLOT_ITEM(sItem.GetNativeID(), EMSLOT(i2)) == EMSLOTCHECK_OK) 
				{
					// Delete exist item
					pChar.RELEASE_SLOT_ITEM(EMSLOT(i2));
					// Set new item to slot
					pChar.SLOT_ITEM(sItem, EMSLOT(i2));
					// Save character
					m_pDBMan->SaveCharacterPutOnItem(&pChar);
					CConsoleMessage::GetInstance()->Write("Item Changed");
					LogFile.Write("%d,%d,%s", vChar[i].UserNum, vChar[i].ChaNum, vChar[i].ChaName.GetString());
					nFindCount++;
				}				
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		// ĳ���� �κ�����
		EMCHARINDEX emCharIndex = CharClassToIndex ( pChar.m_emClass );

		SINVENITEM* pInvenItem = NULL;
		while ( pInvenItem = pChar.m_cInventory.FindItem ( SNATIVEID(m_sFromMain,m_sFromSub) ) )
		{
			CConsoleMessage::GetInstance()->Write("Item Found %d %d %s at Inventory", vChar[i].UserNum, vChar[i].ChaNum, vChar[i].ChaName.GetString());
			pChar.m_cInventory.DeleteItem ( pInvenItem->wPosX, pInvenItem->wPosY );

			// Insert new item
			SITEMCUSTOM sItemY(pNewItemData->sBasicOp.GetNativeID());

			BOOL bOk = pChar.m_cInventory.InsertItem ( sItemY );
			if (bOk)
			{
				// Save Character
				m_pDBMan->SaveCharacterInven(&pChar);
				CConsoleMessage::GetInstance()->Write("Item Changed");
				LogFile.Write("%d,%d,%s", vChar[i].UserNum, vChar[i].ChaNum, vChar[i].ChaName.GetString());
				nFindCount++;				
			}
			else
			{
				CConsoleMessage::GetInstance()->Write("Item Change Failed");
			}
		}
		Sleep( 1 );
	}

	// ������ ���鼭 ������κ��� �ϳ��� �����Ѵ�.
	for(int i=0; i<(int) vInven.size(); i++)
	{
		GLCHARLOGIC pChar;
		if (m_bRunning == FALSE) // �۾��ߴ�
		{
			break;
		}

		nCurrent++;
		nPercent = (int) (nCurrent*100)/nTotalSize;
		m_ChangeProgress.SetPos(nPercent);
		CString strTemp;
		strTemp.Format("%d/%d (%d%%)", nCurrent, nTotalSize, nPercent);
		SetDlgItemText(IDC_SEARCHSTATIC1, strTemp);

		CConsoleMessage::GetInstance()->Write("User Inventory %d %s", vInven[i].ChaNum, vInven[i].ChaName.GetString());

		// ����� �κ�����
		// â��� �� 5 ���� �����Ǿ� �ִ�.
		// ����� �ϳ��� ���. (0 ���� ����Ѵ�)
		// ���Ŀ� �þ ��� 0 ���� 4 ������ ��밡��
		
		CString ChaName;

		pChar.SetUserID( (DWORD) vInven[i].UserNum );
		pChar.m_CharDbNum = (DWORD) vInven[i].ChaNum;
        pChar.SetName(vInven[i].ChaName.GetString());
        
		// �κ��丮�� �����´�.
		if (m_pDBMan->GetUserInven(&pChar) == DB_ERROR)
		{
			continue;
		}

		/////////////////////////////////////////////////////////////////////////////
		// ����� �κ�����
		// â��� �� 5 ���� �����Ǿ� �ִ�.
		// ����� �ϳ��� ���. (0 ���� ����Ѵ�)
		// ���Ŀ� �þ ��� 0 ���� 4 ������ ��밡��	
		for (int nInvenCount=0; nInvenCount < EMSTORAGE_CHANNEL; nInvenCount++)
		{
			pInvenItem = NULL;
			while (pInvenItem = pChar.m_cStorage[nInvenCount].FindItem(SNATIVEID(m_sFromMain,m_sFromSub)))
			{
				CConsoleMessage::GetInstance()->Write("Item Found %d %d %s at User Inven", vChar[i].UserNum, vChar[i].ChaNum, vChar[i].ChaName.GetString());
				pChar.m_cStorage[nInvenCount].DeleteItem(pInvenItem->wPosX, pInvenItem->wPosY);
				 
				// Insert new item
				SITEMCUSTOM sItemY(pNewItemData->sBasicOp.GetNativeID());
				BOOL bOk = pChar.m_cStorage[nInvenCount].InsertItem(sItemY);
				if ( !bOk )
				{
					CConsoleMessage::GetInstance()->Write("Item Change Failed");
				}
				else
				{	
					// Save Character
					m_pDBMan->SaveCharacterUserInven(&pChar);
					CConsoleMessage::GetInstance()->Write("Item Changed");
					LogFile.Write("%d,%d,%s", vChar[i].UserNum, vChar[i].ChaNum, vChar[i].ChaName.GetString());
					nFindCount++;
				}					
			}
		}
		Sleep( 1 );
	}
	m_bRunning = FALSE; // �۾������
	CConsoleMessage::GetInstance()->Write("Change %d items", nFindCount);
	CConsoleMessage::GetInstance()->Write("---- Change End ----");
}

namespace
{
	void Step2ChangeProc(BOOL& bRunning, GLItemLimit* pItemLimit)
	{
		typedef std::set<DWORD>		SETUSER;
		typedef SETUSER::iterator	SETUSER_ITER;

		bRunning = TRUE;

		// ��ü ĳ���� ��ȣ�� �����´�.
		std::vector<AllCharNum> vChar;
		SETUSER					setUser;
		CConsoleMessage::GetInstance()->Write("---- Change Start ----");
		CConsoleMessage::GetInstance()->Write("Get All Character Number");
		
		float fSTART_TIME = sc::DXUtil_Timer ( sc::TIMER_GETAPPTIME );
		
		CGmCfg::GetInstance()->GetDBMan()->GetAllChar(vChar);

		CConsoleMessage::GetInstance()->Write ( "char num : %d", vChar.size() );

		int nITEM_NUM(0);

		int nRET(0);
		GLCHARLOGIC sCHARLOGIC;

		// ������ ���鼭 ĳ���� �ϳ��� �����Ѵ�.
		for ( int i=0; i < (int)vChar.size(); i++ )
		{
			Sleep( 1 );
			
			// �ߴ� ��ư�� ������ �۾��� �ߴܽ�Ų��.
			if (!bRunning)		break;

			//CConsoleMessage::GetInstance()->Write ( "%d %s", vChar[i].ChaNum, vChar[i].ChaName.GetString() );
			
			nRET = CGmCfg::GetInstance()->GetDBMan()->GetCharacterInfo ( vChar[i].ChaNum, vChar[i].UserNum, &sCHARLOGIC );
			if ( nRET == DB_ERROR )
			{
				CConsoleMessage::GetInstance()->Write ( "charid : %d  charname: %s : read - db_error", vChar[i].ChaNum, vChar[i].ChaName.GetString() );
				continue;
			}

			sCHARLOGIC.m_bServerStorage = FALSE;

			setUser.insert ( sCHARLOGIC.GetUserID() );


			for ( int i=0; i<SLOT_TSIZE; ++i )
			{
				SITEMCUSTOM sCUSTOM = sCHARLOGIC.GET_SLOT_ITEM ( (EMSLOT)i );

				nITEM_NUM++;

				sCUSTOM.cGenType = (EMITEMGEN) EMGEN_OLD;
				sc::SeqUniqueGUID::Instance()->getGUID( sCUSTOM.guid );
				sCUSTOM.GenerateBasicStat( false );
				sCUSTOM.GenerateLinkSkill();
				sCUSTOM.GenerateAddOption();

				sCHARLOGIC.SLOT_ITEM ( sCUSTOM, (EMSLOT)i );

				// ��� �� �α� ����
				pItemLimit->LogItemExchange ( sCUSTOM, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, sCHARLOGIC.m_CharDbNum, ITEM_ROUTE_CHAR, sCUSTOM.wTurnNum, MapID(SNATIVEID(false),SNATIVEID(false)), LOG_ITEM_CHAR_SLOT_CURRENT, false );
			}

			const GLInventory::CELL_MAP* pMapItem = sCHARLOGIC.m_cInventory.GetItemList();
			GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
			GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();
			for ( ; iter!=iter_end; ++iter )
			{
				SINVENITEM *pINVENITEM = (*iter).second;

				nITEM_NUM++;

				pINVENITEM->sItemCustom.cGenType = (EMITEMGEN) EMGEN_OLD;
				sc::SeqUniqueGUID::Instance()->getGUID( pINVENITEM->sItemCustom.guid );
				pINVENITEM->sItemCustom.GenerateBasicStat( false );
				pINVENITEM->sItemCustom.GenerateLinkSkill();
				pINVENITEM->sItemCustom.GenerateAddOption();

				// ��� �� �α� ����
				pItemLimit->LogItemExchange ( pINVENITEM->sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, sCHARLOGIC.m_CharDbNum, ITEM_ROUTE_CHAR, pINVENITEM->sItemCustom.wTurnNum, MapID(SNATIVEID(false),SNATIVEID(false)), LOG_ITEM_CHAR_INVEN_CURRENT, false );
			}

			nRET = CGmCfg::GetInstance()->GetDBMan()->SaveCharacter ( &sCHARLOGIC );
			if ( nRET == DB_ERROR )
			{
//				CConsoleMessage::GetInstance()->Write ( "charid : %d  charname: %s : read - db_error", vChar[i].ChaNum, vChar[i].ChaName.GetString() );
				continue;
			}
		}

		CConsoleMessage::GetInstance()->Write ( "user num : %d", setUser.size() );

		SETUSER_ITER iter = setUser.begin();
		SETUSER_ITER iter_end = setUser.end();
		for ( ; iter!=iter_end; ++iter )
		{
			Sleep( 1 );
			
			// �ߴ� ��ư�� ������ �۾��� �ߴܽ�Ų��.
			if (!bRunning)		break;


			DWORD dwUserID = (*iter);

			GLCHARLOGIC sCHARDATA;
			sCHARDATA.SetUserID( dwUserID );

			nRET = CGmCfg::GetInstance()->GetDBMan()->ReadUserInven ( &sCHARDATA );
			if ( nRET==DB_ERROR )
			{
				CConsoleMessage::GetInstance()->Write ( "userid : %d read - db_error", dwUserID );
				continue;
			}

			se::ByteStream streamStorage;
			sCHARDATA.GETSTORAGE_BYBUF ( streamStorage );
			
			const GLInventory::CELL_MAP* pMapItem = sCHARDATA.m_cStorage[0].GetItemList();
			GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
			GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();
			for ( ; iter!=iter_end; ++iter )
			{
				SINVENITEM *pINVENITEM = (*iter).second;

				nITEM_NUM++;

				pINVENITEM->sItemCustom.cGenType = (EMITEMGEN) EMGEN_OLD;
				sc::SeqUniqueGUID::Instance()->getGUID( pINVENITEM->sItemCustom.guid );
				pINVENITEM->sItemCustom.GenerateBasicStat( false );
				pINVENITEM->sItemCustom.GenerateLinkSkill();
				pINVENITEM->sItemCustom.GenerateAddOption();

				// ��� �� �α� ����
				pItemLimit->LogItemExchange ( pINVENITEM->sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_USER, dwUserID, ITEM_ROUTE_USER_INVEN, pINVENITEM->sItemCustom.wTurnNum, MapID(SNATIVEID(false),SNATIVEID(false)), LOG_ITEM_CHAR_STORAGE_CURRENT, false );
			}

			LPBYTE pBuff(NULL);
			DWORD dwSize(0);
			streamStorage.GetBuffer(pBuff,dwSize);

			nRET = CGmCfg::GetInstance()->GetDBMan()->WriteUserInven ( sCHARDATA.GetStorageMoney(), dwUserID, (char*)pBuff, dwSize );
			if ( nRET==DB_ERROR )
			{
				CConsoleMessage::GetInstance()->Write ( "userid : %d write - db_error", dwUserID );
				continue;
			}
		}

		float fEND_TIME = sc::DXUtil_Timer ( sc::TIMER_GETAPPTIME );

		float fTOTAL_TIME = fEND_TIME - fSTART_TIME;

		CConsoleMessage::GetInstance()->Write ( "total item : %d", nITEM_NUM );
		CConsoleMessage::GetInstance()->Write ( "total time : %f", fTOTAL_TIME );
		CConsoleMessage::GetInstance()->Write ( "item time : %f", fTOTAL_TIME/nITEM_NUM );
		CConsoleMessage::GetInstance()->Write ( "user time : %f", fTOTAL_TIME/setUser.size() );
		CConsoleMessage::GetInstance()->Write("---- Change End ----");
		bRunning = FALSE;
	}
};

void CItemChange::OnBnClickedBtnSelectItem()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDlgInvenAdd dlgAdd;

	if (dlgAdd.DoModal() == IDOK)
    {
        if (dlgAdd.m_nSelected != -1)
        {
            SITEMCUSTOM sItem(dlgAdd.m_vItem[dlgAdd.m_nSelected].sBasicOp.sNativeID);
            m_sFromMain       = sItem.Mid();
			m_sFromSub        = sItem.Sid();
			UpdateData(FALSE);
        }
    }
}

void CItemChange::OnBnClickedBtnSelectItem2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDlgInvenAdd dlgAdd;

	if (dlgAdd.DoModal() == IDOK)
    {
        if (dlgAdd.m_nSelected != -1)
        {
            SITEMCUSTOM sItem(dlgAdd.m_vItem[dlgAdd.m_nSelected].sBasicOp.sNativeID);
            m_sToMain       = sItem.Mid();
			m_sToSub        = sItem.Sid();
			UpdateData(FALSE);
        }
    }
}

void CItemChange::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_bInit)
	{
		int nWidth  = cx - 5;
		int nHeight = cy - 160;

		if (nWidth < 0)  cx = 0;
		if (nHeight < 0) cy = 0;
		
		m_EditItemChangeConsole.MoveWindow(0,
		                                   150,
									       nWidth,
									       nHeight);
	}
}

void CItemChange::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (m_bRunning == TRUE) // �۾���
	{
		m_bRunning = FALSE; // �۾��� �ߴܽ�Ų��.
		Sleep( 1000 ); // 1 �ʴ��
	}
	CFormView::OnClose();
}
