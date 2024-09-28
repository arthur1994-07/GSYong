// CItemSearch.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <process.h>
#include "../GMCharEdit.h"
#include "./CSearchDuplicationItems.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"
#include "../SubDialogs/DlgInvenAdd.h"

#include "../../SigmaCore/Util/DateTime.h"

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

static unsigned int WINAPI SearchDuplicationItems(void* p)
{
	if (p == NULL) 
	{
		return 0;
	}
	else
	{
		CSearchDuplicationItems* pDlg = (CSearchDuplicationItems*) p;
		pDlg->SearchProc();
		return 0;
	}
}

static unsigned int WINAPI SearchCharactersHasDupItems(void* p)
{
    if (p == NULL) 
    {
        return 0;
    }
    else
    {
        CSearchDuplicationItems* pDlg = (CSearchDuplicationItems*) p;
        pDlg->SearchCharacterProc();
        return 0;
    }
}

static unsigned int WINAPI SearchExchangeItemHistory(void* p)
{
    if (p == NULL) 
    {
        return 0;
    }
    else
    {
        CSearchDuplicationItems* pDlg = (CSearchDuplicationItems*) p;
        pDlg->SearchExchangeItemHistoryProc();
        return 0;
    }
}

// CSearchDuplicationItems

IMPLEMENT_DYNCREATE(CSearchDuplicationItems, CFormView)

CSearchDuplicationItems::CSearchDuplicationItems()
	: CFormView(CSearchDuplicationItems::IDD)
    , m_nItemMakeNum(0)
    , m_nItemMakeNum2(0)
	, m_bRunning(FALSE)
	, m_hThread(NULL)	
	, m_bInit(FALSE)
    , m_bCheckItemID(false)
    , m_ItemMID(0)
    , m_ItemSID(0)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();

    for ( int i = 0; i < 2; i++ )
    {
        m_nResultSortColumn[i] = 0;
        m_bResultSortDirect[i] = false;
    }
}

CSearchDuplicationItems::~CSearchDuplicationItems()
{
	if (m_bRunning == TRUE) // 작업중
	{
		m_bRunning = FALSE; // 작업을 중단시킨다.
		Sleep( 1000 ); // 1 초대기
	}
}

void CSearchDuplicationItems::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEARH_DUP_LIST, m_ListCharSearch);
    DDX_Control(pDX, IDC_SEARH_DUP_LIST2, m_ListCharSearch2);
	DDX_Text(pDX, IDC_EDIT_ITEM_MAKE_NUM, m_nItemMakeNum);
	DDX_Control(pDX, IDC_SEARCH_DUP_PPROGRESS, m_SearchProgress);
	DDX_Control(pDX, IDC_EDIT_SEARCH_DUP_CONSOLE, m_EditSearchConsole);
    DDX_Text(pDX, IDC_EDIT_ITEM_MAKE_NUM2, m_nItemMakeNum2);
    DDX_Text(pDX, IDC_EDIT_ITEM_MID, m_ItemMID);
    DDX_Text(pDX, IDC_EDIT_ITEM_SID, m_ItemSID);
}

