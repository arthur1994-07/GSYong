// CompareToolDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include <set>
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/SerialFile.h"

#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/G-Logic/GLOGIC.h"
#include "../EngineLib/Common/SeqRandom.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Item/GLItem.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../RanLogic/GLStringTable.h"

#include "../RanLogicServer/DxServerInstance.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "CompareTool.h"
#include "CompareToolDlg.h"
#include "CompareTargetDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

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

// CCompareToolDlg 대화 상자

CCompareToolDlg::CCompareToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompareToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    GLogicData::GetInstance().LoadCountryFile();
    m_pPath = new SUBPATH(GLogicData::GetInstance().GetServiceProvider());
    m_pEngineDevice = new DxResponseMan(GLogicData::GetInstance().GetServiceProvider(), m_pPath);
}

CCompareToolDlg::~CCompareToolDlg()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;

    delete m_pPath;
    m_pPath = NULL;
}

void CCompareToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COMPARE_RESULT, m_list_CompareResult);
}

BEGIN_MESSAGE_MAP(CCompareToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOAD_A, OnBnClickedButtonLoadA)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_B, OnBnClickedButtonLoadB)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_CSVSAVE, OnBnClickedButtonCsvsave)
	ON_BN_CLICKED(IDC_BUTTON_COMPARE, OnBnClickedButtonCompare)
	ON_BN_CLICKED(IDC_BUTTON_COMPARE_LIST, OnBnClickedButtonCompareList)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_COMPARE_RESULT, OnNMCustomdrawListCompareResult)
    ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_BN_CLICKED(IDC_BUTTON_CSVSAVE2, OnBnClickedButtonCsvsave2)
	ON_BN_CLICKED(IDC_RADIO_PART, OnBnClickedRadioPart)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_BUTTON_COMPARE_TARGET, OnBnClickedButtonCompareTarget)
	ON_BN_CLICKED(IDC_RADIO_EDIT_SKILL, OnBnClickedRadioEditSkill)
	ON_BN_CLICKED(IDC_RADIO_EDIT_MOBNPC, OnBnClickedRadioEditMobnpc)
	ON_BN_CLICKED(IDC_RADIO_EDIT_ITEM, OnBnClickedRadioEditItem)
    ON_WM_SIZE()
ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CCompareToolDlg 메시지 처리기

BOOL CCompareToolDlg::OnInitDialog()
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
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		DWORD dwFind = strAppPath.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strAppPath = strAppPath.Left ( dwFind );

			if ( !m_strAppPath.IsEmpty() )
				if ( m_strAppPath.GetAt(0) == '"' )
					m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );

			strcpy ( m_szAppPath, m_strAppPath.GetString() );
		}
	}

	m_pEngineDevice->OneTimeSceneInit(
        m_szAppPath,
        this,
        RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont );

    GLCrowDataMan::GetInstance().OneTimeSceneInit();
	
	m_list_CompareResult.SetExtendedStyle ( m_list_CompareResult.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_CompareResult.InsertColumn( 0, "DifferNum", LVCFMT_LEFT, 70 );
	m_list_CompareResult.InsertColumn( 1, "Type", LVCFMT_LEFT, 50 );
	m_list_CompareResult.InsertColumn( 2, "MID", LVCFMT_LEFT, 40 );
	m_list_CompareResult.InsertColumn( 3, "SID", LVCFMT_LEFT, 40 );




	m_setCompareTargetStr.clear();
	


	CheckRadioButton( IDC_RADIO_EDIT_SKILL, IDC_RADIO_EDIT_ITEM, IDC_RADIO_EDIT_SKILL );

	CheckRadioButton( IDC_RADIO_ALL, IDC_RADIO_PART, IDC_RADIO_ALL );

	SetWin_Enable( this, IDC_BUTTON_COMPARE_TARGET, FALSE );
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CCompareToolDlg::OnDestroy ()
{
    CDialog::OnDestroy();

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    GLCrowDataMan::GetInstance().FinalCleanup ();
    m_pEngineDevice->FinalCleanup ();
}

void CCompareToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCompareToolDlg::OnPaint() 
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
HCURSOR CCompareToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCompareToolDlg::LoadCompareFile( bool bLoadA )
{

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", (CCompareToolDlg*)this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		if( bLoadA ) SetWin_Text( this, IDC_STATIC_NAME_A, dlg.GetFileName().GetString() );
		else		 SetWin_Text( this, IDC_STATIC_NAME_B, dlg.GetFileName().GetString() );	
	}

	


}

