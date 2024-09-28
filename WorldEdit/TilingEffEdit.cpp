// TilingEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../EngineLib/DxEffect/Frame/DxEffectWater.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxMethods.h"

#include "SetVar.h"
#include "ColorPickerDlg.h"

#include "TilingEffEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern D3DXVECTOR3			g_vPICK_RAY_DIR;
extern D3DXVECTOR3			g_vPICK_RAY_ORIG;


// CTilingEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTilingEffEdit, CPropertyPage)
CTilingEffEdit::CTilingEffEdit()
	: CPropertyPage(CTilingEffEdit::IDD)
	, m_pTilingEff(NULL)
    //, m_pPointEX_BackUp(NULL)
    , m_bTileChange(FALSE)
{
}

CTilingEffEdit::~CTilingEffEdit()
{
    //SAFE_DELETE_ARRAY( m_pPointEX_BackUp );
}

void CTilingEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MATERIALS, m_ListMaterials);
	DDX_Control(pDX, IDC_SLIDER_PRIORITY, m_slider_Priority);
}


BEGIN_MESSAGE_MAP(CTilingEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_CHECK_SELECT_MATERIAL, OnBnClickedCheckSelectMaterial)
	ON_BN_CLICKED(IDC_BUTTON_GETMATERIAL_D, OnBnClickedButtonGetmaterialD)
	ON_BN_CLICKED(IDC_BUTTON_GETMATERIAL_F, OnBnClickedButtonGetmaterialF)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, OnBnClickedButtonChange)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_TILE_DRAW, OnBnClickedButtonTileDraw)
	ON_LBN_SELCHANGE(IDC_LIST_MATERIALS, OnLbnSelchangeListMaterials)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_RANDOM, OnBnClickedButtonColorRandom)
	ON_EN_CHANGE(IDC_EDIT_ALPHA, OnEnChangeEditAlpha)
	ON_BN_CLICKED(IDC_CHECK_TEX_DRAW, OnBnClickedCheckTexDraw)
	ON_BN_CLICKED(IDC_CHECK_COLOR_DRAW, OnBnClickedCheckColorDraw)
	ON_BN_CLICKED(IDC_BUTTON_TILE_DELETE, OnBnClickedButtonTileDelete)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_DELETE, OnBnClickedButtonColorDelete)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PRIORITY, OnNMCustomdrawSliderPriority)
	ON_BN_CLICKED(IDC_BUTTON_MATERIAL_UP, &CTilingEffEdit::OnBnClickedButtonMaterialUp)
	ON_BN_CLICKED(IDC_BUTTON_MATERIAL_DOWN, &CTilingEffEdit::OnBnClickedButtonMaterialDown)
END_MESSAGE_MAP()


// CTilingEffEdit 메시지 처리기입니다.
void CTilingEffEdit::UpdatePage ()
{
	if ( !m_pTilingEff )	return;

	m_sProperty_Old = m_sProperty = m_pTilingEff->GetProperty();

	// 
	int nCurMaterial(-1);
	int nIndex = m_ListMaterials.GetCurSel();
	if ( nIndex != -1 )
	{
		nCurMaterial = m_ListMaterials.GetItemData( nIndex );
	}

	//	Note : 모든 항목 지워주기
	//
	m_ListMaterials.ResetContent ();

	//	Note : 선택 되었던 항목 나타내기
	//
	//DWORD dwCurMaterial = m_pTilingEff->GetCurMaterial();
	DxEffectTiling::MATERIALMAP_ITER iter = m_pTilingEff->m_mapMaterials.begin();
	for ( ; iter!=m_pTilingEff->m_mapMaterials.end(); ++iter )
	{
		char *szText = m_pTilingEff->GetStrList ( (*iter).first );

		int nIndex = m_ListMaterials.AddString ( szText );
		m_ListMaterials.SetItemData ( nIndex, (*iter).first );

		if ( nCurMaterial == (*iter).first )
		{
			m_ListMaterials.SetCurSel ( nIndex );		// 선택해. !!
		}
	}

	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SELECT_MATERIAL );
	if ( m_pTilingEff->GetSelectDraw() )			pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_TEX_DRAW );
	if ( m_sProperty.m_dwFlag & USE_TEX_DRAW )		pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_COLOR_DRAW );
	if ( m_sProperty.m_dwFlag & USE_COLOR_DRAW )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );



	char szValue[255];
	CWnd*	pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	_gcvt ( m_pTilingEff->GetCurColor().a, 7, szValue );
	pWnd->SetWindowText ( szValue );



	//	Note : 값 설정.
	//
	/*pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pTilingEff->GetName() );*/
	SetWin_Text( this, IDC_EDIT_FRAME_EFF_NAME, m_pTilingEff->GetName() );

	m_slider_Priority.SetRange( 0, 10 );
	m_slider_Priority.SetPos( m_sProperty.m_dwRenderPriority );
}

