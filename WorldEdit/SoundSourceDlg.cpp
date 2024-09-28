// SoundSourceDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SoundSourceDlg.h"
#include "../EngineLib/DxSound/DxSoundMan.h"
#include "../EngineLib/DxTools/DxViewPort.h"


// CSoundSourceDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSoundSourceDlg, CDialog)
CSoundSourceDlg::CSoundSourceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundSourceDlg::IDD, pParent)
	, m_valKeyword(_T(""))
{
	m_pSound = NULL;
	m_pSoundSourceMan = NULL;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CSoundSourceDlg::~CSoundSourceDlg()
{
	if ( m_SoundSource.FileName.GetLength() )
	{
		if ( m_pSound )
		{
			m_pSound->StopBuffer ( m_BufferID );
			m_pSound = NULL;
			DxSoundMan::GetInstance().ReleaseWav ( m_SoundSource.FileName.GetString(), false );
		}
	}

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CSoundSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SOUNDSOURCE, m_ctrlSoundSource);	
	DDX_Control(pDX, IDC_COMBO_CATEGORY, m_ctrlCategory);
	DDX_Text(pDX, IDC_EDIT_KEYWORD, m_valKeyword);
}


BEGIN_MESSAGE_MAP(CSoundSourceDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_CBN_SELCHANGE(IDC_COMBO_CATEGORY, OnCbnSelchangeComboCategory)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SOUNDSOURCE, OnNMDblclkListSoundsource)
END_MESSAGE_MAP()


// CSoundSourceDlg 메시지 처리기입니다.

BOOL CSoundSourceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctrlSoundSource.SetExtendedStyle (
		m_ctrlSoundSource.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT	rect;
	m_ctrlSoundSource.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	int			ColumnCount = 4;	
	char		*ColumnName[4] = { "종류", "버퍼수", "파일", "코멘트" };
	int			ColumnWidthPercent[4] = { 10, 10, 15, 60 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM
			| LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlSoundSource.InsertColumn (i, &lvcolumn);		
	}

	for ( int i = 0; i < NTypeDesc::DescCount; i++ )
	{
		int nIndex = m_ctrlCategory.AddString ( NTypeDesc::Desc[i] );
		m_ctrlCategory.SetItemData ( nIndex, i );
	}
    m_ctrlCategory.SetCurSel ( m_ctrlCategory.GetCount () - 1 );

	ReloadAllItems ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSoundSourceDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION	pos = m_ctrlSoundSource.GetFirstSelectedItemPosition ();	
	int nItem = m_ctrlSoundSource.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return;
	}

	SSoundSource* pSoundSource = (SSoundSource*) m_ctrlSoundSource.GetItemData ( nItem );
	m_SoundSource = *pSoundSource;

	OnOK();
}

void	CSoundSourceDlg::GetSelectSoundSource ( SSoundSource &SoundSource )
{
	SoundSource = m_SoundSource;
}

void CSoundSourceDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

void	CSoundSourceDlg::SetState ( CSoundSourceMan *pSoundSourceMan )
{
	m_pSoundSourceMan = pSoundSourceMan;
}

void	CSoundSourceDlg::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	UpdateData ( TRUE );
	ReloadAllItems();
}

void	CSoundSourceDlg::ReloadAllItems ()
{
	m_ctrlSoundSource.DeleteAllItems ();	

	int nIndex = m_ctrlCategory.GetCurSel ();
	int	nCategory = (int)m_ctrlCategory.GetItemData ( nIndex );

	int	Amount = 0;
	CString	str;
	m_pSoundSourceMan->GoTail();
	while ( !m_pSoundSourceMan->IsEnd () )
	{
		SSoundSource* pReloadSource = m_pSoundSourceMan->GetRecord ();

		if ( pReloadSource->TYPE == nCategory || nCategory == 7 )
		{
			if ( CompareAllToken ( pReloadSource->Comment, m_valKeyword ) )
			{
				char		*ColumnName[4] = { "종류", "버퍼수", "파일", "코멘트" };
				int nItem = m_ctrlSoundSource.InsertItem ( Amount, NTypeDesc::Desc[pReloadSource->TYPE] );
				str.Format ( "%d", pReloadSource->BufferCount );
				m_ctrlSoundSource.SetItemText ( Amount, 1, str );
				m_ctrlSoundSource.SetItemText ( Amount, 2, pReloadSource->FileName );				
				m_ctrlSoundSource.SetItemText ( Amount, 3, pReloadSource->Comment );

				m_ctrlSoundSource.SetItemData ( nItem, (DWORD_PTR)pReloadSource );

				Amount++;
			}
		}

		m_pSoundSourceMan->GoPrev ();
	}
}

