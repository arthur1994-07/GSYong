// PieceTexEffEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SheetWithTab.h"

#include "../VisualMaterialLib/Manager/VisualMaterialFX.h"
#include "../VisualMaterialLib/Manager/DxVMManager.h"

#include "./TexDlg.h"
#include "./TexMultiDlg.h"
#include "./PiecePage/TexVisualMaterialDlg.h"
#include "../EngineLib/DxEffect/TexEff/DxTexEffMan.h"
#include "../EngineLib/DxEffect/TexEff/DxTextureEffMan.h"
#include "../EngineLib/DxEffect/TexEff/DxTexEffDiffuse.h"
#include "../EngineLib/DxEffect/TexEff/DxTexEffFlowUV.h"
#include "../EngineLib/DxEffect/TexEff/DxTexEffRotate.h"
#include "../EngineLib/DxEffect/TexEff/DxTexEffSpecular.h"
#include "../EngineLib/DxEffect/TexEff/DxTexEffVisualMaterial.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"

#include "PieceTexEffEdit.h"

// CPieceTexEffEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPieceTexEffEdit, CPropertyPage)
CPieceTexEffEdit::CPieceTexEffEdit()
	: CPropertyPage(CPieceTexEffEdit::IDD)
{
}

CPieceTexEffEdit::~CPieceTexEffEdit()
{
}

void CPieceTexEffEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEXVIEW, m_listTexView);
	DDX_Control(pDX, IDC_COMBO_TEXEFF, m_comboTexEff);
	DDX_Control(pDX, IDC_LIST_TEXEFF, m_listTexEff);
}


BEGIN_MESSAGE_MAP(CPieceTexEffEdit, CPropertyPage)
	ON_BN_CLICKED(IDC_RADIO_DEFAULT, OnBnClickedRadioDefault)
	ON_BN_CLICKED(IDC_RADIO_OPACITY, OnBnClickedRadioOpacity)
	ON_BN_CLICKED(IDC_RADIO_HARD_ALPHA, OnBnClickedRadioHardAlpha)
	ON_BN_CLICKED(IDC_RADIO_SOFT_ALPHA, OnBnClickedRadioSoftAlpha)
	ON_BN_CLICKED(IDC_RADIO_SOFT_ALPHA2, OnBnClickedRadioSoftAlpha2)
	ON_BN_CLICKED(IDC_RADIO_SOFT_ALPHA3, OnBnClickedRadioSoftAlpha3)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TEXVIEW, OnLvnItemchangedListTexview)
	ON_EN_CHANGE(IDC_EDIT_POWER, OnEnChangeEditPower)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_STATE, OnBnClickedButtonState)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
    ON_BN_CLICKED(IDC_CHECK_DMSLM, &CPieceTexEffEdit::OnBnClickedCheckDmslm)
	ON_BN_CLICKED(IDC_CHECK_DONT_LM_S_CAST, &CPieceTexEffEdit::OnBnClickedCheckDontLmSCast)
END_MESSAGE_MAP()


// CPieceTexEffEdit 메시지 처리기입니다.
BOOL CPieceTexEffEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CButton* pButton;

	pButton = (CButton*)GetDlgItem( IDC_RADIO_DEFAULT );
	pButton->SetCheck( TRUE );

	RECT listRect;
	m_listTexView.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[3] = { int(nListWidth*0.5f), int(nListWidth*0.5f), int(nListWidth*0.2f) };

	m_listTexView.SetExtendedStyle ( m_listTexView.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_listTexView.InsertColumn ( 0, _T("Name"),	LVCFMT_CENTER, nColWidth[0] );
	m_listTexView.InsertColumn ( 1, _T("VM"),	LVCFMT_CENTER, nColWidth[1] );
	m_listTexView.InsertColumn ( 2, _T("DXT"),	LVCFMT_CENTER, nColWidth[2] );

	//	Note : 효과 타입들을 등록.
	DxTexEffType *pCurTexEffype = DxTexEffMan::GetInstance().GetTexEffList();
	while( pCurTexEffype )
	{
		int nIndex = m_comboTexEff.AddString ( pCurTexEffype->NAME );
		m_comboTexEff.SetItemData ( nIndex, pCurTexEffype->TYPEID );

		pCurTexEffype = pCurTexEffype->pNextType;
	}

	// Note : List 재 설정
	ReSetListCtrl();

	// Note : 현 Tex의 Effect List Reset
	ReSetListEffects();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CPieceTexEffEdit::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// Note : List 재 설정
	ReSetListCtrl();

	// Note : 현 Tex의 Effect List Reset
	ReSetListEffects();

	return CPropertyPage::OnSetActive();
}

