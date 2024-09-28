// AnimPage.cpp : implementation file
//

#include "stdafx.h"
#include "../charedit.h"

#include "../../MfcExLib/ExLibs/FECFileDialog.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"

#include "../../EngineLib/DxCommon/d3dapp.h"
#include "../../EngineLib/DxCommon/dxutil.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../EngineLib/DxTools/NSCharAniSequence.h"
#include "../EngineLib/DxTools/DxSurfaceTex.h"
#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DeviceMan.h"

#include "../../RanLogic/FlyCamera/GLFlyCameraKeyFrameControl.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Dialog/ResolutionDlg.h"

#include "SheetWithTabAnim.h"
#include "./AnimPage.h"
#include "./MultianimPage.h"

#include "../State/CharStateMachine.h"
#include "../Properties/sheetWithTabChar.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern ANIMATE  g_Animate;
extern bool	    g_bMoveTest;
extern MOVETYPE	g_MoveType;

/////////////////////////////////////////////////////////////////////////////
// CAnimPage property page

IMPLEMENT_DYNCREATE(CAnimPage, CPropertyPage)

CAnimPage::CAnimPage() :
	CPropertyPage(CAnimPage::IDD),
	m_pSheetTab(NULL)
{
	//{{AFX_DATA_INIT(CAnimPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_ListAnimation.SetPopupID(IDR_POPUP_ANIMATION, WM_ANIMATECONT);
}

CAnimPage::~CAnimPage()
{
}

void CAnimPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimPage)
	DDX_Control(pDX, IDC_LIST_ANIMATION, m_ListAnimation);
	DDX_Control(pDX, IDC_COMBO_ANIM_MTYPE, m_ComboMType);
	DDX_Control(pDX, IDC_COMBO_ANIM_STYPE, m_ComboSType);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnimPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAnimPage)
	ON_BN_CLICKED(IDC_BUTTON_ANIM_NEW, OnButtonAnimNew)
	ON_BN_CLICKED(IDC_BUTTON_ANIM_DELETE, OnButtonAnimDelete)
	ON_BN_CLICKED(IDC_BUTTON_ANIM_SELECT, OnButtonAnimSelect)
	ON_BN_CLICKED(IDC_BUTTON_ANIM_SETTING, OnButtonAnimSetting)
	ON_MESSAGE(WM_ANIMATECONT, OnAnimationCont)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ANIM_MIX, OnBnClickedButtonAnimMix)
	ON_BN_CLICKED(IDC_BUTTON_ANIM_CONVERSION, OnBnClickedButtonAnimConversion)
	ON_BN_CLICKED(IDC_MOVE_TESTCHECK, OnBnClickedMoveTestcheck)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnBnClickedApplyButton)
	ON_LBN_DBLCLK(IDC_LIST_ANIMATION, OnLbnDblclkListAnimation)
	ON_BN_CLICKED(IDC_BUTTON_RESET_POS, OnBnClickedButtonResetPos)
	ON_BN_CLICKED(IDC_BUTTON_REMAKE_ALL_BIN, &CAnimPage::OnBnClickedButtonRemakeAllBin)
	ON_BN_CLICKED(IDC_BUTTON_REMAKE_SEL_BIN, &CAnimPage::OnBnClickedButtonRemakeSelBin)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_ANI_SEQUENCE, &CAnimPage::OnBnClickedButtonCreateAniSequence)
	ON_BN_CLICKED(IDC_BUTTON_ANIMLIST_EXPORT, &CAnimPage::OnBnClickedButtonAnimListExport)
	ON_BN_CLICKED(IDC_BTN_TRANS_APPLY, OnBnClickedTransApply)
	ON_CBN_SELCHANGE(IDC_COMBO_ANIM_MTYPE, &CAnimPage::OnCbnSelchangeComboMType)
	ON_CBN_SELCHANGE(IDC_COMBO_ANIM_STYPE, &CAnimPage::OnCbnSelchangeComboSType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimPage message handlers

BOOL CAnimPage::OnInitDialog()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	D3DXVECTOR3 vPos(0.0f, 0.0f, 0.0f);
	float fRot(0.0f);
	if ( pFrame )
	{
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		pView->GetSkinTranslationInfo(vPos, fRot);
	}
	SetWin_Num_float(this, IDC_EDIT_X, vPos.x);
	SetWin_Num_float(this, IDC_EDIT_Y, vPos.y);
	SetWin_Num_float(this, IDC_EDIT_Z, vPos.z);
	SetWin_Num_float(this, IDC_EDIT_ROTATE, fRot);

	return __super::OnInitDialog();
}

