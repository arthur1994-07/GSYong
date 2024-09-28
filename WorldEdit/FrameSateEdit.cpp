// FrameSateEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "SheetWithTab.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "FrameSateEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _UNACTIVESUBSET		(0xFFFFFFFF)

// CFrameSateEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFrameSateEdit, CPropertyPage)
CFrameSateEdit::CFrameSateEdit()
	: CPropertyPage(CFrameSateEdit::IDD)
{
	m_pThisFrame = NULL;
	m_ActiveSubSet = _UNACTIVESUBSET;
}

CFrameSateEdit::~CFrameSateEdit()
{
}

void CFrameSateEdit::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_LISTLINE, m_stListLine);
    DDX_Control(pDX, IDC_STATIC_MESHLINE, m_stMeshLine);
    DDX_Control(pDX, IDC_LIST_MESH, m_ListMeshAttribs);
    DDX_Control(pDX, IDC_COMBO_MESHSTATE, m_ComboMeshRendTypes);
    DDX_Control(pDX, IDC_STATIC_FRAMESTATELINE, m_stFrameStateLine);
    DDX_Control(pDX, IDC_COMBO_OBJTYPE, m_ComboObjType);
    DDX_Control(pDX, IDC_COMBO_ANITYPE, m_Combo_AniType);
}

BEGIN_MESSAGE_MAP(CFrameSateEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_MESHSTATE, OnBnClickedButtonMeshstate)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_MESHSTATE_OK, OnBnClickedButtonMeshstateOk)
	ON_LBN_SELCHANGE(IDC_LIST_MESH, OnLbnSelchangeListMesh)
	ON_CBN_SELCHANGE(IDC_COMBO_MESHSTATE, OnCbnSelchangeComboMeshstate)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE, OnBnClickedButtonReplace)
	ON_BN_CLICKED(IDC_CHECK_REPLACEFRAME, OnBnClickedCheckReplaceframe)
    ON_EN_CHANGE(IDC_EDIT_LIGHTMAP_UV_INDEX, &CFrameSateEdit::OnEnChangeEditLightmapUvIndex)
	ON_EN_CHANGE(IDC_EDIT_LM_TEXTURE_SIZE_X, &CFrameSateEdit::OnEnChangeEditLmTextureSizeX)
	ON_EN_CHANGE(IDC_EDIT_LM_TEXTURE_SIZE_Y, &CFrameSateEdit::OnEnChangeEditLmTextureSizeY)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_LIGHTMAP, &CFrameSateEdit::OnBnClickedCheckEnableLightmap)
END_MESSAGE_MAP()


