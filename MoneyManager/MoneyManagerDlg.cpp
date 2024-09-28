// MoneyManagerDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include "../RanLogic/Util/s_CCfg.h"

#include "./COdbcManager.h"
#include "./MoneyManager.h"


#include "./PassWordDlg.h"
#include "./DifferListDlg.h"


#include "./MoneyManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMoneyManagerDlg 대화 상자



CMoneyManagerDlg::CMoneyManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMoneyManagerDlg::IDD, pParent)
    , m_pCfg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMoneyManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FIELDINFO, m_list_FieldInfo);
	DDX_Control(pDX, IDC_PROGRESS_COMPLETE, m_prog_Complete);
}

BEGIN_MESSAGE_MAP(CMoneyManagerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_CHECK_MULTIPLY, OnBnClickedCheckMultiply)
	ON_BN_CLICKED(IDC_BUTTON_MONEYCHECK, OnBnClickedButtonMoneycheck)
	ON_BN_CLICKED(IDC_BUTTON_REENCRYPT, OnBnClickedButtonReencrypt)
	ON_BN_CLICKED(IDC_BUTTON_NEWENCRYPT, OnBnClickedButtonNewencrypt)
END_MESSAGE_MAP()


// CMoneyManagerDlg 메시지 처리기

BOOL CMoneyManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.	
	RECT listRect;
	m_list_FieldInfo.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nColWidth[5] = { int(nListWidth*0.07f), int(nListWidth*0.07f), int(nListWidth*0.3f), int(nListWidth*0.25f), int(nListWidth*0.15f) };

	m_list_FieldInfo.SetExtendedStyle ( m_list_FieldInfo.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_list_FieldInfo.InsertColumn ( 0, _T("CH"),	LVCFMT_CENTER, nColWidth[0] );
	m_list_FieldInfo.InsertColumn ( 1, _T("ID"),	LVCFMT_CENTER, nColWidth[1] );
	m_list_FieldInfo.InsertColumn ( 2, _T("ServerName"),LVCFMT_CENTER, nColWidth[2] );
	m_list_FieldInfo.InsertColumn ( 3, _T("IP"),		LVCFMT_CENTER, nColWidth[3] );
	m_list_FieldInfo.InsertColumn ( 4, _T("port"),		LVCFMT_CENTER, nColWidth[4] );

	SetWin_Num_int( this, IDC_EDIT_MULTIMONEY, 1 );
	SetWin_Num_int( this, IDC_EDIT_ADDMONEY, 0 );
	SetWin_Enable( this, IDC_EDIT_MULTIMONEY, FALSE );

	if( !LoadCfg() )   	  {	DestroyWindow(); return FALSE; }
	if( !OpenDatabase() ) {	DestroyWindow(); return FALSE; }

	SetWin_Num_int( this, IDC_EDIT_ALLUSER, COdbcManager::GetInstance()->GetChaSeq() );

	CPassWordDlg pwDlg;
	if( pwDlg.DoModal() != IDOK ) 
	{
		DestroyWindow();
		return FALSE;
	}

	m_setSTOUserNum = pwDlg.GetStorageUserNum();

	m_prog_Complete.SetRange( 0, 100 );
	m_prog_Complete.SetPos( 0 );
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CMoneyManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CMoneyManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CMoneyManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMoneyManagerDlg::LoadCfg()
{
	int	nRetCode;

    SAFE_DELETE(m_pCfg);
    m_pCfg = new CCfg;

	nRetCode = m_pCfg->Load(_T("server.cfg"));
	if (nRetCode != 0)
	{
		MessageBox(_T("ERROR:CFG file load error"));
		return FALSE;
	}	

	SetWin_Text( this, IDC_EDIT_AGNAME, m_pCfg->GetServerName() );
	SetWin_Text( this, IDC_EDIT_AGIP, m_pCfg->GetServerIP() );
	SetWin_Num_int( this, IDC_EDIT_AGPORT, m_pCfg->GetServicePort() );

	SetWin_Text( this, IDC_EDIT_DBNAME, m_pCfg->GetGameOdbcName() );
	SetWin_Text( this, IDC_EDIT_DBUSER, m_pCfg->GetGameOdbcUser() );
	SetWin_Text( this, IDC_EDIT_DBPW, m_pCfg->GetGameOdbcPass() );



	int num;
	char szValue[128];
	F_SERVER_INFO* pFieldInfo = m_pCfg->GetFieldServerArray();
	for ( int c=0; c<MAX_CHANNEL_NUMBER; ++c )
	{
		for ( int i=0; i<FIELDSERVER_MAX; ++i )
		{
			F_SERVER_INFO fieldInfo = pFieldInfo[c*MAX_CHANNEL_NUMBER+i];
			if ( pFieldInfo[c*MAX_CHANNEL_NUMBER+i].IsValid() )
			{
				num = c*MAX_CHANNEL_NUMBER+i;
				_gcvt( c, 2, szValue );
				m_list_FieldInfo.InsertItem( num, szValue );
				_gcvt( i, 2, szValue );
				m_list_FieldInfo.SetItemText( num, 1, szValue );
				m_list_FieldInfo.SetItemText( num, 2, fieldInfo.szServerName );
				m_list_FieldInfo.SetItemText( num, 3, fieldInfo.szPublicIp );
				_gcvt( fieldInfo.nServicePort, 5, szValue );
				m_list_FieldInfo.SetItemText( num, 4, szValue );
			}
		}
	}

	return TRUE;

}

BOOL CMoneyManagerDlg::OpenDatabase()
{
	int nRetCode = COdbcManager::GetInstance()->OpenUserDB( m_pCfg->GetGameOdbcName(),
															m_pCfg->GetGameOdbcUser(),
															m_pCfg->GetGameOdbcPass()  );

	if (nRetCode == DB_ERROR)
	{	
		MessageBox(_T("DB Open Failed"), _T("ERROR"), MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CMoneyManagerDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    SAFE_DELETE(m_pCfg);
	return CDialog::DestroyWindow();
}

void CMoneyManagerDlg::OnBnClickedCheckMultiply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (GetWin_Check( this, IDC_CHECK_MULTIPLY ))
	{
		SetWin_Enable( this, IDC_EDIT_MULTIMONEY, TRUE );
		SetWin_Enable( this, IDC_EDIT_ADDMONEY, FALSE );

		SetWin_Text( this, IDC_CHECK_MULTIPLY, "Multiply" );
	}
    else
    {
		SetWin_Enable( this, IDC_EDIT_MULTIMONEY, FALSE );
		SetWin_Enable( this, IDC_EDIT_ADDMONEY, TRUE );

		SetWin_Text( this, IDC_CHECK_MULTIPLY, "Add" );
	}
}


void CMoneyManagerDlg::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	float fAddMoney = 0.0f;
	BOOL bMultiply   = GetWin_Check( this, IDC_CHECK_MULTIPLY ); 

	char szTempChar[256];
	if( bMultiply )
	{
		fAddMoney = GetWin_Num_float( this, IDC_EDIT_MULTIMONEY );
		sprintf( szTempChar, "Multiply Money! Money to multiply %.2f", fAddMoney );
	}else{
		fAddMoney = GetWin_Num_float( this, IDC_EDIT_ADDMONEY );	
		sprintf( szTempChar, "Add Money! Money to add %.0f", fAddMoney );
	}
	if( MessageBox( szTempChar, "Infomation", MB_YESNO ) == IDNO ) return;


	int nAllChar = GetWin_Num_int( this, IDC_EDIT_ALLUSER );	
	int nAllUser = (int)m_setSTOUserNum.size();	
	int nMaxUser = nAllChar + nAllUser;
	int iCount = 0;

	std::vector<SMoneyInfo> vecEditChaInfo;
	SMoneyInfo editInfo;

	for( int i = 0; i <= nAllChar; i++ )
	{
		editInfo = COdbcManager::GetInstance()->UpdateGameMoney( i, fAddMoney, bMultiply, GAMEMONEY_TOOLVER );
		if( editInfo.nNumber != DB_ERROR ) vecEditChaInfo.push_back(editInfo);

		int Pos = ((iCount* 100)/(nMaxUser)) ;
		m_prog_Complete.SetPos ( Pos );

		iCount++;
	}

	std::vector<SMoneyInfo> vecEditUserInfo;

    std::set<int>::iterator iter = m_setSTOUserNum.begin();
	for( ;iter != m_setSTOUserNum.end(); ++iter )
	{
		editInfo = COdbcManager::GetInstance()->UpdateSTOMoney( *iter, fAddMoney, bMultiply, GAMEMONEY_TOOLVER );
		if( editInfo.nNumber != DB_ERROR ) vecEditUserInfo.push_back(editInfo);
		
		int Pos = ((iCount* 100)/(nMaxUser)) ;
		m_prog_Complete.SetPos ( Pos );

		iCount++;
	}

	CDifferListDlg differDlg;
	differDlg.UpdateDifferList(vecEditChaInfo,vecEditUserInfo,VIEW_UPDATE);
	differDlg.DoModal();

	m_prog_Complete.SetPos ( 0 );
}

void CMoneyManagerDlg::OnBnClickedButtonMoneycheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nAllChar = GetWin_Num_int( this, IDC_EDIT_ALLUSER );	
	int nAllUser = (int)m_setSTOUserNum.size();	
	int nMaxUser = nAllChar + nAllUser;
	int iCount = 0;

	SMoneyInfo editInfo;
	std::vector<SMoneyInfo> vecEditChaInfo;
	for( int i = 0; i <= nAllChar; i++ )
	{
		BOOL bCheck = COdbcManager::GetInstance()->CheckGameMoney( i, editInfo, GAMEMONEY_TOOLVER  );
		if( !bCheck )
		{
			vecEditChaInfo.push_back(editInfo);
		}

		int Pos = ((iCount* 100)/(nMaxUser)) ;
		m_prog_Complete.SetPos ( Pos );

		iCount++;
	}

	std::vector<SMoneyInfo> vecEditUserInfo;

    std::set<int>::iterator iter = m_setSTOUserNum.begin();
	for( ;iter != m_setSTOUserNum.end(); ++iter, iCount++ )
	{
		BOOL bCheck = COdbcManager::GetInstance()->CheckSTOMoney( *iter, editInfo, GAMEMONEY_TOOLVER );
		if( !bCheck )
		{
			vecEditUserInfo.push_back(editInfo);
		}

		int Pos = ((iCount* 100)/(nMaxUser)) ;
		m_prog_Complete.SetPos ( Pos );	
	}

	CDifferListDlg differDlg;
	differDlg.UpdateDifferList(vecEditChaInfo,vecEditUserInfo,VIEW_CHECK);
	differDlg.DoModal();

	m_prog_Complete.SetPos ( 0 );

}