void CPieceTexEffEdit::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_pSheetTab->SetActiveBackUpPage( PIECETEXEFFINDEX );
}

void CPieceTexEffEdit::OnLvnItemchangedListTexview(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	// Note : 현 Tex의 Effect List Reset
	ReSetListEffects();

	{
		DxTextureEffMan* pTextureEff = GetTextureEffMan();
		if( !pTextureEff )	
			return;

		POSITION pos = m_listTexView.GetFirstSelectedItemPosition();
		if( pos==NULL )
		{
			return;
		}

		int nItem = m_listTexView.GetNextSelectedItem( pos );
		CString strName = m_listTexView.GetItemText( nItem, 0 );

		CButton* pButton;
		pButton = (CButton*)GetDlgItem( IDC_RADIO_DEFAULT );
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_OPACITY );
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_HARD_ALPHA );
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_SOFT_ALPHA );
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_SOFT_ALPHA2 );
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_SOFT_ALPHA3 );
		pButton->SetCheck( FALSE );

		const TextureData* pTextureData = pTextureEff->GetTextureData( strName.GetString() );
		switch ( pTextureData->m_dwTexType )
		{
		case TEXEFF_DEFAULT:
			pButton = (CButton*)GetDlgItem( IDC_RADIO_DEFAULT );
			pButton->SetCheck( TRUE );
			break;

		case TEXEFF_OPAQUE:
			pButton = (CButton*)GetDlgItem( IDC_RADIO_OPACITY );
			pButton->SetCheck( TRUE );
			break;

		case TEXEFF_HARDALPHA:
			pButton = (CButton*)GetDlgItem( IDC_RADIO_HARD_ALPHA );
			pButton->SetCheck( TRUE );
			break;

		case TEXEFF_SOFTALPHA:
			pButton = (CButton*)GetDlgItem( IDC_RADIO_SOFT_ALPHA );
			pButton->SetCheck( TRUE );
			break;

		case TEXEFF_SOFTALPHA_1:
			pButton = (CButton*)GetDlgItem( IDC_RADIO_SOFT_ALPHA2 );
			pButton->SetCheck( TRUE );
			break;

		case TEXEFF_SOFTALPHA_2:
			pButton = (CButton*)GetDlgItem( IDC_RADIO_SOFT_ALPHA3 );
			pButton->SetCheck( TRUE );
			break;
		};
	}
}

DxTextureEffMan* CPieceTexEffEdit::GetTextureEffMan()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxTextureEffMan* pTextureEff = NULL;

	if( pView->m_ViewType==FRAME_VIEW )
	{
		DxFrameMesh* pFrameMesh = pView->GetFrameMesh();
		if( !pFrameMesh )
		{
			//MessageBox( "pFrameMesh Point NULL, X-File Load" );
			GASSERT( 0 && "pFrameMesh Point NULL, X-File Load" );
			return NULL;
		}

		pTextureEff = pFrameMesh->GetTextureEffMan();
		if( !pTextureEff )
		{
			//MessageBox( "DxTextureEffMan Pointer NULL" );
			GASSERT( 0 && "DxTextureEffMan Pointer NULL" );
			return NULL;
		}
	}
	else if( pView->m_ViewType==FRAMEMESH_VIEW )
	{
		DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
		if( !pPieceEdit )
		{
			//MessageBox( "Please.. X-File Load" );
			GASSERT( 0 && "Please.. X-File Load" );
			return NULL;
		}

		DxFrameMesh* pFrameMesh = pPieceEdit->GetFrameMesh();
		if( !pFrameMesh )
		{
			//MessageBox( "pFrameMesh Point NULL, X-File Load" );
			GASSERT( 0 && "pFrameMesh Point NULL, X-File Load" );
			return NULL;
		}

		pTextureEff = pFrameMesh->GetTextureEffMan();
		if( !pTextureEff )
		{
			//MessageBox( "DxTextureEffMan Pointer NULL" );
			GASSERT( 0 && "DxTextureEffMan Pointer NULL" );
			return NULL;
		}
	}
	else
	{
		//MessageBox( "Please.. View .. < Frame Or FrameMesh >" );
		GASSERT( 0 && "Please.. View .. < Frame Or FrameMesh >" );
		return NULL;
	}

	return pTextureEff;
}

void CPieceTexEffEdit::OnEnChangeEditPower()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )	return;

	POSITION pos = m_listTexView.GetFirstSelectedItemPosition();
	
	int nItem = m_listTexView.GetNextSelectedItem( pos );
	if ( nItem == -1 )
	{
		return;
	}
}

