// MaterialPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "MaterialPage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../Enginelib/DxEffect/Char/DxEffChar.h"

#ifdef DEF_SKIN_SYNC
#include "sheetWithTabPiece.h"
#endif

// CMaterialPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMaterialPage, CPropertyPage)
CMaterialPage::CMaterialPage()
	: CPieceRelativePage(CMaterialPage::IDD),

	m_pPiece(NULL),
	m_dwMaterialNum(0),
	m_pMaterials(NULL),
	m_nMaterialSel(LB_ERR)
{
}

CMaterialPage::~CMaterialPage()
{
	ReSetProperty ();
}

void CMaterialPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MATERIAL_LIST, m_listboxMaterial);
	DDX_Control(pDX, IDC_COMBO_ZBIAS, m_comboZBias);
}


BEGIN_MESSAGE_MAP(CMaterialPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_LIST_MATERIAL_LIST, OnLbnSelchangeListMaterialList)

	// 사용 안하는 부분.
	ON_CBN_SELCHANGE(IDC_COMBO_ZBIAS, OnCbnSelchangeComboZbias)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_SEL, OnBnClickedButtonTextureSel)
	ON_EN_CHANGE(IDC_EDIT_TEXTURE, OnEnChangeEditTexture)
	ON_BN_CLICKED(IDC_CHECK_SHADOW, OnBnClickedCheckShadow)
	ON_BN_CLICKED(IDC_CHECK_ALPHA_BLEND_SRC_DEST_ALPHA, &CMaterialPage::OnBnClickedCheckAlphaBlendSrcDestAlpha)

	// Material System 으로 새로 사용되는 부분.
	ON_BN_CLICKED(IDC_CHECK_CUBEMAP, OnBnClickedCubeMap)
	ON_BN_CLICKED(IDC_CHECK_NORMALMAP, OnBnClickedNormalMap)
	ON_BN_CLICKED(IDC_RADIO_BASE, OnBnClickedRadioBase)
	ON_BN_CLICKED(IDC_RADIO_HAIR, OnBnClickedRadioHair)
	ON_BN_CLICKED(IDC_RADIO_CLOTH, OnBnClickedRadioCloth)
	ON_BN_CLICKED(IDC_BUTTON_HAIR_SPEC_COLOR, OnBnClickedButtonHairSpecularColor)
	ON_EN_CHANGE(IDC_EDIT_HAIR_SHIFT_U, OnEnChangeHairShiftU)
	ON_EN_CHANGE(IDC_EDIT_HAIR_SHIFT_V, OnEnChangeHairShiftV)
	ON_EN_CHANGE(IDC_EDIT_HAIR_SPEC_DIRECTION_INTENSITY, OnEnChangeHairSpecDirectionIntensity)
	ON_EN_CHANGE(IDC_EDIT_HAIR_SPEC_EXP_DIRECTION, OnEnChangeHairSpecExpDirection)
	ON_EN_CHANGE(IDC_EDIT_HAIR_SPEC_EXP_CAMERA, OnEnChangeHairSpecExpCamera)
	ON_BN_CLICKED(IDC_BUTTON_CLOTH_APPLY, OnBnClickedClothApply)

END_MESSAGE_MAP()


// CMaterialPage 메시지 처리기입니다.

