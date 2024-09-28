// FormRight.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VersionManager.h"
#include "FormRight.h"
#include "MainFrm.h"
// #include "MIN_CAB_UTIL.h"
#include "DlgCompress.h"
#include "CCfg.h"
// CFormRight

IMPLEMENT_DYNCREATE(CFormRight, CFormView)

CFormRight::CFormRight()
	: CFormView(CFormRight::IDD)
{
	m_bInit = FALSE;
	nSelectedItem = 0;
	m_nDir = 0;
}

CFormRight::~CFormRight()
{
}

void CFormRight::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_File);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}

BEGIN_MESSAGE_MAP(CFormRight, CFormView)
	ON_WM_SIZE()	
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, OnCustomdraw)
	ON_COMMAND(ID_RIGHT_ADD, OnRightAdd)
	ON_COMMAND(ID_RIGHT_EDIT, OnRightEdit)
	ON_COMMAND(ID_RIGHT_DELETE, OnRightDelete)
	ON_COMMAND(ID_RIGHT_PREDOWN_MENU0, OnRightIsPreDownSet0)
	ON_COMMAND(ID_RIGHT_PREDOWN_MENU1, OnRightIsPreDownSet1)
	ON_COMMAND(ID_RIGHT_PREDOWN_MENU2, OnRightIsPreDownSet2)
	ON_COMMAND(ID_RIGHT_PREDOWN_MENU3, OnRightIsPreDownSet3)
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()


// CFormRight 진단입니다.

#ifdef _DEBUG
void CFormRight::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormRight::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CFormRight 메시지 처리기입니다.

void CFormRight::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_File.SetExtendedStyle ( LVS_EX_FULLROWSELECT ); 
	InitListHead(); // 리스트 컨트롤 헤더 초기화
	InitListData(); // 리스트 컨트롤 Data 초기화

	m_bInit = TRUE;
}

// 리스트 컨트롤 헤더 초기화
void CFormRight::InitListHead() 
{
	LVCOLUMN Col;
	CString strTemp;
	
	strTemp			= "File Name";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= strTemp.GetBuffer();
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_FILENAME, &Col);

	strTemp			= "Ver";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 40;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_VER, &Col);

	strTemp			= "Date";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 150;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_DATE, &Col);

#ifdef CHINAPARAM
	strTemp			= "MD5";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 270;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_MD5, &Col);
#else
	strTemp			= "";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 0;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_MD5, &Col);
#endif

    strTemp			= "FTP";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_FTP, &Col);

	strTemp			= "Description";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_DESCRIPTION, &Col);

	strTemp			= "IsPreDown";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 80;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_ISPREDOWN, &Col);

	strTemp			= "IsNew";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(COLUMN_ISNEW, &Col);
}

// 리스트 컨트롤 Data 초기화
void CFormRight::InitListData()
{
	FillData(m_nDir);
}

void CFormRight::FillData(int nFolder)
{
	// 해당 폴더의 파일리스트를 출력한다.
	int nRetCode;
	// Erase all list items
	m_File.DeleteAllItems();
	// Erase all vector elements
	m_vFile.erase(m_vFile.begin(), m_vFile.end());
	
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    IVersionManagerDb* pDb = pFrame->GetDbMan();
	VECTOR_NEWFILE vecNewFileName = pFrame->GetVecNewFileName();
	
    nRetCode = pDb->GetFileList(nFolder, m_vFile);

	if (nRetCode == DB_ERROR)
	{
		MessageBox(_T("파일리스트를 가져올 수 없습니다"), "ERROR", MB_ICONEXCLAMATION);
		return;
	}

	m_nDir = nFolder;

	for (int i=0; i<(int) m_vFile.size(); i++)
	{		
		int nCount; 
		CString strTemp;
		nCount = m_File.GetItemCount();
		bool bIsNew = false;

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));
		lvItem.mask = LVIF_TEXT; // 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;		
		lvItem.pszText = (&m_vFile[i])->strFile.GetBuffer();
		// lvItem.iImage = 0; // 이미지의 색인 번호
		m_File.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
		
		strTemp.Format("%d", (&m_vFile[i])->nVer);
		m_File.SetItemText(i, COLUMN_VER, strTemp.GetString());		

		m_File.SetItemText(i, COLUMN_DATE, (&m_vFile[i])->strDate.GetString());