void CPieceTexEffEdit::ReSetListCtrl()
{
	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )	return;

	m_listTexView.DeleteAllItems ();

	DWORD dwFlag;
    DWORD dwFlagEff;
	LPCSTR szName = NULL;
	std::string szNameTYPE;
    TSTRING strVM;
	for ( int i=0; i<pTextureEff->GetTextureListNUM(); ++i )
	{
		if( pTextureEff->IsGetTextureEFFDXT( i, szName, dwFlag, dwFlagEff, strVM ) )
		{
			m_listTexView.InsertItem( i, szName );

			// Note : Eff 설정
			m_listTexView.SetItemText( i, 1, strVM.c_str() );

			// Note : DXT 설정
			DWORD dwDXT = dwFlag&TEXEFF_DXT;
			if( dwDXT==TEXEFF_DEFAULT )				szNameTYPE = "Default";
			else if( dwDXT==TEXEFF_OPAQUE )			szNameTYPE = "DXT1";
			else if( dwDXT==TEXEFF_HARDALPHA )		szNameTYPE = "DXT1_a";
			else if( dwDXT==TEXEFF_SOFTALPHA )		szNameTYPE = "DXT5_a";
			else if( dwDXT==TEXEFF_SOFTALPHA_1 )	szNameTYPE = "DXT5_a1";
			else if( dwDXT==TEXEFF_SOFTALPHA_2 )	szNameTYPE = "DXT5_a2";
			m_listTexView.SetItemText( i, 2, szNameTYPE.c_str() );
		}
		else
		{
			MessageBox("Code ERROR. Please. Call Me to Sung-Hwan Han");
			return;
		}
	}		
}

void CPieceTexEffEdit::ChangeListCtrlDXT( int nIndex, DWORD dwFlag )
{
	std::string szNameTYPE;
	if( dwFlag&TEXEFF_DXT )
	{
		if( dwFlag==TEXEFF_DEFAULT )			szNameTYPE = "Default";
		else if( dwFlag==TEXEFF_OPAQUE )		szNameTYPE = "DXT1";
		else if( dwFlag==TEXEFF_HARDALPHA )		szNameTYPE = "DXT1_a";
		else if( dwFlag==TEXEFF_SOFTALPHA )		szNameTYPE = "DXT5_a";
		else if( dwFlag==TEXEFF_SOFTALPHA_1 )	szNameTYPE = "DXT5_a1";
		else if( dwFlag==TEXEFF_SOFTALPHA_2 )	szNameTYPE = "DXT5_a2";
		m_listTexView.SetItemText( nIndex, 2, szNameTYPE.c_str() );
	}
}

void CPieceTexEffEdit::SetDXTFlag( DWORD dwFlag )
{
	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )	return;

	POSITION pos = m_listTexView.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( "Please. Check Point" );
		return;
	}
	
	int nItem = m_listTexView.GetNextSelectedItem( pos );
	CString strName = m_listTexView.GetItemText( nItem, 0 );

	pTextureEff->SetDXTFlag( strName.GetString(), dwFlag );

	ChangeListCtrlDXT( nItem, dwFlag );
}

void CPieceTexEffEdit::OnBnClickedRadioDefault()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetDXTFlag( TEXEFF_DEFAULT );
}

void CPieceTexEffEdit::OnBnClickedRadioOpacity()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetDXTFlag( TEXEFF_OPAQUE );
}

void CPieceTexEffEdit::OnBnClickedRadioHardAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetDXTFlag( TEXEFF_HARDALPHA );
}

void CPieceTexEffEdit::OnBnClickedRadioSoftAlpha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetDXTFlag( TEXEFF_SOFTALPHA );
}

void CPieceTexEffEdit::OnBnClickedRadioSoftAlpha2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetDXTFlag( TEXEFF_SOFTALPHA_1 );
}

void CPieceTexEffEdit::OnBnClickedRadioSoftAlpha3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetDXTFlag( TEXEFF_SOFTALPHA_2 );
}

void CPieceTexEffEdit::OnBnClickedCheckDmslm()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    std::string strName;
	if( !GetCheckTexView( strName ) )	
        return;

    DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )	
        return;

    const TextureData* pTextureData = pTextureEff->GetTextureData( strName.c_str() );
    if ( pTextureData == NULL )
        return;

    // 기본 데이터를 셋팅함.
    TextureData sTextureData = *pTextureData;

    // 수치를 알맞게 기입함.
    CButton* pButton = (CButton*) GetDlgItem( IDC_CHECK_DMSLM );
    if ( pButton->GetCheck() )
    {
        sTextureData.m_dwPieceLM_Option |= EMPLO_DONT_LM_SHADOW_RECEIVE;
    }
    else
    {
        sTextureData.m_dwPieceLM_Option &= ~EMPLO_DONT_LM_SHADOW_RECEIVE;
    }

    // 데이터를 셋팅함.
    pTextureEff->SetTextureData( strName.c_str(), sTextureData );
}