void CMaterialPage::SetProperty ( DxSkinPiece *pPiece )
{
	m_pPiece = pPiece;

	m_dwMaterialNum = pPiece->m_dwMaterialNum;
	if ( m_dwMaterialNum )
	{
		SAFE_DELETE_ARRAY(m_pMaterials);

		m_pMaterials = new MATERIAL[m_dwMaterialNum];
		
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			const SMATERIAL_PIECE &smtrlPiece = pPiece->m_arrayMaterialPiece[i];

			if ( pPiece->m_pmcMesh->pMaterials[i].pTextureFilename )
			{
				m_pMaterials[i].strTexture = pPiece->m_pmcMesh->pMaterials[i].pTextureFilename;
			}
			else
			{
				m_pMaterials[i].strTexture = _T("Error");
			}
			m_pMaterials[i].strTextureSel = smtrlPiece.m_strTexture;
			m_pMaterials[i].m_dwFlags = smtrlPiece.m_dwFlags;
			m_pMaterials[i].dwZBias = smtrlPiece.m_dwZBias;
			m_pMaterials[i].m_dwBlendFlag = smtrlPiece.m_dwBlendFlag;
		}
	}

	m_listboxMaterial.ResetContent ();

	if ( !m_pMaterials )	return;

	int nIndex = 0;
	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		nIndex = m_listboxMaterial.AddString ( m_pMaterials[i].strTexture.c_str() );
		m_listboxMaterial.SetItemData ( nIndex, i );
	}

	m_nMaterialSel = LB_ERR;
	m_listboxMaterial.SetCurSel ( m_nMaterialSel );

	m_comboZBias.SetCurSel ( LB_ERR );

	CEdit *pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_TEXTURE );
	pEdit->SetWindowText ( "" );

	CWnd* pWnd;
	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_ALPHA_BLEND_SRC_DEST_ALPHA );
	pButton->SetCheck( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SHADOW );
	if( m_pPiece->m_dwFlag & EMCF_SHADOW )	pButton->SetCheck( TRUE );
	else									pButton->SetCheck( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_RADIO_BASE );
	pButton->ShowWindow( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_HAIR );
	pButton->ShowWindow( FALSE );
	pButton = (CButton*) GetDlgItem ( IDC_RADIO_CLOTH );
	pButton->ShowWindow( FALSE );

	DWORD arraySetting[10] = { IDC_STATIC_MASK_R, 
								IDC_CHECK_CUBEMAP, 
								IDC_STATIC_MASK_B, 
								IDC_STATIC_MASK_B2,
								IDC_STATIC_MASK2_R,
								IDC_STATIC_MASK2_R2,
								IDC_STATIC_MASK2_G,
								IDC_STATIC_MASK2_B,
								IDC_STATIC_MASK2_B2,
								IDC_CHECK_NORMALMAP };

	for ( DWORD i=0; i<10; ++i )
	{
		pWnd = GetDlgItem( arraySetting[i] );
		pWnd->ShowWindow( FALSE );
	}


	DWORD arraySettingHair[15] = { IDC_STATIC_HAIR_SHIFT_R,
									IDC_STATIC_HAIR_SHIFT_U,
									IDC_EDIT_HAIR_SHIFT_U,
									IDC_STATIC_HAIR_SHIFT_V,
									IDC_EDIT_HAIR_SHIFT_V,
									IDC_STATIC_SPEC_DIRECTION_LIGHT,
									IDC_STATIC_HAIR_SPEC_DIRECTION_INTENSITY,
									IDC_EDIT_HAIR_SPEC_DIRECTION_INTENSITY,
									IDC_STATIC_HAIR_SPEC_EXP_TRT,
									IDC_EDIT_HAIR_SPEC_EXP_DIRECTION,
									IDC_STATIC_SPEC_CAMERA_LIGHT,
									IDC_STATIC_HAIR_MASK_G,
									IDC_BUTTON_HAIR_SPEC_COLOR,
									IDC_STATIC_HAIR_SPEC_EXP_R,
									IDC_EDIT_HAIR_SPEC_EXP_CAMERA };

	for ( DWORD i=0; i<15; ++i )
	{
		pWnd = GetDlgItem( arraySettingHair[i] );
		pWnd->ShowWindow( FALSE );
	}


	DWORD arraySettingCloth[12] = { IDC_STATIC_CLOTH_DAMP,
									IDC_STATIC_CLOTH_SOLVER_FREQUENCY,
									IDC_STATIC_CLOTH_STRETCHING,
									IDC_STATIC_CLOTH_HORIZONTAL,
									IDC_STATIC_CLOTH_SHEARING,
									IDC_STATIC_CLOTH_BENDING,
									IDC_EDIT_CLOTH_DAMP,
									IDC_EDIT_CLOTH_SOLVER_FREQUENCY,
									IDC_EDIT_CLOTH_STRETCHING,
									IDC_EDIT_CLOTH_HORIZONTAL,
									IDC_EDIT_CLOTH_SHEARING,
									IDC_EDIT_CLOTH_BENDING };

	for ( DWORD i=0; i<12; ++i )
	{
		pWnd = GetDlgItem( arraySettingCloth[i] );
		pWnd->ShowWindow( FALSE );
	}

	

	////////////////////////////////////////////////////////////////////////////
	////				Material System 으로 새로 사용되는 부분.
	//CWnd* pWnd;
	//CString Str;

	//pButton = (CButton*) GetDlgItem ( IDC_CHECK_CUBEMAP );
	//if ( m_pPiece->m_pMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CUBEMAP )
	//{
	//	pButton->SetCheck( TRUE );
	//}
	//else
	//{
	//	pButton->SetCheck( FALSE );
	//}

	//pWnd = GetDlgItem( IDC_EDIT_CUBEMAP_VALUE );
	//Str.Format ( _T("%3.1f"), m_pPiece->m_pMaterialPiece[m_nMaterialSel].m_fCubeMapValue );
	//pWnd->SetWindowText( Str.GetString() );


	//pButton = (CButton*) GetDlgItem ( IDC_CHECK_LEVEL );
	//if ( m_pPiece->m_pMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_LEVEL )
	//{
	//	pButton->SetCheck( TRUE );
	//}
	//else
	//{
	//	pButton->SetCheck( FALSE );
	//}

	//pButton = (CButton*) GetDlgItem ( IDC_CHECK_NORMALMAP );
	//if ( m_pPiece->m_pMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_NORMALMAP )
	//{
	//	pButton->SetCheck( TRUE );
	//}
	//else
	//{
	//	pButton->SetCheck( FALSE );
	//}
}

