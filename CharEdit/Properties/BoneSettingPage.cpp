// JiggleBonePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "FramePage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../enginelib/DxMeshs/SkinMesh/DxAttBoneData.h"

#include "SheetWithTabChar.h"

#include "BoneSettingPage.h"


// CBoneSettingPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBoneSettingPage, CPropertyPage)

CBoneSettingPage::CBoneSettingPage()
	: CPropertyPage(CBoneSettingPage::IDD)
{

}

CBoneSettingPage::~CBoneSettingPage()
{
}

void CBoneSettingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBoneSettingPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CBoneSettingPage::OnBnClickedOk)
	ON_BN_CLICKED(IDOK3, &CBoneSettingPage::OnBnClickedDontSaveOK)
	ON_BN_CLICKED(IDC_BUTTON_FOOT_COLL, &CBoneSettingPage::OnBnClickedButtonFootColl)
	ON_BN_CLICKED(IDC_BUTTON_JIGGLE_BONE, &CBoneSettingPage::OnBnClickedButtonJiggleBone)
	ON_BN_CLICKED(IDC_BUTTON_COLLISION_OBJECT, &CBoneSettingPage::OnBnClickedButtonCollisionObject)
	ON_BN_CLICKED(IDC_BUTTON_CLOTH_COLLISION, &CBoneSettingPage::OnBnClickedButtonClothCollision)
	ON_BN_CLICKED(IDC_BUTTON_DOCUMENT, &CBoneSettingPage::OnBnClickedButtonDocument)
END_MESSAGE_MAP()


// CBoneSettingPage 메시지 처리기입니다.

// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

void CBoneSettingPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// Bone Setting File 을 Save 하도록 한다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DxSkinAniControlData *pSkinAniControlData(NULL);
	
	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		pSkinAniControlData = pView->GetSkinCharData()->GetSkinAniControlData().get();
		break;
	case EOBJ_ATTBONE:
		pSkinAniControlData = pView->GetAttBoneData()->GetSkinAniControlData().get();
		break;
	case EOBJ_VEHICLE:
		pSkinAniControlData = pView->GetVehicleData()->GetSkinAniControlData().get();
		break;
	};
	
	if ( pSkinAniControlData )
	{
		pSkinAniControlData->SaveFile_BSF();
	}

	// Page 를 변경시킨다.
	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE );
		break;
	case EOBJ_ATTBONE:
		m_pSheetTabChar->SetActivePage( CHARTAB_ATTBONEFILE );
		break;
	case EOBJ_VEHICLE:
		m_pSheetTabChar->SetActivePage( CHARTAB_VEHICLE );
		break;
	};
}

void CBoneSettingPage::OnBnClickedDontSaveOK()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// Bone Setting File 을 Save 하도록 한다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	// Page 를 변경시킨다.
	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		m_pSheetTabChar->SetActivePage( CHARTAB_CHARFILE );
		break;
	case EOBJ_ATTBONE:
		m_pSheetTabChar->SetActivePage( CHARTAB_ATTBONEFILE );
		break;
	case EOBJ_VEHICLE:
		m_pSheetTabChar->SetActivePage( CHARTAB_VEHICLE );
		break;
	};
}

void CBoneSettingPage::OnBnClickedButtonFootColl()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		pFrame->m_cDlgEdit.SetAcitveSheetPage( CHARTAB_FOOT_COLL );
		break;
	case EOBJ_VEHICLE:
		pFrame->m_cDlgEdit.SetAcitveSheetPage( CHARTAB_FOOT_COLL );
		break;
	case EOBJ_ATTBONE:
		AfxMessageBox( _T("오류-FootCollPage는 캐릭터파일, Vehicle파일에서만 접근이 가능합니다") );
		break;
	};
}

void CBoneSettingPage::OnBnClickedButtonJiggleBone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		pFrame->m_cDlgEdit.SetActivePage_JiggleBone( CHARTAB_JIGGLE_BONE, const_cast<DxJiggleBone*>( pView->GetSkinCharData()->GetSkinAniControlData()->GetJiggleBone() ) );
		break;
	case EOBJ_ATTBONE:
		pFrame->m_cDlgEdit.SetActivePage_JiggleBone( CHARTAB_JIGGLE_BONE, const_cast<DxJiggleBone*>( pView->GetAttBoneData()->GetSkinAniControlData()->GetJiggleBone() ) );
		break;
	case EOBJ_VEHICLE:
		pFrame->m_cDlgEdit.SetActivePage_JiggleBone( CHARTAB_JIGGLE_BONE, const_cast<DxJiggleBone*>( pView->GetVehicleData()->GetSkinAniControlData()->GetJiggleBone() ) );
		break;
	};
}

