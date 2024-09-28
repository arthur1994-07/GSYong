// EffParticlePage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../Dialog/ColorPickerDlg.h"
#include "SheetWithTabChar.h"

#include "EffParticlePage.h"
#include ".\effparticlepage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffParticlePage 대화 상자입니다.

extern D3DXVECTOR3		g_vCLOSED_POINT;

IMPLEMENT_DYNAMIC(CEffParticlePage, CPropertyPage)
CEffParticlePage::CEffParticlePage()
	: CEffCharPage(CEffParticlePage::IDD)
{
}

CEffParticlePage::~CEffParticlePage()
{
}

void CEffParticlePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_POINT, m_listPoint);
}


BEGIN_MESSAGE_MAP(CEffParticlePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_LBN_SELCHANGE(IDC_LIST_POINT, OnLbnSelchangeListPoint)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_FIND, OnBnClickedButtonTextureFind)
	ON_BN_CLICKED(IDC_CHECK_RANGE, OnBnClickedCheckRange)
	ON_BN_CLICKED(IDC_CHECK_SEQUENCE, OnBnClickedCheckSequence)
	ON_BN_CLICKED(IDC_CHECK_ROTATE, OnBnClickedCheckRotate)
	ON_BN_CLICKED(IDC_CHECK_REMAIN, OnBnClickedCheckRemain)
END_MESSAGE_MAP()


// CEffParticlePage 메시지 처리기입니다.
void CEffParticlePage::SetProperty( EFFCHAR_PROPERTY *pProp )
{
	if( pProp )	m_Property = *((EFFCHAR_PROPERTY_PARTICLE*)pProp);
	else
	{
		EFFCHAR_PROPERTY_PARTICLE sProp;
		m_Property = sProp;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CWnd*		pWnd;
	CButton*	pButton;
	char		szValue[256];

	pButton = (CButton*)GetDlgItem(IDC_CHECK_REMAIN);
	if( m_Property.m_dwFlag&USE_REMAIN )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem(IDC_CHECK_RANGE);
	if( m_Property.m_dwFlag&USE_RANDOMRANGE )	pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE);
	if( m_Property.m_dwFlag&USE_SEQUENCE )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem(IDC_CHECK_ROTATE);
	if( m_Property.m_dwFlag&USE_ROTATE )		pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	// Note : Check <Enable, Disable>
	ResetEditBox();


	pWnd = GetDlgItem( IDC_EDIT_NUM );
	_gcvt( m_Property.m_dwParticlePec, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_RANGE );
	_gcvt( m_Property.m_fRandomRange, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_DATA );
	_gcvt( m_Property.m_vSize.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_RANDOM );
	_gcvt( m_Property.m_vSize.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_RATE );
	_gcvt( m_Property.m_vSize.z, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LIFE_MIN );
	_gcvt( m_Property.m_fLifeMin, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LIFE_MAX );
	_gcvt( m_Property.m_fLifeMax, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Rotate
	pWnd = GetDlgItem( IDC_EDIT_ROTATE_MIN );
	_gcvt( m_Property.m_vRotate.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_ROTATE_MAX );
	_gcvt( m_Property.m_vRotate.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_COL );
	_gcvt( m_Property.m_dwSequenceCol, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_ROW );
	_gcvt( m_Property.m_dwSequenceRow, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Velocity
	pWnd = GetDlgItem( IDC_EDIT_VEL_MIN );
	_gcvt( m_Property.m_vVelocity.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_VEL_MAX );
	_gcvt( m_Property.m_vVelocity.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText( m_Property.m_szTexture );


	if ( !m_pEffChar )
	{
		m_pEffChar = (DxEffCharParticle*)DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharParticle::TYPEID,
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

    DxEffCharParticle* pEffChar = dynamic_cast<DxEffCharParticle*>(m_pEffChar);

	// Note : 커서를 쓰기 위한 작업
	D3DXVECTOR3 vPos = pEffChar->GetInfluencePosition();
	D3DXMatrixIdentity( &m_matWorld );
	m_matWorld._41 = vPos.x;
	m_matWorld._42 = vPos.y;
	m_matWorld._43 = vPos.z;

	// Note : 커서 활성화
	pView->ActiveEditMatrix( &m_matWorld );

	// Note : ListBox에 Particle List를 셋팅한다.
	ResetParticleList( 0 );
}

void CEffParticlePage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
    DxEffCharParticle* pEffChar = dynamic_cast<DxEffCharParticle*>(m_pEffChar);

	CString strText;
	CWnd* pWnd = GetDlgItem ( IDC_EDIT_NUM );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwParticlePec = atoi( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_RANGE );
	pWnd->GetWindowText ( strText );
	m_Property.m_fRandomRange = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_DATA );
	pWnd->GetWindowText ( strText );
	m_Property.m_vSize.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_RANDOM );
	pWnd->GetWindowText ( strText );
	m_Property.m_vSize.y = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_RATE );
	pWnd->GetWindowText ( strText );
	m_Property.m_vSize.z = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LIFE_MIN );
	pWnd->GetWindowText ( strText );
	m_Property.m_fLifeMin = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LIFE_MAX );
	pWnd->GetWindowText ( strText );
	m_Property.m_fLifeMax = (float)atof( strText.GetString() );

	// Note : Rotate
	pWnd = GetDlgItem( IDC_EDIT_ROTATE_MIN );
	pWnd->GetWindowText ( strText );
	m_Property.m_vRotate.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_ROTATE_MAX );
	pWnd->GetWindowText ( strText );
	m_Property.m_vRotate.y = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_COL );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwSequenceCol = atoi( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ROW );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwSequenceRow = atoi( strText.GetString() );

	// Note : Velocity
	pWnd = GetDlgItem( IDC_EDIT_VEL_MIN );
	pWnd->GetWindowText ( strText );
	m_Property.m_vVelocity.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_VEL_MAX );
	pWnd->GetWindowText ( strText );
	m_Property.m_vVelocity.y = (float)atof( strText.GetString() );


	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText( strText );
	StringCchCopy( m_Property.m_szTexture, 64, strText.GetString() );
	

	// Note : Matrix를 얻은 후 값을 넣어준다.
	pEffChar->SetMatrixToMakePos( m_matWorld );


	pEffChar->SetProperty ( &m_Property );
	pEffChar->Create ( pView->GetD3dDevice() );

	pEffChar->EditApplyResetData();

    EffectApply();
}

