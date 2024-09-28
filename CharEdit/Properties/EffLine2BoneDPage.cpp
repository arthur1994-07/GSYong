// EffLine2BoneDPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../Dialog/ColorPickerDlg.h"
#include "SheetWithTabChar.h"

#ifdef DEF_SKIN_SYNC
#include "SheetWithTabPiece.h"
#include "../EngineLib/DxEffect/Char/DxEffCharBonePosEff.h"
#endif

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "EffLine2BoneDPage.h"
#include ".\effline2bonedpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffLine2BoneDPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffLine2BoneDPage, CPropertyPage)
CEffLine2BoneDPage::CEffLine2BoneDPage()
	: CPieceRelativePage(CEffLine2BoneDPage::IDD)
	, m_rEffChar(NULL)
	, m_rProperty(NULL)
{
}

CEffLine2BoneDPage::~CEffLine2BoneDPage()
{
}

void CEffLine2BoneDPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEffLine2BoneDPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_BACK, OnBnClickedButtonBack)
	ON_BN_CLICKED(IDC_CHECK_SEQUENCE2, OnBnClickedCheckSequence)
	ON_BN_CLICKED(IDC_BUTTON_COLOR3, OnBnClickedButtonColor3)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_FIND, OnBnClickedButtonTextureFind)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_OTHER, OnBnClickedButtonColorOther)
	ON_BN_CLICKED(IDC_RADIO_STARTEND, OnBnClickedRadioStartend)
	ON_BN_CLICKED(IDC_RADIO_BLUR, OnBnClickedRadioBlur)
	ON_BN_CLICKED(IDC_RADIO_EACH, OnBnClickedRadioEach)
	ON_BN_CLICKED(IDC_RADIO_NONE, OnBnClickedRadioNone)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_FIND2, OnBnClickedButtonTextureFind2)
	ON_BN_CLICKED(IDC_CHECK_CAMMOVE, OnBnClickedCheckCammove)
    ON_BN_CLICKED(IDC_CHECK_ABSOLUTE, &CEffLine2BoneDPage::OnBnClickedCheckAbsolute)
END_MESSAGE_MAP()


// CEffLine2BoneDPage 메시지 처리기입니다.

