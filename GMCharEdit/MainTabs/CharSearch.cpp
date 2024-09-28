// CharSearch.cpp : ���� �����Դϴ�.
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


// CCharSearch �����Դϴ�.

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


// CCharSearch �޽��� ó�����Դϴ�.
void CCharSearch::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

	// cfg �ε强��, db ���Ἲ��, �α��� ����
	if (pFrame->m_bCfgLoad && pFrame->m_bDBConnect && pFrame->m_bLogin)
	{
		// ����Ʈ ��Ʈ�� �ʱ�ȭ
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

	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
}

// ����Ʈ ��Ʈ�� ��� �ʱ�ȭ
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

		lvItem.mask = LVIF_TEXT; // �Ӽ� ����
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
		strTemp.Format("%d", m_vChaInfo[i].UserNum);
		lvItem.pszText = strTemp.GetBuffer();		
		m_ListSearch.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
    case SEARCH_CHANUM: // ĳ���͹�ȣ�� �˻�
		nChaNum = ::atoi(strSearch.GetString());
        SearchChaNum(nChaNum);
        break;
    default:
        break;
    }
}

void CCharSearch::OnBnClickedRadioUserid()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_nRadio = CCharSearch::SEARCH_USERID;
}

void CCharSearch::OnBnClickedRadioChaname()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_nRadio = CCharSearch::SEARCH_CHANAME;
}

void CCharSearch::OnBnClickedRadioUsernum()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_nRadio = CCharSearch::SEARCH_USERNUM;
}

void CCharSearch::OnBnClickedRadioChanum()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_nRadio = CCharSearch::SEARCH_CHANUM;
}
void CCharSearch::OnLvnItemchangedListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CCharSearch::OnNMDblclkListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{	
    int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� �׸��� ����
	{
        CPropertySheetCharEdit dlg(m_vChaInfo[nSelected].ChaNum, m_vChaInfo[nSelected].UserNum, _T("Character Edit"));
        dlg.DoModal();
	}

    *pResult = 0;
}

void CCharSearch::OnNMRclickListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// ���õ� ĳ���Ͱ� �ִ��� Ȯ���Ѵ�.
	int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� ĳ���Ͱ� ����
	{	
		// ���콺 ��ǥ ���
		POINT point;
		GetCursorPos(&point);

		// �޴� ǥ��
		CMenu menuTemp, *pContextMenu;
		menuTemp.LoadMenu(IDR_CONTEXT_MAIN);
		pContextMenu = menuTemp.GetSubMenu(0);
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN,		
									point.x,          // �޴��� ǥ�õ� x ��ǥ
									point.y,          // �޴��� ǥ�õ� y ��ǥ
									AfxGetMainWnd()   // WM_COMMAND �޽����� ���� ������
									);
	}
	*pResult = 0;
}

void CCharSearch::OnContext1Makecroncharacter()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_MASTER)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� ĳ���Ͱ� ����
	{
		int nChaNum = m_vChaInfo[nSelected].ChaNum; // ĳ���͹�ȣ
		int nUserNum = 0; // ����ڹ�ȣ
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
			// ����ڹ�ȣ �� ĳ���� �̸��� �����Ѵ�.
			CDlgMakeCronCharacter dlgTemp;			
			if (dlgTemp.DoModal() == IDOK)
			{
				// ĳ���͸� �����Ѵ�.
				// ĳ���� �ʱ� ��ġ ����.
				nUserNum   = dlgTemp.m_nUserNum;
				strChaName = dlgTemp.m_strChaName;

				GLCHARLOGIC NewCharLogic;
				EMCHARINDEX emIndex = CharClassToIndex(pChar->m_emClass);
				NewCharLogic.INIT_NEW_CHAR((EMCHARINDEX)emIndex, (DWORD) nUserNum, (DWORD)m_vChaInfo[nSelected].SGNum, strChaName.GetString(), pChar->m_wSchool, pChar->m_wHair, pChar->m_wFace, pChar->m_wHairColor, pChar->m_wSex);

				// ĳ���� ����
                CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); // ���� ��ȣ
				int nNewChaNum = m_pDBMan->CreateNewCharacter2(&NewCharLogic, pFrame->m_nSvrGrp);

				if (nNewChaNum == DB_ERROR)
				{
					MessageBox("Make character failed. check amount of user character / character name");
				}
				// ������ ĳ���Ϳ� ���� ĳ���� ����Ÿ�� �����ؼ� �����Ѵ�.
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
					// �κ��丮 ������ ��������
					// m_pDBMan->SaveCharacterUserInven(pChar);

                    //! �⺻ ������ ���� �� �Ŀ� �߰��� ���簡 �ʿ��� �κ�
                    m_pDBMan->CopyCharacterProductInfo( nChaNum, nNewChaNum );

				}
			}
		}
		delete pChar;
	}
}

void CCharSearch::OnNMCustomdrawListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// MFC �⺻�ڵ� ����
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CCharSearch::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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