void CMaterialPage::ReSetProperty ()
{
	m_pPiece = 0;
	SAFE_DELETE_ARRAY(m_pMaterials);
}

void CMaterialPage::OnBnClickedButtonApply()
{
	if ( !m_pPiece )		return;

	if ( !m_pMaterials )	return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		SMATERIAL_PIECE &smtrlPiece = m_pPiece->m_arrayMaterialPiece[i];

		smtrlPiece.m_strTexture		= m_pMaterials[i].strTextureSel;
		smtrlPiece.m_dwZBias		= m_pMaterials[i].dwZBias;
		smtrlPiece.m_dwBlendFlag	= m_pMaterials[i].m_dwBlendFlag;

		smtrlPiece.LoadTexture();
		smtrlPiece.LoadMaskTex ( pView->GetD3dDevice() );
	}

#ifdef DEF_SKIN_SYNC
    CsheetWithTabPiece* pParent = static_cast<CsheetWithTabPiece*>(GetParent());

    if (pParent)
    {
        CPieceContainerPage* pGrandParent = static_cast<CPieceContainerPage*>(pParent->GetParent());

        if (pGrandParent)
        {
            pGrandParent->ReSetPart();
        }
    }
#endif
}

void CMaterialPage::OnBnClickedCancel()
{
 //   ReturnPage();

	//ReSetProperty ();
}

void CMaterialPage::OnBnClickedOk()
{
	if ( !m_pPiece )		return;

	OnBnClickedButtonApply();

    ReturnPage();

	ReSetProperty ();

}

void CMaterialPage::OnLbnSelchangeListMaterialList()
{
	if ( !m_pPiece )		return;

	int nIndex = m_listboxMaterial.GetCurSel ();
	if ( nIndex == LB_ERR )	return;

	m_nMaterialSel = m_listboxMaterial.GetItemData ( nIndex );
	DWORD dwZBias = m_pMaterials[m_nMaterialSel].dwZBias;

	m_comboZBias.SetCurSel ( int ( dwZBias ) );

	CEdit *pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_TEXTURE );
	pEdit->SetWindowText ( m_pMaterials[m_nMaterialSel].strTextureSel.c_str() );

	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_ALPHA_BLEND_SRC_DEST_ALPHA );
	pButton->SetCheck( m_pMaterials[m_nMaterialSel].m_dwBlendFlag );


	//////////////////////////////////////////////////////////////////////////
	//				Material System 으로 새로 사용되는 부분.
	CString Str;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_CUBEMAP );
	if ( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CUBEMAP )
	{
		pButton->SetCheck( TRUE );
	}
	else
	{
		pButton->SetCheck( FALSE );
	}

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_NORMALMAP );
	if ( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_NORMALMAP )
	{
		pButton->SetCheck( TRUE );
	}
	else
	{
		pButton->SetCheck( FALSE );
	}

	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_BASE );
		pButton->ShowWindow( TRUE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_HAIR );
		pButton->ShowWindow( TRUE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_CLOTH );
		pButton->ShowWindow( TRUE );

		SetTypeShowWindow();
	}

	CWnd* pWnd;

	//////////////////////////////////////////////////////////////////////////
	pWnd = GetDlgItem( IDC_EDIT_HAIR_SHIFT_U );
	Str.Format ( _T("%3.1f"), m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairShiftUV_SpecExp_DL_CL.x );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_HAIR_SHIFT_V );
	Str.Format ( _T("%3.1f"), m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairShiftUV_SpecExp_DL_CL.y );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_HAIR_SPEC_EXP_DIRECTION );
	Str.Format ( _T("%d"), static_cast<int>( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairShiftUV_SpecExp_DL_CL.z ) );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_HAIR_SPEC_DIRECTION_INTENSITY );
	Str.Format ( _T("%3.2f"), m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairSpecColor_DirectionIntensity.w );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_HAIR_SPEC_EXP_CAMERA );
	Str.Format ( _T("%d"), static_cast<int>( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairShiftUV_SpecExp_DL_CL.w ) );
	pWnd->SetWindowText( Str.GetString() );


	//////////////////////////////////////////////////////////////////////////
	pWnd = GetDlgItem( IDC_EDIT_CLOTH_DAMP );
	Str.Format ( _T("%3.2f"), m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_fClothDampingCoefficient );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_SOLVER_FREQUENCY );
	Str.Format ( _T("%d"), static_cast<int>( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_nClothSolverFrequency ) );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_STRETCHING );
	Str.Format ( _T("%3.2f"), m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vClothConfiguration.x );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_HORIZONTAL );
	Str.Format ( _T("%3.2f"), m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vClothConfiguration.y );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_SHEARING );
	Str.Format ( _T("%3.2f"), m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vClothConfiguration.z );
	pWnd->SetWindowText( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_BENDING );
	Str.Format ( _T("%3.2f"), m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vClothConfiguration.w );
	pWnd->SetWindowText( Str.GetString() );
}