void CEffLine2BoneDPage::SetProperty( DxEffChar* pEffChar, EFFCHAR_PROPERTY *pProp )
{
	//
	m_rEffChar = (DxEffCharLine2BoneEff*)pEffChar;
	m_rProperty = (EFFCHAR_PROPERTY_LINE2BONEEFF*)pProp;

	CWnd*		pWnd;
	CButton*	pButton;
	char		szValue[256];

	pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE2);
	if( m_rProperty->m_dwFlag&USE_SEQUENCE )	pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

    pButton = (CButton*)GetDlgItem(IDC_CHECK_ABSOLUTE);
	if( m_rProperty->m_dwFlag&USE_ABSOLUTE )	pButton->SetCheck( TRUE );
	else										pButton->SetCheck( FALSE );

	pWnd = GetDlgItem( IDC_EDIT_NUM );
	_gcvt( m_rProperty->m_dwParticlePec, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_CENTER_FAR );
	_gcvt( m_rProperty->m_fCreateLength, 7, szValue );
	pWnd->SetWindowText( szValue );	

	pWnd = GetDlgItem( IDC_EDIT_SHAKE );
	_gcvt( m_rProperty->m_fShake, 7, szValue );
	pWnd->SetWindowText( szValue );	
	

	// Note : Life
	pWnd = GetDlgItem( IDC_EDIT_LIFE_MIN );
	_gcvt( m_rProperty->m_vLife.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_LIFE_MAX );
	_gcvt( m_rProperty->m_vLife.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Scale
	pWnd = GetDlgItem( IDC_EDIT_SCALE_MIN );
	_gcvt( m_rProperty->m_vSize.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_MAX );
	_gcvt( m_rProperty->m_vSize.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_RATE );
	_gcvt( m_rProperty->m_vSize.z, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Scale EX
	pWnd = GetDlgItem( IDC_EDIT_SCALE_X_S );
	_gcvt( m_rProperty->m_vScaleX.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_X_E );
	_gcvt( m_rProperty->m_vScaleX.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_Y_S );
	_gcvt( m_rProperty->m_vScaleY.x, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_Y_E_TOP );
	_gcvt( m_rProperty->m_vScaleY.y, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_SCALE_Y_E_BOTTOM );
	_gcvt( m_rProperty->m_vScaleY.z, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Sequece
	pWnd = GetDlgItem( IDC_EDIT_COL );
	_gcvt( m_rProperty->m_dwSequenceCol, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_ROW );
	_gcvt( m_rProperty->m_dwSequenceRow, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Cam To Body Dist
	pWnd = GetDlgItem( IDC_EDIT_FLAREPOS1 );
	_gcvt( m_rProperty->m_fMinMaxDist.min, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_FLAREPOS2 );
	_gcvt( m_rProperty->m_fMinMaxDist.max, 7, szValue );
	pWnd->SetWindowText( szValue );

	// Note : Texture Rotate
	pWnd = GetDlgItem( IDC_EDIT_TEXROTATE1 );
	_gcvt( m_rProperty->m_fMinMaxTexRotate.min, 7, szValue );
	pWnd->SetWindowText( szValue );

	pWnd = GetDlgItem( IDC_EDIT_TEXROTATE2 );
	_gcvt( m_rProperty->m_fMinMaxTexRotate.max, 7, szValue );
	pWnd->SetWindowText( szValue );

	SetWin_Check( this, IDC_CHECK_CAMMOVE, m_rProperty->m_bWithCamMove );

	if( GetWin_Check( this, IDC_CHECK_CAMMOVE ) )
	{
		SetWin_Enable( this, IDC_EDIT_TEXROTATE1, FALSE );
		SetWin_Enable( this, IDC_EDIT_TEXROTATE2, FALSE );
	}else{
		SetWin_Enable( this, IDC_EDIT_TEXROTATE1, TRUE );
		SetWin_Enable( this, IDC_EDIT_TEXROTATE2, TRUE );
	}




	DWORD dwAlpha = ((m_rProperty->m_dwColor&0xff000000)>>24);
	pWnd = GetDlgItem( IDC_EDIT_ALPHA );
	_gcvt( dwAlpha, 7, szValue );
	pWnd->SetWindowText( szValue );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_NONE );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_STARTEND );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_BLUR );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_RADIO_EACH );
	pButton->SetCheck( FALSE );

	switch( m_rProperty->m_dwOtherColor )
	{
	case 0:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_NONE );
		pButton->SetCheck( TRUE );
		break;
	case 1:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_STARTEND );
		pButton->SetCheck( TRUE );
		break;
	case 2:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_BLUR );
		pButton->SetCheck( TRUE );
		break;
	case 3:
		pButton = (CButton*)GetDlgItem( IDC_RADIO_EACH );
		pButton->SetCheck( TRUE );
		break;
	}

	// Note : Texture
	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText( m_rProperty->m_szTexture );

	pWnd = GetDlgItem( IDC_EDIT_TEXTURE2 );
	pWnd->SetWindowText( m_rProperty->m_szTexture2 );
}

void CEffLine2BoneDPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_rEffChar )		return;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CString strText;
	CWnd* pWnd = GetDlgItem ( IDC_EDIT_NUM );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_dwParticlePec = atoi( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_CENTER_FAR );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_fCreateLength = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SHAKE );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_fShake = (float)atof( strText.GetString() );


	// Note : Life
	pWnd = GetDlgItem ( IDC_EDIT_LIFE_MIN );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vLife.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_LIFE_MAX );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vLife.y = (float)atof( strText.GetString() );

	// Note : Scale
	pWnd = GetDlgItem ( IDC_EDIT_SCALE_MIN );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vSize.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_MAX );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vSize.y = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_RATE );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vSize.z = (float)atof( strText.GetString() );


	// Scale EX
	pWnd = GetDlgItem ( IDC_EDIT_SCALE_X_S );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vScaleX.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_X_E );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vScaleX.y = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_Y_S );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vScaleY.x = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_Y_E_TOP );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vScaleY.y = (float)atof( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE_Y_E_BOTTOM );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_vScaleY.z = (float)atof( strText.GetString() );

	// Note : Cam To Body Dist
	pWnd = GetDlgItem( IDC_EDIT_FLAREPOS1 );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_fMinMaxDist.min =  (float)atof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_FLAREPOS2 );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_fMinMaxDist.max =  (float)atof( strText.GetString() );

	// Note : Texture Rotate
	pWnd = GetDlgItem( IDC_EDIT_TEXROTATE1 );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_fMinMaxTexRotate.min =  (float)atof( strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TEXROTATE2 );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_fMinMaxTexRotate.max =  (float)atof( strText.GetString() );

	m_rProperty->m_bWithCamMove = GetWin_Check( this, IDC_CHECK_CAMMOVE ) ? true : false;



	

	// Note : Sequence
	pWnd = GetDlgItem ( IDC_EDIT_COL );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_dwSequenceCol = atoi( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ROW );
	pWnd->GetWindowText ( strText );
	m_rProperty->m_dwSequenceRow = atoi( strText.GetString() );


	pWnd = GetDlgItem ( IDC_EDIT_ALPHA );
	pWnd->GetWindowText ( strText );
	DWORD dwAlpha = atoi( strText.GetString() );
	DWORD dwColor = m_rProperty->m_dwColor & 0xffffff;
	m_rProperty->m_dwColor = dwAlpha<<24;
	m_rProperty->m_dwColor += dwColor;

	// Note : Texture
	pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText( strText );
	StringCchCopy( m_rProperty->m_szTexture, BONEPOSEFF_TEXNAME_SIZE, strText.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_TEXTURE2 );
	pWnd->GetWindowText( strText );
	StringCchCopy( m_rProperty->m_szTexture2, BONEPOSEFF_TEXNAME_SIZE, strText.GetString() );

	m_rEffChar->SetProperty ( m_rProperty );
	m_rEffChar->Create ( pView->GetD3dDevice() );

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

