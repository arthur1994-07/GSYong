// DlgEdit.cpp : implementation file
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../Properties/SheetWithTabChar.h"

#include "DlgEdit.h"
#include ".\dlgedit.h"

#pragma warning(disable:4996)

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CDlgEdit dialog
CDlgEdit::CDlgEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEdit::IDD, pParent),
	m_psheetWithTab(NULL)
{
	//{{AFX_DATA_INIT(CDlgEdit)
	//}}AFX_DATA_INIT
}


void CDlgEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEdit)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_PAGE, m_comboPage);
}


BEGIN_MESSAGE_MAP(CDlgEdit, CDialog)
	//{{AFX_MSG_MAP(CDlgEdit)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_PAGE, OnCbnSelchangeComboPage)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEdit message handlers

void CDlgEdit::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

BOOL CDlgEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect SheetRect;
	CWnd *pWnd;

	//	Note : CsheetWithTabChar 를 만든다.
	//
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP1 );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	m_psheetWithTab = new CsheetWithTabChar ( SheetRect, this );
	m_psheetWithTab->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );

	m_comboPage.ResetContent ();
	
	int nIndex = 0;

#ifndef DEF_SKIN_SYNC
	for ( int i=0; i<CHARTAB_EFFECT_SINGLE; ++i )
	{
		nIndex = m_comboPage.AddString ( CsheetWithTabChar::m_szCharTabPages[i] );
		m_comboPage.SetItemData ( nIndex, i );
	}
#else
    for ( int i=0; i<CHARTAB_SIZE; ++i )
    {
        nIndex = m_comboPage.AddString ( CsheetWithTabChar::m_szCharTabPages[i] );
        m_comboPage.SetItemData ( nIndex, i );
    }
#endif
	m_comboPage.SetCurSel ( 0 );


	//	Note : 케릭터-파일 탭 활성화.
	//
	m_psheetWithTab->SetActivePageTab ( CHARTAB_CHARFILE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEdit::SetActiveSheetCombo ( bool bActive )
{
	m_comboPage.EnableWindow ( bActive );
}

void CDlgEdit::SetActivePage_JiggleBone( int nPage, DxJiggleBone* pJiggleBone )
{
	assert(m_psheetWithTab);

	m_comboPage.SetFocus();
	m_comboPage.SetActiveWindow ();
	m_psheetWithTab->SetActivePage_JiggleBone ( nPage, pJiggleBone );
}

void CDlgEdit::SetActivePage_JiggleBoneColl( int nPage, DxJiggleBoneColl* pJiggleBoneColl )
{
	assert(m_psheetWithTab);

	m_comboPage.SetFocus();
	m_comboPage.SetActiveWindow ();
	m_psheetWithTab->SetActivePage_JiggleBoneColl( nPage, pJiggleBoneColl );
}

void CDlgEdit::SetActivePage_ClothColl( int nPage, DxClothColl* pClothColl )
{
	assert(m_psheetWithTab);

	m_comboPage.SetFocus();
	m_comboPage.SetActiveWindow ();
	m_psheetWithTab->SetActivePage_ClothColl( nPage, pClothColl );
}

void CDlgEdit::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	SAFE_DELETE(m_psheetWithTab);
	
	CDialog::PostNcDestroy();
}


void CDlgEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgEdit::AnimationListing ()
{	
	//if ( m_psheetWithTab2 )
	//	m_psheetWithTab2->m_AnimPage.AnimationListing();
}

void CDlgEdit::ReFlash ()
{
	//if ( m_psheetWithTab )
	//	m_psheetWithTab->m_FramePage.ReTreeFrame();

	//if ( m_psheetWithTab2 )
	//	m_psheetWithTab2->m_AnimPage.AnimationListing();
}


void CDlgEdit::OnCbnSelchangeComboPage()
{
	assert(m_psheetWithTab);

	int nIndex = m_comboPage.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = m_comboPage.GetItemData ( nIndex );
	m_psheetWithTab->SetActivePageTab ( dwData );
}

void CDlgEdit::SetAcitveSheetPage ( int nPage )
{
	assert(m_psheetWithTab);

	m_comboPage.SetFocus();
	m_comboPage.SetActiveWindow ();
	m_psheetWithTab->SetActivePageTab ( nPage );
	m_comboPage.SetCurSel( nPage );
}

// User 가 TimeLine 의 시간을 변경한다면 그와 연관된 작업을 한다.
void CDlgEdit::ChangeTimeOfTimeLine( DWORD dwToTime )
{
	int nIndex = GetActiveIndex();
	m_psheetWithTab->ChangeTimeOfTimeLine( nIndex, dwToTime );
}

// User 가 CDlgTimeLine 의 OnButtonPlay 을 누른다면 그와 연관된 작업을 한다.
void CDlgEdit::OnButtonPlayOfCDlgTimeLine()
{
	int nIndex = GetActiveIndex();
	m_psheetWithTab->OnButtonPlayOfCDlgTimeLine( nIndex );
}

void CDlgEdit::SetSpinControl( int nRadius, int nHeight, int nSpin3, int nSpin4, int nColorHeight )
{
	int nIndex = GetActiveIndex();

#ifndef DEF_SKIN_SYNC
	switch( nIndex )
	{
	case CHARTAB_CHARFILE:
	case CHARTAB_OVERLAY: // by luxes.
	case CHARTAB_BILLBOARD:
		m_psheetWithTab->SetSpinControl( nIndex, nRadius, nHeight, nSpin3, nSpin4, nColorHeight );
		break;
	}
#else
    if (m_psheetWithTab)
    {
        m_psheetWithTab->SetSpinControl( nIndex, nRadius, nHeight, nColorHeight );
    }
#endif	
}