BOOL CMaterialPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	int nIndex = 0;
	for ( DWORD i=0; i<12; ++i )
	{
		CString strText;
		strText.Format ( "%d zbias", i );
		nIndex = m_comboZBias.AddString ( strText );
		m_comboZBias.SetItemData ( nIndex, i );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



//////////////////////////////////////////////////////////////////////////
//								사용 안하는 부분.
void CMaterialPage::OnCbnSelchangeComboZbias()
{
	if ( !m_pPiece )		return;

	int nIndex = m_comboZBias.GetCurSel ();
	if ( nIndex == LB_ERR )				return;
	if ( m_nMaterialSel == LB_ERR )		return;

	m_pMaterials[m_nMaterialSel].dwZBias = m_comboZBias.GetItemData ( nIndex );
}

void CMaterialPage::OnBnClickedButtonTextureSel()
{
	if ( !m_pPiece )		return;

	int nIndex = m_comboZBias.GetCurSel ();
	if ( nIndex == LB_ERR )				return;
	if ( m_nMaterialSel == LB_ERR )		return;

	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();
		m_pMaterials[m_nMaterialSel].strTextureSel = FilePath.GetString ();
		
		m_pMaterials[m_nMaterialSel].m_dwFlags = NULL;

		CEdit *pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_TEXTURE );
		pEdit->SetWindowText ( m_pMaterials[m_nMaterialSel].strTextureSel.c_str() );
	}
}

void CMaterialPage::OnEnChangeEditTexture()
{
	if ( !m_pPiece )		return;

	int nIndex = m_comboZBias.GetCurSel ();
	if ( nIndex == LB_ERR )				return;
	if ( m_nMaterialSel == LB_ERR )		return;

	CString strTexture;
	CEdit *pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_TEXTURE );
	pEdit->GetWindowText ( strTexture );

	m_pMaterials[m_nMaterialSel].strTextureSel = strTexture.GetString ();
	m_pMaterials[m_nMaterialSel].m_dwFlags = NULL;
}

void CMaterialPage::OnBnClickedCheckShadow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pPiece )	return;

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SHADOW );
	if ( pButton->GetCheck() )	m_pPiece->m_dwFlag |= EMCF_SHADOW;
	else						m_pPiece->m_dwFlag &= ~EMCF_SHADOW;
}

void CMaterialPage::OnBnClickedCheckAlphaBlendSrcDestAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_pPiece )		return;

	int nIndex = m_comboZBias.GetCurSel ();
	if ( nIndex == LB_ERR )				return;
	if ( m_nMaterialSel == LB_ERR )		return;

	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_ALPHA_BLEND_SRC_DEST_ALPHA );
	m_pMaterials[m_nMaterialSel].m_dwBlendFlag = pButton->GetCheck() ? 1 : 0;
}



//////////////////////////////////////////////////////////////////////////
//				Material System 으로 새로 사용되는 부분.
void CMaterialPage::OnBnClickedCubeMap()
{
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_CUBEMAP );
	if ( pButton->GetCheck() )
	{
		m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags |= SMATERIAL_PIECE::SKIN_MS_TYPE_CUBEMAP;
	}
	else
	{
		m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags &= ~SMATERIAL_PIECE::SKIN_MS_TYPE_CUBEMAP;
	}
}