void CEffParticlePage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffParticlePage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffParticlePage::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharParticle* pEffChar = dynamic_cast<DxEffCharParticle*>(m_pEffChar);

	pEffChar->InsertPoint( g_vCLOSED_POINT );

	ResetParticleList( pEffChar->GetPoistionSize()-1 );
}

void CEffParticlePage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharParticle* pEffChar = dynamic_cast<DxEffCharParticle*>(m_pEffChar);

	DWORD dwIndex = m_listPoint.GetCurSel(); 
	if( dwIndex == LB_ERR )	return;

	if( pEffChar->DeletePoint( dwIndex ) )
	{
		dwIndex = ( dwIndex >= (pEffChar->GetPoistionSize()-1) ) ? pEffChar->GetPoistionSize()-1 : dwIndex;

		ResetParticleList( dwIndex );
	}
}

void CEffParticlePage::OnBnClickedButtonUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharParticle* pEffChar = dynamic_cast<DxEffCharParticle*>(m_pEffChar);

	DWORD dwIndex = m_listPoint.GetCurSel();
	if( dwIndex == LB_ERR )	return;

	int nPos = pEffChar->DataUp( dwIndex );

	ResetParticleList( nPos );
}

void CEffParticlePage::OnBnClickedButtonDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharParticle* pEffChar = dynamic_cast<DxEffCharParticle*>(m_pEffChar);

	DWORD dwIndex = m_listPoint.GetCurSel();
	if( dwIndex == LB_ERR )	return;

	int nPos = pEffChar->DataDown( dwIndex );

	ResetParticleList( nPos );
}

void CEffParticlePage::OnBnClickedCheckRange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_RANGE);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_RANDOMRANGE;
	else						m_Property.m_dwFlag &= ~USE_RANDOMRANGE;

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

void CEffParticlePage::OnBnClickedCheckSequence()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_SEQUENCE;
	else						m_Property.m_dwFlag &= ~USE_SEQUENCE;

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

void CEffParticlePage::OnBnClickedCheckRotate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ROTATE);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_ROTATE;
	else						m_Property.m_dwFlag &= ~USE_ROTATE;

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

void CEffParticlePage::OnBnClickedCheckRemain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_REMAIN);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_REMAIN;
	else						m_Property.m_dwFlag &= ~USE_REMAIN;

	// Note : Check <Enable, Disable>
	ResetEditBox();
}

void CEffParticlePage::ResetParticleList( int nCur )
{
	if( !m_pEffChar )	return;

    DxEffCharParticle* pEffChar = dynamic_cast<DxEffCharParticle*>(m_pEffChar);

	m_listPoint.ResetContent();

	DWORD dwAll = pEffChar->GetPoistionSize();
	if( !dwAll )	return;

	for( DWORD i=0; i<dwAll; ++i )
	{
		int nIndex = m_listPoint.AddString( pEffChar->GetPositionString(i).c_str() );
		m_listPoint.SetItemData( nIndex, nIndex );
	}

	m_listPoint.SetCurSel( nCur );

	// Note : 커서 셋팅
	pEffChar->SetPoistionEditPos( nCur );
	D3DXVECTOR3 vPos = pEffChar->GetInfluencePosition();
	m_matWorld._41 = vPos.x;
	m_matWorld._42 = vPos.y;
	m_matWorld._43 = vPos.z;
}

void CEffParticlePage::OnLbnSelchangeListPoint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_pEffChar )	return;

    DxEffCharParticle* pEffChar = dynamic_cast<DxEffCharParticle*>(m_pEffChar);

	// Note : 기존 바꾸었던 Matrix를 셋팅한다.
	pEffChar->SetMatrixToMakePos( m_matWorld );

	//
	DWORD dwIndex = m_listPoint.GetCurSel();
	if( dwIndex == LB_ERR )	return;

	pEffChar->SetPoistionEditPos( dwIndex );

	// Note : 커서 셋팅
	D3DXVECTOR3 vPos = pEffChar->GetInfluencePosition();
	m_matWorld._41 = vPos.x;
	m_matWorld._42 = vPos.y;
	m_matWorld._43 = vPos.z;
}

void CEffParticlePage::OnBnClickedButtonColor()
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

void CEffParticlePage::OnBnClickedButtonTextureFind()
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

BOOL CEffParticlePage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();
	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}

//-----------------------------------------------------------------	---------------------------------------------------
void CEffParticlePage::ResetEditBox()
{
	CButton* pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_RANGE);
	if( pButton->GetCheck() )
	{
		CWnd* pWnd = GetDlgItem( IDC_EDIT_RANGE );
		pWnd->EnableWindow( TRUE );
	}
	else
	{
		CWnd* pWnd = GetDlgItem( IDC_EDIT_RANGE );
		pWnd->EnableWindow( FALSE );
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
}






