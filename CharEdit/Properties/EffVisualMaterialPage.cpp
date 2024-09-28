// EffTotalShaderPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../VisualMaterialLib/Manager/VisualMaterialFX.h"
#include "../VisualMaterialLib/Manager/DxVMManager.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharVisualMaterial.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMeshContainer9.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "../CharEdit.h"
#include "../Dialog/ColorPickerDlg.h"
#include "sheetWithTabChar.h"
#include "sheetWithTabTS.h"
#include "sheetWithTabPiece.h"
#include "SheetWithTabFrame.h"
#include "EffVisualMaterialPage.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CEffVisualMaterialPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffVisualMaterialPage, CPropertyPage)
CEffVisualMaterialPage::CEffVisualMaterialPage()
	: CEffCharPage(CEffVisualMaterialPage::IDD)
{
}

CEffVisualMaterialPage::~CEffVisualMaterialPage()
{
}

void CEffVisualMaterialPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_MATRIAL, m_ComboMatrials);
    DDX_Control(pDX, IDC_LIST_PARAMETER, m_ctrlParameter);
}


BEGIN_MESSAGE_MAP(CEffVisualMaterialPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_VM_FILE, &CEffVisualMaterialPage::OnBnClickedButtonVmFile)
    ON_BN_CLICKED(IDC_BUTTON_COLOR1, &CEffVisualMaterialPage::OnBnClickedButtonColor1)
    ON_BN_CLICKED(IDC_BUTTON_COLOR2, &CEffVisualMaterialPage::OnBnClickedButtonColor2)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_0, &CEffVisualMaterialPage::OnBnClickedButtonOpenTexture0)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_1, &CEffVisualMaterialPage::OnBnClickedButtonOpenTexture1)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_2, &CEffVisualMaterialPage::OnBnClickedButtonOpenTexture2)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_3, &CEffVisualMaterialPage::OnBnClickedButtonOpenTexture3)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_4, &CEffVisualMaterialPage::OnBnClickedButtonOpenTexture4)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_5, &CEffVisualMaterialPage::OnBnClickedButtonOpenTexture5)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_TEXTURE_6, &CEffVisualMaterialPage::OnBnClickedButtonOpenTexture6)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_0, &CEffVisualMaterialPage::OnBnClickedButtonResetTexture0)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_1, &CEffVisualMaterialPage::OnBnClickedButtonResetTexture1)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_2, &CEffVisualMaterialPage::OnBnClickedButtonResetTexture2)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_3, &CEffVisualMaterialPage::OnBnClickedButtonResetTexture3)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_4, &CEffVisualMaterialPage::OnBnClickedButtonResetTexture4)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_5, &CEffVisualMaterialPage::OnBnClickedButtonResetTexture5)
    ON_BN_CLICKED(IDC_BUTTON_RESET_TEXTURE_6, &CEffVisualMaterialPage::OnBnClickedButtonResetTexture6)
    ON_CBN_SELCHANGE(IDC_COMBO_MATRIAL, &CEffVisualMaterialPage::OnCbnSelchangeComboMatrial)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PARAMETER, &CEffVisualMaterialPage::OnLvnItemchangedListParameter)
    ON_BN_CLICKED(ID_BUTTON_PARAMETER_INSERT, &CEffVisualMaterialPage::OnBnClickedButtonParameterInsert)
    ON_BN_CLICKED(ID_BUTTON_PARAMETER_MODIFY, &CEffVisualMaterialPage::OnBnClickedButtonParameterModify)
    ON_BN_CLICKED(ID_BUTTON_PARAMETER_DELETE, &CEffVisualMaterialPage::OnBnClickedButtonParameterDelete)
    ON_BN_CLICKED(IDC_CHECK_NIGHT, &CEffVisualMaterialPage::OnBnClickedCheckNight)
END_MESSAGE_MAP()


// CEffVisualMaterialPage �޽��� ó�����Դϴ�.

