// EffBoneListEffPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../Dialog/ColorPickerDlg.h"
#include "SheetWithTabChar.h"

#include "EffBoneListEffPage.h"
#include ".\effbonelisteffpage.h"

#ifdef DEF_SKIN_SYNC
#include "../EngineLib/DxEffect/Char/DxEffCharBonePosEff.h"
#endif

extern std::string	g_strPICK_BONE;

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffBoneListEffPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffBoneListEffPage, CPropertyPage)
CEffBoneListEffPage::CEffBoneListEffPage()
	: CEffCharPage(CEffBoneListEffPage::IDD)
{
	D3DXMatrixIdentity( &m_matWorld );
}

CEffBoneListEffPage::~CEffBoneListEffPage()
{
}

void CEffBoneListEffPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BONE, m_listBone);
}


BEGIN_MESSAGE_MAP(CEffBoneListEffPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_CHECK_POS_MOVE, OnBnClickedCheckPosMove)
	ON_BN_CLICKED(IDC_CHECK_SEQUENCE, OnBnClickedCheckSequence2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_FIND, OnBnClickedButtonTextureFind)
	ON_LBN_SELCHANGE(IDC_LIST_BONE, OnLbnSelchangeListBone)
	ON_BN_CLICKED(IDC_CHECK_ROTATE, OnBnClickedCheckRotate)
	ON_BN_CLICKED(IDC_CHECK_REMAIN, OnBnClickedCheckRemain)
    ON_BN_CLICKED(IDC_CHECK_CREATE_RANDOM_DONT_MOVE, &CEffBoneListEffPage::OnBnClickedCheckCreateRandomDontMove)
END_MESSAGE_MAP()


