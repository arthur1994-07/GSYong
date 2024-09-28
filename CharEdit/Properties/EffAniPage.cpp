// EffAniPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"

#include "sheetWithTabAnim.h"
#include "sheetWithTabChar.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxEffect/EffAni/DxEffAni.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "EffAniSinglePage.h"
#include "EffAniGhostingPage.h"
#include "EffAniFaceOffPage.h"

#include "EffAniPage.h"


// CEffAniPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffAniPage, CPropertyPage)
CEffAniPage::CEffAniPage()
	: CPropertyPage(CEffAniPage::IDD)
{
}

CEffAniPage::~CEffAniPage()
{
}

void CEffAniPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_EFF, m_EffType);
	DDX_Control(pDX, IDC_LIST_EFF, m_EffList);
}


BEGIN_MESSAGE_MAP(CEffAniPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_STATE, OnBnClickedButtonState)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
END_MESSAGE_MAP()


// CEffAniPage �޽��� ó�����Դϴ�.

BOOL CEffAniPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	int nIndex;
	CString	strText;

	m_EffType.ResetContent();

	DxEffAniType* pEffAniType = DxEffAniMan::GetInstance().GetEffectList();
	while ( pEffAniType )
	{
		strText.Format ( pEffAniType->NAME );
		nIndex = m_EffType.AddString ( strText );
		m_EffType.SetItemData ( nIndex, pEffAniType->TYPEID );

		pEffAniType = pEffAniType->pNext;
	}

	if ( pEffAniType )
	{
		m_EffType.SetCurSel ( 0 );
	}

	EffectListting ();
	FrameMarking ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffAniPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	FrameMarking ( TRUE );

	m_pSheetTab->SetActivePage ( ANIMSETPAGEINDEX );
}

void CEffAniPage::OnBnClickedButtonInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	assert(m_pSheetTab&&"SheetTab�� �����Ͱ� �������� ����.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control�� �����Ͱ� �������� ����.");
	
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();	


	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinCharData *pSkinCharData = pView->GetSkinCharData();
		DxSkinAnimation *pAnimCont = pSkinCharData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if( !pAnimCont )	
			return;
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxSkinAnimation* pAnimCont = pView->GetAttBoneData()->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if ( !pAnimCont )	
			return;
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxSkinAnimation* pAnimCont = pView->GetVehicleData()->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if ( !pAnimCont )	
			return;
	}

	int nIndex = m_EffType.GetCurSel();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = m_EffType.GetItemData ( nIndex );
	if ( dwData == LB_ERR )		return;
	switch ( dwData )
	{
	case EMEFFANI_SINGLE:
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_EFFANI_SINGLE );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniSingle.SetEtc ( m_sAnimName, m_pSheetTab );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniSingle.SetProperty ();
		break;
	case EMEFFANI_GHOSTING:
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_EFFANI_GHOSTING );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniGhosting.SetEtc ( m_sAnimName, m_pSheetTab );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniGhosting.SetProperty ();
		break;
	case EMEFFANI_TRACE:
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_EFFANI_TRACE );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniTrace.SetEtc ( m_sAnimName, m_pSheetTab );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniTrace.SetProperty ();
		break;
	case EMEFFANI_FACEOFF:
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_EFFANI_FACEOFF );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniFaceOff.SetEtc ( m_sAnimName, m_pSheetTab );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniFaceOff.SetProperty ();
		break;
	};

	EffectListting ();
	FrameMarking ();
}

void CEffAniPage::OnBnClickedButtonState()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

	SANIMCONINFO::EFFANILIST_CITER iter = pAnimCont->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_CITER iter_end = pAnimCont->m_listEffAni.end();

	int	nIndex = m_EffList.GetCurSel();
	if ( nIndex == LB_ERR )		return;
	DxEffAniData* pEffCur = (DxEffAniData*) m_EffList.GetItemData ( nIndex );

	assert(pEffCur);
	switch ( pEffCur->GetTypeID() )
	{
	case EMEFFANI_SINGLE:
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_EFFANI_SINGLE );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniSingle.SetInstance ( pEffCur );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniSingle.SetEtc ( m_sAnimName, m_pSheetTab );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniSingle.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFANI_GHOSTING:
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_EFFANI_GHOSTING );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniGhosting.SetInstance ( pEffCur );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniGhosting.SetEtc ( m_sAnimName, m_pSheetTab );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniGhosting.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFANI_TRACE:
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_EFFANI_TRACE );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniTrace.SetInstance ( pEffCur );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniTrace.SetEtc ( m_sAnimName, m_pSheetTab );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniTrace.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFANI_FACEOFF:
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_EFFANI_FACEOFF );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniFaceOff.SetInstance ( pEffCur );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniFaceOff.SetEtc ( m_sAnimName, m_pSheetTab );
		pFrame->m_cDlgEdit.GetSheetChar()->m_EffAniFaceOff.SetProperty ( pEffCur->GetProperty() );
		break;
	};
}

