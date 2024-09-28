// FramePage.cpp : implementation file
//

#include "stdafx.h"
#include "../charedit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxAttBoneLink.h"
#include "../EngineLib/DxEffect/Char/dxeffchardata.h"

#include "SheetWithTabChar.h"
#include "SheetWithTabFrame.h"
#include "SheetWithTabAnim.h"

#ifdef DEF_SKIN_SYNC
#include "SheetWithTabPiece.h"
#endif

#include "../../MfcExLib/ExLibs/FECFileDialog.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "FramePage.h"
#include ".\framepage.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CFramePage property page

IMPLEMENT_DYNCREATE(CFramePage, CPropertyPage)

CFramePage::CFramePage() :
	CPropertyPage(CFramePage::IDD),
	m_pSheetTab(NULL)
	, m_fSetYPos(0)
	{
	//{{AFX_DATA_INIT(CFramePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFramePage::~CFramePage()
{
}

void CFramePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PIECE, m_PieceList);
	DDX_Text(pDX, IDC_EDIT_SET_HEIGHT, m_fSetYPos);
}


BEGIN_MESSAGE_MAP(CFramePage, CPropertyPage)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_FRAME, OnRclickTreeFrame)
#ifndef DEF_SKIN_SYNC
	ON_BN_CLICKED(IDC_BUTTON_PIECE_SELECT, OnBnClickedButtonPieceSelect)
#else
    ON_BN_CLICKED(IDC_BUTTON_ANI_EDIT, OnBnClickedButtonAniEdit)
#endif
	ON_BN_CLICKED(IDC_BUTTON_PIECE_DESELECT, OnBnClickedButtonPieceDeselect)
	ON_BN_CLICKED(IDC_BUTTON_SKINBONE, OnBnClickedButtonSkinbone)
	ON_BN_CLICKED(IDC_BUTTON_EFF_LOAD, OnBnClickedButtonEffLoad)
	ON_BN_CLICKED(IDC_BUTTON_EFF_SAVE, OnBnClickedButtonEffSave)
	ON_EN_CHANGE(IDC_EDIT_SKIN_SCALE, OnEnChangeEditSkinScale)
	ON_BN_CLICKED(IDC_BUTTON_SCALE, OnBnClickedButtonScale)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_EFF_NAME, OnBnClickedButtonEffName)
	ON_BN_CLICKED(IDC_BUTTON_SET_HEIGHT, OnBnClickedButtonSetHeight)
	ON_BN_CLICKED(IDC_BUTTON_BOARDPOS, OnBnClickedButtonEffBoardpos)
	ON_BN_CLICKED(IDC_BUTTON_SKIN_SETTING, &CFramePage::OnBnClickedButtonSkinSetting)
	ON_BN_CLICKED(IDC_BUTTON_BONE_SETTING, &CFramePage::OnBnClickedButtonBoneSetting)
	//ON_BN_CLICKED(IDC_BUTTON_COLLISION_OBJECT, &CFramePage::OnBnClickedButtonCollisionObject)
    ON_NOTIFY(NM_CLICK, IDC_LIST_PIECE, &CFramePage::OnNMClickListPiece)
	ON_NOTIFY(NM_DBLCLK,IDC_LIST_PIECE, &CFramePage::OnNMDBLCLKListPiece)
	ON_BN_CLICKED(IDC_BUTTON_FRAME_DOCUMENT, &CFramePage::OnBnClickedButtonFrameDocument)
	ON_BN_CLICKED(IDC_BUTTON_WEAPON_SCALE2, &CFramePage::OnBnClickedButtonWeaponScale2)
	ON_BN_CLICKED(IDC_BUTTON_BILLBOARD, &CFramePage::OnBnClickedButtonBillboard)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFramePage message handlers
