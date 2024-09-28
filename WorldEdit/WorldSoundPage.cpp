// WorldSoundPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MainFrm.h"
#include "sheetWithTab.h"

#include "../EngineLib/DxTools/DxMethods.h"
#include "WorldSoundPage.h"
#include "../EngineLib/DxSound/StaticSoundMan.h"
#include "../EngineLib/DxSound/DxSoundMan.h"
#include "SoundSourceDlg.h"
#include "State/WorldStateMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWorldSoundPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CWorldSoundPage, CPropertyPage)
CWorldSoundPage::CWorldSoundPage()
	: CPropertyPage(CWorldSoundPage::IDD),
	m_pSheetTab(NULL)
	, m_valSound(_T(""))
	, m_valBufferCount(1)
	, m_MapSoundName(_T(""))	
	, m_bLoop(FALSE)
	, m_pVecSound(NULL)
	, m_MinRange(_T(""))
	, m_MaxRange(_T(""))	
	, m_Rolloff(_T(""))
	, m_Distance(_T(""))
	, m_Doppler(_T(""))
	, m_VelocityX(_T(""))
	, m_VelocityY(_T(""))
	, m_VelocityZ(_T(""))
	, m_LoopTime(_T(""))
	, m_Volume(_T(""))
	, m_LastUseItem(-1)
{
	m_bDlgInit = FALSE;
	m_pStaticSoundMan = NULL;
}

CWorldSoundPage::~CWorldSoundPage()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	CStaticSoundMan* pStaticSoundMan = pView->GetStaticSoundMan ();
	m_pStaticSoundMan = pStaticSoundMan;

	if ( m_pStaticSoundMan )
	{
		if ( !m_pStaticSoundMan->GetVecSound ( m_MapSoundName.GetString() ) )
		{
			SAFE_DELETE ( m_pVecSound );
		}
		m_pVecSound = NULL;
	}
}

void CWorldSoundPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SOUND, m_valSound);
	DDX_Text(pDX, IDC_EDIT_BUFFERCOUNT, m_valBufferCount);
	DDX_Control(pDX, IDC_LIST_MAPSOUND, m_ctrlMapSound);
	DDX_Text(pDX, IDC_EDIT_MAPSOUNDNAME, m_MapSoundName);
	DDX_Text(pDX, IDC_EDIT_MINRANGE, m_MinRange);
	DDX_Text(pDX, IDC_EDIT_MAXRANGE, m_MaxRange);	
	DDX_Text(pDX, IDC_EDIT_ROLLOFF, m_Rolloff);
	DDX_Text(pDX, IDC_EDIT_DISTANCE, m_Distance);
	DDX_Text(pDX, IDC_EDIT_DOPPLER, m_Doppler);
	DDX_Text(pDX, IDC_EDIT_VELOCITY_X, m_VelocityX);
	DDX_Text(pDX, IDC_EDIT_VELOCITY_Y, m_VelocityY);
	DDX_Text(pDX, IDC_EDIT_VELOCITY_Z, m_VelocityZ);
	DDX_Text(pDX, IDC_EDIT_LOOPTIME, m_LoopTime);
	DDX_Control(pDX, IDC_EDIT_LOOPTIME, m_ctrlLoopTime);
	DDX_Text(pDX, IDC_EDIT_VOLUME, m_Volume);
}


BEGIN_MESSAGE_MAP(CWorldSoundPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GETSOUND, OnBnClickedButtonGetsound)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnBnClickedCheckLoop)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_CHECK_RENDER_RANGE, OnBnClickedCheckRenderRange)
	ON_BN_CLICKED(IDC_CHECK_ENABLESOUND, OnBnClickedCheckEnablesound)
	ON_LBN_SELCHANGE(IDC_LIST_MAPSOUND, OnLbnSelchangeListMapsound)
	ON_BN_CLICKED(IDC_CHECK_DSP_NAME, OnBnClickedCheckDspName)