void CTilingEffEdit::SetInstance ( DxEffectTiling *pTiling )
{
	//	Note : CView 에서 Active Effect 설정 - 매트릭스 조정용
	//
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->SetActiveEffect ( pTiling );

	m_pTilingEff = pTiling;

    CWorldEditView::m_ViewType = EFFECT_TILE_VIEW;

	UpdatePage ();

    // Command 정리.
    m_sCommand.CleanUp();
}

void CTilingEffEdit::OnBnClickedButtonApply()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CString strText;

//	CWnd *pWnd;

	m_sProperty.m_dwRenderPriority = m_slider_Priority.GetPos();

	//	Note : 속성을 적용.
	//
	m_pTilingEff->SetProperty ( m_sProperty );
	m_pTilingEff->CleanUp ();

	m_pTilingEff->Create ( pView->GetD3DDevice() );
}

void CTilingEffEdit::OnBnClickedButtonOk()
{
	m_pSheetTab->SetActivePage ( 0 );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	//	Note : 물속성을 적용.
	//
	OnBnClickedButtonApply();

	//	Note : 선택 취소 - 매트릭스 조정용
	//
	pView->ResetActiveEffect();

	m_pTilingEff = NULL;
}

void CTilingEffEdit::OnBnClickedButtonCancel()
{
	m_pSheetTab->SetActivePage ( 0 );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	//	Note : 물속성을 되돌림.
	//
	m_pTilingEff->SetProperty ( m_sProperty_Old );
	m_pTilingEff->CleanUp ();

	m_pTilingEff->Create ( pView->GetD3DDevice() );

	//	Note : CView 에서 Active Effect 선택 취소 - 매트릭스 조정용
	//
	pView->ResetActiveEffect();

	m_pTilingEff = NULL;
}

void CTilingEffEdit::OnBnClickedCheckSelectMaterial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SELECT_MATERIAL );
	if ( pButton->GetCheck() )		m_pTilingEff->SetSelectDraw ( TRUE );
	else							m_pTilingEff->SetSelectDraw ( FALSE );
}

void CTilingEffEdit::OnBnClickedButtonTileDraw()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->SetCurType ( _CUR_TILE_DRAW );
}

void CTilingEffEdit::OnBnClickedButtonTileDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->SetCurType ( _CUR_TILE_DEL );
}

void CTilingEffEdit::OnBnClickedButtonGetmaterialD()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->SetCurType ( _CUR_TILE_GET );
}

void CTilingEffEdit::OnBnClickedButtonGetmaterialF()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		// List 에 삽입
		CString Str;
		Str = FilePath;
		Str.Format ( "%s", FilePath.GetString() );
		
		if ( m_ListMaterials.FindString ( 0, Str ) == LB_ERR )
		{
            std::tr1::shared_ptr<CommandTiling_Composite> spCommand( new CommandTiling_Composite() );
            std::tr1::shared_ptr<CommandTiling_MaterialChange> spCommand1( new CommandTiling_MaterialChange(pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials, m_pTilingEff->m_dwPointEX ) );
            std::tr1::shared_ptr<CommandTiling_SelectMaterial> spCommand2( new CommandTiling_SelectMaterial( m_pTilingEff->GetCurMaterial() ) );
            spCommand->Insert( spCommand1 );
            spCommand->Insert( spCommand2 );

			if ( m_pTilingEff->SetStrList ( pView->GetD3DDevice(), FilePath.GetString() ) )
			{
				int nData	= m_sProperty.m_dwUNIQUE_NUMBER;
				Str.Format ( "[%d] %s", nData, FilePath.GetString() );
				int nIndex	= m_ListMaterials.AddString ( Str );
				m_ListMaterials.SetItemData ( nIndex, nData );
				m_ListMaterials.SetCurSel ( nIndex );

				++m_sProperty.m_dwUNIQUE_NUMBER;
				m_pTilingEff->SetProperty ( m_sProperty );

				pView->SetCurType ( _CUR_TILE_SELECT_TEX );			// 그리는것 잠시 대기

                spCommand1->SetMaterial( pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials );
                spCommand2->SelectMaterial( m_pTilingEff->GetCurMaterial() );
                m_sCommand.Insert( spCommand );
			}
			else		// 같은것이 있다.
			{
				DWORD dwCurMaterial = m_pTilingEff->GetCurMaterial();
				int nSize = m_ListMaterials.GetCount();
				for ( int i=0; i<nSize; ++i )
				{
					if ( dwCurMaterial == m_ListMaterials.GetItemData(i) )
					{
						m_ListMaterials.SetCurSel ( i );		    // 선택해. !!
						pView->SetCurType ( _CUR_TILE_SELECT_TEX );	// 그리는것 잠시 대기
					}
				}

                spCommand2->SelectMaterial( m_pTilingEff->GetCurMaterial() );
                m_sCommand.Insert( spCommand2 );
			}
		}
	}
}