BOOL CEffVisualMaterialPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

    // IDC ����
    DWORD dwStatic[] = { IDC_STATIC_0, IDC_STATIC_1, IDC_STATIC_2, IDC_STATIC_3, IDC_STATIC_4, IDC_STATIC_5, IDC_STATIC_6 };
    DWORD dwSrcTexture[] = { IDC_EDIT_SRC_TEXTURE_0, IDC_EDIT_SRC_TEXTURE_1, IDC_EDIT_SRC_TEXTURE_2, IDC_EDIT_SRC_TEXTURE_3,
                                IDC_EDIT_SRC_TEXTURE_4, IDC_EDIT_SRC_TEXTURE_5, IDC_EDIT_SRC_TEXTURE_6 };
    DWORD dwDestTexture[] = { IDC_EDIT_DEST_TEXTURE_0, IDC_EDIT_DEST_TEXTURE_1, IDC_EDIT_DEST_TEXTURE_2, IDC_EDIT_DEST_TEXTURE_3,
                                IDC_EDIT_DEST_TEXTURE_4, IDC_EDIT_DEST_TEXTURE_5, IDC_EDIT_DEST_TEXTURE_6 };
    DWORD dwOpenTexture[] = { IDC_BUTTON_OPEN_TEXTURE_0, IDC_BUTTON_OPEN_TEXTURE_1, IDC_BUTTON_OPEN_TEXTURE_2, IDC_BUTTON_OPEN_TEXTURE_3, 
                                IDC_BUTTON_OPEN_TEXTURE_4, IDC_BUTTON_OPEN_TEXTURE_5, IDC_BUTTON_OPEN_TEXTURE_6 };
    DWORD dwResetTexture[] = { IDC_BUTTON_RESET_TEXTURE_0, IDC_BUTTON_RESET_TEXTURE_1, IDC_BUTTON_RESET_TEXTURE_2, IDC_BUTTON_RESET_TEXTURE_3, 
                                IDC_BUTTON_RESET_TEXTURE_4, IDC_BUTTON_RESET_TEXTURE_5, IDC_BUTTON_RESET_TEXTURE_6 };

    for ( DWORD i=0; i<MAX_USE_TEXTURE_VM; ++i )
    {
        m_dwStatic[i] = dwStatic[i];
        m_dwSrcTexture[i] = dwSrcTexture[i];
        m_dwDestTexture[i] = dwDestTexture[i];
        m_dwOpenTexture[i] = dwOpenTexture[i];
        m_dwResetTexture[i] = dwResetTexture[i];
    }

    RECT listRect;
	m_ctrlParameter.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[2] = { int(nListWidth*1.05f), int(nListWidth*0.55f) };

	m_ctrlParameter.SetExtendedStyle ( m_ctrlParameter.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_ctrlParameter.InsertColumn ( 0, _T("Parameter Name"),	LVCFMT_CENTER, nColWidth[0] );

	// ��� ����
	m_ctrlParameter.DeleteAllItems();

    CWnd* pWnd(NULL);
    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X2 );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y2 );
    pWnd->SetWindowText( _T("1.0") );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z2 );
    pWnd->SetWindowText( _T("1.0") );

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffVisualMaterialPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
			(
			DxEffCharVisualMaterial::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
			);

		if ( !m_pEffChar )
		{
			MessageBox ( _T("����Ʈ ������ �����Ͽ����ϴ�."), _T("ERROR"), MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );
	}

    DxEffCharVisualMaterial* pEffChar = dynamic_cast<DxEffCharVisualMaterial*>(m_pEffChar);

    if ( !pEffChar->m_pmcMesh )
        return;
	
    if ( pEffChar->m_pmcMesh->GetNumMaterials() == 0 )
        return;

    int nIndex;
    CString Str;
	m_ComboMatrials.ResetContent();
	for ( DWORD i=0; i<pEffChar->m_pmcMesh->GetNumMaterials(); ++i )
	{
		Str.Format ( "%s", pEffChar->m_pmcMesh->pMaterials[i].pTextureFilename );
		nIndex = m_ComboMatrials.AddString ( Str );
		m_ComboMatrials.SetItemData ( nIndex, i );
	}

    // ����� ���� �ʴٸ�,
    if ( pEffChar->m_pmcMesh->GetNumMaterials() != pEffChar->m_vecProperty.size() )
    {
        for ( DWORD i=0; i<pEffChar->m_pmcMesh->GetNumMaterials(); ++i )
        {
            std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>(new EFFCHAR_PROPERTY_VISUAL_MATERIAL);
            pEffChar->m_vecProperty.push_back( spProperty );
        }
    }

    // �ӽ� ������ ����.
    m_vecProperty.clear();
    for ( DWORD i=0; i<pEffChar->m_vecProperty.size(); ++i )
    {
        std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>(new EFFCHAR_PROPERTY_VISUAL_MATERIAL);
        spProperty->CloneDummy( *pEffChar->m_vecProperty[i].get() );
        m_vecProperty.push_back( spProperty );
    }

	int nCurSel = 0;
	m_ComboMatrials.SetCurSel( nCurSel );			// �ʱⰪ.. !!
    SelectMaterial( 0 );

    // Dialog ����
    UpdateLisCtrl( -1 );
}

void CEffVisualMaterialPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !m_pEffChar ) return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
    DxEffCharVisualMaterial* pEffChar = dynamic_cast<DxEffCharVisualMaterial*>(m_pEffChar);

	int nIndex = GetCurSel_ComboMatrials();
	if ( nIndex == LB_ERR )
		return;

	CWnd *pWnd;
	CString strText;
	for ( DWORD i=0; i<MAX_USE_TEXTURE_VM; ++i )
	{
		pWnd = GetDlgItem( m_dwDestTexture[i] );
		if ( pWnd->IsWindowVisible() )
		{
			pWnd->GetWindowText( strText );
			m_vecProperty[nIndex]->m_vecTextureResource[i].m_strName = strText;
		}
	}

	pEffChar->SetPropertyNEW( pView->GetD3dDevice(), m_vecProperty );

	pEffChar->Create( pView->GetD3dDevice() );

    EffectApply();
}

void CEffVisualMaterialPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();
	OnBnClickedCancel();
}

void CEffVisualMaterialPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffVisualMaterialPage::ShowWindowDlg( const TCHAR* pNameFX, const vm::VEC_TEXTURE_RESOURCE& vecTextureResource )
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

    // �� ����
    BOOL bShowWindow(FALSE);
    std::tr1::shared_ptr<VisualMaterialFX> spVMFX = NS_VM_MANAGER::Load( pView->GetD3dDevice(), pNameFX, NS_VM_MANAGER::EMRT_SKIN_VS );

    for ( DWORD i=0; i<MAX_USE_TEXTURE_VM; ++i )
    {
        if ( i < spVMFX->GetTextureSize() )
            bShowWindow = TRUE;
        else
            bShowWindow = FALSE;

        if ( i < vecTextureResource.size() )
        {
            ShowWindowDlg( i, bShowWindow, spVMFX->GetTextureName( i ), vecTextureResource[i].m_strName.c_str() );
        }
        else
        {
            ShowWindowDlg( i, bShowWindow, spVMFX->GetTextureName( i ), NULL );
        }
    }
}

void CEffVisualMaterialPage::ShowWindowDlg( int nIndex, BOOL bShowWindow, const TCHAR* pTextureSRC, const TCHAR* pTextureDEST )
{
    GetDlgItem( m_dwStatic[nIndex] )->ShowWindow( bShowWindow );
    GetDlgItem( m_dwSrcTexture[nIndex] )->ShowWindow( bShowWindow );
    GetDlgItem( m_dwDestTexture[nIndex] )->ShowWindow( bShowWindow );
    GetDlgItem( m_dwOpenTexture[nIndex] )->ShowWindow( bShowWindow );
    GetDlgItem( m_dwResetTexture[nIndex] )->ShowWindow( bShowWindow );

    if ( bShowWindow )
    {
        GetDlgItem( m_dwSrcTexture[nIndex] )->SetWindowText( pTextureSRC );
        GetDlgItem( m_dwDestTexture[nIndex] )->SetWindowText( pTextureDEST );
    }
}