BOOL CFramePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_SKINBONE );
	pWnd->SetWindowText ( "" );

	



	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFramePage::SetFrameEditType( EMFRAMEEDITTYPE emFrameEditType )
{ 
	m_emFrameEditType = emFrameEditType; 
	m_PieceList.DeleteAllItems();
	m_PieceList.DeleteColumn(1);
	m_PieceList.DeleteColumn(0);

	RECT listRect;
	m_PieceList.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[2] = { int(nListWidth*0.3f), int(nListWidth*0.7f) };

	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{	
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_SAVE, TRUE );
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_LOAD, TRUE );
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_NAME, TRUE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOARDPOS, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_SET, TRUE );
		SetWin_ShowWindow( this, IDC_EDIT_SET_HEIGHT, TRUE );
		SetWin_ShowWindow( this, IDC_BUTTON_SET_HEIGHT, TRUE );

		m_PieceList.SetExtendedStyle ( m_PieceList.GetExtendedStyle() |
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

		m_PieceList.InsertColumn ( 0, _T("종류"),	LVCFMT_CENTER, nColWidth[0] );
		m_PieceList.InsertColumn ( 1, _T("cps"),	LVCFMT_CENTER, nColWidth[1] );

		for ( int i=0; i<PIECE_SIZE; ++i )
		{
			m_PieceList.InsertItem ( i, DxSkinPiece::m_szPIECETYPE[i], NULL );
			m_PieceList.SetItemData ( i, DWORD(i) );
			m_PieceList.SetItemText ( i, 1, "미지정" );
		}

		SetPieceList ();
	}else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_SAVE, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_LOAD, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_NAME, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOARDPOS, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_SET, FALSE );
		SetWin_ShowWindow( this, IDC_EDIT_SET_HEIGHT, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_SET_HEIGHT, FALSE );

		m_PieceList.SetExtendedStyle ( m_PieceList.GetExtendedStyle() |
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

		m_PieceList.InsertColumn ( 0, _T("종류"),	LVCFMT_CENTER, nColWidth[0] );
		m_PieceList.InsertColumn ( 1, _T("aps"),	LVCFMT_CENTER, nColWidth[1] );

		for ( int i=0; i<ATTBONE_SIZE; ++i )
		{
			m_PieceList.InsertItem ( i, DxSkinPiece::m_szATTBONETYPE[i], NULL );
			m_PieceList.SetItemData ( i, DWORD(i) );
			m_PieceList.SetItemText ( i, 1, "미지정" );
		}

		SetAttBoneList ();
	}else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_SAVE, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_LOAD, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_EFF_NAME, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOARDPOS, TRUE );
		SetWin_ShowWindow( this, IDC_BUTTON_SET, TRUE );
		SetWin_ShowWindow( this, IDC_EDIT_SET_HEIGHT, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_SET_HEIGHT, FALSE );

		m_PieceList.SetExtendedStyle ( m_PieceList.GetExtendedStyle() |
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

		m_PieceList.InsertColumn ( 0, _T("종류"),	LVCFMT_CENTER, nColWidth[0] );
		m_PieceList.InsertColumn ( 1, _T("vps"),	LVCFMT_CENTER, nColWidth[1] );

		for ( int i=0; i<PART_V_SIZE; ++i )
		{
			m_PieceList.InsertItem ( i, DxSkinPiece::m_szV_PARTSTYPE[i], NULL );
			m_PieceList.SetItemData ( i, DWORD(i) );
			m_PieceList.SetItemText ( i, 1, "미지정" );
		}

		SetVehiclePartList ();
	}
}

void CFramePage::SetVehiclePartList ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame ) return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView ) return;
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	DxVehicle *pVehicle = pView->GetVehicle();
	DxVehicleData* pVehicleData = pView->GetVehicleData ();

	if( !pVehicle || !pVehicleData ) return;

#ifndef DEF_SKIN_SYNC
	m_PieceList.DeleteAllItems();
#endif

	PDXSKINPIECE   pVehiclePart = NULL;
	PDXATTBONELINK pAttBoneLink = NULL;

	for ( int i=0; i<PART_V_SIZE; ++i )
	{
        if (m_PieceList.GetItemCount() < PART_V_SIZE)
        {
		    m_PieceList.InsertItem ( i, DxSkinPiece::m_szV_PARTSTYPE[i], NULL );
        }

		m_PieceList.SetItemData ( i, (DWORD) i );

		TSTRING strVehicleName = pVehicleData->GetVehicleName( i );

		if ( strVehicleName.empty() )
		{
			m_PieceList.SetItemText ( i, 1, "미지정" );
		}
		else
		{
			m_PieceList.SetItemText ( i, 1, strVehicleName.c_str() );
		}
	}
}


