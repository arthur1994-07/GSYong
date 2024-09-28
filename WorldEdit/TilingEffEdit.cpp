// TilingEffEdit.cpp : ���� �����Դϴ�.
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


// CTilingEffEdit ��ȭ �����Դϴ�.

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


// CTilingEffEdit �޽��� ó�����Դϴ�.
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

	//	Note : ��� �׸� �����ֱ�
	//
	m_ListMaterials.ResetContent ();

	//	Note : ���� �Ǿ��� �׸� ��Ÿ����
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
			m_ListMaterials.SetCurSel ( nIndex );		// ������. !!
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



	//	Note : �� ����.
	//
	/*pWnd = GetDlgItem ( IDC_EDIT_FRAME_EFF_NAME );
	pWnd->SetWindowText ( m_pTilingEff->GetName() );*/
	SetWin_Text( this, IDC_EDIT_FRAME_EFF_NAME, m_pTilingEff->GetName() );

	m_slider_Priority.SetRange( 0, 10 );
	m_slider_Priority.SetPos( m_sProperty.m_dwRenderPriority );
}

void CTilingEffEdit::SetInstance ( DxEffectTiling *pTiling )
{
	//	Note : CView ���� Active Effect ���� - ��Ʈ���� ������
	//
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->SetActiveEffect ( pTiling );

	m_pTilingEff = pTiling;

    CWorldEditView::m_ViewType = EFFECT_TILE_VIEW;

	UpdatePage ();

    // Command ����.
    m_sCommand.CleanUp();
}

void CTilingEffEdit::OnBnClickedButtonApply()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CString strText;

//	CWnd *pWnd;

	m_sProperty.m_dwRenderPriority = m_slider_Priority.GetPos();

	//	Note : �Ӽ��� ����.
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

	//	Note : ���Ӽ��� ����.
	//
	OnBnClickedButtonApply();

	//	Note : ���� ��� - ��Ʈ���� ������
	//
	pView->ResetActiveEffect();

	m_pTilingEff = NULL;
}

void CTilingEffEdit::OnBnClickedButtonCancel()
{
	m_pSheetTab->SetActivePage ( 0 );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	//	Note : ���Ӽ��� �ǵ���.
	//
	m_pTilingEff->SetProperty ( m_sProperty_Old );
	m_pTilingEff->CleanUp ();

	m_pTilingEff->Create ( pView->GetD3DDevice() );

	//	Note : CView ���� Active Effect ���� ��� - ��Ʈ���� ������
	//
	pView->ResetActiveEffect();

	m_pTilingEff = NULL;
}

void CTilingEffEdit::OnBnClickedCheckSelectMaterial()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_SELECT_MATERIAL );
	if ( pButton->GetCheck() )		m_pTilingEff->SetSelectDraw ( TRUE );
	else							m_pTilingEff->SetSelectDraw ( FALSE );
}

void CTilingEffEdit::OnBnClickedButtonTileDraw()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->SetCurType ( _CUR_TILE_DRAW );
}

void CTilingEffEdit::OnBnClickedButtonTileDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->SetCurType ( _CUR_TILE_DEL );
}

void CTilingEffEdit::OnBnClickedButtonGetmaterialD()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->SetCurType ( _CUR_TILE_GET );
}

void CTilingEffEdit::OnBnClickedButtonGetmaterialF()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		// List �� ����
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

				pView->SetCurType ( _CUR_TILE_SELECT_TEX );			// �׸��°� ��� ���

                spCommand1->SetMaterial( pView->GetD3DDevice(), &m_pTilingEff->m_mapMaterials );
                spCommand2->SelectMaterial( m_pTilingEff->GetCurMaterial() );
                m_sCommand.Insert( spCommand );
			}
			else		// �������� �ִ�.
			{
				DWORD dwCurMaterial = m_pTilingEff->GetCurMaterial();
				int nSize = m_ListMaterials.GetCount();
				for ( int i=0; i<nSize; ++i )
				{
					if ( dwCurMaterial == m_ListMaterials.GetItemData(i) )
					{
						m_ListMaterials.SetCurSel ( i );		    // ������. !!
						pView->SetCurType ( _CUR_TILE_SELECT_TEX );	// �׸��°� ��� ���
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

			// List �� ����
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
                    // �ؽ��ĸ� �ٲ۴�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex, nData;
	nIndex = m_ListMaterials.GetCurSel ();
	if ( nIndex == -1 )	return;
	nData = m_ListMaterials.GetItemData ( nIndex );

    std::tr1::shared_ptr<CommandTiling_SelectMaterial> spCommand( new CommandTiling_SelectMaterial( m_pTilingEff->GetCurMaterial() ) );
    {
	    DxEffectTiling::MATERIALMAP_ITER iter = m_pTilingEff->m_mapMaterials.find(nData);
	    if ( iter!=m_pTilingEff->m_mapMaterials.end() )
	    {
		    m_pTilingEff->SetCurMaterial ( nData );		// Material �ٲ�
	    }
    }
    spCommand->SelectMaterial( m_pTilingEff->GetCurMaterial() );
    m_sCommand.Insert( spCommand );
}

void CTilingEffEdit::OnBnClickedButtonColor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

		if ( cColor.r>1.f )	cColor.r = 1.f;	// ���� ����
		if ( cColor.r<0.f )	cColor.r = 0.f;	// ���� ����
		if ( cColor.g>1.f )	cColor.g = 1.f;	// ���� ����
		if ( cColor.g<0.f )	cColor.g = 0.f;	// ���� ����
		if ( cColor.b>1.f )	cColor.b = 1.f;	// ���� ����
		if ( cColor.b<0.f )	cColor.b = 0.f;	// ���� ����

		m_pTilingEff->SetCurColor ( cColor );
	}
}