// CFrameSateEdit 메시지 처리기입니다.
void CFrameSateEdit::SetInstance ( DxFrame *pDxFrame )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	MeshStateEdit ( FALSE );

	m_pThisFrame = pDxFrame;

	CWnd *pWnd;
	if ( pDxFrame->szName )
	{
		pWnd = GetDlgItem ( IDC_EDIT_FRAMENAME );
		pWnd->SetWindowText ( pDxFrame->szName );
	}

	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_REPLACEFRAME );
	pWnd = GetDlgItem ( IDC_EDIT_REPLACE );
	if ( pDxFrame->pRePlacePiece )
	{
		pWnd->SetWindowText ( pDxFrame->pRePlacePiece->szFileName );
		pButton->SetCheck ( BST_CHECKED );
	}
	else
	{
		pWnd->SetWindowText ( "" );
		pButton->SetCheck ( BST_UNCHECKED );
	}

	if ( !pDxFrame->pmsMeshs )				return;
	if ( !pDxFrame->pmsMeshs->m_pLocalMesh )	return;

	LPD3DXMESH	pMeshData = pDxFrame->pmsMeshs->m_pLocalMesh;

	DWORD					AttribTableSize = 0;
	LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

	pMeshData->GetAttributeTable( NULL, &AttribTableSize );
	pSrcAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];			
	pMeshData->GetAttributeTable( pSrcAttribTable, &AttribTableSize );

	m_ListMeshAttribs.ResetContent ();
	for ( DWORD i=0; i<AttribTableSize; i++ )
	{
		const char *szTexture = pDxFrame->pmsMeshs->m_strTextureFiles[pSrcAttribTable[i].AttribId].GetString();
		if( szTexture && strlen(szTexture) > 0 )
		{
			int nIndex = m_ListMeshAttribs.AddString ( pDxFrame->pmsMeshs->m_strTextureFiles[pSrcAttribTable[i].AttribId].GetString() );
			m_ListMeshAttribs.SetItemData ( nIndex, pSrcAttribTable[i].AttribId );
		}
		else
		{
			int nIndex = m_ListMeshAttribs.AddString ( "[No Texture File]" );
			m_ListMeshAttribs.SetItemData ( nIndex, pSrcAttribTable[i].AttribId );
		}
	}

	SAFE_DELETE(pSrcAttribTable);

	int nIndex;
	nIndex = m_ComboObjType.FindString ( 0, DxFrame::szFrameType[pDxFrame->eFrameType] );
	m_ComboObjType.SetCurSel ( nIndex );

	//	Note : 에니메이션 ROOT-Frame 의 경우...
	//
	DxAnimationMan* pAniMan = pFrameMesh->FindAniRootFrame ( pDxFrame );
	if ( pAniMan )
	{
		m_ComboObjType.EnableWindow ( TRUE );

		nIndex = m_Combo_AniType.FindString ( 0, DxAnimationMan::m_szAniType[pAniMan->m_AniType] );
		m_Combo_AniType.SetCurSel ( nIndex );
	}
	else
	{
		nIndex = m_Combo_AniType.FindString ( 0, DxAnimationMan::m_szAniType[0] );
		m_Combo_AniType.EnableWindow ( FALSE );
	}

    //m_Combo_LightMapType.SetCurSel ( pDxFrame->pmsMeshs->m_emLightMapType );

	pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_ENABLE_LIGHTMAP ) );
	pButton->SetCheck( pDxFrame->pmsMeshs->m_bEnableLightMap );

    CString strText;
    pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_UV_INDEX );
	strText.Format ( "%d", pDxFrame->pmsMeshs->m_dwLightMapIndex );
    pWnd->SetWindowText ( strText );

	pWnd = GetDlgItem( IDC_EDIT_LM_TEXTURE_SIZE_X );
	strText.Format ( "%d", pDxFrame->pmsMeshs->m_wwLightMapUserUV_TextureSize.wA );
	pWnd->SetWindowText ( strText );

	pWnd = GetDlgItem( IDC_EDIT_LM_TEXTURE_SIZE_Y );
	strText.Format ( "%d", pDxFrame->pmsMeshs->m_wwLightMapUserUV_TextureSize.wB );
	pWnd->SetWindowText ( strText );
}

BOOL CFrameSateEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ComboMeshRendTypes.ResetContent ();

	for ( DWORD i=0; i<MATERIAL_NSIZE; i++ )
	{
		int nIndex = m_ComboMeshRendTypes.AddString ( D3DEXMATERIAL::szMtrlTypes[i] );
		m_ComboMeshRendTypes.SetItemData ( nIndex, i );
	}

	for ( DWORD i=0; i<DxFrame::dwFrameTypeNum; i++ )
	{
		int nIndex = m_ComboObjType.AddString ( DxFrame::szFrameType[i] );
		m_ComboObjType.SetItemData ( nIndex, i );
	}

	for ( DWORD i=0; i<DxAnimationMan::m_dwAniTypeNum; i++ )
	{
		int nIndex = m_Combo_AniType.AddString ( DxAnimationMan::m_szAniType[i] );
		m_Combo_AniType.SetItemData ( nIndex, i );
	}

 //   for ( DWORD i=0; i<NSLIGHTMAPDEF::LMT_SIZE; i++ )
	//{
	//	int nIndex = m_Combo_LightMapType.AddString ( NSLIGHTMAPDEF::LIGHTMAPTYPE_NAME[i].c_str() );
	//	m_Combo_LightMapType.SetItemData ( nIndex, i );
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFrameSateEdit::MeshStateEdit ( BOOL bEdit )
{
	CWnd *pWnd;

	pWnd = GetDlgItem ( IDC_STATIC_MESHLINE );
	pWnd->ShowWindow ( bEdit );

	pWnd = GetDlgItem ( IDC_COMBO_MESHSTATE );
	pWnd->ShowWindow ( bEdit );

	pWnd = GetDlgItem ( IDC_BUTTON_MESHSTATE_OK );
	pWnd->ShowWindow ( bEdit );
}

