// EffSinglePage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"
#include "SheetWithTabChar.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../Dialog/EffHighDlg.h"
#include "EffSinglePage.h"
#include ".\effsinglepage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffSinglePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffSinglePage, CPropertyPage)
CEffSinglePage::CEffSinglePage()
: CEffCharPage(CEffSinglePage::IDD)
{
    D3DXMatrixIdentity( &m_matWorld );
}

CEffSinglePage::~CEffSinglePage()
{
}

void CEffSinglePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TRACE_VERT, m_ComboTraceVerts);
	DDX_Control(pDX, IDC_COMBO1, m_pComboWeather);
	DDX_Control(pDX, IDC_COMBO_TRACE_VERT_MOVE, m_ComboVertMove);
}


BEGIN_MESSAGE_MAP(CEffSinglePage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_ATTACK, OnBnClickedRadioAttack)
	ON_BN_CLICKED(IDC_RADIO_ATTACK_EXCEPT, OnBnClickedRadioAttackExcept)
	ON_BN_CLICKED(IDC_CHECK_REPEAT, OnBnClickedCheckRepeat)
	ON_BN_CLICKED(IDC_RADIO_1POINT, OnBnClickedRadio1point)
	ON_BN_CLICKED(IDC_RADIO_2POINT, OnBnClickedRadio2point)
	ON_BN_CLICKED(IDC_CHECK_REPEAT_MOVE, OnBnClickedCheckRepeatMove)
	ON_BN_CLICKED(IDC_RADIO_MATRIX, OnBnClickedRadioMatrix)
	ON_BN_CLICKED(IDC_CHECK_AUTOMOVE, OnBnClickedCheckAutomove)
	ON_BN_CLICKED(IDC_CHECK_TIMEDAY_EFF, OnBnClickedCheckTimeday)
	ON_BN_CLICKED(IDC_CHECK_TIMENIGHT_EFF, OnBnClickedCheckTimenight)
END_MESSAGE_MAP()


// CEffSinglePage 메시지 처리기입니다.
const char NOFILE[] = "";
const char szWeatherName[4][100] = 
{
	"0.전부 적용",
		"1.맑을때만..",
		"2.비올때만..",
		"3.눈올때만.."
};

