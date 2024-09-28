// AmbientMNG.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "../CharEdit.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../EngineLib/DxEffect/Char/DxEffCharAmbient.h"
#include "../EngineLib/DxEffect/EffKeep/DxEffKeep.h"

#include "AmbientMNG.h"
#include "EffAmbientPage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CAmbientMNG ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CAmbientMNG, CPropertyPage)
CAmbientMNG::CAmbientMNG()
	: CPropertyPage(CAmbientMNG::IDD)
{
}

CAmbientMNG::~CAmbientMNG()
{
}

void CAmbientMNG::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SKINPIECE, m_PieceList);
}


BEGIN_MESSAGE_MAP(CAmbientMNG, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_ADDAMBIENT_BUTTON, OnBnClickedAddambientButton)
	ON_BN_CLICKED(IDC_BUTTON_SAVE2, OnBnClickedButtonSave2)
	ON_BN_CLICKED(IDC_AMBIENT_DEFAULT, OnBnClickedAmbientDefault)
END_MESSAGE_MAP()


// CAmbientMNG �޽��� ó�����Դϴ�.


BOOL CAmbientMNG::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_PieceList.SetExtendedStyle ( m_PieceList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_PieceList.InsertColumn ( 0, _T("���ϸ�"),		LVCFMT_CENTER, 60 );
	m_PieceList.InsertColumn ( 1, _T("Ambient ��"),	LVCFMT_CENTER, 105 );
	m_PieceList.InsertColumn ( 2, _T("Type"),	    LVCFMT_CENTER, 45 );

	UpdateList();

	return TRUE;
}

void CAmbientMNG::UpdateList()
{
	
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    std::vector<DxSkinPiece *> vecSkinPieceList = pView->GetSkinPieceList();

	m_PieceList.DeleteAllItems();

	UINT i;
	bool bUseAmbient;
	for( i = 0; i < vecSkinPieceList.size(); i++ )
	{
		m_PieceList.InsertItem( i, vecSkinPieceList[i]->m_szFileName, NULL );

		bUseAmbient = FALSE;
//		GLEFFCHAR_VEC_ITER iter		= vecSkinPieceList[i]->m_vecEFFECT.begin ();
//		GLEFFCHAR_VEC_ITER iter_end = vecSkinPieceList[i]->m_vecEFFECT.end ();

		DxEffChar* pEffCur = NULL;
		UINT j;
		for( j = 0; j < vecSkinPieceList[i]->m_vecEFFECT.size(); j++ )
		{
//			GLEFFCHAR_VEC_ITER iter_cur = iter++;
			pEffCur = vecSkinPieceList[i]->m_vecEFFECT[j];
			if( pEffCur->GetFlag() == EMECF_AMBIENT )
			{

				DxEffCharAmbient* pAmbientEff = (DxEffCharAmbient *)pEffCur;
				char szTempChar[256];
				int r, g, b;
				r = (int)( pAmbientEff->GetColor().r * 255.0f);
				g = (int)( pAmbientEff->GetColor().g * 255.0f);
				b = (int)( pAmbientEff->GetColor().b * 255.0f);
				sprintf( szTempChar, "r:%d g:%d b:%d", r, g, b );
				m_PieceList.SetItemText( i, 1, szTempChar ); 
				if( pAmbientEff->GetUseTextureColor() )
				{
					m_PieceList.SetItemText( i, 2, "Tex" ); 	
				}else{
					m_PieceList.SetItemText( i, 2, "Amb" ); 	
				}
				bUseAmbient = TRUE;
				break;
			}
		}
		if( !bUseAmbient )
		{
			m_PieceList.SetItemText( i, 1, "NULL" ); 
			m_PieceList.SetItemText( i, 2, "-" ); 
			continue;
		}

	}
}

void CAmbientMNG::OnBnClickedButtonNew()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->ClearSkinPieceList();

	UpdateList();
}

void CAmbientMNG::OnBnClickedButtonOpen()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	HRESULT hr;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    std::vector<DxSkinPiece *> vecSkinPieceList;
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	CString szFilter = "Skin Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT, szFilter,
		(CSkinObjPage*)this);

	CString sFileName;
	CString strPathName;

	DWORD MAXFILE = 2562;
	char* pc = new char[MAXFILE];

	dlg.m_ofn.nMaxFile = MAXFILE;
	dlg.m_ofn.lpstrFile = pc;   
	dlg.m_ofn.lpstrFile[0] = NULL;

	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();



	if ( dlg.DoModal() == IDOK )
	{
		pView->ClearSkinPieceList();

		POSITION pos = dlg.GetStartPosition();
		while ( pos != NULL )
		{
			// �ݺ��ؼ� strPathName �۾��� �Ѵ�.
			strPathName = dlg.GetNextPathName(pos);
			int nPosition = strPathName.ReverseFind('\\');
			strPathName = strPathName.Mid(nPosition+1);

			const char *szFileName = strPathName.GetString ();

			DxSkinPiece *pSkinPiece = new DxSkinPiece();
			pSkinPiece->RestoreDeviceObjects( pd3dDevice );
			hr = pSkinPiece->LoadPiece( szFileName, pd3dDevice, FALSE, FALSE );
			
			if ( FAILED(hr) )
			{
				/*sFileName.ReleaseBuffer ();
				char szTempChar[256];
				sprintf( szTempChar, "%s ���� ���� ����", szFileName );
				MessageBox( szTempChar, "����" );
				return;*/
			}else{
				vecSkinPieceList.push_back( pSkinPiece );
			}

			sFileName.ReleaseBuffer ();

		}
	}else return;

	pView->SetSkinPieceList( vecSkinPieceList );

	UpdateList();
}

