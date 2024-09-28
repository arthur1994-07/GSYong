// ReplaceFramePage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"

#include "ReplaceFramePage.h"
#include ".\replaceframepage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	CString g_strExOptions[3] = 
	{
		"NONE",
		"AND",
		"NOT"
	};
};

// CReplaceFramePage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CReplaceFramePage, CPropertyPage)
CReplaceFramePage::CReplaceFramePage()
	: CPropertyPage(CReplaceFramePage::IDD)
{
}

CReplaceFramePage::~CReplaceFramePage()
{
}

void CReplaceFramePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_REPLACELINE, m_stReplaceLine);
	DDX_Control(pDX, IDC_STATIC_FILELINE, m_stFileLine);
	DDX_Control(pDX, IDC_LIST_FRAMES, m_FrameList);
	DDX_Control(pDX, IDC_COMBO_EX_SEARCH, m_comboExSearch);
}


BEGIN_MESSAGE_MAP(CReplaceFramePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_FRAMESEARCH, OnBnClickedButtonFramesearch)
	ON_LBN_SELCHANGE(IDC_LIST_FRAMES, OnLbnSelchangeListFrames)
	ON_BN_CLICKED(IDC_BUTTON_FRAME_DEL, OnBnClickedButtonFrameDel)
	ON_BN_CLICKED(IDC_BUTTON_SELECTFILE, OnBnClickedButtonSelectfile)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE_PROCESSING, OnBnClickedButtonReplaceProcessing)
	ON_BN_CLICKED(IDC_CHECK_RANGE, OnBnClickedCheckRange)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CReplaceFramePage �޽��� ó�����Դϴ�.

void CReplaceFramePage::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePageEX( FRAMEPAGEINDEX, FRAMEPAGEINDEX );
}

BOOL CReplaceFramePage::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CPropertyPage::OnKillActive();
}

BOOL CReplaceFramePage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CButton *pButton = (CButton*) GetDlgItem ( IDC_CHECK_RANGE );
	pButton->SetCheck ( DxReplaceContainer::IsRendAABB() );

	return CPropertyPage::OnSetActive();
}

void CReplaceFramePage::FindFrame ( DxFrame *pframe, CString strFrameStr, CString strFrameStrEx, EXOPTIONS emOption )
{
	if ( pframe->szName )
	{
		CString strFrame = pframe->szName;
		strFrame.MakeLower();

		if ( strFrame.Find(strFrameStr)!=LB_ERR )
		{
			if ( emOption!=EMOPT_NONE && strFrameStrEx.GetLength() )
			{
				if ( emOption==EMOPT_AND )
				{
					if ( strFrame.Find(strFrameStrEx)!=LB_ERR )
					{
						//	Note : ã���� Frame ó��.
						//
						int nIndex = m_FrameList.AddString ( pframe->szName );
						m_FrameList.SetItemData ( nIndex, (DWORD)pframe );
					}
				}
				else if ( emOption==EMOPT_NOT )
				{
					if ( strFrame.Find(strFrameStrEx)==LB_ERR )
					{
						//	Note : ã���� Frame ó��.
						//
						int nIndex = m_FrameList.AddString ( pframe->szName );
						m_FrameList.SetItemData ( nIndex, (DWORD)pframe );
					}
				}
			}
			else
			{
				//	Note : ã���� Frame ó��.
				//
				int nIndex = m_FrameList.AddString ( pframe->szName );
				m_FrameList.SetItemData ( nIndex, (DWORD)pframe );
			}
		}
	}

	if ( pframe->pframeFirstChild )
		FindFrame ( pframe->pframeFirstChild, strFrameStr, strFrameStrEx, emOption );
	
	if ( pframe->pframeSibling )
		FindFrame ( pframe->pframeSibling, strFrameStr, strFrameStrEx, emOption );
}

void CReplaceFramePage::OnBnClickedButtonFramesearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();
	if ( !pFrameMesh )	return;
	DxFrame *pDxFrame = pFrameMesh->GetFrameRoot();
	if ( !pDxFrame )		return;

	CString strWndText, strWndTextEx;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
	pWnd->GetWindowText ( strWndText );

	pWnd = GetDlgItem ( IDC_EDIT_FINDSTR_EX );
	pWnd->GetWindowText ( strWndTextEx );

	int nIndex = m_comboExSearch.GetCurSel();
	EXOPTIONS emOption = (EXOPTIONS) m_comboExSearch.GetItemData ( nIndex );

	if ( strWndText.GetLength() <= 2 )
	{
		MessageBox ( "�˻� �ܾ 2�� �̻��� �����Ͻʽÿ�." );
		return;
	}

	strWndText.MakeLower ();
	strWndTextEx.MakeLower ();

	m_FrameList.ResetContent ();
	FindFrame ( pDxFrame, strWndText, strWndTextEx, emOption );
}