void CPieceTexEffEdit::OnBnClickedCheckDontLmSCast()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	std::string strName;
	if( !GetCheckTexView( strName ) )	
		return;

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )	
		return;

	const TextureData* pTextureData = pTextureEff->GetTextureData( strName.c_str() );
	if ( pTextureData == NULL )
		return;

	// 기본 데이터를 셋팅함.
	TextureData sTextureData = *pTextureData;

	// 수치를 알맞게 기입함.
	CButton* pButton = (CButton*) GetDlgItem( IDC_CHECK_DONT_LM_S_CAST );
	if ( pButton->GetCheck() )
	{
		sTextureData.m_dwPieceLM_Option |= EMPLO_DONT_LM_SHADOW_CAST;
	}
	else
	{
		sTextureData.m_dwPieceLM_Option &= ~EMPLO_DONT_LM_SHADOW_CAST;
	}

	// 데이터를 셋팅함.
	pTextureEff->SetTextureData( strName.c_str(), sTextureData );
}

/*

BOOL DxTextureEffMan::SetTextureData( const TSTRING& strName, const TextureData& sTextureData )
{
    iter = m_mapTexList.find( strName );
	if( iter==m_mapTexList.end() )	
        return FALSE;	// ERROR
	
    (*iter).second = sTextureData;

	return TRUE;
}

const TextureData* DxTextureEffMan::GetTextureData( const TSTRING& strName )
{
	iter = m_mapTexList.find( strName );
	if( iter==m_mapTexList.end() )	
        return NULL;

    return &(*iter).second;
}
*/

void CPieceTexEffEdit::ReSetListEffects()
{
	m_listTexEff.ResetContent ();

	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )	return;
	DxTexEffBase* pProp = pTextureEff->GetEffRoot( strName.c_str() );

	int nIndex;
	while( pProp )
	{
		nIndex = m_listTexEff.AddString( pProp->GetName() );
		m_listTexEff.SetItemData( nIndex, (DWORD_PTR)pProp );

		pProp = pProp->m_pNext;
	}

    const TextureData* pTextureData = pTextureEff->GetTextureData( strName.c_str() );
    if ( pTextureData )
    {
        CButton* pButton = (CButton*) GetDlgItem( IDC_CHECK_DMSLM );
        pButton->SetCheck( pTextureData->m_dwPieceLM_Option & EMPLO_DONT_LM_SHADOW_RECEIVE );

		pButton = (CButton*) GetDlgItem( IDC_CHECK_DONT_LM_S_CAST );
		pButton->SetCheck( pTextureData->m_dwPieceLM_Option & EMPLO_DONT_LM_SHADOW_CAST );
    }
}

BOOL CPieceTexEffEdit::GetCheckTexView( std::string& strName )
{
	POSITION pos = m_listTexView.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		//MessageBox( "Please. Check Point" );
		return FALSE;
	}

	int nItem = m_listTexView.GetNextSelectedItem( pos );
	if( nItem == -1 )
	{
		MessageBox( "ERROR : GetNextSelectedItem()" );
		return FALSE;
	}

	strName = m_listTexView.GetItemText( nItem, 0 );

	return TRUE;
}

void CPieceTexEffEdit::SelectTexEffList( const TSTRING& strName )
{
    DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )	return;

	DWORD dwFlag;
    DWORD dwFlagEff;
	LPCSTR szName = NULL;
    TSTRING strVM;
	for ( int i=0; i<pTextureEff->GetTextureListNUM(); ++i )
	{
		if( pTextureEff->IsGetTextureEFFDXT( i, szName, dwFlag, dwFlagEff, strVM ) )
		{
            if ( strName == szName )
            {
                // Note : 셋팅.~!
                m_listTexView.SetSelectionMark( i );							// 실제 선택됨.
	            m_listTexView.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED);	// 회색으로 표시됨.
	            m_listTexView.EnsureVisible( i, FALSE );						// 스크롤 밑에 있으면 옮겨줌.
	            m_listTexView.SetFocus();										// 푸른색으로 표시됨.

                return;
            }
		}
	}
}

//------------------------------------------------------------------------------------------
//									C	r	e	a	t	e
//------------------------------------------------------------------------------------------
void CPieceTexEffEdit::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	//	Note : 선택된 효과 TYPE.
	DWORD nIndex = m_comboTexEff.GetCurSel ();
	if ( nIndex == -1 ) return;
	DWORD dwTexEffType = m_comboTexEff.GetItemData( nIndex );

	//	Note : 셋팅을 하는 Dialog를 띄운다.
	CreateSettingDialog( dwTexEffType );
}

