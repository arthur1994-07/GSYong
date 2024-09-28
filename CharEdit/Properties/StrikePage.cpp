// StrikePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "SheetWithTabAnim.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "StrikePage.h"


// CStrikePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CStrikePage, CPropertyPage)
CStrikePage::CStrikePage()
	: CPropertyPage(CStrikePage::IDD)
{
}

CStrikePage::~CStrikePage()
{
}

void CStrikePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PIECE, m_comboPiece);
	DDX_Control(pDX, IDC_COMBO_EFF, m_comboEff);
	DDX_Control(pDX, IDC_LIST_STRIKES, m_listStrike);
}


BEGIN_MESSAGE_MAP(CStrikePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CStrikePage 메시지 처리기입니다.
BOOL CStrikePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	int nIndex = 0;
	m_comboPiece.ResetContent();
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		nIndex = m_comboPiece.AddString ( DxSkinPiece::m_szPIECETYPE[i] );
		m_comboPiece.SetItemData ( nIndex, i );
	}
	m_comboPiece.SetCurSel ( PIECE_RHAND );

	m_comboEff.ResetContent();
	for ( int i=0; i<EMSF_SIZE; ++i )
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			nIndex = m_comboEff.AddString(CGameTextMan::GetInstance().GetCommentText("STRIKE_EFF", i).GetString());
		else
			nIndex = m_comboEff.AddString ( COMMENT::STRIKE_EFF[i].c_str() );
		m_comboEff.SetItemData ( nIndex, i );
	}
	m_comboEff.SetCurSel ( EMSF_TARGET );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CStrikePage::SetAnimName ( CString sName )
{
	m_sAnimName = sName;

	ListStrike ();
	FrameMarking ();
}

void CStrikePage::ListStrike ()
{
	const ANIMCONTNODE *pAnimNodeCur = m_pSheetTab->GetSkinAniContrl()->findanicontnode ( m_sAnimName.GetString() );
	if ( !pAnimNodeCur )	return;
	const DxSkinAnimation* pAnimCont = pAnimNodeCur->m_pDxSkinAni;
	
	const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
	if ( !pSelectedAniCont )	return;
	DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;

	m_listStrike.ResetContent();
	for ( int i=0; i<pAnimCont->m_wStrikeCount; ++i )
	{
		CString Str;
		Str.Format ("[%d/%d] 부위[%s] ", 
					i, 
					pAnimCont->m_sStrikeEff[i].m_dwFrame/dwUnitTime,
			        DxSkinPiece::m_szPIECETYPE[pAnimCont->m_sStrikeEff[i].m_emPiece]);

		m_listStrike.AddString ( Str );
	}
}

void CStrikePage::FrameMarking ( BOOL bReset )
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


		WORD wFrames[ACF_STRIKE];

		for ( WORD i=0; i<pAnimCont->m_wStrikeCount; ++i )
		{
			wFrames[i] = (WORD) ( pAnimCont->m_sStrikeEff[i].m_dwFrame/dwUnitTime );
		}

		pFrame->m_cDlgTimeLine.SetFrameMarking ( pAnimCont->m_wStrikeCount, wFrames );
	}
}
namespace
{	
	struct FrameIndex
	{
		FrameIndex(const DWORD _dwFrame, const DWORD _dwIndex)
			: dwFrame(_dwFrame), dwIndex(_dwIndex)
		{
		}
		const FrameIndex& operator=(const FrameIndex& _rhs)
		{
			dwFrame = _rhs.dwFrame;
			dwIndex = _rhs.dwIndex;
			return *this;
		}
		DWORD dwFrame;
		DWORD dwIndex;
	};
	typedef std::vector<FrameIndex> FrameArray;
	typedef FrameArray::iterator FrameArrayIter;

	class FrameCompare
	{
	public:		
		bool operator() (const FrameIndex& lValue, const FrameIndex& rValue)
		{
			return lValue.dwFrame < rValue.dwFrame;
		}
	};

