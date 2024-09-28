// ItemDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "SpecialTool.h"
#include "ItemDialog.h"

#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"


// CItemDialog ��ȭ �����Դϴ�.

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


// CItemDialog �޽��� ó�����Դϴ�.

BOOL CItemDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

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

		//strName.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}

	SetWin_Num_int( this, IDC_EDIT_FINDMID, m_nidITEM.wMainID );
	SetWin_Num_int( this, IDC_EDIT_FINDSID, m_nidITEM.wSubID );

	OnBnClickedButtonFind();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CItemDialog::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