// LOAD A
void CCompareToolDlg::OnBnClickedButtonLoadA()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LoadCompareFile( TRUE );
}

// LOAD B
void CCompareToolDlg::OnBnClickedButtonLoadB()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LoadCompareFile( FALSE );
}

// NEW Button
void CCompareToolDlg::OnBnClickedButtonNew()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_list_CompareResult.DeleteAllItems();
	m_setCompareTargetStr.clear();
	m_mapDefferData.clear();
	SetWin_Text( this, IDC_STATIC_NAME_A, NULL );
	SetWin_Text( this, IDC_STATIC_NAME_B, NULL );	

	CheckRadioButton( IDC_RADIO_ALL, IDC_RADIO_PART, IDC_RADIO_ALL );

	SetWin_Enable( this, IDC_BUTTON_COMPARE_TARGET, FALSE );
	
}

// SAVE Button
void CCompareToolDlg::OnBnClickedButtonCsvsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return;

	std::fstream streamFILE;
	streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

	MAP_DEFFER_ITER iter = m_mapDefferData.begin();
	CStringArray strHeadResultArray;
	int iMaxHeadCount = 0;
	for( ; iter != m_mapDefferData.end(); ++iter )
	{
		SNATIVEID sNativeID = iter->first;		
		STRUTIL::StringSeparate( iter->second.strDifferHead, strHeadResultArray );		

		iMaxHeadCount = (int)strHeadResultArray.GetCount();

		// Save Header
		streamFILE << iMaxHeadCount << ",";
		streamFILE << "HEAD,MID,SID,";
		streamFILE << iter->second.strDifferHead << std::endl;
		// Save A Value
		streamFILE << iMaxHeadCount << ",";
		streamFILE << "A,";
		streamFILE << sNativeID.wMainID << ",";
		streamFILE << sNativeID.wSubID << ",";
		streamFILE << iter->second.strDifferData_A << std::endl;
		// Save B Value
		streamFILE << iMaxHeadCount << ",";
		streamFILE << "B,";
		streamFILE << sNativeID.wMainID << ",";
		streamFILE << sNativeID.wSubID << ",";
		streamFILE << iter->second.strDifferData_B << std::endl;
	}


	
	streamFILE.close();
	
}

// 비교대상 Csv Save
void CCompareToolDlg::OnBnClickedButtonCsvsave2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return;

	std::fstream streamFILE;
	streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

	// 비교대상 Csv Header Save
    streamFILE << "MID, SID, DifferNum" << std::endl;

	CStringArray strHeadResultArray;
	int iMaxHeadCount = 0;
	std::set<DWORD,Align>::iterator iter = m_setCompareID.begin();

	for( ; iter != m_setCompareID.end(); ++iter )
	{
		SNATIVEID sNativeID = *iter;
		STRUTIL::StringSeparate( m_mapDefferData[*iter].strDifferHead, strHeadResultArray );
		iMaxHeadCount = (int)strHeadResultArray.GetCount();
		streamFILE << sNativeID.wMainID << ",";
		streamFILE << sNativeID.wSubID  << ",";
		streamFILE << iMaxHeadCount << ",";
		streamFILE << std::endl;
	}
}


// 비교 대상
void CCompareToolDlg::OnBnClickedButtonCompareList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

// 비교
void CCompareToolDlg::OnBnClickedButtonCompare()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    switch( GetCheckedRadioButton( IDC_RADIO_EDIT_SKILL, IDC_RADIO_EDIT_ITEM ) )
    {
    case IDC_RADIO_EDIT_SKILL:
        {
            //CompareSkillData();
            CompareData();
        }
        break;

    case IDC_RADIO_EDIT_MOBNPC:
        {
            CompareData();
        }
        break;

    case IDC_RADIO_EDIT_ITEM:
        {
            CompareData();
        }
        break;
    }
	/*if( GetWin_Check(this,IDC_RADIO_EDIT_SKILL) )
	{
		CompareSkillData();
	}else
	{
		CompareMobNpcData();
	}*/
}

