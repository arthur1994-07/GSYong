// DlgTimeLine.cpp : implementation file
//

#include "stdafx.h"
#include "../charedit.h"

#include "../EngineLib/DxCommon/d3dapp.h"
#include "../EngineLib/DxCommon/dxutil.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxTools/d3dfont.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "DlgTimeLine.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern ANIMATE g_Animate;

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeLine dialog


CDlgTimeLine::CDlgTimeLine(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTimeLine::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTimeLine)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgTimeLine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTimeLine)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTimeLine, CDialog)
	//{{AFX_MSG_MAP(CDlgTimeLine)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_TOS, OnButtonTos)
	ON_BN_CLICKED(IDC_BUTTON_PKEY, OnButtonPkey)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_NKEY, OnButtonNkey)
	ON_BN_CLICKED(IDC_BUTTON_EKEY, OnButtonEkey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeLine message handlers

BOOL CDlgTimeLine::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWin_Num_float( this, IDC_EDIT_DELAY, 1.0f );
	
	m_TimeLine.SubclassDlgItem ( IDC_STATIC_TIMELINE, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTimeLine::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	//	Note : 좌측 정렬 콘트롤.
	//
	CRect rcWnd;
	GetWindowRect ( rcWnd );
	ScreenToClient ( &rcWnd );

	DWORD RIDs[6] = {
		IDC_EDIT_ANIKEY,
		IDC_BUTTON_TOS,
		IDC_BUTTON_PKEY,
		IDC_BUTTON_PLAY,
		IDC_BUTTON_NKEY,
		IDC_BUTTON_EKEY
	};

	CWnd *pWnds[6];
	
	for ( int i=0; i<6; ++i )
	{
		pWnds[i] = GetDlgItem ( RIDs[i] );

		if ( !pWnds[i] )	return;
	}

	CRect rcControl;
	pWnds[5]->GetWindowRect ( rcControl );
	ScreenToClient ( &rcControl );

	int xStep = rcWnd.right - rcControl.right - 10;

	if ( rcWnd.right )
	{
		for ( int i=0; i<6; ++i )
		{
			pWnds[i]->GetWindowRect ( rcControl );
			ScreenToClient ( &rcControl );

			rcControl.left += xStep;
			rcControl.right += xStep;

			pWnds[i]->MoveWindow ( &rcControl );
		}
	}

	//	Note : 리사이즈 콘트롤.
	//
	if ( rcWnd.right )
	{
		CWnd *pWnd = GetDlgItem ( IDC_STATIC_TIMELINE );
		pWnd->GetWindowRect ( rcControl );
		ScreenToClient ( &rcControl );
		rcControl.right += xStep;

		pWnd->MoveWindow ( &rcControl );
	}
}

void CDlgTimeLine::SetTimeLine ( DWORD dwStart, DWORD dwEnd, DWORD dwUnitTime, BYTE aniType /*= AM_STOP*/ )
{
	g_Animate = (ANIMATE)aniType;

	m_TimeLine.SetTimeState ( dwStart, dwEnd );

	CString Str;
	Str.Format ( "%d", dwStart );

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_ANISKEY );
	pWnd->SetWindowText ( Str );

	Str.Format ( "%d", dwEnd );

	pWnd = GetDlgItem ( IDC_EDIT_ANIEKEY );
	pWnd->SetWindowText ( Str );


	Str.Format ( "%d", (DWORD)(UNITANIKEY_PERSEC/dwUnitTime) );

	pWnd = GetDlgItem ( IDC_EDIT_FPS );
	pWnd->SetWindowText ( Str );


	CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );
	if ( g_Animate == AM_PLAY )
	{
		pButton->SetWindowText ( "||" );
	}
	else
	{
		pButton->SetWindowText ( ">" );
	}
}

void CDlgTimeLine::SetTimeState ( DWORD dwToTime )
{
	static DWORD dwToTimeBack = 0;

	if ( dwToTimeBack != dwToTime )
	{
		m_TimeLine.SetTimeToTime ( dwToTime );
		dwToTimeBack = dwToTime;
	}

	CString Str;
	Str.Format ( "%d", dwToTime );

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_ANIKEY );
	
	CString StrBack;
	pWnd->GetWindowText ( StrBack );

	if ( Str != StrBack )
		pWnd->SetWindowText ( Str );
}

float CDlgTimeLine::GetAniDelay()
{
	return GetWin_Num_float( this, IDC_EDIT_DELAY );
}

void CDlgTimeLine::SetPlay()
{
	CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );
	pButton->SetWindowText ( "||" );
	g_Animate = AM_PLAY;
}

void CDlgTimeLine::SetMultiAnimPlay()
{
	CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );
	pButton->SetWindowText ( "||" );
	g_Animate = AM_MULTIPLAY;
}

void CDlgTimeLine::SetStop()
{
	CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );
	pButton->SetWindowText ( ">" );
	g_Animate = AM_STOP;
}


void CDlgTimeLine::OnButtonTos() 
{
	// TODO: Add your control notification handler code here
	if ( g_Animate == AM_PLAY )
	{
		CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );
		pButton->SetWindowText ( ">" );
	}

	g_Animate = AM_STARTKEY;
}

void CDlgTimeLine::OnButtonPkey() 
{
	// TODO: Add your control notification handler code here
	if ( g_Animate == AM_PLAY )
	{
		CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );
		pButton->SetWindowText ( ">" );
	}

	g_Animate = AM_PREVKEY;
}

void CDlgTimeLine::OnButtonPlay() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );

	if ( g_Animate == AM_PLAY )
	{
		pButton->SetWindowText ( ">" );
		g_Animate = AM_STOP;
	}
	else
	{
		pButton->SetWindowText ( "||" );
		g_Animate = AM_PLAY;

		// User 가 CDlgTimeLine 의 OnButtonPlay 을 누른다면 그와 연관된 작업을 한다.
		CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		pFrame->OnButtonPlayOfCDlgTimeLine();
	}
}

void CDlgTimeLine::OnButtonNkey() 
{
	// TODO: Add your control notification handler code here
	if ( g_Animate == AM_PLAY )
	{
		CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );
		pButton->SetWindowText ( ">" );
	}

	g_Animate = AM_NEXTKEY;
}

void CDlgTimeLine::OnButtonEkey() 
{
	// TODO: Add your control notification handler code here
	if ( g_Animate == AM_PLAY )
	{
		CButton *pButton = (CButton*) GetDlgItem ( IDC_BUTTON_PLAY );
		pButton->SetWindowText ( ">" );
	}

	g_Animate = AM_ENDKEY;
}