	struct SAnimationSet
	{
		SANIMSTRIKE sAnimStrike;		
		VECATTBASE sVectorAttachBone[PIECE_SAVE_SIZE];
		bool bUseStrikeAni[PIECE_SAVE_SIZE];
	};
	typedef std::vector<SAnimationSet> SANIMSET;
	typedef SANIMSET::iterator SANIMSET_ITER;

	void Sorting(SANIMSTRIKE* pArray, SAttachBoneAnimInfo* pAttachBone, WORD wCount)
	{
		FrameArray arrayFrame;

		for ( WORD i=0; i<wCount; ++i )
			arrayFrame.push_back(FrameIndex(pArray[i].m_dwFrame, i));

		std::sort(arrayFrame.begin(), arrayFrame.end(), FrameCompare());

		SANIMSET arrayAnimSet;
		for ( DWORD _i = 0; _i < wCount; ++_i )
		{
			SAnimationSet tempSet;
			tempSet.sAnimStrike = pArray[_i];
			for( DWORD _j = 0; _j < PIECE_SAVE_SIZE; ++_j )
			{
				tempSet.bUseStrikeAni[_j] = pAttachBone[_j].bUseStrikeAni;
				tempSet.sVectorAttachBone[_j] = pAttachBone[_j].vecStrikeAni;
			}
			arrayAnimSet.push_back(tempSet);
		}	
	
		for ( DWORD _i =0; _i < wCount; ++_i )
		{
			SAnimationSet& _sAnimationSet = arrayAnimSet[arrayFrame[_i].dwIndex];
			pArray[_i] = _sAnimationSet.sAnimStrike;
			for( DWORD _j = 0; _j < PIECE_SAVE_SIZE; ++_j )
			{
				pAttachBone[_j].bUseStrikeAni = _sAnimationSet.bUseStrikeAni[_j];
				pAttachBone[_j].vecStrikeAni = _sAnimationSet.sVectorAttachBone[_j];
			}
		}
	}
};

