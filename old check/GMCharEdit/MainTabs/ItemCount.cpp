// ItemCount.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <process.h>
#include "../GMCharEdit.h"
#include "ItemCount.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"
#include "../SubDialogs/DlgInvenAdd.h"

#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Util/CGmCfg.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../Util/CConsoleMessage.h"
#include "../../RanLogicServer/DxServerInstance.h"
#include "../Util/CLogFile.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static unsigned int WINAPI ItemCountThread(void* p)
{
	if (p == NULL) 
	{
		return 0;
	}
	else
	{
		CItemCount* pDlg = (CItemCount*) p;
		pDlg->SearchProc();
		return 0;
	}
}

// CItemCount

IMPLEMENT_DYNCREATE(CItemCount, CFormView)

CItemCount::CItemCount()
	: CFormView(CItemCount::IDD)
	, m_nWeaponsCheck(0)
	, m_nDefenseCheck(0)
	, m_nItemCountCheck(0)
	, m_bRunning(FALSE)
	, m_hThread(NULL)
	, m_bInit(FALSE)
	, m_nSYear(_T(""))
	, m_nSMonth(_T(""))
	, m_nSDay(_T(""))
	, m_nEYear(_T(""))
	, m_nEMonth(_T(""))
	, m_nEDay(_T(""))
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CItemCount::~CItemCount()
{
	if (m_bRunning == TRUE) // 작업중
	{
		m_bRunning = FALSE; // 작업을 중단시킨다.
		Sleep( 1000 ); // 1 초대기
	}
}

void CItemCount::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_WEAPONS, m_nWeaponsCheck);
	DDX_Check(pDX, IDC_CHECK_DEFENSE, m_nDefenseCheck);
	DDX_Check(pDX, IDC_CHECK_ITEMCOUNT, m_nItemCountCheck);

	DDX_Control(pDX, IDC_LIST_WEAPONS, m_ListCtrlWeapons);
	DDX_Control(pDX, IDC_LIST_DEFENSE, m_ListCtrlDefense);
	DDX_Control(pDX, IDC_LIST_ITEMCOUNT, m_ListCtrlCount);
	DDX_Control(pDX, IDC_COMBO_YEAR, m_SYear);
	DDX_Control(pDX, IDC_COMBO_MONTH, m_SMonth);
	DDX_Control(pDX, IDC_COMBO_DAY, m_SDay);
	DDX_Control(pDX, IDC_COMBO_YEAR_1, m_EYear);
	DDX_Control(pDX, IDC_COMBO_MONTH_1, m_EMonth);
	DDX_Control(pDX, IDC_COMBO_DAY_1, m_EDay);
	DDX_CBString(pDX, IDC_COMBO_YEAR, m_nSYear);
	DDX_CBString(pDX, IDC_COMBO_MONTH, m_nSMonth);
	DDX_CBString(pDX, IDC_COMBO_DAY, m_nSDay);
	DDX_CBString(pDX, IDC_COMBO_YEAR_1, m_nEYear);
	DDX_CBString(pDX, IDC_COMBO_MONTH_1, m_nEMonth);
	DDX_CBString(pDX, IDC_COMBO_DAY_1, m_nEDay);
	DDX_Control(pDX, IDC_EDIT_ITEM_COUNT_CONSOLE, m_EditCount);
}

BEGIN_MESSAGE_MAP(CItemCount, CFormView)

	ON_BN_CLICKED(IDC_CHECK_WEAPONS, OnBnClickedCheckWeapons)
	ON_BN_CLICKED(IDC_CHECK_DEFENSE, OnBnClickedCheckDefense)
	ON_BN_CLICKED(IDC_CHECK_ITEMCOUNT, OnBnClickedCheckItemcount)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_WEAPONS, OnNMDblclkListWeapons)	
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DEFENSE, OnNMDblclkListDefense)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ITEMCOUNT, OnNMDblclkListItemcount)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)	
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_WM_CLOSE()	
END_MESSAGE_MAP()


// CItemCount 진단입니다.

