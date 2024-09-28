// AniBoneScalePage.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabAnim.h"
#include "SheetWithTabChar.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxAniJiggleBlend.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"

#include "AniJiggleBlendPage.h"


// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// CAniJiggleBlendPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CAniJiggleBlendPage, CPropertyPage)
CAniJiggleBlendPage::CAniJiggleBlendPage()
	: CPropertyPage(CAniJiggleBlendPage::IDD)
	, m_pSheetTabChar(NULL)
	, m_pSheetTabAni(NULL)
{
}

CAniJiggleBlendPage::~CAniJiggleBlendPage()
{
}

void CAniJiggleBlendPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ATTACH_BONE, m_ctrlAttachBone);
	DDX_Control(pDX, IDC_LIST_KEY_BLEND, m_ctrlKeyBlend);
}


BEGIN_MESSAGE_MAP(CAniJiggleBlendPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_RADIO_BASE, &CAniJiggleBlendPage::OnBnClickedRadioBase)
	ON_BN_CLICKED(IDC_RADIO_ATTACH_BONE, &CAniJiggleBlendPage::OnBnClickedRadioAttachBone)
	ON_EN_CHANGE(IDC_EDIT_BLEND, &CAniJiggleBlendPage::OnEnChangeEditBlend)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ATTACH_BONE, &CAniJiggleBlendPage::OnNMClickListAttachBone)
	ON_NOTIFY(NM_CLICK, IDC_LIST_KEY_BLEND, &CAniJiggleBlendPage::OnNMClickListKeyBlend)
END_MESSAGE_MAP()


// CAniJiggleBlendPage �޽��� ó�����Դϴ�.

BOOL CAniJiggleBlendPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	//////////////////////////////////////////////////////////////////////////
	//
	RECT listRect;
	m_ctrlAttachBone.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );

	m_ctrlAttachBone.SetExtendedStyle ( m_ctrlKeyBlend.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_ctrlAttachBone.InsertColumn ( 0, _T("Part"),	LVCFMT_CENTER, nListWidth );

	int		     nMaxSize          = 0;
	const char (*ppstrBuffer)[64] = NULL;

	switch ( CCharEditView::m_emObjType )
	{
	case EOBJ_SKINCHAR: 
		nMaxSize    = PIECE_SAVE_SIZE;
		ppstrBuffer = DxSkinPiece::m_szPIECETYPE;
		break;
	case EOBJ_VEHICLE:	
		nMaxSize    = PART_V_SIZE;
		ppstrBuffer = DxSkinPiece::m_szV_PARTSTYPE;
		break;
	}

	for( int i = 0; i < nMaxSize; i++ )
	{
		m_ctrlAttachBone.InsertItem( i, ppstrBuffer[i] );
	}

	//////////////////////////////////////////////////////////////////////////
	m_ctrlKeyBlend.SetExtendedStyle ( m_ctrlKeyBlend.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	listRect;
	m_ctrlKeyBlend.GetClientRect(&listRect);
	nListWidth = ( listRect.right - listRect.left );
	int nColWidth[2] = { int(nListWidth*0.5f), int(nListWidth*0.5f) };

	m_ctrlKeyBlend.InsertColumn ( 0, _T("Key"),		LVCFMT_CENTER, nColWidth[0] );
	m_ctrlKeyBlend.InsertColumn ( 1, _T("Blend"),	LVCFMT_CENTER, nColWidth[1] );

	CWnd* pWnd = GetDlgItem( IDC_EDIT_BLEND );
	pWnd->SetWindowText( _T("0.f") );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CAniJiggleBlendPage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CPropertyPage::OnSetActive();
}

BOOL CAniJiggleBlendPage::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CPropertyPage::OnKillActive();
}

void CAniJiggleBlendPage::SetInstance( const TCHAR* szName, CsheetWithTabAnim* pSheetTabAni )
{
	m_sAnimName = szName;
	m_pSheetTabAni = pSheetTabAni;

	CButton* pButton = (CButton*)GetDlgItem ( IDC_RADIO_BASE );
	pButton->SetCheck( true );

	pButton = (CButton*)GetDlgItem ( IDC_RADIO_ATTACH_BONE );
	pButton->SetCheck( false );

	OnBnClickedRadioBase();

	UpdateKeyBlendCTRL();
}

void CAniJiggleBlendPage::OnBnClickedButtonApply()
{
}

void CAniJiggleBlendPage::OnBnClickedOk()
{
	OnBnClickedButtonApply();

	switch ( CCharEditView::m_emObjType )
	{
	case EOBJ_SKINCHAR: m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE    ); break;
	case EOBJ_ATTBONE : m_pSheetTabChar->SetActivePage( CHARTAB_ATTBONEFILE ); break; 
	case EOBJ_VEHICLE : m_pSheetTabChar->SetActivePage( CHARTAB_VEHICLE     ); break; 
	}
}