void CEffAniPage::OnBnClickedButtonDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode_none_const( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

	SANIMCONINFO::EFFANILIST_ITER iter = pAnimCont->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimCont->m_listEffAni.end();

	int	nIndex = m_EffList.GetCurSel();
	if ( nIndex == LB_ERR )		return;

	int nCount = 0;
	for ( ; iter!=iter_end; ++nCount )
	{
		SANIMCONINFO::EFFANILIST_ITER iter_del = iter++;

		if ( nCount == nIndex )
		{
			SAFE_DELETE ( (*iter_del) );
			pAnimCont->m_listEffAni.erase(iter_del);

			continue;
		}
	}

	EffectListting ();

	FrameMarking ();
}

void CEffAniPage::SetAnimName ( CString sName )
{
	m_sAnimName = sName;

	EffectListting ();
	FrameMarking ();
}

void CEffAniPage::EffectListting ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

	SANIMCONINFO::EFFANILIST_ITER iter = pAnimCont->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimCont->m_listEffAni.end();

	int nCount = 0;
	DWORD	dwFrame_1;
	DWORD	dwFrame_2;
	CString Str;
	m_EffList.ResetContent();
	for ( ; iter!=iter_end; ++iter, ++nCount )
	{
		dwFrame_1 = 0;
		dwFrame_2 = 0;
		if ( (*iter)->GetTypeID() == EMEFFANI_SINGLE )
		{
			dwFrame_1 = ((EFFANI_PROPERTY_SINGLE*)(*iter)->GetProperty())->m_dwFrame;

			Str.Format ( "[%d/%d] [%s] ", nCount, dwFrame_1, (*iter)->GetName() );
		}
		else if ( (*iter)->GetTypeID() == EMEFFANI_GHOSTING )
		{
			dwFrame_1 = ((EFFANI_PROPERTY_GHOSTING*)(*iter)->GetProperty())->m_dwFrame_1;
			dwFrame_2 = ((EFFANI_PROPERTY_GHOSTING*)(*iter)->GetProperty())->m_dwFrame_2;

			Str.Format ( "[%d/%d/%d] [%s] ", nCount, dwFrame_1, dwFrame_2, (*iter)->GetName() );
		}
		else
		{
			Str.Format ( "[%s] ", (*iter)->GetName() );
		}

		m_EffList.AddString ( Str );
		m_EffList.SetItemData ( nCount, (DWORD) (*iter) );
	}

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		pSkinChar->m_spSkinAniControlThread->m_pEffAniPlayer->RemoveAllEff ();				// ���� �÷��� �ǰ� �ִ� ȿ���� �� �����ش�.	Ghosting ������ �̰� ��.
		pSkinChar->m_spSkinAniControlThread->m_pEffAniPlayer->ResetEff ( pAnimCont );		// ���� ^^
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone ();
		pAttBone->m_spSkinAniControlThread->m_pEffAniPlayer->RemoveAllEff ();				// ���� �÷��� �ǰ� �ִ� ȿ���� �� �����ش�.	Ghosting ������ �̰� ��.
		pAttBone->m_spSkinAniControlThread->m_pEffAniPlayer->ResetEff ( pAnimCont );		// ���� ^^
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle ();
		pVehicle->m_spSkinAniControlThread->m_pEffAniPlayer->RemoveAllEff ();				// ���� �÷��� �ǰ� �ִ� ȿ���� �� �����ش�.	Ghosting ������ �̰� ��.
		pVehicle->m_spSkinAniControlThread->m_pEffAniPlayer->ResetEff ( pAnimCont );		// ���� ^^
	}
}

void CEffAniPage::FrameMarking ( BOOL bReset )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgTimeLine.SetFrameMarking ( 0 );
}

BOOL CEffAniPage::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	EffectListting ();
	FrameMarking ();

	DxEffAniType* pEffAniType = DxEffAniMan::GetInstance().GetEffectList();
	if ( pEffAniType )
	{
		m_EffType.SetCurSel ( 0 );
	}

	return CPropertyPage::OnSetActive();
}
