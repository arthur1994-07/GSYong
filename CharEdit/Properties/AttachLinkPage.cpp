// AttachLinkPage .cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"

#include "../SigmaCore/Util/Util.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxAttBoneLinkThreadNeedData.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxPieceDefine.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxAttbonelink.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"

#include "../EngineLib/DxTools/DxViewPort.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../State/CharStateMachine.h"

#include "AttachLinkPage.h"
#include ".\attachlinkpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

extern TSTRING	g_strPICK_BONE;


// CAttachLinkPage  대화 상자입니다.

IMPLEMENT_DYNAMIC(CAttachLinkPage , CPropertyPage)
CAttachLinkPage ::CAttachLinkPage ()
	: CPropertyPage(CAttachLinkPage ::IDD)
{
}

CAttachLinkPage ::~CAttachLinkPage ()
{
}

void CAttachLinkPage ::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANIMATION, m_list_Animation);
	DDX_Control(pDX, IDC_COMBO_OBJTYPE, m_comboObjType);
	DDX_Control(pDX, IDC_COMBO_SKINMESHTYPE, m_comboSkinMeshType);
	DDX_Control(pDX, IDC_COMBO_WHEREBACK, m_comboWhereBack);
}


BEGIN_MESSAGE_MAP(CAttachLinkPage , CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_LOADABF, OnBnClickedButtonLoadabf)
	ON_BN_CLICKED(IDC_BUTTON_LOADBONE, OnBnClickedButtonLoadbone)
	ON_BN_CLICKED(IDC_BUTTON_GETBONE, OnBnClickedButtonGetbone)
	ON_BN_CLICKED(IDC_BUTTON_LOADCPS, OnBnClickedButtonLoadcps)
	ON_BN_CLICKED(IDC_CHECK_USECPS, OnBnClickedCheckUsecps)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_LBN_DBLCLK(IDC_LIST_ANIMATION, OnLbnDblclkListAnimation)
	ON_BN_CLICKED(IDC_RADIO_POSITION, OnBnClickedRadioPosition)
	ON_BN_CLICKED(IDC_RADIO_ROTATION, OnBnClickedRadioRotation)
	ON_BN_CLICKED(IDC_BUTTON_RESET1, OnBnClickedButtonReset1)
	ON_BN_CLICKED(IDC_RADIO_SCALE, OnBnClickedRadioScale)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJTYPE, OnCbnSelchangeComboObjtype)
	ON_CBN_SELCHANGE(IDC_COMBO_SKINMESHTYPE, OnCbnSelchangeComboSkinmeshtype)
	ON_CBN_SELCHANGE(IDC_COMBO_WHEREBACK, OnCbnSelchangeComboWhereback)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE1, OnDeltaposSpinValue1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE2, OnDeltaposSpinValue2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VALUE3, OnDeltaposSpinValue3)
	ON_EN_CHANGE(IDC_EDIT_VALUE1, OnEnChangeEditValue1)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_NONATKMODE, OnBnClickedCheckAtkmode)
	ON_BN_CLICKED(IDC_BUTTON_LINK_INVERSE_MATRIX, &CAttachLinkPage::OnBnClickedButtonLinkInverseMatrix)
	ON_BN_CLICKED(IDC_BUTTON_LINK_MATRIX, &CAttachLinkPage::OnBnClickedButtonLinkMatrix)
END_MESSAGE_MAP()