void CTilingEffEdit::OnBnClickedButtonChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_ListMaterials.GetCurSel ();
	if ( nIndex == -1 )	return;
	nData = m_ListMaterials.GetItemData ( nIndex );

	DxEffectTiling::MATERIALMAP_ITER iter = m_pTilingEff->m_mapMaterials.find(nData);
	if ( iter!=m_pTilingEff->m_mapMaterials.end() )
	{
		CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
		CFileDialog dlg ( TRUE, ".BMP", NULL, 
			OFN_HIDEREADONLY, szFilter, this );

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

		if ( dlg.DoModal() == IDOK )
		{
			CString FilePath = dlg.GetFileName ();

			// List 에 삽입
			CString Str;
			Str = FilePath;
			Str.Format ( "%s", FilePath.GetString() );

			if ( m_ListMaterials.FindString ( 0, Str ) == LB_ERR )
			{
                CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	            CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
                //CommandTiling_MaterialChange* pCommand = new CommandTiling_MaterialChange( pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials );
                std::tr1::shared_ptr<CommandTiling_MaterialChange> spCommand( new CommandTiling_MaterialChange(pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials, m_pTilingEff->m_dwPointEX) );
                {
                    // 텍스쳐를 바꾼다.
                    (*iter).second->strTexture = FilePath.GetString();

                    (*iter).second->m_textureRes = TextureManager::GetInstance().LoadTexture(
                        (*iter).second->strTexture,
                        false,
                        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                        0,
                        D3DFMT_UNKNOWN,
                        TEXTURE_RESIZE_NONE,
                        TEXTURE_ETC,
                        false,
						FALSE );
                }
                spCommand->SetMaterial( pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials );
                m_sCommand.Insert( spCommand );
			}
		}
	}

	UpdatePage ();
}

void CTilingEffEdit::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_ListMaterials.GetCurSel ();
	if ( nIndex == -1 )	
        return;
	nData = m_ListMaterials.GetItemData ( nIndex );

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
    //CommandTiling_MaterialChange* pCommand = new CommandTiling_MaterialChange( pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials );
    std::tr1::shared_ptr<CommandTiling_MaterialChange> spCommand( new CommandTiling_MaterialChange(pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials, m_pTilingEff->m_dwPointEX) );
    {
	    DxEffectTiling::MATERIALMAP_ITER iter = m_pTilingEff->m_mapMaterials.find(nData);
	    if ( iter!=m_pTilingEff->m_mapMaterials.end() )
	    {
		    //SAFE_DELETE ( (*iter).second );
		    m_pTilingEff->m_mapMaterials.erase ( nData );
	    }
    }
    spCommand->SetMaterial( pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials );
    m_sCommand.Insert( spCommand );

	UpdatePage ();
}

