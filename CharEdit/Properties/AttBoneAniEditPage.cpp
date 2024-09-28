// AttBoneAniEditPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../charedit.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../EngineLib/DxCommon/d3dapp.h"
#include "../EngineLib/DxCommon/dxutil.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "AttBoneAniEditPage.h"


// CAttBoneAniEditPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNCREATE(CAttBoneAniEditPage, CDialog)

CAttBoneAniEditPage::CAttBoneAniEditPage(CWnd* pParent /* = NULL */)
					: CDialog(CAttBoneAniEditPage::IDD, pParent)
					, m_sAnimName("")
					, m_dwObjType(0)
					, m_dwPieceType(0)
					, m_pSkinAniContrl(NULL)
{
}

CAttBoneAniEditPage::CAttBoneAniEditPage(
    const DWORD dwObjType,
    const DWORD dwPieceType,
    const SAttachBoneAnimInfo& sAttachBoneAnimInfo, 
    const CString& sAnimName,
    DxSkinAniControl *pSkinAniCtrl,
    CWnd* pParent /* = NULL */)
	: CDialog(CAttBoneAniEditPage::IDD, pParent)
	, m_sAnimName(sAnimName)
	, m_dwObjType(dwObjType)
	, m_dwPieceType(dwPieceType)
	, m_pSkinAniContrl(pSkinAniCtrl)
	, m_sAttachBoneAnimInfo(sAttachBoneAnimInfo)
{	
}

CAttBoneAniEditPage::~CAttBoneAniEditPage()
{
}

void CAttBoneAniEditPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ANITYPE, m_ComboType);
	DDX_Control(pDX, IDC_LIST_ATTSTRIKEANI, m_list_AttStrikeAni);
}

BOOL CAttBoneAniEditPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD nIndex;
	for ( DWORD i=0; i<AN_ATTBONE_SIZE; ++i )
	{
		// �Ʒ��� COMMENT�� �����Ϳ��� �о���� �ʴ´�;
		nIndex = m_ComboType.AddString ( COMMENT::ANI_ATTBONE_MAINTYPE[i].c_str() );
		m_ComboType.SetItemData ( nIndex, i );
	}

	m_list_AttStrikeAni.DeleteAllItems();
	m_list_AttStrikeAni.SetExtendedStyle ( m_list_AttStrikeAni.GetExtendedStyle() |	LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	RECT listRect;
	m_list_AttStrikeAni.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = { int(nListWidth*0.4f), int(nListWidth*0.38f), int(nListWidth*0.2f) };

	m_list_AttStrikeAni.InsertColumn ( 0, _T("Ÿ��"),	LVCFMT_CENTER, nColWidth[0] );
	m_list_AttStrikeAni.InsertColumn ( 1, _T("�ִ�"),	LVCFMT_CENTER, nColWidth[1] );
	m_list_AttStrikeAni.InsertColumn ( 2, _T("Frame"),	LVCFMT_CENTER, nColWidth[2] );

	const ANIMCONTNODE *pAnimNodeCur = m_pSkinAniContrl->findanicontnode ( m_sAnimName.GetString() );
	if ( pAnimNodeCur )	
	{
		DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		if( pAnimCont->m_wStrikeCount != m_sAttachBoneAnimInfo.vecStrikeAni.size() )
		{
			m_sAttachBoneAnimInfo.sBasicAni.Init();
			m_sAttachBoneAnimInfo.vecStrikeAni.clear();
			SetWin_Check( this, IDC_CHECK_STRIKE, m_sAttachBoneAnimInfo.bUseStrikeAni );
			OnBnClickedCheckStrike();
		}
	}


	UpdateDialog();

	

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

BEGIN_MESSAGE_MAP(CAttBoneAniEditPage, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_STRIKE, OnBnClickedCheckStrike)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnBnClickedButtonSet)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ATTSTRIKEANI, OnNMDBLClickListAttstrikeani)
END_MESSAGE_MAP()


// CAttBoneAniEditPage �޽��� ó�����Դϴ�.