// CAttachLinkPage  메시지 처리기입니다.
BOOL CAttachLinkPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	for ( int i=0; i<EMATTBONEPARRENTOBJTYPE_SIZE; ++i )
	{
		int nIndex = m_comboObjType.AddString ( SKINTYPE_STING::m_szATTPARENTOBJTYPE[i] );
		m_comboObjType.SetItemData ( nIndex, i );
	}

	m_comboObjType.SetCurSel(0);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	UpdateObjTypeComboBox();

	int nIndex = 0;
	for ( int i=0; i<EMPEACE_WEAPON_SIZE; ++i )
	{
		nIndex = m_comboWhereBack.AddString ( SKINTYPE_STING::m_szWHEREBACKTYPE[i] );
		m_comboWhereBack.SetItemData ( nIndex, i );
	}

	SetTimer( 0, 1, NULL );

	SetWin_Num_float( this, IDC_EDIT_TERM, 1.0f );

	OnBnClickedButtonNew();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAttachLinkPage::OnBnClickedButtonNew()
{	
	if( m_hWnd == NULL ) return;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
	pAttBoneLink->ClearAll();

	pView->SetAttBoneLinkSkeleton( NULL );

	pView->GetSkinPiece()->ClearAll ();

	pView->DeActiveEditMatrix();


	m_list_Animation.ResetContent();

	SetWin_Text( this, IDC_EDIT_ABLFILE, "untitle00.abl" );
	SetWin_Text( this, IDC_EDIT_ABFFILE, "untitle00.abf" );
	SetWin_Text( this, IDC_EDIT_BASEBONE, "BaseBoneName.x" );
	SetWin_Text( this, IDC_EDIT_LINKBONENAME, "LinkBoneName" );
	SetWin_Text( this, IDC_EDIT_CPSFILE, "untitle00.cps" );
	SetWin_Check( this, IDC_CHECK_USECPS, FALSE );
	SetWin_Enable( this, IDC_BUTTON_LOADCPS, FALSE );


	CheckRadioButton( IDC_RADIO_POSITION, IDC_RADIO_SCALE, IDC_RADIO_POSITION );

	SetWin_Enable( this, IDC_LIST_ANIMATION, FALSE );
	SetWin_Enable( this, IDC_BUTTON_LOADBONE, FALSE );
	SetWin_Enable( this, IDC_EDIT_BASEBONE, FALSE );
	SetWin_Enable( this, IDC_BUTTON_GETBONE, FALSE );
	SetWin_Enable( this, IDC_EDIT_LINKBONENAME, FALSE );
	SetWin_Enable( this, IDC_RADIO_POSITION, FALSE );
	SetWin_Enable( this, IDC_RADIO_ROTATION, FALSE );
	SetWin_Enable( this, IDC_RADIO_SCALE, FALSE );
	SetWin_Enable( this, IDC_CHECK_USECPS, FALSE );

	pAttBoneLink->m_emParentObjType = EMATTBONEPARRENTOBJTYPE_CHAR;
	pAttBoneLink->m_emPieceCharType = PIECE_HEAD;

	CharStateMachine::ClearFileNotifier();

	SetParentObjTypeContrl ();
	//SetMeshSetContrl ();
	SetComboWeaponWhereBack();

}

void CAttachLinkPage::OnBnClickedButtonOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Attach Bone Link File (*.abl)|*.abl|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".abl",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAttachLinkPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxAttBoneDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		DxAttBoneDataContainer::GetInstance().CleanUp();

		CString sFileName = dlg.GetFileName();

		LoadAblFile( sFileName.GetString() );

	}

}

void CAttachLinkPage::OnBnClickedButtonSave()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	std::string strName = sc::util::ChangeExtName( pAttBoneLink->GetFileName(), "" );

	CString szFilter = "Attach Bone Link File (*.abl)|*.abl|";
	CFileDialog dlg(FALSE,".abl", strName.c_str() ,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, (CAttachLinkPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxAttBoneDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		//	Note : 케릭터 파일 쓰기.
		//
		BOOL bOk = pAttBoneLink->SaveABLFile ( sFileName.GetString(), pd3dDevice );
		if ( !bOk )
		{
			MessageBox ( "파일 쓰기에 실패하였습니다." );
		}

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_ABFFILE );
		pWnd->SetWindowText ( sFileName.GetString() );
	}
}


void CAttachLinkPage::OnBnClickedButtonLoadabf()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Attach Bone File (*.abf)|*.abf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".abf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAttachLinkPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxAttBoneDataContainer::GetInstance().GetPath ();


	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();


	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		LoadABFFile( sFileName.GetString() );

		AnimationListing();

	}

}

void CAttachLinkPage::OnBnClickedButtonLoadbone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Bone xFile (*.x,*.mxf)|*.X;*.mxf|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAttachLinkPage*)this);

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	dlg.m_ofn.lpstrInitialDir = DxSkeletonManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

		DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
		DxSkeleton *pAttBoneLinkSkeleton = pView->GetAttBoneLinkSkeleton();

		pAttBoneLink->m_strBaseBoneName = dlg.GetFileName().GetString();

		pAttBoneLinkSkeleton = NULL;
        DxSkeletonResource sSkeletonRes = DxSkeletonManager::GetInstance().LoadSkeletonAndBackUp( dlg.GetFileName().GetString() );
        if ( sSkeletonRes.IsValid() )
        {
            pAttBoneLinkSkeleton = sSkeletonRes.get();
        }

		pView->SetAttBoneLinkSkeleton( pAttBoneLinkSkeleton );

		SetWin_Text( this, IDC_EDIT_BASEBONE, pAttBoneLink->m_strBaseBoneName.c_str() );
		
		CharStateMachine::ClearFileNotifier();
		CharStateMachine::InsertFileNotifier();		
	}
}

