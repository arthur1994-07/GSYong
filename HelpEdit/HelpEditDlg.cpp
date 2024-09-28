// HelpEditDlg.cpp : ���� ����
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


// CHelpEditDlg ��ȭ ����



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


// CHelpEditDlg �޽��� ó����

BOOL CHelpEditDlg::OnInitDialog()
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

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CHelpEditDlg::OnPaint() 
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
HCURSOR CHelpEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHelpEditDlg::OnNMRclickTreeHelp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	POINT OriginPoint;
	GetCursorPos ( &OriginPoint );

	//	��Ŭ�� ���� ����
	ForceSelectOnRightClick ( OriginPoint );

	//	�޴� ���� & ���� �޴� ����
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

			if ( hParentItem )	//	�θ� �ִ� ��쿡 ������ �ܴ�.
			{
				CString strTemp;
				strTemp.Format ( "�� ����[%02d]", m_ctrlHelpTree.GetCount () );				
				HTREEITEM hNewItem = m_ctrlHelpTree.InsertItem ( strTemp, hParentItem );
				m_ctrlHelpTree.RedrawWindow ();

				SHELPNODE* pParent = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hParentItem );
                SHELPNODE* pNode = new SHELPNODE;
				pNode->strTitle = std::string ( strTemp.GetString () );
				pNode->strContents = pNode->strTitle;
				BOOL bResult = CHelpDataMan::GetInstance().AddNode ( pParent, pNode, "" );
				if ( !bResult ) assert ( 0 && "�� ��� �ޱ� ����" );

				m_ctrlHelpTree.SetItemData ( hNewItem, (DWORD_PTR)pNode );
			}
			else
			{
				CString strTemp;
				strTemp.Format ( "�� ����[%02d]", m_ctrlHelpTree.GetCount () );				
				HTREEITEM hNewItem = m_ctrlHelpTree.InsertItem ( strTemp );
				m_ctrlHelpTree.RedrawWindow ();

                SHELPNODE* pNode = new SHELPNODE;
				pNode->strTitle = std::string ( strTemp.GetString () );
				pNode->strContents = pNode->strTitle;
				BOOL bResult = CHelpDataMan::GetInstance().AddNode ( NULL, pNode, "" );
				if ( !bResult ) assert ( 0 && "�� ��� �ޱ� ����" );

				m_ctrlHelpTree.SetItemData ( hNewItem, (DWORD_PTR)pNode );
			}
		}
		break;

	case ID_MENUITEM_ADD_CHILD:
		{
			if ( hItem )
			{
				CString strTemp;
				strTemp.Format ( "�� ����[%02d]", m_ctrlHelpTree.GetCount () );				
				HTREEITEM hNewItem = m_ctrlHelpTree.InsertItem ( strTemp, hItem );
				m_ctrlHelpTree.RedrawWindow ();

				SHELPNODE* pParent = (SHELPNODE*) m_ctrlHelpTree.GetItemData ( hItem );
                SHELPNODE* pNode = new SHELPNODE;
				pNode->strTitle = std::string ( strTemp.GetString () );
				pNode->strContents = pNode->strTitle;
				BOOL bResult = CHelpDataMan::GetInstance().AddNode ( pParent, pNode, "" );
				if ( !bResult ) assert ( 0 && "�� ��� �ޱ� ����" );

				m_ctrlHelpTree.SetItemData ( hNewItem, (DWORD_PTR)pNode );
			}
		}
		break;

	case ID_MENUITEM_DEL:
		{
			if ( hItem )
			{
				CString strTitle = m_ctrlHelpTree.GetItemText ( hItem );
				CString strTemp = "\"" + strTitle + "\"" + "��(/��) ���� �����Ͻðڽ��ϱ�?";
				if ( MessageBox ( strTemp, "����", MB_YESNO | MB_ICONQUESTION ) == IDYES )
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
	//	<--	���� �۵� �޴� ����
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
						//	���õ� ��� ����
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
							//MessageBox ( "�·�~" );
						}
						break;

					case VK_RIGHT:
						{
							//MessageBox ( "���~" );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();

	//	�������� �������̰�, ��Ʈ�� �ƴҶ��� Ÿ��Ʋ�� ���� �����ϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlHelpTree.GetSelectedItem ();
	if ( hItem )
	{
		if ( m_bEdited )
		{
			if ( MessageBox ( "���� ������ �����Ǿ����ϴ�. �����Ͻðڽ��ϱ�?", "����", MB_YESNO) == IDYES ) 
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
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bEdited = TRUE;
}

void CHelpEditDlg::OnTvnSelchangedTreeHelp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CEdit* pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_CONTENT );
	pEdit->SetReadOnly ( FALSE );
}

void CHelpEditDlg::OnMenuitemLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	{
		CString szFilter = "���� ���� (*.hlp)|*.hlp|";
		
		//	Note : ���� ���� ���̾˷α׸� ����.
		CFileDialog dlg(TRUE,".hlp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
			(CHelpEditDlg*)this);

		dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
		if ( dlg.DoModal() == IDOK )
		{
			BOOL bResult = CHelpDataMan::GetInstance().LoadFile ( dlg.GetPathName().GetString() );
			if ( !bResult )
			{
				MessageBox ( "���� �ε忡 �����߽��ϴ�." );
				return ;
			}
		}
	}

	UpdateTree ();
}

void CHelpEditDlg::OnMenuitemSave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "���� ���� (*.hlp)|*.hlp|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".hlp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CHelpEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		BOOL bResult = CHelpDataMan::GetInstance().SaveFile ( dlg.GetPathName().GetString() );		
		if ( !bResult )
		{
			MessageBox ( "���� ���忡 �����߽��ϴ�." );
			return ;
		}

		return ;
	}
}

void CHelpEditDlg::OnCsvSave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	if( CHelpDataMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CHelpEditDlg::OnCsvLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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