void CMaterialPage::OnBnClickedNormalMap()
{
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_NORMALMAP );
	if ( pButton->GetCheck() )
	{
		m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags |= SMATERIAL_PIECE::SKIN_MS_TYPE_NORMALMAP;
	}
	else
	{
		m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags &= ~SMATERIAL_PIECE::SKIN_MS_TYPE_NORMALMAP;
	}
}

void CMaterialPage::OnBnClickedRadioBase()
{
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags &= ~SMATERIAL_PIECE::SKIN_MS_TYPE_HAIR;
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags &= ~SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH;
	SetTypeShowWindow();
	NSMaterialSkinManager::OnlyVersionUpForTool();
}

void CMaterialPage::OnBnClickedRadioHair()
{
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags |= SMATERIAL_PIECE::SKIN_MS_TYPE_HAIR;
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags &= ~SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH;
	SetTypeShowWindow();
	NSMaterialSkinManager::OnlyVersionUpForTool();
}

void CMaterialPage::OnBnClickedRadioCloth()
{
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags &= ~SMATERIAL_PIECE::SKIN_MS_TYPE_HAIR;
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags |= SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH;
	SetTypeShowWindow();
	NSMaterialSkinManager::OnlyVersionUpForTool();
}

void CMaterialPage::SetTypeShowWindow()
{
	DWORD arraySetting[10] = { IDC_STATIC_MASK_R, 
								IDC_CHECK_CUBEMAP, 
								IDC_STATIC_MASK_B, 
								IDC_STATIC_MASK_B2,
								IDC_STATIC_MASK2_R,
								IDC_STATIC_MASK2_R2,
								IDC_STATIC_MASK2_G,
								IDC_STATIC_MASK2_B,
								IDC_STATIC_MASK2_B2,
								IDC_CHECK_NORMALMAP };

	DWORD arraySettingHair[15] = { IDC_STATIC_HAIR_SHIFT_R,
									IDC_STATIC_HAIR_SHIFT_U,
									IDC_EDIT_HAIR_SHIFT_U,
									IDC_STATIC_HAIR_SHIFT_V,
									IDC_EDIT_HAIR_SHIFT_V,
									IDC_STATIC_SPEC_DIRECTION_LIGHT,
									IDC_STATIC_HAIR_SPEC_DIRECTION_INTENSITY,
									IDC_EDIT_HAIR_SPEC_DIRECTION_INTENSITY,
									IDC_STATIC_HAIR_SPEC_EXP_TRT,
									IDC_EDIT_HAIR_SPEC_EXP_DIRECTION,
									IDC_STATIC_SPEC_CAMERA_LIGHT,
									IDC_STATIC_HAIR_MASK_G,
									IDC_BUTTON_HAIR_SPEC_COLOR,
									IDC_STATIC_HAIR_SPEC_EXP_R,
									IDC_EDIT_HAIR_SPEC_EXP_CAMERA };

	DWORD arraySettingCloth[12] = { IDC_STATIC_CLOTH_DAMP,
									IDC_STATIC_CLOTH_SOLVER_FREQUENCY,
									IDC_STATIC_CLOTH_STRETCHING,
									IDC_STATIC_CLOTH_HORIZONTAL,
									IDC_STATIC_CLOTH_SHEARING,
									IDC_STATIC_CLOTH_BENDING,
									IDC_EDIT_CLOTH_DAMP,
									IDC_EDIT_CLOTH_SOLVER_FREQUENCY,
									IDC_EDIT_CLOTH_STRETCHING,
									IDC_EDIT_CLOTH_HORIZONTAL,
									IDC_EDIT_CLOTH_SHEARING,
									IDC_EDIT_CLOTH_BENDING };


	CWnd* pWnd;
	CButton* pButton;
	if ( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_HAIR )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_BASE );
		pButton->SetCheck( FALSE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_HAIR );
		pButton->SetCheck( TRUE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_CLOTH );
		pButton->SetCheck( FALSE );

		for ( DWORD i=0; i<10; ++i )
		{
			pWnd = GetDlgItem( arraySetting[i] );
			pWnd->ShowWindow( FALSE );
		}

		for ( DWORD i=0; i<15; ++i )
		{
			pWnd = GetDlgItem( arraySettingHair[i] );
			pWnd->ShowWindow( TRUE );
		}

		for ( DWORD i=0; i<12; ++i )
		{
			pWnd = GetDlgItem( arraySettingCloth[i] );
			pWnd->ShowWindow( FALSE );
		}
	}
	else if ( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_BASE );
		pButton->SetCheck( FALSE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_HAIR );
		pButton->SetCheck( FALSE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_CLOTH );
		pButton->SetCheck( TRUE );

		for ( DWORD i=0; i<10; ++i )
		{
			pWnd = GetDlgItem( arraySetting[i] );
			pWnd->ShowWindow( FALSE );
		}

		for ( DWORD i=0; i<15; ++i )
		{
			pWnd = GetDlgItem( arraySettingHair[i] );
			pWnd->ShowWindow( FALSE );
		}

		for ( DWORD i=0; i<12; ++i )
		{
			pWnd = GetDlgItem( arraySettingCloth[i] );
			pWnd->ShowWindow( TRUE );
		}
	}
	else
	{
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_BASE );
		pButton->SetCheck( TRUE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_HAIR );
		pButton->SetCheck( FALSE );
		pButton = (CButton*) GetDlgItem ( IDC_RADIO_CLOTH );
		pButton->SetCheck( FALSE );

		for ( DWORD i=0; i<10; ++i )
		{
			pWnd = GetDlgItem( arraySetting[i] );
			pWnd->ShowWindow( TRUE );
		}

		for ( DWORD i=0; i<15; ++i )
		{
			pWnd = GetDlgItem( arraySettingHair[i] );
			pWnd->ShowWindow( FALSE );
		}

		for ( DWORD i=0; i<12; ++i )
		{
			pWnd = GetDlgItem( arraySettingCloth[i] );
			pWnd->ShowWindow( FALSE );
		}
	}
}

