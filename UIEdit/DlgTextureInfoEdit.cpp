#include "stdafx.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "./ExtendUI/UIEditControl.h"
#include "./ExtendUI/UIEditButton.h"
#include "./ExtendUI/UIEditGroup.h"

#include "./MainUI/InterfaceAdmin.h"

#include "UIEdit.h"
#include "UIEditDoc.h"
#include "UIEditView.h"
#include "MainFrm.h"
#include "DlgTextureInfoEdit.h"
#include "../EngineLib/G-Logic/GLogic.h"

//

// CDlgTextureInfoEdit 대화 상자입니다.

using namespace std;

IMPLEMENT_DYNAMIC(CDlgTextureInfoEdit, CDialog)

const float ZoomList[10] =
{
	1.0f,
	1.5f,
	2.0f,
	2.5f,
	3.0f,
	4.0f,
	5.0f,
	6.0f,
	7.0f,
	8.0f
};

CDlgTextureInfoEdit::CDlgTextureInfoEdit(CWnd* pParent /*=NULL*/)
: CDialog(CDlgTextureInfoEdit::IDD, pParent)

, m_strTextureFile( _T( "" ) )
, m_nPicWidth( 0 )
, m_nPicHeight( 0 )
, m_fMultiply( 1.0f )
, m_bAlignRect( TRUE )
, m_bDNPoint( 0, 0 )
, m_bUPPoint( 0, 0 )
, m_bMovePoint( 0, 0 )
, m_bMoveDNPoint( 0, 0 )
, m_bMoveUPPoint( 0, 0 )
, m_cSelectRect( 0, 0, 0, 0 )
, m_bMoveTexture(FALSE)

, m_nControlType( 0 )
, m_pTextureImage( NULL )
{
	memset( &m_stImageInfo, 0, sizeof( D3DXIMAGE_INFO ) );
	memset( &m_stLocked, 0, sizeof( D3DLOCKED_RECT ) );
}

CDlgTextureInfoEdit::~CDlgTextureInfoEdit()
{
	SAFE_DELETE( m_pTextureImage );
	DeleteObject(m_bgBrush); 
	SAFE_DELETE_ARRAY( m_pImageData );
}

VOID CDlgTextureInfoEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_A, m_checkBox_A);
}

BEGIN_MESSAGE_MAP(CDlgTextureInfoEdit, CDialog)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BTN_SELPIC, &CDlgTextureInfoEdit::OnBnClickedBtnOPEN)
	ON_BN_CLICKED(IDC_CHECK_ALIGN, &CDlgTextureInfoEdit::OnBnClickedCheckAlign)
	ON_BN_CLICKED(IDC_BTN_OK, &CDlgTextureInfoEdit::OnBnClickedBtnOK)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CDlgTextureInfoEdit::OnBnClickedBtnCANCEL)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_CBN_EDITCHANGE(IDC_COMBO_ZOOM, &CDlgTextureInfoEdit::OnCbnEditchangeComboZoom)
	ON_CBN_SELCHANGE(IDC_COMBO_ZOOM, &CDlgTextureInfoEdit::OnCbnSelchangeComboZoom)
	ON_STN_CLICKED(IDC_STATIC_BGCOLOR, &CDlgTextureInfoEdit::OnStnClickedStaticBgcolor)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CDlgTextureInfoEdit 메시지 처리기입니다.

