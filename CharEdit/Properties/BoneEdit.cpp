// BoneEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../charedit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../EngineLib/DxEffect/Char/dxeffchardata.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabFrame.h"
#include "SheetWithTabBone.h"
#include "SheetWithTabAnim.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "BoneEdit.h"
#include ".\boneedit.h"

extern std::string	g_strPICK_BONE;


// CBoneEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBoneEdit, CPropertyPage)
CBoneEdit::CBoneEdit() :
		CPropertyPage(CBoneEdit::IDD),
		m_pSheetTab(NULL)
{
}

CBoneEdit::~CBoneEdit()
{
}

void CBoneEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EDITBONE, m_list_EditBone);
}


BEGIN_MESSAGE_MAP(CBoneEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GETBONE, OnBnClickedButtonGetbone)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_EDITBONE, OnNMDblclkListEditbone)
END_MESSAGE_MAP()


// CBoneEdit 메시지 처리기입니다.

void CBoneEdit::ResetPage ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();


	SetWin_Text( this, IDC_EDIT_BONENAME, "bone name" );
	SetWin_Text( this, IDC_EDIT_SIZE1, "1.0f" );
	SetWin_Text( this, IDC_EDIT_SIZE2, "1.0f" );
	SetWin_Text( this, IDC_EDIT_SIZE3, "1.0f" );

}

void CBoneEdit::UpdateBoneScaleList()
{

	if( !m_pSheetTab )						return;
	if( !m_pSheetTab->GetSkinAniContrl() )	return;

	const DEQ_BONESCALE& deqTempBoneScale = m_pSheetTab->GetSkinAniContrl()->GetBoneScaleList();

	m_list_EditBone.DeleteAllItems();

	CString strTemp;

	for( DWORD i = 0; i < deqTempBoneScale.size(); i++ )
	{
		m_list_EditBone.InsertItem( i, deqTempBoneScale[i].strBoneName.c_str() );
		strTemp.Format( "%.2f", deqTempBoneScale[i].vScale.x );
		m_list_EditBone.SetItemText ( i, 1, strTemp.GetString() );
		strTemp.Format( "%.2f", deqTempBoneScale[i].vScale.y );
		m_list_EditBone.SetItemText ( i, 2, strTemp.GetString() );
		strTemp.Format( "%.2f", deqTempBoneScale[i].vScale.z );
		m_list_EditBone.SetItemText ( i, 3, strTemp.GetString() );
	}
}

BOOL CBoneEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetWin_Text( this, IDC_EDIT_BONENAME, "bone name" );

	RECT listRect;
	m_list_EditBone.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );

	int nColWidth[5] = { int(nListWidth*0.55f), int(nListWidth*0.15f), int(nListWidth*0.15f), int(nListWidth*0.15f) };

	m_list_EditBone.SetExtendedStyle ( m_list_EditBone.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_list_EditBone.InsertColumn ( 0, _T("NAME"),	LVCFMT_CENTER, nColWidth[0] );
	m_list_EditBone.InsertColumn ( 1, _T("X"),	LVCFMT_CENTER, nColWidth[1] );
	m_list_EditBone.InsertColumn ( 2, _T("Y"),	LVCFMT_CENTER, nColWidth[2] );
	m_list_EditBone.InsertColumn ( 3, _T("Z"),	LVCFMT_CENTER, nColWidth[3] );

	SetWin_Text( this, IDC_EDIT_BONENAME, "bone name" );
	SetWin_Text( this, IDC_EDIT_SIZE1, "1.0f" );
	SetWin_Text( this, IDC_EDIT_SIZE2, "1.0f" );
	SetWin_Text( this, IDC_EDIT_SIZE3, "1.0f" );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CBoneEdit::OnBnClickedButtonGetbone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONENAME );
	pWnd->SetWindowText( g_strPICK_BONE.c_str() );
}

void CBoneEdit::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData* pSkinCharData = pView->GetSkinCharData();

	SBONESCALE boneScale;
	boneScale.strBoneName = GetWin_Text( this, IDC_EDIT_BONENAME ).GetString();
	boneScale.vScale.x    = GetWin_Num_float( this, IDC_EDIT_SIZE1 );
	boneScale.vScale.y    = GetWin_Num_float( this, IDC_EDIT_SIZE2 );
	boneScale.vScale.z    = GetWin_Num_float( this, IDC_EDIT_SIZE3 );

	DxBoneTrans *pBoneCur = NULL;
	pBoneCur = pSkinChar->m_spSkinAniControlThread->GetSkeletonForEdit()->FindBone_t( _T(boneScale.strBoneName.c_str()) );
	if( pBoneCur == NULL )
	{
		MessageBox( "본을 찾을 수가 없습니다." );
		return;
	}

	pSkinChar->m_spSkinAniControlThread->AddBoneScale( boneScale );
	pSkinCharData->GetSkinAniControlData()->AddBoneScale( boneScale );

	
	//pSkinChar->SetBoneScaleList( vecTempBoneScale );
	//pSkinCharData->SetBoneScaleList( vecTempBoneScale );

	UpdateBoneScaleList();
	
	
}

void CBoneEdit::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_list_EditBone.GetFirstSelectedItemPosition();
	int nIndex = m_list_EditBone.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR ) return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData* pSkinCharData = pView->GetSkinCharData();

	pSkinChar->m_spSkinAniControlThread->EraseBoneScale( nIndex );
	pSkinCharData->GetSkinAniControlData()->EraseBoneScale( nIndex );

	UpdateBoneScaleList();

}

void CBoneEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
	DlgEdit.GetSheetChar()->m_CharPage.SetBoneEditTab(FALSE);
}

void CBoneEdit::OnNMDblclkListEditbone(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_list_EditBone.GetFirstSelectedItemPosition();
	int nIndex = m_list_EditBone.GetNextSelectedItem(pos);
	if( nIndex == LB_ERR ) return;

	const SBONESCALE& boneScale = *m_pSheetTab->GetSkinAniContrl()->GetBoneScale(nIndex);

	g_strPICK_BONE = m_pSheetTab->GetSkinAniContrl()->GetBoneScaleList()[nIndex].strBoneName;

	SetWin_Text( this, IDC_EDIT_BONENAME, boneScale.strBoneName.c_str() );
	SetWin_Num_float( this, IDC_EDIT_SIZE1, boneScale.vScale.x );
	SetWin_Num_float( this, IDC_EDIT_SIZE2, boneScale.vScale.y );
	SetWin_Num_float( this, IDC_EDIT_SIZE3, boneScale.vScale.z );

	*pResult = 0;
}