void CFramePage::SetAttBoneList ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame ) return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView ) return;
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	DxAttBone *pAttBone = pView->GetAttBone();
	DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();

	if( !pAttBone || !pAttBoneData ) return;

#ifndef DEF_SKIN_SYNC
	m_PieceList.DeleteAllItems ();
#endif

	PDXSKINPIECE   pAttBonePiece = NULL;
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		TSTRING strAttBoneName = pAttBoneData->GetAttBoneName(i);

        if (m_PieceList.GetItemCount() < ATTBONE_SIZE)
        {
		    m_PieceList.InsertItem ( i, DxSkinPiece::m_szATTBONETYPE[i], NULL );
        }

		m_PieceList.SetItemData ( i, (DWORD) i );

		if ( strAttBoneName.empty() )
		{
			m_PieceList.SetItemText ( i, 1, "미지정" );
		}
		else
		{
			m_PieceList.SetItemText ( i, 1, strAttBoneName.c_str() );
		}
	}
}

void CFramePage::SetPieceList ()
{

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	if( !pFrame ) return;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if( !pView ) return;
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();

	if( !pSkinChar || !pSkinCharData ) return;

#ifndef DEF_SKIN_SYNC
	m_PieceList.DeleteAllItems ();
#endif

	PDXSKINPIECE   pSkinPiece	= NULL;

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
        if (m_PieceList.GetItemCount() < PIECE_SIZE)
        {
		    m_PieceList.InsertItem ( i, DxSkinPiece::m_szPIECETYPE[i], NULL );
        }

		m_PieceList.SetItemData ( i, (DWORD) i );

#ifndef DEF_SKIN_SYNC
		DxSkinPieceBase* pSkinPieceBase = pSkinCharData->GetPiece( pd3dDevice, i, EMSCD_ZERO );
		if ( pSkinPieceBase )
		{
			m_PieceList.SetItemText ( i, 1, pSkinPieceBase->GetFileName() );
		}
		else
		{
			m_PieceList.SetItemText ( i, 1, "미지정" );
		}
#else
        const TCHAR* pName = pSkinCharData->GetPiece(EMPIECECHAR(i));

        if (pName && _tcsclen(pName))
        {
            m_PieceList.SetItemText (i, 1, pName);
        }
        else
        {
            m_PieceList.SetItemText (i, 1, _T("미지정"));
        }
#endif
	}
}

void CFramePage::OnRclickTreeFrame(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMenu Menu;
	int pMenuID = 0;

	DWORD SelectionMade;
	VERIFY ( Menu.LoadMenu ( IDR_POPUP_FRAME ) );

	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );

	POINT pp;
	GetCursorPos (&pp);
	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pp.x, pp.y, this );

	pPopup->DestroyMenu ();

	switch ( SelectionMade )
	{
	case ID_POPUP_TOFRAME_VIEWMOVE:
		{
			//HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
			//if ( htreeitemCur == NULL )	return;

			//CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );
			//char *szFrameName = sSelectedFrame.GetString(sSelectedFrame.GetLength()+1);

			//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			//CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
			//DxSkinObject *pSkinObject = pView->GetSkinObject ();
			//DxSkinMesh *pSkinMesh = pSkinObject->GetSkinMesh();

			//if ( pSkinMesh )
			//{
			//	SFrame *psFrame = pSkinMesh->FindFrame ( szFrameName );

			//	if ( psFrame )
			//	{
			//		D3DXVECTOR3 vToMove;
			//		vToMove.x = psFrame->matCombined._41;
			//		vToMove.y = psFrame->matCombined._42;
			//		vToMove.z = psFrame->matCombined._43;

			//		DxViewPort::GetInstance().CameraJump ( vToMove );
			//	}
			//}
		}
		break;

	case ID_POPUP_FRAMESETTING:
		break;

	default:
		break;
	};
	
	*pResult = 0;
}