BOOL CDlgTextureInfoEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ClearBoxInfo();
	ClearMouseInfo();

	m_cDlgView.Create( CViewDialog::IDD, this );
	m_cDlgView.MoveWindow( 0, 0, 1024, 800 );

	CButton* pButton = ( CButton* )( GetDlgItem( IDC_CHECK_ALIGN ) );
	pButton->SetCheck( m_bAlignRect );

	m_checkBox_A.SetCheck( TRUE );

	{
		CComboBox *pCombo = (CComboBox*) ( GetDlgItem( IDC_COMBO_ZOOM ) );
		assert(pCombo&&"콘트롤이 존제하지 않습니다.");

		pCombo->ResetContent();
		for ( int i = 0; i < 10; i++ )
		{
			CString strTemp;
			strTemp.Format( _T("%f") , ZoomList[ i ] );

			int nIndex = pCombo->AddString ( strTemp );
			pCombo->SetItemData ( nIndex, i );
		}
		pCombo->SetCurSel ( (int)m_fMultiply - 1 );
	}

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();
	if(tempUI != NULL)
	{
		SetTextureInfo(tempUI->GetTextureName(),tempUI->GetTexturePos().left,
			tempUI->GetTexturePos().top,tempUI->GetTexturePos().right,tempUI->GetTexturePos().bottom);
	}

	SetTimer( 0, 100, NULL );

	m_bgBrush = CreateSolidBrush(RGB( 0, 255, 255 ) );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

VOID CDlgTextureInfoEdit::OnTimer(UINT_PTR nIDEvent)
{
	if( IsWindowVisible() )
	{
		m_cDlgView.SetMultiply( m_fMultiply);
		m_cDlgView.SetStartPos(m_bMoveUPPoint.x * m_fMultiply, m_bMoveUPPoint.y * m_fMultiply);
		m_cDlgView.SetLimitRect( CRect(0,0,m_nPicWidth * m_fMultiply,m_nPicHeight * m_fMultiply) );
		m_cDlgView.SetRect( 
			CRect(
			m_cSelectRect.left * m_fMultiply,
			m_cSelectRect.top * m_fMultiply,
			m_cSelectRect.right * m_fMultiply,
			m_cSelectRect.bottom * m_fMultiply) );
		m_cDlgView.Render();
	}

	CDialog::OnTimer(nIDEvent);
}

VOID CDlgTextureInfoEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	CPoint ptMouse;
	ptMouse.x = min( point.x / m_fMultiply, m_nPicWidth );
	ptMouse.y = min( point.y / m_fMultiply, m_nPicHeight );
	ptMouse.x = max( point.x / m_fMultiply, 0 );
	ptMouse.y = max( point.y / m_fMultiply, 0 );

	int x = ptMouse.x - m_bMoveUPPoint.x;
	int y = ptMouse.y - m_bMoveUPPoint.y;

	x = min( x, m_nPicWidth );
	y = min( y, m_nPicHeight );
	x = max( x, 0 );
	y = max( y, 0 );

	m_bDNPoint.x = x;
	m_bDNPoint.y = y;

	m_bUPPoint.x = x;
	m_bUPPoint.y = y;

	MarkSelectRect();
	UpdateDlgCtrlRect();

	CDialog::OnLButtonDown(nFlags, point);
}

VOID CDlgTextureInfoEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint ptMouse;

	ptMouse.x = min( point.x / m_fMultiply, (float)m_nPicWidth  );
	ptMouse.y = min( point.y / m_fMultiply, (float)m_nPicHeight );
	ptMouse.x = max( point.x / m_fMultiply, 0 );
	ptMouse.y = max( point.y / m_fMultiply, 0 );

	int x = ptMouse.x - m_bMoveUPPoint.x;
	int y = ptMouse.y - m_bMoveUPPoint.y;

	x = min( x, (float)m_nPicWidth  );
	y = min( y, (float)m_nPicHeight );
	x = max( x, 0 );
	y = max( y, 0 );

	this->SetDlgItemInt( IDC_EDIT_MOUSE_X, x, FALSE );
	this->SetDlgItemInt( IDC_EDIT_MOUSE_Y, y, FALSE );

	if( GetCapture() == this )
	{
		m_bUPPoint.x = x;
		m_bUPPoint.y = y;

		MarkSelectRect();
		UpdateDlgCtrlRect();
	}

	if(m_bMoveTexture)
	{
		m_bMoveUPPoint.x += ptMouse.x - m_bMoveDNPoint.x;
		m_bMoveUPPoint.y += ptMouse.y - m_bMoveDNPoint.y;

		m_bMoveUPPoint.x = min( m_bMoveUPPoint.x, m_nPicWidth  );
		m_bMoveUPPoint.y = min( m_bMoveUPPoint.y, m_nPicHeight );
		m_bMoveUPPoint.x = max( m_bMoveUPPoint.x, -m_nPicWidth );
		m_bMoveUPPoint.y = max( m_bMoveUPPoint.y, -m_nPicHeight );

		m_bMoveDNPoint = ptMouse;
	}

	CDialog::OnMouseMove(nFlags, point);
}