void CEffLine2BoneDPage::OnBnClickedButtonBack()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedButtonApply();

#ifndef DEF_SKIN_SYNC
	m_pSheetTab->SetActivePageTab( CHARTAB_EFFECT_LINE2BONE );
#else
    SetActiveParentPage(EMEFFCHAR_LINE2BONE);
#endif
}

void CEffLine2BoneDPage::OnBnClickedCheckSequence()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE2);
	if( pButton->GetCheck() )	m_rProperty->m_dwFlag |= USE_SEQUENCE;
	else						m_rProperty->m_dwFlag &= ~USE_SEQUENCE;

	// Note : 셋팅이 바뀌어서 바꿔준다.
	m_rEffChar->SetProperty( m_rProperty );
	m_rEffChar->Create ( pView->GetD3dDevice() );
}

void CEffLine2BoneDPage::OnBnClickedButtonColor3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwAlpha = (m_rProperty->m_dwColor&0xff000000);
	int ThisR = int ( (m_rProperty->m_dwColor&0xff0000)>>16 );
	int ThisG = int ( (m_rProperty->m_dwColor&0xff00)>>8 );
	int ThisB = int ( m_rProperty->m_dwColor&0xff );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();

		m_rProperty->m_dwColor = dwAlpha << 24;
		m_rProperty->m_dwColor += (GetRValue(SelectColor)<<16);
		m_rProperty->m_dwColor += (GetGValue(SelectColor)<<8);
		m_rProperty->m_dwColor += GetBValue(SelectColor);
	}
}

void CEffLine2BoneDPage::OnBnClickedButtonTextureFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, OFN_HIDEREADONLY, szFilter, this );

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

void CEffLine2BoneDPage::OnBnClickedButtonTextureFind2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_TEXTURE2 );
		pWnd->SetWindowText( FilePath );
	}
}

void CEffLine2BoneDPage::OnBnClickedButtonColorOther()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int ThisR = int ( (m_rProperty->m_dwColor2&0xff0000)>>16 );
	int ThisG = int ( (m_rProperty->m_dwColor2&0xff00)>>8 );
	int ThisB = int ( m_rProperty->m_dwColor2&0xff );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );
	
	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();

		m_rProperty->m_dwColor2 = 0xff000000;
		m_rProperty->m_dwColor2 += (GetRValue(SelectColor)<<16);
		m_rProperty->m_dwColor2 += (GetGValue(SelectColor)<<8);
		m_rProperty->m_dwColor2 += GetBValue(SelectColor);
	}
}

void CEffLine2BoneDPage::OnBnClickedRadioNone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_rProperty->m_dwOtherColor = 0;
}

void CEffLine2BoneDPage::OnBnClickedRadioStartend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_rProperty->m_dwOtherColor = 1;
}

void CEffLine2BoneDPage::OnBnClickedRadioBlur()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_rProperty->m_dwOtherColor = 2;
}

void CEffLine2BoneDPage::OnBnClickedRadioEach()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_rProperty->m_dwOtherColor = 3;
}


void CEffLine2BoneDPage::OnBnClickedCheckCammove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GetWin_Check( this, IDC_CHECK_CAMMOVE ) )
	{
		SetWin_Enable( this, IDC_EDIT_TEXROTATE1, FALSE );
		SetWin_Enable( this, IDC_EDIT_TEXROTATE2, FALSE );
	}else{
		SetWin_Enable( this, IDC_EDIT_TEXROTATE1, TRUE );
		SetWin_Enable( this, IDC_EDIT_TEXROTATE2, TRUE );
	}
}

void CEffLine2BoneDPage::OnBnClickedCheckAbsolute()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ABSOLUTE);
	if( pButton->GetCheck() )	m_rProperty->m_dwFlag |= USE_ABSOLUTE;
	else						m_rProperty->m_dwFlag &= ~USE_ABSOLUTE;
}