void CTilingEffEdit::OnLbnSelchangeListMaterials()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_ListMaterials.GetCurSel ();
	if ( nIndex == -1 )	return;
	nData = m_ListMaterials.GetItemData ( nIndex );

    std::tr1::shared_ptr<CommandTiling_SelectMaterial> spCommand( new CommandTiling_SelectMaterial( m_pTilingEff->GetCurMaterial() ) );
    {
	    DxEffectTiling::MATERIALMAP_ITER iter = m_pTilingEff->m_mapMaterials.find(nData);
	    if ( iter!=m_pTilingEff->m_mapMaterials.end() )
	    {
		    m_pTilingEff->SetCurMaterial ( nData );		// Material 바꿈
	    }
    }
    spCommand->SelectMaterial( m_pTilingEff->GetCurMaterial() );
    m_sCommand.Insert( spCommand );
}

void CTilingEffEdit::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DXCOLOR cColor = m_pTilingEff->GetCurColor();
	COLORREF retColor;

	int	R,G,B;
	R = (int)(cColor.r*255.f);
	G = (int)(cColor.g*255.f);
	B = (int)(cColor.b*255.f);

	COLORREF Color = RGB ( R, G, B );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );
		cColor.r = (float)R/255.f;
		cColor.g = (float)G/255.f;
		cColor.b = (float)B/255.f;

		if ( cColor.r>1.f )	cColor.r = 1.f;	// 범위 제한
		if ( cColor.r<0.f )	cColor.r = 0.f;	// 범위 제한
		if ( cColor.g>1.f )	cColor.g = 1.f;	// 범위 제한
		if ( cColor.g<0.f )	cColor.g = 0.f;	// 범위 제한
		if ( cColor.b>1.f )	cColor.b = 1.f;	// 범위 제한
		if ( cColor.b<0.f )	cColor.b = 0.f;	// 범위 제한

		m_pTilingEff->SetCurColor ( cColor );
	}
}

void CTilingEffEdit::OnBnClickedButtonColorRandom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "Color Random ( A, R, G, B )";
	DlgSetVar.m_StrComm1 = "A";
	DlgSetVar.m_StrComm2 = "R";
	DlgSetVar.m_StrComm3 = "G";
	DlgSetVar.m_StrComm4 = "B";
	
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL | USEEND4SCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 800.f;

	D3DXCOLOR cColor = m_pTilingEff->GetColorRandom();

	DlgSetVar.m_fStart		= cColor.a;
	DlgSetVar.m_fStartVar	= cColor.r;
	DlgSetVar.m_fEnd		= cColor.g;
	DlgSetVar.m_fEnd4		= cColor.b;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		cColor.a = DlgSetVar.m_fStart;
		cColor.r = DlgSetVar.m_fStartVar;
		cColor.g = DlgSetVar.m_fEnd;
		cColor.b = DlgSetVar.m_fEnd4;

		if ( cColor.a>1.f )	cColor.a = 1.f;	// 범위 제한
		if ( cColor.a<0.f )	cColor.a = 0.f;	// 범위 제한
		if ( cColor.r>1.f )	cColor.r = 1.f;	// 범위 제한
		if ( cColor.r<0.f )	cColor.r = 0.f;	// 범위 제한
		if ( cColor.g>1.f )	cColor.g = 1.f;	// 범위 제한
		if ( cColor.g<0.f )	cColor.g = 0.f;	// 범위 제한
		if ( cColor.b>1.f )	cColor.b = 1.f;	// 범위 제한
		if ( cColor.b<0.f )	cColor.b = 0.f;	// 범위 제한

		m_pTilingEff->SetColorRandom ( cColor );
	}
}

void CTilingEffEdit::OnEnChangeEditAlpha()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString	strText;
	CWnd* pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strText );
	float fAlpha = (float)atof ( strText.GetString() );

	if ( fAlpha>1.f )	fAlpha = 1.f;	// 범위 제한
	if ( fAlpha<0.f )	fAlpha = 0.f;	// 범위 제한

	D3DXCOLOR cColor = m_pTilingEff->GetCurColor();
	cColor.a = fAlpha;
	m_pTilingEff->SetCurColor ( cColor );
}

void CTilingEffEdit::OnBnClickedCheckTexDraw()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_TEX_DRAW );
	if ( pButton->GetCheck() )	m_sProperty.m_dwFlag |= USE_TEX_DRAW;
	else						m_sProperty.m_dwFlag &= ~USE_TEX_DRAW;

	m_pTilingEff->SetProperty ( m_sProperty );
}

