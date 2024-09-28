// HelpEditDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "./HelpDataMan.h"

#include "HelpEdit.h"
#include "HelpEditDlg.h"
#include "assert.h"

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


// CHelpEditDlg 대화 상자



CHelpEditDlg::CHelpEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpEditDlg::IDD, pParent),
	m_bEdited ( FALSE )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHelpEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_HELP, m_ctrlHelpTree);
}

BEGIN_MESSAGE_MAP(CHelpEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_RCLICK, IDC_TREE_HELP, OnNMRclickTreeHelp)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_HELP, OnTvnEndlabeleditTreeHelp)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_HELP, OnTvnSelchangingTreeHelp)
	ON_EN_CHANGE(IDC_EDIT_CONTENT, OnEnChangeEditContent)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_HELP, OnTvnSelchangedTreeHelp)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)	
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_CSV_SAVE, OnCsvSave)
	ON_COMMAND(ID_CSV_LOAD, OnCsvLoad)
END_MESSAGE_MAP()


// CHelpEditDlg 메시지 처리기

BOOL CHelpEditDlg::OnInitDialog()
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

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CHelpEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHelpEditDlg::OnPaint() 
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
HCURSOR CHelpEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHelpEditDlg::OnNMRclickTreeHelp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POINT OriginPoint;
	GetCursorPos ( &OriginPoint );

	//	우클릭 강제 설정
	ForceSelectOnRightClick ( OriginPoint );

	//	메뉴 선택 & 선택 메뉴 실행
	RunSelectMenu ( GetSelectMenu ( OriginPoint ) );
}

void CHelpEditDlg::RunSelectMenu ( int nSelect )
{
	if ( nSelect < 0 ) return ;

	HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();

	switch ( nSelect )
	{
	case ID_MENUITEM_ADD_SIBLING:
		{
			HTREEITEM hParentItem = NULL;
			if ( hItem ) hParentItem = m_ctrlHelpTree.GetParentItem ( hItem );

			if ( hParentItem )	//	부모가 있는 경우에 형제로 단다.
			{
				CString strTemp;
				strTemp.Format ( "새 설명[%02d]", m_ctrlHelpTree.GetCount () );				
				HTREEITEM hNewItem = m_ctrlHelpTree.InsertItem ( strTemp, hParentItem );
				m_ctrlHelpTree.RedrawWindow ();

				SHELPNODE* pParent = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hParentItem );
                SHELPNODE* pNode = new SHELPNODE;
				pNode->strTitle = std::string ( strTemp.GetString () );
				pNode->strContents = pNode->strTitle;
				BOOL bResult = CHelpDataMan::GetInstance().AddNode ( pParent, pNode, "" );
				if ( !bResult ) assert ( 0 && "새 노드 달기 오류" );

				m_ctrlHelpTree.SetItemData ( hNewItem, (DWORD_PTR)pNode );
			}
			else
			{
				CString strTemp;
				strTemp.Format ( "새 설명[%02d]", m_ctrlHelpTree.GetCount () );				
				HTREEITEM hNewItem = m_ctrlHelpTree.InsertItem ( strTemp );
				m_ctrlHelpTree.RedrawWindow ();

                SHELPNODE* pNode = new SHELPNODE;
				pNode->strTitle = std::string ( strTemp.GetString () );
				pNode->strContents = pNode->strTitle;
				BOOL bResult = CHelpDataMan::GetInstance().AddNode ( NULL, pNode, "" );
				if ( !bResult ) assert ( 0 && "새 노드 달기 오류" );

				m_ctrlHelpTree.SetItemData ( hNewItem, (DWORD_PTR)pNode );
			}
		}
		break;

	case ID_MENUITEM_ADD_CHILD:
		{
			if ( hItem )
			{
				CString strTemp;
				strTemp.Format ( "새 설명[%02d]", m_ctrlHelpTree.GetCount () );				
				HTREEITEM hNewItem = m_ctrlHelpTree.InsertItem ( strTemp, hItem );
				m_ctrlHelpTree.RedrawWindow ();

				SHELPNODE* pParent = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );
                SHELPNODE* pNode = new SHELPNODE;
				pNode->strTitle = std::string ( strTemp.GetString () );
				pNode->strContents = pNode->strTitle;
				BOOL bResult = CHelpDataMan::GetInstance().AddNode ( pParent, pNode, "" );
				if ( !bResult ) assert ( 0 && "새 노드 달기 오류" );

				m_ctrlHelpTree.SetItemData ( hNewItem, (DWORD_PTR)pNode );
			}
		}
		break;

	case ID_MENUITEM_DEL:
		{
			if ( hItem )
			{
				CString strTitle = m_ctrlHelpTree.GetItemText ( hItem );
				CString strTemp = "\"" + strTitle + "\"" + "을(/를) 정말 삭제하시겠습니까?";
				if ( MessageBox ( strTemp, "질문", MB_YESNO | MB_ICONQUESTION ) == IDYES )
				{					
					HTREEITEM hParentItem = m_ctrlHelpTree.GetParentItem ( hItem );
					SHELPNODE* pParent = NULL;
					if ( hParentItem ) pParent = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hParentItem );
					SHELPNODE* pNode = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );

					CHelpDataMan::GetInstance().DelNode ( pParent, pNode->strTitle );

					UpdateTree ();
				}
			}
		}
		break;

	case ID_MENUITEM_CHANGE:
		{
			if ( hItem )
			{

			}
		}
		break;
	}
}