BOOL	CSoundSourceDlg::CompareAllToken ( CString Left, CString Right )
{
	char szRight[1024];
	int	LengthRight = 0;
	CString Token[32];
	int TokenCount = 0;
	
	strcpy ( szRight, Right.GetString () );
	LengthRight = Right.GetLength ();

	for ( int i = 0; i < LengthRight; i++ )
	{
		//	<--	첫번째부터 공백일때	-->	//
		if ( i == 0 && szRight[i] == ' ' )
		{			
		}
		//	<--	두번째부터 공백일때 -->	//
		else if ( szRight[i] == ' ' && szRight[i - 1] != ' ' )
		{			
			TokenCount++;			
		}
		else
		{
			if ( !Token[TokenCount].GetLength () )
			{
				Token[TokenCount] = szRight[i];
			}
			else
			{
				Token[TokenCount] += szRight[i];
			}
		}
	}

	BOOL	bResult = TRUE;
    for ( int j = 0; j <= TokenCount; j++ )
	{
		 if ( !strstr ( Left.GetString (), Token[j].GetString () ) )
		 {
			 bResult = FALSE;
		 }
	}

	return bResult;
}

void CSoundSourceDlg::OnCbnSelchangeComboCategory()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_valKeyword = "";
	UpdateData ( FALSE );
	ReloadAllItems();
}

void CSoundSourceDlg::OnNMDblclkListSoundsource(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if ( m_SoundSource.FileName.GetLength() )
	{			
		if ( m_pSound )
		{
			m_pSound->StopBuffer ( m_BufferID );
			m_pSound = NULL;
			DxSoundMan::GetInstance().ReleaseWav ( m_SoundSource.FileName.GetString(), false );
		}
	}

	POSITION	pos = m_ctrlSoundSource.GetFirstSelectedItemPosition ();	
	int nItem = m_ctrlSoundSource.GetNextSelectedItem ( pos );	

	SSoundSource* pSoundSource = (SSoundSource*) m_ctrlSoundSource.GetItemData ( nItem );
	m_SoundSource = *pSoundSource;
	
	m_pSound = DxSoundMan::GetInstance().LoadWav (
		m_SoundSource.FileName.GetString(), false );

	if ( !m_pSound )
	{
		CString strTemp;
		strTemp.Format ( "파일이 존재하지 않습니다.[%s]", m_SoundSource.FileName );
		MessageBox ( strTemp.GetString(), "오류", MB_OK );
		return ;
	}	

	//	Note : 카메라 방향에 따라 소리변화 반영	
	//
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3	&vLookDir = DxViewPort::GetInstance().GetLookDir();

	m_pSound->PlayBuffer ( m_BufferID );

	//	<--	포지션 재 설정	-->	//
	static	LPDIRECTSOUND3DBUFFER	pDS3DBuffer;
	m_pSound->Get3DBufferInterface ( m_BufferID, &pDS3DBuffer );
	
	if ( pDS3DBuffer )
	{
		pDS3DBuffer->SetMinDistance ( 10.0f, DS3D_DEFERRED );
		pDS3DBuffer->SetMaxDistance ( 10.0f, DS3D_DEFERRED );
		pDS3DBuffer->SetPosition (
			vLookatPt.x, vLookatPt.y,
			vLookatPt.z, DS3D_DEFERRED );				

		//	<--	볼륨 조절	-->	//
		static	LPDIRECTSOUNDBUFFER		pDSBuffer;
		pDSBuffer = m_pSound->GetBuffer ( m_BufferID );
		if ( pDSBuffer )
		{
			pDSBuffer->SetVolume ( 0 );
		}
	}

	//	<--	원래는 Listener의 위치를 바꿔주는 메쏘드가 있었으나,
	//		여기서만 쓰이는 결과를 보게되었다. 그리고, 그 메쏘드들이
	//		FrameMove에서 처리되어도 되는 내용을 가지고 있음을 발견하고,
	//		그것을 수정하다보니, 여기서 FrameMove를 호출하게 되었다.
	//		결국, Listener의 위치를 바꾸기 위함이다.
	//	-->
	DxSoundMan::GetInstance().FrameMove( vLookDir, vLookatPt );
}