BEGIN_MESSAGE_MAP(CSearchDuplicationItems, CFormView)
	ON_NOTIFY(NM_DBLCLK, IDC_SEARH_DUP_LIST, OnNMDblclkList)
	ON_BN_CLICKED(IDC_BTN_SEARCH_DUP_START, OnBnClickedBtnSearchDupStart)	
	ON_EN_MAXTEXT(IDC_EDIT_SEARCH_DUP_CONSOLE, OnEnMaxtextEditItemSearchConsole)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_SEARCH_DUP_STOP, OnBnClickedBtnSearchDupStop)
	ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BTN_MAKE_TYPE, &CSearchDuplicationItems::OnBnClickedBtnMakeType)
    ON_BN_CLICKED(IDC_BTN_EXCHANGE_FLAG, &CSearchDuplicationItems::OnBnClickedBtnExchangeFlag)
    ON_BN_CLICKED(IDC_BTN_SEARCH_DUP_STOP2, &CSearchDuplicationItems::OnBnClickedBtnSearchDupStop2)
    ON_BN_CLICKED(IDC_BTN_SEARCH_DUP_START2, &CSearchDuplicationItems::OnBnClickedBtnSearchDupStart2)
    ON_NOTIFY(HDN_ITEMCLICK, 0, &CSearchDuplicationItems::OnHdnItemclickSearhDupList)
    ON_NOTIFY(NM_DBLCLK, IDC_SEARH_DUP_LIST2, &CSearchDuplicationItems::OnNMDblclkList2)
    ON_BN_CLICKED(IDC_BTN_VIEW_HISTORY, &CSearchDuplicationItems::OnBnClickedBtnViewHistory)
    ON_BN_CLICKED(IDC_BTN_SEARCH_DUP_STOP3, &CSearchDuplicationItems::OnBnClickedBtnSearchDupStop3)
    ON_BN_CLICKED(IDC_CHECK_ITEM_ID, &CSearchDuplicationItems::OnBnClickedCheckItemId)
END_MESSAGE_MAP()


// CSearchDuplicationItems 진단입니다.

#ifdef _DEBUG
void CSearchDuplicationItems::AssertValid() const
{
	CFormView::AssertValid();
}

void CSearchDuplicationItems::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CSearchDuplicationItems 메시지 처리기입니다.

void CSearchDuplicationItems::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.		
	m_ListCharSearch.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    m_ListCharSearch2.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	InitListHead();

	m_bInit = TRUE;
}

void CSearchDuplicationItems::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bRunning == TRUE) // 작업중
	{
		m_bRunning = FALSE; // 작업을 중단시킨다.
		Sleep( 1000 ); // 1 초대기
	}
	CFormView::OnClose();
}

// 아이템 검색 시작
void CSearchDuplicationItems::OnBnClickedBtnSearchDupStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bRunning == TRUE) // 작업중
	{
		MessageBox(_T("Already Search is running, First Stop Search Process"));
		return;
	}
	else // 작업시작
	{
		UpdateData(TRUE);
		StartSearchItem();	
	}
}

// 아이템 검색 중단
void CSearchDuplicationItems::OnBnClickedBtnSearchDupStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bRunning == TRUE) // 작업중
	{
		m_bRunning = FALSE; // 작업중단				
	}
	else // 이미 작업중단중...
	{
	}	
}

// Edit 박스가 최대 문자수에 도달했을때...
void CSearchDuplicationItems::OnEnMaxtextEditItemSearchConsole()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_EDIT_SEARCH_DUP_CONSOLE)->SetWindowText(_T(""));	

}

void CSearchDuplicationItems::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int nSelected = m_ListCharSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

    if (nSelected != -1) // 선택된 항목이 있음
    {	
        if (m_bRunning == TRUE) // 작업중...
        {
            MessageBox(_T("First Stop Search Process!"));
        }
        else
        {
            CString strTemp = m_ListCharSearch.GetItemText(nSelected, 0);
            CString strTemp2 = m_ListCharSearch.GetItemText(nSelected, 1);

            GetDlgItem(IDC_EDIT_ITEM_MAKE_NUM)->SetWindowText( strTemp.GetString() );
            GetDlgItem(IDC_EDIT_ITEM_MAKE_NUM2)->SetWindowText( strTemp.GetString() );
        }
    }
	*pResult = 0;
}

void CSearchDuplicationItems::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int nSelected = m_ListCharSearch2.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

    if (nSelected != -1) // 선택된 항목이 있음
    {	
        if (m_bRunning == TRUE) // 작업중...
        {
            MessageBox(_T("First Stop Search Process!"));
        }
        else
        {
            CString strTemp = m_ListCharSearch2.GetItemText(nSelected, 1);
            CString strTemp2 = m_ListCharSearch2.GetItemText(nSelected, 0);

            int nChaNum = atoi(strTemp);
            int nUserNum = atoi(strTemp2);

            // current character data backup.
            if ( m_pDBMan->CharacterInfoBackup( nChaNum ) == DB_ERROR )
            {
                MessageBox(_T("CharacterInfoBackup Error"));

                *pResult = 0;

                return;
            }
            //

            CPropertySheetCharEdit dlg(nChaNum, nUserNum, _T("Character Edit"), NULL, 0, true);
            if ( dlg.DoModal() == IDCANCEL )
            {
                // restore character data.
                if (m_pDBMan->RestoreCharacterInfoBackup(nChaNum) == DB_ERROR)
                {
                    MessageBox(_T("GetCharacterInfoBackup Error"));

                    *pResult = 0;

                    return;
                }
            }
        }
    }

    *pResult = 0;
}