void CMoneyManagerDlg::OnBnClickedButtonReencrypt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nAllChar = GetWin_Num_int( this, IDC_EDIT_ALLUSER );	
	int nAllUser = (int)m_setSTOUserNum.size();	
	int nMaxUser = nAllChar + nAllUser;
	int iCount = 0;

	LONGLONG lnEncryptMoney = 0;

	SMoneyInfo editInfo;
	std::vector<SMoneyInfo> vecEditChaInfo;
	for( int i = 0; i <= nAllChar; i++, iCount++ )
	{
		BOOL bCheck = COdbcManager::GetInstance()->CheckGameMoney( i, editInfo, GAMEMONEY_TOOLVER  );
		if( !bCheck )
		{
			lnEncryptMoney = min( editInfo.lnMoney1, editInfo.lnMoney2 );
			COdbcManager::GetInstance()->EncryptGameMoney( editInfo.nNumber, lnEncryptMoney, GAMEMONEY_TOOLVER );
			editInfo.lnMoney1 = max( editInfo.lnMoney1, editInfo.lnMoney2 );
			editInfo.lnMoney2 = lnEncryptMoney;
			vecEditChaInfo.push_back(editInfo);
		}

		int Pos = ((iCount* 100)/(nMaxUser)) ;
		m_prog_Complete.SetPos ( Pos );
	}

	std::vector<SMoneyInfo> vecEditUserInfo;
	
    std::set<int>::iterator iter = m_setSTOUserNum.begin();
	for( ;iter != m_setSTOUserNum.end(); ++iter, iCount++ )
	{
		BOOL bCheck = COdbcManager::GetInstance()->CheckSTOMoney( *iter, editInfo, GAMEMONEY_TOOLVER );
		if( !bCheck )
		{
			lnEncryptMoney = min( editInfo.lnMoney1, editInfo.lnMoney2 );
			COdbcManager::GetInstance()->EncryptSTOMoney( editInfo.nNumber, lnEncryptMoney, GAMEMONEY_TOOLVER );
			editInfo.lnMoney1 = max( editInfo.lnMoney1, editInfo.lnMoney2 );
			editInfo.lnMoney2 = lnEncryptMoney;
			vecEditUserInfo.push_back(editInfo);
		}

		int Pos = ((iCount* 100)/(nMaxUser)) ;
		m_prog_Complete.SetPos ( Pos );
	}

	CDifferListDlg differDlg;
	differDlg.UpdateDifferList(vecEditChaInfo,vecEditUserInfo,VIEW_CORRECT);
	differDlg.DoModal();

	m_prog_Complete.SetPos ( 0 );
}