void CEffVisualMaterialPage::OnBnClickedButtonVmFile()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CString szFilter = _T("Visual Material ( *.vm ) |*.vm");
	CFileDialog dlg( TRUE, ".vm", NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    CCharEditApp* pApp = (CCharEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();
	dlg.m_ofn.lpstrInitialDir = pPath->FullPathVisualMaterial().c_str(); //SUBPATH::FULL_PATH_VISUAL_MATERIAL;

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_VM_FILE );
		pWnd->SetWindowText( FilePath.GetString() );

        // ������ ������.
        int nIndex = GetCurSel_ComboMatrials();
		if ( nIndex == LB_ERR )
			return;

        {
            CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

            std::tr1::shared_ptr<VisualMaterialFX> spVMFX = NS_VM_MANAGER::Load( pView->GetD3dDevice(), FilePath.GetString(), NS_VM_MANAGER::EMRT_SKIN_VS );

            m_vecProperty[nIndex]->m_strVisualMaterial = FilePath.GetString();

            m_vecProperty[nIndex]->m_vecTextureResource.clear();
            for ( DWORD i=0; i<spVMFX.get()->GetTextureSize(); ++i )
            {
                vm::NAME_TEXTURE sNameTexture;
                sNameTexture.m_strName = spVMFX->GetTextureName( i );
                m_vecProperty[nIndex]->m_vecTextureResource.push_back( sNameTexture );
            }
        }

        // ShowWindowDlg
        ShowWindowDlg( FilePath.GetString(), m_vecProperty[nIndex]->m_vecTextureResource );

        OnBnClickedButtonApply();

        UpdateLisCtrl( -1 );
	}
}

void CEffVisualMaterialPage::OnBnClickedButtonColor1()
{
    DxEffCharVisualMaterial* pEffChar = dynamic_cast<DxEffCharVisualMaterial*>(m_pEffChar);

    int ThisR = int ( pEffChar->m_vUserColor1.x * 255.0f );
	int ThisG = int ( pEffChar->m_vUserColor1.y * 255.0f );
	int ThisB = int ( pEffChar->m_vUserColor1.z * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pEffChar->m_vUserColor1.x = GetRValue(SelectColor)/255.0f;
		pEffChar->m_vUserColor1.y = GetGValue(SelectColor)/255.0f;
		pEffChar->m_vUserColor1.z = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}
}

void CEffVisualMaterialPage::OnBnClickedButtonColor2()
{
    DxEffCharVisualMaterial* pEffChar = dynamic_cast<DxEffCharVisualMaterial*>(m_pEffChar);

    int ThisR = int ( pEffChar->m_vUserColor2.x * 255.0f );
	int ThisG = int ( pEffChar->m_vUserColor2.y * 255.0f );
	int ThisB = int ( pEffChar->m_vUserColor2.z * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		pEffChar->m_vUserColor2.x = GetRValue(SelectColor)/255.0f;
		pEffChar->m_vUserColor2.y = GetGValue(SelectColor)/255.0f;
		pEffChar->m_vUserColor2.z = GetBValue(SelectColor)/255.0f;

		OnBnClickedButtonApply();
	}
}

void CEffVisualMaterialPage::OnBnClickedButtonOpenTexture0()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 0 );
}

void CEffVisualMaterialPage::OnBnClickedButtonOpenTexture1()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 1 );
}

void CEffVisualMaterialPage::OnBnClickedButtonOpenTexture2()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 2 );
}

void CEffVisualMaterialPage::OnBnClickedButtonOpenTexture3()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 3 );
}

void CEffVisualMaterialPage::OnBnClickedButtonOpenTexture4()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 4 );
}

void CEffVisualMaterialPage::OnBnClickedButtonOpenTexture5()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 5 );
}

void CEffVisualMaterialPage::OnBnClickedButtonOpenTexture6()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonOpenTexture( 6 );
}

void CEffVisualMaterialPage::OnBnClickedButtonResetTexture0()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 0 );
}

void CEffVisualMaterialPage::OnBnClickedButtonResetTexture1()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 1 );
}

void CEffVisualMaterialPage::OnBnClickedButtonResetTexture2()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 2 );
}

void CEffVisualMaterialPage::OnBnClickedButtonResetTexture3()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 3 );
}

void CEffVisualMaterialPage::OnBnClickedButtonResetTexture4()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 4 );
}

void CEffVisualMaterialPage::OnBnClickedButtonResetTexture5()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 5 );
}

void CEffVisualMaterialPage::OnBnClickedButtonResetTexture6()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnBnClickedButtonResetTexture( 6 );
}

void CEffVisualMaterialPage::OnBnClickedButtonOpenTexture( DWORD i )
{
    CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg( TRUE, ".BMP", NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CWnd *pWnd;
		pWnd = GetDlgItem( m_dwDestTexture[i] );
		pWnd->SetWindowText( dlg.GetFileName() );

        int nIndex = GetCurSel_ComboMatrials();
		if ( nIndex == LB_ERR )
			return;

        m_vecProperty[nIndex]->m_vecTextureResource[i].m_strName = dlg.GetFileName();
	}
}