void CAnimPage::AnimationListing ()
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	m_ListAnimation.ResetContent();

	if( !m_pSheetTab )						return;
	if( !m_pSheetTab->GetSkinAniContrl() )	return;

	const DxSkinAniControl::VECNODE& vecANI = m_pSheetTab->GetSkinAniContrl()->GetAniList ();
	for ( size_t n=0; n<vecANI.size(); ++n )
	{
		m_ListAnimation.AddString(vecANI[n]->m_pDxSkinAni->m_strCfgFile.c_str());
	}
	
	SetDlgItemText( IDC_RUNSPEED_EDIT, "30" );
	SetDlgItemText( IDC_WALKSPEED_EDIT, "10" );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->SetMoveSpeed( 30.0f, 10.0f );

	m_ComboMType.ResetContent();
	int nIndex;
	for ( DWORD i=0; i<AN_TYPE_SIZE; ++i )
	{
		nIndex = m_ComboMType.AddString(COMMENT::GetAnimMainType(i));
		m_ComboMType.SetItemData ( nIndex, i );
	}
}

BOOL CAnimPage::LoadAnimation( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	if ( !m_pSheetTab->GetSkinAniContrl()->LoadAnimationForEdit( pszFileName, pView->GetD3dDevice () ) )
	{
		MessageBox ( "에니메이션 읽기에 실패하였습니다." );
		return FALSE;
	}

	m_pSheetTab->ApplyAniContrl ();
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
	{
		//pView->GetSkinChar()->SELECTANI ( pszFileName );
		//pView->GetSkinCharData()->SELECTANI ( pszFileName );
		pView->GetSkinChar()->m_spSkinAniControlThread->SELECTANI_EDIT( pszFileName );
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		//pView->GetAttBone()->SELECTANI ( pszFileName );
		//pView->GetAttBoneData()->SELECTANI ( pszFileName );
		pView->GetAttBone()->m_spSkinAniControlThread->SELECTANI_EDIT( pszFileName );
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		//pView->GetVehicle()->SELECTANI ( pszFileName );
		//pView->GetVehicleData()->SELECTANI ( pszFileName );
		pView->GetVehicle()->m_spSkinAniControlThread->SELECTANI_EDIT( pszFileName );
	}

	////	Note : 선택된 에니메이션으로 타임라인 초기화.
	////
	const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
 	if( !pSelectedAniCont )
	{
 		return FALSE;
	}
	DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
	if ( dwUnitTime > 0 )
	{
		pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
			pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
	}

	AnimationListing ();
	return TRUE;
}

//void CAnimPage::OnButtonAnimNew() 
//{
//	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
//	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");
//
//	
//
//
//	//	Note : 파일 필터를 정의한다.
//	CString szFilter = "Skin Animation xFile (*.x,*.mxf)|*.X;*.mxf|";
//	
//	//	Note : 파일 오픈 다이알로그를 만듬.
//	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
//		(CAnimPage*)this);
//
//	dlg.m_ofn.lpstrInitialDir = DxSkinAnimationManager::GetInstance().GetPath().c_str();
//	if ( dlg.DoModal() == IDOK )
//	{
//		CString sFileName = dlg.GetFileName();
//		sFileName.MakeLower ();
//
//		LoadAnimation( sFileName.GetString() );
//		
//	}
//}