void CAttachLinkPage::OnBnClickedButtonGetbone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
	pAttBoneLink->m_spThreadNeedData->m_strLinkBoneName = g_strPICK_BONE;
	

	SetWin_Text( this, IDC_EDIT_LINKBONENAME, pAttBoneLink->m_spThreadNeedData->m_strLinkBoneName.c_str() );

	DxSkeleton    *pLinkSkeleton = pView->GetAttBoneLinkSkeleton();

	D3DXMATRIX matBase;
	D3DXMatrixIdentity( &matBase );
	if( pLinkSkeleton )
	{
		DxBoneTrans* pBoneTran = pLinkSkeleton->FindBone_t( pAttBoneLink->m_spThreadNeedData->m_strLinkBoneName.c_str() );
		if( pBoneTran )
		{
			CheckRadioButton( IDC_RADIO_POSITION, IDC_RADIO_SCALE, IDC_RADIO_POSITION );

			// 상황에 맞는 Matrix 값 유지.
			D3DXMatrixInverse( &pAttBoneLink->m_spThreadNeedData->m_matInverseMatrix, NULL, &pBoneTran->matCombined );
		}
	}
}

void CAttachLinkPage::OnBnClickedButtonLoadcps()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Skin Piece File (*.cps)|*.cps|Vehicle Parts File (*.vps)|*.vps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAttachLinkPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();

	if ( dlg.DoModal() == IDOK )
	{
		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
		pView->GetSkinPiece()->ClearAll ();

		TSTRING strPieceName( dlg.GetFileName().GetString() );
		HRESULT hr = pSkinPiece->LoadPiece( strPieceName, pd3dDevice, FALSE, FALSE );
		if (FAILED(hr))
            return;
		if (stricmp( pSkinPiece->m_szSkeleton, pAttBoneLink->m_strBaseBoneName.c_str()))
		{
			MessageBox( "Base 본과 CPS의 본이 서로 다르므로 해당 CPS를 사용할 수 없습니다.", "오류" );
			pSkinPiece->ClearAll();
			return;
		}

		// 정상일 때만 값을 셋팅한다.
		pAttBoneLink->m_strPieceName = strPieceName;

		SetWin_Text( this, IDC_EDIT_LINKBONENAME, pAttBoneLink->m_spThreadNeedData->m_strLinkBoneName.c_str() );
		SetWin_Text( this, IDC_EDIT_CPSFILE,	  pAttBoneLink->m_strPieceName.c_str() );

		if ( pSkinPiece->m_emPieceType == PIECETYPE_CHARACTER )
		{
			pAttBoneLink->m_emParentObjType	  = EMATTBONEPARRENTOBJTYPE_CHAR;
			pAttBoneLink->m_emPieceCharType	  = pSkinPiece->m_emType;
		}
		else if ( pSkinPiece->m_emPieceType == PIECETYPE_V_PARTS )
		{
			pAttBoneLink->m_emParentObjType	  = EMATTBONEPARRENTOBJTYPE_VEHICLE;
			pAttBoneLink->m_emPieceCharType	  = pSkinPiece->m_emV_PartType;
		}
	
		pAttBoneLink->m_spThreadNeedData->m_emWeaponWhereBack = pSkinPiece->m_emWeaponWhereBack;

		SetParentObjTypeContrl ();
		//SetMeshSetContrl ();
		SetComboWeaponWhereBack();

		CharStateMachine::ClearFileNotifier();
		CharStateMachine::InsertFileNotifier();
	}
}


void CAttachLinkPage::OnBnClickedCheckUsecps()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	pAttBoneLink->m_bUseCPS = GetWin_Check( this, IDC_CHECK_USECPS );

	if( pAttBoneLink->m_bUseCPS )
	{
		SetWin_Enable( this, IDC_BUTTON_LOADCPS, TRUE );

		SetWin_Enable( this, IDC_COMBO_OBJTYPE, FALSE );
		SetWin_Enable( this, IDC_COMBO_SKINMESHTYPE, FALSE );
		SetWin_Enable( this, IDC_COMBO_WHEREBACK, FALSE );
	}else{
		SetWin_Enable( this, IDC_BUTTON_LOADCPS, FALSE );		

		SetWin_Enable( this, IDC_COMBO_OBJTYPE, TRUE );
		SetWin_Enable( this, IDC_COMBO_SKINMESHTYPE, TRUE );
		SetWin_Enable( this, IDC_COMBO_WHEREBACK, TRUE );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
		DxSkinPiece *pSkinPiece = pView->GetSkinPiece();

		pSkinPiece->ClearAll();

		SetWin_Text( this, IDC_EDIT_CPSFILE, "untitle00.cps" );

		pAttBoneLink->m_strPieceName.clear();
	}
	
}