void CTilingEffEdit::OnBnClickedCheckColorDraw()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_COLOR_DRAW );
	if ( pButton->GetCheck() )	m_sProperty.m_dwFlag |= USE_COLOR_DRAW;
	else						m_sProperty.m_dwFlag &= ~USE_COLOR_DRAW;

	m_pTilingEff->SetProperty ( m_sProperty );
}
void CTilingEffEdit::OnBnClickedButtonColorDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    BackUpTileMaterial();

	m_pTilingEff->DeleteColorALL();

    std::tr1::shared_ptr<CommandTiling_TileChange> spCommand( new CommandTiling_TileChange(m_dwPointEX,&m_matMaterials_BackUp,&m_pTilingEff->m_mapMaterials) );
    m_sCommand.Insert( spCommand );
}

void CTilingEffEdit::OnNMCustomdrawSliderPriority(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	str.Format( "우선순위 - %d", m_slider_Priority.GetPos() );
	SetWin_Text( this, IDC_STATIC_PRIORITY, str.GetString() );
	*pResult = 0;
}

void CTilingEffEdit::UpdateKeyBoardMouse()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

    if ( DxInputDevice::GetInstance().GetKeyState( DIK_1 ) & DXKEY_UP )
    {
        OnBnClickedButtonTileDraw();
    }
    else if ( DxInputDevice::GetInstance().GetKeyState( DIK_2 ) & DXKEY_UP )
    {
        OnBnClickedButtonTileDelete();
    }
    else if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_DOWNED &&
                DxInputDevice::GetInstance().GetKeyState( DIK_Z ) & DXKEY_UP )
    {
        // Undo
        m_sCommand.Undo( pView->GetD3DDevice(), m_pTilingEff );

        // Edit 정보 리셋
        UpdatePage ();
    }
    else if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_DOWNED &&
                DxInputDevice::GetInstance().GetKeyState( DIK_Y ) & DXKEY_UP )
    {
        // Redo
        m_sCommand.Redo( pView->GetD3DDevice(), m_pTilingEff );

        // Edit 정보 리셋
        UpdatePage ();
    }
    else if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWNED )
    {
        if ( !pView->Pick() )
            return;

        switch ( CWorldEditView::m_CurType )
	    {
	    case _CUR_TILE_DRAW:
        case _CUR_TILE_DEL:
            {
                // 이전정보를 저장하도록 하자.
                if ( !m_bTileChange )
                {
                    m_bTileChange = TRUE;
					BackUpTileMaterial();
                }
            }
            break;
        };

        switch ( CWorldEditView::m_CurType )
	    {
	    case _CUR_TILE_DRAW:
            {
                m_pTilingEff->TileEdit ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );
                m_pTilingEff->DxLeftMouseClick ();
            }
            break;

        case _CUR_TILE_DEL:
            {
                m_pTilingEff->TileEdit ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );
                m_pTilingEff->DeleteClick ();
            }
            break;
        };
    }
    else if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
    {
        if ( !pView->Pick() )
            return;

        switch ( CWorldEditView::m_CurType )
	    {
        case _CUR_TILE_GET:
            {
                std::tr1::shared_ptr<CommandTiling_SelectMaterial> spCommand( new CommandTiling_SelectMaterial( m_pTilingEff->GetCurMaterial() ) );
                {
				    m_pTilingEff->IsTriangle ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );

                    m_pTilingEff->SetListBox ( pView->GetD3DDevice() );
				    UpdatePage();

				    CWorldEditView::m_CurType = _CUR_TILE_DRAW;		// 선택을 했으니 그리는 것으로 변경
                }
                spCommand->SelectMaterial( m_pTilingEff->GetCurMaterial() );
                m_sCommand.Insert( spCommand );
            }
            break;

        case _CUR_TILE_DRAW:
        case _CUR_TILE_DEL:
            {
                if ( m_bTileChange )
                {
                    m_bTileChange = FALSE;
                    std::tr1::shared_ptr<CommandTiling_TileChange> spCommand( new CommandTiling_TileChange(m_dwPointEX,&m_matMaterials_BackUp,&m_pTilingEff->m_mapMaterials) );
                    m_sCommand.Insert( spCommand );
                }
            }
            break;
        };
    }

    // 텍스쳐를 선택했다면 마우스가 아무것도 눌려지지 않은 상태가 될때 까지 기다리자.
    if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_IDLE )
    {
        if ( CWorldEditView::m_CurType == _CUR_TILE_SELECT_TEX )
        {
            pView->SetCurType ( _CUR_TILE_DRAW );	// 그리는 상태로 변경
        }
    }
}