void CAnimPage::OnButtonAnimNew() 
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	CsheetWithTabChar* pSheetChar = pFrame->m_cDlgEdit.GetSheetChar();


	//	Note : 파일 필터를 정의한다.
	CString szFilter = _T("Skin Animation xFile |*.X|");

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFECFileDialog dlg ( TRUE, _T(".x"), NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = DxSkinAnimationManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		POSITION pos = dlg.GetStartPosition();
		while (pos != NULL)
		{
			CString sFileName = dlg.GetNextPathName(pos);
			sFileName.MakeLower ();

			int nPosition = sFileName.ReverseFind('\\');
			sFileName = sFileName.Mid(nPosition+1);

			

			CString strCfgFile;
			//  [10/15/2013 gbgim];
			// - AnimPage를 사용하는 곳은 CharPage, AttachBonePage, VehiclePage이다;
			//   현재 활성화된 페이지를 기준으로해서 다이얼로그의 값을 읽어옴;
			//   CharPage, AttachBonePage에는 체크 박스 다이얼로그가 있으나;
			//   VehiclePage에는 없다;
			CButton* pButton = (CButton*)pFrame->m_cDlgEdit.GetSheetChar()->GetActivePage()->GetDlgItem( IDC_CHECK_NAME_CHF_CFG );
			if ( pButton != NULL && pButton->GetCheck() )
			{
				CString strChfFile;
				STRUTIL::ChangeExt( pView->GetSkinChar()->GetFileName(), _T(".chf"), strChfFile, _T("") );

				// Note : 파일 이름 변환
				STRUTIL::ChangeExt( sFileName.GetString(), _T(".x"), strCfgFile, _T("") );
				strCfgFile += _T('_');
				strCfgFile += strChfFile.GetString();
				strCfgFile += _T(".cfg");
			}
			else
			{
				// 그래픽팀에서 _xxx.cfg(chf파일명) 가 붙는게 싫다고 하여 변경됨.
				// .cfg 만 붙도록 변경됨.
				CString strTemp = sFileName;
				STRUTIL::ChangeExt( strTemp.GetString(), _T(".x"), strCfgFile, _T(".cfg") );
			}

			if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
			{
				if ( !pView->GetSkinCharData()->LoadAnimationFromTool( pView->GetSkinChar()->GetFileName(), 
					strCfgFile.GetString(), 
					sFileName,
					pView->GetD3dDevice(),
					true ) )
				{
					MessageBox ( _T("에니메이션 읽기에 실패하였습니다.") );
					return;
				}

				m_pSheetTab->ApplyAniContrl ();
				pView->GetSkinChar()->SELECTANI_EDIT( strCfgFile.GetString() );
			}
			else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
			{
				if ( !pView->GetAttBoneData()->LoadAnimationFromTool( pView->GetAttBone()->GetFileName(), 
					strCfgFile.GetString(), 
					sFileName,
					pView->GetD3dDevice(),
					true ) )
				{
					MessageBox ( _T("에니메이션 읽기에 실패하였습니다.") );
					return;
				}

				m_pSheetTab->ApplyAniContrl ();
				pView->GetAttBone()->m_spSkinAniControlThread->SELECTANI_EDIT( strCfgFile.GetString() );
			}
			else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
			{
				if ( !pView->GetVehicleData()->LoadAnimationFromTool( pView->GetVehicle()->GetFileName(), 
					strCfgFile.GetString(), 
					sFileName,
					pView->GetD3dDevice(),
					true ) )
				{
					MessageBox ( _T("에니메이션 읽기에 실패하였습니다.") );
					return;
				}

				m_pSheetTab->ApplyAniContrl ();
				pView->GetVehicle()->m_spSkinAniControlThread->SELECTANI_EDIT( strCfgFile.GetString() );
			}

			////	Note : 선택된 에니메이션으로 타임라인 초기화.
			////
			DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM_none_const();
			if ( pSelectedAniCont )
			{
				DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
				if ( dwUnitTime > 0 )
				{
					// 기본값을 넣어준다.
					pSelectedAniCont->m_dwETime = pSelectedAniCont->m_dwETimeOrig;

					pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
						pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
				}
			}
		}

		CharStateMachine::ClearFileNotifier();
		CharStateMachine::InsertFileNotifier();

		AnimationListing ();




		//CString sFileName = dlg.GetFileName();
		//sFileName.MakeLower ();

		//CString strCfgFile;
		//if ( CCharPage::g_bMakeNameChfCfg )
		//{
		//	CString strChfFile;
		//	STRUTIL::ChangeExt( pView->GetSkinChar()->GetFileName(), _T(".chf"), strChfFile, _T("") );

		//	// Note : 파일 이름 변환
		//	STRUTIL::ChangeExt( sFileName.GetString(), _T(".x"), strCfgFile, _T("") );
		//	strCfgFile += _T('_');
		//	strCfgFile += strChfFile.GetString();
		//	strCfgFile += _T(".cfg");
		//}
		//else
		//{
		//	// 그래픽팀에서 _xxx.cfg(chf파일명) 가 붙는게 싫다고 하여 변경됨.
		//	// .cfg 만 붙도록 변경됨.
		//	STRUTIL::ChangeExt( sFileName.GetString(), _T(".x"), strCfgFile, _T(".cfg") );
		//}

		//if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
		//{
		//	if ( !pView->GetSkinCharData()->LoadAnimationFromTool( pView->GetSkinChar()->GetFileName(), 
		//		strCfgFile.GetString(), 
		//		sFileName,
		//		pView->GetD3dDevice(),
		//		true ) )
		//	{
		//		MessageBox ( _T("에니메이션 읽기에 실패하였습니다.") );
		//		return;
		//	}

		//	//pView->GetSkinChar()->SetCharData( &pView->m_SkinCharData, pView->GetD3dDevice(), FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

		//	m_pSheetTab->ApplyAniContrl ();
		//	pView->GetSkinChar()->SELECTANI_EDIT( strCfgFile.GetString() );
		//}
		//else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
		//{
		//	if ( !pView->GetAttBoneData()->LoadAnimationFromTool( pView->GetAttBone()->GetFileName(), 
		//		strCfgFile.GetString(), 
		//		sFileName,
		//		pView->GetD3dDevice(),
		//		true ) )
		//	{
		//		MessageBox ( _T("에니메이션 읽기에 실패하였습니다.") );
		//		return;
		//	}

		//	//pView->GetAttBone()->SetCharData( &pView->m_AttBoneData, pView->GetD3dDevice(), FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

		//	m_pSheetTab->ApplyAniContrl ();
		//	pView->GetAttBone()->SELECTANI_EDIT( strCfgFile.GetString() );
		//}
		//else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
		//{
		//	if ( !pView->GetVehicleData()->LoadAnimationFromTool( pView->GetVehicle()->GetFileName(), 
		//		strCfgFile.GetString(), 
		//		sFileName,
		//		pView->GetD3dDevice(),
		//		true ) )
		//	{
		//		MessageBox ( _T("에니메이션 읽기에 실패하였습니다.") );
		//		return;
		//	}

		//	//pView->GetVehicle()->SetCharData( &pView->m_VehicleData, pView->GetD3dDevice(), FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

		//	m_pSheetTab->ApplyAniContrl ();
		//	pView->GetVehicle()->SELECTANI_EDIT( strCfgFile.GetString() );
		//}

		//////	Note : 선택된 에니메이션으로 타임라인 초기화.
		//////
		//DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
		//DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
		//if ( dwUnitTime > 0 )
		//{
		//	// 기본값을 넣어준다.
		//	pSelectedAniCont->m_dwETime = pSelectedAniCont->m_dwETimeOrig;

		//	pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
		//		pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
		//}

		//CharStateMachine::ClearFileNotifier();
		//CharStateMachine::InsertFileNotifier();

		//AnimationListing ();
	}
}