END_MESSAGE_MAP()

// CWorldSoundPage �޽��� ó�����Դϴ�.

void CWorldSoundPage::OnBnClickedButtonGetsound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );

	CSoundSourceDlg	dlg;
	dlg.SetState ( &m_SoundSourceMan );
	if ( dlg.DoModal() == IDOK )
	{
		dlg.GetSelectSoundSource ( m_SoundSource );
		
		m_valSound = m_SoundSource.Comment;
		m_valBufferCount = m_SoundSource.BufferCount; 

		UpdateData ( FALSE );
	}	
}

void CWorldSoundPage::ActiveSoundEdit ( BOOL bShow, BOOL bNew )
{
	int nSoundContrls[] =
	{
		IDC_STATIC_E,
		IDC_EDIT_MAPSOUNDNAME,
		IDC_BUTTON_GETSOUND,
		IDC_EDIT_SOUND,
		IDC_BUTTON1,
		IDC_BUTTON2,
		IDC_BUTTON3,
		IDC_BUTTON4,
		IDC_BUTTON5,
		IDC_EDIT_VOLUME,
		IDC_EDIT_BUFFERCOUNT,
		IDC_EDIT_MINRANGE,
		IDC_EDIT_MAXRANGE,
		IDC_CHECK_LOOP,
		IDC_BUTTON_CANCEL,
		IDC_BUTTON_ADD,
		IDC_EDIT_LOOPTIME
	};
	int NumContrl = sizeof(nSoundContrls) / sizeof(int);

	CWnd *pWnd;

	for ( int i=0; i<NumContrl; i++ )
	{
		pWnd = GetDlgItem(nSoundContrls[i]);
		pWnd->ShowWindow ( bShow );
	}

	if ( bNew )
	{
		pWnd = GetDlgItem ( IDC_EDIT_MAPSOUNDNAME );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pWnd = GetDlgItem ( IDC_EDIT_MAPSOUNDNAME );
		pWnd->EnableWindow ( FALSE );
	}
}

BOOL CWorldSoundPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	CStaticSoundMan* pStaticSoundMan = pView->GetStaticSoundMan ();

	m_pStaticSoundMan = pStaticSoundMan;

	CString	WavTableFile;	
	WavTableFile = DxSoundMan::GetInstance().GetPath();
    WavTableFile += "WavTable.sg";
	m_SoundSourceMan.LoadFile ( WavTableFile.GetString() );

	m_bDlgInit = TRUE;

	UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CWorldSoundPage::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CButton* pButton;
	pButton = (CButton*) GetDlgItem( IDC_CHECK_DSP_NAME );
	pButton->SetCheck( FALSE );

	OnBnClickedCheckDspName();

	return CPropertyPage::OnKillActive();
}

void CWorldSoundPage::OnBnClickedCheckDspName()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton;
	pButton = (CButton*) GetDlgItem( IDC_CHECK_DSP_NAME );
	m_pStaticSoundMan->SetDSPName( (BOOL)pButton->GetCheck() );
}

void CWorldSoundPage::OnBnClickedButtonCreate()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	if ( m_pVecSound )
	{
		return;
	}

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_LIST_MAPSOUND );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( FALSE );


	m_pVecSound = new SVecSound;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();
	m_pVecSound->m_matWorld._41 = vCollPos.x;
	m_pVecSound->m_matWorld._42 = vCollPos.y;
	m_pVecSound->m_matWorld._43 = vCollPos.z;

	pView->ActiveEditMatrix2( &m_pVecSound->m_matWorld );

	DefaultValueSet ();
	ActiveSoundEdit( TRUE, TRUE );
}