BOOL CTilingEffEdit::OnKillActive()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

    CWorldEditView::m_ViewType = FRAME_VIEW;

    return CPropertyPage::OnKillActive();
}

void CTilingEffEdit::BackUpTileMaterial()
{
	m_dwPointEX = m_pTilingEff->m_dwPointEX;

	m_matMaterials_BackUp.clear();
	DxEffectTiling::MATERIALMAP_ITER iter = m_pTilingEff->m_mapMaterials.begin();
	for ( ; iter != m_pTilingEff->m_mapMaterials.end(); ++iter )
	{
		std::tr1::shared_ptr<DxEffectTiling::MATERIALEX> spMaterials( new DxEffectTiling::MATERIALEX(m_dwPointEX) );
		memcpy( spMaterials->m_pPointUseColor, (*iter).second->m_pPointUseColor, sizeof(DxEffectTiling::POINT_USE_COLOR)*m_dwPointEX );
		spMaterials->listTriangle = (*iter).second->listTriangle;
		m_matMaterials_BackUp.insert( std::make_pair( (*iter).first, spMaterials ) );
	}
}

void CTilingEffEdit::OnBnClickedButtonMaterialUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	

	int nIndex;
	nIndex = m_ListMaterials.GetCurSel ();
	if ( nIndex == -1 )	
		return;

	// 최상단이라서 Change 불가능하다.
	if ( nIndex == 0 )
		return;

	int nData1 = m_ListMaterials.GetItemData ( nIndex );
	int nData2 = m_ListMaterials.GetItemData ( nIndex-1 );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	// Change
	{
		DxEffectTiling::MATERIALMAP_ITER iter1 = m_pTilingEff->m_mapMaterials.find(nData1);
		if ( iter1==m_pTilingEff->m_mapMaterials.end() )
			return;

		DxEffectTiling::MATERIALMAP_ITER iter2 = m_pTilingEff->m_mapMaterials.find(nData2);
		if ( iter2==m_pTilingEff->m_mapMaterials.end() )
			return;

		// swap
		std::tr1::shared_ptr<DxEffectTiling::MATERIALEX> spMaterialEX;
		spMaterialEX = (*iter1).second;
		(*iter1).second = (*iter2).second;
		(*iter2).second = spMaterialEX;
	}

	// 선택한 위치를 바꿔준다.
	m_ListMaterials.SetCurSel( nIndex-1 );

	// Update Page
	UpdatePage ();

	// 선택하면 작업해야 하는 것을 해준다. Undo에 필요한 작업도 셋팅되어진다.
	OnLbnSelchangeListMaterials();
}

void CTilingEffEdit::OnBnClickedButtonMaterialDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex;
	nIndex = m_ListMaterials.GetCurSel ();
	if ( nIndex == -1 )	
		return;

	// 최하단이라서 Change 불가능하다.
	if ( nIndex == m_ListMaterials.GetCount()-1 )
		return;

	int nData1 = m_ListMaterials.GetItemData ( nIndex );
	int nData2 = m_ListMaterials.GetItemData ( nIndex+1 );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	// Change
	{
		DxEffectTiling::MATERIALMAP_ITER iter1 = m_pTilingEff->m_mapMaterials.find(nData1);
		if ( iter1==m_pTilingEff->m_mapMaterials.end() )
			return;

		DxEffectTiling::MATERIALMAP_ITER iter2 = m_pTilingEff->m_mapMaterials.find(nData2);
		if ( iter2==m_pTilingEff->m_mapMaterials.end() )
			return;

		// swap
		std::tr1::shared_ptr<DxEffectTiling::MATERIALEX> spMaterialEX;
		spMaterialEX = (*iter1).second;
		(*iter1).second = (*iter2).second;
		(*iter2).second = spMaterialEX;
	}

	// 선택한 위치를 바꿔준다.
	m_ListMaterials.SetCurSel( nIndex+1 );

	// Update Page
	UpdatePage ();

	// 선택하면 작업해야 하는 것을 해준다. Undo에 필요한 작업도 셋팅되어진다.
	OnLbnSelchangeListMaterials();
}
