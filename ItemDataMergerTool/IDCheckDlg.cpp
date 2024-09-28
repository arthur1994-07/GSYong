// IDCheckDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Item/GLItem.h"

#include "ItemDataMergerTool.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "IDCheckDlg.h"

// CIDCheckDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CIDCheckDlg, CDialog)
CIDCheckDlg::CIDCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIDCheckDlg::IDD, pParent)
{
}

CIDCheckDlg::CIDCheckDlg(const SNATIVEID sNativeID, const CString strItemName, GLItemMan *pItemMan )
	: CDialog(CIDCheckDlg::IDD, NULL)
	, m_sItemID(sNativeID)
	, m_pItemMan(pItemMan)
	, m_strItemName(strItemName)
{
}

CIDCheckDlg::~CIDCheckDlg()
{
}

void CIDCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIDCheckDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, OnBnClickedSearchButton)
END_MESSAGE_MAP()


// CIDCheckDlg 메시지 처리기입니다.

BOOL CIDCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetWin_Num_int( this, IDC_MID_EDIT, m_sItemID.wMainID ); 
	SetWin_Num_int( this, IDC_SID_EDIT, m_sItemID.wSubID );

	SetWin_Text( this, IDC_NAME_STATIC, m_strItemName.GetString() );


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CIDCheckDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sItemID.wMainID = GetWin_Num_int( this, IDC_MID_EDIT ); 
	m_sItemID.wSubID  = GetWin_Num_int( this, IDC_SID_EDIT );
	OnOK();
}

void CIDCheckDlg::OnBnClickedSearchButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sItemID.wMainID = GetWin_Num_int( this, IDC_MID_EDIT ); 
	m_sItemID.wSubID  = GetWin_Num_int( this, IDC_SID_EDIT );
	SITEM *pItem = m_pItemMan->GetItem( m_sItemID );
	if( pItem )
	{
		CString strTempChar;
		strTempChar.Format( "%s 아이템이 이미 사용하고 있는 아이디 입니다.", pItem->GetName() );
		SetWin_Text( this, IDC_MSG_STATIC, strTempChar.GetString() );
	}else{
		SetWin_Text( this, IDC_MSG_STATIC, "사용하고 있지 않은 아이디 입니다." );		
	}

}