// CEffBoneListEffPage 메시지 처리기입니다.
void CEffBoneListEffPage::SetProperty( EFFCHAR_PROPERTY *pProp )
{
	if( pProp )	m_Property = *((EFFCHAR_PROPERTY_BONELISTEFF*)pProp);
	else
	{
		EFFCHAR_PROPERTY_BONELISTEFF sProp;
		m_Property = sProp;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CWnd*		pWnd;
	CButton*	pButton;
	char		szValue[256];

	// Note : 
	pButton = (CButton*)GetDlgItem(IDC_CHECK_REMAIN);
	if( m_Property.m_dwFlag&USE_REMAIN )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE);
	if( m_Property.m_dwFlag&USE_SEQUENCE )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem(IDC_CHECK_ROTATE);
	if( m_Property.m_dwFlag&USE_ROTATE )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

    pButton = (CButton*)GetDlgItem(IDC_CHECK_CREATE_RANDOM_DONT_MOVE);
	if( m_Property.m_dwFlag&USE_CREATERANDOM_DONTMOVE )	pButton->SetCheck( TRUE );
	else										        pButton->SetCheck( FALSE );


	pWnd = GetDlgItem( IDC_EDIT_NUM );
	_gcvt( m_Property.m_dwParticlePec, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_CENTER_FAR );
	_gcvt( m_Property.m_fCreateLength, 7, szValue );
	pWnd->SetWindowText( szValue );	

	// Note : Life
	pWnd = GetDlgItem( IDC_EDIT_LIFE_MIN );
	_gcvt( m_Property.m_vLife.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LIFE_MAX );
	_gcvt( m_Property.m_vLife.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Scale
	pWnd = GetDlgItem( IDC_EDIT_SCALE_MIN );
	_gcvt( m_Property.m_vSize.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_MAX );
	_gcvt( m_Property.m_vSize.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_RATE );
	_gcvt( m_Property.m_vSize.z, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Sequece
	pWnd = GetDlgItem( IDC_EDIT_COL );
	_gcvt( m_Property.m_dwSequenceCol, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_ROW );
	_gcvt( m_Property.m_dwSequenceRow, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Rotate
	pWnd = GetDlgItem( IDC_EDIT_ROTATE_MIN );
	_gcvt( m_Property.m_vRotate.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_ROTATE_MAX );
	_gcvt( m_Property.m_vRotate.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Velocity
	pWnd = GetDlgItem( IDC_EDIT_VEL_MIN );
	_gcvt( m_Property.m_vVelocity.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_VEL_MAX );
	_gcvt( m_Property.m_vVelocity.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Texture
	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText( m_Property.m_szTexture );


	if ( !m_pEffChar )
	{
		m_pEffChar = (DxEffCharBoneListEff*)DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharBoneListEff::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( "이팩트 생성에 실패하였습니다.", "ERROR", MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	// Note : 선택된 Bone의 옵션에서 bVertex의 상태가 어떠한가 ?
	pButton = (CButton*)GetDlgItem( IDC_CHECK_POS_MOVE );
	if( pEffChar->GetEnableBonePos() )	pButton->SetCheck( TRUE );
	else									pButton->SetCheck( FALSE );

	// Note : Local
	D3DXVECTOR3 vPos = pEffChar->GetInfluencePosition();
	pWnd = GetDlgItem( IDC_EDIT_LOCAL_X );
	_gcvt( vPos.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Y );
	_gcvt( vPos.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Z );
	_gcvt( vPos.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : 커서를 쓰기 위한 작업
	D3DXMATRIX matWorld = pEffChar->GetInfluenceMatrix();
	m_matWorld = matWorld;

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();

	// Note : Check <Enable, Disable>
	ResetEditBox();

	// Note : ListBox에 CustomBone설정들을 셋팅한다.
	ResetBoneList(0);

	// Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();
}

void CEffBoneListEffPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	CString strText;
	CWnd* pWnd = GetDlgItem ( IDC_EDIT_NUM );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwParticlePec = atoi( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_CENTER_FAR );
	pWnd->GetWindowText ( strText );
	m_Property.m_fCreateLength = (float)atof( strText.GetString() );

	// Note : Life
	pWnd = GetDlgItem ( IDC_EDIT_LIFE_MIN );
	pWnd->GetWindowText ( strText );
	m_Property.m_vLife.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LIFE_MAX );
	pWnd->GetWindowText ( strText );
	m_Property.m_vLife.y = (float)atof( strText.GetString() );

	// Note : Scale
	pWnd = GetDlgItem ( IDC_EDIT_SCALE_MIN );
	pWnd->GetWindowText ( strText );
	m_Property.m_vSize.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_MAX );
	pWnd->GetWindowText ( strText );
	m_Property.m_vSize.y = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_RATE );
	pWnd->GetWindowText ( strText );
	m_Property.m_vSize.z = (float)atof( strText.GetString() );

	// Note : Sequence
	pWnd = GetDlgItem ( IDC_EDIT_COL );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwSequenceCol = atoi( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ROW );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwSequenceRow = atoi( strText.GetString() );

	// Note : Rotate
	pWnd = GetDlgItem( IDC_EDIT_ROTATE_MIN );
	pWnd->GetWindowText ( strText );
	m_Property.m_vRotate.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ROTATE_MAX );
	pWnd->GetWindowText ( strText );
	m_Property.m_vRotate.y = (float)atof( strText.GetString() );

	// Note : Velocity
	pWnd = GetDlgItem( IDC_EDIT_VEL_MIN );
	pWnd->GetWindowText ( strText );
	m_Property.m_vVelocity.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_VEL_MAX );
	pWnd->GetWindowText ( strText );
	m_Property.m_vVelocity.y = (float)atof( strText.GetString() );

	// Note : Texture
	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText( strText );
	StringCchCopy( m_Property.m_szTexture, BONEPOSEFF_TEXNAME_SIZE, strText.GetString() );
	

	// Note : Matrix를 얻은 후 값을 넣어준다.
	pEffChar->SetChangeMatrix( m_matWorld );

	// Note : Local
	char		szValue[256];
	D3DXVECTOR3 vPos = pEffChar->GetInfluencePosition();
	pWnd = GetDlgItem( IDC_EDIT_LOCAL_X );
	_gcvt( vPos.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Y );
	_gcvt( vPos.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Z );
	_gcvt( vPos.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();

	pEffChar->SetProperty ( &m_Property );
	pEffChar->Create ( pView->GetD3dDevice() );

	pEffChar->EditApplyResetData();

    EffectApply();
}

void CEffBoneListEffPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
	
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffBoneListEffPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
	
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffBoneListEffPage::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	pEffChar->InsertBone( g_strPICK_BONE.c_str() );

	ResetBoneList( pEffChar->GetCustomBoneSize()-1 );
}

void CEffBoneListEffPage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	DWORD dwIndex = m_listBone.GetCurSel(); 
	if( dwIndex == LB_ERR )	return;

	if( pEffChar->DeleteBone( dwIndex ) )
	{
		dwIndex = ( dwIndex >= (pEffChar->GetCustomBoneSize()-1) ) ? pEffChar->GetCustomBoneSize()-1 : dwIndex;

		ResetBoneList( dwIndex );
	}
}

void CEffBoneListEffPage::OnBnClickedButtonUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	DWORD dwIndex = m_listBone.GetCurSel();
	if( dwIndex == LB_ERR )	return;

	int nPos = pEffChar->DataUp( dwIndex );

	ResetBoneList( nPos );
}

void CEffBoneListEffPage::OnBnClickedButtonDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	DWORD dwIndex = m_listBone.GetCurSel();
	if( dwIndex == LB_ERR )	return;

	int nPos = pEffChar->DataDown( dwIndex );

	ResetBoneList( nPos );
}

void CEffBoneListEffPage::OnBnClickedCheckRemain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_REMAIN);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_REMAIN;
	else						m_Property.m_dwFlag &= ~USE_REMAIN;

	// Note : 셋팅이 바뀌어서 바꿔준다.
	m_pEffChar->SetProperty( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

void CEffBoneListEffPage::OnBnClickedCheckPosMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	// Note : TRUE인지 FALSE인지 셋팅한다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_POS_MOVE);
	if( pButton->GetCheck() )	pEffChar->SetCheckBoneOffSetPos( TRUE );
	else						pEffChar->SetCheckBoneOffSetPos( FALSE );

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();
}