void CAmbientMNG::OnBnClickedButtonSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( MessageBox( "��� �����̸����� ���� ����ϴ�.", "�˸�", MB_YESNO ) == IDNO )
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	std::vector<DxSkinPiece *> vecSkinPieceList = pView->GetSkinPieceList();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	UINT i;
	for( i = 0; i < vecSkinPieceList.size(); i++ )
	{
			//	Note : ��Ų ���� ���̺�.
		char szTempChar[256];
		ZeroMemory( szTempChar, 256 );
		strcpy( szTempChar, vecSkinPieceList[i]->m_szFileName );
		vecSkinPieceList[i]->SavePiece ( szTempChar );	
	}

}

void CAmbientMNG::OnBnClickedButtonSave2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( MessageBox( "�����̸����� ���� ����ϴ�.", "�˸�", MB_YESNO ) == IDNO )
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	std::vector<DxSkinPiece *> vecSkinPieceList = pView->GetSkinPieceList();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();


	int count = m_PieceList.GetSelectedCount();
	POSITION pos = m_PieceList.GetFirstSelectedItemPosition();

	int i, iIndex;
	for ( i = 0; i < count; i++)
	{
		SAmbientEffList pushEffList;
		iIndex = m_PieceList.GetNextSelectedItem(pos);     				

		char szTempChar[256];
		ZeroMemory( szTempChar, 256 );
		strcpy( szTempChar, vecSkinPieceList[iIndex]->m_szFileName );
		vecSkinPieceList[iIndex]->SavePiece ( szTempChar );	
	}


}

void CAmbientMNG::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CPropertyPage::OnActivate(nState, pWndOther, bMinimized);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CAmbientMNG::OnBnClickedAddambientButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int count = m_PieceList.GetSelectedCount();
	POSITION pos = m_PieceList.GetFirstSelectedItemPosition();
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	std::vector<DxSkinPiece *> vecSkinPieceList = pView->GetSkinPieceList();
	int iIndex = 0, i;
	UINT j;

	if( count == 0 )
	{
		MessageBox( "����Ʈ���� Piece������ �����ϼ���", "�˸�" );
		return;
	}

    std::vector<SAmbientEffList> ambeintEffList;
	for ( i = 0; i < count; i++)
	{
		SAmbientEffList pushEffList;
		iIndex = m_PieceList.GetNextSelectedItem(pos);     				

		for( j = 0; j < vecSkinPieceList[iIndex]->m_vecEFFECT.size(); j++ )
		{
			if( vecSkinPieceList[iIndex]->m_vecEFFECT[j]->GetFlag() == EMECF_AMBIENT )
			{
				pushEffList.pEffChar =  vecSkinPieceList[iIndex]->m_vecEFFECT[j];
				break;
			}
		}

		pushEffList.iSelectNum = iIndex;
		ambeintEffList.push_back( pushEffList );

	}

#ifndef DEF_SKIN_SYNC
	m_pSheetTab->SetActivePageTab ( CHARTAB_EFFECT_AMBIENT );
	m_pSheetTab->m_EffAmbient.SetProperty ();
	m_pSheetTab->m_EffAmbient.SetInstanceList( ambeintEffList );
#endif
	

}




void CAmbientMNG::OnBnClickedAmbientDefault()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int count = m_PieceList.GetSelectedCount();
	POSITION pos = m_PieceList.GetFirstSelectedItemPosition();
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    std::vector<DxSkinPiece *> vecSkinPieceList = pView->GetSkinPieceList();
	int iIndex = 0, i;
	UINT j;

	if( count == 0 )
	{
		MessageBox( "����Ʈ���� Piece������ �����ϼ���", "�˸�" );
		return;
	}

	EFFCHAR_PROPERTY_AMBIENT	Property;
	Property.m_cColor = D3DXCOLOR( 210.0f/255.0f, 210.0f/255.0f, 210.0f/255.0f, 255.0f/255.0f );


	int iAmbientNum;
	std::vector<SAmbientEffList> ambeintEffList;
	for ( i = 0; i < count; i++)
	{
		iAmbientNum = -1;
		iIndex = m_PieceList.GetNextSelectedItem(pos);     				

		for( j = 0; j < vecSkinPieceList[iIndex]->m_vecEFFECT.size(); j++ )
		{
			if( vecSkinPieceList[iIndex]->m_vecEFFECT[j]->GetFlag() == EMECF_AMBIENT )
			{			
				iAmbientNum = j;
				break;
			}
		}
		if( iAmbientNum == -1 )
		{
			DxEffChar* pEffChar;
			pEffChar = DxEffCharMan::GetInstance().CreateEffect( DxEffCharAmbient::TYPEID, &Property, NULL, vecSkinPieceList[iIndex] );
			vecSkinPieceList[iIndex]->AddEffList( pEffChar );
		}else{
			vecSkinPieceList[iIndex]->m_vecEFFECT[iAmbientNum]->SetProperty( &Property );
			vecSkinPieceList[iIndex]->m_vecEFFECT[iAmbientNum]->Create ( pView->GetD3dDevice() );
		}
	}
	UpdateList();
}