BOOL CAttachLinkPage::LoadABFFile ( const char* szFile )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	pAttBoneLink->SetABFFile( szFile, pd3dDevice );

	SetWin_Text( this, IDC_EDIT_ABFFILE, pAttBoneLink->m_strAttBoneName.c_str() );

	CheckRadioButton( IDC_RADIO_POSITION, IDC_RADIO_SCALE, IDC_RADIO_POSITION );

	OnBnClickedRadioPosition();

	SetWin_Enable( this, IDC_LIST_ANIMATION, TRUE );
	SetWin_Enable( this, IDC_BUTTON_LOADBONE, TRUE );
	SetWin_Enable( this, IDC_EDIT_BASEBONE, TRUE );
	SetWin_Enable( this, IDC_BUTTON_GETBONE, TRUE );
	SetWin_Enable( this, IDC_EDIT_LINKBONENAME, TRUE );
	SetWin_Enable( this, IDC_RADIO_POSITION, TRUE );
	SetWin_Enable( this, IDC_RADIO_ROTATION, TRUE );
	SetWin_Enable( this, IDC_RADIO_SCALE, TRUE );
	SetWin_Enable( this, IDC_CHECK_USECPS, TRUE );

	SetWin_Check( this, IDC_CHECK_NONATKMODE, FALSE );

	CharStateMachine::ClearFileNotifier();
	CharStateMachine::InsertFileNotifier();

	return TRUE;
}

BOOL CAttachLinkPage::LoadAblFile( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
	DxSkeleton *pAttBoneLinkSkeleton = pView->GetAttBoneLinkSkeleton();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();

	OnBnClickedButtonNew();

    BOOL bOk = pAttBoneLink->LoadABLFile(std::string(pszFileName), pd3dDevice, EMSCD_ZERO );
	if ( !bOk )
	{
		MessageBox ( "파일 읽기에 실패하였습니다." );
		return FALSE;
	}


	// Load 본 파일
	pAttBoneLinkSkeleton = NULL;
    DxSkeletonResource sSkeletonRes = DxSkeletonManager::GetInstance().LoadSkeletonAndBackUp( pAttBoneLink->m_strBaseBoneName );
    if ( sSkeletonRes.IsValid() )
    {
        pAttBoneLinkSkeleton = sSkeletonRes.get();
    }
	pView->SetAttBoneLinkSkeleton( pAttBoneLinkSkeleton );




	pAttBoneLink->GetAttBone()->m_spSkinAniControlThread->SELECTANI ( AN_ATTBONE_STAY );

	const DxSkinAnimation* pSelectedAniCont = pAttBoneLink->GetAttBone()->m_spSkinAniControlThread->GETCURANIM ();
	if ( pSelectedAniCont )
	{
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
		if ( dwUnitTime > 0 )
		{
			pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
				pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
		}

		pFrame->m_cDlgTimeLine.SetPlay();
	}

	// Load CPS
	pView->GetSkinPiece()->ClearAll ();

	if( !pAttBoneLink->m_strPieceName.empty() )
	{
		HRESULT hr = pSkinPiece->LoadPiece( pAttBoneLink->m_strPieceName, pd3dDevice, FALSE, FALSE );
		if (FAILED(hr))
            return FALSE;
	}

	SetWin_Text( this, IDC_EDIT_ABFFILE, pAttBoneLink->m_strAttBoneName.c_str() );

	SetWin_Enable( this, IDC_LIST_ANIMATION, TRUE );
	SetWin_Enable( this, IDC_BUTTON_LOADBONE, TRUE );
	SetWin_Enable( this, IDC_EDIT_BASEBONE, TRUE );
	SetWin_Enable( this, IDC_BUTTON_GETBONE, TRUE );
	SetWin_Enable( this, IDC_EDIT_LINKBONENAME, TRUE );
	SetWin_Enable( this, IDC_RADIO_POSITION, TRUE );
	SetWin_Enable( this, IDC_RADIO_ROTATION, TRUE );
	SetWin_Enable( this, IDC_RADIO_SCALE, TRUE );
	SetWin_Enable( this, IDC_CHECK_USECPS, TRUE );

	CheckRadioButton( IDC_RADIO_POSITION, IDC_RADIO_SCALE, IDC_RADIO_POSITION );

	UpdateEditWindow();

	UpdateValueData();

	AnimationListing();
	
	// Bone 링크

	D3DXMATRIX mat;
	D3DXMatrixIdentity( &mat );
	pAttBoneLinkSkeleton->ResetBone_PureThread();
	pAttBoneLinkSkeleton->UpdateBones_PureThread( pAttBoneLinkSkeleton->m_skeletonPart.pBoneRoot, mat, 1.f, 1.f );
	g_strPICK_BONE = pAttBoneLink->m_spThreadNeedData->m_strLinkBoneName;
	OnBnClickedButtonGetbone();

	CharStateMachine::InsertFileNotifier();

	return TRUE;
}


