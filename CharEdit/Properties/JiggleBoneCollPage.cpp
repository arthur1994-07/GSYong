// JiggleBonePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "FramePage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxJiggleBoneColl.h"

#include "JiggleBoneCollPage.h"


extern TSTRING	g_strPICK_BONE;
extern INT		g_nSELECT_COLL_BONE;

// CJiggleBoneCollPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CJiggleBoneCollPage, CPropertyPage)

CJiggleBoneCollPage::CJiggleBoneCollPage()
	: CPropertyPage(CJiggleBoneCollPage::IDD)
	, m_pJiggleBoneColl(NULL)
{

}

CJiggleBoneCollPage::~CJiggleBoneCollPage()
{
}

void CJiggleBoneCollPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COLLISION_MESH, m_listCollisionMesh);
}

BEGIN_MESSAGE_MAP(CJiggleBoneCollPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CJiggleBoneCollPage::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CJiggleBoneCollPage::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, &CJiggleBoneCollPage::OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CJiggleBoneCollPage::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, &CJiggleBoneCollPage::OnBnClickedButtonAuto)	
	ON_BN_CLICKED(IDC_RADIO_BOX, &CJiggleBoneCollPage::OnBnClickedCheckBox)
	ON_BN_CLICKED(IDC_RADIO_SPHERE2, &CJiggleBoneCollPage::OnBnClickedCheckSphere)
	ON_BN_CLICKED(IDC_RADIO_CAPSULE, &CJiggleBoneCollPage::OnBnClickedCheckCapsule)
	ON_LBN_SELCHANGE(IDC_LIST_COLLISION_MESH, &CJiggleBoneCollPage::OnLbnSelchangeListCollisionMesh)
END_MESSAGE_MAP()


// CJiggleBoneCollPage 메시지 처리기입니다.

void CJiggleBoneCollPage::SetData( DxJiggleBoneColl* pJiggleBoneColl )
{
	m_pJiggleBoneColl = pJiggleBoneColl;
}

// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

void CJiggleBoneCollPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedButtonApply();

	m_pSheetTabChar->SetActivePage( CHARTAB_BONE_SETTING );
}

BOOL CJiggleBoneCollPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	// Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();

	UpdateListCollisionMesh();

	return CPropertyPage::OnSetActive();
}

BOOL CJiggleBoneCollPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();

	pView->DeActiveEditMatrix();

	g_nSELECT_COLL_BONE = -1;

	return CPropertyPage::OnKillActive();
}

void CJiggleBoneCollPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	JiggleBoneCollData* pJiggleBoneCollData;
	int nIndex = GetCurSelCollisionData( &pJiggleBoneCollData );
	if ( nIndex == LB_ERR )
	{
		// 모습 표현
		SetCharData();

		return;
	}

	CButton* pRadioBox = (CButton*)GetDlgItem( IDC_RADIO_BOX );
	CButton* pRadioSphere = (CButton*)GetDlgItem( IDC_RADIO_SPHERE2 );
	CButton* pRadioCapsule = (CButton*)GetDlgItem( IDC_RADIO_CAPSULE );

	if ( pRadioBox->GetCheck() )
	{
		pJiggleBoneCollData->m_emMeshType = JiggleBoneCollData::EMMT_BOX;
	}
	else if ( pRadioSphere->GetCheck() )
	{
		pJiggleBoneCollData->m_emMeshType = JiggleBoneCollData::EMMT_SPHERE;
	}
	else if ( pRadioCapsule->GetCheck() )
	{
		pJiggleBoneCollData->m_emMeshType = JiggleBoneCollData::EMMT_CAPSULE;
	}

	CWnd* pWnd;
	CString Str;	

	pWnd = GetDlgItem( IDC_EDIT_POSITION_X );
	pWnd->GetWindowText( Str );
	pJiggleBoneCollData->m_vPosition.x = (float)_tstof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_POSITION_Y );
	pWnd->GetWindowText( Str );
	pJiggleBoneCollData->m_vPosition.y = (float)_tstof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_POSITION_Z );
	pWnd->GetWindowText( Str );
	pJiggleBoneCollData->m_vPosition.z = (float)_tstof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_1 );
	pWnd->GetWindowText( Str );
	pJiggleBoneCollData->m_vHalfScale.x = (float)_tstof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_2 );
	pWnd->GetWindowText( Str );
	pJiggleBoneCollData->m_vHalfScale.y = (float)_tstof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_3 );
	pWnd->GetWindowText( Str );
	pJiggleBoneCollData->m_vHalfScale.z = (float)_tstof( Str.GetString() );

	// 모습 표현
	SetCharData();
}