void CBoneSettingPage::OnBnClickedButtonCollisionObject()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		pFrame->m_cDlgEdit.SetActivePage_JiggleBoneColl( CHARTAB_JIGGLE_BONE_COLL, const_cast<DxJiggleBoneColl*>( pView->GetSkinCharData()->GetSkinAniControlData()->GetJiggleBoneColl() ) );
		break;
	case EOBJ_ATTBONE:
		pFrame->m_cDlgEdit.SetActivePage_JiggleBoneColl( CHARTAB_JIGGLE_BONE_COLL, const_cast<DxJiggleBoneColl*>( pView->GetAttBoneData()->GetSkinAniControlData()->GetJiggleBoneColl() ) );
		break;
	case EOBJ_VEHICLE:
		pFrame->m_cDlgEdit.SetActivePage_JiggleBoneColl( CHARTAB_JIGGLE_BONE_COLL, const_cast<DxJiggleBoneColl*>( pView->GetVehicleData()->GetSkinAniControlData()->GetJiggleBoneColl() ) );
		break;
	};
}

void CBoneSettingPage::OnBnClickedButtonClothCollision()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		pFrame->m_cDlgEdit.SetActivePage_ClothColl( CHARTAB_CLOTH_COLL, const_cast<DxClothColl*>( pView->GetSkinCharData()->GetSkinAniControlData()->GetClothColl() ) );
		break;
	case EOBJ_ATTBONE:
		AfxMessageBox( _T("오류-ClothCollision는 캐릭터파일에서만 접근이 가능합니다") );
		//pFrame->m_cDlgEdit.SetActivePage_ClothColl( CHARTAB_CLOTH_COLL, const_cast<DxClothColl*>( pView->GetAttBoneData()->GetClothColl() ) );
		break;
	case EOBJ_VEHICLE:
		AfxMessageBox( _T("오류ClothCollision는 캐릭터파일파일에서만 접근이 가능합니다") );
		//pFrame->m_cDlgEdit.SetActivePage_ClothColl( CHARTAB_CLOTH_COLL, const_cast<DxClothColl*>( pView->GetVehicleData()->GetClothColl() ) );
		break;
	};
}

void CBoneSettingPage::OnBnClickedButtonDocument()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TSTRING strText = 
		"여기서 셋팅하는것은 상단에서 설정된 bone의 확장자만 변경된(bsf(Bone Setting File)) 상태로 저장되게 된다. \r\n"
		"만약 같은 본을 쓰는 chf는 다 영향을 받게 된다. \r\n"
		"예로 b_w.x 가 셋팅되어있었다면, 저장시 b_w.bsf 로 저장되고, b_w.x 를 사용한 chf 들은 같이 영향을 받는다. \r\n"
		"\r\n"
		"\r\n"
		"[Foot Collision] - 캐릭터의 몸이 지형과 비슷하게 기울어져 있는게 필요하다면 여기서 셋팅해야한다. \r\n"
		"\r\n"
		"[Jiggle Bone] - PhysX 의 Joint 를 활용하여 제작한 Jiggle Bone. \r\n"
		"\r\n"
		"[Collision Object ( Jiggle Bone )] - Jiggle Bone 만 연동가능한 Collsion Object 를 생성 할 수 있다. \r\n"
		"\r\n"
		"[Collision Object ( Cloth )] - Cloth 만 연동가능한 Collsion Cylinder 를 생성 할 수 있다. \r\n"
		"\r\n"
		"[Don`t Save && OK] - bsf 에 save 는 하지 않는다. 하지만 셋팅된 것을 유지된다. Tool 을 끄면 변경 정보는 사용 불가능하다. \r\n"
		"\r\n"
		"[Save && OK] - bsf 에 save 를 한다. \r\n"
		;

	AfxMessageBox( strText.c_str() );
}