void CAttachLinkPage::AnimationListing ()
{

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	m_list_Animation.ResetContent();

	const DxSkinAniControl::VECNODE& vecANI = pAttBoneLink->GetAttBone()->m_spSkinAniControlThread->GetAniList ();
	for ( size_t n=0; n<vecANI.size(); ++n )
	{
		m_list_Animation.AddString ( vecANI[n]->m_pDxSkinAni->m_strCfgFile.c_str() );
	}

	
}

void CAttachLinkPage::SetParentObjTypeContrl ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	int nIndex = 0;
	nIndex = m_comboObjType.FindString ( 0, SKINTYPE_STING::m_szATTPARENTOBJTYPE[pAttBoneLink->m_emParentObjType] );
	m_comboObjType.SetCurSel ( nIndex );

	UpdateObjTypeComboBox();
}

void CAttachLinkPage::SetMeshSetContrl ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	int nIndex = 0;
	nIndex = m_comboSkinMeshType.FindString ( 0, DxSkinPiece::m_szPIECETYPE[pAttBoneLink->m_emPieceCharType] );

	m_comboSkinMeshType.SetCurSel ( nIndex );
}

void CAttachLinkPage::SetComboWeaponWhereBack()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	int nIndex = m_comboWhereBack.FindString ( 0, SKINTYPE_STING::m_szWHEREBACKTYPE[pAttBoneLink->m_spThreadNeedData->m_emWeaponWhereBack] );
	m_comboWhereBack.SetCurSel ( nIndex );
}


void CAttachLinkPage::UpdateEditWindow()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();


	SetWin_Text( this, IDC_EDIT_ABLFILE, pAttBoneLink->GetFileName() );
	SetWin_Text( this, IDC_EDIT_ABFFILE, pAttBoneLink->m_strAttBoneName.c_str() );
	SetWin_Text( this, IDC_EDIT_BASEBONE, pAttBoneLink->m_strBaseBoneName.c_str() );
	SetWin_Text( this, IDC_EDIT_LINKBONENAME, pAttBoneLink->m_spThreadNeedData->m_strLinkBoneName.c_str() );
	SetWin_Text( this, IDC_EDIT_CPSFILE, pAttBoneLink->m_strPieceName.c_str() );
	SetWin_Check( this, IDC_CHECK_USECPS, pAttBoneLink->m_bUseCPS );
	SetWin_Enable( this, IDC_BUTTON_LOADCPS, pAttBoneLink->m_bUseCPS );

	UpdateValueData();

	// Note : 초기화
	SetWin_Enable( this, IDC_COMBO_OBJTYPE, TRUE );
	SetWin_Enable( this, IDC_COMBO_SKINMESHTYPE, TRUE );
	SetWin_Enable( this, IDC_COMBO_WHEREBACK, TRUE );

	if( pAttBoneLink->m_bUseCPS )
	{
		SetWin_Enable( this, IDC_COMBO_OBJTYPE, FALSE );
		SetWin_Enable( this, IDC_COMBO_SKINMESHTYPE, FALSE );
		SetWin_Enable( this, IDC_COMBO_WHEREBACK, FALSE );


		SetParentObjTypeContrl ();
		//SetMeshSetContrl ();
		SetComboWeaponWhereBack();

	}
}