void CCompareToolDlg::AddDefferData( SNATIVEID sNativedID, CString strHead, CString strData_A, CString strData_B )
{
	SDefferData sDefferData = m_mapDefferData[sNativedID.dwID];

	if( strData_A == strData_B ) return;


	sDefferData.strDifferHead += strHead;
	sDefferData.strDifferHead += ",";
	if( strData_A.GetLength() != 0 ) sDefferData.strDifferData_A += strData_A;
	else							 sDefferData.strDifferData_A += " ";
	sDefferData.strDifferData_A += ",";

	if( strData_B.GetLength() != 0 ) sDefferData.strDifferData_B += strData_B;
	else							 sDefferData.strDifferData_B += " ";
	sDefferData.strDifferData_B += ",";

	m_mapDefferData[sNativedID.dwID] = sDefferData;

}

// 리스트에 인서트
void CCompareToolDlg::UpdateCompareList()
{
	CStringArray strHeadResultArray;
	CStringArray strResultArray_A;
	CStringArray strResultArray_B;
	CString strTemp;

	MAP_DEFFER_ITER iter = m_mapDefferData.begin();
	int iMaxColumn = 0;

	for( int i = 0; i < 500; i++ )
		m_list_CompareResult.DeleteColumn( 4 + i );

	for( ; iter != m_mapDefferData.end(); ++iter )
	{
		STRUTIL::StringSeparate( iter->second.strDifferHead, strHeadResultArray );
		if( iMaxColumn < strHeadResultArray.GetCount() )
			iMaxColumn = (int)strHeadResultArray.GetCount();

	}	

	for( DWORD i = 0; i < (DWORD)iMaxColumn; i++ )
	{		
		strTemp.Format( "Value_%d", i );
		m_list_CompareResult.InsertColumn( 4+i, strTemp.GetString(), LVCFMT_LEFT, 100 );		
	}

	int iCount = 0, iMaxHeadCount;	
	iter = m_mapDefferData.begin();
	for( ; iter != m_mapDefferData.end(); ++iter )
	{
		SNATIVEID sNativeID = iter->first;
		STRUTIL::StringSeparate( iter->second.strDifferHead, strHeadResultArray );
		STRUTIL::StringSeparate( iter->second.strDifferData_A, strResultArray_A );
		STRUTIL::StringSeparate( iter->second.strDifferData_B, strResultArray_B );

		iMaxHeadCount = (int)strHeadResultArray.GetCount();
		strTemp.Format( "%d", iMaxHeadCount );

		m_list_CompareResult.InsertItem( iCount,   strTemp.GetString(), NULL );
		m_list_CompareResult.InsertItem( iCount+1, strTemp.GetString(), NULL );
		m_list_CompareResult.InsertItem( iCount+2, strTemp.GetString(), NULL );

		m_list_CompareResult.SetItemText( iCount,   1, "HEAD" );
		m_list_CompareResult.SetItemText( iCount+1, 1, "A" );
		m_list_CompareResult.SetItemText( iCount+2, 1, "B" );

		m_list_CompareResult.SetItemText( iCount,   2, "MID" );
		strTemp.Format( "%d", sNativeID.wMainID );
		m_list_CompareResult.SetItemText( iCount+1, 2, strTemp.GetString() );
		m_list_CompareResult.SetItemText( iCount+2, 2, strTemp.GetString() );

		m_list_CompareResult.SetItemText( iCount,   3, "SID" );
		strTemp.Format( "%d", sNativeID.wSubID );
		m_list_CompareResult.SetItemText( iCount+1, 3, strTemp.GetString() );
		m_list_CompareResult.SetItemText( iCount+2, 3, strTemp.GetString() );

		for( DWORD i = 0; i < (DWORD)strHeadResultArray.GetCount(); i++ )
		{
			m_list_CompareResult.SetItemText( iCount,   4+i, strHeadResultArray[i] );
			m_list_CompareResult.SetItemText( iCount+1, 4+i, strResultArray_A[i] );
			m_list_CompareResult.SetItemText( iCount+2, 4+i, strResultArray_B[i] );	
		}

		iCount += 3;

	}

}

