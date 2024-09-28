// SkinPiecePage.cpp : This is the implementation file.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "SheetWithTabChar.h"

#include "SheetWithTabPiece.h"
#include "EffCharPage.h"
#include "UserBackSlotPage.h"
#include "MaterialPage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../EngineLib/DxEffect/Char/DxEffChar.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../MfcExLib/ExLibs/FECFileDialog.h"

#include "../EngineLib/DxEffect/Char/dxeffchardata.h"

#include "SkinPiecePage.h"
#include "skinpiecepage.h"
#include "..\enginelib\DxMeshs\SkinMesh\DxSkinPieceContainer.h"

#include "../State/CharStateMachine.h"

// CSkinPiecePage This is a dialog box.

IMPLEMENT_DYNAMIC(CSkinPiecePage, CPropertyPage)
CSkinPiecePage::CSkinPiecePage()
: CPieceRelativePage(CSkinPiecePage::IDD)
    // : CPropertyPage(CSkinPiecePage::IDD)
{
}

CSkinPiecePage::~CSkinPiecePage()
{
	
}

void CSkinPiecePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SkINMESH, m_comboMeshs);
	DDX_Control(pDX, IDC_STATIC_SETLINE, m_stSetLine);
	DDX_Control(pDX, IDC_COMBO_SKINMESHTYPE, m_comboSkinMeshType);
	DDX_Control(pDX, IDC_LIST_VERTICS, m_listVertics);
	DDX_Control(pDX, IDC_COMBO_EFF_TYPE, m_EffType);
	DDX_Control(pDX, IDC_LIST_EFFLIST, m_EffList);
	DDX_Control(pDX, IDC_COMBO_WHEREBACK, m_comboWhereBack);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_combo_Type);
}


BEGIN_MESSAGE_MAP(CSkinPiecePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SKINFILE, OnBnClickedButtonSkinfile)
	ON_BN_CLICKED(IDC_BUTTON_BONEFILE, OnBnClickedButtonBonefile)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_ALLSAVE2, 	OnBnClickedButtonAllSave)
	ON_BN_CLICKED(IDC_BUTTON_MAKEOBJ, OnBnClickedButtonMakeobj)
	ON_BN_CLICKED(IDC_BUTTON_SKINMESHSELECT, OnBnClickedButtonSkinmeshselect)
	ON_CBN_SELCHANGE(IDC_COMBO_SKINMESHTYPE, OnCbnSelchangeComboSkinmeshtype)
	ON_BN_CLICKED(IDC_BUTTON_EFFINSERT, OnBnClickedButtonEffinsert)
	ON_BN_CLICKED(IDC_BUTTON_EFFSTATE, OnBnClickedButtonEffstate)
	ON_BN_CLICKED(IDC_BUTTON_EFFDEL, OnBnClickedButtonEffdel)
	ON_BN_CLICKED(IDC_BUTTON_VERTINSERT, OnBnClickedButtonVertinsert)
	ON_BN_CLICKED(IDC_BUTTON_VERTDEL, OnBnClickedButtonVertdel)
	ON_BN_CLICKED(IDC_CHECK_VERT_DRAW, OnBnClickedCheckVertDraw)
	ON_BN_CLICKED(IDC_BUTTON_EFF_SAVE, OnBnClickedButtonEffSave)
	ON_BN_CLICKED(IDC_BUTTON_EFF_LOAD, OnBnClickedButtonEffLoad)
	ON_BN_CLICKED(IDC_BUTTON_MATERIAL, OnBnClickedButtonMaterial)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VERTICS, OnLvnItemchangedListVertics)
	ON_CBN_SELCHANGE(IDC_COMBO_WHEREBACK, OnCbnSelchangeComboWhereback)
	ON_LBN_DBLCLK(IDC_LIST_EFFLIST, OnLbnDblclkListEfflist)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_BUTTON_SLOT_WEAPON, &CSkinPiecePage::OnBnClickedButtonSlotWeapon)
	ON_BN_CLICKED(IDC_BUTTON_DOCUMENT, &CSkinPiecePage::OnBnClickedButtonDocument)
	ON_BN_CLICKED(IDC_RADIO_HAIR_NONE, OnBnClickedButtonHairNone)
	ON_BN_CLICKED(IDC_RADIO_HAIR_BASE, OnBnClickedButtonHairBase)
	ON_BN_CLICKED(IDC_RADIO_HAIR_NEW, OnBnClickedButtonHairNew)
	ON_BN_CLICKED(IDC_BUTTON_ADD_HAIR_CPS_LOAD, OnBnClickedButtonAddHairCpsLoad)
END_MESSAGE_MAP()


// CSkinPiecePage It's a message handler 메시지 처리기입니다.

BOOL CSkinPiecePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add additional initialization actions here 여기에 추가 초기화 작업을 추가합니다.

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );
	pWnd->SetWindowText ( "" );

	pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
	pWnd->SetWindowText ( "" );

	pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
	pWnd->SetWindowText ( "untitle.cps" );

	ShowMeshSetContrl(FALSE);


	int nIndex = 0;
	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		nIndex = m_comboSkinMeshType.AddString ( DxSkinPiece::m_szPIECETYPE[i] );
		m_comboSkinMeshType.SetItemData ( nIndex, i );
	}
	
	for ( int i=0; i<EMPEACE_WEAPON_SIZE; ++i )
	{
		nIndex = m_comboWhereBack.AddString ( SKINTYPE_STING::m_szWHEREBACKTYPE[i] );
		m_comboWhereBack.SetItemData ( nIndex, i );
	}

	for ( int i=0; i<PIECETYPE_SIZE; ++i )
	{
		nIndex = m_combo_Type.AddString ( SKINTYPE_STING::m_szPIECETYPE[i] );
		m_combo_Type.SetItemData ( nIndex, i );
	}
	m_combo_Type.SetCurSel(0);


	RECT listRect;
	m_listVertics.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = { int(nListWidth*0.25f), int(nListWidth*0.65f) };

	m_listVertics.SetExtendedStyle ( m_listVertics.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_listVertics.InsertColumn ( 0, _T("종류"),	LVCFMT_CENTER, nColWidth[0] );
	m_listVertics.InsertColumn ( 1, _T("위치"),	LVCFMT_CENTER, nColWidth[1] );


	DxEffCharType* pEffCharType = DxEffCharMan::GetInstance().GetEffectList();
	while ( pEffCharType )
	{
		int nIndex = m_EffType.AddString ( pEffCharType->NAME );
		m_EffType.SetItemData ( nIndex, pEffCharType->TYPEID );
		pEffCharType = pEffCharType->pNext;
	}

	VertListting ();
	EffectListting ();

	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_VERT_DRAW );
	pButton->SetCheck ( CCharEditView::m_bSkinTraceVertDraw );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX Property page must return FALSE 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSkinPiecePage::PostNcDestroy()
{
	// TODO: Add specialized code here and/or call the base class 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CPropertyPage::PostNcDestroy();
}

