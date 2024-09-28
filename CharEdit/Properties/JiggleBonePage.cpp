// JiggleBonePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "FramePage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxJiggleBone.h"

#include "JiggleBonePage.h"


extern TSTRING	g_strPICK_BONE;
extern TSTRING	g_strJIGGLE_BONE;
extern TSTRING	g_strJIGGLE_BONE_PREV;
extern float	g_fJIGGLE_BONE_SWING_Y;
extern float	g_fJIGGLE_BONE_SWING_Z;
extern D3DXVECTOR3 g_vJIGGLE_BONE_ROTATE;

// CJiggleBonePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CJiggleBonePage, CPropertyPage)

CJiggleBonePage::CJiggleBonePage()
	: CPropertyPage(CJiggleBonePage::IDD)
	, m_pJiggleBone(NULL)
{

}

CJiggleBonePage::~CJiggleBonePage()
{
}

void CJiggleBonePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_JIGGLE_BONE, m_listJigleBone);
	DDX_Control(pDX, IDC_LIST_JOINT, m_listJoint);
}


BEGIN_MESSAGE_MAP(CJiggleBonePage, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CJiggleBonePage::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, &CJiggleBonePage::OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CJiggleBonePage::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_CHECK_COLLISION, &CJiggleBonePage::OnBnCheckCollision)
	ON_LBN_SELCHANGE(IDC_LIST_JIGGLE_BONE, &CJiggleBonePage::OnLbnSelchangeListJiggleBone)
	ON_LBN_SELCHANGE(IDC_LIST_JOINT, &CJiggleBonePage::OnLbnSelchangeListJoint)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_JOINT, &CJiggleBonePage::OnBnClickedDeleteJoint)
	ON_BN_CLICKED(IDC_BUTTON_RESET_JOINT, &OnBnClickedResetJoint)
	ON_EN_CHANGE(IDC_EDIT_TWIST, &CJiggleBonePage::OnEnChangeEditTwist)
	ON_EN_CHANGE(IDC_EDIT_SWING, &CJiggleBonePage::OnEnChangeEditSwing)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CJiggleBonePage::OnBnClickedButtonApply)
	ON_EN_CHANGE(IDC_EDIT_DENSITY, &CJiggleBonePage::OnEnChangeEditDensity)
	ON_EN_CHANGE(IDC_EDIT_ROTATE_X, &CJiggleBonePage::OnEnChangeEditRotateX)
	ON_EN_CHANGE(IDC_EDIT_ROTATE_Y, &CJiggleBonePage::OnEnChangeEditRotateY)
	ON_EN_CHANGE(IDC_EDIT_ROTATE_Z, &CJiggleBonePage::OnEnChangeEditRotateZ)
	ON_EN_CHANGE(IDC_EDIT_SWING_Z, &CJiggleBonePage::OnEnChangeEditSwingZ)
	ON_EN_CHANGE(IDC_EDIT_TWIST_RESTITUTION, &CJiggleBonePage::OnEnChangeEditTwistRestitution)
	ON_EN_CHANGE(IDC_EDIT_TWIST_SPRING, &CJiggleBonePage::OnEnChangeEditTwistSpring)
	ON_EN_CHANGE(IDC_EDIT_TWIST_DAMPING, &CJiggleBonePage::OnEnChangeEditTwistDamping)
	ON_EN_CHANGE(IDC_EDIT_SWING_RESTITUTION, &CJiggleBonePage::OnEnChangeEditSwingRestitution)
	ON_EN_CHANGE(IDC_EDIT_SWING_SPRING, &CJiggleBonePage::OnEnChangeEditSwingSpring)
	ON_EN_CHANGE(IDC_EDIT_SWING_DAMPING, &CJiggleBonePage::OnEnChangeEditSwingDamping)
END_MESSAGE_MAP()


// CJiggleBonePage 메시지 처리기입니다.

void CJiggleBonePage::SetData( DxJiggleBone* pJiggleBone )
{
	m_pJiggleBone = pJiggleBone;
}


// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

void CJiggleBonePage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedButtonApply();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	m_pSheetTabChar->SetActivePage( CHARTAB_BONE_SETTING );
}

BOOL CJiggleBonePage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	//
	UpdateListJiggleBone();

	// Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();

	return CPropertyPage::OnSetActive();
}

BOOL CJiggleBonePage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();

	// Select 해제
	g_strJIGGLE_BONE = _T("");

	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}

void CJiggleBonePage::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	const DxSkeleton* pSkeleton;
	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		pSkeleton = pView->GetSkinChar()->m_spSkinAniControlThread->GetSkeletonForEdit();
		break;
	case EOBJ_ATTBONE:
		pSkeleton = pView->GetAttBone()->m_spSkinAniControlThread->GetSkeletonForEdit();
		break;
	case EOBJ_VEHICLE:
		pSkeleton = pView->GetVehicle()->m_spSkinAniControlThread->GetSkeletonForEdit();
		break;
	}
	if ( !pSkeleton )
		return;

	m_pJiggleBone->Insert( pSkeleton, g_strPICK_BONE );

	// Note : List 정리
	UpdateListJiggleBone();
}