void CSearchDuplicationItems::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_bInit)
	{
		int nWidth  = cx - 5;
		int nHeight = cy - 460;

		if (nWidth < 0)  cx = 0;
		if (nHeight < 0) cy = 0;
		
		m_EditSearchConsole.MoveWindow(0,
		                               110,
									   nWidth,
									   200);

		m_ListCharSearch.MoveWindow(0, // rect.left,
								    320, // rect.top,
								    nWidth,
								    150);

        m_ListCharSearch2.MoveWindow(0, // rect.left,
                                    320+150+10, // rect.top,
                                    nWidth,
                                    nHeight);
	}
}

void CSearchDuplicationItems::InitListHead()
{
    {
        LVCOLUMN Col;

        // Num, Name, Class, Tribe, Level,	
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;		
        Col.pszText		= _T("MakeNum");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch.InsertColumn(0, &Col);

        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= _T("Count");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch.InsertColumn(1, &Col);
    }

    {
        LVCOLUMN Col;

        // Num, Name, Class, Tribe, Level,	
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 100;
        Col.fmt			= LVCFMT_CENTER;		
        Col.pszText		= _T("User Num");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch2.InsertColumn(0, &Col);

        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 100;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= _T("ChaNum");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch2.InsertColumn(1, &Col);

        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= _T("Name");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch2.InsertColumn(2, &Col);

        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= _T("Class");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch2.InsertColumn(3, &Col);

        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= _T("Level");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch2.InsertColumn(4, &Col);

        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= _T("Online");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch2.InsertColumn(6, &Col);

        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= _T("Status");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch2.InsertColumn(7, &Col);

        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= _T("Count");
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListCharSearch2.InsertColumn(8, &Col);
    }
}


void CSearchDuplicationItems::StartSearchItem()
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
								SearchDuplicationItems,
								this,
								0,
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// 쓰래드 생성에 실패하였습니다.
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
		strGmCmd << "Search Duplication Items";
		strGmCmd << '\0';
		m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
		strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		///////////////////////////////////////////////////////////////////////////	
	}
}