#ifdef CHINAPARAM
		m_File.SetItemText( i, COLUMN_MD5, (&m_vFile[i])->strMD5.GetString() );
#else
		m_File.SetItemText( i, COLUMN_MD5, "" );
#endif        
		strTemp.Format("%d", (&m_vFile[i])->nFtp);
        m_File.SetItemText(i, COLUMN_FTP, strTemp);

		m_File.SetItemText(i, COLUMN_DESCRIPTION, (&m_vFile[i])->strDesc.GetString());

		strTemp.Format("%d", (&m_vFile[i])->nIsPreDown);
		m_File.SetItemText(i, COLUMN_ISPREDOWN, strTemp.GetString());

		// 새로 추가된 파일일 경우 컬럼에 NEW 스트링 추가 후;
		// OnCustomdraw 함수에서 줄 색상변경;
		for( int j=0; j<(int)vecNewFileName.size(); ++j )
		{
			int nIdx = vecNewFileName[j].first;
			CString	strFile = vecNewFileName[j].second;
			if( nIdx == m_vFile[i].nIdx && strFile == m_vFile[i].strFile )
			{
				m_File.SetItemText(i, COLUMN_ISNEW, "NEW");
				break;
			}
		}

	}

}

void CFormRight::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// 컨트롤 사이즈 조정
	if ( m_bInit && nType != SIZE_MINIMIZED )
	{		
		CRect cRect;
		GetClientRect(&cRect);
				
		CRect ctlRect;
		m_File.GetWindowRect(&ctlRect);
		m_Edit.MoveWindow(10, 5, cx-15, 25);
		m_File.MoveWindow(10, 30, cx-15, cy-30);
	}
}

void CFormRight::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 선택된 서버가 있는지 확인한다.
	//int nSelected = m_File.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	//
	//if (nSelected != -1) // 선택된 서버가 있음
	//{	
	//	// 선택된 폴더를 에디터 박스에 출력한다.
	//	m_Edit.SetWindowText((&m_vFile[nSelected])->strFile.GetString());
	//}
	*pResult = 0;
}

void CFormRight::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelected = m_File.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // 선택된 서버가 있음
	{	
		// 선택된 폴더를 에디터 박스에 출력한다.
		m_Edit.SetWindowText((&m_vFile[nSelected])->strFile.GetString());
	}
	*pResult = 0;
}

void CFormRight::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelected = m_File.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	ASSERT(nSelected != -1);
	CString strNew = m_File.GetItemText(nSelected, COLUMN_ISNEW);
	bool bIsNew = !strcmp( strNew.GetString(), "NEW" );
	
	// 마우스 좌표 얻기
	POINT point;
	GetCursorPos(&point);
	
	// 메뉴 표시
	static CMenu menu;
	menu.LoadMenu (IDR_CONTEXT_RIGHT);
	CMenu* pContextMenu = menu.GetSubMenu (0);
	
	CMenu* pIsPreDownMenu = pContextMenu->GetSubMenu(3);
	int nIsPreDown = (&m_vFile[nSelected])->nIsPreDown;
	EnableIsPreDownMenu( pIsPreDownMenu, nIsPreDown, bIsNew );

    pContextMenu->TrackPopupMenu (
				TPM_LEFTALIGN |	TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
				point.x, point.y,
				AfxGetMainWnd ());	
	
	*pResult = 0;
}

void CFormRight::EnableIsPreDownMenu( CMenu *pIsPreDown, int nIsPreDown, bool bIsNew )
{
	pIsPreDown->EnableMenuItem( ID_RIGHT_PREDOWN_MENU0, 1);
	pIsPreDown->EnableMenuItem( ID_RIGHT_PREDOWN_MENU1, 1);
	pIsPreDown->EnableMenuItem( ID_RIGHT_PREDOWN_MENU2, 1);
	pIsPreDown->EnableMenuItem( ID_RIGHT_PREDOWN_MENU3, 1);

	if( bIsNew )
	{
		pIsPreDown->EnableMenuItem( ID_RIGHT_PREDOWN_MENU2, 0);
		return;
	}

	switch( nIsPreDown )
	{
	case PREDOWN_NONE:
		break;
	case PREDOWN_PATCH:
		pIsPreDown->EnableMenuItem( ID_RIGHT_PREDOWN_MENU0, 0);
		break;
	case PREDOWN_DOWN:
		pIsPreDown->EnableMenuItem( ID_RIGHT_PREDOWN_MENU1, 0);
		pIsPreDown->EnableMenuItem( ID_RIGHT_PREDOWN_MENU3, 0);
		break;
	case PREDOWN_DELETE:
		break;
	default:
		break;
	}
}