void CPieceTexEffEdit::CreateSettingDialog( const DWORD dwType )
{
	switch( dwType )
	{
	case DEF_TEXEFF_DIFFUSE:
		CreateDiffuseDlg( dwType );
		break;
	case DEF_TEXEFF_FLOWUV:
		CreateFlowUVDlg( dwType );
		break;
	case DEF_TEXEFF_ROTATE:
		CreateRotateDlg( dwType );
		break;
	case DEF_TEXEFF_SPECULAR:
		CreateSpecularDlg( dwType );
		break;
    case DEF_TEXEFF_VISUAL_MATERIAL:
		CreateVisualMaterialDlg( dwType );
		break;
	}
}

void CPieceTexEffEdit::CreateDiffuseDlg( const DWORD dwType )
{
	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_DIFFUSE_PROPERTY	sProp;
	CTexDlg sDlg;
	sDlg.m_emMode = EMTEDIFFUSE;
	sDlg.m_bSpeed = sProp.bSpeed;
	sDlg.m_fSpeed = sProp.fSpeed;
	sDlg.m_fAlpha = sProp.fAlpha;
	sDlg.m_strBaseTex = strName.c_str();
	sDlg.m_strAlphaTex = sProp.strTex.c_str();
	sDlg.m_bNightLight = sProp.bNightLight;

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 값을 넣는다.
		sProp.bSpeed = sDlg.m_bSpeed;
		sProp.fSpeed = sDlg.m_fSpeed;
		sProp.fAlpha = sDlg.m_fAlpha;
		sProp.strTex = sDlg.m_strAlphaTex.c_str();
		sProp.bNightLight = sDlg.m_bNightLight;

		// Note : 효과를 생성한다.
		CreateDiffuse( dwType, strName.c_str(), sProp );
	}
}

void CPieceTexEffEdit::CreateDiffuse( const DWORD dwType, const char* pName, const TEXEFF_DIFFUSE_PROPERTY& sProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
	DxTexEffBase* pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwType, pView->GetD3DDevice(), (PBYTE)&sProp );

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	pTextureEff->InsertTextureEff( *pTextureEff->GetMapTexEff(), pName, pTexEffBase );

	ReSetListEffects();
}

void CPieceTexEffEdit::CreateFlowUVDlg( const DWORD dwType )
{
	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_FLOWUV_PROPERTY	sProp;
	CTexMultiDlg sDlg;
	sDlg.m_bNEWUV		= (BOOL)(sProp.dwFlag&TEXEFFFLOWUV_NEWUV);
	sDlg.m_bRandomSpeed	= (BOOL)(sProp.dwFlag&TEXEFFFLOWUV_RANDOMSPEED);
	sDlg.m_bModulate2X	= (BOOL)(sProp.dwFlag&TEXEFFFLOWUV_MODULATE2X);
	sDlg.m_fUVScale		= sProp.fScale;
	sDlg.m_fAlpha		= sProp.fAlpha;
	sDlg.m_fSpeedX		= sProp.vSpeed.x;
	sDlg.m_fSpeedY		= sProp.vSpeed.y;
	sDlg.m_strBaseTex	= strName.c_str();
	sDlg.m_strAlphaTex	= sProp.strAlphaTex.c_str();
	sDlg.m_strFlowTex	= sProp.strFlowTex.c_str();

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 데이터 셋팅
		if( sDlg.m_bNEWUV )			sProp.dwFlag |= TEXEFFFLOWUV_NEWUV;
		else						sProp.dwFlag &= ~TEXEFFFLOWUV_NEWUV;

		if( sDlg.m_bRandomSpeed )	sProp.dwFlag |= TEXEFFFLOWUV_RANDOMSPEED;
		else						sProp.dwFlag &= ~TEXEFFFLOWUV_RANDOMSPEED;

		if( sDlg.m_bModulate2X )	sProp.dwFlag |= TEXEFFFLOWUV_MODULATE2X;
		else						sProp.dwFlag &= ~TEXEFFFLOWUV_MODULATE2X;

		sProp.fAlpha	= sDlg.m_fAlpha;
		sProp.vSpeed	= D3DXVECTOR2( sDlg.m_fSpeedX, sDlg.m_fSpeedY );
		sProp.fScale	= sDlg.m_fUVScale;

		sProp.strAlphaTex = sDlg.m_strAlphaTex.c_str();
		sProp.strFlowTex = sDlg.m_strFlowTex.c_str();

		// Note : 효과를 생성한다.
		CreateFlowUV( dwType, strName.c_str(), sProp );
	}
}