void CWorldSoundPage::OnBnClickedButtonAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );

	//	<**	�ʻ��� �̸��� �������� �ʰų�,
	//		������ �������� ���� ����
	//		��ȿ!!!!
	//	**>
	if ( !m_MapSoundName.GetLength() ||
		!m_valSound.GetLength() ||
		m_MapSoundName.GetLength() > 256 )
	{
		MessageBox ( "�Ҹ��� �̸��� ���ų�, 256�ڸ� �Ѿ����ϴ�.\nȤ��, ���� ������"
			"�Է��Ͻ��� �����Ű� �ƴ�����?" );
		return;
	}

	if( m_pVecSound == NULL )
	{
		MessageBox ( "���� ���� ���� �����߿� ���簡 �ȵ˴ϴ�.", "�˸�", MB_OK );
		return;
	}

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_LIST_MAPSOUND );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( TRUE );
	
	float	fValue;
	fValue = (float)atof ( m_LoopTime.GetString () );
    m_pVecSound->m_LoopTime = fValue;
	m_pVecSound->m_CurrentTime = fValue;	

	//	<--	�ִ� ������ 1/16�� �����ؼ� �ּҹ����� �����Ѵ�.	-->	//
	//	<--	�ѿ����� �����Ѵ�.
	//	<--	���������������� ��ȯ 2002-11-28T13:40
//	float fValue2 = (float) atof ( m_Rolloff.GetString() );	
//	fValue = ( fValue / (16 / fValue2) );
	//	-->
	fValue = (float)atof ( m_MinRange.GetString () );
	m_pVecSound->m_MinRange = fValue;

	fValue = (float)atof ( m_MaxRange.GetString () );
	m_pVecSound->m_MaxRange = fValue;
	
	if ( m_pVecSound->m_MinRange <= m_pVecSound->m_MaxRange )
	{
		const	float	INTERVALSIZE = 40.0f;
		m_pVecSound->m_FadeOutRange = m_pVecSound->m_MaxRange + INTERVALSIZE;
	}
	else
	{
		m_pVecSound->m_FadeOutRange = FLT_MAX;
	}

//	m_pVecSound->m_lVolume = atol ( m_Volume.GetString () );

//	_ASSERT ( m_SoundSource.BufferCount && "���۰� 0�Դϴ�. Ȯ�� ��Ź�帳�ϴ�." );

	strcpy ( m_pVecSound->m_szSoundName, m_MapSoundName.GetString() );

	m_pVecSound->Load (	m_SoundSource.FileName.GetString(), (m_bLoop)?true:false, false );

	SVecSound* pGetVecSound = m_pStaticSoundMan->GetVecSound ( m_MapSoundName.GetString() );
    if ( !pGetVecSound )	//	���� ��ϵǴ� ���
	{
		m_pStaticSoundMan->AddVecSound ( m_pVecSound );		
	}
	else if ( pGetVecSound == m_pVecSound )	//	���� �Ǵ� ���
	{
		//	�ƹ� �ǹ� ���� ��������, ����ȭ�Ǵٰ�
		//	�� ���� ���ư���� �� �д�.
		int JustTemp = 0;
	}
	else
	{
		MessageBox ( "�Ӷ�ī��~ �̸� �Ȱ��� ���̰�... ��,.��;;" );
		return ;
	}

    WorldStateMachine::EditAsset(m_pVecSound->m_szSoundName);

	m_pVecSound = NULL;
	ListingUpdate ();

	ActiveSoundEdit( FALSE, FALSE );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix();
    
	OnLbnSelchangeListMapsound();

}

void CWorldSoundPage::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_LIST_MAPSOUND );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( TRUE );

	SVecSound* pGetVecSound = m_pStaticSoundMan->GetVecSound ( m_MapSoundName.GetString() );
    if ( pGetVecSound == m_pVecSound )	//	�����ϴ� ��
	{
		m_pVecSound = NULL;
	}
	else	//	������ ��
	{
		SAFE_DELETE ( m_pVecSound );		
	}

	m_LastUseItem = m_ctrlMapSound.GetCurSel ();

	ListingUpdate ();

	ActiveSoundEdit( FALSE, FALSE );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix();
}