void CDlgEdit::ResetDialog()
{
    int nIndex = GetActiveIndex();
    if( m_psheetWithTab )	
    {
        m_psheetWithTab->ResetDialog( nIndex );
    }
}

void CDlgEdit::UpdateKeyBoardMouse()
{
    int nIndex = GetActiveIndex();

    if( m_psheetWithTab )	
    {
        m_psheetWithTab->UpdateKeyBoardMouse( nIndex );
    }
}

int CDlgEdit::GetActiveIndex()
{
	if( m_psheetWithTab )	return m_psheetWithTab->GetActiveIndex();
	else					return -1;
}



void CDlgEdit::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	char szTempChar[MAX_PATH] ={0};
	DWORD dwNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );

	DragQueryFile ( hDropInfo, 0, szTempChar, MAX_PATH );

	char dir[_MAX_DIR] = {0};
	char name[_MAX_FNAME] = {0};
	char ext[_MAX_EXT] = {0};
	_splitpath( szTempChar, NULL, dir, name, ext );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();

	CString strExt(ext);
	strExt.MakeUpper();

	CString strDir(dir);
	strDir.MakeUpper();

	CString strOpenName(name);
	strOpenName += ext;
	strOpenName.MakeLower();

	if( GetActiveIndex() == CHARTAB_CHARFILE )
	{		
		if( strExt == _T(".CHF") )
		{
			if( dwNumFiles != 1 )
			{
				MessageBox( "한 개의 파일만 선택해야 합니다", "알림" );
				return;
			}

			m_psheetWithTab->m_CharPage.LoadChfFile(strOpenName.GetString());
		}else if( strExt == _T(".CPS") )
		{			
			for( DWORD i = 0; i < dwNumFiles; i++ )
			{
				DragQueryFile ( hDropInfo, i, szTempChar, MAX_PATH );
				_splitpath( szTempChar, NULL, dir, name, ext );

				strOpenName = name;
				strOpenName += ext;
				strOpenName.MakeLower();				
				m_psheetWithTab->m_CharPage.InsertSkinPiece(strOpenName.GetString());
			}

		}else if( strExt == _T(".ABL") )
		{			
			for( DWORD i = 0; i < dwNumFiles; i++ )
			{
				DragQueryFile ( hDropInfo, i, szTempChar, MAX_PATH );
				_splitpath( szTempChar, NULL, dir, name, ext );

				strOpenName = name;
				strOpenName += ext;
				strOpenName.MakeLower();				
				m_psheetWithTab->m_CharPage.InsertSkinPiece(strOpenName.GetString());
			}

		}else if( strExt == _T(".EFFSKIN_A") )
		{
			for( DWORD i = 0; i < dwNumFiles; i++ )
			{
				DragQueryFile ( hDropInfo, i, szTempChar, MAX_PATH );
				_splitpath( szTempChar, NULL, dir, name, ext );

				strOpenName = name;
				strOpenName += ext;
				strOpenName.MakeLower();

				m_psheetWithTab->m_CharPage.InsertSkinEff(strOpenName.GetString());
			}
		}else if( strExt == _T(".X") )
		{
			if( strstr( strDir.GetString(), "SKELETON" ) )
			{
				if( dwNumFiles != 1 )
				{
					MessageBox( "한 개의 파일만 선택해야 합니다", "알림" );
					return;
				}

				m_psheetWithTab->m_CharPage.LoadSkinBone(strOpenName.GetString());
			}else if( strstr( strDir.GetString(), "ANIMATION" ) )
			{
				for( DWORD i = 0; i < dwNumFiles; i++ )
				{
					DragQueryFile ( hDropInfo, i, szTempChar, MAX_PATH );
					_splitpath( szTempChar, NULL, dir, name, ext );

					strOpenName = name;
					strOpenName += ext;
					strOpenName.MakeLower();

					m_psheetWithTab->m_CharPage.LoadAnimation(strOpenName.GetString());
				}
			}
		}
	}
#ifndef DEF_SKIN_SYNC
    else if( GetActiveIndex() == CHARTAB_CHARPIECE )
	{

		if( dwNumFiles != 1 )
		{
			MessageBox( "한 개의 파일만 선택해야 합니다", "알림" );
			return;
		}

		if( strExt == _T(".CPS") )
		{
			m_psheetWithTab->m_SkinPiecePage.LoadSkinPiece(strOpenName.GetString());
		}else if( strExt == _T(".X") ){
			if( strstr( strDir.GetString(), "SKIN" ) )
			{
				m_psheetWithTab->m_SkinPiecePage.SetEditFileName(strOpenName.GetString(),FALSE);
			}else if( strstr( strDir.GetString(), "SKELETON" ) ){
				m_psheetWithTab->m_SkinPiecePage.SetEditFileName(strOpenName.GetString(),TRUE);
			}
		}
	}
    else if( strExt == _T(".SOB") )
	{
		SetAcitveSheetPage( CHARTAB_CHAROBJ );	//CHARTAB_CHAROBJ 2
		m_psheetWithTab->m_SkinObjPage.LoadSkinObject(strOpenName.GetString());
		m_comboPage.SetCurSel(2);
	}
#endif

	pView->SetFocus();


	CDialog::OnDropFiles(hDropInfo);
}
