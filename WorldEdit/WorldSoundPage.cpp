// WorldSoundPage.cpp : 구현 파일입니다.
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

// CWorldSoundPage 대화 상자입니다.

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

// CWorldSoundPage 메시지 처리기입니다.

void CWorldSoundPage::OnBnClickedButtonGetsound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CWorldSoundPage::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CButton* pButton;
	pButton = (CButton*) GetDlgItem( IDC_CHECK_DSP_NAME );
	pButton->SetCheck( FALSE );

	OnBnClickedCheckDspName();

	return CPropertyPage::OnKillActive();
}

void CWorldSoundPage::OnBnClickedCheckDspName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton;
	pButton = (CButton*) GetDlgItem( IDC_CHECK_DSP_NAME );
	m_pStaticSoundMan->SetDSPName( (BOOL)pButton->GetCheck() );
}

void CWorldSoundPage::OnBnClickedButtonCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData ( TRUE );

	//	<**	맵사운드 이름이 지정되지 않거나,
	//		파일이 지정되지 않은 경우는
	//		무효!!!!
	//	**>
	if ( !m_MapSoundName.GetLength() ||
		!m_valSound.GetLength() ||
		m_MapSoundName.GetLength() > 256 )
	{
		MessageBox ( "소리의 이름이 없거나, 256자를 넘었습니다.\n혹시, 아직 음원을"
			"입력하시지 않으신건 아닌지요?" );
		return;
	}

	if( m_pVecSound == NULL )
	{
		MessageBox ( "사운드 생성 에러 수정중엔 복사가 안됩니다.", "알림", MB_OK );
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

	//	<--	최대 영역의 1/16를 적용해서 최소범위를 지정한다.	-->	//
	//	<--	롤오프도 지정한다.
	//	<--	완전수동세팅으로 전환 2002-11-28T13:40
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

//	_ASSERT ( m_SoundSource.BufferCount && "버퍼가 0입니다. 확인 부탁드립니다." );

	strcpy ( m_pVecSound->m_szSoundName, m_MapSoundName.GetString() );

	m_pVecSound->Load (	m_SoundSource.FileName.GetString(), (m_bLoop)?true:false, false );

	SVecSound* pGetVecSound = m_pStaticSoundMan->GetVecSound ( m_MapSoundName.GetString() );
    if ( !pGetVecSound )	//	새로 등록되는 경우
	{
		m_pStaticSoundMan->AddVecSound ( m_pVecSound );		
	}
	else if ( pGetVecSound == m_pVecSound )	//	수정 되는 경우
	{
		//	아무 의미 없는 놈이지만, 최적화되다가
		//	이 블럭이 날아갈까봐 써 둔다.
		int JustTemp = 0;
	}
	else
	{
		MessageBox ( "머라카노~ 이름 똑같다 아이가... ㅡ,.ㅡ;;" );
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
    if ( pGetVecSound == m_pVecSound )	//	수정하던 놈
	{
		m_pVecSound = NULL;
	}
	else	//	생성할 놈
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
	
	//	<--	중복방지용
	//	이것은 적절하지 못한 아이디어였다.
	//	사용자가 데이타를 잘못 입력할 경우, 잘못 입력했음을 경고해주고,
	//	프로그램이 그것을 자동으로 수정하지는 말아야한다.
	//	편리하게 만들려고 한 것이, 오히려 사용자를 더 혼란스럽게 만들수
	//	있다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bLoop = !m_bLoop;
	m_ctrlLoopTime.EnableWindow ( !m_bLoop );
}

void CWorldSoundPage::OnBnClickedButtonModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
				//	2003-02-03 T11:46 버그 수정
				//	모든 데이타를 가져야만, '등록'을 누를때 데이타가 정상적으로 등록된다.
				m_SoundSource = *pSoundSource;
				m_valSound = pSoundSource->Comment;
				bFound = TRUE;
				break;
			}			
			m_SoundSourceMan.GoNext();
		}

		if ( !bFound )
		{
			MessageBox ( "WorldSoundPage.cpp, 483번째줄입니다. 일어날수 없는 상황" );
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlMapSound.GetCurSel ();
	SVecSound* pVecSound = (SVecSound*)m_ctrlMapSound.GetItemData ( nIndex );

	m_pStaticSoundMan->DelVecSound ( pVecSound->m_szSoundName );

    WorldStateMachine::EditAsset(pVecSound->m_szSoundName);

    ListingUpdate();	
	ActiveSoundEdit( FALSE, FALSE );
}

void CWorldSoundPage::OnBnClickedCheckRenderRange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		if ( !pGetVecSound )	//	새로 등록되는 경우
		{
			m_pStaticSoundMan->AddVecSound ( m_pVecSound );		
		}
		m_pVecSound = NULL;
		ListingUpdate ();		

		pView->DeActiveEditMatrix();

		OnLbnSelchangeListMapsound();

		
	}else{
		// Note : 커서 없애기
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix();
	}

}

void CWorldSoundPage::OnBnClickedCheckEnablesound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

		//	<--	완전 수동세팅으로 전환	2002-11-28T13:41
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		//_ASSERT(pLoadVecSound->m_BufferCount&&"버퍼가 0 이하이다!");		// 수정 바래요.

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

	//	<--	완전 수동 세팅으로 전환 2002-11-28T13:43
//	pButton = (CButton*) GetDlgItem(IDC_EDIT_MINRANGE);
//	pButton->EnableWindow ( FALSE );
	//	-->
	ActiveSoundEdit( FALSE, FALSE );

	UpdateData ( FALSE );
}



