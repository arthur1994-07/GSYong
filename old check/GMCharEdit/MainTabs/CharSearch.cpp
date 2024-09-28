// CharSearch.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "CharSearch.h"
#include "../MainFrm.h"

#include "../../RanLogicServer/DxServerInstance.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"
#include "../SubDialogs/DlgMakeCronCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CCharSearch

IMPLEMENT_DYNCREATE(CCharSearch, CFormView)

CCharSearch::CCharSearch()
	: CFormView(CCharSearch::IDD)
	, m_nRadio(CCharSearch::SEARCH_CHANAME)
	, m_bInit(FALSE)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CCharSearch::~CCharSearch()
{
}

void CCharSearch::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEARCH, m_ListSearch);
	DDX_Control(pDX, IDC_EDIT1, m_EditSearch);
}

BEGIN_MESSAGE_MAP(CCharSearch, CFormView)
	ON_BN_CLICKED(IDC_SEARCH, OnBnClickedSearch)
	ON_BN_CLICKED(IDC_RADIO_CHANAME, OnBnClickedRadioChaname)
	ON_BN_CLICKED(IDC_RADIO_CHANUM, OnBnClickedRadioChanum)
	ON_BN_CLICKED(IDC_RADIO_USERNUM, OnBnClickedRadioUsernum)
	ON_BN_CLICKED(IDC_RADIO_USERID, OnBnClickedRadioUserid)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SEARCH, OnLvnItemchangedListSearch)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SEARCH, OnNMDblclkListSearch)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SEARCH, OnNMRclickListSearch)
	ON_COMMAND(ID_CONTEXT1_MAKECRONCHARACTER, OnContext1Makecroncharacter)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SEARCH, OnNMCustomdrawListSearch)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCharSearch 진단입니다.

#ifdef _DEBUG
void CCharSearch::AssertValid() const
{
	CFormView::AssertValid();
}

void CCharSearch::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CCharSearch 메시지 처리기입니다.
void CCharSearch::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

	// cfg 로드성공, db 연결성공, 로그인 성공
	if (pFrame->m_bCfgLoad && pFrame->m_bDBConnect && pFrame->m_bLogin)
	{
		// 리스트 컨트롤 초기화
		m_ListSearch.SetExtendedStyle (LVS_EX_FULLROWSELECT);
        InitListHead();	
		m_bInit = TRUE;

		CButton* pButton = (CButton*) GetDlgItem(IDC_RADIO_CHANAME);
		pButton->SetCheck(1);
		m_nRadio = CCharSearch::SEARCH_CHANAME;

		UpdateData(FALSE);
	}
	else
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);		
	} 
}

void CCharSearch::SearchUserID(CString strID)
{
    int nRetCode;
	nRetCode = m_pDBMan->GetUserNum(strID);
    if (nRetCode == DB_ERROR) MessageBox(_T("Database Error"));

	m_vChaInfo.clear();    

	std::strstream strTemp;
	strTemp << "SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
	strTemp << "ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ChaDex, ";
	strTemp << "ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline, ChaDeleted ";
	strTemp << "From ChaInfo WHERE UserNum=" << nRetCode;
	strTemp << '\0';

    nRetCode = m_pDBMan->SearchCharacter(strTemp, m_vChaInfo);

    if (nRetCode == DB_ERROR) 
	{
        MessageBox(_T("Database Error"));
	}
    else
	{
		if (m_vChaInfo.size() == 0)
		{
			MessageBox(_T("No Result"));		
		}
		else
		{
			FillData();
		}
		m_strSearch = strTemp.str();
	}

	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}