BOOL CFramePage::LoadSkinBone( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();


	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{
		DxSkinChar *pSkinChar = pView->GetSkinChar();
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();

		CString sFileName =pszFileName;
		BOOL bOk = pSkinCharData->GetSkinAniControlData()->LoadSkeleton ( sFileName.GetString(), pd3dDevice );
		if ( !bOk )
		{
			MessageBox ( "본파일 읽기에 실패하였습니다." );
			return FALSE;
		}
		else
		{
			pSkinChar->SetCharData ( pSkinCharData, pd3dDevice, false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

			CWnd *pWnd;
			pWnd = GetDlgItem ( IDC_EDIT_SKINBONE );
			pWnd->SetWindowText ( sFileName.GetString() );
		}
	}else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{
		DxAttBone *pAttBoneChar = pView->GetAttBone();
		DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();

		CString sFileName =pszFileName;
		BOOL bOk = pAttBoneData->GetSkinAniControlData()->LoadSkeleton ( sFileName.GetString(), pd3dDevice );
		if ( !bOk )
		{
			MessageBox ( "본파일 읽기에 실패하였습니다." );
			return FALSE;
		}
		else
		{
			pAttBoneChar->SetAttBoneData ( pAttBoneData, pd3dDevice );

			CWnd *pWnd;
			pWnd = GetDlgItem ( IDC_EDIT_SKINBONE );
			pWnd->SetWindowText ( sFileName.GetString() );
		}
	}else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{
		DxVehicle *pVehicle = pView->GetVehicle();
		DxVehicleData* pVehicleData = pView->GetVehicleData ();

		CString sFileName =pszFileName;
		BOOL bOk = pVehicleData->GetSkinAniControlData()->LoadSkeleton ( sFileName.GetString(), pd3dDevice );
		if ( !bOk )
		{
			MessageBox ( "본파일 읽기에 실패하였습니다." );
			return FALSE;
		}
		else
		{
			pVehicle->SetVehiclePartData ( pVehicleData, pd3dDevice );

			CWnd *pWnd;
			pWnd = GetDlgItem ( IDC_EDIT_SKINBONE );
			pWnd->SetWindowText ( sFileName.GetString() );
		}
	}
	return TRUE;
}

void CFramePage::OnBnClickedButtonSkinbone()
{
	
	CString szFilter = "Bone xFile (*.x,*.mxf)|*.X;*.mxf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CFramePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkeletonManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		LoadSkinBone( dlg.GetFileName().GetString() );
	}

}

HRESULT CFramePage::InsertVehiclePart( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	DxVehicle *pVehicle = pView->GetVehicle();
	DxVehicleData* pVehicleData = pView->GetVehicleData ();

	//	Note : 새로이 Piece를 설정한다.
	//
	HRESULT hr;
	//hr = pVehicleData->SetVehiclePart( pszFileName, pd3dDevice, FALSE, FG_BONEFILE_CHECK|FG_MUSTNEWLOAD );
	hr = pVehicleData->SetPieceEDIT( pszFileName, pd3dDevice, EMSCD_ZERO, FG_BONEFILE_CHECK|FG_MUSTNEWLOAD );
	if ( FAILED(hr) )
	{
		if ( hr == E_DIFFERENT_BONEFILE )
			MessageBox ( "삽입하고자 하는 '조각'의 본파일과 현제 지정된 본파일이 틀립니다." );

		return hr;
	}

	pVehicle->SetVehiclePartData ( pVehicleData, pd3dDevice );
	SetVehiclePartList ();

	return S_OK;
}

HRESULT CFramePage::InsertAttBone( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	DxAttBone *pAttBoneChar = pView->GetAttBone();
	DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();

	//	Note : 새로이 Piece를 설정한다.
	//
	HRESULT hr;
	hr = pAttBoneData->SetAttBonePiece( pszFileName, pd3dDevice, EMSCD_ZERO, FG_BONEFILE_CHECK|FG_MUSTNEWLOAD );
	if ( FAILED(hr) )
	{
		if ( hr == E_DIFFERENT_BONEFILE )
			MessageBox ( "삽입하고자 하는 '조각'의 본파일과 현제 지정된 본파일이 틀립니다." );

		return hr;
	}

	pAttBoneChar->SetAttBoneData ( pAttBoneData, pd3dDevice );
	SetAttBoneList ();

	return S_OK;
}