void CSearchDuplicationItems::SearchProc()
{
	nFindCount = 0;
	int nPercent = 0;	
	
	m_bRunning = TRUE;

    GetDlgItem(IDC_BTN_SEARCH_DUP_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SEARCH_DUP_START2)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_VIEW_HISTORY)->EnableWindow(FALSE);

	CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_SEARCH_DUP_CONSOLE)->m_hWnd);

	// Set Range Of progress bar
	m_SearchProgress.SetRange(0, 100);

    m_SearchProgress.SetPos( 0 );

    CString strTemp;
    strTemp.Format("%d/%d (%d%%)", 0, 0, 0);
    SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);
	
	m_ListCharSearch.DeleteAllItems();

	CConsoleMessage::GetInstance()->Write("---- Search Start ----");

    std::string strGenType = "Gen Type : ";

    if ( GenType.size() == 0 )
        strGenType += std::string( "all" );
    else
    {
        for ( size_t i = 0; i < GenType.size(); i++ )
        {
            if ( i == 0 )
                strGenType += sc::string::format( "%1%", GenType[i] );
            else
                strGenType += sc::string::format( ", %1%", GenType[i] );
        }
    }

    std::string strExchangeFlag = "Exchange Flag : ";

    if ( ExchangeFlag.size() == 0 )
        strExchangeFlag += std::string( "all" );
    else
    {
        for ( size_t i = 0; i < ExchangeFlag.size(); i++ )
        {
            if ( i == 0 )
                strExchangeFlag += sc::string::format( "%1%", ExchangeFlag[i] );
            else
                strExchangeFlag += sc::string::format( ", %1%", ExchangeFlag[i] );
        }
    }

    CConsoleMessage::GetInstance()->Write( strGenType.c_str() );
    CConsoleMessage::GetInstance()->Write( strExchangeFlag.c_str() );

    CConsoleMessage::GetInstance()->Write("Get All Exchange Item Log");
    CConsoleMessage::GetInstance()->Write("Please wait...");

    // Exchange log를 가져온다.
    ExchangeLog.clear();
    m_pDBMan->GetExchangeItemLog( GenType, ExchangeFlag, ExchangeLog );

    CLogFile LogFile(CString("SearchDuplicationItems"));

    LogFile.Write("Gen Type: %s, Exchange Flag : %s", strGenType.c_str(), strExchangeFlag.c_str());
    LogFile.Write("MakeNum, Count");

    size_t LogSize = ExchangeLog.size();

    std::string strFormatTemp;
    // Insert to table.
    for ( size_t i = 0; i < LogSize; i++ )
    {
        if (m_bRunning == FALSE)
        {
            break;
        }

        nPercent = static_cast<int>( (i*100)/LogSize );
        m_SearchProgress.SetPos(nPercent);

        strTemp.Format("%d/%d (%d%%)", i, LogSize, nPercent);
        SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);

        strFormatTemp = sc::string::format( "MakeNum : %1%, Count : %2%", ExchangeLog[i].MakeNum, ExchangeLog[i].Count );
        CConsoleMessage::GetInstance()->Write( strFormatTemp.c_str() );

        strFormatTemp = sc::string::format( "%1%, %2%", ExchangeLog[i].MakeNum, ExchangeLog[i].Count );
        strTemp.Format("%s", strFormatTemp.c_str());
        LogFile.Write(strTemp.GetBuffer());

        strFormatTemp = sc::string::format( "%1%", ExchangeLog[i].MakeNum );
        strTemp.Format("%s", strFormatTemp.c_str());
        int nIndex = m_ListCharSearch.InsertItem( nFindCount, strTemp );

        strFormatTemp = sc::string::format( "%1%", ExchangeLog[i].Count );
        strTemp.Format("%s", strFormatTemp.c_str());
        m_ListCharSearch.SetItemText( nIndex, 1, strTemp );

        nFindCount++;

        Sleep(1);
    }

    m_SearchProgress.SetPos( 100 );

    strTemp.Format("%d/%d (%d%%)", LogSize, LogSize, 100);
    SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);
	
	CConsoleMessage::GetInstance()->Write("Find %d Logs", LogSize);
	CConsoleMessage::GetInstance()->Write("---- Search End ----");

	m_bRunning = FALSE;

    GetDlgItem(IDC_BTN_SEARCH_DUP_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_SEARCH_DUP_START2)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_VIEW_HISTORY)->EnableWindow(TRUE);
}

void CSearchDuplicationItems::OnBnClickedBtnMakeType()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDlgInvenAdd dlgAdd(GEN_TYPE);

    if (dlgAdd.DoModal() == IDOK)
    {
        if (dlgAdd.m_nSelected != -1)
        {
            GenType = dlgAdd.m_vSelected;

            UpdateData(FALSE);
        }
    }
}

void CSearchDuplicationItems::StartSearchCharacter()
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
								SearchCharactersHasDupItems,
								this,
								0,
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// 쓰래드 생성에 실패하였습니다.
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
		strGmCmd << "Search Characters has this " << m_nItemMakeNum;
		strGmCmd << '\0';
		m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
		strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		///////////////////////////////////////////////////////////////////////////	
	}
}