void CTilingEffEdit::OnBnClickedButtonColorRandom()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

		if ( cColor.a>1.f )	cColor.a = 1.f;	// ���� ����
		if ( cColor.a<0.f )	cColor.a = 0.f;	// ���� ����
		if ( cColor.r>1.f )	cColor.r = 1.f;	// ���� ����
		if ( cColor.r<0.f )	cColor.r = 0.f;	// ���� ����
		if ( cColor.g>1.f )	cColor.g = 1.f;	// ���� ����
		if ( cColor.g<0.f )	cColor.g = 0.f;	// ���� ����
		if ( cColor.b>1.f )	cColor.b = 1.f;	// ���� ����
		if ( cColor.b<0.f )	cColor.b = 0.f;	// ���� ����

		m_pTilingEff->SetColorRandom ( cColor );
	}
}

void CTilingEffEdit::OnEnChangeEditAlpha()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString	strText;
	CWnd* pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strText );
	float fAlpha = (float)atof ( strText.GetString() );

	if ( fAlpha>1.f )	fAlpha = 1.f;	// ���� ����
	if ( fAlpha<0.f )	fAlpha = 0.f;	// ���� ����

	D3DXCOLOR cColor = m_pTilingEff->GetCurColor();
	cColor.a = fAlpha;
	m_pTilingEff->SetCurColor ( cColor );
}

void CTilingEffEdit::OnBnClickedCheckTexDraw()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_TEX_DRAW );
	if ( pButton->GetCheck() )	m_sProperty.m_dwFlag |= USE_TEX_DRAW;
	else						m_sProperty.m_dwFlag &= ~USE_TEX_DRAW;

	m_pTilingEff->SetProperty ( m_sProperty );
}

void CTilingEffEdit::OnBnClickedCheckColorDraw()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_COLOR_DRAW );
	if ( pButton->GetCheck() )	m_sProperty.m_dwFlag |= USE_COLOR_DRAW;
	else						m_sProperty.m_dwFlag &= ~USE_COLOR_DRAW;

	m_pTilingEff->SetProperty ( m_sProperty );
}
void CTilingEffEdit::OnBnClickedButtonColorDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

    BackUpTileMaterial();

	m_pTilingEff->DeleteColorALL();

    std::tr1::shared_ptr<CommandTiling_TileChange> spCommand( new CommandTiling_TileChange(m_dwPointEX,&m_matMaterials_BackUp,&m_pTilingEff->m_mapMaterials) );
    m_sCommand.Insert( spCommand );
}

void CTilingEffEdit::OnNMCustomdrawSliderPriority(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString str;
	str.Format( "�켱���� - %d", m_slider_Priority.GetPos() );
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

        // Edit ���� ����
        UpdatePage ();
    }
    else if ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_DOWNED &&
                DxInputDevice::GetInstance().GetKeyState( DIK_Y ) & DXKEY_UP )
    {
        // Redo
        m_sCommand.Redo( pView->GetD3DDevice(), m_pTilingEff );

        // Edit ���� ����
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
                // ���������� �����ϵ��� ����.
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

				    CWorldEditView::m_CurType = _CUR_TILE_DRAW;		// ������ ������ �׸��� ������ ����
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

    // �ؽ��ĸ� �����ߴٸ� ���콺�� �ƹ��͵� �������� ���� ���°� �ɶ� ���� ��ٸ���.
    if ( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_IDLE )
    {
        if ( CWorldEditView::m_CurType == _CUR_TILE_SELECT_TEX )
        {
            pView->SetCurType ( _CUR_TILE_DRAW );	// �׸��� ���·� ����
        }
    }
}

BOOL CTilingEffEdit::OnKillActive()
{
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	

	int nIndex;
	nIndex = m_ListMaterials.GetCurSel ();
	if ( nIndex == -1 )	
		return;

	// �ֻ���̶� Change �Ұ����ϴ�.
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

	// ������ ��ġ�� �ٲ��ش�.
	m_ListMaterials.SetCurSel( nIndex-1 );

	// Update Page
	UpdatePage ();

	// �����ϸ� �۾��ؾ� �ϴ� ���� ���ش�. Undo�� �ʿ��� �۾��� ���õǾ�����.
	OnLbnSelchangeListMaterials();
}

void CTilingEffEdit::OnBnClickedButtonMaterialDown()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex;
	nIndex = m_ListMaterials.GetCurSel ();
	if ( nIndex == -1 )	
		return;

	// ���ϴ��̶� Change �Ұ����ϴ�.
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

	// ������ ��ġ�� �ٲ��ش�.
	m_ListMaterials.SetCurSel( nIndex+1 );

	// Update Page
	UpdatePage ();

	// �����ϸ� �۾��ؾ� �ϴ� ���� ���ش�. Undo�� �ʿ��� �۾��� ���õǾ�����.
	OnLbnSelchangeListMaterials();
}
