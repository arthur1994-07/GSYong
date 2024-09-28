// AniDivPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "SheetWithTabAnim.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "AniDivPage.h"


// CAniDivPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAniDivPage, CPropertyPage)
CAniDivPage::CAniDivPage()
	: CPropertyPage(CAniDivPage::IDD)
{
}

CAniDivPage::~CAniDivPage()
{
}

void CAniDivPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANIDIV, m_AniDivList);
}


BEGIN_MESSAGE_MAP(CAniDivPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAniDivPage 메시지 처리기입니다.
//
void CAniDivPage::SetAnimName ( CString sName )
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	m_sAnimName = sName;

	ListtingAniDiv ();
	FrameMarking ();
}

void CAniDivPage::FrameMarking ( BOOL bReset )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	if ( bReset )
	{
		pFrame->m_cDlgTimeLine.SetFrameMarking ( 0 );
	}
	else
	{
		const ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
		if ( !pAnimNodeCur )	return;
		const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

		const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
		if ( !pSelectedAniCont )	return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;


		WORD wDivFrames[ACF_DIV];

		for ( WORD i=0; i<pAnimCont->m_wDivCount; ++i )
		{
			wDivFrames[i] = (WORD) ( pAnimCont->m_wDivFrame[i] / dwUnitTime );
		}

		pFrame->m_cDlgTimeLine.SetFrameMarking ( pAnimCont->m_wDivCount, wDivFrames );
	}
}

void CAniDivPage::ListtingAniDiv ()
{
	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	
		return;
	const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;

	m_AniDivList.ResetContent ();

	const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
	if ( !pSelectedAniCont )	
		return;
	DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;

	int nIndex = 0;
	for ( int i=0; i<pAnimCont->m_wDivCount; ++i )
	{
		CString Str;
		Str.Format ( "[%d]  %d", i, pAnimCont->m_wDivFrame[i] / dwUnitTime );

		nIndex = m_AniDivList.AddString ( Str );
		m_AniDivList.SetItemData ( nIndex, i );
	}
}

namespace
{
	void Sorting ( WORD* pwArray, WORD wCount )
	{
		WORD *pTempArray = new WORD[wCount];
		SecureZeroMemory ( pTempArray, sizeof(WORD)*wCount );

		WORD wMark = 0;
		for ( WORD i=0; i<wCount; ++i )
		{
			wMark = 0;
			WORD &wThis = pTempArray[i] = 0xFFFF;

			for ( WORD j=0; j<wCount; ++j )
			{
				if ( pwArray[j] < wThis )
				{
					wThis = pwArray[j];
					wMark = j;
				}
			}

			pwArray[wMark] = 0xFFFF;
		}

		memcpy ( pwArray, pTempArray, sizeof(WORD)*wCount );
	}
};

