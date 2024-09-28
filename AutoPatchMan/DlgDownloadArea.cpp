// DlgDownloadArea.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoPatchMan.h"
#include "DlgDownloadArea.h"
#include ".\dlgdownloadarea.h"
#include "LauncherText.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDlgDownloadArea 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgDownloadArea, CDialog)
CDlgDownloadArea::CDlgDownloadArea(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDownloadArea::IDD, pParent)
	, m_nDownloadArea(0)
	, m_pMemDC(NULL)
	, m_pOldBitmap(NULL)
	, m_pBitmapMem(NULL)
	, m_bLBDown( FALSE )
{
}

CDlgDownloadArea::~CDlgDownloadArea()
{
}

void CDlgDownloadArea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_RADIO1, m_radioDownArea1 );
	DDX_Control(pDX, IDC_RADIO2, m_radioDownArea2 );
	DDX_Control(pDX, IDC_RADIO3, m_radioDownArea3 );
}


BEGIN_MESSAGE_MAP(CDlgDownloadArea, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDownloadArea 메시지 처리기입니다.

BOOL CDlgDownloadArea::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitCtrls ();
	UpdateCtrls ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL	CDlgDownloadArea::InitCtrls	()
{
	m_stTitle.SubclassDlgItem( IDC_TITLE_STATIC, this );
	m_stTitle.SetFontSize( 10, true );
	m_stArea.SubclassDlgItem( IDC_AREA_STATIC, this );

	m_btnOK.SetBitmaps(IDB_BITMAP_OK_ON, IDB_BITMAP_OK_NOR, IDB_BITMAP_OK_DN, IDB_BITMAP_OK_NOR );
	m_btnOK.SizeToContent();

	m_radioDownArea1.SetIcon(IDI_RADIO_ON, IDI_RADIO_OFF);
	m_radioDownArea1.DrawTransparent();
	m_radioDownArea2.SetIcon(IDI_RADIO_ON, IDI_RADIO_OFF);
	m_radioDownArea2.DrawTransparent();
	m_radioDownArea3.SetIcon(IDI_RADIO_ON, IDI_RADIO_OFF);
	m_radioDownArea3.DrawTransparent();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetDlgItemText( IDC_TITLE_STATIC, ID2LAUNCHERTEXT("IDD_DIALOG_DOWNLOAD_AREA"));
	SetDlgItemText( IDC_AREA_STATIC, ID2LAUNCHERTEXT("IDC_AREA_STATIC") );
	SetDlgItemText( IDOK, "" );
	
	if ( !m_strAreaName[0].IsEmpty() ) SetDlgItemText( IDC_RADIO_AREA1, m_strAreaName[0] );
	if ( !m_strAreaName[1].IsEmpty() ) SetDlgItemText( IDC_RADIO_AREA2, m_strAreaName[1] );
	if ( !m_strAreaName[2].IsEmpty() ) SetDlgItemText( IDC_RADIO_AREA3, m_strAreaName[2] );	// 중국지역 추가

	return TRUE;
}

BOOL	CDlgDownloadArea::UpdateCtrls ()
{
	SetWin_Check ( this, IDC_RADIO1, m_nDownloadArea == 0 );
	SetWin_Check ( this, IDC_RADIO2, m_nDownloadArea == 1 );
	SetWin_Check ( this, IDC_RADIO3, m_nDownloadArea == 2 );

	return TRUE;
}

BOOL	CDlgDownloadArea::InverseUpdateCtrls ()
{
	if ( GetWin_Check( this, IDC_RADIO1 ) ) m_nDownloadArea = 0;
	if ( GetWin_Check( this, IDC_RADIO2 ) ) m_nDownloadArea = 1;
	if ( GetWin_Check( this, IDC_RADIO3 ) ) m_nDownloadArea = 2;

	return TRUE;
}

void CDlgDownloadArea::BuildBkDC()
{
	CBitmap bmpbk;
	bmpbk.LoadBitmap(  IDB_BITMAP_DOWN_BACK );
	BITMAP bmpInfo;
	bmpbk.GetBitmap( &bmpInfo);

	CDC* pWndDC = GetDC();
	CDC memdc;
	memdc.CreateCompatibleDC( pWndDC);
	CBitmap* pOld = memdc.SelectObject( &bmpbk);


	CRect rtClient;
	GetClientRect(&rtClient);

	if ( m_pMemDC )
	{
		m_pMemDC->SelectObject( m_pOldBitmap);
		delete m_pBitmapMem;
		m_pBitmapMem = NULL;
		delete m_pMemDC;
		m_pMemDC = NULL;
	}
	
	if ( m_pMemDC == NULL )
	{
		m_pMemDC = new CDC;
		m_pMemDC->CreateCompatibleDC( pWndDC);

		m_pBitmapMem = new CBitmap;
		m_pBitmapMem->CreateCompatibleBitmap( pWndDC, rtClient.Width(), rtClient.Height());
		m_pOldBitmap = (CBitmap*)m_pMemDC->SelectObject( m_pBitmapMem);
	}

	m_pMemDC->StretchBlt( 0,0, rtClient.Width(), rtClient.Height(), &memdc, 0,0,bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
    memdc.SelectObject( pOld);
	ReleaseDC( pWndDC);

	//when the parent dialog's background is rebuild, notify the child which has an transparent tag.
	CTransparentHelper::NotifyTransparentChild( GetSafeHwnd());
}


LRESULT CDlgDownloadArea::OnTransaprentBk( WPARAM wParam, LPARAM lParam)
{
	HDC hdc = ( HDC)wParam;
	HWND hwnd = ( HWND)lParam;
    CTransparentHelper::OnTranparentControl( m_pMemDC->GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}


BOOL CDlgDownloadArea::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return TRUE;
}

void CDlgDownloadArea::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	BuildBkDC();
	Invalidate(FALSE);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgDownloadArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_MouseClickPos = point;
	m_bLBDown = TRUE;
	SetCapture();

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgDownloadArea::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( m_bLBDown )
	{
		ReleaseCapture ();
		m_bLBDown = FALSE;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgDownloadArea::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if ( !m_bLBDown ) return;

	RECT rect;
	GetWindowRect ( &rect );

	CPoint MovePos;
	MovePos.x = m_MouseClickPos.x - point.x;
	MovePos.y = m_MouseClickPos.y - point.y;
	
	rect.left -= MovePos.x;
	rect.top -= MovePos.y;
	rect.right -= MovePos.x;
	rect.bottom -= MovePos.y;

	MoveWindow ( &rect );

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgDownloadArea::OnPaint()
{
	CPaintDC dc(this);
	CRect rtClient;
	GetClientRect(&rtClient);

	if ( m_pMemDC )
	{
		dc.BitBlt( 0,0,rtClient.Width(), rtClient.Height(), m_pMemDC, 0,0, SRCCOPY);
	}
}

void CDlgDownloadArea::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SAFE_DELETE ( m_pMemDC );
	SAFE_DELETE ( m_pBitmapMem );

	CDialog::PostNcDestroy();
}

void CDlgDownloadArea::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCheck = m_radioDownArea1.GetCheck();
	if ( nCheck ) 
	{
		m_radioDownArea1.SetCheck( 1 );
		m_radioDownArea2.SetCheck( 0 );
		m_radioDownArea3.SetCheck( 0 );
	}
	else
	{
		m_radioDownArea1.SetCheck( 1 );
	}
}

void CDlgDownloadArea::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCheck = m_radioDownArea2.GetCheck();
	if ( nCheck ) 
	{
		m_radioDownArea1.SetCheck( 0 );
		m_radioDownArea2.SetCheck( 1 );
		m_radioDownArea3.SetCheck( 0 );
	}
	else
	{
		m_radioDownArea2.SetCheck( 1 );
	}
}

void CDlgDownloadArea::OnBnClickedRadio3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCheck = m_radioDownArea3.GetCheck();
	if ( nCheck ) 
	{
		m_radioDownArea1.SetCheck( 0 );
		m_radioDownArea2.SetCheck( 0 );
		m_radioDownArea3.SetCheck( 1 );
	}
	else
	{
		m_radioDownArea3.SetCheck( 1 );
	}
}

void CDlgDownloadArea::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InverseUpdateCtrls ();

	OnOK();
}