BOOL CEffSinglePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	pWnd->SetWindowText ( NOFILE );

	int nIndex;
	m_ComboTraceVerts.ResetContent ();

	for ( int i=0; i<6; ++i )
	{
		nIndex = m_ComboTraceVerts.AddString ( szTRACE_NAME[i] );
		m_ComboTraceVerts.SetItemData ( nIndex, i );
	}
	for ( int i=11; i<14; ++i )
	{
		nIndex = m_ComboTraceVerts.AddString ( szTRACE_NAME[i] );
		m_ComboTraceVerts.SetItemData ( nIndex, i );
	}


	m_pComboWeather.ResetContent ();

	nIndex = m_pComboWeather.AddString ( szWeatherName[0] );
	m_pComboWeather.SetItemData ( nIndex, 0 );

	nIndex = m_pComboWeather.AddString ( szWeatherName[1] );
	m_pComboWeather.SetItemData ( nIndex, 1 );

	nIndex = m_pComboWeather.AddString ( szWeatherName[2] );
	m_pComboWeather.SetItemData ( nIndex, 2 );

	//nIndex = m_pComboWeather.AddString ( szWeatherName[3] );
	//m_pComboWeather.SetItemData ( nIndex, 3 );

	m_ComboVertMove.ResetContent ();

	for ( int i=0; i<6; ++i )
	{
		nIndex = m_ComboVertMove.AddString ( szTRACE_NAME[i] );
		m_ComboVertMove.SetItemData ( nIndex, i );
	}
	for ( int i=11; i<14; ++i )
	{
		nIndex = m_ComboVertMove.AddString ( szTRACE_NAME[i] );
		m_ComboVertMove.SetItemData ( nIndex, i );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffSinglePage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	char	szValue[256];
	CWnd		*pWnd;
	CButton*	pButton;

	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_SINGLE*)pProp);
	else
	{
		m_Property = EFFCHAR_PROPERTY_SINGLE();
	}

	if( m_Property.m_dwFlag & USE_1POINT )
	{
		pButton = (CButton*)GetDlgItem( IDC_RADIO_1POINT );
		pButton->SetCheck ( TRUE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_2POINT );
		pButton->SetCheck ( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_MATRIX );
		pButton->SetCheck ( FALSE );
	}
	else if( m_Property.m_dwFlag & USE_2POINT )
	{
		pButton = (CButton*)GetDlgItem( IDC_RADIO_1POINT );
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_2POINT );
		pButton->SetCheck( TRUE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_MATRIX );
		pButton->SetCheck( FALSE );
	}
	else if( m_Property.m_dwFlag & USE_MATRIX )
	{
		pButton = (CButton*)GetDlgItem( IDC_RADIO_1POINT );
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_2POINT );
		pButton->SetCheck( FALSE );
		pButton = (CButton*)GetDlgItem( IDC_RADIO_MATRIX );
		pButton->SetCheck( TRUE );
	}


	if (m_Property.m_nAniMType==0)
	{
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
		pButton->SetCheck(1);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK);
		pButton->SetCheck(0);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK_EXCEPT);
		pButton->SetCheck(0);
	}
	else if (m_Property.m_nAniMType==1)
	{
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
		pButton->SetCheck(0);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK);
		pButton->SetCheck(1);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK_EXCEPT);
		pButton->SetCheck(0);
	}
	else if (m_Property.m_nAniMType==2)
	{
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
		pButton->SetCheck(0);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK);
		pButton->SetCheck(0);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK_EXCEPT);
		pButton->SetCheck(1);
	}

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TIMEDAY_EFF );
	pButton->SetCheck ( !(m_Property.m_dwFlag&USE_NOT_TIMEDAY) );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TIMENIGHT_EFF );
	pButton->SetCheck ( !(m_Property.m_dwFlag&USE_NOT_TIMENIGHT) );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_REPEAT );
	if ( m_Property.m_dwFlag&USE_REPEAT )		pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_REPEAT_MOVE );
	if ( m_Property.m_dwFlag&USE_REPEATMOVE )	pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_AUTOMOVE );
	if ( m_Property.m_dwFlag&USE_AUTOMOVE )		pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pWnd = GetDlgItem ( IDC_EDIT_NORMAL_UP );
	_gcvt ( m_Property.m_fDepthUp, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_NORMAL_DOWN );
	_gcvt ( m_Property.m_fDepthDown, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	_gcvt ( m_Property.m_fScale, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_REPEAT_TIME );
	_gcvt ( m_Property.m_fCoolTime, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_REPEAT_SPEED );
	_gcvt ( m_Property.m_fRepeatSpeed, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	pWnd->SetWindowText ( m_Property.m_szFileName );

	m_pComboWeather.SetCurSel (m_Property.m_nWeather);

	m_ComboTraceVerts.SelectString ( 0, m_Property.m_szTrace );
	m_ComboVertMove.SelectString ( 0, m_Property.m_szTraceBack );

    SetMatrixAndActiveEditMatrix();
}

void CEffSinglePage::OnBnClickedButtonSelect()
{
	//	Note : 파일 오픈 다이알로그를 만듬.
	CString szFilter = "Effect Group ( *.egp ) |*.egp";
	CFileDialog dlg ( TRUE, ".egp", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString FileName = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
		pWnd->SetWindowText ( FileName );
	}
}

void CEffSinglePage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

    m_Property.m_matBase = SetRotateMatrix( pSkinPiece, m_matWorld, m_Property.m_szTrace );

	CWnd		*pWnd;
	CString		strText;

	pWnd = GetDlgItem ( IDC_EDIT_NORMAL_UP );
	pWnd->GetWindowText ( strText );
	m_Property.m_fDepthUp = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_NORMAL_DOWN );
	pWnd->GetWindowText ( strText );
	m_Property.m_fDepthDown = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SCALE );
	pWnd->GetWindowText ( strText );
	m_Property.m_fScale = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_REPEAT_TIME );
	pWnd->GetWindowText ( strText );
	m_Property.m_fCoolTime = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_REPEAT_SPEED );
	pWnd->GetWindowText ( strText );
	m_Property.m_fRepeatSpeed = (float)atof ( strText.GetString() );

	CString FileName;
	pWnd = GetDlgItem ( IDC_EDIT_EFFECT_FILE );
	pWnd->GetWindowText ( FileName );

	int nIndex = m_ComboTraceVerts.GetCurSel ();
	if ( LB_ERR == nIndex )		return;
	DWORD dwData = m_ComboTraceVerts.GetItemData ( nIndex );

	strcpy ( m_Property.m_szFileName, FileName.GetString() );
	strcpy ( m_Property.m_szTrace, szTRACE_NAME[dwData] );

	nIndex = m_pComboWeather.GetCurSel ();
	if ( LB_ERR == nIndex )		return;
	dwData = m_pComboWeather.GetItemData ( nIndex );
	m_Property.m_nWeather = dwData;

	nIndex = m_ComboVertMove.GetCurSel ();
	if ( LB_ERR == nIndex )		return;
	dwData = m_ComboVertMove.GetItemData ( nIndex );
	strcpy ( m_Property.m_szTraceBack, szTRACE_NAME[dwData] );

    SetMatrixAndActiveEditMatrix();

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
			(
			DxEffCharSingle::TYPEID,
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
		m_pEffChar->CleanUp();
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    EffectApply();
}