int	 CHelpEditDlg::GetSelectMenu ( POINT pt )
{
	//	<--	실제 작동 메뉴 따기
	int pMenuID = 0;
	CMenu Menu;

	int SelectionMade = -1;
	{
		VERIFY ( Menu.LoadMenu ( IDR_MENU_POPUP ) );

		CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
		ASSERT ( pPopup != NULL );	

		SelectionMade = pPopup->TrackPopupMenu (
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
			pt.x, pt.y, this );

		pPopup->DestroyMenu ();
	}

	return SelectionMade;
}

void CHelpEditDlg::ForceSelectOnRightClick ( POINT pt )
{		
	m_ctrlHelpTree.ScreenToClient( &pt );

	UINT uFlag = 0;
//	PITEMNODE pItemNode = NULL;
	HTREEITEM hItem = m_ctrlHelpTree.HitTest ( CPoint(pt), &uFlag );

 //   SetSelectItem ( NULL );	
	if ( hItem )
	{
//		m_ctrlHelpTree.SelectItem ( hItem );
		if ( uFlag & TVHT_ONITEMLABEL )
		{			
//			SetSelectItem ( (PITEMNODE)m_ctrlHelpTree.GetItemData ( hItem ) );
		}
	}
}

BOOL CHelpEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( pMsg )
	{
		switch ( pMsg->message )
		{
		case WM_KEYDOWN:
			{
				switch ( pMsg->wParam )
				{
				case VK_RETURN:
				case VK_ESCAPE:
					{
						return FALSE;
					}
					break;

				case VK_F2:
					{
						HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();
						if ( hItem )
						{
							m_ctrlHelpTree.EditLabel ( hItem );
	//						m_bEdited = TRUE;
						}

						return FALSE;
					}
					break;

				case VK_DELETE:
					{
						//	선택된 노드 삭제
						RunSelectMenu ( ID_MENUITEM_DEL );
					}
					break;
				}


				if ( GetKeyState(VK_CONTROL) & 0x8000 )
				{
					switch ( pMsg->wParam )
					{
					case VK_LEFT:
						{							
							//MessageBox ( "좌로~" );
						}
						break;

					case VK_RIGHT:
						{
							//MessageBox ( "우로~" );
						}
						break;

					case VK_UP:
						{
							HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();
							if ( hItem )
							{
								HTREEITEM hPrevItem = m_ctrlHelpTree.GetPrevSiblingItem ( hItem );								
								HTREEITEM hParentItem = m_ctrlHelpTree.GetParentItem ( hItem );

								SHELPNODE* pParent = NULL;
								if ( hParentItem ) pParent = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hParentItem );
								SHELPNODE* pNodeA = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );
								SHELPNODE* pNodeB = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hPrevItem );
								CHelpDataMan::GetInstance().ChangeNode ( pParent, pNodeA->strTitle, pNodeB->strTitle );

								UpdateTree ();

								SelectItem ( m_ctrlHelpTree.GetRootItem (), pNodeA );
							}
						}
						break;

					case VK_DOWN:
						{
							HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();
							if ( hItem )
							{
								HTREEITEM hNextItem = m_ctrlHelpTree.GetNextSiblingItem ( hItem );								
								HTREEITEM hParentItem = m_ctrlHelpTree.GetParentItem ( hItem );

								SHELPNODE* pParent = NULL;
								if ( hParentItem ) pParent = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hParentItem );
								SHELPNODE* pNodeA = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );
								SHELPNODE* pNodeB = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hNextItem );
								CHelpDataMan::GetInstance().ChangeNode ( pParent, pNodeA->strTitle, pNodeB->strTitle );

								UpdateTree ();

								SelectItem ( m_ctrlHelpTree.GetRootItem (), pNodeA );
							}
						}
						break;
					}

					return TRUE;
				}
			}
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL	CHelpEditDlg::SelectItem ( HTREEITEM hItem, SHELPNODE* pNode )
{
	SHELPNODE* pCurNode = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );
	if ( pCurNode == pNode )
	{
		m_ctrlHelpTree.SelectItem ( hItem );
		return TRUE;
	}

	HTREEITEM hChild = m_ctrlHelpTree.GetChildItem ( hItem );
	if ( hChild && SelectItem ( hChild, pNode ) ) return TRUE;		

	HTREEITEM hNextSibling = m_ctrlHelpTree.GetNextSiblingItem ( hItem );
	if ( hNextSibling && SelectItem ( hNextSibling, pNode ) ) return TRUE;	

	return FALSE;
}