#ifdef _DEBUG
void CItemCount::AssertValid() const
{
	CFormView::AssertValid();
}

void CItemCount::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


void CItemCount::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.		
	m_ListCtrlWeapons.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ListCtrlDefense.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ListCtrlCount.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InitComboHead();	
	InitListHead();

	m_bInit = TRUE;
}

void CItemCount::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bRunning == TRUE) // 작업중
	{
		m_bRunning = FALSE; // 작업을 중단시킨다.
		Sleep( 1000 ); // 1 초대기
	}

	CFormView::OnClose();
}

void CItemCount::InitListHead()
{
	LVCOLUMN Col, Col1, Col2;
	
	// 무기류 +7이상 아이템 리스트
	// Date, +7Item, +8Item, +9Item, +10Item, +11Item, +12Item, +13Item, +14Item, +15Item
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 120;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= _T("Date");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(0, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("+7Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(1, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("+8Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(2, &Col);
    
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("+9Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(3, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("+10Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(4, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("+11Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(5, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("+12Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(6, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("+13Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(7, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("+14Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(8, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 78;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("+15Item");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListCtrlWeapons.InsertColumn(9, &Col);

	// 방어구 +7이상 아이템 리스트
	// Date, +7Item, +8Item, +9Item, +10Item, +11Item, +12Item, +13Item, +14Item, +15Item
	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 120;
	Col1.fmt		= LVCFMT_CENTER;		
	Col1.pszText	= _T("Date");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(0, &Col1);
	
	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;	
	Col1.pszText	= _T("+7Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(1, &Col1);
	
	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;
	Col1.pszText	= _T("+8Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(2, &Col1);
    
	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;
	Col1.pszText	= _T("+9Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(3, &Col1);

	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;
	Col1.pszText	= _T("+10Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(4, &Col1);

	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;
	Col1.pszText	= _T("+11Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(5, &Col1);

	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;
	Col1.pszText	= _T("+12Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(6, &Col1);

	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;
	Col1.pszText	= _T("+13Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(7, &Col1);

	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;
	Col1.pszText	= _T("+14Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(8, &Col1);

	Col1.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col1.cx			= 78;
	Col1.fmt		= LVCFMT_CENTER;
	Col1.pszText	= _T("+15Item");
	Col1.iSubItem	= 0;
	Col1.iImage		= 0;
	m_ListCtrlDefense.InsertColumn(9, &Col1);

	// 게임내 아이템 수량
	// Date, ItemMID,ItemSID, ItemName, Count
	Col2.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col2.cx			= 150;
	Col2.fmt		= LVCFMT_CENTER;		
	Col2.pszText	= _T("Date");
	Col2.iSubItem	= 0;
	Col2.iImage		= 0;
	m_ListCtrlCount.InsertColumn(0, &Col2);
	
	Col2.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col2.cx			= 100;
	Col2.fmt		= LVCFMT_CENTER;	
	Col2.pszText	= _T("ItemMID");
	Col2.iSubItem	= 0;
	Col2.iImage		= 0;
	m_ListCtrlCount.InsertColumn(1, &Col2);
	
	Col2.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col2.cx			= 100;
	Col2.fmt		= LVCFMT_CENTER;
	Col2.pszText	= _T("ItemSID");
	Col2.iSubItem	= 0;
	Col2.iImage		= 0;
	m_ListCtrlCount.InsertColumn(2, &Col2);
    
	Col2.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col2.cx			= 100;
	Col2.fmt		= LVCFMT_CENTER;
	Col2.pszText	= _T("ItemName");
	Col2.iSubItem	= 0;
	Col2.iImage		= 0;
	m_ListCtrlCount.InsertColumn(3, &Col2);

	Col2.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col2.cx			= 100;
	Col2.fmt		= LVCFMT_CENTER;
	Col2.pszText	= _T("Count");
	Col2.iSubItem	= 0;
	Col2.iImage		= 0;
	m_ListCtrlCount.InsertColumn(4, &Col2);
}

void CItemCount::InitComboHead()
{

	////////////////////////////////////////////////////////////////////////////////////////////
	// 시작날짜 
	// 시작날짜 Year 콤보박스에 대한 설정
	CString strTemp;
	for( int i=2004; i<2021; ++i )
	{
		strTemp.Format("%d", i);
		m_SYear.AddString(strTemp);
		m_SYear.SetItemData(i, i);		
	}
	m_SYear.SetCurSel(0);

	// 시작날짜 Month 콤보박스에 대한 설정
	for( int i=1; i<13; ++i )
	{
		strTemp.Format("%d", i);
		m_SMonth.AddString(strTemp);
		m_SMonth.SetItemData(i, i);		
	}
	m_SMonth.SetCurSel(0);

	// 시작날짜 Day 콤보박스에 대한 설정
	for( int i=1; i<32; ++i )
	{
		strTemp.Format("%d", i);
		m_SDay.AddString(strTemp);
		m_SDay.SetItemData(i, i);		
	}
	m_SDay.SetCurSel(0);

	////////////////////////////////////////////////////////////////////////////////////////////
	// 종료날짜 
	// 종료날짜 Year 콤보박스에 대한 설정
	for( int i=2004; i<2021; ++i )
	{
		strTemp.Format("%d", i);
		m_EYear.AddString(strTemp);
		m_EYear.SetItemData(i, i);
	}
	m_EYear.SetCurSel(0);

	// 종료날짜 Month 콤보박스에 대한 설정
	for( int i=1; i<13; ++i )
	{
		strTemp.Format("%d", i);
		m_EMonth.AddString(strTemp);
		m_EMonth.SetItemData(i, i);
	}
	m_EMonth.SetCurSel(0);

	// 종료날짜 Day 콤보박스에 대한 설정
	for( int i=1; i<32; ++i )
	{
		strTemp.Format("%d", i);
		m_EDay.AddString(strTemp);
		m_EDay.SetItemData(i, i);
	}
	m_EDay.SetCurSel(0);
}

// 무기류 아이템 검색을 체크하였을 경우
void CItemCount::OnBnClickedCheckWeapons()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (((CButton*)GetDlgItem(IDC_CHECK_WEAPONS))->GetCheck())
	{
		m_nWeaponsCheck = 1;
	}
	else
	{
		m_nWeaponsCheck = 0;
	}
}

// 방어구 아이템 검색을 체크하였을 경우
void CItemCount::OnBnClickedCheckDefense()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (((CButton*)GetDlgItem(IDC_CHECK_DEFENSE))->GetCheck())
	{
		m_nDefenseCheck = 1;
	}
	else
	{
		m_nDefenseCheck = 0;
	}
}

// 아이템 Count일 체크하였을 경우
void CItemCount::OnBnClickedCheckItemcount()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (((CButton*)GetDlgItem(IDC_CHECK_ITEMCOUNT))->GetCheck())
	{
		m_nItemCountCheck = 1;
	}
	else
	{
		m_nItemCountCheck = 0;
	}
}

void CItemCount::OnNMDblclkListWeapons(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CItemCount::OnNMDblclkListDefense(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CItemCount::OnNMDblclkListItemcount(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

// 아이템 검색 시작 버튼에 대한 처리
void CItemCount::OnBnClickedButton1()
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
		ItemSearchStart();	
	}	
}

// 아이템 검색 종료 버튼에 대한 처리
void CItemCount::OnBnClickedButtonStop()
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

void CItemCount::ItemSearchStart()
{
	DWORD dwThreadID = 130;

	m_hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0,
								ItemCountThread,
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
		strGmCmd << "Item Count Search ";
		strGmCmd << '\0';
		m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
		// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		strGmCmd.freeze( false );
		///////////////////////////////////////////////////////////////////////////
	}
}

void CItemCount::SearchProc()
{
	int nTotalSize = 0;
	int nCurrent   = 0;
	int nCount     = 0;

	std::vector< std::vector<int> > vecCon;
	std::vector< std::vector<CString> > vecCon1;

	for ( int i = 0 ; i < 301 ; ++i )
	{
		std::vector<int> element;
		element.resize(301);
		vecCon.push_back(element);
	}

	for ( int i = 0 ; i < 301 ; ++i )
	{
		std::vector<CString> element1;
		element1.resize(301);
		vecCon1.push_back(element1);
	}

	for( int i = 0; i < 301; ++i )
	{
		for( int j = 0; j < 301; j++ )
		{
			vecCon[i][j] = 0;
			vecCon1[i][j] = "";
		}
	}

	m_bRunning = TRUE;
	LV_ITEM lvItem;

	int						nSYear;
	int						nSMonth;
	int						nSDay;

	int						nEYear;
	int						nEMonth;
	int						nEDay;

	// 무기류 아이템에 대한 통계치 변수
	int						nWPlus7 = 0;
	int						nWPlus8 = 0;
	int						nWPlus9 = 0;
	int						nWPlus10 = 0;
	int						nWPlus11 = 0;
	int						nWPlus12 = 0;
	int						nWPlus13 = 0;
	int						nWPlus14 = 0;
	int						nWPlus15 = 0;

	// 방어구 아이템에 대한 통계치 변수
	int						nDPlus7 = 0;
	int						nDPlus8 = 0;
	int						nDPlus9 = 0;
	int						nDPlus10 = 0;
	int						nDPlus11 = 0;
	int						nDPlus12 = 0;
	int						nDPlus13 = 0;
	int						nDPlus14 = 0;
	int						nDPlus15 = 0;
	
	// 아이템 검색이 시작되면 모든 체크 박스를 비활성화를 만든다.
	GetDlgItem(IDC_CHECK_WEAPONS)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_DEFENSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ITEMCOUNT)->EnableWindow(FALSE);

	// Edit박스 설정
	m_EditCount.Clear();
	m_EditCount.SetLimitText(0);

	// 콤보박스에 선택한 값 읽어오기.
	// 시작날짜에 대한 값 읽어오기
	nSYear = atoi(m_nSYear);
	nSMonth = atoi(m_nSMonth);
	nSDay = atoi(m_nSDay);
	
	// 종료날짜에 대한 값 읽어오기
	nEYear = atoi(m_nEYear);
	nEMonth = atoi(m_nEMonth);
	nEDay = atoi(m_nEDay);

	// 전체 리스트 컨트롤러의 표시된 정보 초기화
	m_ListCtrlWeapons.DeleteAllItems();
	m_ListCtrlDefense.DeleteAllItems();
	m_ListCtrlCount.DeleteAllItems();

	// 시작날짜와 종료 날짜에 대한 확인
	if( nSYear > nEYear ) 
	{
		MessageBox(_T("Check The Year Data"));
		m_bRunning = FALSE;
	}

	if( nEYear == nSYear && nSMonth > nEMonth )
	{
		MessageBox(_T("Check The Month Data"));
		m_bRunning = FALSE;
	}

	if( nEYear == nSYear && nSMonth == nEMonth && nSDay > nEDay )
	{
		MessageBox(_T("Check The Day Data"));
		m_bRunning = FALSE;
	}

	CString strData;

	strData.Format("---- Search Start ----\r\n");
	AddString ( strData );

	strData.Format("Get All Character Number\r\n");
	AddString ( strData );
	
	std::vector<AllCharNum> vChar;
	m_pDBMan->GetAllChar(vChar);

	strData.Format("Total Character Number : %d\r\n", (int) vChar.size());
	AddString ( strData );

	// 전체 인벤 숫자를 가져온다
	strData.Format("Get All Inventory Number\r\n");
	AddString ( strData );

	std::vector<AllCharNum> vInven;
	m_pDBMan->GetAllInven(vInven);
	
	strData.Format("Total User Inventory Number : %d\r\n", (int) vInven.size());	
	AddString ( strData );
    
	nTotalSize = (int) vChar.size() + (int) vInven.size();

	CLogFile LogFile(CString("ItemCount"));
	LogFile.Write("Date, ItemMID, ItmeSID, ItemName, Count");


	__time64_t tBeginTime = CTime::GetCurrentTime().GetTime();
	__time64_t tSpanTime;
	__time64_t tCurTime;
	int nHour, nMin, nSec;
	CString strTime;
	

	///////////////////////////////////////////////////////////////////////////
	// 루프를 돌면서 캐릭터 하나씩 조사한다.

	for(int i=0; i<(int) vChar.size(); i++)	
	{
		GLCHARLOGIC pChar;
		if (m_bRunning == FALSE)
		{
			break;
		}
		
		tCurTime = CTime::GetCurrentTime().GetTime();
		tSpanTime = tCurTime - tBeginTime;
		nHour = static_cast<int> (tSpanTime / 3600);
		nMin = static_cast<int> ((tSpanTime % 3600) / 60);
		nSec = static_cast<int> (tSpanTime % 60);

		strTime.Format( "%02d:%02d:%02d",nHour,nMin,nSec );
		SetDlgItemText( IDC_STATIC_TIME, strTime );

		strData.Format("Character %d %s\r\n", vChar[i].ChaNum, vChar[i].ChaName.GetString());
		AddString ( strData );
		
		if (m_pDBMan->GetCharacterInfo(vChar[i].ChaNum, vChar[i].UserNum, &pChar) == DB_ERROR)
		{
            continue;
		}
		
		// 착용아이템조사
		int nMaxSize = EMSLOT(SLOT_TSIZE);

		// 무기류, 방어류 아이템 검색 체크 버튼에 체크를 하였을 경우...
		if( 1 == m_nWeaponsCheck || 1 == m_nDefenseCheck || 1 == m_nItemCountCheck )
		{
			for (int i2=0; i2<nMaxSize; i2++)
			{
				// 아이템 생성시간.
				CTime borntime = pChar.m_PutOnItems[i2].tBORNTIME;
				if( borntime.GetYear() >= nSYear && borntime.GetYear() <= nEYear &&
							borntime.GetMonth() >= nSMonth && borntime.GetMonth() >= nEMonth &&
									borntime.GetDay() >= nSDay && borntime.GetDay() >= nEDay )
				{
					if( 1 == m_nWeaponsCheck )
					{
						// 아이템 공격 속성확인
						if( 7 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus7++;
						if( 8 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus8++;
						if( 9 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus9++;
						if( 10 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus10++;
						if( 11 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus11++;
						if( 12 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus12++;
						if( 13 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus13++;
						if( 14 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus14++;
						if( 15 == pChar.m_PutOnItems[i2].cDAMAGE ) nWPlus15++;
					}

					if( 1 == m_nDefenseCheck )
					{
						// 아이템 방어 속성
						if( 7 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus7++;
						if( 8 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus8++;
						if( 9 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus9++;
						if( 10 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus10++;
						if( 11 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus11++;
						if( 12 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus12++;
						if( 13 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus13++;
						if( 14 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus14++;
						if( 15 == pChar.m_PutOnItems[i2].cDEFENSE ) nDPlus15++;
					}

					if( 1 == m_nItemCountCheck )
					{						
						// 아이템 정보 받아오기.
						SITEM *pItemData = GLogicData::GetInstance().GetItem(pChar.m_PutOnItems[i2].GetNativeID());
						if( pItemData != NULL )
						{
							CString strName = pItemData->GetName();
							DWORD wMain = pChar.m_PutOnItems[i2].Mid();
							DWORD wSub = pChar.m_PutOnItems[i2].Sid();

							vecCon[wMain][wSub] += 1;
							int temp = vecCon[wMain][wSub];
							if( temp == 1 )
							{
								vecCon1[wMain][wSub] = strName;
								CString temp = vecCon1[wMain][wSub];
							}
						}						
					}
				}
			}
		}		

		// 캐릭터 인벤조사
		EMCHARINDEX emCharIndex = CharClassToIndex ( pChar.m_emClass );

		const GLInventory::CELL_MAP* pItemMap2 = pChar.m_cInventory.GetItemList();
		GLInventory::CELL_MAP_CITER iter2 = pItemMap2->begin();
		GLInventory::CELL_MAP_CITER iter_end2 = pItemMap2->end();
		
		// MakeNum을 검색조건에 추가할때
		if( 1 == m_nWeaponsCheck || 1 == m_nDefenseCheck || 1 == m_nItemCountCheck )
		{
			for ( ; iter2!=iter_end2; ++iter2 )
			{
				const SINVENITEM &sInvenItem = *(*iter2).second;

				CTime borntime1 = sInvenItem.sItemCustom.tBORNTIME;
				if( borntime1.GetYear() >= nSYear && borntime1.GetYear() <= nEYear &&
							borntime1.GetMonth() >= nSMonth && borntime1.GetMonth() >= nEMonth &&
									borntime1.GetDay() >= nSDay && borntime1.GetDay() >= nEDay )
				{
					SITEM *pItemData = GLogicData::GetInstance().GetItem ( sInvenItem.GetNativeID() );
					if (pItemData != NULL) 
					{
						if( 1 == m_nWeaponsCheck )
						{
							// 아이템 공격 속성확인
							if( 7 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus7++;
							if( 8 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus8++;
							if( 9 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus9++;
							if( 10 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus10++;
							if( 11 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus11++;
							if( 12 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus12++;
							if( 13 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus13++;
							if( 14 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus14++;
							if( 15 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus15++;
						}

						if( 1 == m_nDefenseCheck )
						{
							// 아이템 방어 속성
							if( 7 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus7++;
							if( 8 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus8++;
							if( 9 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus9++;
							if( 10 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus10++;
							if( 11 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus11++;
							if( 12 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus12++;
							if( 13 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus13++;
							if( 14 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus14++;
							if( 15 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus15++;
						}

						if( 1 == m_nItemCountCheck )
						{
							
							CString strName1 = pItemData->GetName();
							DWORD wMain = sInvenItem.sItemCustom.Mid();
							DWORD wSub = sInvenItem.sItemCustom.Sid();

							vecCon[wMain][wSub] += 1;
							int temp = vecCon[wMain][wSub];
							if( temp == 1 )
							{
								vecCon1[wMain][wSub] = strName1;
							}
						}						
					}
				}
			}
		}		
		Sleep( 1 );
	}

	///////////////////////////////////////////////////////////////////////////
	// 루프를 돌면서 사용자인벤을 하나씩 조사한다.
	for(int i=0; i<(int) vInven.size(); i++)
	{
		GLCHARLOGIC pChar;
		if (m_bRunning == FALSE)
		{
			break;
		}
		/*
		nCurrent++;
		nPercent = (int) (nCurrent*100)/nTotalSize;
		m_SearchProgress.SetPos(nPercent);
		CString strTemp;
		strTemp.Format("%d/%d (%d%%)", nCurrent, nTotalSize, nPercent);
		SetDlgItemText(IDC_SEARCHSTATIC1, strTemp);
		*/

		tCurTime = CTime::GetCurrentTime().GetTime();
		tSpanTime = tCurTime - tBeginTime;
		nHour = static_cast<int> (tSpanTime / 3600);
		nMin = static_cast<int> ((tSpanTime % 3600 ) / 60);
		nSec = static_cast<int> (tSpanTime % 60);

		strTime.Format( "%02d:%02d:%02d",nHour,nMin,nSec );
		SetDlgItemText( IDC_STATIC_TIME, strTime );

		strData.Format("User Inventory %d %s\r\n", vInven[i].ChaNum, vInven[i].ChaName.GetString());
		AddString ( strData );

		// 사용자 인벤조사
		// 창고는 총 5 개로 구성되어 있다.
		// 현재는 하나만 사용. (0 번만 사용한다)
		// 추후에 늘어날 경우 0 에서 4 번까지 사용가능
		CString ChaName;

		pChar.SetUserID( (DWORD) vInven[i].UserNum );
		pChar.m_CharDbNum = (DWORD) vInven[i].ChaNum;
        pChar.SetName(vInven[i].ChaName.GetString());
        
		// 인벤토리를 가져온다.
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

			// MakeNum을 검색조건에 추가할때
			if( 1 == m_nWeaponsCheck || 1 == m_nDefenseCheck || 1 == m_nItemCountCheck )
			{
				for ( ; iter!=iter_end; ++iter )
				{
					const SINVENITEM &sInvenItem = *(*iter).second;

					CTime borntime2 = sInvenItem.sItemCustom.tBORNTIME;
					if( borntime2.GetYear() >= nSYear && borntime2.GetYear() <= nEYear &&
							borntime2.GetMonth() >= nSMonth && borntime2.GetMonth() >= nEMonth &&
									borntime2.GetDay() >= nSDay && borntime2.GetDay() >= nEDay )
					{
						SITEM *pItemData = GLogicData::GetInstance().GetItem ( sInvenItem.GetNativeID() );
						if (pItemData != NULL)
						{					
							if( 1 == m_nWeaponsCheck )
							{
								// 아이템 공격 속성확인
								if( 7 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus7++;
								if( 8 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus8++;
								if( 9 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus9++;
								if( 10 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus10++;
								if( 11 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus11++;
								if( 12 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus12++;
								if( 13 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus13++;
								if( 14 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus14++;
								if( 15 == sInvenItem.sItemCustom.cDAMAGE ) nWPlus15++;
							}

							if( 1 == m_nDefenseCheck )
							{
								// 아이템 방어 속성
								if( 7 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus7++;
								if( 8 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus8++;
								if( 9 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus9++;
								if( 10 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus10++;
								if( 11 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus11++;
								if( 12 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus12++;
								if( 13 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus13++;
								if( 14 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus14++;
								if( 15 == sInvenItem.sItemCustom.cDEFENSE ) nDPlus15++;
							}

							if( 1 == m_nItemCountCheck )
							{

								CString strName2 = pItemData->GetName();
								DWORD wMain = sInvenItem.sItemCustom.Mid();
								DWORD wSub = sInvenItem.sItemCustom.Sid();

								vecCon[wMain][wSub] += 1;						
								int temp = vecCon[wMain][wSub];
								if( temp == 1 )
								{
									vecCon1[wMain][wSub] = strName2;
								}
							}
						}
					}
					Sleep( 1 );
				}
			}
			Sleep( 1 );
		}
		Sleep( 1 );
	}
	// 무기류, 방어구 아이템 Count에 대한 리스트 출력
	if( 1 == m_nWeaponsCheck )
	{
		CString strTemp;
		int nCount = m_ListCtrlWeapons.GetItemCount();
		::memset(&lvItem, 0, sizeof(LV_ITEM));
		lvItem.mask = LVIF_TEXT; // 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
		strTemp.Format("%d/%d/%d - %d/%d/%d", nSYear, nSMonth, nSDay, nEYear, nEMonth, nEDay );
		lvItem.pszText = strTemp.GetBuffer();
		m_ListCtrlWeapons.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
		strTemp.Format("%d", nWPlus7 );
		m_ListCtrlWeapons.SetItemText(nCount, 1, strTemp);
		strTemp.Format("%d", nWPlus8 );
		m_ListCtrlWeapons.SetItemText(nCount, 2, strTemp);
		strTemp.Format("%d", nWPlus9 );
		m_ListCtrlWeapons.SetItemText(nCount, 3, strTemp);
		strTemp.Format("%d", nWPlus10 );
		m_ListCtrlWeapons.SetItemText(nCount, 4, strTemp);
		strTemp.Format("%d", nWPlus11 );
		m_ListCtrlWeapons.SetItemText(nCount, 5, strTemp);
		strTemp.Format("%d", nWPlus12 );
		m_ListCtrlWeapons.SetItemText(nCount, 6, strTemp);
		strTemp.Format("%d", nWPlus13 );
		m_ListCtrlWeapons.SetItemText(nCount, 7, strTemp);
		strTemp.Format("%d", nWPlus14 );
		m_ListCtrlWeapons.SetItemText(nCount, 8, strTemp);
		strTemp.Format("%d", nWPlus15 );
		m_ListCtrlWeapons.SetItemText(nCount, 9, strTemp);
	}

	 if( 1 == m_nDefenseCheck )
	 {
        CString strTemp1;
		int nCount1 = m_ListCtrlDefense.GetItemCount();
		::memset(&lvItem, 0, sizeof(LV_ITEM));
		lvItem.mask = LVIF_TEXT; // 속성 지정
		lvItem.iItem = nCount1;
		lvItem.iSubItem = 0;
		strTemp1.Format("%d/%d/%d - %d/%d/%d", nSYear, nSMonth, nSDay, nEYear, nEMonth, nEDay );
		lvItem.pszText = strTemp1.GetBuffer();
		m_ListCtrlDefense.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
		strTemp1.Format("%d", nDPlus7 );
		m_ListCtrlDefense.SetItemText(nCount1, 1, strTemp1);
		strTemp1.Format("%d", nDPlus8 );
		m_ListCtrlDefense.SetItemText(nCount1, 2, strTemp1);
		strTemp1.Format("%d", nDPlus9 );
		m_ListCtrlDefense.SetItemText(nCount1, 3, strTemp1);
		strTemp1.Format("%d", nDPlus10 );
		m_ListCtrlDefense.SetItemText(nCount1, 4, strTemp1);
		strTemp1.Format("%d", nDPlus11 );
		m_ListCtrlDefense.SetItemText(nCount1, 5, strTemp1);
		strTemp1.Format("%d", nDPlus12 );
		m_ListCtrlDefense.SetItemText(nCount1, 6, strTemp1);
		strTemp1.Format("%d", nDPlus13 );
		m_ListCtrlDefense.SetItemText(nCount1, 7, strTemp1);
		strTemp1.Format("%d", nDPlus14 );
		m_ListCtrlDefense.SetItemText(nCount1, 8, strTemp1);
		strTemp1.Format("%d", nDPlus15 );
		m_ListCtrlDefense.SetItemText(nCount1, 9, strTemp1);
	 }

	 if( 1 == m_nItemCountCheck )
	 {        
		for( int a = 0; a < 301; ++a )
		{
			for( int b = 0; b < 301; ++b )
			{
				if( 0 != vecCon[a][b] )
				{
					CString strTemp2;
					CString temp = vecCon1[a][b];
					int nCount2 = m_ListCtrlCount.GetItemCount();
					::memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT; // 속성 지정
					lvItem.iItem = nCount2;
					lvItem.iSubItem = 0;
					strTemp2.Format("%d/%d/%d - %d/%d/%d", nSYear, nSMonth, nSDay, nEYear, nEMonth, nEDay );
					lvItem.pszText = strTemp2.GetBuffer();
					m_ListCtrlCount.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.

					strTemp2.Format("%d", a );
					m_ListCtrlCount.SetItemText(nCount2, 1, strTemp2);
					strTemp2.Format("%d", b );
					m_ListCtrlCount.SetItemText(nCount2, 2, strTemp2);
					strTemp2.Format("%s", temp );
					m_ListCtrlCount.SetItemText(nCount2, 3, strTemp2);
					strTemp2.Format("%d", vecCon[a][b] );
					m_ListCtrlCount.SetItemText(nCount2, 4, strTemp2);

					CString strTemp3;
					strTemp3.Format("%d/%d/%d - %d/%d/%d", nSYear, nSMonth, nSDay, nEYear, nEMonth, nEDay );
					LogFile.Write("%s,%d,%d,%s,%d", strTemp3.GetString(), a, b, vecCon1[a][b].GetString(), vecCon[a][b]);
				}
			}
		}
	 }

	strData.Format("---- Search End ----\r\n");
	AddString ( strData );
	m_bRunning = FALSE;

	GetDlgItem(IDC_CHECK_WEAPONS)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_DEFENSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_ITEMCOUNT)->EnableWindow(TRUE);
}

void CItemCount::AddString ( const char* szData )
{
	int len = m_EditCount.GetWindowTextLength();

	m_EditCount.SetSel( len, len );
	m_EditCount.ReplaceSel( szData );	

}