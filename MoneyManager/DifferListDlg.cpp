// DifferListDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "./CODbcManager.h"
#include "./MoneyManager.h"
#include "./DifferListDlg.h"


// CDifferListDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDifferListDlg, CDialog)
CDifferListDlg::CDifferListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDifferListDlg::IDD, pParent)
{
}

CDifferListDlg::~CDifferListDlg()
{
}

void CDifferListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIFFERLIST, m_list_GameMoney);
	DDX_Control(pDX, IDC_LIST_DIFFERLIST2, m_list_STOMoney);
}


BEGIN_MESSAGE_MAP(CDifferListDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SAVE2, OnBnClickedButtonSave2)
END_MESSAGE_MAP()


// CDifferListDlg 메시지 처리기입니다.

BOOL CDifferListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	RECT listRect;
	m_list_GameMoney.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nColWidth[3] = { int(nListWidth*0.19f), int(nListWidth*0.4f), int(nListWidth*0.4f) };

	m_list_GameMoney.SetExtendedStyle ( m_list_GameMoney.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_STOMoney.SetExtendedStyle ( m_list_STOMoney.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_list_GameMoney.InsertColumn ( 0, _T("ChaNum"),	LVCFMT_CENTER, nColWidth[0] );
	m_list_STOMoney.InsertColumn ( 0, _T("UserNum"),	LVCFMT_CENTER, nColWidth[0] );
	if( m_emViewType == VIEW_CHECK )
	{
		SetWindowText( "Money Different List" );

		m_list_GameMoney.InsertColumn ( 1, _T("Money"),	LVCFMT_CENTER, nColWidth[1] );
		m_list_GameMoney.InsertColumn ( 2, _T("Temp Money"),LVCFMT_CENTER, nColWidth[2] );
		m_list_STOMoney.InsertColumn ( 1, _T("Money"),	LVCFMT_CENTER, nColWidth[1] );
		m_list_STOMoney.InsertColumn ( 2, _T("Temp Money"),LVCFMT_CENTER, nColWidth[2] );
	}else if( m_emViewType == VIEW_CORRECT ){
		SetWindowText( "Correct Money list" );

		m_list_GameMoney.InsertColumn ( 1, _T("Bad Money"),	LVCFMT_CENTER, nColWidth[1] );
		m_list_GameMoney.InsertColumn ( 2, _T("Correct Money"),LVCFMT_CENTER, nColWidth[2] );
		m_list_STOMoney.InsertColumn ( 1, _T("Bad Money"),	LVCFMT_CENTER, nColWidth[1] );
		m_list_STOMoney.InsertColumn ( 2, _T("Correct Money"),LVCFMT_CENTER, nColWidth[2] );
	}else if( m_emViewType == VIEW_UPDATE ){
		SetWindowText( "Update Money list" );
		
		m_list_GameMoney.InsertColumn ( 1, _T("Old Money"),	LVCFMT_CENTER, nColWidth[1] );
		m_list_GameMoney.InsertColumn ( 2, _T("New Money"),LVCFMT_CENTER, nColWidth[2] );
		m_list_STOMoney.InsertColumn ( 1, _T("Old Money"),	LVCFMT_CENTER, nColWidth[1] );
		m_list_STOMoney.InsertColumn ( 2, _T("New Money"),LVCFMT_CENTER, nColWidth[2] );
	}else if( m_emViewType == VIEW_NEWENCRYPT ){
		SetWindowText( "New-Encrypt Money list" );

		m_list_GameMoney.InsertColumn ( 1, _T("Old Money"),	LVCFMT_CENTER, nColWidth[1] );
		m_list_GameMoney.InsertColumn ( 2, _T("New Money"),LVCFMT_CENTER, nColWidth[2] );
		m_list_STOMoney.InsertColumn ( 1, _T("Old Money"),	LVCFMT_CENTER, nColWidth[1] );
		m_list_STOMoney.InsertColumn ( 2, _T("New Money"),LVCFMT_CENTER, nColWidth[2] );
	}

	char szValue[128];
	for( int i = 0; i < m_vecGameMoneyInfo.size(); i++ )
	{
		SMoneyInfo moneyInfo = m_vecGameMoneyInfo[i];
		_gcvt( moneyInfo.nNumber, 10, szValue );
		m_list_GameMoney.InsertItem( i, szValue );

		sprintf( szValue, "%I64d", moneyInfo.lnMoney1 );
		m_list_GameMoney.SetItemText( i, 1, szValue );

		sprintf( szValue, "%I64d", moneyInfo.lnMoney2 );
		m_list_GameMoney.SetItemText( i, 2, szValue );
	}

	for( int i = 0; i < m_vecSTOMoneyInfo.size(); i++ )
	{
		SMoneyInfo moneyInfo = m_vecSTOMoneyInfo[i];
		_gcvt( moneyInfo.nNumber, 10, szValue );
		m_list_STOMoney.InsertItem( i, szValue );

		sprintf( szValue, "%I64d", moneyInfo.lnMoney1 );
		m_list_STOMoney.SetItemText( i, 1, szValue );

		sprintf( szValue, "%I64d", moneyInfo.lnMoney2 );
		m_list_STOMoney.SetItemText( i, 2, szValue );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDifferListDlg::UpdateDifferList( const std::vector<SMoneyInfo>& vecGameMoneyInfo, const std::vector<SMoneyInfo>& vecSTOMoneyInfo, const VIEWTYPE emViewType )
{
	m_vecGameMoneyInfo	= vecGameMoneyInfo;
	m_vecSTOMoneyInfo	= vecSTOMoneyInfo;
	m_emViewType	= emViewType;
}

CString	CDifferListDlg::GetAppPath()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if (!strCommandLine.IsEmpty())
	{
		DWORD dwFind = strCommandLine.ReverseFind('\\');
		if (dwFind != -1)
		{
			strFullPath = strCommandLine.Left(dwFind);

			if (!strFullPath.IsEmpty())
				if (strFullPath.GetAt(0) == '"')
					strFullPath = strFullPath.Right(strFullPath.GetLength() - 1);
		}
	}
	return strFullPath;
}

BOOL CDifferListDlg::SaveEditList( std::vector<SMoneyInfo> vecEditChaInfo, BOOL bGameMoney /* = TRUE */ )
{

	CTime cCurTime = CTime::GetCurrentTime();

	CString strTime; 
	strTime.Format ( "%04d%02d%02d%02d%02d",
		cCurTime.GetYear(),
		cCurTime.GetMonth(),
		cCurTime.GetDay(),
		cCurTime.GetHour(),
		cCurTime.GetMinute() );

	CString strFileName;
	if( bGameMoney )
	{
		if( m_emViewType == VIEW_CHECK )
		{
			strFileName.Format( "%s\\GameMoneyDifferList.%s.txt", GetAppPath().GetString(), strTime );
		}else if( m_emViewType == VIEW_CORRECT )
		{
			strFileName.Format( "%s\\CorrectGameMoneyList.%s.txt", GetAppPath().GetString(), strTime );
		}else if( m_emViewType == VIEW_UPDATE )
		{
			strFileName.Format( "%s\\UpdateGameMoneyList.%s.txt", GetAppPath().GetString(), strTime );
		}else if( m_emViewType == VIEW_NEWENCRYPT )
		{
			strFileName.Format( "%s\\NewEncryptGameMoneyList.%s.txt", GetAppPath().GetString(), strTime );
		}
	}else{
		if( m_emViewType == VIEW_CHECK )
		{
			strFileName.Format( "%s\\StorageMoneyDifferList.%s.txt", GetAppPath().GetString(), strTime );
		}else if( m_emViewType == VIEW_CORRECT )
		{
			strFileName.Format( "%s\\CorrectStorageMoneyList.%s.txt", GetAppPath().GetString(), strTime );
		}else if( m_emViewType == VIEW_UPDATE )
		{
			strFileName.Format( "%s\\UpdateStorageMoneyList.%s.txt", GetAppPath().GetString(), strTime );
		}else if( m_emViewType == VIEW_NEWENCRYPT )
		{
			strFileName.Format( "%s\\NewEncryptStorageMoneyList.%s.txt", GetAppPath().GetString(), strTime );
		}
	}

	FILE* fp = _fsopen( strFileName.GetString(), "wt", _SH_DENYNO );
	if( !fp ) return FALSE;

	SMoneyInfo editInfo;
	for( int i = 0; i < vecEditChaInfo.size(); i++ )
	{
		editInfo = vecEditChaInfo[i];
		if( bGameMoney )
		{
			if( m_emViewType == VIEW_CHECK )
			{
				fprintf( fp, "ChaNum : %d. Money : %I64d, TempMoney : %I64d. \n", editInfo.nNumber, editInfo.lnMoney1, editInfo.lnMoney2 );
			}else if( m_emViewType == VIEW_CORRECT )
			{
				fprintf( fp, "ChaNum : %d. BadMoney : %I64d, CorrectMoney : %I64d. \n", editInfo.nNumber, editInfo.lnMoney1, editInfo.lnMoney2 );
			}else if( m_emViewType == VIEW_UPDATE )
			{
				fprintf( fp, "ChaNum : %d. OldMoney : %I64d => NewMoney : %I64d. \n", editInfo.nNumber, editInfo.lnMoney1, editInfo.lnMoney2 );
			}
		}else{
			if( m_emViewType == VIEW_CHECK )
			{
				fprintf( fp, "UserNum : %d. Money : %I64d, TempMoney : %I64d. \n", editInfo.nNumber, editInfo.lnMoney1, editInfo.lnMoney2 );
			}else if( m_emViewType == VIEW_CORRECT )
			{
				fprintf( fp, "UserNum : %d. BadMoney : %I64d, CorrectMoney : %I64d. \n", editInfo.nNumber, editInfo.lnMoney1, editInfo.lnMoney2 );
			}else if( m_emViewType == VIEW_UPDATE )
			{
				fprintf( fp, "UserNum : %d. OldMoney : %I64d => NewMoney : %I64d. \n", editInfo.nNumber, editInfo.lnMoney1, editInfo.lnMoney2 );
			}
		}
	}
	fclose(fp);

	return TRUE;
}

void CDifferListDlg::OnBnClickedButtonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SaveEditList( m_vecGameMoneyInfo, TRUE );
}

void CDifferListDlg::OnBnClickedButtonSave2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SaveEditList( m_vecSTOMoneyInfo, FALSE );
}