HRESULT CFramePage::InsertSkinPiece( const char *pszFileName )
{

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();


	//	Note : 새로이 Piece를 설정한다.
	//
	HRESULT hr = pSkinCharData->SetPieceEDIT( pszFileName, pd3dDevice, EMSCD_ZERO, FG_BONEFILE_CHECK|FG_MUSTNEWLOAD );
	if ( FAILED(hr) )
	{
		if ( hr == E_DIFFERENT_BONEFILE )
			MessageBox ( _T("삽입하고자 하는 '조각'의 본파일과 현재 지정된 본파일이 틀립니다.") );

		return S_FALSE;
	}

	pSkinChar->SetCharData( pSkinCharData, pd3dDevice, FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE, TRUE );
	SetPieceList ();

	return S_OK;	
}

#ifndef DEF_SKIN_SYNC
void CFramePage::OnBnClickedButtonPieceSelect()
{

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();



	CString strSkeleton;

	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{
		DxSkinChar *pSkinChar = pView->GetSkinChar();
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();
		strSkeleton = pSkinCharData->GetSkeletonName ();
	}else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{
		DxAttBone *pAttBoneChar = pView->GetAttBone();
		DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();

		strSkeleton = pAttBoneData->GetSkinAniControlData()->GetSkeletonName ();
	}else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{
		DxVehicle *pVehicle = pView->GetVehicle();
		DxVehicleData* pVehicleData = pView->GetVehicleData ();

		strSkeleton = pVehicleData->GetSkinAniControlData()->GetSkeletonName ();
	}

	if ( strSkeleton.IsEmpty() )
	{
		MessageBox ( "본 파일이 지정되지 않았습니다.\r\n 먼저 지정하신후 '조각'을 삽입하세요." );
		return;
	}

	CString szFilter;
	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{	
		szFilter = "Charactor Piece File (*.cps)|*.cps|Attach Bone Link File (*.abl)|*.abl|";
	}else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{	
		szFilter = "Attach Bone Piece File (*.aps)|*.aps|";
	}else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{	
		szFilter = "Vehicle Parts File (*.vps)|*.vps|Attach Bone Link File (*.abl)|*.abl|";
	}
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFECFileDialog dlg ( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		POSITION pos = dlg.GetStartPosition();
		while (pos != NULL)
		{
			CString sFileName = dlg.GetNextPathName(pos);
			int nPosition = sFileName.ReverseFind('\\');
			sFileName = sFileName.Mid(nPosition+1);

			if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
			{	
				InsertSkinPiece( sFileName.GetString() );
			}else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
			{	
				InsertAttBone( sFileName.GetString() );
			}else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
			{	
				InsertVehiclePart( sFileName.GetString() );
			}
		}
	}
}
#else
void CFramePage::OnBnClickedButtonAniEdit()
{
    CsheetWithTabFrame* pParent = static_cast<CsheetWithTabFrame*>(GetParent());

    if (pParent)
    {
        CPieceContainerPage* pGrandParent = static_cast<CPieceContainerPage*>(pParent->GetParent());

        if (pGrandParent)
        {
            CsheetWithTabPiece* pSheetPiece = pGrandParent->GetPieceTab();
            CsheetWithTabAnim* pSheetAnim = pGrandParent->GetAnimationTab();

            if (pSheetPiece && pSheetAnim)
            {
                pSheetPiece->ShowWindow(SW_HIDE);
                pSheetAnim->ShowWindow(SW_SHOW);
            }
        }
    }
}
#endif