void CSkinPiecePage::ShowMeshSetContrl ( BOOL bOk )
{
	int nContrlIndex[] =
	{
		IDC_STATIC_MTYPE,				//	0
		IDC_COMBO_SKINMESHTYPE,			//	1
		IDC_STATIC_WHEREBACK,			//	2
		IDC_COMBO_WHEREBACK,			//	3
		IDC_BUTTON_EFFINSERT,			//	4
		IDC_BUTTON_EFFSTATE,			//	5
		IDC_BUTTON_EFFDEL,				//	6
		IDC_LIST_EFFLIST,				//	7
		IDC_STATIC_VERTICS,				//	8
		IDC_LIST_VERTICS,				//	9
		IDC_BUTTON_VERTINSERT,			//	10
		IDC_BUTTON_VERTDEL,				//	11
		IDC_CHECK_VERT_DRAW,			//	12
		IDC_COMBO_EFF_TYPE,				//	13
		IDC_BUTTON_EFF_SAVE,			//	14
		IDC_BUTTON_EFF_LOAD,			//	15
		IDC_BUTTON_SLOT_WEAPON,
		IDC_BUTTON_DOCUMENT,
		IDC_BUTTON_MATERIAL,
		IDC_STATIC_HAIR,
		IDC_RADIO_HAIR_NONE,
		IDC_RADIO_HAIR_BASE,
		IDC_RADIO_HAIR_NEW,
		IDC_EDIT_ADD_HAIR_CPS,
		IDC_BUTTON_ADD_HAIR_CPS_LOAD
	};
	int nSizeIndex = sizeof(nContrlIndex) / sizeof(int);

	CWnd *pWnd;
	for ( int i=0; i<nSizeIndex; ++i )
	{
		pWnd = GetDlgItem ( nContrlIndex[i] );
		pWnd->ShowWindow ( bOk );
	}
}

void CSkinPiecePage::OnBnClickedButtonNew()
{
    InitPage();

#ifndef DEF_SKIN_SYNC
    CharStateMachine::ClearFileNotifier();
#endif
}

void CSkinPiecePage::InitPage(bool bResetPiece)
{
    if( m_hWnd == NULL ) return;

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    DxSkinPiece *pSkinPiece = pView->GetSkinPiece();

#ifndef DEF_SKIN_SYNC
    pView->GetSkinPiece()->ClearAll ();
#endif

    CWnd *pWnd;
    pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );
    pWnd->SetWindowText ( "" );

    pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
    pWnd->SetWindowText ( "" );

    m_comboSkinMeshType.ResetContent();
    int nIndex = 0;

#ifdef DEF_SKIN_SYNC
    if (pSkinPiece)
    {
        m_combo_Type.SetCurSel(pSkinPiece->m_emPieceType);
    }
#endif

    if( m_combo_Type.GetCurSel() == PIECETYPE_CHARACTER )
    {
        m_comboWhereBack.EnableWindow( TRUE );
        SetWin_Text( this, IDC_EDIT_CHARFILE, "untitle.cps" );	

        for ( int i=0; i<PIECE_SIZE; ++i )
        {
            nIndex = m_comboSkinMeshType.AddString ( DxSkinPiece::m_szPIECETYPE[i] );
            m_comboSkinMeshType.SetItemData ( nIndex, i );
        }
#ifndef DEF_SKIN_SYNC
        pSkinPiece->m_emPieceType = PIECETYPE_CHARACTER;
#endif
    }else if( m_combo_Type.GetCurSel() == PIECETYPE_ATTACHBONE )
    {
        m_comboWhereBack.EnableWindow( FALSE );
        SetWin_Text( this, IDC_EDIT_CHARFILE, "untitle.aps" );

        for ( int i=0; i<ATTBONE_SIZE; ++i )
        {
            nIndex = m_comboSkinMeshType.AddString ( DxSkinPiece::m_szATTBONETYPE[i] );
            m_comboSkinMeshType.SetItemData ( nIndex, i );
        }
#ifndef DEF_SKIN_SYNC
        pSkinPiece->m_emPieceType = PIECETYPE_ATTACHBONE;
#endif
    }
    else if( m_combo_Type.GetCurSel() == PIECETYPE_V_PARTS )
    {
        m_comboWhereBack.EnableWindow( FALSE );
        SetWin_Text( this, IDC_EDIT_CHARFILE, "untitle.vps" );

        for ( int i=0; i<PART_V_SIZE; ++i )
        {
            nIndex = m_comboSkinMeshType.AddString ( DxSkinPiece::m_szV_PARTSTYPE[i] );
            m_comboSkinMeshType.SetItemData ( nIndex, i );
        }
#ifndef DEF_SKIN_SYNC
        pSkinPiece->m_emPieceType = PIECETYPE_V_PARTS;
#endif
    }

#ifdef DEF_SKIN_SYNC
    if (bResetPiece)
    {
        CsheetWithTabPiece* pParentSheet = GetParent();

        if (pParentSheet)
        {
            pParentSheet->ResetPiece();
        }
    }
#endif

    m_comboSkinMeshType.SetCurSel(0);
    m_comboMeshs.ResetContent();    

    ShowMeshSetContrl(FALSE);
    VertListting();
    EffectListting();
}

void CSkinPiecePage::InitSkinPiece()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

    if (pSkinPiece == 0 || pSkinPiece->m_szFileName == 0)
    {
        InitPage(false);
        return;
    }

    SetFileName();
    MeshListting();

    ShowMeshSetContrl(TRUE);
    SetMeshSetContrl ();
    SetComboWeaponWhereBack();
    VertListting ();
    EffectListting ();    
}

HRESULT CSkinPiecePage::LoadSkinPiece( const char *pszFileName )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

#ifndef DEF_SKIN_SYNC
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
#else
    DxSkinPiece *pSkinPiece = new DxSkinPiece;
#endif

	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();	

#ifndef DEF_SKIN_SYNC
	OnBnClickedButtonNew();
#endif
 
	//	Note : Save skin fragment.
	//
	HRESULT hr = pSkinPiece->LoadPiece( pszFileName, pd3dDevice, FALSE, FALSE );

#ifndef DEF_SKIN_SYNC
    pSkinPiece->UpdateLOD(D3DXVECTOR3(0, 0, 0), pd3dDevice);
    if ( FAILED( hr ) ) return E_FAIL;
#else
	if ( FAILED( hr ) )
    {
        SAFE_DELETE(pSkinPiece);
        return E_FAIL;
    }
#endif

#ifdef DEF_SKIN_SYNC
    if (CheckBone(pSkinPiece) == false)
    {
        SAFE_DELETE(pSkinPiece);
        return E_FAIL;
    }

    if (CheckParts(pSkinPiece) == false)
    {
        SAFE_DELETE(pSkinPiece);
        return E_FAIL;
    }

    if (SetSkinPiece(pSkinPiece) == false)
    {
        SAFE_DELETE(pSkinPiece);
        return E_FAIL;
    }

    pSkinPiece = pView->GetSkinPiece();

    if (pSkinPiece->m_szFileName == 0 || pSkinPiece->m_szMeshName == 0)
    {
        return S_OK;
    }
#endif

    SetFileName();
    MeshListting();

	ShowMeshSetContrl(TRUE);
	SetMeshSetContrl ();
	SetComboWeaponWhereBack();
	VertListting ();
	EffectListting ();

#ifndef DEF_SKIN_SYNC
	CharStateMachine::InsertFileNotifier();
#endif

	return hr;
}

void CSkinPiecePage::OnBnClickedButtonOpen()
{
	HRESULT hr;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();


	CString szFilter;
	if( m_combo_Type.GetCurSel() == PIECETYPE_CHARACTER )
	{
		szFilter = "Skin Piece File (*.cps)|*.cps|";
	}else if( m_combo_Type.GetCurSel() == PIECETYPE_ATTACHBONE )
	{
		szFilter = "Attach Bone Piece File (*.aps)|*.aps|";
	}else if( m_combo_Type.GetCurSel() == PIECETYPE_V_PARTS )
	{
		szFilter = "Vehicle Parts File (*.vps)|*.vps|";
	}
	
	//	Note : Create a file open dialog 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	CString sFileName;
	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		sFileName = dlg.GetFileName ();
		const char *szFileName = sFileName.GetString ();


		//	Note : Save skin fragment 스킨 조각 세이브.
		//
		hr = LoadSkinPiece( szFileName );
		if ( FAILED(hr) )
		{
			sFileName.ReleaseBuffer ();
			return;
		}

		sFileName.ReleaseBuffer ();
	}
	else	return;

	
}