void CCharSearch::SearchChaName(const CString& strChaName)
{
    int nRetCode;
		
    m_vChaInfo.clear();

    std::string ChaName(strChaName.GetString());
    sc::string::replaceForDb(ChaName);

    std::string Query;
	
	/*
	strTemp << " SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
    strTemp << " ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ";
    strTemp << " ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline, ChaDeleted ";
    strTemp << " From ChaInfo WHERE ChaName LIKE '%" << strChaName << "%'";
	strTemp << '\0';
	*/

    Query = sc::string::format("\
IF EXISTS(SELECT name FROM sysobjects WHERE name = N'ChaNameInfo' AND type = 'U') \
BEGIN \
    SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, \
        ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, \
        ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline, ChaDeleted \
    FROM dbo.ChaInfo \
    WHERE ChaNum IN (SELECT ChaNum FROM dbo.ChaNameInfo WHERE ChaName LIKE '%%%1%%%') \
    ORDER BY UserNum DESC \
END \
ELSE \
BEGIN \
    SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, \
        ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, \
        ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline, ChaDeleted \
    FROM dbo.ChaInfo \
    WHERE ChaName LIKE '%%%1%%%' \
    ORDER BY UserNum desc \
END",
        ChaName);

    nRetCode = m_pDBMan->SearchCharacter(Query, m_vChaInfo);

    if (nRetCode == DB_ERROR) 
	{
        MessageBox(_T("Database Error"));
	}
    else
	{
		if (m_vChaInfo.size() == 0)
		{
			MessageBox(_T("No Result"));		
		}
		else
		{
			FillData();
		}
		m_strSearch = Query.c_str();
	}	
}

void CCharSearch::SearchUserNum(int nUserNum)
{
	if (nUserNum == 0)
	{
		MessageBox(_T("Input must be numeric type"));
		return;
	}

    int nRetCode;
		
    m_vChaInfo.clear();

    std::strstream strTemp;
	strTemp << " SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
	strTemp << " ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ";
	strTemp << " ChaDex, ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline, ChaDeleted ";
	strTemp << " From ChaInfo WHERE UserNum=" << nUserNum;
	strTemp << '\0';

    nRetCode = m_pDBMan->SearchCharacter(strTemp, m_vChaInfo);

    if (nRetCode == DB_ERROR) 
	{
        MessageBox(_T("Database Error"));
	}
    else
	{
        if (m_vChaInfo.size() == 0)
		{
			MessageBox(_T("No Result"));		
		}
		else
		{
			FillData();
		}
		m_strSearch = strTemp.str();
	}

	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}

void CCharSearch::SearchChaNum(int nChaNum)
{
	if (nChaNum == 0)
	{
		MessageBox(_T("Input must be numeric type"));
		return;
	}

    int nRetCode;
		
    m_vChaInfo.clear();

	std::strstream strTemp;
	strTemp << "SELECT ChaNum, UserNum, ChaName, ChaTribe, ChaClass, SGNum, ";
	strTemp << "ChaLevel, ChaPower, ChaStrong, ChaStrength, ChaSpirit, ChaDex,";
	strTemp << "ChaIntel, ChaStRemain, ChaMoney, ChaExp, ChaOnline, Chadeleted ";
	strTemp << "From ChaInfo WHERE ChaNum=" << nChaNum;
	strTemp << '\0';

    nRetCode = m_pDBMan->SearchCharacter(strTemp, m_vChaInfo);

    if (nRetCode == DB_ERROR) 
	{
        MessageBox(_T("Database Error"));
	}
    else
	{
        if (m_vChaInfo.size() == 0)
		{
			MessageBox(_T("No Result"));		
		}
		else
		{
			FillData();
		}
		m_strSearch = strTemp.str();
	}

	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}

// 리스트 컨트롤 헤더 초기화
void CCharSearch::InitListHead() 
{
	LVCOLUMN Col;

	// Num, Name, Class, Tribe, Level,	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 70;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= "User Num";
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(0, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 70;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= _T("Cha Num");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(1, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 170;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Name");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(2, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Class");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(3, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 60;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Level");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(4, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 60;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Online");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(5, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 60;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Status");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(6, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Tribe");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(7, &Col);
}

void CCharSearch::FillData()
{
	// Erase all list items
	m_ListSearch.DeleteAllItems();
	
	for (int i=0; i<(int) m_vChaInfo.size(); i++)
	{		
        // Num, Name, Class, Tribe, Level,
		int nCount; 
		CString strTemp;
		nCount = m_ListSearch.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));

		lvItem.mask = LVIF_TEXT; // 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
		strTemp.Format("%d", m_vChaInfo[i].UserNum);
		lvItem.pszText = strTemp.GetBuffer();		
		m_ListSearch.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.

        strTemp.Format("%d", m_vChaInfo[i].ChaNum);
		m_ListSearch.SetItemText(i, 1, strTemp);
				
		m_ListSearch.SetItemText(i, 2, _T(m_vChaInfo[i].ChaName));
        
        strTemp.Format("%d", m_vChaInfo[i].ChaClass);
		m_ListSearch.SetItemText(i, 3, strTemp);        

        strTemp.Format("%d", m_vChaInfo[i].ChaLevel);
        m_ListSearch.SetItemText(i, 4, strTemp);

		if (m_vChaInfo[i].ChaOnline == 1)
			m_ListSearch.SetItemText(i, 5, _T("Online"));
		else
			m_ListSearch.SetItemText(i, 5, _T("Offline"));

		if (m_vChaInfo[i].ChaDeleted == 1)
			m_ListSearch.SetItemText(i, 6, _T("Deleted"));
		else
			m_ListSearch.SetItemText(i, 6, _T("OK"));

		strTemp.Format("%d", m_vChaInfo[i].ChaTribe);
        m_ListSearch.SetItemText(i, 7, strTemp);
	}
}

