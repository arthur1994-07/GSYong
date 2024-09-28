// CompareToolDlg.cpp : ���� ����
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

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
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

// CCompareToolDlg ��ȭ ����

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


// CCompareToolDlg �޽��� ó����

BOOL CCompareToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CCompareToolDlg::OnDestroy ()
{
    CDialog::OnDestroy();

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CCompareToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CCompareToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCompareToolDlg::LoadCompareFile( bool bLoadA )
{

	//	Note : ���� ���� ���̾˷α׸� ����.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LoadCompareFile( TRUE );
}

// LOAD B
void CCompareToolDlg::OnBnClickedButtonLoadB()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LoadCompareFile( FALSE );
}

// NEW Button
void CCompareToolDlg::OnBnClickedButtonNew()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

// �񱳴�� Csv Save
void CCompareToolDlg::OnBnClickedButtonCsvsave2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return;

	std::fstream streamFILE;
	streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

	// �񱳴�� Csv Header Save
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


// �� ���
void CCompareToolDlg::OnBnClickedButtonCompareList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

// ��
void CCompareToolDlg::OnBnClickedButtonCompare()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

// ����Ʈ�� �μ�Ʈ
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

// ������ ��
void CCompareToolDlg::CompareData()
{
	CString strFileName_A = GetWin_Text( this, IDC_STATIC_NAME_A );
	CString strFileName_B = GetWin_Text( this, IDC_STATIC_NAME_B );

	if( strFileName_A.GetLength() == 0 ||
		strFileName_B.GetLength() == 0 )
	{
		MessageBox( "���� ������ �ҷ��;� �մϴ�.", "�˸�" );
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


	// �ʱ�ȭ
	m_list_CompareResult.DeleteAllItems();
	m_mapDefferData.clear();
	m_setCompareID.clear();


	// Load Head A
	if( !StrFile_A.GetNextLine( strLine ) ) return;
	STRUTIL::StringSeparate( strLine, strHeadArray_A );

	// Load Head B
	if( !StrFile_B.GetNextLine( strLine ) ) return;
	STRUTIL::StringSeparate( strLine, strHeadArray_B );

	SetWin_Text( this, IDC_BUTTON_COMPARE, "����" );

	// ����� �� �о��ٸ� ���ۻ���� ���δ�;
	StrFile_A.ChangeBufferSize( 10240 );
	StrFile_B.ChangeBufferSize( 10240 );

	// �����͸� ������ list�� vector�� ������ �Ѵ�.
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


		// Mid Sid �� ã���� ���
		CString strBasicHead, strApplyHead;
		SNATIVEID sNativeID = SNATIVEID( iMID, iSID );
		// ã���� ��� ����Ʈ�� �߰��Ѵ�.
		m_setCompareID.insert( sNativeID.dwID );

		// Basic �����Ϳ� �ٸ����� ã�´�.
		int i, j;
		for( i = 0; i < strHeadArray_A.GetCount(); i++ )
		{
			strBasicHead = strHeadArray_A[i];

			// All�� Part �񱳸� �����Ѵ�.
			if( GetWin_Check( this, IDC_RADIO_PART ) )
			{
				if( m_setCompareTargetStr.find( strBasicHead ) == m_setCompareTargetStr.end() )
					continue;
			}

			for( j = 0; j < strHeadArray_B.GetCount(); j++ )
			{
				if( strBasicHead == strHeadArray_B[j] ) break;					
			}

			// �� ã���� ���
			if( j == strHeadArray_B.GetCount() )
			{
				AddDefferData( sNativeID, strBasicHead, strDataArray_A[i], NULL );
			}else{ // ã���� ���
				strTemp1 = strDataArray_A[i];
				strTemp2 = strDataArray_B[j];
				if( strDataArray_A[i] != strDataArray_B[j] )
				{
					AddDefferData( sNativeID, strBasicHead, strDataArray_A[i], strDataArray_B[i] );
				}
			}

		}
	}

	SetWin_Text( this, IDC_BUTTON_COMPARE, "��" );

	UpdateCompareList();
}