void CWorldSoundPage::DefaultValueSet ( void )
{	
	CString	str;
	str.Format ( "MapSound[%04d]", m_pStaticSoundMan->GetCount() );
	
	//	<--	�ߺ�������
	//	�̰��� �������� ���� ���̵���.
	//	����ڰ� ����Ÿ�� �߸� �Է��� ���, �߸� �Է������� ������ְ�,
	//	���α׷��� �װ��� �ڵ����� ���������� ���ƾ��Ѵ�.
	//	���ϰ� ������� �� ����, ������ ����ڸ� �� ȥ�������� �����
	//	�ִ�.
	//int	RegisterCount = m_pStaticSoundMan->GetCount();
	//while ( m_pStaticSoundMan->GetVecSound ( str.GetString() ) )
	//{
	//	RegisterCount++;
	//	str.Format ( "MapSound[%04d]", RegisterCount );
	//}
	//	-->
	
	m_MapSoundName = str;
    m_valSound = "";
	m_valBufferCount = 1;

	char	szValue[128];
    _gcvt ( 4.0f, 7, szValue );
	m_MinRange = szValue;
	
	_gcvt ( 128.0f, 7, szValue );
	m_MaxRange = szValue;

	m_Volume.Format ( "%d", 0 );

	m_bLoop = TRUE;	

	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_LOOP);
	pButton->SetCheck( m_bLoop );

	_gcvt ( 0.0f, 7, szValue );
	m_LoopTime = szValue;
	m_ctrlLoopTime.EnableWindow ( !m_bLoop );	

	UpdateData ( FALSE );	
}

void CWorldSoundPage::OnBnClickedCheckLoop()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bLoop = !m_bLoop;
	m_ctrlLoopTime.EnableWindow ( !m_bLoop );
}

void CWorldSoundPage::OnBnClickedButtonModify()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_ctrlMapSound.GetCurSel ();
	if ( nIndex == -1 )
	{
		return;
	}
	m_LastUseItem = nIndex;

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_LIST_MAPSOUND );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( FALSE );

	UpdateData ( TRUE );

	m_valSound = "";	
	m_pVecSound = (SVecSound*)m_ctrlMapSound.GetItemData ( nIndex );	
	
	if ( m_pVecSound )
	{
		m_MapSoundName = m_pVecSound->m_szSoundName;
		m_SoundSource.FileName = m_pVecSound->m_strFileName.c_str();

		BOOL	bFound = FALSE;
        m_SoundSourceMan.GoHead();
        while ( !m_SoundSourceMan.IsEnd() )
		{
			SSoundSource*	pSoundSource;
			pSoundSource = m_SoundSourceMan.GetRecord ();

			if ( m_pVecSound->m_strFileName == pSoundSource->FileName.GetString() )
			{
				//	2003-02-03 T11:46 ���� ����
				//	��� ����Ÿ�� �����߸�, '���'�� ������ ����Ÿ�� ���������� ��ϵȴ�.
				m_SoundSource = *pSoundSource;
				m_valSound = pSoundSource->Comment;
				bFound = TRUE;
				break;
			}			
			m_SoundSourceMan.GoNext();
		}

		if ( !bFound )
		{
			MessageBox ( "WorldSoundPage.cpp, 483��°���Դϴ�. �Ͼ�� ���� ��Ȳ" );
			return;
		}

		m_valBufferCount = 1;		
		char szValue[128];

		_gcvt ( m_pVecSound->m_MinRange, 7, szValue );
		m_MinRange = szValue;
		_gcvt ( m_pVecSound->m_MaxRange, 7, szValue );
		m_MaxRange = szValue;

//		m_Volume.Format ( "%d", m_pVecSound->m_lVolume );
		
		m_bLoop = m_pVecSound->m_bLoop;	

		CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_LOOP);
		pButton->SetCheck( m_bLoop );

		_gcvt ( m_pVecSound->m_LoopTime, 7, szValue );
		m_LoopTime = szValue;
		m_ctrlLoopTime.EnableWindow ( !m_bLoop );		

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->ActiveEditMatrix2( &m_pVecSound->m_matWorld );

		UpdateData ( FALSE );

		ActiveSoundEdit( TRUE, FALSE );
	}
}