VOID CDlgTextureInfoEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( GetCapture() == this )
	{
		CPoint ptMouse;

		ptMouse.x = min( point.x / m_fMultiply, (float)m_nPicWidth );
		ptMouse.y = min( point.y / m_fMultiply, (float)m_nPicHeight);
		ptMouse.x = max( point.x / m_fMultiply, 0 );
		ptMouse.y = max( point.y / m_fMultiply, 0 );


		int x = ptMouse.x - m_bMoveUPPoint.x;
		int y = ptMouse.y - m_bMoveUPPoint.y;

		x = min( x, (float)m_nPicWidth );
		y = min( y, (float)m_nPicHeight );
		x = max( x, 0 );
		y = max( y, 0 );

		m_bUPPoint.x = x;
		m_bUPPoint.y = y;

		MarkSelectRect();

		if( m_bAlignRect )
			AlignTextureRect();

		UpdateDlgCtrlRect();
	}

	ReleaseCapture();

	CDialog::OnLButtonUp(nFlags, point);
}

VOID CDlgTextureInfoEdit::OnBnClickedBtnOPEN() // 텍스쳐 파일 선택
{
	CString szFilter = _T("Texture file  (*.dds,*.bmp)|*.dds;*bmp|");
	CFileDialog dlg ( TRUE,_T(".dds"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

	CString strInitDir = TextureManager::GetInstance().GetPath().c_str();
	strInitDir += _T("\\gui");
	
	dlg.m_ofn.lpstrInitialDir = strInitDir;
	if ( dlg.DoModal() == IDOK )
	{
		m_cDlgView.SetTexture( TextureResource() );

		m_strTextureFile.Empty();
		m_nPicWidth = 0;
		m_nPicHeight = 0;

        TextureResource textureRes = TextureManager::GetInstance().LoadTexture(
            std::string(dlg.GetFileName().GetString()),
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            1,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_UI,
            false,
			FALSE );

		if( textureRes.GetTexture() )
		{
			HRESULT hrImage;
			D3DXIMAGE_INFO stImageInfo;
			LPDIRECT3DTEXTUREQ pTextureImage = NULL;
			hrImage = D3DXCreateTextureFromFileEx ( DXUTGetD3DDevice(), dlg.GetFileName(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
				(D3DFORMAT)D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
				D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &stImageInfo, NULL, &pTextureImage );
			if( !pTextureImage )
				return;

			m_pTextureImage = pTextureImage;
			memcpy(&m_stImageInfo, &stImageInfo, sizeof( D3DXIMAGE_INFO ) );

			m_cDlgView.SetTexture( textureRes );

			m_strTextureFile = dlg.GetFileName();
			m_nPicWidth = stImageInfo.Width;
			m_nPicHeight = stImageInfo.Height;
		}
	}

	ClearBoxInfo();
	ClearMouseInfo();
}

VOID CDlgTextureInfoEdit::OnBnClickedCheckAlign()
{
	CButton* pButton = ( CButton* )( GetDlgItem( IDC_CHECK_ALIGN ) );
	m_bAlignRect = pButton->GetCheck();
}

VOID CDlgTextureInfoEdit::OnBnClickedBtnOK() 
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();

	if( tempUI != NULL )
	{
		UIRECT rcTemp( 0, 0, 1, 1 );

		if( m_nControlType == PICURE_CON )
		{
			tempUI->SetTextureName( m_strTextureFile );
			tempUI->SetTexturePos( rcTemp );
			tempUI->SetTextureSize( m_nPicWidth, m_nPicHeight );
			tempUI->SetUseRender( m_strTextureFile.GetLength() );
			tempUI->SetTexturePos
				(
				UIRECT
				(
				FLOAT( m_cSelectRect.left ) / m_nPicWidth,
				FLOAT( m_cSelectRect.top ) / m_nPicHeight,
				FLOAT( m_cSelectRect.Width() ) / m_nPicWidth,
				FLOAT( m_cSelectRect.Height() ) / m_nPicHeight
				)
				);
			pInterfaceAdmin->m_pMainDlg->SetSelectedUITextureInfo(m_strTextureFile);
		}
		else if( m_nControlType == BUTTON_IN_CON )
		{
			CUIEditButton* pWin = ( CUIEditButton* ) (tempUI);

			pWin->m_pMouseInFlip->SetTextureName( m_strTextureFile );
			pWin->m_pMouseInFlip->SetTexturePos( rcTemp );
			pWin->m_pMouseInFlip->SetTextureSize( m_nPicWidth, m_nPicHeight );
			pWin->m_pMouseInFlip->SetUseRender( m_strTextureFile.GetLength() );
			pWin->m_pMouseInFlip->SetTexturePos
				(
				UIRECT
				(
				FLOAT( m_cSelectRect.left ) / m_nPicWidth,
				FLOAT( m_cSelectRect.top ) / m_nPicHeight,
				FLOAT( m_cSelectRect.Width() ) / m_nPicWidth,
				FLOAT( m_cSelectRect.Height() ) / m_nPicHeight
				)
				);
			pInterfaceAdmin->m_pMainDlg->SetSelectedUIButtonIn(m_strTextureFile);	
		}
		else if( m_nControlType == BUTTON_CLICK_CON )
		{
			CUIEditButton* pWin = ( CUIEditButton* ) (tempUI);

			pWin->m_pMouseClickFlip->SetTextureName( m_strTextureFile );
			pWin->m_pMouseClickFlip->SetTexturePos( rcTemp );
			pWin->m_pMouseClickFlip->SetTextureSize( m_nPicWidth, m_nPicHeight );
			pWin->m_pMouseClickFlip->SetUseRender( m_strTextureFile.GetLength() );
			pWin->m_pMouseClickFlip->SetTexturePos
				(
				UIRECT
				(
				FLOAT( m_cSelectRect.left ) / m_nPicWidth,
				FLOAT( m_cSelectRect.top ) / m_nPicHeight,
				FLOAT( m_cSelectRect.Width() ) / m_nPicWidth,
				FLOAT( m_cSelectRect.Height() ) / m_nPicHeight
				)
				);
			pInterfaceAdmin->m_pMainDlg->SetSelectedUIButtonClick(m_strTextureFile);	
		}
		else if( m_nControlType == ANIMATION_CON )
		{
			//CUIEditAnimation* pWin = dynamic_cast<CUIEditAnimation*> (tempUI);
			//pWin->SetAni(
			//	m_nAniIndex,
			//	m_strTextureFile,
			//	UIRECT (
			//	FLOAT( m_cSelectRect.left ) / m_nPicWidth,
			//	FLOAT( m_cSelectRect.top ) / m_nPicHeight,
			//	FLOAT( m_cSelectRect.Width() ) / m_nPicWidth,
			//	FLOAT( m_cSelectRect.Height() ) / m_nPicHeight
			//	),m_nPicWidth, m_nPicHeight);
			////pWin->AddAni(m_strTextureFile,UIRECT
			////	(
			////	FLOAT( m_cSelectRect.left ) / m_nPicWidth,
			////	FLOAT( m_cSelectRect.top ) / m_nPicHeight,
			////	FLOAT( m_cSelectRect.Width() ) / m_nPicWidth,
			////	FLOAT( m_cSelectRect.Height() ) / m_nPicHeight
			////	),m_nPicWidth, m_nPicHeight);
		}

		if( m_nControlType == PICURE_CON )
		{
			rcTemp = tempUI->GetLocalPos();
			rcTemp.sizeX = m_cSelectRect.Width();
			rcTemp.sizeY = m_cSelectRect.Height();
			rcTemp.right = rcTemp.left + rcTemp.sizeX;
			rcTemp.bottom = rcTemp.top + rcTemp.sizeY;
			tempUI->SetLocalPos(rcTemp);
			tempUI->ResizeWindow( rcTemp );
			tempUI->InitDeviceObjects( DXUTGetD3DDevice() );
			tempUI->ResizeWindow( rcTemp );
			tempUI->RestoreDeviceObjects( DXUTGetD3DDevice() );

		}
		else tempUI->InitDeviceObjects( DXUTGetD3DDevice() );

		CUIEditGroup* pSelected = pInterfaceAdmin->GetSelectedControl();
		INT iFindUIList = pInterfaceAdmin->FindUIListIndex( pSelected );
		pInterfaceAdmin->m_pUIContainer[ iFindUIList ].nTextureWidth  = m_nPicWidth;
		pInterfaceAdmin->m_pUIContainer[ iFindUIList ].nTextureHeight = m_nPicHeight;
	}

	ShowWindow( SW_HIDE );

	ClearBoxInfo();
	ClearMouseInfo();

	m_cDlgView.CloseWindow();


	if(pInterfaceAdmin->m_pMainDlg->m_CheckZoom.GetCheck())
	{
		CMainFrame* pFrame = ( CMainFrame* )AfxGetApp()->m_pMainWnd;
		if( pFrame )
			pFrame->m_cDlgMainView.ShowWindow( SW_SHOWNOACTIVATE );
	}
}

VOID CDlgTextureInfoEdit::OnBnClickedBtnCANCEL() 
{
	ShowWindow( SW_HIDE );

	ClearBoxInfo();
	ClearMouseInfo();

	m_cDlgView.CloseWindow();

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	if(pInterfaceAdmin->m_pMainDlg->m_CheckZoom.GetCheck())
	{
		CMainFrame* pFrame = ( CMainFrame* )AfxGetApp()->m_pMainWnd;
		if( pFrame )
			pFrame->m_cDlgMainView.ShowWindow( SW_SHOWNOACTIVATE );
	}
}

VOID CDlgTextureInfoEdit::SetTextureInfo( CString strText, FLOAT fLeft, FLOAT ftop, FLOAT fRight, FLOAT fBottom )
{
	ClearBoxInfo();
	ClearMouseInfo();

    TextureResource textureRes = TextureManager::GetInstance().LoadTexture(
        strText.GetString(),
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        1,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_UI,
        false,
		FALSE );

	if( textureRes.GetTexture() )
	{
		m_strTextureFile = strText;
		m_nPicWidth = 0;
		m_nPicHeight = 0;

		HRESULT hrImage;
		D3DXIMAGE_INFO stImageInfo;
		LPDIRECT3DTEXTUREQ pTextureImage = NULL;
		hrImage = D3DXCreateTextureFromFileEx ( DXUTGetD3DDevice(), strText, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
			(D3DFORMAT)D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
			D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &stImageInfo, NULL, &pTextureImage );

        if( !pTextureImage )
        {
            m_cDlgView.SetTexture( TextureResource() );
			return;
        }

		m_pTextureImage = pTextureImage;
		memcpy(&m_stImageInfo, &stImageInfo, sizeof( D3DXIMAGE_INFO ) );

		m_cDlgView.SetTexture( textureRes );
		m_cDlgView.SetTextureSize( stImageInfo.Width, stImageInfo.Height );

		m_nPicWidth = stImageInfo.Width;
		m_nPicHeight = stImageInfo.Height;

		m_cSelectRect = CRect
			(
			LONG( fLeft * FLOAT( m_nPicWidth ) ),
			LONG( ftop * FLOAT( m_nPicHeight ) ),
			LONG( fRight * FLOAT( m_nPicWidth ) ),
			LONG( fBottom * FLOAT( m_nPicHeight ) )
			);

		AlignSelectRect();
		UpdateDlgCtrlRect();
	}
}

VOID CDlgTextureInfoEdit::ClearBoxInfo()
{
	m_cSelectRect = CRect( 0, 0, 0, 0 );

	UpdateDlgCtrlRect();
}

VOID CDlgTextureInfoEdit::ClearMouseInfo()
{
	ReleaseCapture();

	m_bDNPoint = CPoint( 0, 0 );
	m_bUPPoint = CPoint( 0, 0 );

	this->SetDlgItemInt( IDC_EDIT_MOUSE_X, 0, FALSE );
	this->SetDlgItemInt( IDC_EDIT_MOUSE_Y, 0, FALSE );
}

VOID CDlgTextureInfoEdit::UpdateDlgCtrlRect()
{
	this->SetDlgItemInt( IDC_EDIT_BOX_X, m_cSelectRect.left, FALSE );
	this->SetDlgItemInt( IDC_EDIT_BOX_Y, m_cSelectRect.top, FALSE );
	this->SetDlgItemInt( IDC_EDIT_BOX_X2, m_cSelectRect.right, FALSE );
	this->SetDlgItemInt( IDC_EDIT_BOX_Y2, m_cSelectRect.bottom, FALSE );
	this->SetDlgItemInt( IDC_EDIT_BOX_WIDTH, m_cSelectRect.Width(), FALSE );
	this->SetDlgItemInt( IDC_EDIT_BOX_HEIGHT, m_cSelectRect.Height(), FALSE );
}

VOID CDlgTextureInfoEdit::MarkSelectRect()
{
	m_cSelectRect = CRect
		(
		min( m_bDNPoint.x, m_bUPPoint.x ),
		min( m_bDNPoint.y, m_bUPPoint.y ),
		max( m_bDNPoint.x, m_bUPPoint.x ),
		max( m_bDNPoint.y, m_bUPPoint.y )
		);
}

VOID CDlgTextureInfoEdit::AlignSelectRect()
{
	m_cSelectRect = CRect
		(
		min( m_cSelectRect.left, m_cSelectRect.right ),
		min( m_cSelectRect.top, m_cSelectRect.bottom ),
		max( m_cSelectRect.left, m_cSelectRect.right ),
		max( m_cSelectRect.top, m_cSelectRect.bottom )
		);
}

VOID CDlgTextureInfoEdit::AlignTextureRect()
{
	if( !m_pTextureImage )
		return;

	CRect cAlignRect
		(
		m_cSelectRect.right,
		m_cSelectRect.bottom,
		m_cSelectRect.left,
		m_cSelectRect.top
		);

	HRESULT hr = m_pTextureImage->LockRect( 0, &m_stLocked, NULL, 0 );
	if ( FAILED(hr) )
	{
		SAFE_DELETE_ARRAY( m_pImageData );
		return;	// intel 칩의 구형 그래픽드라이버 사용되면 오류가 발생할 수 있음.
	}

	for( LONG lY = m_cSelectRect.top; lY < m_cSelectRect.bottom; ++lY )
	{
		for( LONG lX = m_cSelectRect.left; lX < m_cSelectRect.right; ++lX )
		{
			D3DXCOLOR stColor =GetTexelData( INT( lX ), INT( lY ) );

			if( m_checkBox_A.GetCheck() )
			{
				if( stColor.r == 0.0f && stColor.g == 0.0f && stColor.b == 0.0f && stColor.a == 0.0f )
					continue;
			}
			else
			{
				if( stColor.r == 0.0f && stColor.g == 0.0f && stColor.b == 0.0f )
					continue;
			}


			cAlignRect.left = min( cAlignRect.left, lX );
			cAlignRect.top = min( cAlignRect.top, lY );
			cAlignRect.right = max( cAlignRect.right, lX + 1);
			cAlignRect.bottom = max( cAlignRect.bottom, lY + 1);
		}
	}

	m_cSelectRect = cAlignRect;

	AlignSelectRect();

	m_pTextureImage->UnlockRect( 0 );

	SAFE_DELETE_ARRAY( m_pImageData );
}

void CDlgTextureInfoEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bMoveTexture = TRUE;
	m_bMoveDNPoint = CPoint( point.x / m_fMultiply , point.y / m_fMultiply );

	CDialog::OnRButtonDown(nFlags, point);
}

void CDlgTextureInfoEdit::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_bMoveTexture = FALSE;
	m_bMoveDNPoint = CPoint( 0, 0 );
	//m_bMoveUPPoint = CPoint( 0, 0 );

	CDialog::OnRButtonUp(nFlags, point);
}

