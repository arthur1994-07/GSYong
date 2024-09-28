// ItemDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "SpecialTool.h"
#include "ItemDialog.h"

#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"


// CItemDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CItemDialog, CDialog)
CItemDialog::CItemDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CItemDialog::IDD, pParent)
{
}

CItemDialog::~CItemDialog()
{
}

void CItemDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEM, m_cItemList);
}


BEGIN_MESSAGE_MAP(CItemDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CItemDialog 메시지 처리기입니다.

BOOL CItemDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	TCHAR szItem[128] = {0};

	for ( WORD i=0; i<GLItemMan::MAX_MID; ++i )
	for ( WORD j=0; j<GLItemMan::MAX_SID; ++j )
	{
		SITEM* pITEM = GLogicData::GetInstance().GetItem ( i, j );
		if ( !pITEM )	continue;

		//std::strstream strName;
		//strName << "[" << i << "/" << j << "] ";
		//strName << pITEM->GetName() << std::ends;

		_snprintf( szItem, 128, "[%03u/%03u] %s", i, j, pITEM->GetName() );

		int nIndex = m_cItemList.AddString ( szItem );

		m_cItemList.SetItemData ( nIndex, pITEM->sBasicOp.sNativeID.dwID );

		//strName.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}

	SetWin_Num_int( this, IDC_EDIT_FINDMID, m_nidITEM.wMainID );
	SetWin_Num_int( this, IDC_EDIT_FINDSID, m_nidITEM.wSubID );

	OnBnClickedButtonFind();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CItemDialog::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	int nIndex = m_cItemList.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The item was not selected.", "Caution", MB_OK );
		return;
	}

	m_nidITEM.dwID = (DWORD) m_cItemList.GetItemData ( nIndex );

	OnOK();
}

void CItemDialog::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	int nSize = m_cItemList.GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) m_cItemList.GetItemData( i );

		if ( nMainID == sNativeID.wMainID && nSubID == sNativeID.wSubID )
		{
			m_cItemList.SetCurSel( i );
			return;
		}
	}
}