void CEffVisualMaterialPage::OnBnClickedButtonResetTexture( DWORD i )
{
    CWnd *pWnd;
    CString strText;
    pWnd = GetDlgItem( m_dwSrcTexture[i] );
	pWnd->GetWindowText( strText );

    pWnd = GetDlgItem( m_dwDestTexture[i] );
    pWnd->SetWindowText( strText.GetString() );

    int nIndex = GetCurSel_ComboMatrials();
	if ( nIndex == LB_ERR )
		return;

    m_vecProperty[nIndex]->m_vecTextureResource[i].m_strName = strText.GetString();
}

void CEffVisualMaterialPage::OnCbnSelchangeComboMatrial()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = GetCurSel_ComboMatrials();
	if ( nIndex == LB_ERR )
		return;

    SelectMaterial( nIndex );
}

void CEffVisualMaterialPage::SelectMaterial( int nIndex )
{
    CWnd* pWnd = GetDlgItem( IDC_EDIT_VM_FILE );
    pWnd->SetWindowText( m_vecProperty[nIndex]->m_strVisualMaterial.c_str() );

    ShowWindowDlg( m_vecProperty[nIndex]->m_strVisualMaterial.c_str(), m_vecProperty[nIndex]->m_vecTextureResource );
}

void CEffVisualMaterialPage::VmfxReload_DialogReset()
{
    DxEffCharVisualMaterial* pEffChar = dynamic_cast<DxEffCharVisualMaterial*>(m_pEffChar);

    // ���� ������ ����
    {
        m_vecProperty.clear();
        for ( DWORD i=0; i<pEffChar->m_vecProperty.size(); ++i )
        {
            std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>(new EFFCHAR_PROPERTY_VISUAL_MATERIAL);
            spProperty->CloneDummy( *pEffChar->m_vecProperty[i].get() );
            m_vecProperty.push_back( spProperty );
        }
    }

	int nIndex = GetCurSel_ComboMatrials();
	if ( nIndex == LB_ERR )
		return;

    // Dlg ����
    SelectMaterial( nIndex );
}

void CEffVisualMaterialPage::OnLvnItemchangedListParameter(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nIndex = GetCurSel_ComboMatrials();
	if ( nIndex == LB_ERR )
		return;

    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    *pResult = 0;

    POSITION pos = m_ctrlParameter.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		return;
	}

	int nSize = m_ctrlParameter.GetSelectedCount();
	if( nSize==0 )	return;

    int nItem = m_ctrlParameter.GetNextSelectedItem( pos );

    const MAP_PARAMETER_NAME_DATA2& mapParameter = m_vecProperty[nIndex]->m_mapParameter;
    MAP_PARAMETER_NAME_DATA2_CITER iter = mapParameter.begin();
    for ( int i=0; iter!=mapParameter.end(); ++iter, ++i )
    {
        if ( i == nItem )
        {
            CString Str;
            CWnd* pWnd(NULL);
            CButton* pButton(NULL);
            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_NAME );
            pWnd->SetWindowText( (*iter).first.c_str() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X );
            Str.Format ( "%3.2f", (*iter).second.m_vParameter.x );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y );
            Str.Format ( "%3.2f", (*iter).second.m_vParameter.y );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z );
            Str.Format ( "%3.2f", (*iter).second.m_vParameter.z );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X2 );
            Str.Format ( "%3.2f", (*iter).second.m_vNight.x );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y2 );
            Str.Format ( "%3.2f", (*iter).second.m_vNight.y );
            pWnd->SetWindowText( Str.GetString() );

            pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z2 );
            Str.Format ( "%3.2f", (*iter).second.m_vNight.z );
            pWnd->SetWindowText( Str.GetString() );

            pButton = (CButton*)GetDlgItem( IDC_CHECK_NIGHT );
            if ( (*iter).second.m_bNight )
            {
                pButton->SetCheck( TRUE );
            }
            else
            {
                pButton->SetCheck( FALSE );
            }
        }
    }
}

