// EffectSoundEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "EffsheetWithTab.h"
#include "EffectSoundEdit.h"

#include "../../EngineLib/DxSound/DxSoundMan.h"
#include "../Dialog/SoundSourceDlg.h"
#include ".\effectsoundedit.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CEffectSoundEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CEffectSoundEdit, CPropertyPage)
CEffectSoundEdit::CEffectSoundEdit()
	: CPropertyPage(CEffectSoundEdit::IDD)
	, m_MaxRange(_T(""))
	, m_MinRange(_T(""))
	, m_valBufferCount(_T(""))
	, m_valSound(_T(""))
	, m_pSoundSource(NULL)
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CEffectSoundEdit::~CEffectSoundEdit()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CEffectSoundEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAXRANGE, m_MaxRange);
	DDX_Text(pDX, IDC_EDIT_MINRANGE, m_MinRange);
	DDX_Text(pDX, IDC_EDIT_BUFFERCOUNT, m_valBufferCount);
	DDX_Text(pDX, IDC_EDIT_SOUND, m_valSound);
}


BEGIN_MESSAGE_MAP(CEffectSoundEdit, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GETSOUND, OnBnClickedButtonGetsound)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
END_MESSAGE_MAP()


// CEffectSoundEdit �޽��� ó�����Դϴ�.

void CEffectSoundEdit::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );	

	if (!m_pSoundSource ||
		!m_pSoundSource->FileName.GetLength () )
	{
		return ;
	}

	float	fValue;
	fValue = (float)atof ( m_MaxRange.GetString () );
	m_pMovSound->m_MaxRange = fValue;
	
	fValue = (float)atof ( m_MinRange.GetString () );
	m_pMovSound->m_MinRange = fValue;

	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_LOOP);	

	m_pMovSound->Load ( m_pSoundSource->FileName.GetString(), (pButton->GetCheck())?true:false, false );

	SAFE_DELETE ( m_pSoundSource );

	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

void CEffectSoundEdit::OnBnClickedButtonGetsound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );

	CSoundSourceDlg	dlg;
	dlg.SetState ( &m_SoundSourceMan );
	if ( dlg.DoModal() == IDOK )
	{
		SAFE_DELETE ( m_pSoundSource );

		m_pSoundSource = new SSoundSource;
		dlg.GetSelectSoundSource ( *m_pSoundSource );
		
		m_valSound = m_pSoundSource->Comment;
		m_valBufferCount.Format ( "%d", m_pSoundSource->BufferCount );

		UpdateData ( FALSE );
	}

}

BOOL CEffectSoundEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.	
	CString	WavTableFile;	
	WavTableFile = DxSoundMan::GetInstance().GetPath();
    WavTableFile += "WavTable.sg";
	m_SoundSourceMan.LoadFile ( WavTableFile.GetString() );	

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffectSoundEdit::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SAFE_DELETE ( m_pSoundSource );
	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

void CEffectSoundEdit::SetProperty ( SMovSound* pMovSound, DWORD dwPageIndex )
{
	SAFE_DELETE ( m_pSoundSource );

	m_pMovSound = pMovSound;
	m_dwOldPageIndex = dwPageIndex;

    m_SoundSourceMan.GoHead();
    while ( !m_SoundSourceMan.IsEnd() )
	{
		SSoundSource*	pSoundSource;
		pSoundSource = m_SoundSourceMan.GetRecord ( );

		if ( pMovSound->m_strFileName == pSoundSource->FileName.GetString() )
		{
			m_valSound = pSoundSource->Comment;
			m_valBufferCount.Format ( "%d", 1 );

			char szValue[256];		
			_gcvt ( pMovSound->m_MaxRange, 7, szValue );
			m_MaxRange = szValue;

			_gcvt ( pMovSound->m_MinRange, 7, szValue );
			m_MinRange = szValue;
						
			CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_LOOP);
			pButton->SetCheck( m_pMovSound->m_bLoop );

			m_pSoundSource = new SSoundSource;
            *m_pSoundSource = *pSoundSource;

			UpdateData ( FALSE );

			return;
		}
		m_SoundSourceMan.GoNext();
	}

	//	<--	����Ʈ ��	-->	//
	m_valSound = "";
	m_valBufferCount.Format ( "%d", 0 );

	char szValue[256];	
	_gcvt ( 250.0f, 7, szValue );
	m_MaxRange = szValue;

	_gcvt ( 100.0f, 7, szValue );
	m_MinRange = szValue;

	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_LOOP);
	pButton->SetCheck( FALSE );	

	UpdateData ( FALSE );
}

void CEffectSoundEdit::OnBnClickedButtonDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if ( MessageBox ( "���� �����Ͻðڽ��ϱ�?", "����", MB_YESNO ) == IDNO ) return ;

	m_pMovSound->m_MaxRange = 0.0f;
	m_pMovSound->m_MinRange = 0.0f;
	m_pMovSound->UnLoad ();

	SAFE_DELETE ( m_pSoundSource );
	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}