void CPieceTexEffEdit::CreateFlowUV( const DWORD dwType, const char* pName, const TEXEFF_FLOWUV_PROPERTY& sProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
	DxTexEffBase* pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwType, pView->GetD3DDevice(), (PBYTE)&sProp );

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	pTextureEff->InsertTextureEff( *pTextureEff->GetMapTexEff(), pName, pTexEffBase );

	ReSetListEffects();
}

void CPieceTexEffEdit::CreateRotateDlg( const DWORD dwType )
{
	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_ROTATE_PROPERTY	sProp;
	CTexDlg sDlg;
	sDlg.m_emMode = EMTEDIFFUSE;
	sDlg.m_fSpeed = sProp.fSpeed;
	sDlg.m_strBaseTex = strName.c_str();
	sDlg.m_strAlphaTex = sProp.strTex.c_str();

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 값을 넣는다.
		sProp.fSpeed = sDlg.m_fSpeed;
		sProp.strTex = sDlg.m_strAlphaTex.c_str();

		// Note : 효과를 생성한다.
		CreateRotate( dwType, strName.c_str(), sProp );
	}
}

void CPieceTexEffEdit::CreateRotate( const DWORD dwType, const char* pName, const TEXEFF_ROTATE_PROPERTY& sProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
	DxTexEffBase* pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwType, pView->GetD3DDevice(), (PBYTE)&sProp );

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	pTextureEff->InsertTextureEff( *pTextureEff->GetMapTexEff(), pName, pTexEffBase );

	ReSetListEffects();
}

void CPieceTexEffEdit::CreateSpecularDlg( const DWORD dwType )
{
	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_SPECULAR_PROPERTY	sProp;
	CTexDlg sDlg;
	sDlg.m_emMode = EMTESPECULAR;
	sDlg.m_strBaseTex = strName.c_str();
	sDlg.m_strAlphaTex = sProp.strTex.c_str();

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 값을 넣는다.
		sProp.strTex = sDlg.m_strAlphaTex.c_str();

		// Note : 효과를 생성한다.
		CreateSpecular( dwType, strName.c_str(), sProp );
	}
}

void CPieceTexEffEdit::CreateSpecular( const DWORD dwType, const char* pName, const TEXEFF_SPECULAR_PROPERTY& sProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
	DxTexEffBase* pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwType, pView->GetD3DDevice(), (PBYTE)&sProp );

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	pTextureEff->InsertTextureEff( *pTextureEff->GetMapTexEff(), pName, pTexEffBase );

	ReSetListEffects();
}

void CPieceTexEffEdit::CreateVisualMaterialDlg( const DWORD dwType )
{
	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_VISUAL_MATERIAL_PROPERTY	sProp;
	CTexVisualMaterialDlg sDlg;
	sDlg.m_strFX = sProp.m_strVisualMaterial.c_str();
    sDlg.m_dwUsedTexture = 0;
    sDlg.m_mapParameter = sProp.m_mapParameter;

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 값을 넣는다.
        {
            sProp.m_mapParameter = sDlg.m_mapParameter;
		    sProp.m_strVisualMaterial = sDlg.m_strFX.c_str();

            sProp.m_vecTextureResource.clear();
            for ( DWORD i=0; i<sDlg.m_dwUsedTexture; ++i )
            {
                vm::NAME_TEXTURE sNameTexture;
                sNameTexture.m_strName = sDlg.m_strTextureDEST[i];
                sProp.m_vecTextureResource.push_back( sNameTexture );
            }
        }

		// Note : 효과를 생성한다.
		CreateVisualMaterial( dwType, strName.c_str(), sProp );
	}
}

void CPieceTexEffEdit::CreateVisualMaterial( const DWORD dwType, const char* pName, const TEXEFF_VISUAL_MATERIAL_PROPERTY& sProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
	DxTexEffBase* pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwType, pView->GetD3DDevice(), (PBYTE)&sProp );

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;

	pTextureEff->InsertTextureEff( *pTextureEff->GetMapTexEff(), pName, pTexEffBase );

	ReSetListEffects();
}

//------------------------------------------------------------------------------------------
//									S	t	a	t	e
//------------------------------------------------------------------------------------------
void CPieceTexEffEdit::OnBnClickedButtonState()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	//	Note : 선택된 효과 TYPE.
	DWORD nIndex = m_listTexEff.GetCurSel ();
	if ( nIndex == -1 ) return;
	DWORD dwTexEff = m_listTexEff.GetItemData( nIndex );
	
	//	Note : 셋팅을 하는 Dialog를 띄운다.
	StateSettingDialog( (DxTexEffBase*)dwTexEff );
}