void CFrameSateEdit::OnBnClickedButtonMeshstate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ListMeshAttribs.GetCurSel ();
	if ( nIndex == -1 )	return;

	MeshStateEdit ( TRUE );

	DWORD dwData = m_ListMeshAttribs.GetItemData ( nIndex );
	m_ActiveSubSet = dwData;

	assert(m_pThisFrame);
	assert(m_pThisFrame->pmsMeshs);

	DWORD dwTypeIndex = m_pThisFrame->pmsMeshs->m_exMaterials[m_ActiveSubSet].MtrlType;
	SetWin_Combo_Sel ( this, IDC_COMBO_MESHSTATE, D3DEXMATERIAL::szMtrlTypes[dwTypeIndex] );
}

void CFrameSateEdit::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MeshStateEdit ( FALSE );

	if ( m_pThisFrame && m_ActiveSubSet != _UNACTIVESUBSET )
	{
		m_pThisFrame->pmsMeshs->m_exMaterials[m_ActiveSubSet].bSubSetTriRend = FALSE;
		m_ActiveSubSet = _UNACTIVESUBSET;
	}

	m_pThisFrame = NULL;

	m_ListMeshAttribs.ResetContent ();
	CWorldEditView::SetCurType ( _CUR_SELECT );

	m_pSheetTab->SetActivePageEX( FRAMEPAGEINDEX, FRAMEPAGEINDEX );
}


void CFrameSateEdit::OnBnClickedButtonMeshstateOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MeshStateEdit ( FALSE );
}

BOOL CFrameSateEdit::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CWorldEditView::SetCurType ( _CUR_NONE );

	return CPropertyPage::OnSetActive();
}

BOOL CFrameSateEdit::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	MeshStateEdit ( FALSE );

	if ( m_pThisFrame && m_ActiveSubSet != _UNACTIVESUBSET )
	{
        // [2010.03.22] shhan - m_pThisFrame->pmsMeshs 이 NULL 이라서 뻑이났다.
        if ( m_pThisFrame->pmsMeshs && m_pThisFrame->pmsMeshs->m_exMaterials && m_ActiveSubSet < m_pThisFrame->pmsMeshs->m_cMaterials )
        {
		    m_pThisFrame->pmsMeshs->m_exMaterials[m_ActiveSubSet].bSubSetTriRend = FALSE;
        }

        m_ActiveSubSet = _UNACTIVESUBSET;
	}

	m_pThisFrame = NULL;

	m_ListMeshAttribs.ResetContent ();
	CWorldEditView::SetCurType ( _CUR_SELECT );

	return CPropertyPage::OnKillActive();
}

void CFrameSateEdit::OnLbnSelchangeListMesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_pThisFrame )	return;
	int nIndex = m_ListMeshAttribs.GetCurSel ();
	if ( nIndex == -1 )		return;


	DWORD dwData = m_ListMeshAttribs.GetItemData ( nIndex );

	assert(m_pThisFrame->pmsMeshs);
	m_pThisFrame->pmsMeshs->m_exMaterials[dwData].bSubSetTriRend = TRUE;

	if ( m_ActiveSubSet != _UNACTIVESUBSET )
	{
		m_pThisFrame->pmsMeshs->m_exMaterials[m_ActiveSubSet].bSubSetTriRend = FALSE;
	}

	m_ActiveSubSet = dwData;
}