void CStrikePage::OnBnClickedButtonInsert()
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
		if ( !pAnimCont )	
			return;

		const DxSkinAnimation* pSelectedAniCont = pSkinChar->m_spSkinAniControlThread->GETCURANIM();
		if ( !pSelectedAniCont )	return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;

		if ( pAnimCont->m_wStrikeCount >= ACF_STRIKE )
		{
			CString Str;
			Str.Format ( "구분동작 키는 %d 개 까지만 가능합니다.", ACF_STRIKE );
			MessageBox ( Str );
			return;
		}

		DWORD dwThisTime = (DWORD) pSkinChar->GETCURTIME ();
		for ( WORD i=0; i<pAnimCont->m_wStrikeCount; ++i )
		{
			if ( pAnimCont->m_sStrikeEff[i].m_dwFrame == (WORD)dwThisTime )
			{
				CString Str;
				Str.Format ( "[%d] 구분동작 키로 이미 등록되었습니다.", dwThisTime/dwUnitTime );
				MessageBox ( Str );
				return;
			}
		}

		int nIndex = m_comboPiece.GetCurSel();
		EMPIECECHAR emPiece = (EMPIECECHAR) m_comboPiece.GetItemData ( nIndex );

		nIndex = m_comboPiece.GetCurSel();
		EMSTRIKE_EFF emStrikeEff = (EMSTRIKE_EFF) m_comboEff.GetItemData ( nIndex );

		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_dwFrame = (WORD) dwThisTime;
		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_emPiece = emPiece;
		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_emEffect = emStrikeEff;		
		pAnimCont->m_wStrikeCount++;		

		SAttachBoneAnimBasicInfo sTempBasicAni;
		sTempBasicAni.nStartAnim = -3;
		sTempBasicAni.emAttAniType = AN_ATTBONE_ATTACK_001;
		for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
		{
			pAnimCont->m_sAttBoneAnimInfo[i].bUseStrikeAni = true;
			pAnimCont->m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
		}
		

		Sorting ( pAnimCont->m_sStrikeEff, pAnimCont->m_sAttBoneAnimInfo, pAnimCont->m_wStrikeCount );

	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone ();
		DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

		DxSkinAnimation* pAnimCont = pAttBoneData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if ( !pAnimCont )	
			return;

		const DxSkinAnimation* pSelectedAniCont = pAttBone->m_spSkinAniControlThread->GETCURANIM();
		if ( !pSelectedAniCont )	return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;

		if ( pAnimCont->m_wStrikeCount >= ACF_STRIKE )
		{
			CString Str;
			Str.Format ( "구분동작 키는 %d 개 까지만 가능합니다.", ACF_STRIKE );
			MessageBox ( Str );
			return;
		}

		DWORD dwThisTime = (DWORD) pAttBone->m_spSkinAniControlThread->GETCURTIME ();
		for ( WORD i=0; i<pAnimCont->m_wStrikeCount; ++i )
		{
			if ( pAnimCont->m_sStrikeEff[i].m_dwFrame == (WORD)dwThisTime )
			{
				CString Str;
				Str.Format ( "[%d] 구분동작 키로 이미 등록되었습니다.", dwThisTime/dwUnitTime );
				MessageBox ( Str );
				return;
			}
		}

		int nIndex = m_comboPiece.GetCurSel();
		EMPIECECHAR emPiece = (EMPIECECHAR) m_comboPiece.GetItemData ( nIndex );

		nIndex = m_comboPiece.GetCurSel();
		EMSTRIKE_EFF emStrikeEff = (EMSTRIKE_EFF) m_comboEff.GetItemData ( nIndex );

		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_dwFrame = (WORD) dwThisTime;
		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_emPiece = emPiece;
		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_emEffect = emStrikeEff;
		pAnimCont->m_wStrikeCount++;

		SAttachBoneAnimBasicInfo sTempBasicAni;
		sTempBasicAni.nStartAnim = -3;
		sTempBasicAni.emAttAniType = AN_ATTBONE_ATTACK_001;
		for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
		{
			pAnimCont->m_sAttBoneAnimInfo[i].bUseStrikeAni = true;
			pAnimCont->m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
		}

		Sorting ( pAnimCont->m_sStrikeEff, pAnimCont->m_sAttBoneAnimInfo, pAnimCont->m_wStrikeCount );
		pAttBone->SetAttBoneData ( pAttBoneData, pView->GetD3dDevice() );
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle ();
		DxVehicleData *pVehicleData = pView->GetVehicleData();

		DxSkinAnimation* pAnimCont = pVehicleData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if ( !pAnimCont )	
			return;

		const DxSkinAnimation* pSelectedAniCont = pVehicle->m_spSkinAniControlThread->GETCURANIM();
		if ( !pSelectedAniCont )	return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;

		if ( pAnimCont->m_wStrikeCount >= ACF_STRIKE )
		{
			CString Str;
			Str.Format ( "구분동작 키는 %d 개 까지만 가능합니다.", ACF_STRIKE );
			MessageBox ( Str );
			return;
		}

		DWORD dwThisTime = (DWORD) pVehicle->m_spSkinAniControlThread->GETCURTIME ();
		for ( WORD i=0; i<pAnimCont->m_wStrikeCount; ++i )
		{
			if ( pAnimCont->m_sStrikeEff[i].m_dwFrame == (WORD)dwThisTime )
			{
				CString Str;
				Str.Format ( "[%d] 구분동작 키로 이미 등록되었습니다.", dwThisTime/dwUnitTime );
				MessageBox ( Str );
				return;
			}
		}

		int nIndex = m_comboPiece.GetCurSel();
		EMPIECECHAR emPiece = (EMPIECECHAR) m_comboPiece.GetItemData ( nIndex );

		nIndex = m_comboPiece.GetCurSel();
		EMSTRIKE_EFF emStrikeEff = (EMSTRIKE_EFF) m_comboEff.GetItemData ( nIndex );

		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_dwFrame = (WORD) dwThisTime;
		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_emPiece = emPiece;
		pAnimCont->m_sStrikeEff[pAnimCont->m_wStrikeCount].m_emEffect = emStrikeEff;
		pAnimCont->m_wStrikeCount++;

		SAttachBoneAnimBasicInfo sTempBasicAni;
		sTempBasicAni.nStartAnim = -3;
		sTempBasicAni.emAttAniType = AN_ATTBONE_ATTACK_001;
		for( DWORD i = 0; i < PIECE_SAVE_SIZE; i++ )
		{
			pAnimCont->m_sAttBoneAnimInfo[i].bUseStrikeAni = true;
			pAnimCont->m_sAttBoneAnimInfo[i].vecStrikeAni.push_back(sTempBasicAni);
		}

		Sorting ( pAnimCont->m_sStrikeEff, pAnimCont->m_sAttBoneAnimInfo, pAnimCont->m_wStrikeCount );
		pVehicle->SetVehiclePartData ( pVehicleData, pView->GetD3dDevice() );
	}


	ListStrike ();
	FrameMarking ();
}

