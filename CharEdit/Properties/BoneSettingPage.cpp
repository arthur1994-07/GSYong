// JiggleBonePage.cpp : ���� �����Դϴ�.
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


// CBoneSettingPage ��ȭ �����Դϴ�.

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


// CBoneSettingPage �޽��� ó�����Դϴ�.

// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

void CBoneSettingPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// Bone Setting File �� Save �ϵ��� �Ѵ�.
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

	// Page �� �����Ų��.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// Bone Setting File �� Save �ϵ��� �Ѵ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	// Page �� �����Ų��.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		AfxMessageBox( _T("����-FootCollPage�� ĳ��������, Vehicle���Ͽ����� ������ �����մϴ�") );
		break;
	};
}

void CBoneSettingPage::OnBnClickedButtonJiggleBone()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	switch( pView->m_emObjType )
	{
	case EOBJ_SKINCHAR:
		pFrame->m_cDlgEdit.SetActivePage_ClothColl( CHARTAB_CLOTH_COLL, const_cast<DxClothColl*>( pView->GetSkinCharData()->GetSkinAniControlData()->GetClothColl() ) );
		break;
	case EOBJ_ATTBONE:
		AfxMessageBox( _T("����-ClothCollision�� ĳ�������Ͽ����� ������ �����մϴ�") );
		//pFrame->m_cDlgEdit.SetActivePage_ClothColl( CHARTAB_CLOTH_COLL, const_cast<DxClothColl*>( pView->GetAttBoneData()->GetClothColl() ) );
		break;
	case EOBJ_VEHICLE:
		AfxMessageBox( _T("����ClothCollision�� ĳ�����������Ͽ����� ������ �����մϴ�") );
		//pFrame->m_cDlgEdit.SetActivePage_ClothColl( CHARTAB_CLOTH_COLL, const_cast<DxClothColl*>( pView->GetVehicleData()->GetClothColl() ) );
		break;
	};
}

void CBoneSettingPage::OnBnClickedButtonDocument()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	TSTRING strText = 
		"���⼭ �����ϴ°��� ��ܿ��� ������ bone�� Ȯ���ڸ� �����(bsf(Bone Setting File)) ���·� ����ǰ� �ȴ�. \r\n"
		"���� ���� ���� ���� chf�� �� ������ �ް� �ȴ�. \r\n"
		"���� b_w.x �� ���õǾ��־��ٸ�, ����� b_w.bsf �� ����ǰ�, b_w.x �� ����� chf ���� ���� ������ �޴´�. \r\n"
		"\r\n"
		"\r\n"
		"[Foot Collision] - ĳ������ ���� ������ ����ϰ� ������ �ִ°� �ʿ��ϴٸ� ���⼭ �����ؾ��Ѵ�. \r\n"
		"\r\n"
		"[Jiggle Bone] - PhysX �� Joint �� Ȱ���Ͽ� ������ Jiggle Bone. \r\n"
		"\r\n"
		"[Collision Object ( Jiggle Bone )] - Jiggle Bone �� ���������� Collsion Object �� ���� �� �� �ִ�. \r\n"
		"\r\n"
		"[Collision Object ( Cloth )] - Cloth �� ���������� Collsion Cylinder �� ���� �� �� �ִ�. \r\n"
		"\r\n"
		"[Don`t Save && OK] - bsf �� save �� ���� �ʴ´�. ������ ���õ� ���� �����ȴ�. Tool �� ���� ���� ������ ��� �Ұ����ϴ�. \r\n"
		"\r\n"
		"[Save && OK] - bsf �� save �� �Ѵ�. \r\n"
		;

	AfxMessageBox( strText.c_str() );
}