//void CAnimPage::OnButtonAnimDelete() 
//{
//	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
//	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");
//
//	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );
//	DWORD nIndex = pListBox->GetCurSel();
//	if ( nIndex == LB_ERR )		return;
//
//	CString sItemText;
//	pListBox->GetText ( nIndex, sItemText );
//
//	m_pSheetTab->GetSkinAniContrl()->ReleaseAnimation ( sItemText.GetString() );
//
//	m_pSheetTab->ApplyAniContrl ();
//
//	AnimationListing ();
//}

void CAnimPage::OnButtonAnimDelete() 
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );

	for( int nIndex=0; nIndex<pListBox->GetCount(); ++nIndex )
	{
		int nCheck = pListBox->GetSel( nIndex );

		if( nCheck > 0 )
		{
			CString sItemText;
			pListBox->GetText ( nIndex, sItemText );

			//			DxSkinAniMan::GetInstance( pd3dDevice )->DeleteAnim( sItemText.GetString() );
			m_pSheetTab->GetSkinAniContrl()->ReleaseAnimationForEdit( sItemText.GetString() );

			if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
			{
				pView->GetSkinCharData()->ReleaseAnimationFromTool( pd3dDevice, sItemText.GetString() );
			}
			else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
			{
				pView->GetAttBoneData()->ReleaseAnimationFromTool( pd3dDevice, sItemText.GetString() );
			}
			else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
			{
				pView->GetVehicleData()->ReleaseAnimationFromTool( pd3dDevice, sItemText.GetString() );
			}
		}
	}

    CharStateMachine::ClearFileNotifier();
    CharStateMachine::InsertFileNotifier();

	m_pSheetTab->ApplyAniContrl ();

	AnimationListing ();
}