void CFramePage::OnBnClickedButtonPieceDeselect()
{
	DWORD dwPieceType = NULL;
	if ( m_PieceList.GetSelectedCount() )
	{
		POSITION pos = m_PieceList.GetFirstSelectedItemPosition();
		int nIndex = m_PieceList.GetNextSelectedItem(pos);
		if ( nIndex==LB_ERR )	return;

		dwPieceType = m_PieceList.GetItemData ( nIndex );
	}

	int nRet = MessageBox ( "PIECE 를 정말로 삭제 하시겠습니까?", "주의", MB_ICONEXCLAMATION | MB_YESNO );
	if ( IDYES != nRet )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{	
		DxSkinChar *pSkinChar = pView->GetSkinChar();
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();

		pSkinCharData->ResetPieceEDIT( pd3dDevice, dwPieceType );

		pSkinChar->SetCharData( pSkinCharData, pd3dDevice, FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
		SetPieceList ();
	}else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{
		DxAttBone *pAttBoneChar = pView->GetAttBone();
		DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();

		pAttBoneData->ResetAttBonePiece ( pd3dDevice, dwPieceType );

		pAttBoneChar->SetAttBoneData ( pAttBoneData, pd3dDevice, false, 0, EMSCD_ALL_THREAD, NULL );
		SetAttBoneList ();
	}else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{
		DxVehicle *pVehicle = pView->GetVehicle();
		DxVehicleData* pVehicleData = pView->GetVehicleData ();

		//pVehicleData->ResetVehiclePart ( pd3dDevice, dwPieceType );
		pVehicleData->ResetPieceEDIT ( dwPieceType );

		pVehicle->SetVehiclePartData ( pVehicleData, pd3dDevice );
		SetVehiclePartList ();
	}
}

void CFramePage::ResetPage ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();
	

	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{	
		DxSkinChar *pSkinChar = pView->GetSkinChar();
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData();

		const char *szSkeleton = pSkinCharData->GetSkeletonName ();
		CWnd *pWnd = GetDlgItem ( IDC_EDIT_SKINBONE );

		if ( !szSkeleton )	pWnd->SetWindowText ( "" );
		else				pWnd->SetWindowText ( szSkeleton );

		SetWin_Num_float ( this, IDC_EDIT_SKIN_SCALE, pSkinCharData->m_fScale );

		SetWin_Num_float ( this, IDC_EDIT_SET_HEIGHT, pSkinCharData->m_fHeightYPos );

		SetPieceList ();
	}else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{
		DxAttBone *pAttBoneChar = pView->GetAttBone();
		DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();

		const char *szSkeleton = pAttBoneData->GetSkinAniControlData()->GetSkeletonName ();
		CWnd *pWnd = GetDlgItem ( IDC_EDIT_SKINBONE );

		if ( !szSkeleton )	pWnd->SetWindowText ( "" );
		else				pWnd->SetWindowText ( szSkeleton );

		SetWin_Num_float ( this, IDC_EDIT_SKIN_SCALE, pAttBoneData->m_fScale );

		SetAttBoneList ();
	}else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{
		DxVehicle *pVehicle = pView->GetVehicle();
		DxVehicleData* pVehicleData = pView->GetVehicleData ();

		const char *szSkeleton = pVehicleData->GetSkinAniControlData()->GetSkeletonName ();
		CWnd *pWnd = GetDlgItem ( IDC_EDIT_SKINBONE );

		if ( !szSkeleton )	pWnd->SetWindowText ( "" );
		else				pWnd->SetWindowText ( szSkeleton );

		SetWin_Num_float ( this, IDC_EDIT_SKIN_SCALE, pVehicleData->m_fScale );

		SetVehiclePartList ();
	}
}

void CFramePage::OnBnClickedButtonEffSave()
{
	//	Note : 파일 필터를 정의한다.
	CString szFilter = "Skin effect |*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg ( FALSE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath();

	if ( dlg.DoModal()==IDOK )
	{
		CString sFileName = dlg.GetFileName();
		sFileName.MakeLower ();
		const char *szFileName = sFileName.GetString();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();

		DxEffCharDataArray EffCharDataArray;
		EffCharDataArray.Import ( pSkinCharData, pView->GetD3dDevice() );
		EffCharDataArray.SaveFile ( szFileName );
	}
}