void CFormRight::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	*pResult = 0;

	if( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		CString strIsPreDown = m_File.GetItemText(nItem, COLUMN_ISPREDOWN);
		CString strIsNEW = m_File.GetItemText(nItem, COLUMN_ISNEW);

		if( 1 != strIsPreDown.GetLength() && strIsNEW.IsEmpty() )
			return;

		BYTE nIsPreDown = (BYTE)( *(strIsPreDown.GetBuffer()) - 0x30 );

		if( !strcmp( strIsNEW.GetString(), "NEW" ) )
		{
			pLVCD->clrTextBk = RGB(255, 200, 200);
			*pResult = CDRF_DODEFAULT;
			return;
		}

		switch( nIsPreDown )
		{
		case PREDOWN_NONE:
			pLVCD->clrTextBk = RGB(255, 255, 255);
			break;
		case PREDOWN_PATCH:
			pLVCD->clrTextBk = RGB(255, 255, 200);
			break;
		case PREDOWN_DOWN:
			pLVCD->clrTextBk = RGB(200, 255, 200);
			break;
		case PREDOWN_DELETE:
			pLVCD->clrTextBk = RGB(192, 192, 192);
			break;
		default:
			return;
		}

		*pResult = CDRF_DODEFAULT;

	}
}

// 파일추가
void CFormRight::OnRightAdd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    IVersionManagerDb* pDb = pFrame->GetDbMan();
    IVersionManConfig* pConfig = pFrame->GetConfig();

	// 사용하지 않는 기능이므로 삭제
	MessageBox("사용하지 않는 기능이므로 삭제되었습니다. FileList를 이용해서 추가하세요");
	/*
	std::vector<CompressFile> vCompressFile;
	CFileDialog DlgFile(TRUE, NULL,	NULL);
	DlgFile.m_ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_ENABLESIZING;
	
	if (DlgFile.DoModal() == IDOK ) 
	{
		POSITION pos = DlgFile.GetStartPosition();
		CString strTarget(pConfig->GetTargetPath());
		strTarget += pDb->GetFolderName(m_nDir);		

		while ( pos != NULL )
		{
			// 반복해서 strPathName 작업을 한다.
			CString strPathName = DlgFile.GetNextPathName(pos);
			CString strFileName = strPathName;
			int nPosition = strFileName.ReverseFind('\\');
			strFileName = strFileName.Mid(nPosition+1);
			strFileName += ".cab";

			// 완성된 src, target 을 vector 에 삽입			
			CompressFile sTemp;
			sTemp.strSrc		= strPathName;
			sTemp.strTarget		= strTarget;
			sTemp.strFileName	= strFileName;
			sTemp.nDir			= m_nDir;
			vCompressFile.push_back(sTemp);

            
			//// 압축해서 해당 디렉토리에 저장한다.
			//if (CAB_UTIL_MIN::MinMakeCab(strPathName.GetString(), strTarget.GetString()) == TRUE)
			//{ 
			//	// 같은 이름의 파일이 있는지 조사한다.
			//	BOOL bExist = pFrame->m_pOdbc->IsFileExist(strFileName);
			//	if (bExist)	pFrame->m_pOdbc->UpdateFile(strFileName);
			//	else 		pFrame->m_pOdbc->InsertFile(m_nDir, strFileName);
			//}
			//else
			//{
			//	MessageBox("압축실패");
			//}			
		}
		// 압축 대화상자를 띄운다.
		
		CDlgCompress DlgCompress(pFrame, vCompressFile);
		DlgCompress.DoModal();
	}
	// Right Refresh
	InitListData();
	*/
}

// 수정
void CFormRight::OnRightEdit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