void CSkinPiecePage::OnBnClickedButtonSave()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	CString StrFileName;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );
	pWnd->GetWindowText ( StrFileName );

	CString szFilter;
	CString szExt;
	if( m_combo_Type.GetCurSel() == PIECETYPE_CHARACTER )
	{
		szFilter = "Skin Piece File (*.cps)|*.cps|";
		szExt    = ".cps";
	}else if( m_combo_Type.GetCurSel() == PIECETYPE_ATTACHBONE )
	{
		szFilter = "Skin Piece File (*.aps)|*.aps|";
		szExt    = ".aps";
	}else if( m_combo_Type.GetCurSel() == PIECETYPE_V_PARTS )
	{
		szFilter = "Vehicle Parts File (*.vps)|*.vps|";
		szExt    = ".vps";
	}
	
	//	Note : Create a file open dialog 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,szExt,StrFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		const char *szFileName = sFileName.GetString ();

		//	Note : Save skin fragment 스킨 조각 세이브.
		pSkinPiece->SavePiece ( szFileName );

        SetFileName();              

		sFileName.ReleaseBuffer ();

        SetSkinPiece(pSkinPiece);
	}
}


void CSkinPiecePage::OnBnClickedButtonAllSave()
{
	HRESULT hr;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	
	CString strFind = DxSkinObject::GetPath ();

	if( m_combo_Type.GetCurSel() == PIECETYPE_CHARACTER )
	{
		strFind += "*.cps";
	}else if( m_combo_Type.GetCurSel() == PIECETYPE_ATTACHBONE )
	{
		strFind += "*.aps";
	}else if( m_combo_Type.GetCurSel() == PIECETYPE_V_PARTS )
	{
		strFind += "*.vps";
	}
	

	WIN32_FIND_DATA pInfo;
	lstrcpy( pInfo.cFileName, strFind.GetString() );

	HANDLE hContext = FindFirstFile( strFind.GetString(), &pInfo );
	if( hContext == INVALID_HANDLE_VALUE )
	{
		FindClose( hContext );
		return;
	}

	do
	{
		pView->GetSkinPiece()->ClearAll ();

		hr = pSkinPiece->LoadPiece( pInfo.cFileName, pd3dDevice, FALSE, FALSE );
		if ( FAILED(hr) )
		{
			MessageBox( "File Load error",pInfo.cFileName,MB_OK);
		}
		else 
		{
			hr = pSkinPiece->SavePiece ( pInfo.cFileName );
			if ( FAILED(hr))	MessageBox( "File Save error",pInfo.cFileName,MB_OK);
		}
	}
	while( FindNextFile( hContext, &pInfo ) != 0 );

	pView->GetSkinPiece()->ClearAll ();

	FindClose( hContext );

	MessageBox( _T("All Save Success"), _T("Success"), MB_OK );

}

void CSkinPiecePage::SetEditFileName( const char *pszFileName, BOOL bBone )
{
	CWnd *pWnd;
	if( bBone )
	{
		pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
		pWnd->SetWindowText ( pszFileName );
	}else{
		pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );
		pWnd->SetWindowText ( pszFileName );
	}

	ShowMeshSetContrl(FALSE);
}

void CSkinPiecePage::OnBnClickedButtonSkinfile()
{
	CString szFilter = "Skin xFile (*.x,*.mxf)|*.X;*.mxf|";
	
	//	Note : Create a file open dialog.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinMeshManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		SetEditFileName( dlg.GetFileName().GetString(), FALSE );
	}
}

void CSkinPiecePage::OnBnClickedButtonBonefile()
{
	CString szFilter = "Bone xFile (*.x,*.mxf)|*.X;*.mxf|";
	
	//	Note : Create a file open dialog.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkeletonManager::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		SetEditFileName( dlg.GetFileName().GetString(), TRUE );
	}
}


void CSkinPiecePage::OnBnClickedButtonMakeobj()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

#ifndef DEF_SKIN_SYNC
    DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
#else
    DxSkinPiece *pSkinPiece = new DxSkinPiece;
#endif

	CString sSkinMesh, sSkinBone;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );
	pWnd->GetWindowText ( sSkinMesh );
	pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );
	pWnd->GetWindowText ( sSkinBone );

	if ( sSkinBone.IsEmpty() || sSkinMesh.IsEmpty () )
	{
		MessageBox ( "Skin Skeleton or Skin Mesh is empty" );
		return;
	}

	const char* szBone = sSkinBone.GetString();
	const char* szMesh = sSkinMesh.GetString();

	if ( !pView->GetD3dDevice() )	return;

	HRESULT hr;
	hr = pSkinPiece->SetSkinMesh( szMesh, szBone, pView->GetD3dDevice(), FALSE, FALSE );

#ifndef DEF_SKIN_SYNC
	if ( FAILED(hr) )	MessageBox ( "스킨, Failed to create bone." );
#else
    if ( FAILED(hr) )
    {
        SAFE_DELETE(pSkinPiece);
        MessageBox ( "스킨, Failed to create bone." );
        return ;
    }
#endif

#ifdef DEF_SKIN_SYNC
    if (CheckBone(pSkinPiece) == false)
    {
        SAFE_DELETE(pSkinPiece);
        return ;
    }

    CPieceContainerPage* pGrandParent = GetGrandParent();

    if (pGrandParent)
    {
        pGrandParent->SetSkinPieceType(pSkinPiece);
    }

    if (SetSkinPiece(pSkinPiece) == false)
    {
        SAFE_DELETE(pSkinPiece);
        return ;
    }
#endif

	sSkinBone.ReleaseBuffer ();
	sSkinMesh.ReleaseBuffer ();

    MeshListting();

	ShowMeshSetContrl(FALSE);
}

void CSkinPiecePage::OnBnClickedButtonSkinmeshselect()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	int nIndex = m_comboMeshs.GetCurSel ();
	if ( LB_ERR==nIndex )	return;

#ifndef DEF_SKIN_SYNC
	DWORD dwData = m_comboMeshs.GetItemData ( nIndex );
	pSkinPiece->SelectMesh ( pView->GetD3dDevice(), (PSMESHCONTAINER) dwData );
#else
    SMeshContainer* pMeshContainCur = 0;

    if (pSkinPiece->m_skinMeshRes.IsValid())
    {
        pMeshContainCur = pSkinPiece->m_skinMeshRes.get()->GetMeshClobalList();
    }
    else
    {
        return ;
    }

    int nTemp = 0;

    while (pMeshContainCur)
    {
        if (nTemp == nIndex)
        {
            pSkinPiece->SelectMesh(pView->GetD3dDevice(), pMeshContainCur);
            break;
        }

        ++nTemp;
        pMeshContainCur = pMeshContainCur->pGlobalNext;
    }

    if (pMeshContainCur == 0)
    {
        return ;
    }

    if (SetSkinPiece(pSkinPiece) == false)
    {
        return ;
    }
#endif

#ifndef DEF_SKIN_SYNC
	CharStateMachine::ClearFileNotifier();
	CharStateMachine::InsertFileNotifier();