// 데이터 비교
void CCompareToolDlg::CompareData()
{
	CString strFileName_A = GetWin_Text( this, IDC_STATIC_NAME_A );
	CString strFileName_B = GetWin_Text( this, IDC_STATIC_NAME_B );

	if( strFileName_A.GetLength() == 0 ||
		strFileName_B.GetLength() == 0 )
	{
		MessageBox( "비교할 파일을 불러와야 합니다.", "알림" );
		return;
	}

	sc::CStringFile StrFile_A( 102400 );
	sc::CStringFile StrFile_B( 102400 );

	CString strLine;
	CStringArray strHeadArray_A;
	CStringArray strDataArray_A;

	CStringArray strHeadArray_B;
	CStringArray strDataArray_B;

	CString strTemp1, strTemp2;

	int iMID = 0;
	int iSID = 0;


	if( !StrFile_A.Open ( strFileName_A.GetString() ) )
		return;
	if( !StrFile_B.Open ( strFileName_B.GetString() ) )
		return;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );


	// 초기화
	m_list_CompareResult.DeleteAllItems();
	m_mapDefferData.clear();
	m_setCompareID.clear();


	// Load Head A
	if( !StrFile_A.GetNextLine( strLine ) ) return;
	STRUTIL::StringSeparate( strLine, strHeadArray_A );

	// Load Head B
	if( !StrFile_B.GetNextLine( strLine ) ) return;
	STRUTIL::StringSeparate( strLine, strHeadArray_B );

	SetWin_Text( this, IDC_BUTTON_COMPARE, "비교중" );

	// 헤더를 다 읽었다면 버퍼사이즈를 줄인다;
	StrFile_A.ChangeBufferSize( 10240 );
	StrFile_B.ChangeBufferSize( 10240 );

	// 데이터를 읽은후 list와 vector에 저장을 한다.
	std::list<CString> listStrDataA;
	std::map<DWORD,CString> mapStrDataB;
	std::map<DWORD,CString>::iterator mapIter;
	while(1)
	{
		// Load Data
		if( !StrFile_A.GetNextLine( strLine ) ) break;
		listStrDataA.push_back( strLine );
	}
	while(1)
	{
		// Load Data
		if( !StrFile_B.GetNextLine( strLine ) ) break;

		STRUTIL::StringSeparate( strLine, strDataArray_B );

		iMID	  = atoi(strDataArray_B[0]);
		iSID	  = atoi(strDataArray_B[1]);

		mapStrDataB.insert( std::make_pair( SNATIVEID(iMID,iSID).dwID, strLine) );
	}

	std::list<CString>::iterator iter_A;
	
	for( iter_A = listStrDataA.begin(); iter_A != listStrDataA.end(); ++iter_A )
	{
		// Load Data
		strLine = *iter_A;
		STRUTIL::StringSeparate( *iter_A, strDataArray_A );

		if ( strHeadArray_A.GetCount() != strDataArray_A.GetCount() )
		{
			TCHAR szError[260] = {0};
			StringCchPrintf( szError, 260, "Head Count : %d, Data Count : %d!!",
				strHeadArray_A.GetCount(),
				strDataArray_A.GetCount() );
			::MessageBox ( NULL, szError, "Warning !", MB_ICONEXCLAMATION|MB_OK );
			break;
		}

		iMID	  = atoi(strDataArray_A[0]);
		iSID	  = atoi(strDataArray_A[1]);

		mapIter = mapStrDataB.find( SNATIVEID(iMID,iSID).dwID );
		if( mapIter != mapStrDataB.end() )
		{
			STRUTIL::StringSeparate( mapIter->second, strDataArray_B );
		}else continue;


		// Mid Sid 를 찾았을 경우
		CString strBasicHead, strApplyHead;
		SNATIVEID sNativeID = SNATIVEID( iMID, iSID );
		// 찾았을 경우 리스트에 추가한다.
		m_setCompareID.insert( sNativeID.dwID );

		// Basic 데이터에 다른점을 찾는다.
		int i, j;
		for( i = 0; i < strHeadArray_A.GetCount(); i++ )
		{
			strBasicHead = strHeadArray_A[i];

			// All과 Part 비교를 구분한다.
			if( GetWin_Check( this, IDC_RADIO_PART ) )
			{
				if( m_setCompareTargetStr.find( strBasicHead ) == m_setCompareTargetStr.end() )
					continue;
			}

			for( j = 0; j < strHeadArray_B.GetCount(); j++ )
			{
				if( strBasicHead == strHeadArray_B[j] ) break;					
			}

			// 못 찾았을 경우
			if( j == strHeadArray_B.GetCount() )
			{
				AddDefferData( sNativeID, strBasicHead, strDataArray_A[i], NULL );
			}else{ // 찾았을 경우
				strTemp1 = strDataArray_A[i];
				strTemp2 = strDataArray_B[j];
				if( strDataArray_A[i] != strDataArray_B[j] )
				{
					AddDefferData( sNativeID, strBasicHead, strDataArray_A[i], strDataArray_B[i] );
				}
			}

		}
	}

	SetWin_Text( this, IDC_BUTTON_COMPARE, "비교" );

	UpdateCompareList();
}