void CDlgTextureInfoEdit::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bMoveTexture = FALSE;
	m_bMoveDNPoint = CPoint( 0, 0 );
	m_bMoveUPPoint = CPoint( 0, 0 );

	CDialog::OnMButtonDown(nFlags, point);
}

void CDlgTextureInfoEdit::OnBnClickedCheckScale1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fMultiply = 1.0f;
	m_cDlgView.SetMultiply(m_fMultiply);
}

void CDlgTextureInfoEdit::OnBnClickedCheckScale2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fMultiply = 1.5f;
	m_cDlgView.SetMultiply(m_fMultiply);
}

void CDlgTextureInfoEdit::OnBnClickedCheckScale3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fMultiply = 2.0f;
	m_cDlgView.SetMultiply( m_fMultiply );
}


void CDlgTextureInfoEdit::OnCbnEditchangeComboZoom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nSel = ( ( CComboBox* )GetDlgItem ( IDC_COMBO_ZOOM ) )->GetCurSel ();

	m_fMultiply = ZoomList[ nSel ];

	m_cDlgView.SetMultiply( m_fMultiply );
}


void CDlgTextureInfoEdit::OnCbnSelchangeComboZoom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fMultiply = ((CComboBox*)GetDlgItem ( IDC_COMBO_ZOOM ))->GetCurSel () + 1;
	m_cDlgView.SetMultiply(m_fMultiply);
}