#endif

	ShowMeshSetContrl(TRUE);
	SetMeshSetContrl ();
	SetComboWeaponWhereBack();
}

void CSkinPiecePage::OnBnClickedButtonHairNone()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();

	pSkinPiece->SetHatHairOption( HAT_HAIR_NONE );

	HatHairOptionWnd();
}

void CSkinPiecePage::OnBnClickedButtonHairBase()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();

	pSkinPiece->SetHatHairOption( HAT_HAIR_BASE );

	HatHairOptionWnd();
}

void CSkinPiecePage::OnBnClickedButtonHairNew()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();

	pSkinPiece->SetHatHairOption( HAT_HAIR_NEW );

	HatHairOptionWnd();
}

void CSkinPiecePage::HatHairOptionWnd()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	int nContrlIndex_AddHair[] =
	{
		IDC_EDIT_ADD_HAIR_CPS,
		IDC_BUTTON_ADD_HAIR_CPS_LOAD
	};
	int nSizeIndex_AddHair = sizeof(nContrlIndex_AddHair) / sizeof(int);

	for ( int i=0; i<nSizeIndex_AddHair; ++i )
	{
		CWnd* pWnd = GetDlgItem( nContrlIndex_AddHair[i] );
		pWnd->ShowWindow( FALSE );
	}

	//  If you are using new hair, show the necessary controls.만약 새로운 헤어를 사용한다면 그쪽에 필요한 Control 들을 보여주도록 한다.
	if ( pSkinPiece->IsHatHairOption( HAT_HAIR_NEW ) )
	{
		CWnd* pWnd = GetDlgItem( IDC_EDIT_ADD_HAIR_CPS );
		pWnd->SetWindowText( pSkinPiece->GetAddHairCps().c_str() );

		for ( int i=0; i<nSizeIndex_AddHair; ++i )
		{
			pWnd = GetDlgItem( nContrlIndex_AddHair[i] );
			pWnd->ShowWindow( TRUE );
		}
	}
}

void CSkinPiecePage::OnBnClickedButtonAddHairCpsLoad()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece();

	CString szFilter;
	if( m_combo_Type.GetCurSel() == PIECETYPE_CHARACTER )
	{
		szFilter = "Skin Piece File (*.cps)|*.cps|";
	}
	else
	{
		return;
	}

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkinObjPage*)this);

	CString sFileName;
	dlg.m_ofn.lpstrInitialDir = DxSkinObject::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		sFileName = dlg.GetFileName ();
		const char *szFileName = sFileName.GetString ();

		DxSkinPiece sSkinPieceHair;
		HRESULT hr = sSkinPieceHair.LoadPiece( szFileName, pView->GetD3dDevice(), FALSE, FALSE );
		if ( hr != S_OK )
		{
			AfxMessageBox( "Loading 실패" );
			sFileName.ReleaseBuffer ();
			return;
		}

		if ( sSkinPieceHair.GetSlot() != PIECE_HAIR_4_HAT )
		{
			AfxMessageBox( "Because it is not the hair of the hat, setting is not possible." );
			sFileName.ReleaseBuffer ();
			return;
		}
		
		pSkinPiece->m_strAddHairCps = szFileName;

		CWnd* pWnd = (CWnd*)GetDlgItem( IDC_EDIT_ADD_HAIR_CPS );
		pWnd->SetWindowText( pSkinPiece->m_strAddHairCps.c_str() );

		sFileName.ReleaseBuffer ();
	}
}

void CSkinPiecePage::EnableWnd_HairRenderOff()
{
	int nContrlIndex[] =
	{
		IDC_STATIC_HAIR,
		IDC_RADIO_HAIR_NONE,
		IDC_RADIO_HAIR_BASE,
		IDC_RADIO_HAIR_NEW
	};
	int nSizeIndex = sizeof(nContrlIndex) / sizeof(int);

	for ( int i=0; i<nSizeIndex; ++i )
	{
		CWnd* pWnd = GetDlgItem( nContrlIndex[i] );
		pWnd->ShowWindow( FALSE );
	}

	int nContrlIndex_AddHair[] =
	{
		IDC_EDIT_ADD_HAIR_CPS,
		IDC_BUTTON_ADD_HAIR_CPS_LOAD
	};
	int nSizeIndex_AddHair = sizeof(nContrlIndex_AddHair) / sizeof(int);

	for ( int i=0; i<nSizeIndex_AddHair; ++i )
	{
		CWnd* pWnd = GetDlgItem( nContrlIndex_AddHair[i] );
		pWnd->ShowWindow( FALSE );
	}

	int nIndex = m_comboSkinMeshType.GetCurSel ();
	if ( nIndex == LB_ERR )	
		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CButton* pButtonHairNone = (CButton*)GetDlgItem( IDC_RADIO_HAIR_NONE );
	CButton* pButtonHairBase = (CButton*)GetDlgItem( IDC_RADIO_HAIR_BASE );
	CButton* pButtonHairNew = (CButton*)GetDlgItem( IDC_RADIO_HAIR_NEW );

	if ( pSkinPiece->IsHatHairOption( HAT_HAIR_NONE ) )
	{
		pButtonHairNone->SetCheck( TRUE );
		pButtonHairBase->SetCheck( FALSE );
		pButtonHairNew->SetCheck( FALSE );
	}
	else if ( pSkinPiece->IsHatHairOption( HAT_HAIR_BASE ) )
	{
		pButtonHairNone->SetCheck( FALSE );
		pButtonHairBase->SetCheck( TRUE );
		pButtonHairNew->SetCheck( FALSE );
	}
	else if ( pSkinPiece->IsHatHairOption( HAT_HAIR_NEW ) )
	{
		pButtonHairNone->SetCheck( FALSE );
		pButtonHairBase->SetCheck( FALSE );
		pButtonHairNew->SetCheck( TRUE );
	}

	DWORD dwData = m_comboSkinMeshType.GetItemData ( nIndex );

	if( m_combo_Type.GetCurSel() == PIECETYPE_CHARACTER )
	{
		// 헤드기어일 경우 헤어를 렌더링 할 지 안 할 지 셋팅 할 수 있다.
		if ( (EMPIECECHAR) dwData == PIECE_HEADGEAR )
		{
			for ( int i=0; i<nSizeIndex; ++i )
			{
				CWnd* pWnd = GetDlgItem( nContrlIndex[i] );
				pWnd->ShowWindow( TRUE );
			}

			// 만약 새로운 헤어를 사용한다면 그쪽에 필요한 Control 들을 보여주도록 한다.
			if ( pSkinPiece->IsHatHairOption( HAT_HAIR_NEW ) )
			{
				CWnd* pWnd = GetDlgItem( IDC_EDIT_ADD_HAIR_CPS );
				pWnd->SetWindowText( pSkinPiece->GetAddHairCps().c_str() );

				for ( int i=0; i<nSizeIndex_AddHair; ++i )
				{
					pWnd = GetDlgItem( nContrlIndex_AddHair[i] );
					pWnd->ShowWindow( TRUE );
				}
			}
		}
	}
}

void CSkinPiecePage::SetMeshSetContrl ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	int nIndex = 0;
	if( m_combo_Type.GetCurSel() == PIECETYPE_CHARACTER )
	{
		nIndex = m_comboSkinMeshType.FindString ( 0, DxSkinPiece::m_szPIECETYPE[pSkinPiece->m_emType] );
	}else if( m_combo_Type.GetCurSel() == PIECETYPE_ATTACHBONE )
	{
		nIndex = m_comboSkinMeshType.FindString ( 0, DxSkinPiece::m_szATTBONETYPE[pSkinPiece->m_emAttBoneType] );
	}else if( m_combo_Type.GetCurSel() == PIECETYPE_V_PARTS )
	{
		nIndex = m_comboSkinMeshType.FindString ( 0, DxSkinPiece::m_szV_PARTSTYPE[pSkinPiece->m_emV_PartType] );
	}
	
	m_comboSkinMeshType.SetCurSel ( nIndex );

	EnableWnd_HairRenderOff();
}