void CSearchDuplicationItems::SearchCharacterProc()
{
    int nTotalSize = 0;
    nFindCount = 0;
    int nPercent = 0;

    m_bRunning = TRUE;

    GetDlgItem(IDC_BTN_SEARCH_DUP_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SEARCH_DUP_START2)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_VIEW_HISTORY)->EnableWindow(FALSE);

    CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_SEARCH_DUP_CONSOLE)->m_hWnd);

    // Set Range Of progress bar
    m_SearchProgress.SetRange(0, 100);

    m_SearchProgress.SetPos( 0 );

    CString strTemp;
    strTemp.Format("%d/%d (%d%%)", 0, 0, 0);
    SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);

    m_ListCharSearch2.DeleteAllItems();

    CConsoleMessage::GetInstance()->Write("---- Search Start ----");

    // 전체 인벤 숫자를 가져온다
    CConsoleMessage::GetInstance()->Write("Get All Character Number");
    CConsoleMessage::GetInstance()->Write("Please wait...");
    std::vector<AllCharNum> vChar;
    m_pDBMan->GetAllChar(vChar);
    CConsoleMessage::GetInstance()->Write("Total Character Number : %d", static_cast<int>(vChar.size()));

    nTotalSize = static_cast<int>(vChar.size());

    CLogFile LogFile(CString("SearchCharactersHasDupItems"));

    LogFile.Write("MakeNum: %d", m_nItemMakeNum );
    LogFile.Write("UserNum, ChaNum, ChaName, Class, Count");

    size_t CharSize = vChar.size();

    // Insert to table.
    for ( size_t i = 0; i < CharSize; i++ )
    {
        if (m_bRunning == FALSE)
        {
            break;
        }

        GLCHARLOGIC pChar;

        nPercent = static_cast<int>( (i*100)/CharSize );
        m_SearchProgress.SetPos(nPercent);

        strTemp.Format("%d/%d (%d%%)", i, CharSize, nPercent);
        SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);

        CConsoleMessage::GetInstance()->Write("Character %d %s", vChar[i].ChaNum, vChar[i].ChaName.GetString());

        if (m_pDBMan->GetCharacterInfo(vChar[i].ChaNum, vChar[i].UserNum, &pChar) == DB_ERROR)
        {			
            continue;
        }

        std::vector<CharInfo> vChaInfo;
        std::strstream strQuery;
        strQuery << "SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
        strQuery << "ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ChaDex, ";
        strQuery << "ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline, ChaDeleted ";
        strQuery << "From ChaInfo WHERE ChaNum=" << vChar[i].ChaNum;
        strQuery << '\0';

        if (m_pDBMan->SearchCharacter(strQuery, vChaInfo) == DB_ERROR) 
        {
            continue;
        }
        else
        {
            if ( vChaInfo.size() == 0 || vChaInfo.size() > 1 )
                continue;
        }

        const GLInventory::CELL_MAP* pItemMap = pChar.m_cInventory.GetItemList();
        GLInventory::CELL_MAP_CITER iter = pItemMap->begin();
        GLInventory::CELL_MAP_CITER iter_end = pItemMap->end();

        int CountDuplicationItem = 0;
        for ( ; iter != iter_end; iter++ )
        {
            const SINVENITEM &sInvenItem = *(*iter).second;

            if ( m_bCheckItemID )
            {
                int ItemMID = static_cast<int>(sInvenItem.Mid());
                int ItemSID = static_cast<int>(sInvenItem.Sid());

                if ( ItemMID == m_ItemMID && ItemSID == m_ItemSID && /*sInvenItem.CustomItem().lnGenNum*/0 == m_nItemMakeNum )
                    CountDuplicationItem++;
            }
            else
            {
                if ( /*sInvenItem.CustomItem().lnGenNum*/0 == m_nItemMakeNum )
                    CountDuplicationItem++;
            }
        }

        if ( CountDuplicationItem >= 1 )
        {
            LogFile.Write("%d, %d, %s, %d, %d", vChar[i].UserNum, vChar[i].ChaNum, vChar[i].ChaName.GetString(), vChaInfo[0].ChaClass, CountDuplicationItem);

            CString strTemp;
            strTemp.Format("%d", vChar[i].UserNum);
            int nIndex = m_ListCharSearch2.InsertItem( nFindCount, strTemp );

            strTemp.Format("%d", vChar[i].ChaNum);
            m_ListCharSearch2.SetItemText( nIndex, 1, strTemp );

            strTemp.Format("%s", vChar[i].ChaName.GetString());
            m_ListCharSearch2.SetItemText( nIndex, 2, strTemp );

            strTemp.Format("%d", vChaInfo[0].ChaClass);
            m_ListCharSearch2.SetItemText( nIndex, 3, strTemp );

            strTemp.Format("%d", vChaInfo[0].ChaLevel);
            m_ListCharSearch2.SetItemText( nIndex, 4, strTemp );

            if (vChaInfo[0].ChaOnline == 1)
                m_ListCharSearch2.SetItemText(nIndex, 5, _T("Online"));
            else
                m_ListCharSearch2.SetItemText(nIndex, 5, _T("Offline"));

            if (vChaInfo[0].ChaDeleted == 1)
                m_ListCharSearch2.SetItemText(nIndex, 6, _T("Deleted"));
            else
                m_ListCharSearch2.SetItemText(nIndex, 6, _T("OK"));

            strTemp.Format("%d", CountDuplicationItem);
            m_ListCharSearch2.SetItemText( nIndex, 7, strTemp );

            nFindCount++;
        }

        Sleep(1);
    }

    m_SearchProgress.SetPos( 100 );

    strTemp.Format("%d/%d (%d%%)", CharSize, CharSize, 100);
    SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);

    CConsoleMessage::GetInstance()->Write("Find %d Characters", nFindCount);
    CConsoleMessage::GetInstance()->Write("---- Search End ----");

    m_bRunning = FALSE;

    GetDlgItem(IDC_BTN_SEARCH_DUP_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_SEARCH_DUP_START2)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_VIEW_HISTORY)->EnableWindow(TRUE);
}