void CPieceTexEffEdit::StateSettingDialog( DxTexEffBase* pTexEff )
{
	DWORD dwTypeID = pTexEff->GetTypeID();
	switch( dwTypeID )
	{
	case DEF_TEXEFF_DIFFUSE:
		StateDiffuseDlg( dwTypeID, pTexEff );
		break;
	case DEF_TEXEFF_FLOWUV:
		StateFlowUVDlg( dwTypeID, pTexEff );
		break;
	case DEF_TEXEFF_ROTATE:
		StateRotateDlg( dwTypeID, pTexEff );
		break;
	case DEF_TEXEFF_SPECULAR:
		StateSpecularDlg( dwTypeID, pTexEff );
		break;
    case DEF_TEXEFF_VISUAL_MATERIAL:
		StateVisualMaterialDlg( dwTypeID, pTexEff );
		break;
	}
}

void CPieceTexEffEdit::StateDiffuseDlg( const DWORD dwType, DxTexEffBase* pTexEff )
{
	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_DIFFUSE_PROPERTY	sProp = *((DxTexEffDiffuse*)pTexEff)->GetProperty();
	//sProp.pTex = NULL;

	CTexDlg sDlg;
	sDlg.m_emMode = EMTEDIFFUSE;
	sDlg.m_bSpeed = sProp.bSpeed;
	sDlg.m_fSpeed = sProp.fSpeed;
	sDlg.m_fAlpha = sProp.fAlpha;
	sDlg.m_strBaseTex = strName.c_str();
	sDlg.m_strAlphaTex = sProp.strTex.c_str();
	sDlg.m_bNightLight = sProp.bNightLight;

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 값을 넣는다.
		sProp.bSpeed = sDlg.m_bSpeed;
		sProp.fSpeed = sDlg.m_fSpeed;
		sProp.fAlpha = sDlg.m_fAlpha;
		sProp.strTex = sDlg.m_strAlphaTex.c_str();
		sProp.bNightLight = sDlg.m_bNightLight;

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
		PBYTE pByte = (PBYTE)&sProp;
		pTexEff->SetProperty( pView->GetD3DDevice(), pByte );

		ReSetListEffects();
	}
}

void CPieceTexEffEdit::StateFlowUVDlg( const DWORD dwType, DxTexEffBase* pTexEff )
{
	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_FLOWUV_PROPERTY	sProp = *((DxTexEffFlowUV*)pTexEff)->GetProperty();
	//sProp.pAlphaTex = NULL;
	//sProp.pFlowTex = NULL;

	CTexMultiDlg sDlg;
	sDlg.m_bNEWUV		= (BOOL)(sProp.dwFlag&TEXEFFFLOWUV_NEWUV);
	sDlg.m_bRandomSpeed	= (BOOL)(sProp.dwFlag&TEXEFFFLOWUV_RANDOMSPEED);
	sDlg.m_bModulate2X	= (BOOL)(sProp.dwFlag&TEXEFFFLOWUV_MODULATE2X);
	sDlg.m_fUVScale = sProp.fScale;
	sDlg.m_fAlpha	= sProp.fAlpha;
	sDlg.m_fSpeedX	= sProp.vSpeed.x;
	sDlg.m_fSpeedY	= sProp.vSpeed.y;
	sDlg.m_strBaseTex = strName.c_str();
	sDlg.m_strAlphaTex = sProp.strAlphaTex.c_str();
	sDlg.m_strFlowTex = sProp.strFlowTex.c_str();

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 데이터 셋팅
		if( sDlg.m_bNEWUV )			sProp.dwFlag |= TEXEFFFLOWUV_NEWUV;
		else						sProp.dwFlag &= ~TEXEFFFLOWUV_NEWUV;

		if( sDlg.m_bRandomSpeed )	sProp.dwFlag |= TEXEFFFLOWUV_RANDOMSPEED;
		else						sProp.dwFlag &= ~TEXEFFFLOWUV_RANDOMSPEED;

		if( sDlg.m_bModulate2X )	sProp.dwFlag |= TEXEFFFLOWUV_MODULATE2X;
		else						sProp.dwFlag &= ~TEXEFFFLOWUV_MODULATE2X;

		sProp.fAlpha	= sDlg.m_fAlpha;
		sProp.vSpeed	= D3DXVECTOR2( sDlg.m_fSpeedX, sDlg.m_fSpeedY );
		sProp.fScale	= sDlg.m_fUVScale;

		sProp.strAlphaTex = sDlg.m_strAlphaTex.c_str();
		sProp.strFlowTex = sDlg.m_strFlowTex.c_str();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
		PBYTE pByte = (PBYTE)&sProp;
		pTexEff->SetProperty( pView->GetD3DDevice(), pByte );

		ReSetListEffects();
	}
}