void CJiggleBonePage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listJigleBone.GetCurSel();
	if( nIndex==LB_ERR )	
		return;

	m_pJiggleBone->Delete( nIndex );

	// Note : List 정리
	UpdateListJiggleBone();
}

void CJiggleBonePage::OnBnCheckCollision()
{
	int nIndex = m_listJigleBone.GetCurSel();
	if( nIndex==LB_ERR )	
		return;

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_COLLISION );

	m_pJiggleBone->SetCollision( nIndex, pButton->GetCheck() );
}

void CJiggleBonePage::UpdateListJiggleBone()
{
	m_listJigleBone.ResetContent();

	DxJiggleBone::VEC_JIGGLEBONEDATA_ITER iter = m_pJiggleBone->m_vecJiggleBoneData.begin();
	for( ; iter!=m_pJiggleBone->m_vecJiggleBoneData.end(); ++iter )
	{
		int nIndex = m_listJigleBone.AddString(  (*iter)->m_strStartJiggleBone.c_str() );
		m_listJigleBone.SetItemData( nIndex, nIndex );
	}

	// 지글본 셋팅이 변경되면 Joint 도 Edit 상 표시를 없애준다.
	UpdateListJoint( NULL );
}

void CJiggleBonePage::UpdateListJoint( JiggleBoneData* pJiggleBoneData )
{
	m_listJoint.ResetContent();

	if ( !pJiggleBoneData )
	{
		// Select 해제
		g_strJIGGLE_BONE = _T("");
		return;
	}

	if ( pJiggleBoneData->m_dwJointNumberUSED <= 0 )
		return;

	// 마지막꺼는 Joint 로 활용될 수 없다.
	for ( DWORD i=0; i<pJiggleBoneData->m_vecJointData.size()-1 && i<pJiggleBoneData->m_dwJointNumberUSED-1; ++i )
	{
		TSTRING strName(_T("["));
		//strName += pJiggleBoneData->m_vecJointData[i]->m_strBoneParent;
		//strName += _T("]->[");
		strName += pJiggleBoneData->m_vecJointData[i]->m_strBoneJoint;
		strName += _T("]");

		int nIndex = m_listJoint.AddString( strName.c_str() );
		m_listJoint.SetItemData( nIndex, nIndex );
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix();
}

void CJiggleBonePage::OnLbnSelchangeListJiggleBone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listJigleBone.GetCurSel();
	if( nIndex==LB_ERR )	
		return;

	JiggleBoneData* pJiggleBoneData = m_pJiggleBone->GetJiggleBone( nIndex );
	UpdateListJoint( pJiggleBoneData );

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_COLLISION );
	pButton->SetCheck ( pJiggleBoneData->m_bJiggleBoneColl );
}