void CAnimPage::OnButtonAnimSelect()
{
	assert(m_pSheetTab&&"SheetTab의 포인터가 지정되지 않음.");
	assert(m_pSheetTab->GetSkinAniContrl()&&"Skin Animation Control의 포인터가 지정되지 않음.");

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );
	DWORD nIndex = pListBox->GetCurSel();
	if ( nIndex == -1 ) return;

	//애니메이션을 선택했을때 이동테스트 버튼 사용안함
	CButton *pButton = (CButton *)GetDlgItem(IDC_MOVE_TESTCHECK);
	pButton->SetCheck( FALSE );
	OnBnClickedMoveTestcheck();
	
	CString sItemText;
	pListBox->GetText ( nIndex, sItemText );

	//	Note : 트렌스폼 초기화.
	//
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
	{
		pView->GetSkinChar()->SELECTANI_EDIT ( sItemText.GetString() );

		// SetCharData 내부에서 새로만들기 때문에 재 셋팅이 필요함.
		m_pSheetTab->m_pSkinAniContrl = pView->GetSkinChar()->m_spSkinAniControlThread.get();
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		pView->GetAttBone()->m_spSkinAniControlThread->SELECTANI_EDIT ( sItemText.GetString() );
	
		// SetCharData 내부에서 새로만들기 때문에 재 셋팅이 필요함.
		m_pSheetTab->m_pSkinAniContrl = pView->GetAttBone()->m_spSkinAniControlThread.get();
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		pView->GetVehicle()->m_spSkinAniControlThread->SELECTANI_EDIT ( sItemText.GetString() );
		
		// SetCharData 내부에서 새로만들기 때문에 재 셋팅이 필요함.
		m_pSheetTab->m_pSkinAniContrl = pView->GetVehicle()->m_spSkinAniControlThread.get();
	}

	pView->SetAttTIMER(0.0f);
	pView->SetAttSTEP(0);

	//	Note : 선택된 에니메이션으로 타임라인 초기화.
	//
	const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM ();
	if ( pSelectedAniCont )
	{
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
		if ( dwUnitTime > 0 )
		{
			pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
				pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
		}
	}

	pFrame->m_cDlgTimeLine.SetPlay();	
}


void CAnimPage::OnBnClickedMoveTestcheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton = (CButton *)GetDlgItem(IDC_MOVE_TESTCHECK);
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	if( pButton->GetCheck() == BST_CHECKED )
	{
		g_bMoveTest = TRUE;

		g_MoveType = MOVE_STAY;


		if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
		{
			//pView->GetSkinChar()->SELECTANI( AN_GUARD_N );
			//pView->GetSkinCharData()->SELECTANI( AN_GUARD_N );
			pView->GetSkinChar()->SELECTANI( AN_GUARD_N );
		}
		else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
		{
			pView->GetAttBone()->SELECTANI( AN_ATTBONE_STAY );
			//pView->GetAttBoneData()->SELECTANI( AN_ATTBONE_STAY );
		}
		else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
		{
			pView->GetVehicle()->SELECTANI( AN_VEHICLE_STAY );
			//pView->GetVehicleData()->SELECTANI( AN_VEHICLE_STAY );
		}


		//	Note : 선택된 에니메이션으로 타임라인 초기화.
		//
		const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM ();
		if( pSelectedAniCont != NULL )
		{
			DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
			if ( dwUnitTime > 0 )
			{
				pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
					pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime, AM_PLAY );
			}
		}

	}else{
	
		g_bMoveTest = FALSE;
		
	}
}

void CAnimPage::OnButtonAnimSetting() 
{
	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );
	DWORD nIndex = pListBox->GetCurSel();
	if ( nIndex == -1 ) return;
	
	CString sItemText;
	pListBox->GetText ( nIndex, sItemText );
	
	OnButtonAnimSelect();

	m_pSheetTab->AnimSetActive ( sItemText );
}

void CAnimPage::OnButtonAddMultiAnim()
{
	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );
	for( int nIndex=0; nIndex<pListBox->GetCount(); ++nIndex )
	{
		int nCheck = pListBox->GetSel( nIndex );
		if( nCheck > 0 )
		{
			CString sItemText;
			pListBox->GetText ( nIndex, sItemText );
			m_pSheetTab->m_AnimTabPage.GetMultiAnimPage()->AddAnim(sItemText);
		}
	}
}

LRESULT CAnimPage::OnAnimationCont(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case ID_POPUP_ADDANI:
		OnButtonAnimNew();
		break;

	case ID_POPUP_DELANI:
		OnButtonAnimDelete();
		break;

	case ID_POPUP_SELECTANI:
		OnButtonAnimSelect();
		break;
		
	case ID_POPUP_SETANI:
		OnButtonAnimSetting();
		break;

	case ID_POPUP_ADDMULTIANI:
		OnButtonAddMultiAnim();
		break;
	};

	return 0L;
}

void CAnimPage::OnBnClickedButtonAnimMix()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
	{
		DxSkinChar* pSkinChar = pView->GetSkinChar();
		pSkinChar->m_spSkinAniControlThread->DOSHOCKMIX ();
	}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBone* pAttBone = pView->GetAttBone();
		pAttBone->m_spSkinAniControlThread->DOSHOCKMIX ();
	}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicle* pVehicle = pView->GetVehicle();
		pVehicle->m_spSkinAniControlThread->DOSHOCKMIX ();
	}

}