void CReplaceFramePage::OnLbnSelchangeListFrames()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	int nIndex = m_FrameList.GetCurSel ();
	if ( nIndex == -1 )	return;

	DxFrame *pDxFrame = (DxFrame*) m_FrameList.GetItemData ( nIndex );

	pFrameMesh->SetCollisionDetectedFrame ( pDxFrame );
	
	D3DXVECTOR3 vPos;
	vPos.x = pDxFrame->matCombined._41;
	vPos.y = pDxFrame->matCombined._42;
	vPos.z = pDxFrame->matCombined._43;
	DxViewPort::GetInstance().CameraJump ( vPos );
}


void CReplaceFramePage::OnBnClickedButtonFrameDel()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//int nIndex = m_FrameList.GetCurSel ();
	//if ( nIndex == -1 )	return;

	//m_FrameList.DeleteString ( nIndex );

	int FullNum = m_FrameList.GetCount();
	for( int nIndex=0; nIndex<FullNum; )
	{
		if( m_FrameList.GetSel(nIndex) > 0 )
		{
			m_FrameList.DeleteString( nIndex );
			--FullNum;	// 1�ܰ� ����
		}
		else
		{
			++nIndex;
		}
	}
}

void CReplaceFramePage::OnBnClickedButtonSelectfile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	HRESULT hr = S_OK;
	CWnd *pWnd;

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Piece File |*.pis|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".pis",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CDlgEdit*)this);

	dlg.m_ofn.lpstrInitialDir = DxReplaceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		pWnd = GetDlgItem ( IDC_EDIT_OFFSET );
		pWnd->SetWindowText ( dlg.GetFileName().GetString() );
	}
}

void CReplaceFramePage::OnBnClickedButtonReplaceProcessing()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	int nCount = m_FrameList.GetCount();

	CString sFileName;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_OFFSET );
	pWnd->GetWindowText ( sFileName );
	if ( sFileName.IsEmpty() )	return;

	for ( int i=0; i<nCount; i++ )
	{
		DxFrame* pDxFrame = (DxFrame*) m_FrameList.GetItemData ( i );

		if ( !pFrameMesh->AddPieceObj ( pDxFrame->szName, sFileName.GetString() ) )
		{
			MessageBox ( "ġȯ ���� ������ �����Ͽ����ϴ�." );
			break;
		}

		//	Note : ���ĸ� ����Ʈ���� ����.
		//
		pFrameMesh->DelAlphaMapFrame ( pDxFrame );
	}

	//	Note : Ʈ���� ������ �Ѵ�.
	//
	pFrameMesh->MakePieceAABBTree ();
}

void CReplaceFramePage::OnBnClickedCheckRange()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton = (CButton*) GetDlgItem ( IDC_CHECK_RANGE );
	DxReplaceContainer::SetRendAABB ( pButton->GetCheck () );
}

BOOL CReplaceFramePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	int nIndex;
	m_comboExSearch.ResetContent ();
	nIndex = m_comboExSearch.AddString ( g_strExOptions[0] );
	m_comboExSearch.SetItemData ( nIndex, 0 );

	m_comboExSearch.SetCurSel ( nIndex );

	nIndex = m_comboExSearch.AddString ( g_strExOptions[1] );
	m_comboExSearch.SetItemData ( nIndex, 1 );

	nIndex = m_comboExSearch.AddString ( g_strExOptions[2] );
	m_comboExSearch.SetItemData ( nIndex, 2 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CReplaceFramePage::OnBnClickedButtonClear()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	int nCount = m_FrameList.GetCount();

	for ( int i=0; i<nCount; i++ )
	{
		DxFrame* pDxFrame = (DxFrame*) m_FrameList.GetItemData ( i );

		pFrameMesh->DelPieceObj( pDxFrame->szName );

		//	Note : ���ĸ� ����Ʈ���� ����.
		//
		pFrameMesh->ModiflyAlphaMapFrame( pDxFrame );
	}

	//	Note : Ʈ���� ������ �Ѵ�.
	//
	pFrameMesh->MakePieceAABBTree ();
}