void CCharSearch::OnBnClickedSearch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

    CString strSearch;
    m_EditSearch.GetWindowText(strSearch);

	if (strSearch.GetLength() < 1)
	{
		MessageBox(_T("Please type search string"));
		return;
	}

    int nUserNum;
    int nChaNum;

    switch (m_nRadio)
    {
    case SEARCH_USERID:
        SearchUserID(strSearch);
        break;
    case SEARCH_CHANAME:        
        SearchChaName(strSearch);
        break;
    case SEARCH_USERNUM:
		nUserNum = ::atoi(strSearch.GetString());         
        SearchUserNum(nUserNum);
        break;
    case SEARCH_CHANUM: // 캐릭터번호로 검색
		nChaNum = ::atoi(strSearch.GetString());
        SearchChaNum(nChaNum);
        break;
    default:
        break;
    }
}

void CCharSearch::OnBnClickedRadioUserid()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_nRadio = CCharSearch::SEARCH_USERID;
}

void CCharSearch::OnBnClickedRadioChaname()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_nRadio = CCharSearch::SEARCH_CHANAME;
}

void CCharSearch::OnBnClickedRadioUsernum()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_nRadio = CCharSearch::SEARCH_USERNUM;
}

void CCharSearch::OnBnClickedRadioChanum()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_nRadio = CCharSearch::SEARCH_CHANUM;
}
void CCharSearch::OnLvnItemchangedListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CCharSearch::OnNMDblclkListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{	
    int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // 선택된 항목이 있음
	{
        CPropertySheetCharEdit dlg(m_vChaInfo[nSelected].ChaNum, m_vChaInfo[nSelected].UserNum, _T("Character Edit"));
        dlg.DoModal();
	}

    *pResult = 0;
}

void CCharSearch::OnNMRclickListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 선택된 캐릭터가 있는지 확인한다.
	int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // 선택된 캐릭터가 있음
	{	
		// 마우스 좌표 얻기
		POINT point;
		GetCursorPos(&point);

		// 메뉴 표시
		CMenu menuTemp, *pContextMenu;
		menuTemp.LoadMenu(IDR_CONTEXT_MAIN);
		pContextMenu = menuTemp.GetSubMenu(0);
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN,		
									point.x,          // 메뉴가 표시될 x 좌표
									point.y,          // 메뉴가 표시될 y 좌표
									AfxGetMainWnd()   // WM_COMMAND 메시지를 받을 윈도우
									);
	}
	*pResult = 0;
}

