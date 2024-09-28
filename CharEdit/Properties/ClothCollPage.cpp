// JiggleBonePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "FramePage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxClothColl.h"

#include "JiggleBoneCollPage.h"


extern TSTRING	g_strPICK_BONE;
extern INT		g_nSELECT_COLL_BONE;

// CClothCollPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CClothCollPage, CPropertyPage)

CClothCollPage::CClothCollPage()
	: CPropertyPage(CClothCollPage::IDD)
	, m_pClothColl(NULL)
{

}

CClothCollPage::~CClothCollPage()
{
}

void CClothCollPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COLLISION_MESH, m_listCollisionMesh);
}

BEGIN_MESSAGE_MAP(CClothCollPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CClothCollPage::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CClothCollPage::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, &CClothCollPage::OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CClothCollPage::OnBnClickedButtonDelete)
	ON_LBN_SELCHANGE(IDC_LIST_COLLISION_MESH, &CClothCollPage::OnLbnSelchangeListCollisionMesh)
END_MESSAGE_MAP()


// CClothCollPage 메시지 처리기입니다.

void CClothCollPage::SetData( DxClothColl* pJiggleBoneColl )
{
	m_pClothColl = pJiggleBoneColl;
}

// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

void CClothCollPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedButtonApply();

	m_pSheetTabChar->SetActivePage( CHARTAB_BONE_SETTING );
}

BOOL CClothCollPage::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	// Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();

	UpdateListCollisionMesh();

	return CPropertyPage::OnSetActive();
}

BOOL CClothCollPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();

	pView->DeActiveEditMatrix();

	g_nSELECT_COLL_BONE = -1;

	return CPropertyPage::OnKillActive();
}

void CClothCollPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	ClothCollData* pJiggleBoneCollData;
	int nIndex = GetCurSelCollisionData( &pJiggleBoneCollData );
	if ( nIndex == LB_ERR )
	{
		// 모습 표현
		SetCharData();

		return;
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

	pWnd = GetDlgItem( IDC_EDIT_SIZE );
	pWnd->GetWindowText( Str );
	pJiggleBoneCollData->m_fRadius = (float)_tstof( Str.GetString() );

	// 모습 표현
	SetCharData();
}

void CClothCollPage::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	const DxSkeleton* pSkeleton(NULL);
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

	m_pClothColl->Insert( pSkeleton, g_strPICK_BONE );

	// Note : List 정리
	UpdateListCollisionMesh();

	m_listCollisionMesh.SetCurSel( m_listCollisionMesh.GetCount()-1 );
	g_nSELECT_COLL_BONE = m_listCollisionMesh.GetCount()-1;

	OnLbnSelchangeListCollisionMesh();

	// 모습 표현
	SetCharData();
}

void CClothCollPage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	int nIndex = m_listCollisionMesh.GetCurSel();
	if( nIndex==LB_ERR )	
		return;

	const DxSkeleton* pSkeleton(NULL);
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
	}
	if ( !pSkeleton )
		return;

	m_pClothColl->Delete( pSkeleton, nIndex );

	g_nSELECT_COLL_BONE = -1;

	// Note : List 정리
	UpdateListCollisionMesh();

	// 모습 표현
	SetCharData();
}

void CClothCollPage::SetCharData()
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

	// 정보들을 Refresh 해 줘야한다.
	NSMaterialSkinManager::OnlyVersionUpForTool();

	// FrameMove 를 위한 정보를 Refresh 한다.
	m_pClothColl->RefreshCollisionMeshForFrameMove();
}

void CClothCollPage::UpdateListCollisionMesh()
{
	m_listCollisionMesh.ResetContent();

	DxClothColl::VEC_CLOTH_COLLDATA_ITER iter = m_pClothColl->m_vecCollisionMeshData.begin();
	for( ; iter!=m_pClothColl->m_vecCollisionMeshData.end(); ++iter )
	{
		int nIndex = m_listCollisionMesh.AddString(  (*iter)->m_strBoneName.c_str() );
		m_listCollisionMesh.SetItemData( nIndex, nIndex );
	}
}

int CClothCollPage::GetCurSelCollisionData( ClothCollData** ppJiggleBoneCollData )
{
	int nIndex = m_listCollisionMesh.GetCurSel();
	if( nIndex==LB_ERR )	
		return LB_ERR;

	(*ppJiggleBoneCollData) = m_pClothColl->GetClothCollData( nIndex );
	if ( !(*ppJiggleBoneCollData) )
		return LB_ERR;

	return nIndex;
}

void CClothCollPage::OnLbnSelchangeListCollisionMesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ClothCollData* pJiggleBoneCollData;
	int nIndex = GetCurSelCollisionData( &pJiggleBoneCollData );
	if ( nIndex == LB_ERR )
		return;

	g_nSELECT_COLL_BONE = nIndex;

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

	pWnd = GetDlgItem( IDC_EDIT_SIZE );
	Str.Format ( _T("%3.1f"), pJiggleBoneCollData->m_fRadius );
	pWnd->SetWindowText( Str.GetString() );
}