void CAnimPage::OnBnClickedButtonAnimConversion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	assert(m_pSheetTab&& _T("SheetTab의 포인터가 지정되지 않음.") );
	assert(m_pSheetTab->GetSkinAniContrl()&& _T("Skin Animation Control의 포인터가 지정되지 않음.") );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );
	DWORD nIndex = pListBox->GetCurSel();
	if ( nIndex == -1 ) return;
	
	CString sItemText;
	pListBox->GetText ( nIndex, sItemText );

	//	Note : 트렌스폼 초기화.
	//
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
	{
		//pView->GetSkinChar()->SELECTANI ( sItemText.GetString() );
		//pView->GetSkinCharData()->SELECTANI ( sItemText.GetString() );
		pView->GetSkinChar()->m_spSkinAniControlThread->SELECTANI_EDIT( sItemText.GetString() );
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		//pView->GetAttBone()->SELECTANI ( sItemText.GetString() );
		//pView->GetAttBoneData()->SELECTANI ( sItemText.GetString() );
		pView->GetAttBone()->m_spSkinAniControlThread->SELECTANI_EDIT( sItemText.GetString() );
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		//pView->GetVehicle()->SELECTANI ( sItemText.GetString() );
		//pView->GetVehicleData()->SELECTANI ( sItemText.GetString() );
		pView->GetVehicle()->m_spSkinAniControlThread->SELECTANI_EDIT( sItemText.GetString() );
	}


	//	Note : 선택된 에니메이션으로 타임라인 초기화.
	//
	const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM ();
	if( pSelectedAniCont )
	{
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
		if ( dwUnitTime > 0 )
		{
			pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
				pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
		}
	}

	// Note : 애니메이션은 계속 돌아가도록 한다.
	g_Animate = AM_PLAY;
}




void CAnimPage::OnBnClickedApplyButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	char szTempChar[16] = {0,};
	float fRunSpeed, fWalkSpeed;
	GetDlgItemText( IDC_RUNSPEED_EDIT, szTempChar, 16 ); 
	fRunSpeed = (float)atof(szTempChar );
	GetDlgItemText( IDC_WALKSPEED_EDIT, szTempChar, 16 );
	fWalkSpeed = (float)atof(szTempChar );

	pFrame->SetMoveSpeed( fRunSpeed, fWalkSpeed );
}


void CAnimPage::OnLbnDblclkListAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnButtonAnimSelect();
}

void CAnimPage::OnBnClickedButtonResetPos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->ResetSkinPos();
}

void CAnimPage::OnBnClickedButtonRemakeAllBin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->GetSkinCharData()->SaveBinFileAll();
}

void CAnimPage::OnBnClickedButtonRemakeSelBin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	assert(m_pSheetTab&& _T("SheetTab의 포인터가 지정되지 않음.") );
	assert(m_pSheetTab->GetSkinAniContrl()&& _T("Skin Animation Control의 포인터가 지정되지 않음.") );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );
	DWORD nIndex = pListBox->GetCurSel();
	if ( nIndex == -1 ) return;

	CString sItemText;
	pListBox->GetText ( nIndex, sItemText );

	//	Note : 트렌스폼 초기화.
	//
	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR || CCharEditView::m_emObjType == EOBJ_FLYCAMERA )
	{
		pView->GetSkinCharData()->SaveBinFile( sItemText.GetString() );
	}
}

void CAnimPage::OnBnClickedButtonCreateAniSequence()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );

	// 2개 이상일 경우 작동시키지 않는다.
	if( pListBox->GetSelCount() >= 2 )
	{
		MessageBox( _T("2개이상 선택되어서 작동이 되지 않습니다.") );
		return;
	}

	DWORD nIndex = pListBox->GetCurSel();
	if ( nIndex == -1 ) return;

	CString sItemText;
	pListBox->GetText ( nIndex, sItemText );

	CResolutionDlg sDlg;
	sDlg.m_dwResolutionINDEX	= 0;
	sDlg.m_dwAA_INDEX			= 4;
	sDlg.m_dwFPS				= 30;
	sDlg.m_fAddTime				= 0.5f;

	sDlg.m_fColorPower			= 1.3f;

	if( sDlg.DoModal() == IDOK )
	{
		CsheetWithTabChar* pSheetWithTab = pFrame->m_cDlgEdit.GetSheetChar();
		GLFlyCameraKeyFrameControl* pFlyCam = &pSheetWithTab->m_FlyCameraPage.GetKeyFrameCtrl();

		std::vector<std::string> vecAnim;
		vecAnim.push_back(sItemText.GetString());
		const DWORD dwAnimSeqFlag = sDlg.m_bOnlyCurrentFrame ? NSCharAniSequence::EMANISEQ_ONLYCURRENTFRAME : 0;
		NSCharAniSequence::Create
		( 
			pView->GetD3dDevice(),
			pView->GetEngineDevice(),
			pView->GetSkinCharData(),
			pView->GetSkinCharData()->GetFileName().c_str(), 
			vecAnim, 
			pView->GetLandMan(),
			sDlg.m_dwResolutionINDEX,
			sDlg.m_dwAA_INDEX,
			sDlg.m_dwFPS,
			sDlg.m_fAddTime,
			sDlg.m_fColorPower,
			pView->GetSkinMatrix(),
			dwAnimSeqFlag,
			pView->GetSkinChar()->GETCURKEYTIME(),
			CCharEditView::m_emObjType == EOBJ_FLYCAMERA ? pFlyCam : NULL
		);
	}
}

