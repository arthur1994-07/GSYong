// EffBonePosEffPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../Dialog/ColorPickerDlg.h"
#include "SheetWithTabChar.h"

#include "EffBonePosEffPage.h"
#include ".\effboneposeffpage.h"

extern std::string	g_strPICK_BONE;

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffBonePosEffPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffBonePosEffPage, CPropertyPage)
CEffBonePosEffPage::CEffBonePosEffPage()
	: CEffCharPage(CEffBonePosEffPage::IDD)
{
	D3DXMatrixIdentity( &m_matWorld );
}

CEffBonePosEffPage::~CEffBonePosEffPage()
{
}

void CEffBonePosEffPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffBonePosEffPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_GETBONENAME, OnBnClickedButtonGetbonename)
	ON_BN_CLICKED(IDC_CHECK_POS_MOVE, OnBnClickedCheckPosMove)
	ON_BN_CLICKED(IDC_CHECK_SEQUENCE2, OnBnClickedCheckSequence2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_FIND, OnBnClickedButtonTextureFind)
	ON_BN_CLICKED(IDC_CHECK_ROTATE2, OnBnClickedCheckRotate2)
	ON_BN_CLICKED(IDC_CHECK_REMAIN2, OnBnClickedCheckRemain2)
END_MESSAGE_MAP()


// CEffBonePosEffPage 메시지 처리기입니다.
void CEffBonePosEffPage::SetProperty( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )	m_Property = *((EFFCHAR_PROPERTY_BONEPOSEFF*)pProp);
	else
	{
		EFFCHAR_PROPERTY_BONEPOSEFF sProp;
		m_Property = sProp;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CWnd*		pWnd;
	CButton*	pButton;
	char		szValue[256];

	pButton = (CButton*)GetDlgItem(IDC_CHECK_REMAIN2);
	if( m_Property.m_dwFlag&USE_REMAIN )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE2);
	if( m_Property.m_dwFlag&USE_SEQUENCE )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem(IDC_CHECK_POS_MOVE);
	if( m_Property.m_dwFlag&USE_BONEOFFSETPOS )	pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem(IDC_CHECK_ROTATE2);
	if( m_Property.m_dwFlag&USE_ROTATE )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	// Note : Check <Enable, Disable>
	ResetEditBox();


	pWnd = GetDlgItem( IDC_EDIT_NUM );
	_gcvt( m_Property.m_dwParticlePec, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_CENTER_FAR );
	_gcvt( m_Property.m_fCreateLength, 7, szValue );
	pWnd->SetWindowText( szValue );	

	// Note : Local
	pWnd = GetDlgItem( IDC_EDIT_LOCAL_X );
	_gcvt( m_Property.m_vLocalPos.x, 4, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Y );
	_gcvt( m_Property.m_vLocalPos.y, 4, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Z );
	_gcvt( m_Property.m_vLocalPos.z, 4, szValue );
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

	// Note : Velocity
	pWnd = GetDlgItem( IDC_EDIT_VEL_MIN );
	_gcvt( m_Property.m_vVelocity.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_VEL_MAX );
	_gcvt( m_Property.m_vVelocity.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_VEL_RATE );
	_gcvt( m_Property.m_vVelocity.z, 7, szValue );
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

	// Note : 
	pWnd = GetDlgItem( IDC_EDIT_BONE_NAME );
	pWnd->SetWindowText( m_Property.m_szBoneName );

	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText( m_Property.m_szTexture );


	if ( !m_pEffChar )
	{
		m_pEffChar = (DxEffCharBonePosEff*)DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharBonePosEff::TYPEID,
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

    DxEffCharBonePosEff* pEffChar = dynamic_cast<DxEffCharBonePosEff*>(m_pEffChar);

	// Note : 커서를 쓰기 위한 작업
	D3DXMATRIX matWorld = pEffChar->GetInfluenceMatrix();
	m_matWorld = matWorld;

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();

	// Note : 본을 렌더한다고 알려준다.
	pView->SetRenderTypeBone();
}

void CEffBonePosEffPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    DxEffCharBonePosEff* pEffChar = dynamic_cast<DxEffCharBonePosEff*>(m_pEffChar);

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

	// Note : Velocity
	pWnd = GetDlgItem ( IDC_EDIT_VEL_MIN );
	pWnd->GetWindowText ( strText );
	m_Property.m_vVelocity.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_VEL_MAX );
	pWnd->GetWindowText ( strText );
	m_Property.m_vVelocity.y = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_VEL_RATE );
	pWnd->GetWindowText ( strText );
	m_Property.m_vVelocity.z = (float)atof( strText.GetString() );

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

	// Note :
	pWnd = GetDlgItem( IDC_EDIT_BONE_NAME );
	pWnd->GetWindowText( strText );
	StringCchCopy( m_Property.m_szBoneName, BONEPOSEFF_TEXNAME_SIZE, strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText( strText );
	StringCchCopy( m_Property.m_szTexture, BONEPOSEFF_TEXNAME_SIZE, strText.GetString() );
	

	// Note : Matrix를 얻은 후 값을 넣어준다.
	m_Property.m_vLocalPos = pEffChar->SetChangeMatrix( m_matWorld );

	// Note : Local
	char		szValue[256];
	pWnd = GetDlgItem( IDC_EDIT_LOCAL_X );
	_gcvt( m_Property.m_vLocalPos.x, 4, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Y );
	_gcvt( m_Property.m_vLocalPos.y, 4, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LOCAL_Z );
	_gcvt( m_Property.m_vLocalPos.z, 4, szValue );
	pWnd->SetWindowText( szValue );

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();


	pEffChar->SetProperty ( &m_Property );
	pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CEffBonePosEffPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffBonePosEffPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CEffBonePosEffPage::OnBnClickedButtonGetbonename()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
    DxEffCharBonePosEff* pEffChar = dynamic_cast<DxEffCharBonePosEff*>(m_pEffChar);

	StringCchCopy( m_Property.m_szBoneName, BONEPOSEFF_TEXNAME_SIZE, g_strPICK_BONE.c_str() );

	CWnd* pWnd = GetDlgItem( IDC_EDIT_BONE_NAME );
	pWnd->SetWindowText( m_Property.m_szBoneName );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

	// Note : 커서를 쓰기 위한 작업
	D3DXMATRIX matWorld = pEffChar->GetInfluenceMatrix();
	m_matWorld = matWorld;
}

void CEffBonePosEffPage::OnBnClickedCheckPosMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_POS_MOVE);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_BONEOFFSETPOS;
	else						m_Property.m_dwFlag &= ~USE_BONEOFFSETPOS;

	// Note : 커서를 사용.  미 사용 셋팅
	CheckCurserMRS();

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );
}

void CEffBonePosEffPage::OnBnClickedCheckSequence2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE2);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_SEQUENCE;
	else						m_Property.m_dwFlag &= ~USE_SEQUENCE;

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

void CEffBonePosEffPage::OnBnClickedCheckRotate2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ROTATE2);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_ROTATE;
	else						m_Property.m_dwFlag &= ~USE_ROTATE;

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

void CEffBonePosEffPage::OnBnClickedCheckRemain2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_REMAIN2);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_REMAIN;
	else						m_Property.m_dwFlag &= ~USE_REMAIN;
}

void CEffBonePosEffPage::OnBnClickedButtonColor()
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

void CEffBonePosEffPage::OnBnClickedButtonTextureFind()
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

void CEffBonePosEffPage::ResetEditBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE2);
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

	pButton = (CButton*)GetDlgItem(IDC_CHECK_ROTATE2);
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
}

void CEffBonePosEffPage::CheckCurserMRS()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_POS_MOVE);
	if( pButton->GetCheck() )	pView->ActiveEditMatrix( &m_matWorld );
	else						pView->DeActiveEditMatrix();
}

BOOL CEffBonePosEffPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}