void CSkinPiecePage::SetComboWeaponWhereBack()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	int nIndex = m_comboWhereBack.FindString ( 0, SKINTYPE_STING::m_szWHEREBACKTYPE[pSkinPiece->m_emWeaponWhereBack] );
	m_comboWhereBack.SetCurSel ( nIndex );
}

TCHAR* CopyString(const TCHAR* pStr)
{
    TCHAR* pOut = 0;

    if (pStr)
    {
        int nSize = _tcsclen(pStr) + 1;

        if (nSize > 1)
        {
            pOut = new TCHAR[nSize];

            if (pOut)
            {
                _tcscpy(pOut, pStr);
                return pOut;
            }
        }
    }

    return pOut;
}

void CSkinPiecePage::OnCbnSelchangeComboSkinmeshtype()
{
#ifndef DEF_SKIN_SYNC
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

    SetPieceType(pSkinPiece);
#else
    int nIndex = m_comboSkinMeshType.GetCurSel ();
    if ( nIndex == LB_ERR )		return;

    DWORD dwData = m_comboSkinMeshType.GetItemData ( nIndex );

    CsheetWithTabPiece* pParentSheet = GetParent();

    if (pParentSheet)
    {
        pParentSheet->ReplaceSkinPiece(dwData);
    }
#endif
}

void CSkinPiecePage::OnCbnSelchangeComboWhereback()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	int nIndex = m_comboWhereBack.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = m_comboWhereBack.GetItemData ( nIndex );
	pSkinPiece->m_emWeaponWhereBack = (EMPEACEZONEWEAPON)dwData;
}

void CSkinPiecePage::VertListting ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	m_listVertics.DeleteAllItems ();

	int i = 0;
	int nIndex = 0;
	for ( i=0; i<MAX_TRACE; ++i )
	{
		nIndex = m_listVertics.InsertItem ( i, szTRACE_NAME[i], NULL );
		m_listVertics.SetItemData ( nIndex, i );
	}

    if (pSkinPiece == 0)
    {
        return;
    }

	SVERTEXINFLU *pVertInflu = NULL;
	for ( i=0; i<MAX_TRACE; ++i )
	{
		pVertInflu = pSkinPiece->GetTracePos ( szTRACE_NAME[i] );

		if ( !pVertInflu )		m_listVertics.SetItemText ( i, 1, "NONE" );
		else
		{
			CString Str;
			Str.Format ( "%d, %3.1f, %3.1f, %3.1f",
				pVertInflu->m_dwIndex, pVertInflu->m_vVector.x, pVertInflu->m_vVector.y, pVertInflu->m_vVector.z );

			m_listVertics.SetItemText ( i, 1, Str );
		}
	}
}

void CSkinPiecePage::OnBnClickedButtonVertinsert()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	POSITION pos = m_listVertics.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( "Please select a location to set.", "정보", MB_OK );
		return;
	}
	
	int nItem = m_listVertics.GetNextSelectedItem(pos);
	DWORD dwData = m_listVertics.GetItemData ( nItem );

	if ( CCharEditView::m_VertexInflu.m_dwIndex != COL_ERR )
	{
		pSkinPiece->SetTracePos ( szTRACE_NAME[dwData], CCharEditView::m_VertexInflu );
	}

	VertListting ();
}

void CSkinPiecePage::OnBnClickedButtonVertdel()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	POSITION pos = m_listVertics.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( "Please select release location.", "정보", MB_OK );
		return;
	}
	
	int nItem = m_listVertics.GetNextSelectedItem(pos);
	DWORD dwData = m_listVertics.GetItemData ( nItem );

	pSkinPiece->DelTracePos ( szTRACE_NAME[dwData] );

	VertListting ();
}

void CSkinPiecePage::OnBnClickedCheckVertDraw()
{
	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_VERT_DRAW );
	CCharEditView::m_bSkinTraceVertDraw = pButton->GetCheck ();
}

void CSkinPiecePage::EffectListting ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	m_EffList.ResetContent ();

    if (pSkinPiece == 0)
    {
        return;
    }

	DxEffChar* pEffCur = NULL;
	int nIndex = 0;

	GLEFFCHAR_VEC_ITER iter		= pSkinPiece->m_vecEFFECT.begin ();
	GLEFFCHAR_VEC_ITER iter_end = pSkinPiece->m_vecEFFECT.end ();
	for ( ; iter!=iter_end; )
	{
		GLEFFCHAR_VEC_ITER iter_cur = iter++;
		pEffCur = (*iter_cur);

		nIndex = m_EffList.AddString ( pEffCur->GetName() );
		m_EffList.SetItemData ( nIndex, (DWORD) pEffCur );
	}
}