BOOL CFramePage::InsertSkinEff( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinChar *pSkinChar = pView->GetSkinChar();
	DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();

	DxEffCharDataArray EffCharDataArray;
	//EffCharDataArray.Import ( pSkinCharData );
	BOOL bOK = EffCharDataArray.LoadFile ( pszFileName );
	if ( !bOK )
	{
		MessageBox ( "DxEffCharDataArray 효과 파일 읽기에 실패하였습니다.", pszFileName, MB_OK );
		return FALSE;
	}

	EffCharDataArray.SetSkinCharData ( pSkinCharData, pView->GetD3dDevice() );
	pSkinChar->SetCharData ( pSkinCharData, pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	return TRUE;
}

void CFramePage::OnBnClickedButtonEffLoad()
{
	//	Note : 파일 필터를 정의한다.
	CString szFilter = "Skin effect |*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg ( TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath();

	if ( dlg.DoModal()==IDOK )
	{
		CString sFileName = dlg.GetFileName();
		sFileName.MakeLower ();
		const char *szFileName = sFileName.GetString();

		InsertSkinEff( szFileName );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
		DlgEdit.GetSheetChar()->m_CharPage.GetAnimationTab()->AniEffName( FALSE );
	}
}

void CFramePage::OnEnChangeEditSkinScale()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{	
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();
		pSkinCharData->m_fScale = GetWin_Num_float ( this, IDC_EDIT_SKIN_SCALE );
	}else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{
		DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();
		pAttBoneData->m_fScale = GetWin_Num_float ( this, IDC_EDIT_SKIN_SCALE );
	}else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{
		DxVehicleData* pVehicleData = pView->GetVehicleData ();
		pVehicleData->m_fScale = GetWin_Num_float ( this, IDC_EDIT_SKIN_SCALE );
	}	
}

void CFramePage::OnBnClickedButtonScale()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{
		DxSkinChar *pSkinChar = pView->GetSkinChar();
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();

		pSkinChar->SetCharData ( pSkinCharData, pView->GetD3dDevice(), FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	}
	else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{
		DxAttBone *pAttBoneChar = pView->GetAttBone();
		DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();

		pAttBoneChar->SetAttBoneData ( pAttBoneData, pView->GetD3dDevice(), FALSE, 0, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE, NULL );
	}
	else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{
		DxVehicle *pVehicle= pView->GetVehicle();
		DxVehicleData* pVehicleData = pView->GetVehicleData ();

		pVehicle->SetVehiclePartData ( pVehicleData, pView->GetD3dDevice(), FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	}
}


void CFramePage::OnBnClickedButtonSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
	DlgEdit.GetSheetChar()->m_CharPage.SetBoneEditTab(TRUE);
}

void CFramePage::OnBnClickedButtonEffName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;

	DlgEdit.GetSheetChar()->m_CharPage.GetAnimationTab()->AniEffName();
}

void CFramePage::OnBnClickedButtonSetHeight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();
	pSkinCharData->m_fHeightYPos = GetWin_Num_float ( this, IDC_EDIT_SET_HEIGHT );
}

void CFramePage::OnBnClickedButtonEffBoardpos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
	DlgEdit.GetSheetChar()->m_VehiclePage.SetBoardEditTab(TRUE);
}

void CFramePage::OnBnClickedButtonSkinSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_SKIN_INFO );
}

void CFramePage::OnBnClickedButtonBoneSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	const DxSkeleton* pSkeleton(NULL);
	switch( m_emFrameEditType )
	{
	case EMFRAMEEDIT_CHAR:
		{
			DxSkinChar *pSkinChar = pView->GetSkinChar();
			pSkeleton = pSkinChar->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	case EMFRAMEEDIT_ATTBONE:
		{
			DxAttBone *pAttBone = pView->GetAttBone();
			pSkeleton = pAttBone->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	case EMFRAMEEDIT_VEHICLE:
		{
			DxVehicle *pVehicle = pView->GetVehicle();
			pSkeleton = pVehicle->m_spSkinAniControlThread->GetSkeletonForEdit();
		}
		break;
	default:
		break;
	}

	if ( !pSkeleton )
	{
		AfxMessageBox( _T("Bone 이 없어서 작업이 불가능 합니다.") );
		return;
	}


	//////////////////////////////////////////////////////////////////////////
	// 정상 작동
	if( m_emFrameEditType == EMFRAMEEDIT_CHAR )
	{
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_BONE_SETTING );
	}
	else if( m_emFrameEditType == EMFRAMEEDIT_ATTBONE )
	{
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_BONE_SETTING );
	}
	else if( m_emFrameEditType == EMFRAMEEDIT_VEHICLE )
	{
		pFrame->m_cDlgEdit.SetAcitveSheetPage ( CHARTAB_BONE_SETTING );
	}
}

