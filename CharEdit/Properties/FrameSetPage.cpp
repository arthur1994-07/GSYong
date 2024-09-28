// FrameSetPage.cpp : implementation file
//

#include "stdafx.h"
#include "../charedit.h"

#include "../EngineLib/DxCommon/d3dapp.h"
#include "../EngineLib/DxCommon/dxutil.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxTools/d3dfont.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "SheetWithTabFrame.h"
#include "FrameSetPage.h"

/////////////////////////////////////////////////////////////////////////////
// CFrameSetPage property page

IMPLEMENT_DYNCREATE(CFrameSetPage, CPropertyPage)

CFrameSetPage::CFrameSetPage() :
	CPropertyPage(CFrameSetPage::IDD),
	m_pSheetTab(NULL)
{
	//{{AFX_DATA_INIT(CFrameSetPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFrameSetPage::~CFrameSetPage()
{
}

void CFrameSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrameSetPage)
	DDX_Control(pDX, IDC_LIST_PIECE, m_ListPiece);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrameSetPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFrameSetPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelchangeComboType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameSetPage message handlers

void CFrameSetPage::SetFrameName ( CString sName )
{
	m_sFrameName = sName;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinObject *pSkinObject = pView->GetSkinObject ();

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->SetWindowText ( m_sFrameName );

	//char *szName = m_sFrameName.GetString(m_sFrameName.GetLength()+1);
	//SReplaceFrame* pReplaceFrame = pSkinObject->FindReplaceFrame ( szName );

	//m_ComboType.ResetContent();

	//DWORD nIndex;
	//for ( DWORD i=0; i<RP_TYPE_END+1; i++ )
	//{
	//	nIndex = m_ComboType.AddString ( SReplacePiece::szReplaceType[i] );
	//	m_ComboType.SetItemData ( nIndex, i );
	//}

	//if ( pReplaceFrame )
	//{
	//	m_ComboType.SelectString ( 0, SReplacePiece::szReplaceType[pReplaceFrame->rpType] );
	//}
	//else
	//{
	//	m_ComboType.SelectString ( 0, SReplacePiece::szReplaceType[0] );
	//}

	//if ( pReplaceFrame )
	//	m_rpType = pReplaceFrame->rpType;
	//else
	//	m_rpType = RP_NOTYPE;

	PieceListing();
}

void CFrameSetPage::PieceListing()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinObject *pSkinObject = pView->GetSkinObject ();

	const char *szName = m_sFrameName.GetString();
	//SReplaceFrame* pReplaceFrame = pSkinObject->FindReplaceFrame ( szName );

	//m_ListPiece.ResetContent();

	//SReplacePiece *pReplacePieceCur = CReplaceContainer::GetInstance().GetReplacePieceList();
	//
	//int nSource = m_ListPiece.AddString ( "<<원본 프레임>>" );
	//m_ListPiece.SetItemData ( nSource, NULL );

	//while ( pReplacePieceCur )
	//{
	//	if ( RPTYPE(m_rpType) == pReplacePieceCur->rpType )
	//	{
	//		int nIndex = m_ListPiece.AddString ( pReplacePieceCur->pXFileName );
	//		m_ListPiece.SetItemData ( nIndex, DWORD(pReplacePieceCur) );
	//	}

	//	pReplacePieceCur = pReplacePieceCur->pReplacePieceNext;
	//}

	//if ( pReplaceFrame )
	//{
	//	if ( pReplaceFrame->szXFile )
	//	{
	//		int nSelect = m_ListPiece.FindString ( 0, pReplaceFrame->szXFile );
	//		m_ListPiece.SetCurSel ( nSelect );
	//	}
	//	else
	//	{
	//		m_ListPiece.SetCurSel ( nSource );
	//	}
	//}
}

void CFrameSetPage::OnButtonOk() 
{
	CString Str;
	DWORD nIndex = m_ComboType.GetCurSel();
	if ( nIndex == -1 ) return;

	m_ComboType.GetWindowText ( Str );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinObject *pSkinObject = pView->GetSkinObject ();

	//if ( Str == SReplacePiece::szReplaceType[0] )
	//{
	//	char* szName = m_sFrameName.GetString(m_sFrameName.GetLength()+1);
	//	pSkinObject->DelReplaceFrame ( szName );
	//}
	//else
	//{
	//	char* szName = m_sFrameName.GetString(m_sFrameName.GetLength()+1);
	//	DWORD dwData = m_ComboType.GetItemData(nIndex);

	//	//	Note : 치환 프레임 설정.
	//	//
	//	int nSelect = m_ListPiece.GetCurSel();

	//	if ( nSelect != -1 )
	//	{
	//		CString strName;
	//		m_ListPiece.GetText ( nSelect, strName );

	//		SReplacePiece* pReplacePiece = (SReplacePiece*) m_ListPiece.GetItemData(nSelect);

	//		SReplaceFrame* pReplaceFrame = new SReplaceFrame;

	//		pReplaceFrame->szFrame = new char[m_sFrameName.GetLength()+1];
	//		strcpy ( pReplaceFrame->szFrame, szName );

	//		pReplaceFrame->rpType = (RPTYPE)dwData;

	//		if ( strName != "<<원본 프레임>>" )
	//		{
	//			pReplaceFrame->szXFile = new char[strlen(pReplacePiece->pXFileName)+1];
	//			strcpy ( pReplaceFrame->szXFile, pReplacePiece->pXFileName );

	//			pReplaceFrame->pReplaceFrame = pReplacePiece->m_pFrame;
	//			pReplaceFrame->pSkinMesh = pReplacePiece->pSkinMesh;
	//		}

	//		pSkinMesh->DelReplaceFrame ( pReplaceFrame->szFrame );
	//		pSkinMesh->AddReplaceFrame ( pReplaceFrame );
	//	}
	//	
	//}

	//// TODO: Add your control notification handler code here
	//m_pSheetTab->SetActivePage ( 0 );
}


void CFrameSetPage::OnSelchangeComboType() 
{
	DWORD nIndex = m_ComboType.GetCurSel();
	DWORD dwData = m_ComboType.GetItemData(nIndex);

	m_rpType = dwData;
	PieceListing();	
}
