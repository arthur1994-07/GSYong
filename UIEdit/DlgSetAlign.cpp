#include "StdAfx.h"
#include "DlgSetAlign.h"

#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/d3dfont.h"

#include "UIEditDoc.h"
#include "UIEditView.h"
#include "MainFrm.h"

#include "../EngineLib/GUInterface/BasicButton.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../EngineLib/GUInterface/BasicTextButton.h"

#include "../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/BasicTextBoxEx.h"


#include "ExtendUI/UIEditGroup.h"
#include "MainUI/InterfaceAdmin.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CDlgSetAlign, CDialog)

CDlgSetAlign::CDlgSetAlign(CWnd* pParent /*=NULL*/)
: CDialog(CDlgSetAlign::IDD, pParent)

, m_nSelectPosX( -1 )
, m_nSelectPosY( -1 )
, m_nSelectSizeX( -1 )
, m_nSelectSizeY( -1 )
, m_nRadioPosX( -1 )
, m_nRadioPosY( -1 )
, m_nRadioSizeX( -1 )
, m_nRadioSizeY( -1 )
, m_nCheckBG( FALSE )
, m_nCheckWide( FALSE )
, m_nCheckBasicProp( FALSE )

{
}

CDlgSetAlign::~CDlgSetAlign()
{
}

void CDlgSetAlign::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_LEFT, m_nRadioPosX);
	DDX_Radio(pDX, IDC_RADIO_TOP, m_nRadioPosY);
	DDX_Radio(pDX, IDC_RADIO_SIZE_X, m_nRadioSizeX);
	DDX_Radio(pDX, IDC_RADIO_SIZE_Y, m_nRadioSizeY);
}

