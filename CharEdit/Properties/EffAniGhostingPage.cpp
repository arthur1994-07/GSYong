// EffAniGhostingPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditdoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabAnim.h"

#include "../Dialog/ColorPickerDlg.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"


#include "EffAniGhostingPage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffAniGhostingPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffAniGhostingPage, CPropertyPage)
CEffAniGhostingPage::CEffAniGhostingPage()
	: CPropertyPage(CEffAniGhostingPage::IDD),
	m_pEffAni(NULL)
{
}

CEffAniGhostingPage::~CEffAniGhostingPage()
{
}

void CEffAniGhostingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FRAME_1, m_comboFrame_1);
	DDX_Control(pDX, IDC_COMBO_FRAME_2, m_comboFrame_2);
}


BEGIN_MESSAGE_MAP(CEffAniGhostingPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_FRAME_1, OnCbnSelchangeComboFrame1)
	ON_CBN_SELCHANGE(IDC_COMBO_FRAME_2, OnCbnSelchangeComboFrame2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
END_MESSAGE_MAP()


// CEffAniGhostingPage 메시지 처리기입니다.
BOOL CEffAniGhostingPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffAniGhostingPage::SetInitDialog()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView* pView = (CCharEditView*)pFrame->GetActiveView();

	DWORD dwStartTime	= 0;
	DWORD dwEndTime		= 0;


	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		dwStartTime	= (DWORD)(pSkinChar->m_spSkinAniControlThread->GETSTARTTIME () / pSkinChar->m_spSkinAniControlThread->GETUNITTIME());
		dwEndTime		= (DWORD)(pSkinChar->m_spSkinAniControlThread->GETENDTIME () / pSkinChar->m_spSkinAniControlThread->GETUNITTIME());
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone ();
		dwStartTime	= (DWORD)(pAttBone->m_spSkinAniControlThread->GETSTARTTIME () / pAttBone->m_spSkinAniControlThread->GETUNITTIME());
		dwEndTime		= (DWORD)(pAttBone->m_spSkinAniControlThread->GETENDTIME () / pAttBone->m_spSkinAniControlThread->GETUNITTIME());
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle();
		dwStartTime	= (DWORD)(pVehicle->m_spSkinAniControlThread->GETSTARTTIME () / pVehicle->m_spSkinAniControlThread->GETUNITTIME());
		dwEndTime		= (DWORD)(pVehicle->m_spSkinAniControlThread->GETENDTIME () / pVehicle->m_spSkinAniControlThread->GETUNITTIME());
	}

	m_comboFrame_1.ResetContent();
	m_comboFrame_2.ResetContent();

	int nCount = 0;
	char	szValue[256];
	for ( DWORD i=dwStartTime; i<=dwEndTime; ++i, ++nCount )
	{
		_gcvt ( i, 7, szValue );
		m_comboFrame_1.AddString ( szValue );
		m_comboFrame_1.SetItemData ( nCount, i );

		m_comboFrame_2.AddString ( szValue );
		m_comboFrame_2.SetItemData ( nCount, i );
	}
}

void CEffAniGhostingPage::SetEtc ( CString sAnimName, CsheetWithTabAnim *pSheetTabAni )
{
	m_sAnimName = sAnimName;
	m_pSheetTabAni = pSheetTabAni;

	SetInitDialog();
}

void CEffAniGhostingPage::SetProperty ( EFFANI_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFANI_PROPERTY_GHOSTING*)pProp);

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView* pView = (CCharEditView*)pFrame->GetActiveView();
	DWORD dwStartTime	= 0;


	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		dwStartTime	= (DWORD)(pSkinChar->m_spSkinAniControlThread->GETSTARTTIME () / pSkinChar->m_spSkinAniControlThread->GETUNITTIME());
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone ();
		dwStartTime	= (DWORD)(pAttBone->m_spSkinAniControlThread->GETSTARTTIME () / pAttBone->m_spSkinAniControlThread->GETUNITTIME());
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle ();
		dwStartTime	= (DWORD)(pVehicle->m_spSkinAniControlThread->GETSTARTTIME () / pVehicle->m_spSkinAniControlThread->GETUNITTIME());
	}


	int nIndex = m_Property.m_dwFrame_1 - dwStartTime;
	if ( nIndex < 0 )	nIndex = 0;
	m_comboFrame_1.SetCurSel ( nIndex );

	nIndex = m_Property.m_dwFrame_2 - dwStartTime;
	if ( nIndex < 0 )	nIndex = 0;
	m_comboFrame_2.SetCurSel ( nIndex );

	char	szValue[256];
	CWnd	*pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_NUM );
	_gcvt ( m_Property.m_dwGhostingNum, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_UNIT );
	_gcvt ( m_Property.m_dwUnit, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	_gcvt ( m_Property.m_dwAlphaMax, 7, szValue );
	pWnd->SetWindowText ( szValue );

	FrameMarking ();
}

void CEffAniGhostingPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	OnCbnSelchangeComboFrame1();
	OnCbnSelchangeComboFrame2();

	CString strText;
	CWnd	*pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_NUM );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwGhostingNum = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_UNIT );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwUnit = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwAlphaMax = atoi ( strText.GetString() );


	SetProperty ( &m_Property );

	if ( !m_pEffAni )
	{
		m_pEffAni = DxEffAniMan::GetInstance().CreateEffect
		(
			DxEffAniData_Ghosting::TYPEID,
			&m_Property
		);

		if ( !m_pEffAni )
		{
			MessageBox ( "이팩트 생성에 실패하였습니다.", "ERROR", MB_OK );
			return;
		}

		m_Property = *((EFFANI_PROPERTY_GHOSTING*)m_pEffAni->GetProperty ());

		ANIMCONTNODE *pAnimNodeCur = m_pSheetTabAni->GetSkinAniContrl()->findanicontnode_none_const ( m_sAnimName.GetString() );
		if ( !pAnimNodeCur )	return;
		DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		pAnimCont->m_listEffAni.push_back ( m_pEffAni );		// 효과를 삽입한다.
	}
	else
	{
		m_pEffAni->SetProperty ( &m_Property );
		m_pEffAni->Create ( pView->GetD3dDevice() );
	}

	FrameMarking ();
}

void CEffAniGhostingPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

	m_pEffAni = NULL;
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_CHARFILE );
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_ATTBONEFILE );
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_VEHICLE );
	}
}

void CEffAniGhostingPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pEffAni = NULL;
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_CHARFILE );
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_ATTBONEFILE );
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		m_pSheetTabChar->SetActivePage ( CHARTAB_VEHICLE );
	}
}

void CEffAniGhostingPage::FrameMarking ()
{
	WORD	pIndices[2];

	pIndices[0] = (WORD)m_Property.m_dwFrame_1;
	pIndices[1] = (WORD)m_Property.m_dwFrame_2;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgTimeLine.SetFrameMarking ( 2, pIndices );
}

void CEffAniGhostingPage::OnCbnSelchangeComboFrame1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCurSel = m_comboFrame_1.GetCurSel();
	m_Property.m_dwFrame_1 = m_comboFrame_1.GetItemData ( nCurSel );

	//	2번째 것이 1번째 것 보다 작을 경우 수정해준다.
	if ( m_Property.m_dwFrame_2 < m_Property.m_dwFrame_1 )
	{
		m_Property.m_dwFrame_2 = m_Property.m_dwFrame_1;

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView* pView = (CCharEditView*)pFrame->GetActiveView();
		DWORD dwStartTime	= 0;

		if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
		{
			DxSkinChar* pSkinChar = pView->GetSkinChar ();
			dwStartTime	= (DWORD)(pSkinChar->m_spSkinAniControlThread->GETSTARTTIME () / pSkinChar->m_spSkinAniControlThread->GETUNITTIME());
		}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
		{
			DxAttBone* pAttBone = pView->GetAttBone ();
			dwStartTime	= (DWORD)(pAttBone->m_spSkinAniControlThread->GETSTARTTIME () / pAttBone->m_spSkinAniControlThread->GETUNITTIME());
		}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
		{
			DxVehicle* pVehicle = pView->GetVehicle ();
			dwStartTime	= (DWORD)(pVehicle->m_spSkinAniControlThread->GETSTARTTIME () / pVehicle->m_spSkinAniControlThread->GETUNITTIME());
		}

		int nIndex = m_Property.m_dwFrame_2 - dwStartTime;
		if ( nIndex < 0 )	nIndex = 0;
		m_comboFrame_2.SetCurSel ( nIndex );
	}
}

void CEffAniGhostingPage::OnCbnSelchangeComboFrame2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCurSel = m_comboFrame_2.GetCurSel();
	m_Property.m_dwFrame_2 = m_comboFrame_2.GetItemData ( nCurSel );

	//	2번째 것이 1번째 것 보다 작을 경우 수정해준다.
	if ( m_Property.m_dwFrame_2 < m_Property.m_dwFrame_1 )
	{
		m_Property.m_dwFrame_2 = m_Property.m_dwFrame_1;

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView* pView = (CCharEditView*)pFrame->GetActiveView();
		DWORD dwStartTime	= 0;

		if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
		{
			DxSkinChar* pSkinChar = pView->GetSkinChar ();
			dwStartTime	= (DWORD)(pSkinChar->m_spSkinAniControlThread->GETSTARTTIME () / pSkinChar->m_spSkinAniControlThread->GETUNITTIME());
		}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
		{
			DxAttBone* pAttBone = pView->GetAttBone ();
			dwStartTime	= (DWORD)(pAttBone->m_spSkinAniControlThread->GETSTARTTIME () / pAttBone->m_spSkinAniControlThread->GETUNITTIME());
		}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
		{
			DxVehicle* pVehicle = pView->GetVehicle ();
			dwStartTime	= (DWORD)(pVehicle->m_spSkinAniControlThread->GETSTARTTIME () / pVehicle->m_spSkinAniControlThread->GETUNITTIME());
		}

		int nIndex = m_Property.m_dwFrame_2 - dwStartTime;
		if ( nIndex < 0 )	nIndex = 0;
		m_comboFrame_2.SetCurSel ( nIndex );
	}
}

void CEffAniGhostingPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DCOLOR &d3dColor = m_Property.m_dwColor;
	COLORREF retColor;
	COLORREF Color = RGB ( ((d3dColor&0xff0000)>>16), ((d3dColor&0xff00)>>8), (d3dColor&0xff) );
	CColorPickerDlg colorDlg ( Color );

	int	R,G,B;

	if ( IDOK == colorDlg.DoModal () )
	{
		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );
		m_Property.m_dwColor = D3DCOLOR_ARGB ( 0x00, R, G, B );
	}
}