void CCharSearch::OnContext1Makecroncharacter()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_MASTER)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // 선택된 캐릭터가 있음
	{
		int nChaNum = m_vChaInfo[nSelected].ChaNum; // 캐릭터번호
		int nUserNum = 0; // 사용자번호
		CString strChaName;

		GLCHARLOGIC* pChar = new GLCHARLOGIC;

		if (m_pDBMan->GetCharacterInfo(nChaNum, m_vChaInfo[nSelected].UserNum, pChar) == DB_ERROR)
		{			
			MessageBox(_T("Database Error"));
		}
		else if (m_pDBMan->GetUserInven(pChar) == DB_ERROR)
		{		
			MessageBox(_T("Database Error"));
		}
		else
		{
			// 사용자번호 및 캐릭터 이름을 설정한다.
			CDlgMakeCronCharacter dlgTemp;			
			if (dlgTemp.DoModal() == IDOK)
			{
				// 캐릭터를 생성한다.
				// 캐릭터 초기 수치 설정.
				nUserNum   = dlgTemp.m_nUserNum;
				strChaName = dlgTemp.m_strChaName;

				GLCHARLOGIC NewCharLogic;
				EMCHARINDEX emIndex = CharClassToIndex(pChar->m_emClass);
				NewCharLogic.INIT_NEW_CHAR((EMCHARINDEX)emIndex, (DWORD) nUserNum, (DWORD)m_vChaInfo[nSelected].SGNum, strChaName.GetString(), pChar->m_wSchool, pChar->m_wHair, pChar->m_wFace, pChar->m_wHairColor, pChar->m_wSex);

				// 캐릭터 생성
                CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); // 서버 번호
				int nNewChaNum = m_pDBMan->CreateNewCharacter2(&NewCharLogic, pFrame->m_nSvrGrp);

				if (nNewChaNum == DB_ERROR)
				{
					MessageBox("Make character failed. check amount of user character / character name");
				}
				// 생성된 캐릭터에 이전 캐릭터 데이타를 복제해서 저장한다.
				pChar->m_CharDbNum = (DWORD) nNewChaNum;

				if (nNewChaNum > 0)
				{
					pChar->m_sSaveMapID = NATIVEID_NULL();
					m_pDBMan->SaveCharacterBase     (pChar);
					m_pDBMan->SaveCharacterInven    ( pChar, true );
					m_pDBMan->SaveCharacterPutOnItem( pChar, true );
					m_pDBMan->SaveCharacterSkill    (pChar);
					m_pDBMan->SaveCharacterQuest    (pChar);
					m_pDBMan->SaveCharacterSkillQuickSlot(pChar);
					m_pDBMan->SaveCharacterActionQuickSlot(pChar);
					m_pDBMan->SaveCharacterSkillFact(pChar);
					// 인벤토리 저장은 제외했음
					// m_pDBMan->SaveCharacterUserInven(pChar);

                    //! 기본 정보를 복사 한 후에 추가로 복사가 필요한 부분
                    m_pDBMan->CopyCharacterProductInfo( nChaNum, nNewChaNum );

				}
			}
		}
		delete pChar;
	}
}

void CCharSearch::OnNMCustomdrawListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// MFC 기본코드 삭제
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//*pResult = 0;	

	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*> (pNMHDR);
    *pResult = 0;

	// First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        // This is the notification message for an item.  We'll request
        // notifications before each subitem's prepaint stage.
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
    }
    else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
    {
        // This is the prepaint stage for a subitem. Here's where we set the
        // item's text and background colors. Our return value will tell 
        // Windows to draw the subitem itself, but it will use the new colors
        // we set here.
        // The text color will cycle through red, green, and light blue.
        // The background color will be light blue for column 0, red for
        // column 1, and black for column 2.    
        COLORREF crText, crBkgnd;
        
		crText  = RGB(0, 0, 0);
		crBkgnd = RGB(255, 255, 255);

		if (pLVCD->iSubItem == 5)
		{
			int nItemMain = static_cast<int> (pLVCD->nmcd.dwItemSpec);
			if (m_ListSearch.GetItemText(nItemMain, pLVCD->iSubItem) ==	_T("Online"))
			{
				crText  = RGB(  0,   0,   0);
				crBkgnd = RGB(  0, 204, 255);				
			}
		}
		else if (pLVCD->iSubItem == 6)
		{
			int nItemMain = static_cast<int> (pLVCD->nmcd.dwItemSpec);
			if (m_ListSearch.GetItemText(nItemMain, pLVCD->iSubItem) ==	_T("Deleted"))
			{
				crText  = RGB(  0,   0,   0);				
				crBkgnd = RGB(255, 102, 102);
			}
		}
		else
		{
		}
		
		// Store the colors back in the NMLVCUSTOMDRAW struct.
		pLVCD->clrText   = crText;
		pLVCD->clrTextBk = crBkgnd;
        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;
    }
}

void CCharSearch::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CCharSearch::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_bInit)
	{

		int nWidth  = cx - 5;
		int nHeight = cy - 110;

		if (nWidth < 0)  cx = 0;
		if (nHeight < 0) cy = 0;

		m_ListSearch.MoveWindow(0, // rect.left,
								100, // rect.top,
								nWidth,
								nHeight);
	}
}