void CJiggleBonePage::OnLbnSelchangeListJoint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	const DxSkeleton* pSkeleton;
	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		pSkeleton = pView->GetSkinChar()->m_spSkinAniControlThread->GetSkeletonForEdit();
		break;
	case EOBJ_ATTBONE:
		pSkeleton = pView->GetAttBone()->m_spSkinAniControlThread->GetSkeletonForEdit();
		break;
	case EOBJ_VEHICLE:
		pSkeleton = pView->GetVehicle()->m_spSkinAniControlThread->GetSkeletonForEdit();
		break;
	}
	if ( !pSkeleton )
		return;

	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_TWIST );
	Str.Format ( _T("%d"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwTwist );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TWIST_RESTITUTION );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_fTwistRestitution );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TWIST_SPRING );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_fTwistSpring );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TWIST_DAMPING );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_fTwistDamping );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SWING );
	Str.Format ( _T("%d"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwSwingY );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SWING_Z );
	Str.Format ( _T("%d"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwSwingZ );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SWING_RESTITUTION );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_fSwingRestitution );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SWING_SPRING );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_fSwingSpring );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SWING_DAMPING );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_fSwingDamping );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_DENSITY );
	Str.Format ( _T("%3.2f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_fDensity );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ROTATE_X );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_vRotate.x );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ROTATE_Y );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_vRotate.y );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ROTATE_Z );
	Str.Format ( _T("%3.1f"), pJiggleBoneData->m_vecJointData[ nIndex ]->m_vRotate.z );
	pWnd->SetWindowText( Str.GetString() );

	// Select 되었다고 알려야 된다.
	g_strJIGGLE_BONE_PREV = pJiggleBoneData->m_vecJointData[ nIndex ]->m_strBoneParent;
	g_strJIGGLE_BONE = pJiggleBoneData->m_vecJointData[ nIndex ]->m_strBoneJoint;
	g_fJIGGLE_BONE_SWING_Y = static_cast<float>( pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwSwingY );
	g_fJIGGLE_BONE_SWING_Z = static_cast<float>( pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwSwingZ );

	DxBoneTrans* pBoneTrans = pSkeleton->FindBone_t( g_strJIGGLE_BONE );
	if ( pBoneTrans )
	{
		g_vJIGGLE_BONE_ROTATE = pJiggleBoneData->m_vecJointData[ nIndex ]->m_vRotate;
	}
}

int CJiggleBonePage::GetCurSelJoint( JiggleBoneData** ppJiggleBoneData )
{
	(*ppJiggleBoneData) = NULL;

	int nIndex = m_listJigleBone.GetCurSel();
	if( nIndex==LB_ERR )	
		return LB_ERR;

	(*ppJiggleBoneData) = m_pJiggleBone->GetJiggleBone( nIndex );
	if ( !(*ppJiggleBoneData) )
		return LB_ERR;

	return m_listJoint.GetCurSel();
}

void CJiggleBonePage::OnBnClickedDeleteJoint()
{
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	pJiggleBoneData->DeleteJoint( nIndex );

	// Select 해제
	g_strJIGGLE_BONE = _T("");
}

void CJiggleBonePage::OnBnClickedResetJoint()
{
	JiggleBoneData* pJiggleBoneData;
	GetCurSelJoint( &pJiggleBoneData );

	if ( pJiggleBoneData )
	{
		pJiggleBoneData->ResetJoint();
	}	
}

void CJiggleBonePage::OnEnChangeEditTwist()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_TWIST );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwTwist = _tstoi( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditSwing()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_SWING );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwSwingY = _tstoi( Str.GetString() );
	g_fJIGGLE_BONE_SWING_Y = static_cast<float>( pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwSwingY );
}

void CJiggleBonePage::OnEnChangeEditSwingZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_SWING_Z );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwSwingZ = _tstoi( Str.GetString() );
	g_fJIGGLE_BONE_SWING_Z = static_cast<float>( pJiggleBoneData->m_vecJointData[ nIndex ]->m_dwSwingZ );
}

void CJiggleBonePage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	const DxSkeleton* pSkeleton;
	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		{
			DxSkinChar *pSkinChar = pView->GetSkinChar();
			DxSkinCharData* pSkinCharData = pView->GetSkinCharData();
			pSkinChar->SetCharData( pSkinCharData, pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
			pSkeleton = pSkinChar->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	case EOBJ_ATTBONE:
		{
			DxAttBone *pAttBone = pView->GetAttBone();
			DxAttBoneData* pAttBoneData = pView->GetAttBoneData();
			pAttBone->SetAttBoneData( pAttBoneData, pView->GetD3dDevice(), false, 0, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE, NULL );
			pSkeleton = pAttBone->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	case EOBJ_VEHICLE:
		{
			DxVehicle *pVehicle = pView->GetVehicle();
			DxVehicleData* pVehicleData = pView->GetVehicleData ();
			pVehicle->SetVehiclePartData( pVehicleData, pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
			pSkeleton = pVehicle->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	default:
		break;
	}
	if ( !pSkeleton )
		return;

	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );

	if ( pJiggleBoneData )
	{
		if( nIndex!=LB_ERR )	
		{
			g_vJIGGLE_BONE_ROTATE = pJiggleBoneData->m_vecJointData[ nIndex ]->m_vRotate;
		}

		UpdateListJoint( pJiggleBoneData );
	}
}

void CJiggleBonePage::OnEnChangeEditDensity()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_DENSITY );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_fDensity = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditRotateX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_ROTATE_X );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_vRotate.x = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditRotateY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_ROTATE_Y );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_vRotate.y = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditRotateZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_ROTATE_Z );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_vRotate.z = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditTwistRestitution()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_TWIST_RESTITUTION );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_fTwistRestitution = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditTwistSpring()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_TWIST_SPRING );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_fTwistSpring = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditTwistDamping()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_TWIST_DAMPING );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_fTwistDamping = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditSwingRestitution()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_SWING_RESTITUTION );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_fSwingRestitution = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditSwingSpring()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_SWING_SPRING );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_fSwingSpring = (float)_tstof( Str.GetString() );
}

void CJiggleBonePage::OnEnChangeEditSwingDamping()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneData* pJiggleBoneData;
	int nIndex = GetCurSelJoint( &pJiggleBoneData );
	if ( nIndex == LB_ERR )
		return;

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_SWING_DAMPING );
	pWnd->GetWindowText( Str );
	pJiggleBoneData->m_vecJointData[ nIndex ]->m_fSwingDamping = (float)_tstof( Str.GetString() );
}