void CAnimPage::OnBnClickedButtonAnimListExport()
{
	// 파일 오픈 다이얼로그 초기화;
	std::string strFilter = "AnimList Excel File (*.xls)|*.xls|";
	CFileDialog dlg(FALSE, ".xls", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter.c_str(), this);
	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath();
	if ( dlg.DoModal() != IDOK )
		return ;

	// 엑셀 데이터 출력을 위한 초기화;
	int sheet = 0, row = 0, col = 0;

	CString FullPathName(dlg.GetPathName()), FileName(dlg.GetFileTitle());
	FullPathName.TrimRight("\\"+dlg.GetFileName());

	std::string strSaveName
		, strFile(FileName.GetString())
		, strPath(FullPathName.GetString());

	sc::Excel ExcelFile(strFile, strPath, strSaveName, sheet, false);
	{
		ExcelFile.SetCell(sheet, 0, 0, L"Main Type_enum");
		ExcelFile.SetCell(sheet, 0, 1, L"Sub Type_enum");
		ExcelFile.SetCell(sheet, 0, 2, L"Main Type_string");
		ExcelFile.SetCell(sheet, 0, 3, L"Sub Type_string");
		ExcelFile.SetCell(sheet, 0, 4, L"Cfg File");
	}
	// 애니메이션 데이터 초기화;
	const DxSkinAniControl* const pAnimControl = m_pSheetTab->GetSkinAniContrl();
	if ( pAnimControl == NULL )
	{
		MessageBox( "Did not load to Animation data!" );
		return ;
	}
	const DxSkinAniControl::VECNODE& vecAnimList = pAnimControl->GetAniList();

	const DWORD dwAnimSize = vecAnimList.size();
	for ( DWORD idx = 0; idx < dwAnimSize; ++idx )
	{
		const ANIMCONTNODE* const pAnimCont = vecAnimList[idx];
		const int nMType = pAnimCont->GetMType();
		const int nSType = pAnimCont->GetSType();
		const DxSkinAnimation* const pAnim = pAnimCont->m_pDxSkinAni;
		const std::string& strCfg = pAnim->m_strCfgFile;

		row = idx+1;
		ExcelFile.SetCell(sheet, row, 0, nMType);
		ExcelFile.SetCell(sheet, row, 1, nSType);
		ExcelFile.SetCell(sheet, row, 2, sc::string::ansiToUnicode(COMMENT::GetAnimMainType(nMType).GetString()));
		ExcelFile.SetCell(sheet, row, 3, sc::string::ansiToUnicode(COMMENT::GetAnimSubType(nMType,nSType).GetString()));
		ExcelFile.SetCell(sheet, row, 4, sc::string::ansiToUnicode(strCfg));
	}

	bool bSaveExcel = ExcelFile.Save();
}

void CAnimPage::OnCbnSelchangeComboMType()
{
	const int nMType(m_ComboMType.GetCurSel());
	SubAnimComboInit(m_ComboSType, nMType);
	OnCbnSelchangeComboSType();
}