void CPieceTexEffEdit::StateRotateDlg( const DWORD dwType, DxTexEffBase* pTexEff )
{
	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_ROTATE_PROPERTY	sProp = *((DxTexEffRotate*)pTexEff)->GetProperty();
	//sProp.pTex = NULL;

	CTexDlg sDlg;
	sDlg.m_emMode = EMTEDIFFUSE;
	sDlg.m_fSpeed = sProp.fSpeed;
	sDlg.m_strBaseTex = strName.c_str();
	sDlg.m_strAlphaTex = sProp.strTex.c_str();

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 값을 넣는다.
		sProp.fSpeed = sDlg.m_fSpeed;
		sProp.strTex = sDlg.m_strAlphaTex.c_str();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
		PBYTE pByte = (PBYTE)&sProp;
		pTexEff->SetProperty( pView->GetD3DDevice(), pByte );

		ReSetListEffects();
	}
}

void CPieceTexEffEdit::StateSpecularDlg( const DWORD dwType, DxTexEffBase* pTexEff )
{
	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_SPECULAR_PROPERTY	sProp = *((DxTexEffSpecular*)pTexEff)->GetProperty();
	//sProp.pTex = NULL;

	CTexDlg sDlg;
	sDlg.m_emMode = EMTESPECULAR;
	sDlg.m_strBaseTex = strName.c_str();
	sDlg.m_strAlphaTex = sProp.strTex.c_str();

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 값을 넣는다.
		sProp.strTex = sDlg.m_strAlphaTex.c_str();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
		PBYTE pByte = (PBYTE)&sProp;
		pTexEff->SetProperty( pView->GetD3DDevice(), pByte );

		ReSetListEffects();
	}
}

void CPieceTexEffEdit::StateVisualMaterialDlg( const DWORD dwType, DxTexEffBase* pTexEff )
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	TEXEFF_VISUAL_MATERIAL_PROPERTY	sProp;
    sProp.CloneDummy( *((DxTexEffVisualMaterial*)pTexEff)->GetProperty() );

	CTexVisualMaterialDlg sDlg;
	sDlg.m_strFX = sProp.m_strVisualMaterial.c_str();
    sDlg.m_mapParameter = sProp.m_mapParameter;

    // 초기화
    for ( DWORD i=0; i<CTexVisualMaterialDlg::MAX_USE_TEXTURE_VM; ++i )
    {
        sDlg.m_strTextureSRC[i] = _T("");
        sDlg.m_strTextureDEST[i] = _T("");
    }

    // 값 대입
    std::tr1::shared_ptr<VisualMaterialFX> spVMFX = NS_VM_MANAGER::Load( pView->GetD3DDevice(), sProp.m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT );

    for ( DWORD i=0; i<spVMFX->GetTextureSize(); ++i )
    {
        if ( sProp.m_spVMFX->GetTextureName( i ) )
        {
            sDlg.m_strTextureSRC[i] = sProp.m_spVMFX->GetTextureName( i );
        }
        else
        {
            sDlg.m_strTextureSRC[i] = _T("");
        }
    }
    for ( DWORD i=0; i<sProp.m_vecTextureResource.size(); ++i )
    {
        sDlg.m_strTextureDEST[i] = sProp.m_vecTextureResource[i].m_strName.c_str();
    }

    // size 설정.
    sDlg.m_dwUsedTexture = spVMFX->GetTextureSize();

	if( sDlg.DoModal() == IDOK )
	{
		// Note : 값을 넣는다.
        {
            sProp.m_mapParameter = sDlg.m_mapParameter;
		    sProp.m_strVisualMaterial = sDlg.m_strFX.c_str();

            sProp.m_vecTextureResource.clear();
            for ( DWORD i=0; i<sDlg.m_dwUsedTexture; ++i )
            {
                vm::NAME_TEXTURE sNameTexture;
                sNameTexture.m_strName = sDlg.m_strTextureDEST[i];
                sProp.m_vecTextureResource.push_back( sNameTexture );
            }
        }

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView();
		PBYTE pByte = (PBYTE)&sProp;
		pTexEff->SetProperty( pView->GetD3DDevice(), pByte );

		ReSetListEffects();
	}
}

//------------------------------------------------------------------------------------------
//									D	e	l	e	t	e
//------------------------------------------------------------------------------------------
void CPieceTexEffEdit::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	std::string strName;
	if( !GetCheckTexView( strName ) )	return;

	//	Note : 선택된 효과 TYPE.
	DWORD nIndex = m_listTexEff.GetCurSel ();
	if ( nIndex == -1 ) return;
	DWORD dwTexEff = m_listTexEff.GetItemData( nIndex );

	DxTextureEffMan* pTextureEff = GetTextureEffMan();
	if( !pTextureEff )		return;
	pTextureEff->DeleteTexEff( strName.c_str(), (DxTexEffBase*)dwTexEff );

	ReSetListEffects();
}