void CWorldSoundPage::OnBnClickedButtonDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_ctrlMapSound.GetCurSel ();
	SVecSound* pVecSound = (SVecSound*)m_ctrlMapSound.GetItemData ( nIndex );

	m_pStaticSoundMan->DelVecSound ( pVecSound->m_szSoundName );

    WorldStateMachine::EditAsset(pVecSound->m_szSoundName);

    ListingUpdate();	
	ActiveSoundEdit( FALSE, FALSE );
}

void CWorldSoundPage::OnBnClickedCheckRenderRange()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_RENDER_RANGE);
	m_pStaticSoundMan->EnableRenderRange ( pButton->GetCheck() );
}

void CWorldSoundPage::UpdatePageClickDOWN()
{
	if( !m_bDlgInit )	return;

//	ListingUpdate();	

	int nIndex = m_ctrlMapSound.GetCurSel ();
	SVecSound* pVecSound = (SVecSound*)m_ctrlMapSound.GetItemData ( nIndex );

	if( pVecSound )
	{
		CString	str;

		str.Format ( "MapSound[%04d]", m_pStaticSoundMan->GetCount() );
		int	RegisterCount = 0;
		while ( m_pStaticSoundMan->GetVecSound ( str.GetString() ) )
		{
			str.Format ( "MapSound[%04d]", RegisterCount );
			RegisterCount++;
		}

		m_MapSoundName = str;

		if ( m_pVecSound == NULL )
		{
			m_pVecSound = new SVecSound;
		}

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

		m_pVecSound->m_matWorld = pVecSound->m_matWorld;

//		pView->ActiveEditMatrix2( &m_pVecSound->m_matWorld );


		m_pVecSound->m_LoopTime     = pVecSound->m_LoopTime;
		m_pVecSound->m_CurrentTime  = pVecSound->m_CurrentTime;	
		m_pVecSound->m_MinRange     = pVecSound->m_MinRange;
		m_pVecSound->m_MaxRange     = pVecSound->m_MaxRange;
		m_pVecSound->m_FadeOutRange = pVecSound->m_FadeOutRange;

		strcpy ( m_pVecSound->m_szSoundName, m_MapSoundName.GetString() );

		m_pVecSound->Load (	pVecSound->m_strFileName, (m_bLoop)?true:false, false );

		SVecSound* pGetVecSound = m_pStaticSoundMan->GetVecSound ( m_MapSoundName.GetString() );
		if ( !pGetVecSound )	//	���� ��ϵǴ� ���
		{
			m_pStaticSoundMan->AddVecSound ( m_pVecSound );		
		}
		m_pVecSound = NULL;
		ListingUpdate ();		

		pView->DeActiveEditMatrix();

		OnLbnSelchangeListMapsound();

		
	}else{
		// Note : Ŀ�� ���ֱ�
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix();
	}

}