void CAttBoneAniEditPage::UpdateDialog()
{
	if( !m_pSkinAniContrl ) return;
	if( m_sAnimName.IsEmpty() ) return;


	const char (*ppstrBuffer)[64] = NULL;
	switch ( m_dwObjType )
	{
	case EOBJ_SKINCHAR: 
		ppstrBuffer = DxSkinPiece::m_szPIECETYPE;
		break;
	case EOBJ_VEHICLE:	
		ppstrBuffer = DxSkinPiece::m_szV_PARTSTYPE;
		break;
	}

	SetWin_Text( this, IDC_STATIC_NAME, ppstrBuffer[m_dwPieceType] );

	SetWin_Check( this, IDC_CHECK_STRIKE, m_sAttachBoneAnimInfo.bUseStrikeAni );

	if( !m_sAttachBoneAnimInfo.bUseStrikeAni )
	{
		SetWin_Enable( this, IDC_BUTTON_SET, FALSE ); 

		m_ComboType.SetCurSel( (DWORD)m_sAttachBoneAnimInfo.sBasicAni.emAttAniType );
		SetWin_Num_int( this, IDC_EDIT_FRAMENUM, m_sAttachBoneAnimInfo.sBasicAni.nStartAnim );

		m_list_AttStrikeAni.DeleteAllItems();

		m_list_AttStrikeAni.EnableWindow(FALSE);

	}else{

		m_list_AttStrikeAni.DeleteAllItems();

		m_list_AttStrikeAni.EnableWindow(TRUE);

		SetWin_Enable( this, IDC_BUTTON_SET, TRUE ); 

		const ANIMCONTNODE *pAnimNodeCur = m_pSkinAniContrl->findanicontnode ( m_sAnimName.GetString() );
		if ( !pAnimNodeCur )	return;
		const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		const DxSkinAnimation* pSelectedAniCont = m_pSkinAniContrl->GETCURANIM();
		if ( !pSelectedAniCont )	return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;

		SAttachBoneAnimBasicInfo sTempBasicAni;

		for ( int i=0; i<pAnimCont->m_wStrikeCount; ++i )
		{

			sTempBasicAni = m_sAttachBoneAnimInfo.vecStrikeAni[i];

			CString Str;
			Str.Format ("[%d/%d] ����[%s] ",  i, pAnimCont->m_sStrikeEff[i].m_dwFrame/dwUnitTime, 
				DxSkinPiece::m_szPIECETYPE[pAnimCont->m_sStrikeEff[i].m_emPiece]);

			m_list_AttStrikeAni.InsertItem( i, Str.GetString(), NULL );

			// �Ʒ��� COMMNET�� �����Ϳ��� �о�����ʴ´�;
			m_list_AttStrikeAni.SetItemText( i, 1, COMMENT::ANI_ATTBONE_MAINTYPE[sTempBasicAni.emAttAniType].c_str() );
			Str.Format( "%d", sTempBasicAni.nStartAnim );
			m_list_AttStrikeAni.SetItemText( i, 2, Str.GetString() );
		}
	}
}

void CAttBoneAniEditPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( !m_sAttachBoneAnimInfo.bUseStrikeAni )
	{
		m_sAttachBoneAnimInfo.sBasicAni.nStartAnim   = GetWin_Num_int( this, IDC_EDIT_FRAMENUM );
		m_sAttachBoneAnimInfo.sBasicAni.emAttAniType = (EMANI_MAINTYPE)m_ComboType.GetCurSel();
	}
	OnOK();
}

void CAttBoneAniEditPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnCancel();
}

void CAttBoneAniEditPage::OnBnClickedCheckStrike()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( GetWin_Check( this, IDC_CHECK_STRIKE ) )
	{
		m_sAttachBoneAnimInfo.bUseStrikeAni = TRUE;
		m_sAttachBoneAnimInfo.sBasicAni.Init();


		SAttachBoneAnimBasicInfo sTempBasicAni;
		sTempBasicAni.Init();

		const ANIMCONTNODE *pAnimNodeCur = m_pSkinAniContrl->findanicontnode ( m_sAnimName.GetString() );
		if ( !pAnimNodeCur )	return;
		const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		for ( int i=0; i<pAnimCont->m_wStrikeCount; ++i )
		{
			
			m_sAttachBoneAnimInfo.vecStrikeAni.push_back( sTempBasicAni );
		}

	}else{
		m_sAttachBoneAnimInfo.bUseStrikeAni = FALSE;
		m_sAttachBoneAnimInfo.vecStrikeAni.clear();
	}
	UpdateDialog();
}


void CAttBoneAniEditPage::OnBnClickedButtonSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION	pos = m_list_AttStrikeAni.GetFirstSelectedItemPosition ();	
	int nIndex = m_list_AttStrikeAni.GetNextSelectedItem ( pos );	
	if( nIndex == -1 ) return;

	m_sAttachBoneAnimInfo.vecStrikeAni[nIndex].nStartAnim  = GetWin_Num_int( this, IDC_EDIT_FRAMENUM );
	m_sAttachBoneAnimInfo.vecStrikeAni[nIndex].emAttAniType = (EMANI_MAINTYPE)m_ComboType.GetCurSel();

	UpdateDialog();
}

void CAttBoneAniEditPage::OnNMDBLClickListAttstrikeani(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION	pos = m_list_AttStrikeAni.GetFirstSelectedItemPosition ();	
	int nIndex = m_list_AttStrikeAni.GetNextSelectedItem ( pos );	
	if( nIndex == -1 ) return;
	
	m_ComboType.SetCurSel( (DWORD)m_sAttachBoneAnimInfo.vecStrikeAni[nIndex].emAttAniType );
	SetWin_Num_int( this, IDC_EDIT_FRAMENUM, m_sAttachBoneAnimInfo.vecStrikeAni[nIndex].nStartAnim );	

	*pResult = 0;
}