void CCompareToolDlg::OnNMCustomdrawListCompareResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	/*RECT rc;
	m_list_CompareResult.GetItemRect( 0, &rc, FALSE );

	CDC* pDC = CDC::FromHandle(pNMCD->hdc);
	pDC->FillSolidRect(&rc, GetSysColor(COLOR_HIGHLIGHTTEXT) ); 
	pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ;*/

	*pResult = 0;
}

void CCompareToolDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if(nIDCtl == IDC_LIST_COMPARE_RESULT){
		// m_list_box : 리스트박스의 객체
		// lpDrawItemStruct->itemID : 리스트박스 아이템의 ID(인덱스)
		// 넘겨온 ID(인덱스)가 리스트박스에 존재하는 ID(인덱스)인가 체크하고
		// 리스트박스에 추가된 아이템이 없다면 Owner Draw 할 것이 없으므로 return;
		if(lpDrawItemStruct->itemID >= (UINT)m_list_CompareResult.GetItemCount() || !m_list_CompareResult.GetItemCount()) return;

		// lpDrawItemStruct->itemAction : 아이템에 발생된 정보
		switch(lpDrawItemStruct->itemAction){
			// 아이템에 포커스 존재하고 선택여부
			case ODA_DRAWENTIRE: case ODA_FOCUS: case ODA_SELECT:

				// Selected 아이템과 Non select 아이템을 구별하여 Draw 한다.
				if(lpDrawItemStruct->itemState & ODS_SELECTED){
					DrawCursor(lpDrawItemStruct, 1);
					DrawItem(lpDrawItemStruct, 1);

				} else {
					DrawCursor(lpDrawItemStruct, 0);
					DrawItem(lpDrawItemStruct, 0);
					break;
				}
		}
	} else CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);

}

void CCompareToolDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nIDCtl == IDC_LIST_COMPARE_RESULT){
		// 리스트박스 아이템의 높이
		lpMeasureItemStruct->itemHeight = 20;
	}
	CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

// 리스트박스 아이템의 정보를 Draw 하는 함수
void CCompareToolDlg::DrawItem(LPDRAWITEMSTRUCT lpDS, char parm_push_flag)
{
	CRect r(lpDS->rcItem);
	CDC *pDC = CDC::FromHandle(lpDS->hDC);


	// 헤더
	if(parm_push_flag)
	{
		pDC->SetTextColor(0x4080FF);
	}else{
		pDC->SetTextColor(0x915F36);
	}

	int old_mode = pDC->SetBkMode(TRANSPARENT);

	// 리스트박스 아이템에 추가된 문자열을 얻어온다.
	TCHAR  lpBuffer[256];
	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_PARAM ;
	lvi.iItem = lpDS->itemID ; 	
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer ;
	lvi.cchTextMax = sizeof(lpBuffer);
	m_list_CompareResult.GetItem(&lvi);

	LV_COLUMN lvc, lvcprev;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;

	RECT rc;
	rc = lpDS->rcItem;
	rc.top += 3;

	CFont fontMain, fontSub;
	fontMain.CreateFont(16,                        // nHeight
						0,                         // nWidth
						0,                         // nEscapement
						0,                         // nOrientation
						FW_BOLD,                   // nWeight
						FALSE,                     // bItalic
						FALSE,                     // bUnderline
						0,                         // cStrikeOut
						ANSI_CHARSET,              // nCharSet
						OUT_DEFAULT_PRECIS,        // nOutPrecision
						CLIP_DEFAULT_PRECIS,       // nClipPrecision
						DEFAULT_QUALITY,           // nQuality
						DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
						"굴림체");

	fontSub.CreateFont(14,                        // nHeight
					   0,                         // nWidth
					   0,                         // nEscapement
					   0,                         // nOrientation
					   FW_NORMAL,                 // nWeight
					   FALSE,                     // bItalic
					   FALSE,                     // bUnderline
					   0,                         // cStrikeOut
					   ANSI_CHARSET,              // nCharSet
					   OUT_DEFAULT_PRECIS,        // nOutPrecision
					   CLIP_DEFAULT_PRECIS,       // nClipPrecision
					   DEFAULT_QUALITY,           // nQuality
					   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
					   "굴림체");
	for ( int nCol=0; m_list_CompareResult.GetColumn(nCol, &lvc); nCol++)
	{
		if ( nCol > 0 ) 
		{
			// Get Previous Column Width in order to move the next display item
			m_list_CompareResult.GetColumn(nCol-1, &lvcprev) ;
			rc.left += lvcprev.cx;
		}

		rc.right = rc.left + lvc.cx;

		// Get the text 
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDS->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		m_list_CompareResult.GetItem(&lvi);

		RECT drawRC = rc;
		drawRC.left  += 3;
		drawRC.right -= 3;

		if( nCol < 4 )
		{
			pDC->SelectObject(&fontMain);
		}else{
			pDC->SelectObject(&fontSub);
		}

		// 얻어온 문자열을 출력한다.
		//pDC->TextOut(lpDS->rcItem.left + 10, lpDS->rcItem.top + 5, lpBuffer);
		pDC->DrawText( lpBuffer, &drawRC, DT_MODIFYSTRING );
		pDC->SetBkMode(old_mode);

	}

	
}