void CFrameSateEdit::OnCbnSelchangeComboMeshstate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	int nIndex = m_ComboMeshRendTypes.GetCurSel ();
	DXMTRLTYPE MtrlType = (DXMTRLTYPE) m_ComboMeshRendTypes.GetItemData ( nIndex );
	m_pThisFrame->pmsMeshs->m_exMaterials[m_ActiveSubSet].MtrlType = MtrlType;

	pFrameMesh->ModiflyAlphaMapFrame ( m_pThisFrame );
}

void CFrameSateEdit::OnBnClickedButtonReplace()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	HRESULT hr = S_OK;
	CWnd *pWnd;

	//	Note : 파일 필터를 정의한다.
	CString szFilter = "Piece File |*.pis|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".pis",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CDlgEdit*)this);

	dlg.m_ofn.lpstrInitialDir = DxReplaceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		pWnd = GetDlgItem ( IDC_EDIT_REPLACE );
		pWnd->SetWindowText ( sFileName );
	}
}

void CFrameSateEdit::OnBnClickedCheckReplaceframe()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();

	HRESULT hr = S_OK;
	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_REPLACE );

	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_REPLACEFRAME );

	if ( pButton->GetCheck() )
	{
		CString StrFileName;
		pWnd->GetWindowText ( StrFileName );
		if ( StrFileName.GetLength() == 0 )
		{
			pButton->SetCheck ( FALSE );
			return;
		}

		if ( !pFrameMesh->AddPieceObj ( m_pThisFrame->szName, StrFileName.GetString() ) )
		{
			pButton->SetCheck ( FALSE );
			MessageBox ( "치환 조각 설정에 실패하였습니다." );
		}
		else
		{
			//	Note : 트리를 재정렬 한다.
			//
			pFrameMesh->MakePieceAABBTree ();

			//	Note : 알파맵 리스트에서 제거.
			//
			pFrameMesh->DelAlphaMapFrame ( m_pThisFrame );
		}
	}
	else
	{
		pFrameMesh->DelPieceObj ( m_pThisFrame->szName );

		//	Note : 트리를 재정렬 한다.
		//
		pFrameMesh->MakePieceAABBTree ();

		//	Note : 알파맵 리스트에 업데이트.
		//
		pFrameMesh->ModiflyAlphaMapFrame ( m_pThisFrame );

		pWnd->SetWindowText ( "" );
	}
}


void CFrameSateEdit::OnEnChangeEditLightmapUvIndex()
{
    // TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
    // CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
    // ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
    // 해당 알림 메시지를 보냅니다.

    // TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

    if ( !m_pThisFrame )	
        return;

    if ( !m_pThisFrame->pmsMeshs )
        return;

    CString strText;
    CWnd* pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_UV_INDEX );
	pWnd->GetWindowText ( strText );
    m_pThisFrame->pmsMeshs->m_dwLightMapIndex = atoi ( strText.GetString() );
}

void CFrameSateEdit::OnEnChangeEditLmTextureSizeX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_pThisFrame )
		return;

	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_EDIT_LM_TEXTURE_SIZE_X );
	pWnd->GetWindowText( strText );
	m_pThisFrame->pmsMeshs->m_wwLightMapUserUV_TextureSize.wA = _tstoi( strText.GetString() );
}

void CFrameSateEdit::OnEnChangeEditLmTextureSizeY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_pThisFrame )
		return;

	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem( IDC_EDIT_LM_TEXTURE_SIZE_Y );
	pWnd->GetWindowText( strText );
	m_pThisFrame->pmsMeshs->m_wwLightMapUserUV_TextureSize.wB = _tstoi( strText.GetString() );
}

void CFrameSateEdit::OnBnClickedCheckEnableLightmap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !m_pThisFrame )
		return;

	CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_ENABLE_LIGHTMAP ) );
	m_pThisFrame->pmsMeshs->m_bEnableLightMap = pButton->GetCheck() ? TRUE : FALSE;
}