void CHelpEditDlg::OnTvnEndlabeleditTreeHelp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();

	//	아이템이 정상적이고, 루트가 아닐때만 타이틀이 편집 가능하다.
	if ( hItem /*&& m_TreeRoot != hItem*/ )
	{
		CEdit* pEdit = m_ctrlHelpTree.GetEditControl ();
		if ( pEdit )
		{
			CString strTitle;
			pEdit->GetWindowText ( strTitle );

			m_ctrlHelpTree.SetItemText ( hItem, strTitle );
			SHELPNODE* pNode = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );
			pNode->strTitle = strTitle.GetString ();
		}
	}
}

void CHelpEditDlg::OnTvnSelchangingTreeHelp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();
	if ( hItem )
	{
		if ( m_bEdited )
		{
			if ( MessageBox ( "도움말 설명이 편집되었습니다. 적용하시겠습니까?", "질문", MB_YESNO) == IDYES ) 
			{
				CString strContents;

				CEdit* pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_CONTENT );				
				pEdit->GetWindowText ( strContents );

				SHELPNODE* pNode = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );
				pNode->strContents = std::string ( strContents.GetString () );
			}
		}

		m_bEdited = FALSE;

		CEdit* pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_CONTENT );
		pEdit->SetReadOnly ( TRUE );
	}
}

void CHelpEditDlg::OnEnChangeEditContent()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bEdited = TRUE;
}

void CHelpEditDlg::OnTvnSelchangedTreeHelp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();
	if ( hItem )
	{
		SHELPNODE* pNode = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );

		CEdit* pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_CONTENT );
		pEdit->SetWindowText ( pNode->strContents.c_str () );
	}
}

void CHelpEditDlg::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();
	if ( hItem )
	{
		CString strContents;

		CEdit* pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_CONTENT );				
		pEdit->GetWindowText ( strContents );

		SHELPNODE* pNode = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );
		pNode->strContents = std::string ( strContents.GetString () );

		m_bEdited = FALSE;

		pEdit->SetReadOnly ( TRUE );
	}
}

void CHelpEditDlg::OnBnClickedButtonEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CEdit* pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_CONTENT );
	pEdit->SetReadOnly ( FALSE );
}

void CHelpEditDlg::OnMenuitemLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	{
		CString szFilter = "도움말 파일 (*.hlp)|*.hlp|";
		
		//	Note : 파일 오픈 다이알로그를 만듬.
		CFileDialog dlg(TRUE,".hlp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
			(CHelpEditDlg*)this);

		dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
		if ( dlg.DoModal() == IDOK )
		{
			BOOL bResult = CHelpDataMan::GetInstance().LoadFile ( dlg.GetPathName().GetString() );
			if ( !bResult )
			{
				MessageBox ( "파일 로드에 실패했습니다." );
				return ;
			}
		}
	}

	UpdateTree ();
}

void CHelpEditDlg::OnMenuitemSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "도움말 파일 (*.hlp)|*.hlp|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".hlp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CHelpEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		BOOL bResult = CHelpDataMan::GetInstance().SaveFile ( dlg.GetPathName().GetString() );		
		if ( !bResult )
		{
			MessageBox ( "파일 저장에 실패했습니다." );
			return ;
		}

		return ;
	}
}

void CHelpEditDlg::OnCsvSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( CHelpDataMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CHelpEditDlg::OnCsvLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( CHelpDataMan::GetInstance().LoadCsvFile( (CWnd*)this ) == S_OK )
	{
		UpdateTree();
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CHelpEditDlg::UpdateTree ()
{
	m_ctrlHelpTree.DeleteAllItems ();	

	HELPNODE_LIST& listHelp = CHelpDataMan::GetInstance().GetHelpDataMan ();
	HELPNODE_LIST_ITER iter = listHelp.begin ();
	HELPNODE_LIST_ITER iter_end = listHelp.end ();

	for ( ; iter != iter_end; ++iter )
	{		
		UpdateNode ( NULL, (*iter) );
	}

	m_ctrlHelpTree.RedrawWindow ();
}

void CHelpEditDlg::UpdateNode ( HTREEITEM hParent, SHELPNODE* pNode )
{
	HTREEITEM hItem = NULL;
	if ( !hParent )	hItem = m_ctrlHelpTree.InsertItem ( pNode->strTitle.c_str () );
	else			hItem = m_ctrlHelpTree.InsertItem ( pNode->strTitle.c_str (), hParent );

	m_ctrlHelpTree.SetItemData ( hItem, (DWORD_PTR) pNode );

	HELPNODE_LIST& listHelp = pNode->listSubNode;
	HELPNODE_LIST_ITER iter = listHelp.begin ();
	HELPNODE_LIST_ITER iter_end = listHelp.end ();

	for ( ; iter != iter_end; ++iter )
	{
		UpdateNode ( hItem, (*iter) );
	}
}