void CSkinPiecePage::OnBnClickedButtonEffinsert()
{
	int nIndex = m_EffType.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = m_EffType.GetItemData ( nIndex );

#ifdef DEF_SKIN_SYNC
    CsheetWithTabPiece* pParentSheet = GetParent();

    if (pParentSheet)
    {
        pParentSheet->SetActivePage(dwData);

        CEffCharPage* pEffPage = static_cast<CEffCharPage*>(pParentSheet->GetActivePage());

        if (pEffPage)
        {
            pEffPage->SetProperty();
        }
    }
#else
	switch ( dwData )
	{
	case EMEFFCHAR_SINGLE:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_SINGLE );
		m_pSheetTabParent->m_EffSingle.SetProperty ();
		break;
	case EMEFFCHAR_BLUR:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_BLUR );
		m_pSheetTabParent->m_EffBlur.SetProperty ();
		break;
	case EMEFFCHAR_CLONEBLUR:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_CLONEBLUR );
		m_pSheetTabParent->m_EffCloneBlur.SetProperty ();
		break;
	case EMEFFCHAR_DUST:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_DUST );
		m_pSheetTabParent->m_EffDust.SetProperty ();
		break;
	case EMEFFCHAR_SHOCK:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_SHOCK );
		m_pSheetTabParent->m_EffShock.SetProperty ();
		break;
	case EMEFFCHAR_ATTRIBUTE:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_ATTRIBUTE );
		m_pSheetTabParent->m_EffAttribute.SetProperty ();
		break;
	case EMEFFCHAR_ARROW:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_ARROW );
		m_pSheetTabParent->m_EffArrow.SetProperty ();
		break;
	case EMEFFCHAR_SPECULAR:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_SPECULAR );
		m_pSheetTabParent->m_EffSpecular.SetProperty ();
		break;
	case EMEFFCHAR_SPECULAR2:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_SPECULAR2 );
		m_pSheetTabParent->m_EffSpecular2.SetProperty ();
		break;
	case EMEFFCHAR_EMIT:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_EMIT );
		m_pSheetTabParent->m_EffEmit.SetProperty ();
		break;
	case EMEFFCHAR_ALPHA:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_ALPHA );
		m_pSheetTabParent->m_EffAlpha.SetProperty ();
		break;
	case EMEFFCHAR_NEON:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_NEON );
		m_pSheetTabParent->m_EffNeon.SetProperty ();
		break;
	case EMEFFCHAR_MARK:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_MARK );
		m_pSheetTabParent->m_EffMark.SetProperty ();
		break;
	case EMEFFCHAR_NOALPHA:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_NOALPHA );
		m_pSheetTabParent->m_EffNoAlpha.SetProperty ();
		break;
	case EMEFFCHAR_REFLECTION2:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_REFLECTION2 );
		m_pSheetTabParent->m_EffReflection2.SetProperty ();
		break;
	case EMEFFCHAR_AMBIENT:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_AMBIENT );
		m_pSheetTabParent->m_EffAmbient.SetProperty ();
		break;
	case EMEFFCHAR_DOT3:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_DOT3 );
		m_pSheetTabParent->m_EffDot3.SetProperty ();
		break;
	case EMEFFCHAR_MULTITEX:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_MULTITEX );
		m_pSheetTabParent->m_EffMultiTex.SetProperty ();
		break;
	case EMEFFCHAR_GHOSTING:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_GHOSTING );
		m_pSheetTabParent->m_EffGhosting.SetProperty ();
		break;
	case EMEFFCHAR_LEVEL:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_LEVEL );
		m_pSheetTabParent->m_EffLevel.SetProperty ();
		break;
	case EMEFFCHAR_TOON:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_TOON );
		m_pSheetTabParent->m_EffToon.SetProperty ();
		break;
	case EMEFFCHAR_TEXDIFF:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_TEXDIFF );
		m_pSheetTabParent->m_EffTexDiff.SetProperty ();
		break;
	case EMEFFCHAR_PARTICLE:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_PARTICLE );
		m_pSheetTabParent->m_EffParticle.SetProperty ();
		break;
	case EMEFFCHAR_BONEPOSEFF:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_BONEPOSEFF );
		m_pSheetTabParent->m_EffBonePosEff.SetProperty ();
		break;
	case EMEFFCHAR_BONELISTEFF:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_BONELISTEFF );
		m_pSheetTabParent->m_EffBoneListEff.SetProperty ();
		break;
	case EMEFFCHAR_USERCOLOR:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_USERCOLOR );
		m_pSheetTabParent->m_EffUserColor.SetProperty ();
		break;
	case EMEFFCHAR_NORMALMAP:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_NORMALMAP );
		m_pSheetTabParent->m_EffNormalMap.SetProperty ();
		break;
	case EMEFFCHAR_LINE2BONE:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_LINE2BONE );
		m_pSheetTabParent->m_EffLine2Bone.SetProperty ();
		break;
	case EMEFFCHAR_AROUNDEFFECT:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_AROUNDEFFECT );
		m_pSheetTabParent->m_AroundEff.SetProperty ();
		break;
	case EMEFFCHAR_OVERLAY:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_OVERLAY );
		m_pSheetTabParent->m_EffOverlay.SetProperty ();
		break;
	case EMEFFCHAR_TOTALSHADER:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_TOTALSHADER );
		m_pSheetTabParent->m_EffTotalShader.SetProperty ();
		break;
    case EMEFFCHAR_VISUAL_MATERIAL:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_VISUAL_MATERIAL );
		m_pSheetTabParent->m_EffVisualMaterial.SetProperty ();
		break;
    case EMEFFCHAR_SPLINE_SINGLE_EFF:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_SPLINE_SINGLE_EFF );
		m_pSheetTabParent->m_EffSplineSingleEff.SetProperty ();
		break;
	case EMEFFCHAR_WORLD_BATTLE_FLAG:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_WB_FLAG );
		m_pSheetTabParent->m_EffWorldBattleFlag.SetProperty ();
		break;
	case EMEFFCHAR_NULL:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_NONE );
		m_pSheetTabParent->m_EffNone.SetProperty ();
		break;
	};
#endif

	EffectListting ();
}

void CSkinPiecePage::OnBnClickedButtonEffstate()
{
	int nIndex = m_EffList.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DxEffChar* pEffCur = (DxEffChar*) m_EffList.GetItemData ( nIndex );

	assert(pEffCur);

#ifdef DEF_SKIN_SYNC
    if (pEffCur)
    {
        CsheetWithTabPiece* pParentSheet = GetParent();

        if (pParentSheet)
        {
            pParentSheet->SetActivePage(pEffCur->GetTypeID());

            CEffCharPage* pEffPage = static_cast<CEffCharPage*>(pParentSheet->GetActivePage());

            if (pEffPage)
            {
                pEffPage->SetInstance(pEffCur);
                pEffPage->SetProperty(pEffCur->GetProperty());
            }
        }
    }
#else
	switch ( pEffCur->GetTypeID() )
	{
	case EMEFFCHAR_SINGLE:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_SINGLE );
		m_pSheetTabParent->m_EffSingle.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffSingle.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_BLUR:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_BLUR );
		m_pSheetTabParent->m_EffBlur.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffBlur.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_CLONEBLUR:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_CLONEBLUR );
		m_pSheetTabParent->m_EffCloneBlur.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffCloneBlur.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_DUST:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_DUST );
		m_pSheetTabParent->m_EffDust.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffDust.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_SHOCK:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_SHOCK );
		m_pSheetTabParent->m_EffShock.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffShock.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_ATTRIBUTE:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_ATTRIBUTE );
		m_pSheetTabParent->m_EffAttribute.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffAttribute.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_ARROW:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_ARROW );
		m_pSheetTabParent->m_EffArrow.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffArrow.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_SPECULAR:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_SPECULAR );
		m_pSheetTabParent->m_EffSpecular.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffSpecular.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_SPECULAR2:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_SPECULAR2 );
		m_pSheetTabParent->m_EffSpecular2.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffSpecular2.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_EMIT:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_EMIT );
		m_pSheetTabParent->m_EffEmit.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffEmit.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_ALPHA:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_ALPHA );
		m_pSheetTabParent->m_EffAlpha.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffAlpha.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_NEON:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_NEON );
		m_pSheetTabParent->m_EffNeon.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffNeon.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_MARK:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_MARK );
		m_pSheetTabParent->m_EffMark.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffMark.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_NOALPHA:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_NOALPHA );
		m_pSheetTabParent->m_EffNoAlpha.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffNoAlpha.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_REFLECTION2:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_REFLECTION2 );
		m_pSheetTabParent->m_EffReflection2.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffReflection2.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_AMBIENT:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_AMBIENT );
		m_pSheetTabParent->m_EffAmbient.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffAmbient.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_DOT3:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_DOT3 );
		m_pSheetTabParent->m_EffDot3.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffDot3.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_MULTITEX:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_MULTITEX );
		m_pSheetTabParent->m_EffMultiTex.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffMultiTex.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_GHOSTING:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_GHOSTING );
		m_pSheetTabParent->m_EffGhosting.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffGhosting.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_LEVEL:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_LEVEL );
		m_pSheetTabParent->m_EffLevel.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffLevel.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_TOON:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_TOON );
		m_pSheetTabParent->m_EffToon.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffToon.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_TEXDIFF:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_TEXDIFF );
		m_pSheetTabParent->m_EffTexDiff.SetInstance( pEffCur );
		m_pSheetTabParent->m_EffTexDiff.SetProperty( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_PARTICLE:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_PARTICLE );
		m_pSheetTabParent->m_EffParticle.SetInstance( pEffCur );
		m_pSheetTabParent->m_EffParticle.SetProperty( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_BONEPOSEFF:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_BONEPOSEFF );
		m_pSheetTabParent->m_EffBonePosEff.SetInstance( pEffCur );
		m_pSheetTabParent->m_EffBonePosEff.SetProperty( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_BONELISTEFF:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_BONELISTEFF );
		m_pSheetTabParent->m_EffBoneListEff.SetInstance( pEffCur );
		m_pSheetTabParent->m_EffBoneListEff.SetProperty( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_USERCOLOR:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_USERCOLOR );
		m_pSheetTabParent->m_EffUserColor.SetInstance( pEffCur );
		m_pSheetTabParent->m_EffUserColor.SetProperty( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_NORMALMAP:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_NORMALMAP );
		m_pSheetTabParent->m_EffNormalMap.SetInstance( pEffCur );
		m_pSheetTabParent->m_EffNormalMap.SetProperty( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_LINE2BONE:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_LINE2BONE );
		m_pSheetTabParent->m_EffLine2Bone.SetInstance( pEffCur );
		m_pSheetTabParent->m_EffLine2Bone.SetProperty( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_AROUNDEFFECT:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_AROUNDEFFECT );
		m_pSheetTabParent->m_AroundEff.SetInstance( pEffCur );	
		m_pSheetTabParent->m_AroundEff.SetProperty ( pEffCur->GetProperty() );
		break;
	case EMEFFCHAR_OVERLAY:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_OVERLAY );
		m_pSheetTabParent->m_EffOverlay.SetInstance( pEffCur );	
		m_pSheetTabParent->m_EffOverlay.SetProperty (  pEffCur->GetProperty()  );
		break;
	case EMEFFCHAR_TOTALSHADER:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_TOTALSHADER );
		m_pSheetTabParent->m_EffTotalShader.SetInstance( pEffCur );	
		m_pSheetTabParent->m_EffTotalShader.SetProperty (  pEffCur->GetProperty()  );
		break;
    case EMEFFCHAR_VISUAL_MATERIAL:
        m_pSheetTabParent->SetActivePageTab( CHARTAB_VISUAL_MATERIAL );
		m_pSheetTabParent->m_EffVisualMaterial.SetInstance( pEffCur );	
		m_pSheetTabParent->m_EffVisualMaterial.SetProperty (  pEffCur->GetProperty()  );
		break;
    case EMEFFCHAR_SPLINE_SINGLE_EFF:
        m_pSheetTabParent->SetActivePageTab( CHARTAB_SPLINE_SINGLE_EFF );
		m_pSheetTabParent->m_EffSplineSingleEff.SetInstance( pEffCur );	
		m_pSheetTabParent->m_EffSplineSingleEff.SetProperty (  pEffCur->GetProperty()  );
		break;
	case EMEFFCHAR_WORLD_BATTLE_FLAG:
		m_pSheetTabParent->SetActivePageTab( CHARTAB_EFFECT_WB_FLAG );
		m_pSheetTabParent->m_EffWorldBattleFlag.SetInstance( pEffCur );	
		m_pSheetTabParent->m_EffWorldBattleFlag.SetProperty (  pEffCur->GetProperty()  );
		break;
	case EMEFFCHAR_NULL:
		m_pSheetTabParent->SetActivePageTab ( CHARTAB_EFFECT_NONE );
		m_pSheetTabParent->m_EffNone.SetInstance ( pEffCur );
		m_pSheetTabParent->m_EffNone.SetProperty ( pEffCur->GetProperty() );
		break;
	};