// 리스트박스 아이템의 커서를 그리는 함수
void CCompareToolDlg::DrawCursor(LPDRAWITEMSTRUCT lpDS, char parm_push_flag)
{
	CDC *pDC = CDC::FromHandle(lpDS->hDC);
	CBrush fill_brush, *p_old_brush = NULL;
	CPen border_pen , *p_old_pen = NULL;
	CRect r(lpDS->rcItem);

	// 헤더
	if(parm_push_flag)
	{
		border_pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		fill_brush.CreateSolidBrush(0xD2D3EF);
	}else{
		if( lpDS->itemID % 3 == 0 )
		{
			border_pen.CreatePen(PS_SOLID, 3, 0xD2D3EF);
			fill_brush.CreateSolidBrush(0xEEDFD3);
		}else{
			border_pen.CreatePen(PS_SOLID, 0, 0xEEDFD3);
			fill_brush.CreateSolidBrush(RGB(255, 255, 255));
		}
	}

	p_old_pen = pDC->SelectObject(&border_pen);
	p_old_brush = pDC->SelectObject(&fill_brush);

	pDC->Rectangle(r);
	pDC->SelectObject(p_old_brush);
	pDC->SelectObject(p_old_pen);

	fill_brush.DeleteObject();
	border_pen.DeleteObject();
}



void CCompareToolDlg::OnBnClickedRadioAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetWin_Enable( this, IDC_BUTTON_COMPARE_TARGET, FALSE );
}

void CCompareToolDlg::OnBnClickedRadioPart()
{
	SetWin_Enable( this, IDC_BUTTON_COMPARE_TARGET, TRUE );	
}

void CCompareToolDlg::OnBnClickedButtonCompareTarget()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strLine = GetWin_Text( this, IDC_STATIC_NAME_A );
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( strLine.GetLength() == 0 )
	{
		MessageBox( "A 파일을 불러와야 합니다.", "알림" );
		return;
	}		

	DWORD compareType;
	if( GetWin_Check( this, IDC_RADIO_EDIT_SKILL ) )
	{
		compareType = 0;
	}else
	{
		compareType = 1;
	}
	
	CCompareTargetDlg targetDlg;
	targetDlg.InitStrHeadArray( strLine.GetString(), m_setCompareTargetStr, compareType );
	if( targetDlg.DoModal() == IDOK )
	{
		m_setCompareTargetStr = targetDlg.GetCompareTargetStr();
	}
}
void CCompareToolDlg::OnBnClickedRadioEditSkill()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_setCompareTargetStr.clear();
}

void CCompareToolDlg::OnBnClickedRadioEditMobnpc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_setCompareTargetStr.clear();
}

void CCompareToolDlg::OnBnClickedRadioEditItem()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_setCompareTargetStr.clear();
}

void CCompareToolDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    m_list_CompareResult.MoveWindow( 12, 71, cx - 24, cy - 83 );
}
void CCompareToolDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    lpMMI->ptMinTrackSize.x = 759;
    lpMMI->ptMinTrackSize.y = 183;
    CDialog::OnGetMinMaxInfo(lpMMI);
}