BOOL CDlgSetAlign::OnInitDialog()
{
	CDialog::OnInitDialog();

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();

	if( tempUI != NULL )
	{
		CButton *pCheck;
		DWORD dwAlignFlag = tempUI->GetAlignFlag();

		if( dwAlignFlag & UI_FLAG_LEFT )
		{
			pCheck = ( CButton* )GetDlgItem( IDC_RADIO_LEFT );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if( dwAlignFlag & UI_FLAG_RIGHT )
		{
			pCheck = ( CButton* )GetDlgItem( IDC_RADIO_RIGHT );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if( dwAlignFlag & UI_FLAG_CENTER_X )
		{
			pCheck = ( CButton* )GetDlgItem( IDC_RADIO_CENTER_X );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if( dwAlignFlag & UI_FLAG_TOP )
		{
			pCheck = ( CButton* )GetDlgItem( IDC_RADIO_TOP );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if( dwAlignFlag & UI_FLAG_BOTTOM )
		{
			pCheck = ( CButton* )GetDlgItem( IDC_RADIO_BOTTOM );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if( dwAlignFlag & UI_FLAG_CENTER_Y )
		{
			pCheck = ( CButton* )GetDlgItem( IDC_RADIO_CENTER_Y );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if(dwAlignFlag & UI_FLAG_XSIZE)
		{
			pCheck = ( CButton* )GetDlgItem( IDC_RADIO_SIZE_X );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if( dwAlignFlag & UI_FLAG_XSIZE_PROPORTION )
		{
			pCheck=( CButton* )GetDlgItem( IDC_RADIO_PROP_SIZE_X );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if( dwAlignFlag & UI_FLAG_YSIZE )
		{
			pCheck=(CButton *)GetDlgItem( IDC_RADIO_SIZE_Y );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
		if(dwAlignFlag & UI_FLAG_YSIZE_PROPORTION)
		{
			pCheck=( CButton* )GetDlgItem( IDC_RADIO_PROP_SIZE_Y );
			pCheck->SetCheck( TRUE );
			UpdateData( TRUE );
		}
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgSetAlign, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CDlgSetAlign::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_LEFT, &CDlgSetAlign::OnBnClickedRadioLeft)
	ON_BN_CLICKED(IDC_RADIO_RIGHT, &CDlgSetAlign::OnBnClickedRadioRight)
	ON_BN_CLICKED(IDC_RADIO_CENTER_X, &CDlgSetAlign::OnBnClickedRadioCenterX)
	ON_BN_CLICKED(IDC_RADIO_BOTTOM, &CDlgSetAlign::OnBnClickedRadioBottom)
	ON_BN_CLICKED(IDC_RADIO_CENTER_Y, &CDlgSetAlign::OnBnClickedRadioCenterY)
	ON_BN_CLICKED(IDC_RADIO_PROP_SIZE_X, &CDlgSetAlign::OnBnClickedRadioPropSizeX)
	ON_BN_CLICKED(IDC_RADIO_SIZE_Y, &CDlgSetAlign::OnBnClickedRadioSizeY)
	ON_BN_CLICKED(IDC_RADIO_PROP_SIZE_Y, &CDlgSetAlign::OnBnClickedRadioPropSizeY)
END_MESSAGE_MAP()
void CDlgSetAlign::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
}

void CDlgSetAlign::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelectPosX = GetCheckedRadioButton(IDC_RADIO_LEFT, IDC_RADIO_CENTER_X);
	m_nSelectPosY = GetCheckedRadioButton(IDC_RADIO_TOP, IDC_RADIO_CENTER_Y);
	m_nSelectSizeX = GetCheckedRadioButton(IDC_RADIO_SIZE_X, IDC_RADIO_PROP_SIZE_X);
	m_nSelectSizeY= GetCheckedRadioButton(IDC_RADIO_SIZE_Y, IDC_RADIO_PROP_SIZE_Y);

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	CUIEditGroup* tempUI = pInterfaceAdmin->GetSelectedControl();

	if(tempUI != NULL)
	{
		DWORD dwAlignFlag = -1;

		if( m_nSelectPosX == IDC_RADIO_LEFT )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_LEFT;
			else dwAlignFlag |= UI_FLAG_LEFT;
		}

		if( m_nSelectPosX == IDC_RADIO_RIGHT )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_RIGHT;
			else dwAlignFlag |= UI_FLAG_RIGHT;
		}

		if( m_nSelectPosX == IDC_RADIO_CENTER_X )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_CENTER_X;
			else dwAlignFlag |= UI_FLAG_CENTER_X;
		}

		if( m_nSelectPosY == IDC_RADIO_TOP )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_TOP;
			else dwAlignFlag |= UI_FLAG_TOP;
		}

		if( m_nSelectPosY == IDC_RADIO_BOTTOM )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_BOTTOM;
			else dwAlignFlag |= UI_FLAG_BOTTOM;
		}

		if (m_nSelectPosY == IDC_RADIO_CENTER_Y )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_CENTER_Y;
			else dwAlignFlag |= UI_FLAG_CENTER_Y;
		}

		if( m_nSelectSizeX == IDC_RADIO_SIZE_X )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_XSIZE;
			else dwAlignFlag |= UI_FLAG_XSIZE;
		}

		if( m_nSelectSizeX == IDC_RADIO_PROP_SIZE_X )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_XSIZE_PROPORTION;
			else dwAlignFlag |= UI_FLAG_XSIZE_PROPORTION;
		}

		if( m_nSelectSizeY == IDC_RADIO_SIZE_Y )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_YSIZE;
			else dwAlignFlag |= UI_FLAG_YSIZE;
		}

		if( m_nSelectSizeY == IDC_RADIO_PROP_SIZE_Y )
		{
			if(dwAlignFlag == -1) dwAlignFlag = UI_FLAG_YSIZE_PROPORTION;
			else dwAlignFlag |= UI_FLAG_YSIZE_PROPORTION;
		}

		//CButton *pCheck;
		//BOOL bCheck;

		//pCheck = ( CButton* )GetDlgItem( IDC_CHECK_BG );
		//bCheck = pCheck->GetCheck();
		//if( bCheck )
		//{
		//	if( dwAlignFlag == -1 ) dwAlignFlag = UI_FLAG_BACKGROUND;
		//	else dwAlignFlag |= UI_FLAG_BACKGROUND;
		//}

		//pCheck = ( CButton* )GetDlgItem( IDC_CHECK_WIDE );
		//bCheck = pCheck->GetCheck();
		//if( bCheck )
		//{
		//	if( dwAlignFlag == -1 ) dwAlignFlag = UI_FLAG_WIDE;
		//	else dwAlignFlag |= UI_FLAG_WIDE;
		//}

		//pCheck=( CButton* )GetDlgItem( IDC_CHECK_BASIC_PROP );
		//bCheck = pCheck->GetCheck();
		//if( bCheck )
		//{
		//	if( dwAlignFlag == -1 ) dwAlignFlag = UI_FLAG_BASIC_PROPORTION;
		//	else dwAlignFlag |= UI_FLAG_BASIC_PROPORTION;
		//}

		if( dwAlignFlag == -1 ) dwAlignFlag = UI_FLAG_DEFAULT;

		tempUI->ResetAlignFlag();
		tempUI->SetAlignFlag( dwAlignFlag );

		if( tempUI->GetParent() )
		{
			UIRECT rcParent = tempUI->GetParent()->GetGlobalPos();
			tempUI->AlignSubControlEX( rcParent, rcParent );
		}
		else 
		{
			UIRECT rcNewWindow( 0, 0, pInterfaceAdmin->m_nViewWidth, pInterfaceAdmin->m_nViewHeight );
			tempUI->AlignMainControlEX( rcNewWindow, rcNewWindow );
		}

		tempUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	}

	OnOK();
}