void CAniJiggleBlendPage::OnBnClickedButtonInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DWORD dwCurKeyTime = 0;
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		dwCurKeyTime = pSkinChar->m_spSkinAniControlThread->GETCURKEYTIME();
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone();
		dwCurKeyTime = pAttBone->m_spSkinAniControlThread->GETCURKEYTIME();
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle();
		dwCurKeyTime = pVehicle->m_spSkinAniControlThread->GETCURKEYTIME();
	}

	ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		return;

	int nIndex(-1);
	CButton* pButton = (CButton*)GetDlgItem ( IDC_RADIO_ATTACH_BONE );
	if ( pButton->GetCheck() )
	{
		POSITION pos = m_ctrlAttachBone.GetFirstSelectedItemPosition();
		nIndex = m_ctrlAttachBone.GetNextSelectedItem(pos);
		if ( nIndex==LB_ERR )	
			return;
	}

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_BLEND );
	pWnd->GetWindowText( strText );
	float fBlend = (float)atof ( strText.GetString() );

	pAnimCont->m_scpAniJiggleBlend->Insert( nIndex, dwCurKeyTime, fBlend );

	UpdateKeyBlendCTRL();
}

void CAniJiggleBlendPage::OnBnClickedButtonDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )
		return;

	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )
		return;

	int nIndex(-1);
	CButton* pButton = (CButton*)GetDlgItem ( IDC_RADIO_ATTACH_BONE );
	if ( pButton->GetCheck() )
	{
		POSITION pos = m_ctrlAttachBone.GetFirstSelectedItemPosition();
		nIndex = m_ctrlAttachBone.GetNextSelectedItem(pos);
		if ( nIndex==LB_ERR )	
			return;
	}

	POSITION pos = m_ctrlKeyBlend.GetFirstSelectedItemPosition();
	int nIndexB = m_ctrlKeyBlend.GetNextSelectedItem(pos);
	if ( nIndexB==LB_ERR )
		return;

	pAnimCont->m_scpAniJiggleBlend->Delete( nIndex, nIndexB );

	UpdateKeyBlendCTRL();
}

void CAniJiggleBlendPage::UpdateKeyBlendCTRL()
{
	m_ctrlKeyBlend.DeleteAllItems();

	if( !m_pSheetTabAni )	
		return;

	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	
		return;

	const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		
		return;

	int nIndex(-1);
	CButton* pButton = (CButton*)GetDlgItem ( IDC_RADIO_ATTACH_BONE );
	if ( pButton->GetCheck() )
	{
		POSITION pos = m_ctrlAttachBone.GetFirstSelectedItemPosition();
		nIndex = m_ctrlAttachBone.GetNextSelectedItem(pos);
		if ( nIndex==LB_ERR )	
			return;
	}

	int nCount(0);
	char szValue[256];
	const DxAniJiggleBlend::MAP_KEY_BLEND* pmapKeyBlend = pAnimCont->m_scpAniJiggleBlend->GetMapKeyBlend( nIndex );
	if ( !pmapKeyBlend )
		return;

	DxAniJiggleBlend::MAP_KEY_BLEND_CITER citer = pmapKeyBlend->begin();
	for ( ; citer!=pmapKeyBlend->end(); ++citer, ++nCount )
	{
		_gcvt ( (*citer).first, 7, szValue );
		m_ctrlKeyBlend.InsertItem( nCount, szValue );

		_gcvt ( (*citer).second, 7, szValue );
		m_ctrlKeyBlend.SetItemText( nCount, 1, szValue );
	}
}

void CAniJiggleBlendPage::OnBnClickedRadioBase()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ctrlAttachBone.EnableWindow( FALSE );

	UpdateKeyBlendCTRL();
}

void CAniJiggleBlendPage::OnBnClickedRadioAttachBone()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ctrlAttachBone.EnableWindow( TRUE );

	UpdateKeyBlendCTRL();
}

void CAniJiggleBlendPage::OnEnChangeEditBlend()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CAniJiggleBlendPage::OnNMClickListAttachBone(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	UpdateKeyBlendCTRL();
}

void CAniJiggleBlendPage::OnNMClickListKeyBlend(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	
		return;

	const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	if ( !pAnimCont )		
		return;

	int nIndex(-1);
	CButton* pButton = (CButton*)GetDlgItem ( IDC_RADIO_ATTACH_BONE );
	if ( pButton->GetCheck() )
	{
		POSITION pos = m_ctrlAttachBone.GetFirstSelectedItemPosition();
		nIndex = m_ctrlAttachBone.GetNextSelectedItem(pos);
		if ( nIndex==LB_ERR )	
			return;
	}

	const DxAniJiggleBlend::MAP_KEY_BLEND* pmapKeyBlend = pAnimCont->m_scpAniJiggleBlend->GetMapKeyBlend( nIndex );
	if ( !pmapKeyBlend )
		return;
	 
	POSITION pos = m_ctrlKeyBlend.GetFirstSelectedItemPosition();
	int nIndexB = m_ctrlKeyBlend.GetNextSelectedItem(pos);
	if ( nIndexB==LB_ERR )
		return;

	int nCount(0);
	DxAniJiggleBlend::MAP_KEY_BLEND_CITER citerB = pmapKeyBlend->begin();
	for ( ; citerB!=pmapKeyBlend->end(); ++citerB, ++nCount )
	{
		if ( nCount == nIndexB )
		{
			char szValue[256];
			_gcvt ( (*citerB).second, 7, szValue );

			CWnd* pWnd = GetDlgItem( IDC_EDIT_BLEND );
			pWnd->SetWindowText( szValue );

			return;
		}
	}
}