void CEffVisualMaterialPage::OnBnClickedButtonParameterInsert()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int nIndex = GetCurSel_ComboMatrials();
	if ( nIndex == LB_ERR )
		return;

    // ParameterName �� ������ �۾��� �� ����.
    {
        CString Str;
        CWnd* pWnd(NULL);
        pWnd = GetDlgItem( IDC_EDIT_PARAMETER_NAME );
        pWnd->GetWindowText( Str );

        // always false, using strcmp
        if ( Str.GetString() == _T("") )
        {
            // �۾��� �� ����.
            return;
        }
    }

    // Dialog ���� Data�� �����´�.
    TSTRING strParameterName;
    ParameterData sData;
    GetPARAMETER_NAME_DATA_IntoDialog( strParameterName, sData );

    // �߰�
    MAP_PARAMETER_NAME_DATA2& mapParameter = m_vecProperty[nIndex]->m_mapParameter;
    mapParameter.insert( std::make_pair( strParameterName, sData ) );

    // Dialog ����
    DWORD dwCount(0);
    MAP_PARAMETER_NAME_DATA2_CITER iter = mapParameter.begin();
    for ( ; iter!=mapParameter.end(); ++iter, ++dwCount )
    {
        if ( (*iter).first == strParameterName )
            break;        
    }
    UpdateLisCtrl( dwCount );
}

void CEffVisualMaterialPage::OnBnClickedButtonParameterModify()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int nIndex = GetCurSel_ComboMatrials();
	if ( nIndex == LB_ERR )
		return;

    // ParameterName �� ������ �۾��� �� ����.
    {
        CString Str;
        CWnd* pWnd(NULL);
        pWnd = GetDlgItem( IDC_EDIT_PARAMETER_NAME );
        pWnd->GetWindowText( Str );
        
        // always false, using strcmp
        if ( Str.GetString() == _T("") )
        {
            // �۾��� �� ����.
            return;
        }
    }

    POSITION pos = m_ctrlParameter.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		return;
	}

	int nSize = m_ctrlParameter.GetSelectedCount();
	if( nSize==0 )	return;

    int nItem = m_ctrlParameter.GetNextSelectedItem( pos );

    MAP_PARAMETER_NAME_DATA2& mapParameter = m_vecProperty[nIndex]->m_mapParameter;
    MAP_PARAMETER_NAME_DATA2_ITER iter = mapParameter.begin();
    for ( int i=0; iter!=mapParameter.end(); ++iter, ++i )
    {
        if ( i == nItem )
        {
            mapParameter.erase( iter );

            // Dialog ���� Data�� �����´�.
            TSTRING strParameterName;
            ParameterData sData;
            GetPARAMETER_NAME_DATA_IntoDialog( strParameterName, sData );

            // �߰�
            mapParameter.insert( std::make_pair( strParameterName, sData ) );

            // Dialog ����
            DWORD dwCount(0);
            MAP_PARAMETER_NAME_DATA2_CITER iter2 = mapParameter.begin();
            for ( ; iter2!=mapParameter.end(); ++iter2, ++dwCount )
            {
                if ( (*iter2).first == strParameterName )
                    break;        
            }
            UpdateLisCtrl( dwCount );

            return;
        }
    }
}

void CEffVisualMaterialPage::OnBnClickedButtonParameterDelete()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int nIndex = GetCurSel_ComboMatrials();
	if ( nIndex == LB_ERR )
		return;

    POSITION pos = m_ctrlParameter.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		return;
	}

	int nSize = m_ctrlParameter.GetSelectedCount();
	if( nSize==0 )	return;

    int nItem = m_ctrlParameter.GetNextSelectedItem( pos );

    MAP_PARAMETER_NAME_DATA2& mapParameter = m_vecProperty[nIndex]->m_mapParameter;
    MAP_PARAMETER_NAME_DATA2_ITER iter = mapParameter.begin();
    for ( int i=0; iter!=mapParameter.end(); ++iter, ++i )
    {
        if ( i == nItem )
        {
            // ����
            mapParameter.erase( iter );

            // Dialog ����
            UpdateLisCtrl( mapParameter.size()-1 );

            return;
        }
    }
}