void CDlgTextureInfoEdit::OnStnClickedStaticBgcolor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CColorDialog	dlgC(m_cDlgView.GetBgColor());
	if(dlgC.DoModal() == IDOK)
	{
		COLORREF SelectedColor = dlgC.GetColor();

		m_cDlgView.SetBgColor(GetRValue(SelectedColor), GetGValue(SelectedColor), GetBValue(SelectedColor));
		DeleteObject(m_bgBrush); 
		m_bgBrush = CreateSolidBrush(SelectedColor);

		UpdateData(FALSE);

		Invalidate();
	}
}

HBRUSH CDlgTextureInfoEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if(pWnd->GetDlgCtrlID() == IDC_STATIC_BGCOLOR)
	{
		pDC->SetBkColor(m_cDlgView.GetBgColor());
		return m_bgBrush;
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

D3DXCOLOR CDlgTextureInfoEdit::GetTexelData( INT iX, INT iY )
{
	if( !m_pTextureImage )
		return D3DXCOLOR( DWORD( 0 ) );

	if( iX >= (INT)m_stImageInfo.Width || iY >= (INT)m_stImageInfo.Height )
		return D3DXCOLOR( DWORD( 0 ) );

	DWORD *dwReturn = ( DWORD* )m_stLocked.pBits;

	return D3DXCOLOR( dwReturn[ iX + iY * m_stImageInfo.Height ] );
}