void CFramePage::OnNMClickListPiece(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return;
    }

#ifdef DEF_SKIN_SYNC
    UINT nSelectCount = m_PieceList.GetSelectedCount();

    int nItem = -1;

    if (nSelectCount > 0)
    {
        for (UINT i = 0; i < nSelectCount; ++i)
        {
            nItem = m_PieceList.GetNextItem(nItem, LVNI_SELECTED);
            
            if (nItem != -1)
            {
                break;
            }
        }
    }

    CsheetWithTabFrame* pParent = static_cast<CsheetWithTabFrame*>(GetParent());

    if (pParent && nItem != -1)
    {
        CPieceContainerPage* pGrandParent = static_cast<CPieceContainerPage*>(pParent->GetParent());

        if (pGrandParent)
        {
            pGrandParent->SetSkinPiece(nItem);

            CsheetWithTabPiece* pSheetPiece = pGrandParent->GetPieceTab();
            CsheetWithTabAnim* pSheetAnim = pGrandParent->GetAnimationTab();

            if (pSheetPiece && pSheetAnim)
            {
                pSheetPiece->SetActivePage(CsheetWithTabPiece::PIECEPAGE);

                m_PieceList.SetFocus();

                pSheetAnim->ShowWindow(SW_HIDE);
                pSheetPiece->ShowWindow(SW_SHOW);
            }
        }
    }    
#endif
}

void CFramePage::OnNMDBLCLKListPiece(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	// CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if ( pFrame->GetActiveView() == NULL )
		return;

	int nSelectCnt = static_cast<int>( m_PieceList.GetSelectedCount() );
	int nItem = -1, i = 0;
	if ( nSelectCnt > 0 )
	{
		for ( ; i < nSelectCnt; ++i )
		{
			nItem = m_PieceList.GetNextItem(nItem, LVNI_SELECTED);
			
			if (nItem != -1)	// 리스트에서 선택된 것이 있다면;
				break;
		}
	}

	CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
	if ( DlgEdit && nItem != -1 )
	{
		char cBuffer[100];
		// itemIdx, ColumnIdx, string, string, length;
		const int nLen = m_PieceList.GetItemText( nItem, 1, cBuffer, 100 );

		CString szFileExt = sc::string::getFileExt(cBuffer);
		if ( szFileExt == "cps" )
		{
			DlgEdit.SetAcitveSheetPage( CHARTAB_CHARPIECE );
			CSkinPiecePage* pSkinPiecePage = &(DlgEdit.GetSheetChar()->m_SkinPiecePage);
			pSkinPiecePage->LoadSkinPiece(cBuffer);
		}
		else if ( szFileExt == "abl" )
		{
			DlgEdit.SetAcitveSheetPage( CHARTAB_ATTACHLINK );
			CAttachLinkPage* pAttachLinkPage = &(DlgEdit.GetSheetChar()->m_AttachLinkPage);
			pAttachLinkPage->LoadAblFile( cBuffer );
		}
		
		
		// CsheetWithTabFrame* pParent = static_cast<CsheetWithTabFrame*>(GetParent());
		// CPieceContainerPage* pGrandParent = static_cast<CPieceContainerPage*>(pParent->GetParent());
	}
}


void CFramePage::OnBnClickedButtonFrameDocument()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TSTRING strText = 
		"[Skin] - 얼굴과 몸의 Specular 컬러와 Power 값을 줄 수 있다. Fake SSS 도 여기에서 셋팅한다. \r\n"
		"\r\n"
		"[Setting Bone Information] - Bone 과 연관되어 기능을 할 수 것 들을 셋팅 할 수 있다. \r\n"
		;

	AfxMessageBox( strText.c_str() );
}

void CFramePage::OnBnClickedButtonWeaponScale2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgEdit.SetAcitveSheetPage( CHARTAB_WEAPON_SCALE );
}

void CFramePage::OnBnClickedButtonBillboard()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	pFrame->m_cDlgEdit.SetAcitveSheetPage( CHARTAB_BILLBOARD );
}