void CMoneyManagerDlg::OnBnClickedButtonNewencrypt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nAllChar = GetWin_Num_int( this, IDC_EDIT_ALLUSER );	
	int nAllUser = (int)m_setSTOUserNum.size();	
	int nMaxUser = nAllChar + nAllUser;
	int iCount = 0;

	SMoneyInfo editInfo;
	std::vector<SMoneyInfo> vecEditChaInfo;
	for( int i = 0; i <= nAllChar; i++, iCount++ )
	{
		editInfo = COdbcManager::GetInstance()->NewEncryptGameMoney_Ver100to101( i );		
		if( editInfo.nNumber != DB_ERROR ) vecEditChaInfo.push_back(editInfo);

		int Pos = ((iCount* 100)/(nMaxUser)) ;
		m_prog_Complete.SetPos ( Pos );
	}

	std::vector<SMoneyInfo> vecEditUserInfo;

    std::set<int>::iterator iter = m_setSTOUserNum.begin();
	for( ;iter != m_setSTOUserNum.end(); ++iter, iCount++ )
	{

		editInfo = COdbcManager::GetInstance()->NewEncryptSTOMoney_Ver100to101( *iter );		
		if( editInfo.nNumber != DB_ERROR ) vecEditUserInfo.push_back(editInfo);

		int Pos = ((iCount* 100)/(nMaxUser)) ;
		m_prog_Complete.SetPos ( Pos );
	}

	CDifferListDlg differDlg;
	differDlg.UpdateDifferList(vecEditChaInfo,vecEditUserInfo,VIEW_NEWENCRYPT);
	differDlg.DoModal();

	m_prog_Complete.SetPos ( 0 );
}