void CEffVisualMaterialPage::OnBnClickedCheckNight()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CEffVisualMaterialPage::GetPARAMETER_NAME_DATA_IntoDialog( TSTRING& strParameterName, ParameterData& sData )
{
    CString strText;
    CWnd* pWnd(NULL);
    CButton* pButton(NULL);
    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_NAME );
    pWnd->GetWindowText( strText );
    strParameterName = strText.GetString();

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X );
    pWnd->GetWindowText( strText );
    sData.m_vParameter.x = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y );
    pWnd->GetWindowText( strText );
    sData.m_vParameter.y = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z );
    pWnd->GetWindowText( strText );
    sData.m_vParameter.z = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_X2 );
    pWnd->GetWindowText( strText );
    sData.m_vNight.x = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Y2 );
    pWnd->GetWindowText( strText );
    sData.m_vNight.y = (float)atof ( strText.GetString() );

    pWnd = GetDlgItem( IDC_EDIT_PARAMETER_Z2 );
    pWnd->GetWindowText( strText );
    sData.m_vNight.z = (float)atof ( strText.GetString() );

    pButton = (CButton*)GetDlgItem( IDC_CHECK_NIGHT );
    if ( pButton->GetCheck() )
    {
        sData.m_bNight = TRUE;
    }
    else
    {
        sData.m_bNight = FALSE;
    }
}

void CEffVisualMaterialPage::UpdateLisCtrl( int nSelectIndex )
{
    // ���͸����� �⺻������ �־�� �Ѵ�.

    int nIndex = GetCurSel_ComboMatrials();
    if ( nIndex == LB_ERR )
        return;

    // ���ù����� ������ ���� ���
    if ( nSelectIndex >= m_ctrlParameter.GetItemCount() )
    {
        nSelectIndex = -1;
    }

    // ��� ����
	m_ctrlParameter.DeleteAllItems();

    // �ٽ� ���
    MAP_PARAMETER_NAME_DATA2& mapParameter = m_vecProperty[nIndex]->m_mapParameter;
    MAP_PARAMETER_NAME_DATA2_CITER iter = mapParameter.begin();
    for ( int i=0; iter!=mapParameter.end(); ++iter, ++i )
    {
        m_ctrlParameter.InsertItem( i, (*iter).first.c_str() );
    }

    // ���õ� ���� ���� ���� �۵���Ų��.
    if ( nSelectIndex != -1 )
    {
        m_ctrlParameter.SetSelectionMark( nSelectIndex );								// ���� ���õ�.
	    m_ctrlParameter.SetItemState( nSelectIndex, LVIS_SELECTED, LVIS_SELECTED);	    // ȸ������ ǥ�õ�.
	    m_ctrlParameter.EnsureVisible( nSelectIndex, FALSE );							// ��ũ�� �ؿ� ������ �Ű���.
	    m_ctrlParameter.SetFocus();												        // Ǫ�������� ǥ�õ�.
    }
}

int CEffVisualMaterialPage::GetCurSel_ComboMatrials()
{
	int nIndex = m_ComboMatrials.GetCurSel();
	if ( nIndex == LB_ERR )
	{
		AfxMessageBox( _T("Material�� �̸��� ��Ȯ���� �ʽ��ϴ�. �ٽ� �������ּ���.") );
	}
	return nIndex;
}

BOOL CEffVisualMaterialPage::OnSetActive()
{
#ifdef DEF_SKIN_SYNC
    CsheetWithTabPiece* pParent = static_cast<CsheetWithTabPiece*>(GetParent());

    if (pParent)
    {
        CPieceContainerPage* pGrandParent = static_cast<CPieceContainerPage*>(pParent->GetParent());

        if (pGrandParent)
        {
            pGrandParent->ShowWindow(false);
            pParent->FullWindowsSize(true);
        }
    }
#endif
    return CEffCharPage::OnSetActive();
}

BOOL CEffVisualMaterialPage::OnKillActive()
{
#ifdef DEF_SKIN_SYNC
    CsheetWithTabPiece* pParent = static_cast<CsheetWithTabPiece*>(GetParent());

    if (pParent)
    {
        CPieceContainerPage* pGrandParent = static_cast<CPieceContainerPage*>(pParent->GetParent());

        if (pGrandParent)
        {
            pGrandParent->ShowWindow(true);
            pParent->FullWindowsSize(false);
        }
    }
#endif
    return CEffCharPage::OnKillActive();
}