// 삭제
void CFormRight::OnRightDelete()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
    TRACE("RIGHT DELETE \n");

    int nRetCode = MessageBox(_T("정말로 삭제하시겠습니까?"), _T("삭제 확인"), MB_OKCANCEL);

    if (nRetCode != 1)
    {
        return;
    }

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    IVersionManagerDb* pDb = pFrame->GetDbMan();
    IVersionManConfig* pConfig = pFrame->GetConfig();

    UINT i, uSelectedCount = m_File.GetSelectedCount();
    int  nItem = -1;
    
    CString strTarget(pConfig->GetTargetPath());
	strTarget += pDb->GetFolderName(m_nDir);

    // Check selected item
    if (uSelectedCount > 0)
    {
        for (i=0;i < uSelectedCount;i++)
        {
            nItem = m_File.GetNextItem(nItem, LVNI_SELECTED);
            ASSERT(nItem != -1);
            TRACE("SELECTED %d \n", nItem);

            // 파일을 지운다.
            CString strDelFullPath;
            strDelFullPath.Format("%s\\%s", strTarget, (&m_vFile[nItem])->strFile);

            if (DeleteFile(strDelFullPath) == 0)
            {                
                MessageBox(_T("삭제에 실패하였습니다"));
            }
            else
            {
                // DB 에서 삭제한다.
                if (pDb->DeleteFile((&m_vFile[nItem])->nIdx) == DB_ERROR)
                {
                    MessageBox(_T("파일은 삭제 되었지만 DB 삭제에 실패하였습니다"));
                }
            }
        }
        // Right View 를 리플래쉬 한다.
        InitListData();
    }
}

void CFormRight::OnRightIsPreDownSet0()
{
	TRACE("RIGHT IsPreDown 0 \n");

	int nRetCode = MessageBox(_T("IsPreDown 설정을 변경하시겠습니까?"), _T("확인"), MB_OKCANCEL);

	if (nRetCode != 1)
	{
		return;
	}

	SetIsPreDown( PREDOWN_NONE );
}

// 패치적용;
void CFormRight::OnRightIsPreDownSet1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TRACE("RIGHT IsPreDown 1 \n");

	int nRetCode = MessageBox(_T("IsPreDown 설정을 변경하시겠습니까?"), _T("확인"), MB_OKCANCEL);

	if (nRetCode != 1)
	{
		return;
	}

	SetIsPreDown( PREDOWN_PATCH );
}

// 사전다운로드;
void CFormRight::OnRightIsPreDownSet2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TRACE("RIGHT IsPreDown 2 \n");

	int nRetCode = MessageBox(_T("IsPreDown 설정을 변경하시겠습니까?"), _T("확인"), MB_OKCANCEL);

	if (nRetCode != 1)
	{
		return;
	}

	SetIsPreDown( PREDOWN_DOWN );
}

// 사전다운로드 파일 삭제;
void CFormRight::OnRightIsPreDownSet3()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TRACE("RIGHT IsPreDown 3 \n");

	int nRetCode = MessageBox(_T("IsPreDown 설정을 변경하시겠습니까?"), _T("확인"), MB_OKCANCEL);

	if (nRetCode != 1)
	{
		return;
	}

	SetIsPreDown( PREDOWN_DELETE );
}

void CFormRight::SetIsPreDown( const EM_PREDOWN &emIsPreDown )
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	IVersionManagerDb* pDb = pFrame->GetDbMan();
	IVersionManConfig* pConfig = pFrame->GetConfig();

	UINT i, uSelectedCount = m_File.GetSelectedCount();
	int  nItem = -1;
	int  nIsPreDown = (int)emIsPreDown;

	CString strTarget(pConfig->GetTargetPath());
	strTarget += pDb->GetFolderName(m_nDir);

	// Check selected item
	if (uSelectedCount > 0)
	{
		for (i=0;i < uSelectedCount;i++)
		{
			nItem = m_File.GetNextItem(nItem, LVNI_SELECTED);
			ASSERT(nItem != -1);
			TRACE("SELECTED %d \n", nItem);

			(&m_vFile[nItem])->nIsPreDown = nIsPreDown;

			// DB 에서 IsPreDown 변경;
			if (pDb->UpdateIsPreDown( (&m_vFile[nItem])->strFile, (&m_vFile[nItem])->nIsPreDown ) == DB_ERROR)
			{
				MessageBox(_T("DB isPreDown 변경에 실패하였습니다"));
			}
		}
		InitListData();
	}
}