void CMaterialPage::OnBnClickedButtonHairSpecularColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	int ThisR = int ( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairSpecColor_DirectionIntensity.x * 255.0f );
	int ThisG = int ( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairSpecColor_DirectionIntensity.y * 255.0f );
	int ThisB = int ( m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairSpecColor_DirectionIntensity.z * 255.0f );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();
		m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairSpecColor_DirectionIntensity.x = GetRValue(SelectColor)/255.0f;
		m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairSpecColor_DirectionIntensity.y = GetGValue(SelectColor)/255.0f;
		m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairSpecColor_DirectionIntensity.z = GetBValue(SelectColor)/255.0f;
	}
}

void CMaterialPage::OnEnChangeHairShiftU()
{
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_EDIT_HAIR_SHIFT_U );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairShiftUV_SpecExp_DL_CL.x = (float) _tstof( strText.GetString() );
}

void CMaterialPage::OnEnChangeHairShiftV()
{
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_EDIT_HAIR_SHIFT_V );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairShiftUV_SpecExp_DL_CL.y = (float) _tstof( strText.GetString() );
}

void CMaterialPage::OnEnChangeHairSpecDirectionIntensity()
{
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_EDIT_HAIR_SPEC_DIRECTION_INTENSITY );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairSpecColor_DirectionIntensity.w = (float) _tstof( strText.GetString() );
}

void CMaterialPage::OnEnChangeHairSpecExpDirection()
{
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_EDIT_HAIR_SPEC_EXP_DIRECTION );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairShiftUV_SpecExp_DL_CL.z = (float) _tstof( strText.GetString() );
}


void CMaterialPage::OnEnChangeHairSpecExpCamera()
{
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_EDIT_HAIR_SPEC_EXP_CAMERA );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vHairShiftUV_SpecExp_DL_CL.w = (float) _tstof( strText.GetString() );
}

void CMaterialPage::OnBnClickedClothApply()
{
	if ( !m_pPiece )
		return;

	if ( m_nMaterialSel == LB_ERR )
		return;

	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_EDIT_CLOTH_DAMP );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_fClothDampingCoefficient = (float) _tstof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_SOLVER_FREQUENCY );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_nClothSolverFrequency = _tstoi( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_STRETCHING );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vClothConfiguration.x = (float) _tstof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_HORIZONTAL );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vClothConfiguration.y = (float) _tstof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_SHEARING );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vClothConfiguration.z = (float) _tstof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_CLOTH_BENDING );
	pWnd->GetWindowText( strText );
	m_pPiece->m_arrayMaterialPiece[m_nMaterialSel].m_vClothConfiguration.w = (float) _tstof( strText.GetString() );

	// Data를 새로 만들게 된다.
	NSMaterialSkinManager::OnlyVersionUpForTool();
}