void CSearchDuplicationItems::StartSearchExchangeItemHistory()
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
								SearchExchangeItemHistory,
								this,
								0,
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// 쓰래드 생성에 실패하였습니다.
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
		strGmCmd << "Search History of MakeNum - " << m_nItemMakeNum2;
		strGmCmd << '\0';
		m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
		strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		///////////////////////////////////////////////////////////////////////////	
	}
}

void CSearchDuplicationItems::SearchExchangeItemHistoryProc()
{
    nFindCount = 0;
    int nPercent = 0;	

    m_bRunning = TRUE;

    GetDlgItem(IDC_BTN_SEARCH_DUP_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_SEARCH_DUP_START2)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_VIEW_HISTORY)->EnableWindow(FALSE);

    CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_SEARCH_DUP_CONSOLE)->m_hWnd);

    // Set Range Of progress bar
    m_SearchProgress.SetRange(0, 100);

    m_SearchProgress.SetPos( 0 );

    CString strTemp;
    strTemp.Format("%d/%d (%d%%)", 0, 0, 0);
    SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);

    m_ListCharSearch.DeleteAllItems();

    CConsoleMessage::GetInstance()->Write("---- Search Start ----");

    std::string strMakeNum = sc::string::format( "MakeNum : %d", m_nItemMakeNum2);

    CConsoleMessage::GetInstance()->Write( strMakeNum.c_str() );

    CConsoleMessage::GetInstance()->Write("Get Item History");
    CConsoleMessage::GetInstance()->Write("Please wait...");

    // Exchange log를 가져온다.
    ExchangeHistory.clear();
    m_pDBMan->GetExchanageItemHistory( m_nItemMakeNum2, ExchangeHistory );

    CLogFile LogFile(CString("SearchExchangeItemHistory"));

    LogFile.Write("MakeNum : %d", m_nItemMakeNum2);
    LogFile.Write("ExchangeNum, NIDMain, NIDSub, SGNum, SvrNum, FldNum, MakeType, MakeNum, ItemAmount, ItemFromFlag, ItemFrom, ItemToFlag, ItemTo, ExchangeFlag, ExchangeDate, TradePrice");

    size_t HistorySize = ExchangeHistory.size();

    // Insert to table.
    for ( size_t i = 0; i < HistorySize; i++ )
    {
        if (m_bRunning == FALSE)
        {
            break;
        }

        nPercent = static_cast<int>( (i*100)/HistorySize );
        m_SearchProgress.SetPos(nPercent);

        strTemp.Format("%d/%d (%d%%)", i, HistorySize, nPercent);
        SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);

        std::string strTime = sc::time::DateTimeFormat(ExchangeHistory[i].ExchangeDate);

        CConsoleMessage::GetInstance()->Write( "ExchangeNum : %d", ExchangeHistory[i].ExchangeNum );
        std::string strLog = sc::string::format("%1%, %2%, %3%, %4%, %5%, %6%, %7%, %8%, %9%, %10%, %11%, %12%, %13%, %14%, %15%, %16%", ExchangeHistory[i].ExchangeNum, ExchangeHistory[i].NIDMain, ExchangeHistory[i].NIDSub, ExchangeHistory[i].SGNum, ExchangeHistory[i].SvrNum, ExchangeHistory[i].FldNum, ExchangeHistory[i].MakeType, ExchangeHistory[i].MakeNum, ExchangeHistory[i].ItemAmount, ExchangeHistory[i].ItemFromFlag, ExchangeHistory[i].ItemFrom, ExchangeHistory[i].ItemToFlag, ExchangeHistory[i].ItemTo, ExchangeHistory[i].ExchangeFlag, strTime.c_str(), ExchangeHistory[i].TradePrice);
        CString strLogTemp;
        strLogTemp.Format( "%s", strLog.c_str() );
        LogFile.Write( strLogTemp.GetBuffer() );

        nFindCount++;

        Sleep(1);
    }

    m_SearchProgress.SetPos( 100 );

    strTemp.Format("%d/%d (%d%%)", HistorySize, HistorySize, 100);
    SetDlgItemText(IDC_SEARCH_DUP_STATIC, strTemp);

    CConsoleMessage::GetInstance()->Write("Find %d records", HistorySize);
    CConsoleMessage::GetInstance()->Write("---- Search End ----");

    m_bRunning = FALSE;

    GetDlgItem(IDC_BTN_SEARCH_DUP_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_SEARCH_DUP_START2)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_VIEW_HISTORY)->EnableWindow(TRUE);

    ShowExchangeItemHistory();
}