void CJiggleBoneCollPage::OnBnClickedButtonInsert()
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

	m_pJiggleBoneColl->Insert( pSkeleton, g_strPICK_BONE );

	// Note : List 정리
	UpdateListCollisionMesh();

	m_listCollisionMesh.SetCurSel( m_listCollisionMesh.GetCount()-1 );
	g_nSELECT_COLL_BONE = m_listCollisionMesh.GetCount()-1;

	OnLbnSelchangeListCollisionMesh();

	// 모습 표현
	SetCharData();
}

void CJiggleBoneCollPage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listCollisionMesh.GetCurSel();
	if( nIndex==LB_ERR )	
		return;

	m_pJiggleBoneColl->Delete( nIndex );

	g_nSELECT_COLL_BONE = -1;

	// Note : List 정리
	UpdateListCollisionMesh();

	// 모습 표현
	SetCharData();
}

void CJiggleBoneCollPage::SetCharData()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		{
			DxSkinChar *pSkinChar = pView->GetSkinChar();
			DxSkinCharData* pSkinCharData = pView->GetSkinCharData();
			pSkinChar->SetCharData( pSkinCharData, pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
		}
		break;
	case EOBJ_ATTBONE:
		{
			DxAttBone *pAttBone = pView->GetAttBone();
			DxAttBoneData* pAttBoneData = pView->GetAttBoneData();
			pAttBone->SetAttBoneData( pAttBoneData, pView->GetD3dDevice(), false, 0, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE, NULL );
		}
		break;
	case EOBJ_VEHICLE:
		{
			DxVehicle *pVehicle = pView->GetVehicle();
			DxVehicleData* pVehicleData = pView->GetVehicleData ();
			pVehicle->SetVehiclePartData( pVehicleData, pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
		}
		break;
	default:
		break;
	};
}

void CJiggleBoneCollPage::OnBnClickedButtonAuto()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DxSkeleton* pSkeleton(NULL);
	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		{
			DxSkinChar *pSkinChar = pView->GetSkinChar();
			pSkeleton = pSkinChar->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	case EOBJ_ATTBONE:
		{
			DxAttBone *pAttBone = pView->GetAttBone();
			pSkeleton = pAttBone->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	case EOBJ_VEHICLE:
		{
			DxVehicle *pVehicle = pView->GetVehicle();
			pSkeleton = pVehicle->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	default:
		break;
	};

	JiggleBoneCollData* pJiggleBoneCollData;
	int nIndex = GetCurSelCollisionData( &pJiggleBoneCollData );
	if ( nIndex == LB_ERR )
		return;

	if ( pJiggleBoneCollData->m_nBoneIndex == -1 )
		return;

	const DxBoneTrans* pBoneTrans = pSkeleton->FindBone_Index_Bone( pJiggleBoneCollData->m_nBoneIndex );
	if ( !pBoneTrans )
		return;

	OnBnClickedButtonApply();

	float fLength(0.f);
	switch ( pJiggleBoneCollData->m_emMeshType )
	{
	case JiggleBoneCollData::EMMT_BOX:
		{
			fLength = pBoneTrans->pBoneTransFirstChild->m_pQuatPosORIG.m_vPos.x + pJiggleBoneCollData->m_vHalfScale.y + pJiggleBoneCollData->m_vHalfScale.z;

			pJiggleBoneCollData->m_vPosition.x = pBoneTrans->pBoneTransFirstChild->m_pQuatPosORIG.m_vPos.x * 0.5f;
			pJiggleBoneCollData->m_vPosition.y = 0.f;
			pJiggleBoneCollData->m_vPosition.z = 0.f;
			pJiggleBoneCollData->m_vHalfScale.x = fLength*0.5f;
		}
		break;

	case JiggleBoneCollData::EMMT_SPHERE:
		pJiggleBoneCollData->m_vPosition.x = 0.f;
		pJiggleBoneCollData->m_vPosition.y = 0.f;
		pJiggleBoneCollData->m_vPosition.z = 0.f;
		break;

	case JiggleBoneCollData::EMMT_CAPSULE:
		{
			pJiggleBoneCollData->m_vPosition.x = pBoneTrans->pBoneTransFirstChild->m_pQuatPosORIG.m_vPos.x * 0.5f;
			pJiggleBoneCollData->m_vPosition.y = 0.f;
			pJiggleBoneCollData->m_vPosition.z = 0.f;
			pJiggleBoneCollData->m_vHalfScale.x = pBoneTrans->pBoneTransFirstChild->m_pQuatPosORIG.m_vPos.x * 0.5f;
		}
		break;

	default:
		break;
	};

	CWnd* pWnd;
	CString Str;	

	pWnd = GetDlgItem( IDC_EDIT_POSITION_X );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vPosition.x );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_POSITION_Y );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vPosition.y );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_POSITION_Z );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vPosition.z );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_1 );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vHalfScale.x );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_2 );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vHalfScale.y );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_3 );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vHalfScale.z );
	pWnd->SetWindowText( Str.GetString() );

	// 모습 표현
	SetCharData();
}