void CWorldSoundPage::OnBnClickedCheckEnablesound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_ENABLESOUND);
	m_pStaticSoundMan->EnableSound ( pButton->GetCheck() );

	if ( pButton->GetCheck() )
	{
		UpdateData ( TRUE );

		float	value;
		float	RolloffFactor;
		value = (float) atof ( m_Doppler.GetString() );
		DxSoundMan::GetInstance().SetDopplerFactor ( value );

		value = (float) atof ( m_Rolloff.GetString() );
		DxSoundMan::GetInstance().SetRolloffFactor ( value );
		RolloffFactor = value;

		value = (float) atof ( m_Distance.GetString() );
		DxSoundMan::GetInstance().SetDistanceFactor ( value );

        D3DXVECTOR3	Velocity;
		Velocity.x = (float) atof ( m_VelocityX.GetString() );
		Velocity.y = (float) atof ( m_VelocityY.GetString() );
		Velocity.z = (float) atof ( m_VelocityZ.GetString() );
		DxSoundMan::GetInstance().SetVelocityFactor ( Velocity );

		//	<--	���� ������������ ��ȯ	2002-11-28T13:41
		//SVecSound* pVecSound = m_pStaticSoundMan->GetVecSoundHead ();
		//float	fValue;
		//while ( pVecSound )
		//{
		//	fValue = 16 / RolloffFactor;
		//	pVecSound->m_MinRange = pVecSound->m_MaxRange / fValue;

		//	pVecSound = pVecSound->m_pNext;
		//}
		//	-->
	}
}

void CWorldSoundPage::OnLbnSelchangeListMapsound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_ctrlMapSound.GetCurSel ();
	SVecSound* pVecSound = (SVecSound*)m_ctrlMapSound.GetItemData ( nIndex );

	if( pVecSound != NULL )
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->ActiveEditMatrix2( &pVecSound->m_matWorld );
	}

	D3DXVECTOR3 vPos( pVecSound->m_matWorld._41, pVecSound->m_matWorld._42, pVecSound->m_matWorld._43 );
	DxViewPort::GetInstance().CameraJump ( vPos );
}

void CWorldSoundPage::ListingUpdate ()
{
	while ( m_ctrlMapSound.GetCount () )
	{
		m_ctrlMapSound.DeleteString ( 0 );
	}

	SVecSound *pLoadVecSound = m_pStaticSoundMan->GetVecSoundHead();
	for ( int i = 0; i < m_pStaticSoundMan->GetCount(); i++ )
	{		
		int nIndex = m_ctrlMapSound.AddString ( pLoadVecSound->m_szSoundName );
        m_ctrlMapSound.SetItemData ( nIndex, (DWORD_PTR)pLoadVecSound );
		//_ASSERT(pLoadVecSound->m_BufferCount&&"���۰� 0 �����̴�!");		// ���� �ٷ���.

		pLoadVecSound = pLoadVecSound->m_pNext;
	}

	if ( m_LastUseItem == -1 )
	{
		m_LastUseItem = 0;
	}

	m_ctrlMapSound.SetCurSel ( m_LastUseItem );
	m_LastUseItem = -1;
}

void CWorldSoundPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	m_LastUseItem = -1;

	ListingUpdate ();

	char szValue[128];

	_gcvt ( DxSoundMan::GetInstance().GetDopplerFactor(), 7, szValue );
	m_Doppler = szValue;
	_gcvt ( DxSoundMan::GetInstance().GetRolloffFactor(), 7, szValue );
	m_Rolloff = szValue;
	_gcvt ( DxSoundMan::GetInstance().GetDistanceFactor(), 7, szValue );
	m_Distance = szValue;

	D3DXVECTOR3	Velocity;
	Velocity = DxSoundMan::GetInstance().GetVelocityFactor ();
	_gcvt ( Velocity.x, 7, szValue );
	m_VelocityX = szValue;
	_gcvt ( Velocity.y, 7, szValue );
	m_VelocityY = szValue;
	_gcvt ( Velocity.z, 7, szValue );
	m_VelocityZ = szValue;

	CButton *pButton = (CButton*) GetDlgItem(IDC_CHECK_ENABLESOUND);
	pButton->SetCheck ( TRUE );
	m_pStaticSoundMan->EnableSound ( pButton->GetCheck() );

	//	<--	���� ���� �������� ��ȯ 2002-11-28T13:43
//	pButton = (CButton*) GetDlgItem(IDC_EDIT_MINRANGE);
//	pButton->EnableWindow ( FALSE );
	//	-->
	ActiveSoundEdit( FALSE, FALSE );

	UpdateData ( FALSE );
}