void CSearchDuplicationItems::ShowExchangeItemHistory()
{
    CDlgInvenAdd dlgAdd(EXCHANGE_ITEM_HISTORY, NULL, &ExchangeHistory );

    if (dlgAdd.DoModal() == IDOK)
    {
        if (dlgAdd.m_nSelected != -1)
        {
            UpdateData(FALSE);
        }
    }
}

void CSearchDuplicationItems::OnBnClickedBtnExchangeFlag()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDlgInvenAdd dlgAdd(EXCHANGE_FLAG);

    if (dlgAdd.DoModal() == IDOK)
    {
        if (dlgAdd.m_nSelected != -1)
        {
            ExchangeFlag = dlgAdd.m_vSelected;

            UpdateData(FALSE);
        }
    }
}

void CSearchDuplicationItems::OnBnClickedBtnSearchDupStop2()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_bRunning == TRUE) // 작업중
    {
        m_bRunning = FALSE; // 작업중단				
    }
    else // 이미 작업중단중...
    {
    }
}

void CSearchDuplicationItems::OnBnClickedBtnSearchDupStart2()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_bRunning == TRUE) // 작업중
    {
        MessageBox(_T("Already Search is running, First Stop Search Process"));
        return;
    }
    else // 작업시작
    {
        UpdateData(TRUE);
        StartSearchCharacter();
    }
}