void CJiggleBoneCollPage::OnBnClickedCheckBox()
{
	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_1 );
	pWnd->ShowWindow( TRUE );
	pWnd->SetWindowText( _T("Half X") );

	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_2 );
	pWnd->ShowWindow( TRUE );
	pWnd->SetWindowText( _T("Half Y") );

	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_3 );
	pWnd->ShowWindow( TRUE );
	pWnd->SetWindowText( _T("Half Z") );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_1 );
	pWnd->ShowWindow( TRUE );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_2 );
	pWnd->ShowWindow( TRUE );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_3 );
	pWnd->ShowWindow( TRUE );
}

void CJiggleBoneCollPage::OnBnClickedCheckSphere()
{
	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_1 );
	pWnd->ShowWindow( FALSE );
	pWnd->SetWindowText( _T("NULL") );

	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_2 );
	pWnd->ShowWindow( TRUE );
	pWnd->SetWindowText( _T("Radius") );

	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_3 );
	pWnd->ShowWindow( FALSE );
	pWnd->SetWindowText( _T("NULL") );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_1 );
	pWnd->ShowWindow( FALSE );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_2 );
	pWnd->ShowWindow( TRUE );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_3 );
	pWnd->ShowWindow( FALSE );
}

void CJiggleBoneCollPage::OnBnClickedCheckCapsule()
{
	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_1 );
	pWnd->ShowWindow( TRUE );
	pWnd->SetWindowText( _T("Half Y") );

	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_2 );
	pWnd->ShowWindow( TRUE );
	pWnd->SetWindowText( _T("Radius") );

	pWnd = GetDlgItem( IDC_STATIC_TYPE_VALUE_3 );
	pWnd->ShowWindow( FALSE );
	pWnd->SetWindowText( _T("NULL") );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_1 );
	pWnd->ShowWindow( TRUE );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_2 );
	pWnd->ShowWindow( TRUE );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_3 );
	pWnd->ShowWindow( FALSE );
}

void CJiggleBoneCollPage::UpdateListCollisionMesh()
{
	m_listCollisionMesh.ResetContent();

	DxJiggleBoneColl::VEC_JIGGLEBONECOLLDATA_ITER iter = m_pJiggleBoneColl->m_vecCollisionMeshData.begin();
	for( ; iter!=m_pJiggleBoneColl->m_vecCollisionMeshData.end(); ++iter )
	{
		int nIndex = m_listCollisionMesh.AddString(  (*iter)->m_strBoneName.c_str() );
		m_listCollisionMesh.SetItemData( nIndex, nIndex );
	}
}

int CJiggleBoneCollPage::GetCurSelCollisionData( JiggleBoneCollData** ppJiggleBoneCollData )
{
	int nIndex = m_listCollisionMesh.GetCurSel();
	if( nIndex==LB_ERR )	
		return LB_ERR;

	(*ppJiggleBoneCollData) = m_pJiggleBoneColl->GetJiggleBoneCollData( nIndex );
	if ( !(*ppJiggleBoneCollData) )
		return LB_ERR;

	return nIndex;
}

void CJiggleBoneCollPage::OnLbnSelchangeListCollisionMesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	JiggleBoneCollData* pJiggleBoneCollData;
	int nIndex = GetCurSelCollisionData( &pJiggleBoneCollData );
	if ( nIndex == LB_ERR )
		return;

	g_nSELECT_COLL_BONE = nIndex;

	CButton* pRadioBox = (CButton*)GetDlgItem( IDC_RADIO_BOX );
	CButton* pRadioSphere = (CButton*)GetDlgItem( IDC_RADIO_SPHERE2 );
	CButton* pRadioCapsule = (CButton*)GetDlgItem( IDC_RADIO_CAPSULE );

	pRadioBox->SetCheck( FALSE );
	pRadioSphere->SetCheck( FALSE );
	pRadioCapsule->SetCheck( FALSE );

	switch ( pJiggleBoneCollData->m_emMeshType )
	{
	case JiggleBoneCollData::EMMT_BOX:
		pRadioBox->SetCheck( TRUE );
		OnBnClickedCheckBox();
		break;

	case JiggleBoneCollData::EMMT_SPHERE:
		pRadioSphere->SetCheck( TRUE );
		OnBnClickedCheckSphere();
		break;

	case JiggleBoneCollData::EMMT_CAPSULE:
		pRadioCapsule->SetCheck( TRUE );
		OnBnClickedCheckCapsule();
		break;

	default:
		break;
	};

	CWnd* pWnd;
	CString Str;
	pWnd = GetDlgItem( IDC_EDIT_POSITION_X );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vPosition.x );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_POSITION_Y );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vPosition.y );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_POSITION_Z );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vPosition.z );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_1 );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vHalfScale.x );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_2 );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vHalfScale.y );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TYPE_VALUE_3 );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_vHalfScale.z );
	pWnd->SetWindowText( Str.GetString() );
}