#endif
}

void CSkinPiecePage::OnBnClickedButtonEffdel()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	int nIndex = m_EffList.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DxEffChar* pEffChar = (DxEffChar*) m_EffList.GetItemData ( nIndex );
	assert(pEffChar);

	int nRet = MessageBox ( "Do you really want to deleteEffect?", "주의", MB_ICONEXCLAMATION | MB_YESNO );
	if ( IDYES == nRet )
	{
        pSkinPiece->DelEffList ( pView->GetD3dDevice(), pEffChar );
	}

#ifdef DEF_SKIN_SYNC
    CPieceContainerPage* pGrandParent = GetGrandParent();

    if (pGrandParent)
    {
        pGrandParent->ReSetPart();
    }
#endif

	EffectListting ();
}



//void CSkinPiecePage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
//{
//	CPropertyPage::OnActivate(nState, pWndOther, bMinimized);
//
//	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//}

void CSkinPiecePage::OnBnClickedButtonEffSave()
{
	//	Note : 파일 필터를 정의한다.
	CString szFilter = "Skin effect |*.effskin|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg ( FALSE,".effskin",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath();

	if ( dlg.DoModal()==IDOK )
	{
		CString sFileName = dlg.GetFileName();
		sFileName.MakeLower ();
		const char *szFileName = sFileName.GetString();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

		DxEffCharData EffCharData;
		EffCharData.Import ( pSkinPiece->m_emType, pSkinPiece->m_vecEFFECT );
		EffCharData.SaveFile ( szFileName );
	}
}

void CSkinPiecePage::OnBnClickedButtonEffLoad()
{
	//	Note : 파일 필터를 정의한다.
	CString szFilter = "Skin effect |*.effskin|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg ( TRUE,".effskin",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath();

	if ( dlg.DoModal()==IDOK )
	{
		CString sFileName = dlg.GetFileName();
		sFileName.MakeLower ();
		const char *szFileName = sFileName.GetString();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
		DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

		DxEffCharData EffCharData;
		EffCharData.LoadFile ( szFileName );

		EffCharData.SetPiece( pSkinPiece, pView->GetD3dDevice() );
	}

	EffectListting ();
}

void CSkinPiecePage::OnBnClickedButtonMaterial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

#ifndef DEF_SKIN_SYNC
	m_pSheetTabParent->SetActivePageTab ( CHARTAB_MATERIAL );
	m_pSheetTabParent->m_SkinMaterial.SetProperty ( pSkinPiece );
#else    
    CsheetWithTabPiece* pParentSheet = GetParent();

    SetActiveParentPage(CsheetWithTabPiece::EMEFFPAGE_MATERIAL);

    if (pParentSheet)
    {
        pParentSheet->SetActivePage(CsheetWithTabPiece::EMEFFPAGE_MATERIAL);

        CMaterialPage* pMaterialPage = static_cast<CMaterialPage*>(pParentSheet->GetActivePage());

        if (pMaterialPage)
        {
            pMaterialPage->SetProperty(pSkinPiece);
        }
    }
#endif
}

void CSkinPiecePage::OnLvnItemchangedListVertics(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	POSITION pos = m_listVertics.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		CCharEditView::m_strSelectedTracVert = "";
		return;
	}

	int nItem = m_listVertics.GetNextSelectedItem(pos);
	DWORD dwData = m_listVertics.GetItemData ( nItem );

	CCharEditView::m_strSelectedTracVert = szTRACE_NAME[dwData];
	
	*pResult = 0;
}

void CSkinPiecePage::OnLbnDblclkListEfflist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonEffstate();
}



void CSkinPiecePage::OnCbnSelchangeComboType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonNew();	

}

void CSkinPiecePage::SetFileName()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

    //	Note : 디스플레이 콘트롤 초기화.
    //
    CWnd *pWnd;

    pWnd = GetDlgItem ( IDC_EDIT_CHARFILE );

    if (pSkinPiece && pSkinPiece->m_szFileName)
    {
        pWnd->SetWindowText ( pSkinPiece->m_szFileName );
    }
    else
    {
        pWnd->SetWindowText ("");
    }

    pWnd = GetDlgItem ( IDC_EDIT_SKINFILE );

    if (pSkinPiece && pSkinPiece->m_szXFileName)
    {
        pWnd->SetWindowText ( pSkinPiece->m_szXFileName );
    }
    else
    {
        pWnd->SetWindowText ("");
    }

    pWnd = GetDlgItem ( IDC_EDIT_BONEFILE );

    if (pSkinPiece && pSkinPiece->m_szSkeleton)
    {
        pWnd->SetWindowText ( pSkinPiece->m_szSkeleton );
    }
    else
    {
        pWnd->SetWindowText ("");
    }
}