void CAttachLinkPage::UpdateObjTypeComboBox ()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	int nIndex = m_comboObjType.GetCurSel ();

	EMATTBONEPARRENTOBJTYPE emParentObjType = EMATTBONEPARRENTOBJTYPE_CHAR;
	if ( nIndex != LB_ERR )
	{
		emParentObjType = (EMATTBONEPARRENTOBJTYPE)m_comboObjType.GetItemData ( nIndex );	
		pAttBoneLink->m_emParentObjType = emParentObjType;
	}

	int        nSize            = 0;
	const char (*ppstrType)[64] = NULL;

	switch ( emParentObjType )
	{
	case EMATTBONEPARRENTOBJTYPE_CHAR:
		nSize     = PIECE_NONE;
		ppstrType = DxSkinPiece::m_szPIECETYPE;
		break;
	case EMATTBONEPARRENTOBJTYPE_VEHICLE:
		nSize     = PART_V_SIZE;
		ppstrType = DxSkinPiece::m_szV_PARTSTYPE;
		break;
	}

	m_comboSkinMeshType.ResetContent();

	for ( int i=0; i<nSize; ++i )
	{
		int nIndex = m_comboSkinMeshType.AddString ( ppstrType[i] );
		m_comboSkinMeshType.SetItemData ( nIndex, i );
	}

	m_comboSkinMeshType.SetCurSel (pAttBoneLink->m_emPieceCharType);
	//pAttBoneLink->m_emPieceCharType = (EMPIECECHAR) m_comboSkinMeshType.GetItemData (0);
}

void CAttachLinkPage::OnLbnDblclkListAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	CListBox *pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ANIMATION );
	DWORD nIndex = pListBox->GetCurSel();
	if ( nIndex == -1 ) return;

	CString sItemText;
	pListBox->GetText ( nIndex, sItemText );

	pAttBoneLink->GetAttBone()->m_spSkinAniControlThread->SELECTANI_EDIT ( sItemText.GetString() );

	const DxSkinAnimation* pSelectedAniCont = pAttBoneLink->GetAttBone()->m_spSkinAniControlThread->GETCURANIM ();
	DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
	if ( dwUnitTime > 0 )
	{
		pFrame->SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
			pSelectedAniCont->m_dwETimeOrig/dwUnitTime, dwUnitTime );
	}

	pFrame->m_cDlgTimeLine.SetPlay();	
}


void CAttachLinkPage::OnBnClickedRadioPosition()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

    bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( bNonAtkMode )
	{
		SetWin_Num_float( this, IDC_EDIT_VALUE1, pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos.x );
		SetWin_Num_float( this, IDC_EDIT_VALUE2, pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos.y );
		SetWin_Num_float( this, IDC_EDIT_VALUE3, pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos.z );
	}else{
		SetWin_Num_float( this, IDC_EDIT_VALUE1, pAttBoneLink->m_spThreadNeedData->m_vPos.x );
		SetWin_Num_float( this, IDC_EDIT_VALUE2, pAttBoneLink->m_spThreadNeedData->m_vPos.y );
		SetWin_Num_float( this, IDC_EDIT_VALUE3, pAttBoneLink->m_spThreadNeedData->m_vPos.z );
	}

}

void CAttachLinkPage::OnBnClickedRadioRotation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( bNonAtkMode )
	{
		SetWin_Num_float( this, IDC_EDIT_VALUE1, pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate.x );
		SetWin_Num_float( this, IDC_EDIT_VALUE2, pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate.y );
		SetWin_Num_float( this, IDC_EDIT_VALUE3, pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate.z );
	}else{
		SetWin_Num_float( this, IDC_EDIT_VALUE1, pAttBoneLink->m_spThreadNeedData->m_vRotate.x );
		SetWin_Num_float( this, IDC_EDIT_VALUE2, pAttBoneLink->m_spThreadNeedData->m_vRotate.y );
		SetWin_Num_float( this, IDC_EDIT_VALUE3, pAttBoneLink->m_spThreadNeedData->m_vRotate.z );
	}

}

void CAttachLinkPage::OnBnClickedRadioScale()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( bNonAtkMode )
	{
		SetWin_Num_float( this, IDC_EDIT_VALUE1, pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale.x );
		SetWin_Num_float( this, IDC_EDIT_VALUE2, pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale.y );
		SetWin_Num_float( this, IDC_EDIT_VALUE3, pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale.z );
	}else{
		SetWin_Num_float( this, IDC_EDIT_VALUE1, pAttBoneLink->m_spThreadNeedData->m_vScale.x );
		SetWin_Num_float( this, IDC_EDIT_VALUE2, pAttBoneLink->m_spThreadNeedData->m_vScale.y );
		SetWin_Num_float( this, IDC_EDIT_VALUE3, pAttBoneLink->m_spThreadNeedData->m_vScale.z );
	}

}

void CAttachLinkPage::OnBnClickedButtonReset1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		else			  pAttBoneLink->m_spThreadNeedData->m_vPos		 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		else			  pAttBoneLink->m_spThreadNeedData->m_vRotate		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}else if( GetWin_Check( this, IDC_RADIO_SCALE ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
		else			  pAttBoneLink->m_spThreadNeedData->m_vScale	   = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	}

	UpdateValueData();
}