void CDlgSetAlign::OnBnClickedCheckBg()
{
	UpdateData(TRUE);
	if(m_nCheckBG == TRUE)
	{
		m_nRadioPosX = -1;
		m_nRadioPosY = -1;
		m_nRadioSizeX = -1;
		m_nRadioSizeY = -1;
		m_nCheckWide = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedCheckWide()
{
	UpdateData(TRUE);
	if(m_nCheckWide == TRUE)
	{
		m_nRadioSizeX = -1;
		m_nRadioSizeY = -1;
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedCheckBasicProp()
{
	UpdateData(TRUE);
	if(m_nCheckBasicProp == TRUE)
	{
		m_nRadioSizeX = -1;
		m_nRadioSizeY = -1;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioLeft()
{
	if(m_nRadioPosX == 0)
	{
		m_nRadioPosX = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioPosX != -1)
	{
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioRight()
{
	if(m_nRadioPosX == 1)
	{
		m_nRadioPosX = -1;
		UpdateData(FALSE);
	}


	UpdateData(TRUE);
	if(m_nRadioPosX != -1)
	{
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioCenterX()
{
	if(m_nRadioPosX == 2)
	{
		m_nRadioPosX = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioPosX != -1)
	{
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioPropX()
{
	if(m_nRadioPosX == 3)
	{
		m_nRadioPosX = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioPosX != -1)
	{
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioTop()
{
	if(m_nRadioPosY == 0)
	{
		m_nRadioPosY = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioPosY != -1)
	{
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioBottom()
{
	if(m_nRadioPosY == 1)
	{
		m_nRadioPosY = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioPosY != -1)
	{
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioCenterY()
{
	if(m_nRadioPosY == 2)
	{
		m_nRadioPosY = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioPosY != -1)
	{
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioPropY()
{
	if(m_nRadioPosY == 3)
	{
		m_nRadioPosY = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioPosY != -1)
	{
		m_nCheckBG = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioSizeX()
{
	if(m_nRadioSizeX == 0)
	{
		m_nRadioSizeX = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioSizeX != -1)
	{
		m_nCheckBG = FALSE;
		m_nCheckWide = FALSE;
		m_nCheckBasicProp = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioPropSizeX()
{
	if(m_nRadioSizeX == 1)
	{
		m_nRadioSizeX = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioSizeX != -1)
	{
		m_nCheckBG = FALSE;
		m_nCheckWide = FALSE;
		m_nCheckBasicProp = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioSizeY()
{
	if(m_nRadioSizeY == 0)
	{
		m_nRadioSizeY = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioSizeY != -1)
	{
		m_nCheckBG = FALSE;
		m_nCheckWide = FALSE;
		m_nCheckBasicProp = FALSE;
	}
	UpdateData(FALSE);
}

void CDlgSetAlign::OnBnClickedRadioPropSizeY()
{
	if(m_nRadioSizeY == 1)
	{
		m_nRadioSizeY = -1;
		UpdateData(FALSE);
	}

	UpdateData(TRUE);
	if(m_nRadioSizeY != -1)
	{
		m_nCheckBG = FALSE;
		m_nCheckWide = FALSE;
		m_nCheckBasicProp = FALSE;
	}
	UpdateData(FALSE);
}