void CEffBoneListEffPage::OnBnClickedCheckSequence2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_SEQUENCE;
	else						m_Property.m_dwFlag &= ~USE_SEQUENCE;

	// Note : Check <Enable, Disable>
	ResetEditBox();

	// Note : 셋팅이 바뀌어서 바꿔준다.
	pEffChar->SetProperty( &m_Property );
	pEffChar->Create ( pView->GetD3dDevice() );
}

void CEffBoneListEffPage::OnBnClickedCheckRotate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ROTATE);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_ROTATE;
	else						m_Property.m_dwFlag &= ~USE_ROTATE;

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

void CEffBoneListEffPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( (m_Property.m_dwColor&0xff0000)>>16 );
	int ThisG = int ( (m_Property.m_dwColor&0xff00)>>8 );
	int ThisB = int ( m_Property.m_dwColor&0xff );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();

		m_Property.m_dwColor = 0xff000000;
		m_Property.m_dwColor += (GetRValue(SelectColor)<<16);
		m_Property.m_dwColor += (GetGValue(SelectColor)<<8);
		m_Property.m_dwColor += GetBValue(SelectColor);
	}
}

void CEffBoneListEffPage::OnBnClickedButtonTextureFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText( FilePath );
	}
}

void CEffBoneListEffPage::OnLbnSelchangeListBone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if( !m_pEffChar )	return;

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	// Note : 기존 바꾸었던 Matrix를 셋팅한다.
	pEffChar->SetChangeMatrix( m_matWorld );	

	DWORD dwIndex = m_listBone.GetCurSel();
	if( dwIndex == LB_ERR )	return;

	// Note : 어떤 Bone을 선택 했는지 셋팅한다.
	pEffChar->SetCheckBone( dwIndex );

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

BOOL CEffBoneListEffPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}

//--------------------------------------------------------------------------------------------------------------------
void CEffBoneListEffPage::ResetBoneList( int nCur )
{
	if( !m_pEffChar )	return;

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	m_listBone.ResetContent();

	DWORD dwAll = pEffChar->GetCustomBoneSize();
	if( !dwAll )	return;

	for( DWORD i=0; i<dwAll; ++i )
	{
		int nIndex = m_listBone.AddString( pEffChar->GetBoneString(i).c_str() );
		m_listBone.SetItemData( nIndex, nIndex );
	}

	m_listBone.SetCurSel( nCur );
}

void CEffBoneListEffPage::CheckCurserMRS()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_POS_MOVE);
	if( pButton->GetCheck() )	pView->ActiveEditMatrix( &m_matWorld );
	else						pView->DeActiveEditMatrix();
}

void CEffBoneListEffPage::ResetEditBox()
{
	char	szValue[256];
	CWnd*	pWnd;
	CButton* pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE);
	if( pButton->GetCheck() )
	{
		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_COL );
		pWnd->EnableWindow( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_ROW );
		pWnd->EnableWindow( TRUE );
	}
	else
	{
		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_COL );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_ROW );
		pWnd->EnableWindow( FALSE );
	}

	pButton = (CButton*)GetDlgItem(IDC_CHECK_ROTATE);
	if( pButton->GetCheck() )
	{
		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_ROTATE_MIN );
		pWnd->EnableWindow( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_ROTATE_MAX );
		pWnd->EnableWindow( TRUE );
	}
	else
	{
		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_ROTATE_MIN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_ROTATE_MAX );
		pWnd->EnableWindow( FALSE );
	}

	pButton = (CButton*)GetDlgItem(IDC_CHECK_REMAIN);
	if( pButton->GetCheck() )
	{
		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_VEL_MIN );
		pWnd->EnableWindow( TRUE );
		pWnd = GetDlgItem( IDC_EDIT_VEL_MAX );
		pWnd->EnableWindow( TRUE );
	}
	else
	{
		CWnd* pWnd;
		pWnd = GetDlgItem( IDC_EDIT_VEL_MIN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_EDIT_VEL_MAX );
		pWnd->EnableWindow( FALSE );
	}

    DxEffCharBoneListEff* pEffChar = dynamic_cast<DxEffCharBoneListEff*>(m_pEffChar);

	// Note : 선택된 Bone의 옵션에서 bVertex의 상태가 어떠한가 ?
	pButton = (CButton*)GetDlgItem( IDC_CHECK_POS_MOVE );
	if( pEffChar->GetEnableBonePos() )	pButton->SetCheck( TRUE );
	else									pButton->SetCheck( FALSE );

	// Note : Local
	D3DXVECTOR3 vPos = pEffChar->GetInfluencePosition();
	pWnd = GetDlgItem( IDC_EDIT_LOCAL_X );
	_gcvt( vPos.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Y );
	_gcvt( vPos.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Z );
	_gcvt( vPos.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : 커서를 쓰기 위한 작업
	D3DXMATRIX matWorld = pEffChar->GetInfluenceMatrix();
	m_matWorld = matWorld;

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();
}

void CEffBoneListEffPage::OnBnClickedCheckCreateRandomDontMove()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_CREATE_RANDOM_DONT_MOVE);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_CREATERANDOM_DONTMOVE;
	else						m_Property.m_dwFlag &= ~USE_CREATERANDOM_DONTMOVE;
}