void CAttachLinkPage::OnCbnSelchangeComboObjtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	if ( !pAttBoneLink )
	{
		return;
	}

	pAttBoneLink->m_emPieceCharType = 0;

	UpdateObjTypeComboBox ();
}

void CAttachLinkPage::OnCbnSelchangeComboSkinmeshtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	int nIndex = m_comboSkinMeshType.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = m_comboSkinMeshType.GetItemData ( nIndex );
	pAttBoneLink->m_emPieceCharType = dwData;
}

void CAttachLinkPage::OnCbnSelchangeComboWhereback()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	int nIndex = m_comboWhereBack.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = m_comboWhereBack.GetItemData ( nIndex );
	pAttBoneLink->m_spThreadNeedData->m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwData;
}

void CAttachLinkPage::UpdateValueData( bool bSet /* = TRUE */ )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( bSet )
	{
		D3DXVECTOR3 vSetValue;
		if( GetWin_Check( this, IDC_RADIO_POSITION ) )
		{
			if( !bNonAtkMode )	vSetValue =  pAttBoneLink->m_spThreadNeedData->m_vPos;						
			else				vSetValue =  pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos;
		}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
		{
			if( !bNonAtkMode )	vSetValue =  pAttBoneLink->m_spThreadNeedData->m_vRotate;						
			else				vSetValue =  pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate;
		}else if( GetWin_Check( this, IDC_RADIO_SCALE ) )
		{
			if( !bNonAtkMode )	vSetValue =  pAttBoneLink->m_spThreadNeedData->m_vScale;						
			else				vSetValue =  pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale;
		}

		SetWin_Num_float( this, IDC_EDIT_VALUE1, vSetValue.x );
		SetWin_Num_float( this, IDC_EDIT_VALUE2, vSetValue.y );
		SetWin_Num_float( this, IDC_EDIT_VALUE3, vSetValue.z );

		// Note : 타입설정
		INT nMaxObjType = m_comboObjType.GetCount();
		for ( INT i=0; i<nMaxObjType; ++i )
		{
			DWORD dwData = m_comboObjType.GetItemData (i);

			if ( (DWORD)pAttBoneLink->m_emParentObjType == dwData )
			{
				m_comboObjType.SetCurSel(i);
				break;
			}
		}

		// Note : 부위타입 리스트 설정
		int        nSize            = 0;
		const char (*ppstrType)[64] = NULL;

		switch ( pAttBoneLink->m_emParentObjType )
		{
		case EMATTBONEPARRENTOBJTYPE_CHAR:
			nSize     = PIECE_NONE;
			ppstrType = DxSkinPiece::m_szPIECETYPE;
			break;
		case EMATTBONEPARRENTOBJTYPE_VEHICLE:
			nSize     = PART_V_SIZE;
			ppstrType = DxSkinPiece::m_szV_PARTSTYPE;
			break;
		}

		m_comboSkinMeshType.ResetContent();

		for ( int i=0; i<nSize; ++i )
		{
			int nIndex = m_comboSkinMeshType.AddString ( ppstrType[i] );
			m_comboSkinMeshType.SetItemData ( nIndex, i );
		}

		// Note : 부위설정
		INT nMaxSkinMeshTypeCount = m_comboSkinMeshType.GetCount();
		for ( INT i=0; i<nMaxSkinMeshTypeCount; ++i )
		{
			DWORD dwData = m_comboSkinMeshType.GetItemData (i);

			if ( (DWORD)pAttBoneLink->m_emPieceCharType == dwData )
			{
				m_comboSkinMeshType.SetCurSel(i);
				break;
			}
		}

		// Note : WeaponWhereBack 설정
		INT nMaxWhereBackCount = m_comboWhereBack.GetCount();
		for ( INT i=0; i<nMaxWhereBackCount; ++i )
		{
			DWORD dwData = m_comboWhereBack.GetItemData (i);

			if ( (DWORD)pAttBoneLink->m_spThreadNeedData->m_emWeaponWhereBack == dwData )
			{
				m_comboWhereBack.SetCurSel(i);
				break;
			}
		}

	}else{
		D3DXVECTOR3 vGetValue;
		vGetValue.x = GetWin_Num_float( this, IDC_EDIT_VALUE1 );
		vGetValue.y = GetWin_Num_float( this, IDC_EDIT_VALUE2 );
		vGetValue.z = GetWin_Num_float( this, IDC_EDIT_VALUE3 );

		if( GetWin_Check( this, IDC_RADIO_POSITION ) )
		{
			if( !bNonAtkMode )  pAttBoneLink->m_spThreadNeedData->m_vPos	   = vGetValue;
			else				pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos = vGetValue;
		}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
		{
			if( !bNonAtkMode )  pAttBoneLink->m_spThreadNeedData->m_vRotate		  = vGetValue;
			else				pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate = vGetValue;
		}else if( GetWin_Check( this, IDC_RADIO_SCALE ) )
		{
			if( !bNonAtkMode )  pAttBoneLink->m_spThreadNeedData->m_vScale		 = vGetValue;
			else				pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale = vGetValue;
		}

	}
}


