// SoundSourceDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "SoundSourceDlg.h"
#include "../EngineLib/DxSound/DxSoundMan.h"
#include "../EngineLib/DxTools/DxViewPort.h"


// CSoundSourceDlg ��ȭ �����Դϴ�.

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


// CSoundSourceDlg �޽��� ó�����Դϴ�.

BOOL CSoundSourceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_ctrlSoundSource.SetExtendedStyle (
		m_ctrlSoundSource.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT	rect;
	m_ctrlSoundSource.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	int			ColumnCount = 4;	
	char		*ColumnName[4] = { "����", "���ۼ�", "����", "�ڸ�Ʈ" };
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSoundSourceDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnCancel();
}

void	CSoundSourceDlg::SetState ( CSoundSourceMan *pSoundSourceMan )
{
	m_pSoundSourceMan = pSoundSourceMan;
}

void	CSoundSourceDlg::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
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
				char		*ColumnName[4] = { "����", "���ۼ�", "����", "�ڸ�Ʈ" };
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
		//	<--	ù��°���� �����϶�	-->	//
		if ( i == 0 && szRight[i] == ' ' )
		{			
		}
		//	<--	�ι�°���� �����϶� -->	//
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_valKeyword = "";
	UpdateData ( FALSE );
	ReloadAllItems();
}

void CSoundSourceDlg::OnNMDblclkListSoundsource(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		strTemp.Format ( "������ �������� �ʽ��ϴ�.[%s]", m_SoundSource.FileName );
		MessageBox ( strTemp.GetString(), "����", MB_OK );
		return ;
	}	

	//	Note : ī�޶� ���⿡ ���� �Ҹ���ȭ �ݿ�	
	//
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3	&vLookDir = DxViewPort::GetInstance().GetLookDir();

	m_pSound->PlayBuffer ( m_BufferID );

	//	<--	������ �� ����	-->	//
	static	LPDIRECTSOUND3DBUFFER	pDS3DBuffer;
	m_pSound->Get3DBufferInterface ( m_BufferID, &pDS3DBuffer );
	
	if ( pDS3DBuffer )
	{
		pDS3DBuffer->SetMinDistance ( 10.0f, DS3D_DEFERRED );
		pDS3DBuffer->SetMaxDistance ( 10.0f, DS3D_DEFERRED );
		pDS3DBuffer->SetPosition (
			vLookatPt.x, vLookatPt.y,
			vLookatPt.z, DS3D_DEFERRED );				

		//	<--	���� ����	-->	//
		static	LPDIRECTSOUNDBUFFER		pDSBuffer;
		pDSBuffer = m_pSound->GetBuffer ( m_BufferID );
		if ( pDSBuffer )
		{
			pDSBuffer->SetVolume ( 0 );
		}
	}

	//	<--	������ Listener�� ��ġ�� �ٲ��ִ� �޽�尡 �־�����,
	//		���⼭�� ���̴� ����� ���ԵǾ���. �׸���, �� �޽�����
	//		FrameMove���� ó���Ǿ �Ǵ� ������ ������ ������ �߰��ϰ�,
	//		�װ��� �����ϴٺ���, ���⼭ FrameMove�� ȣ���ϰ� �Ǿ���.
	//		�ᱹ, Listener�� ��ġ�� �ٲٱ� �����̴�.
	//	-->
	DxSoundMan::GetInstance().FrameMove( vLookDir, vLookatPt );
}