void CSkinPiecePage::MeshListting()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    DxSkinPiece *pSkinPiece = pView->GetSkinPiece();
    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();	

    if (pSkinPiece == 0)
    {
        return;
    }

    SMeshContainer* pMeshContainCur = 0;

    if ( pSkinPiece->m_skinMeshRes.IsValid() )
    {
        pMeshContainCur = pSkinPiece->m_skinMeshRes.get()->GetMeshClobalList ();
    }
    else
    {
        return;
    }

    m_comboMeshs.ResetContent ();

    while ( pMeshContainCur )
    {
        DWORD dwIndex = m_comboMeshs.AddString ( pMeshContainCur->Name );

#ifndef DEF_SKIN_SYNC
        m_comboMeshs.SetItemData ( dwIndex, DWORD(pMeshContainCur) );
#endif

        if (pSkinPiece->m_szMeshName && 
            !strcmp(pMeshContainCur->Name, pSkinPiece->m_szMeshName) )
        {
            m_comboMeshs.SetCurSel ( dwIndex );
        }

        pMeshContainCur = pMeshContainCur->m_pGlobalNext;
    }
}

bool CSkinPiecePage::CheckBone(DxSkinPiece* pTempSkin)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return false;
    }    

#ifdef DEF_SKIN_SYNC
    CPieceContainerPage* pGrandParent = GetGrandParent();

    if (pGrandParent)
    {
        if (pGrandParent->IsSameBoneFile(pTempSkin->m_szSkeleton) == false)
        {
            AfxMessageBox(_T("설정 되어 있는 본과 일치 하지 않습니다."));
            return false;
        }
    }
#endif

    return true;
}

bool CSkinPiecePage::CheckParts(DxSkinPiece* pTempSkin)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0)
    {
        return false;
    }

#ifdef DEF_SKIN_SYNC
    CPieceContainerPage* pGrandParent = GetGrandParent();

    if (pGrandParent)
    {
        if (pGrandParent->HasParts(pTempSkin))
        {
            if (AfxMessageBox(_T("파츠가 이미 설정 되어 있습니다.\n교체 하시겠습니까?")) == IDNO)
            {
                return false;
            }
        }
    }
#endif

    return true;
}

bool CSkinPiecePage::SetSkinPiece(DxSkinPiece* pSkinPiece)
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    if (pView == 0 || pSkinPiece == 0)
    {
        return false;
    }

    LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

    // 파일명이 없다면 만든다.
    if (pSkinPiece->m_szFileName == 0)
    {
        CString strName;

        CWnd* pWnd = GetDlgItem (IDC_EDIT_CHARFILE);

        if (pWnd)
        {
            pWnd->GetWindowText(strName);
        }

        if (strName.IsEmpty() == false)
        {
            pSkinPiece->m_szFileName = new TCHAR[strName.GetLength() + 1];
            _tcscpy(pSkinPiece->m_szFileName, strName.GetBuffer());
        }
        else
        {
            return false;
        }
    }

    // 메쉬명이 없다면 만든다.
    if (pSkinPiece->m_szMeshName == 0)
    {
        CString strName;

        int nIndex = m_comboMeshs.GetCurSel();

        if (LB_ERR != nIndex)
        {
            m_comboMeshs.GetLBText(nIndex, strName);
        }

        if (strName.IsEmpty() == false)
        {
            pSkinPiece->m_szMeshName = new TCHAR[strName.GetLength() + 1];
            _tcscpy(pSkinPiece->m_szMeshName, strName.GetBuffer());
        }
    }

    // 로딩이 완료되지 않았다면 완료 시킨다.
    if (pSkinPiece->m_bCompletedLOAD == FALSE && pSkinPiece->m_szMeshName)
    {
        if (pSkinPiece->ImmediatelyLoad(pd3dDevice) == FALSE)
        {
            return false;
        }
    }

#ifdef DEF_SKIN_SYNC
    // 최종 설정
    CsheetWithTabPiece* pParentSheet = GetParent();

    if (pParentSheet)
    {
        pParentSheet->SetSkinPiece(pSkinPiece);
    }
    else
    {
        return false;
    }
#endif

    return true;
}

CsheetWithTabPiece* CSkinPiecePage::GetParent()
{
    return static_cast<CsheetWithTabPiece*>(CPropertyPage::GetParent());
}

CPieceContainerPage* CSkinPiecePage::GetGrandParent()
{
    CsheetWithTabPiece* pParentSheet = GetParent();

    if (pParentSheet)
    {
        return static_cast<CPieceContainerPage*>(pParentSheet->GetParent());
    }
    
    return 0;
}

void CSkinPiecePage::SetPieceType(DxSkinPiece* pSkinPiece)
{
    int nIndex = m_comboSkinMeshType.GetCurSel ();
    if ( nIndex == LB_ERR )	
		return;

    DWORD dwData = m_comboSkinMeshType.GetItemData ( nIndex );

    if( m_combo_Type.GetCurSel() == PIECETYPE_CHARACTER )
    {
        pSkinPiece->m_emType = (EMPIECECHAR) dwData;
    }
	else if( m_combo_Type.GetCurSel() == PIECETYPE_ATTACHBONE )
    {
        pSkinPiece->m_emAttBoneType = (EMPIECEATTBONE) dwData;
    }
	else if( m_combo_Type.GetCurSel() == PIECETYPE_V_PARTS )
    {
        pSkinPiece->m_emV_PartType = (EMPIECEVEHICLE) dwData;
    }

	EnableWnd_HairRenderOff();
}

void CSkinPiecePage::OnBnClickedButtonSlotWeapon()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	int nIndex = m_comboWhereBack.GetCurSel ();
	if ( nIndex == LB_ERR )		
		return;

	DWORD dwData = m_comboWhereBack.GetItemData ( nIndex );
	EMPEACEZONEWEAPON emWeaponWhereBack = (EMPEACEZONEWEAPON)dwData;

	if ( emWeaponWhereBack != EMPEACE_WEAPON_USER_SLOT )
		return;

#ifndef DEF_SKIN_SYNC
	// m_pSheetTab->SetActivePageTab ( CHARTAB_USER_BACK_SLOT );
#else    
	CsheetWithTabPiece* pParentSheet = GetParent();

	SetActiveParentPage(CsheetWithTabPiece::EMEFFPAGE_USER_BACK_SLOT);

	if (pParentSheet)
	{
		pParentSheet->SetActivePage(CsheetWithTabPiece::EMEFFPAGE_USER_BACK_SLOT);
	}
#endif

	//m_pSheetTab->SetActivePageTab ( CHARTAB_USER_BACK_SLOT );
}

void CSkinPiecePage::OnBnClickedButtonDocument()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TSTRING strText = 
		"[0.Overlay] - 이제는 염색에 관한 색팅을 하기위해 존재한다. 다른 존재의 이유는 사라짐. \r\n"
		"[1.Visual Material] - 프로그래머가 만든 정해진 Material 이 아닌 디자이너가 만든 Material 을 설정할 수 있다. \r\n"
		"[2.Level (연마효과)] - 어느부위에 연마가 들어가는지 셋팅한다. \r\n"
		;

	AfxMessageBox( strText.c_str() );
}