void CAttachLinkPage::OnDeltaposSpinValue1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
	float fDelta = GetWin_Num_float( this, IDC_EDIT_TERM );

	bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos.x += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vPos.x	   += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate.x += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vRotate.x		  += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_SCALE ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale.x += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vScale.x		 += (float)pNMUpDown->iDelta * fDelta;
	}


	UpdateValueData();
}

void CAttachLinkPage::OnDeltaposSpinValue2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
	float fDelta = GetWin_Num_float( this, IDC_EDIT_TERM );

	bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos.y += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vPos.y	   += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate.y += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vRotate.y		  += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_SCALE ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale.y += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vScale.y		 += (float)pNMUpDown->iDelta * fDelta;
	}


	UpdateValueData();
}

void CAttachLinkPage::OnDeltaposSpinValue3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
	float fDelta = GetWin_Num_float( this, IDC_EDIT_TERM );

	bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos.z += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vPos.z	   += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate.z += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vRotate.z		  += (float)pNMUpDown->iDelta * fDelta;
	}else if( GetWin_Check( this, IDC_RADIO_SCALE ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale.z += (float)pNMUpDown->iDelta * fDelta;
		else			  pAttBoneLink->m_spThreadNeedData->m_vScale.z		 += (float)pNMUpDown->iDelta * fDelta;
	}

	UpdateValueData();
}

void CAttachLinkPage::OnEnChangeEditValue1()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CAttachLinkPage::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	UpdateValueData( FALSE );
	CPropertyPage::OnTimer(nIDEvent);
}

void CAttachLinkPage::OnBnClickedCheckAtkmode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();
	pAttBoneLink->SetAttackMode( !GetWin_Check( this, IDC_CHECK_NONATKMODE ) );

	UpdateValueData();

}

void CAttachLinkPage::OnBnClickedButtonLinkInverseMatrix()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	D3DXVECTOR3 vScale(1.f,1.f,1.f);
	D3DXQUATERNION vQuat;
	D3DXQuaternionIdentity( &vQuat );
	D3DXVECTOR3 vPos(0.f,0.f,0.f);

	DxSkeleton    *pLinkSkeleton = pView->GetAttBoneLinkSkeleton();
	D3DXMATRIX matBase;
	D3DXMatrixIdentity( &matBase );
	if( pLinkSkeleton )
	{
		DxBoneTrans* pBoneTran = pLinkSkeleton->FindBone_t( pAttBoneLink->m_spThreadNeedData->m_strLinkBoneName.c_str() );
		if( pBoneTran )
		{
			// 회전만 Link Matrix 를 따라가지 않는다.
			pAttBoneLink->m_spThreadNeedData->m_bInverseMatrix = TRUE;
			D3DXMatrixInverse( &pAttBoneLink->m_spThreadNeedData->m_matInverseMatrix, NULL, &pBoneTran->matCombined );
		}
	}




	bool bNonAtkMode = GetWin_Check( this, IDC_CHECK_NONATKMODE ) ? true : false;

	if( GetWin_Check( this, IDC_RADIO_POSITION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkPos = vPos;
		else			  pAttBoneLink->m_spThreadNeedData->m_vPos		 = vPos;
	}
	else if( GetWin_Check( this, IDC_RADIO_ROTATION ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkRotate = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		else			  pAttBoneLink->m_spThreadNeedData->m_vRotate		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}
	else if( GetWin_Check( this, IDC_RADIO_SCALE ) )
	{
		if( bNonAtkMode ) pAttBoneLink->m_spThreadNeedData->m_vNonAtkScale = vScale;
		else			  pAttBoneLink->m_spThreadNeedData->m_vScale	   = vScale;
	}
}

void CAttachLinkPage::OnBnClickedButtonLinkMatrix()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DxAttBoneLink *pAttBoneLink = pView->GetAttBoneLink();

	// 링크 Matrix를 그대로 따라간다.
	pAttBoneLink->m_spThreadNeedData->m_bInverseMatrix = FALSE;
}