void CCompareToolDlg::OnNMCustomdrawListCompareResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	/*RECT rc;
	m_list_CompareResult.GetItemRect( 0, &rc, FALSE );

	CDC* pDC = CDC::FromHandle(pNMCD->hdc);
	pDC->FillSolidRect(&rc, GetSysColor(COLOR_HIGHLIGHTTEXT) ); 
	pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ;*/

	*pResult = 0;
}

void CCompareToolDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	if(nIDCtl == IDC_LIST_COMPARE_RESULT){
		// m_list_box : ����Ʈ�ڽ��� ��ü
		// lpDrawItemStruct->itemID : ����Ʈ�ڽ� �������� ID(�ε���)
		// �Ѱܿ� ID(�ε���)�� ����Ʈ�ڽ��� �����ϴ� ID(�ε���)�ΰ� üũ�ϰ�
		// ����Ʈ�ڽ��� �߰��� �������� ���ٸ� Owner Draw �� ���� �����Ƿ� return;
		if(lpDrawItemStruct->itemID >= (UINT)m_list_CompareResult.GetItemCount() || !m_list_CompareResult.GetItemCount()) return;

		// lpDrawItemStruct->itemAction : �����ۿ� �߻��� ����
		switch(lpDrawItemStruct->itemAction){
			// �����ۿ� ��Ŀ�� �����ϰ� ���ÿ���
			case ODA_DRAWENTIRE: case ODA_FOCUS: case ODA_SELECT:

				// Selected �����۰� Non select �������� �����Ͽ� Draw �Ѵ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if(nIDCtl == IDC_LIST_COMPARE_RESULT){
		// ����Ʈ�ڽ� �������� ����
		lpMeasureItemStruct->itemHeight = 20;
	}
	CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

// ����Ʈ�ڽ� �������� ������ Draw �ϴ� �Լ�
void CCompareToolDlg::DrawItem(LPDRAWITEMSTRUCT lpDS, char parm_push_flag)
{
	CRect r(lpDS->rcItem);
	CDC *pDC = CDC::FromHandle(lpDS->hDC);


	// ���
	if(parm_push_flag)
	{
		pDC->SetTextColor(0x4080FF);
	}else{
		pDC->SetTextColor(0x915F36);
	}

	int old_mode = pDC->SetBkMode(TRANSPARENT);

	// ����Ʈ�ڽ� �����ۿ� �߰��� ���ڿ��� ���´�.
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
						"����ü");

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
					   "����ü");
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

		// ���� ���ڿ��� ����Ѵ�.
		//pDC->TextOut(lpDS->rcItem.left + 10, lpDS->rcItem.top + 5, lpBuffer);
		pDC->DrawText( lpBuffer, &drawRC, DT_MODIFYSTRING );
		pDC->SetBkMode(old_mode);

	}

	
}


// ����Ʈ�ڽ� �������� Ŀ���� �׸��� �Լ�
void CCompareToolDlg::DrawCursor(LPDRAWITEMSTRUCT lpDS, char parm_push_flag)
{
	CDC *pDC = CDC::FromHandle(lpDS->hDC);
	CBrush fill_brush, *p_old_brush = NULL;
	CPen border_pen , *p_old_pen = NULL;
	CRect r(lpDS->rcItem);

	// ���
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetWin_Enable( this, IDC_BUTTON_COMPARE_TARGET, FALSE );
}

void CCompareToolDlg::OnBnClickedRadioPart()
{
	SetWin_Enable( this, IDC_BUTTON_COMPARE_TARGET, TRUE );	
}

void CCompareToolDlg::OnBnClickedButtonCompareTarget()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strLine = GetWin_Text( this, IDC_STATIC_NAME_A );
	
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( strLine.GetLength() == 0 )
	{
		MessageBox( "A ������ �ҷ��;� �մϴ�.", "�˸�" );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_setCompareTargetStr.clear();
}

void CCompareToolDlg::OnBnClickedRadioEditMobnpc()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_setCompareTargetStr.clear();
}

void CCompareToolDlg::OnBnClickedRadioEditItem()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_setCompareTargetStr.clear();
}

void CCompareToolDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
    m_list_CompareResult.MoveWindow( 12, 71, cx - 24, cy - 83 );
}
void CCompareToolDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    lpMMI->ptMinTrackSize.x = 759;
    lpMMI->ptMinTrackSize.y = 183;
    CDialog::OnGetMinMaxInfo(lpMMI);
}