void CEffSinglePage::OnBnClickedOk()
{
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffSinglePage::OnBnClickedCancel()
{
    ReturnPage();
}

void CEffSinglePage::OnBnClickedRadioAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
	pButton->SetCheck(1);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK);
	pButton->SetCheck(0);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK_EXCEPT);
	pButton->SetCheck(0);

	m_Property.m_nAniMType = 0;
}

void CEffSinglePage::OnBnClickedRadioAttack()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
	pButton->SetCheck(0);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK);
	pButton->SetCheck(1);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK_EXCEPT);
	pButton->SetCheck(0);

	m_Property.m_nAniMType = 1;
}

void CEffSinglePage::OnBnClickedRadioAttackExcept()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
	pButton->SetCheck(0);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK);
	pButton->SetCheck(0);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_ATTACK_EXCEPT);
	pButton->SetCheck(1);

	m_Property.m_nAniMType = 2;
}

void CEffSinglePage::OnBnClickedCheckRepeat()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_REPEAT);
	if ( pButton->GetCheck() )		m_Property.m_dwFlag |= USE_REPEAT;
	else							m_Property.m_dwFlag &= ~USE_REPEAT;
}

void CEffSinglePage::OnBnClickedRadioMatrix()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_1POINT;
	m_Property.m_dwFlag &= ~USE_2POINT;
	m_Property.m_dwFlag |= USE_MATRIX;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if ( m_pEffChar )
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    SetMatrixAndActiveEditMatrix();
}

void CEffSinglePage::OnBnClickedRadio1point()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag |= USE_1POINT;
	m_Property.m_dwFlag &= ~USE_2POINT;
	m_Property.m_dwFlag &= ~USE_MATRIX;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if ( m_pEffChar )
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    pView->DeActiveEditMatrix();
}

void CEffSinglePage::OnBnClickedRadio2point()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Property.m_dwFlag &= ~USE_1POINT;
	m_Property.m_dwFlag |= USE_2POINT;
	m_Property.m_dwFlag &= ~USE_MATRIX;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if ( m_pEffChar )
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    pView->DeActiveEditMatrix();
}

void CEffSinglePage::OnBnClickedCheckRepeatMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_REPEAT_MOVE );

	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_REPEATMOVE;
	else						m_Property.m_dwFlag &= ~USE_REPEATMOVE;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if ( m_pEffChar )
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}
}

void CEffSinglePage::OnBnClickedCheckAutomove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_AUTOMOVE );

	if ( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_AUTOMOVE;
	else						m_Property.m_dwFlag &= ~USE_AUTOMOVE;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	if ( m_pEffChar )
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}
}

void CEffSinglePage::OnBnClickedCheckTimeday()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TIMEDAY_EFF );
	if ( pButton->GetCheck() )	m_Property.m_dwFlag &= ~USE_NOT_TIMEDAY;
	else						m_Property.m_dwFlag |= USE_NOT_TIMEDAY; 
}

void CEffSinglePage::OnBnClickedCheckTimenight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_TIMENIGHT_EFF );
	if ( pButton->GetCheck() )	m_Property.m_dwFlag &= ~USE_NOT_TIMENIGHT;
	else						m_Property.m_dwFlag |= USE_NOT_TIMENIGHT;
}

void CEffSinglePage::SetMatrixAndActiveEditMatrix()
{
    if ( !(m_Property.m_dwFlag&USE_MATRIX) )  return;

    CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

    DxSkinPiece* pSkinPiece = pView->GetSkinPiece();

    D3DXVECTOR3 vPos, vNormal;
    if ( E_FAIL == pSkinPiece->CalculateVertexInflu( m_Property.m_szTrace, vPos, vNormal ) )    return;

    vPos = vPos - (vNormal*m_Property.m_fDepthUp);

    D3DXMATRIX matTrans;
    D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );
    D3DXMatrixMultiply( &m_matWorld, &m_Property.m_matBase, &matTrans );

    // Note : 커서 활성화
    pView->ActiveEditMatrix( &m_matWorld );
}

D3DXMATRIX CEffSinglePage::SetRotateMatrix( DxSkinPiece* pSKinPiece, const D3DXMATRIX& matWorld, const TCHAR* pName )
{
    HRESULT hr;

    D3DXMATRIX matBase;
    D3DXMatrixIdentity( &matBase );

    if ( !(m_Property.m_dwFlag&USE_MATRIX) )  return matBase;

    D3DXVECTOR3 vPos, vNormal;
    hr = pSKinPiece->CalculateVertexInflu( pName, vPos, vNormal );

    if ( hr == E_FAIL ) return matBase;

    vPos = vPos - (vNormal*m_Property.m_fDepthUp);

    D3DXMATRIX matTrans;
    D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );

    D3DXMATRIX matInverse;
    D3DXMatrixInverse( &matInverse, NULL, &matTrans );
    D3DXMatrixMultiply( &matBase, &matWorld, &matInverse );

    return matBase;
}