void CStrikePage::OnBnClickedButtonDel()
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();	

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar ();
		DxSkinCharData *pSkinCharData = pView->GetSkinCharData();

		DxSkinAnimation* pAnimCont = pSkinCharData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if ( !pAnimCont )	
			return;

		int nIndex = m_listStrike.GetCurSel ();
		if ( nIndex==LB_ERR )	return;
		int nArrayIndex = (int) m_listStrike.GetItemData ( nIndex );	// 이것은 문제가 있다.
		nArrayIndex = nIndex;											// 수정 됨.
		
		pAnimCont->m_sStrikeEff[nArrayIndex].m_dwFrame = 0xFFFF;

		for ( WORD i=nArrayIndex+1; i<pAnimCont->m_wStrikeCount; ++i )
		{
			pAnimCont->m_sStrikeEff[i-1] = pAnimCont->m_sStrikeEff[i];
		}

		pAnimCont->m_wStrikeCount--;
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone ();
		DxAttBoneData *pAttBoneData = pView->GetAttBoneData();

		DxSkinAnimation* pAnimCont = pAttBoneData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if ( !pAnimCont )	
			return;

		int nIndex = m_listStrike.GetCurSel ();
		if ( nIndex==LB_ERR )	return;
		int nArrayIndex = (int) m_listStrike.GetItemData ( nIndex );	// 이것은 문제가 있다.
		nArrayIndex = nIndex;											// 수정 됨.

		pAnimCont->m_sStrikeEff[nArrayIndex].m_dwFrame = 0xFFFF;

		for ( WORD i=nArrayIndex+1; i<pAnimCont->m_wStrikeCount; ++i )
		{
			pAnimCont->m_sStrikeEff[i-1] = pAnimCont->m_sStrikeEff[i];
		}

		pAnimCont->m_wStrikeCount--;
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle ();
		DxVehicleData *pVehicleData = pView->GetVehicleData();

		DxSkinAnimation* pAnimCont = pVehicleData->GetSkinAniControlData()->FindAnimation( m_sAnimName.GetString() );
		if ( !pAnimCont )	
			return;

		int nIndex = m_listStrike.GetCurSel ();
		if ( nIndex==LB_ERR )	return;
		int nArrayIndex = (int) m_listStrike.GetItemData ( nIndex );	// 이것은 문제가 있다.
		nArrayIndex = nIndex;											// 수정 됨.

		pAnimCont->m_sStrikeEff[nArrayIndex].m_dwFrame = 0xFFFF;

		for ( WORD i=nArrayIndex+1; i<pAnimCont->m_wStrikeCount; ++i )
		{
			pAnimCont->m_sStrikeEff[i-1] = pAnimCont->m_sStrikeEff[i];
		}

		pAnimCont->m_wStrikeCount--;
	}

	// 모션이 변경되어서 작업하지 않는다. 필요하다면 해줘야한다.
	//m_pSheetTab->ApplyAniContrl();

	ListStrike ();
	FrameMarking ();
}

void CStrikePage::OnBnClickedOk()
{
	FrameMarking ( TRUE );

	m_sAnimName = "";
	m_pSheetTab->SetActivePage ( ANIMSETPAGEINDEX );
}