void CAnimPage::OnCbnSelchangeComboSType()
{
	const EMANI_MAINTYPE emMType((EMANI_MAINTYPE)m_ComboMType.GetCurSel());
	const EMANI_SUBTYPE emSType((EMANI_SUBTYPE)m_ComboSType.GetCurSel());
	if ( emMType == LB_ERR || emSType == LB_ERR )
		return;

	// 기존 모든 항목 선택해제, 지원하는 함수가 없다;
	int nSelItem[255];
	const int nLBSize(m_ListAnimation.GetSelItems(m_ListAnimation.GetSelCount(),nSelItem));
	for ( int nSelIdx = 0; nSelIdx < nLBSize; ++nSelIdx )
	{
		m_ListAnimation.SetSel(nSelItem[nSelIdx], FALSE);
	}


	const DxSkinAniControl* const pAnimControl = m_pSheetTab->GetSkinAniContrl();
	const DWORD dwAnimAmout(pAnimControl->GETANIAMOUNT(emMType, emSType));
	if ( dwAnimAmout == 0 )
		MessageBox(_T("did not find animation"));

	for ( DWORD dwAnimIdx = 0; dwAnimIdx < dwAnimAmout; ++dwAnimIdx )
	{
		const PANIMCONTNODE pAnim = pAnimControl->GETANI_none_const(emMType, emSType, dwAnimIdx);
		if ( pAnim->GetMType() != emMType || pAnim->GetSType() != emSType )
			MessageBox(_T("did not find animation"));

		const std::string strCfgName(pAnim->m_pDxSkinAni->m_strCfgFile);
		const int nFindSel(m_ListAnimation.FindString(-1, strCfgName.c_str()));
		m_ListAnimation.SetSel(nFindSel);
	}
// 	const PANIMCONTNODE pAnim = pAnimControl->GETANI(emMType, emSType);
// 	if ( pAnim->GetMType() != emMType || pAnim->GetSType() != emSType )
// 		MessageBox(_T("did not find animation"));
//
// 	const std::string strCfgName(pAnim->m_pDxSkinAni->m_strCfgFile);
// 	const int nFindSel(m_ListAnimation.FindString(-1, strCfgName.c_str()));
// 	const int nRes = m_ListAnimation.SetCurSel(nFindSel);
}

void CAnimPage::SubAnimComboInit(CComboBox& comboBox, const int nMType)
{
	int nIndex = comboBox.GetCurSel ();
//	if ( nIndex==LB_ERR )	return;

	comboBox.ResetContent();
	switch ( nMType )
	{
	case AN_SKILL:
	case AN_SKILL_B:
	case AN_SKILL_C:
	case AN_SKILL_D:
	case AN_SKILL_E:
	case AN_SKILL_F:
	case AN_SKILL_G:
	case AN_SKILL_H:
	case AN_GESTURE:
	case AN_SPEC:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = comboBox.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_00", i).GetString());
				else
					nIndex = comboBox.AddString ( COMMENT::ANI_SUBTYPE_00[i].c_str() );

				comboBox.SetItemData ( nIndex, i );
			}
		}
		break;
	case AN_GATHERING:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = comboBox.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_01", i).GetString());
				else
					nIndex = comboBox.AddString ( COMMENT::ANI_SUBTYPE_01[i].c_str() );
				comboBox.SetItemData ( nIndex, i );
			}
		}
		break;
	case AN_FLY:
		{
			for ( DWORD i=0; i<AN_SUB_FLY_SIZE; ++i )
			{
				// 아래의 COMMENT는 데이터에서 읽어오지 않는다;
				nIndex = comboBox.AddString ( COMMENT::ANI_SUBTYPE_FLY[i].c_str() );
				comboBox.SetItemData ( nIndex, i );
			}
		}
		break;
	case AN_BIKE_A:
	case AN_BIKE_B:
	case AN_BIKE_C:
	case AN_BIKE_D:
	case AN_CAR_A:
	case AN_CAR_B:
	case AN_CAR_C:
	case AN_CAR_D:
	case AN_REARCAR_A:
	case AN_REARCAR_B:
	case AN_REARCAR_C:
	case AN_REARCAR_D:
		{
			for ( DWORD i=0; i<AN_SUB_00_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = comboBox.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_03", i).GetString());
				else
					nIndex = comboBox.AddString ( COMMENT::ANI_SUBTYPE_03[i].c_str() );
				comboBox.SetItemData ( nIndex, i );
			}
		}
		break;
	default:
		{
			for ( DWORD i=0; i<AN_SUB_SIZE; ++i )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					nIndex = comboBox.AddString(CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE", i).GetString());
				else
					nIndex = comboBox.AddString ( COMMENT::ANI_SUBTYPE[i].c_str() );
				comboBox.SetItemData ( nIndex, i );
			}
		}
		break;
	};
}

void CAnimPage::OnBnClickedTransApply()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	pView->SetSkinTranslationInfo(D3DXVECTOR3(
		GetWin_Num_float( this, IDC_EDIT_X )
		, GetWin_Num_float( this, IDC_EDIT_Y )
		, GetWin_Num_float( this, IDC_EDIT_Z ) )
		, GetWin_Num_float( this, IDC_EDIT_ROTATE ) );
};