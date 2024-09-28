// CompareTargetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <set>
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/SerialFile.h"

#include "../EngineLib/Common/SeqRandom.h"
#include "../RanLogic/GLStringTable.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "CompareTool.h"
#include "CompareTargetDlg.h"


// CCompareTargetDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCompareTargetDlg, CDialog)
CCompareTargetDlg::CCompareTargetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompareTargetDlg::IDD, pParent)
{
}

CCompareTargetDlg::~CCompareTargetDlg()
{
}

void CCompareTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COMPARE_TARGET, m_list_CompareTarget);
	DDX_Control(pDX, IDC_LIST_COMPARE_SEARCH, m_list_CompareSearch);
	DDX_Control(pDX, IDC_LIST_COMPARE, m_list_Compare);
}


BEGIN_MESSAGE_MAP(CCompareTargetDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_HEAD_SEARCH, OnBnClickedButtonHeadSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_COMPARE_TARGET, OnLvnItemchangedListCompareTarget)
END_MESSAGE_MAP()


// CCompareTargetDlg 메시지 처리기입니다.

void CCompareTargetDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_setCompareTargetStr.clear();
	int iItemCount = m_list_CompareTarget.GetItemCount();
	char szTempChar[256];
	for( int i = 0; i < iItemCount; i++ )
	{
		if( m_list_CompareTarget.GetCheck( i ) )
		{
			m_list_CompareTarget.GetItemText( i, 1, szTempChar, 256 );
			m_setCompareTargetStr.insert( szTempChar );
		}
	}

	OnOK();
}

BOOL CCompareTargetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	RECT listRect;
	m_list_CompareTarget.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	m_list_CompareTarget.SetExtendedStyle ( m_list_CompareTarget.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES );
	m_list_CompareTarget.InsertColumn( 0, "Num", LVCFMT_LEFT, (int)((float)nListWidth * 0.1f) );
	m_list_CompareTarget.InsertColumn( 1, "Name", LVCFMT_LEFT, (int)((float)nListWidth * 0.8f) );
	m_list_CompareSearch.GetClientRect(&listRect);
	nListWidth = ( listRect.right - listRect.left );
	m_list_CompareSearch.SetExtendedStyle ( m_list_CompareSearch.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_CompareSearch.InsertColumn( 0, "Num", LVCFMT_LEFT, (int)((float)nListWidth * 0.2f) );
	m_list_CompareSearch.InsertColumn( 1, "Name", LVCFMT_LEFT, (int)((float)nListWidth * 0.7f) );
	nListWidth = ( listRect.right - listRect.left );
	m_list_Compare.SetExtendedStyle ( m_list_Compare.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_Compare.InsertColumn( 0, "Num", LVCFMT_LEFT, (int)((float)nListWidth * 0.2f) );
	m_list_Compare.InsertColumn( 1, "Name", LVCFMT_LEFT, (int)((float)nListWidth * 0.7f) );

	CString strLine;
	for( int i = 0; i < m_strHeadArray.GetCount() - 2; i++ )
	{
		strLine.Format( "%d", i );
		m_list_CompareTarget.InsertItem( i, strLine.GetString(), NULL );
		strLine = m_strHeadArray[i + 2];
		m_list_CompareTarget.SetItemText( i, 1, strLine.GetString() );

		if( m_setCompareTargetStr.find( strLine ) != m_setCompareTargetStr.end() )
		{
			m_list_CompareTarget.SetCheck( i, TRUE );
		}else{
			m_list_CompareTarget.SetCheck( i, FALSE );
		}
	
	}

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CCompareTargetDlg::InitStrHeadArray( const LPCSTR szFile, std::set<CString> setCompareTargetStr, DWORD compareType )
{
	sc::CStringFile StrFile( 102400 );
	CStringArray strBasicHeadArray;
	//CStringArray strApplyHeadArray;
	CString strLine;

	if( !StrFile.Open( szFile ) ) return;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	m_setCompareTargetStr.clear();
	m_setCompareTargetStr = setCompareTargetStr;

	if( compareType == 0 )
	{
		if( StrFile.GetNextLine( strLine ) )
			STRUTIL::StringSeparate( strLine, strBasicHeadArray );
		//if( StrFile.GetNextLine( strLine ) )
		//	STRUTIL::StringSeparate( strLine, strApplyHeadArray );
		
		m_strHeadArray.RemoveAll();

		m_strHeadArray.Append( strBasicHeadArray );
		//m_strHeadArray.Append( strApplyHeadArray );
	}else if( compareType == 1 )
	{
		if( StrFile.GetNextLine( strLine ) )
			STRUTIL::StringSeparate( strLine, strBasicHeadArray );
	
		m_strHeadArray.RemoveAll();

		m_strHeadArray.Append( strBasicHeadArray );
	}

	StrFile.Close();
	
}

void CCompareTargetDlg::OnBnClickedButtonHeadSearch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strSearch = GetWin_Text( this, IDC_EDIT_HEAD_SEARCH );
	CString strTemp;

	m_list_CompareSearch.DeleteAllItems();
	int iItemCount = m_list_CompareTarget.GetItemCount();
	char szTempChar[256];
	int iCount = 0;
	for( int i = 0; i < iItemCount; i++ )
	{
		m_list_CompareTarget.GetItemText( i, 1, szTempChar, 256 );
		strTemp = szTempChar;
		if( strstr( szTempChar, strSearch.GetString() ) != NULL )
//		if( strTemp.Find( strSearch.GetString() ) != -1 )
		{
			strTemp.Format( "%d", i );
			m_list_CompareSearch.InsertItem( iCount, strTemp.GetString(), NULL );
			m_list_CompareSearch.SetItemText( iCount, 1, szTempChar );
			iCount++;
		}
	}
	
}


void CCompareTargetDlg::OnLvnItemchangedListCompareTarget(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_list_Compare.DeleteAllItems();
	CString strTemp;
	int iItemCount = m_list_CompareTarget.GetItemCount(), iCount = 0;
	char szTempChar[256];
	for( int i = 0; i < iItemCount; i++ )
	{
		if( m_list_CompareTarget.GetCheck( i ) )
		{
			m_list_CompareTarget.GetItemText( i, 1, szTempChar, 256 );

			strTemp.Format( "%d", i );
			m_list_Compare.InsertItem( iCount, strTemp.GetString(), NULL );
			m_list_Compare.SetItemText( iCount, 1, szTempChar );
			iCount++;
		}
	}
	*pResult = 0;
}