void CSearchDuplicationItems::OnHdnItemclickSearhDupList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if ( m_bRunning == TRUE )
        return;

    if( phdr->hdr.hwndFrom == m_ListCharSearch.GetDlgItem( 0 )->GetSafeHwnd() )
    {
        if( m_nResultSortColumn[0] == phdr->iItem )
        {
            m_bResultSortDirect[0] = !m_bResultSortDirect[0];
        }
        else
        {
            m_nResultSortColumn[0] = phdr->iItem;
            m_bResultSortDirect[0] = true;
        }

        m_ListCharSearch.DeleteAllItems();

        std::multimap<LONGLONG, int> SortMap;

        if ( m_nResultSortColumn[0] == 0 )
        {
            for ( size_t i = 0; i < ExchangeLog.size(); i++ )
                SortMap.insert( std::pair<LONGLONG, int>( ExchangeLog[i].MakeNum, i ) );
        }
        else
        {
            for ( size_t i = 0; i < ExchangeLog.size(); i++ )
                SortMap.insert( std::pair<LONGLONG, int>( ExchangeLog[i].Count, i ) );
        }

        // Insert to table.
        if ( !m_bResultSortDirect[0] )
        {
            std::multimap<LONGLONG, int>::iterator iter = SortMap.begin();
            for ( ; iter != SortMap.end(); iter++ )
            {
                int LogIndex = iter->second;

                CString strTemp;
                std::string strFormatTemp;

                strFormatTemp = sc::string::format( "%1%", ExchangeLog[LogIndex].MakeNum );
                strTemp.Format("%s", strFormatTemp.c_str());
                int nIndex = m_ListCharSearch.InsertItem( 0, strTemp );

                strFormatTemp = sc::string::format( "%1%", ExchangeLog[LogIndex].Count );
                strTemp.Format("%s", strFormatTemp.c_str());
                m_ListCharSearch.SetItemText( nIndex, 1, strTemp );
            }
        }
        else
        {
            std::multimap<LONGLONG, int>::reverse_iterator iter = SortMap.rbegin();
            for ( ; iter != SortMap.rend(); iter++ )
            {
                int LogIndex = iter->second;

                CString strTemp;

                std::string strFormatTemp;

                strFormatTemp = sc::string::format( "%1%", ExchangeLog[LogIndex].MakeNum );
                strTemp.Format("%s", strFormatTemp.c_str());
                int nIndex = m_ListCharSearch.InsertItem( 0, strTemp );

                strFormatTemp = sc::string::format( "%1%", ExchangeLog[LogIndex].Count );
                strTemp.Format("%s", strFormatTemp.c_str());
                m_ListCharSearch.SetItemText( nIndex, 1, strTemp );
            }
        }
    }
    else if( phdr->hdr.hwndFrom == m_ListCharSearch2.GetDlgItem( 0 )->GetSafeHwnd() )
    {
        if( m_nResultSortColumn[1] == phdr->iItem )
        {
            m_bResultSortDirect[1] = !m_bResultSortDirect[0];
        }
        else
        {
            m_nResultSortColumn[1] = phdr->iItem;
            m_bResultSortDirect[1] = true;
        }
    }

    *pResult = 0;
}
void CSearchDuplicationItems::OnBnClickedBtnViewHistory()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_bRunning == TRUE) // 작업중
    {
        MessageBox(_T("Already Search is running, First Stop Search Process"));
        return;
    }
    else // 작업시작
    {
        UpdateData(TRUE);
        StartSearchExchangeItemHistory();	
    }
}

void CSearchDuplicationItems::OnBnClickedBtnSearchDupStop3()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_bRunning == TRUE) // 작업중
    {
        m_bRunning = FALSE; // 작업중단				
    }
    else // 이미 작업중단중...
    {
    }
}

void CSearchDuplicationItems::OnBnClickedCheckItemId()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    CButton* pButton;
    pButton = (CButton*) GetDlgItem( IDC_CHECK_ITEM_ID );
    if ( !pButton )
        return;

    if ( pButton->GetCheck() )
    {
        m_bCheckItemID = true;

        GetDlgItem( IDC_EDIT_ITEM_MID )->EnableWindow( TRUE );
        GetDlgItem( IDC_EDIT_ITEM_SID )->EnableWindow( TRUE );
    }
    else
    {
        m_bCheckItemID = false;

        GetDlgItem( IDC_EDIT_ITEM_MID )->EnableWindow( FALSE );
        GetDlgItem( IDC_EDIT_ITEM_SID )->EnableWindow( FALSE );
    }
}