void CAniDivPage::OnBnClickedButtonAdd ()
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();	




	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

		DxSkinAnimation* pAnimCont = pSkinCharData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if( !pAnimCont )
			return;

		const DxSkinAnimation* pSelectedAniCont = pSkinChar->m_spSkinAniControlThread->GETCURANIM();
		if ( !pSelectedAniCont )	return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;


		if ( pAnimCont->m_wDivCount >= ACF_DIV )
		{
			CString Str;
			Str.Format ( "구분동작 키는 %d 개 까지만 가능합니다.", ACF_DIV );
			MessageBox ( Str );
			return;
		}

		DWORD dwThisTime = (DWORD) pSkinChar->GETCURTIME ();

		for ( WORD i=0; i<pAnimCont->m_wDivCount; ++i )
		{
			if ( pAnimCont->m_wDivFrame[i] == (WORD)dwThisTime )
			{
				CString Str;
				Str.Format ( "[%d] 구분동작 키로 이미 등록되었습니다.", dwThisTime/dwUnitTime );
				MessageBox ( Str );
				return;
			}
		}

		pAnimCont->m_wDivFrame[pAnimCont->m_wDivCount] = (WORD) dwThisTime;
		pAnimCont->m_wDivCount++;

		Sorting ( pAnimCont->m_wDivFrame, pAnimCont->m_wDivCount );
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone ();
		DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

		DxSkinAnimation* pAnimCont = pAttBoneData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if( !pAnimCont )
			return;

		const DxSkinAnimation* pSelectedAniCont = pAttBone->m_spSkinAniControlThread->GETCURANIM();
		if ( !pSelectedAniCont )	
			return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;


		if ( pAnimCont->m_wDivCount >= ACF_DIV )
		{
			CString Str;
			Str.Format ( "구분동작 키는 %d 개 까지만 가능합니다.", ACF_DIV );
			MessageBox ( Str );
			return;
		}

		DWORD dwThisTime = (DWORD) pAttBone->m_spSkinAniControlThread->GETCURTIME ();

		for ( WORD i=0; i<pAnimCont->m_wDivCount; ++i )
		{
			if ( pAnimCont->m_wDivFrame[i] == (WORD)dwThisTime )
			{
				CString Str;
				Str.Format ( "[%d] 구분동작 키로 이미 등록되었습니다.", dwThisTime/dwUnitTime );
				MessageBox ( Str );
				return;
			}
		}

		pAnimCont->m_wDivFrame[pAnimCont->m_wDivCount] = (WORD) dwThisTime;
		pAnimCont->m_wDivCount++;

		Sorting ( pAnimCont->m_wDivFrame, pAnimCont->m_wDivCount );
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle ();
		DxVehicleData *pVehicleData = pView->GetVehicleData();

		DxSkinAnimation* pAnimCont = pVehicleData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if( !pAnimCont )
			return;

		const DxSkinAnimation* pSelectedAniCont = pVehicle->m_spSkinAniControlThread->GETCURANIM();
		if ( !pSelectedAniCont )	
			return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;


		if ( pAnimCont->m_wDivCount >= ACF_DIV )
		{
			CString Str;
			Str.Format ( "구분동작 키는 %d 개 까지만 가능합니다.", ACF_DIV );
			MessageBox ( Str );
			return;
		}

		DWORD dwThisTime = (DWORD) pVehicle->m_spSkinAniControlThread->GETCURTIME ();

		for ( WORD i=0; i<pAnimCont->m_wDivCount; ++i )
		{
			if ( pAnimCont->m_wDivFrame[i] == (WORD)dwThisTime )
			{
				CString Str;
				Str.Format ( "[%d] 구분동작 키로 이미 등록되었습니다.", dwThisTime/dwUnitTime );
				MessageBox ( Str );
				return;
			}
		}

		pAnimCont->m_wDivFrame[pAnimCont->m_wDivCount] = (WORD) dwThisTime;
		pAnimCont->m_wDivCount++;

		Sorting ( pAnimCont->m_wDivFrame, pAnimCont->m_wDivCount );
	}

	// 모션이 변경되어서 작업하지 않는다. 필요하다면 해줘야한다.
	//m_pSheetTab->ApplyAniContrl();

	ListtingAniDiv ();
	FrameMarking ();
}

void CAniDivPage::OnBnClickedButtonDel ()
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();	

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

		DxSkinAnimation* pAnimCont = pSkinCharData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if( !pAnimCont )
			return;

		int nIndex = m_AniDivList.GetCurSel ();
		if ( nIndex==LB_ERR )	return;
		int nArrayIndex = (int) m_AniDivList.GetItemData ( nIndex );
		
		pAnimCont->m_wDivFrame[nArrayIndex] = 0;

		for ( WORD i=nArrayIndex+1; i<pAnimCont->m_wDivCount; ++i )
		{
			pAnimCont->m_wDivFrame[i-1] = pAnimCont->m_wDivFrame[i];
		}

		pAnimCont->m_wDivCount--;
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone ();
		DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

		DxSkinAnimation* pAnimCont = pAttBoneData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if( !pAnimCont )
			return;

		int nIndex = m_AniDivList.GetCurSel ();
		if ( nIndex==LB_ERR )	return;
		int nArrayIndex = (int) m_AniDivList.GetItemData ( nIndex );

		pAnimCont->m_wDivFrame[nArrayIndex] = 0;

		for ( WORD i=nArrayIndex+1; i<pAnimCont->m_wDivCount; ++i )
		{
			pAnimCont->m_wDivFrame[i-1] = pAnimCont->m_wDivFrame[i];
		}

		pAnimCont->m_wDivCount--;
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle ();
		DxVehicleData *pVehicleData = pView->GetVehicleData();

		DxSkinAnimation* pAnimCont = pVehicleData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if( !pAnimCont )
			return;

		int nIndex = m_AniDivList.GetCurSel ();
		if ( nIndex==LB_ERR )	return;
		int nArrayIndex = (int) m_AniDivList.GetItemData ( nIndex );

		pAnimCont->m_wDivFrame[nArrayIndex] = 0;

		for ( WORD i=nArrayIndex+1; i<pAnimCont->m_wDivCount; ++i )
		{
			pAnimCont->m_wDivFrame[i-1] = pAnimCont->m_wDivFrame[i];
		}

		pAnimCont->m_wDivCount--;
	}

	m_pSheetTab->ApplyAniContrl();

	ListtingAniDiv ();
	FrameMarking ();
}

void CAniDivPage::OnBnClickedOk ()
{
	FrameMarking ( TRUE );

	m_sAnimName = "";
	m_pSheetTab->SetActivePage ( ANIMSETPAGEINDEX );
}

