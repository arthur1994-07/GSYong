// DlgMap.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "./DlgSpecialActivity.h"
#include "./DlgMap.h"

//#include <strstream>
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../RanLogic/Land/GLMapList.h"


// CDlgMap ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgMap, CDialog)
CDlgMap::CDlgMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMap::IDD, pParent),
	m_nidMAP(false),
    m_pMapList(NULL)
{
}

CDlgMap::~CDlgMap()
{
}

void CDlgMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAP, m_listMAP);
}


BEGIN_MESSAGE_MAP(CDlgMap, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CDlgMap �޽��� ó�����Դϴ�.

BOOL CDlgMap::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szName[128] = {0};    

    if ( !m_pMapList ) return FALSE;

	GLMapList::FIELDMAP_ITER iter = m_pMapList->GetMapList().begin();
	GLMapList::FIELDMAP_ITER iter_end = m_pMapList->GetMapList().end();


	for ( ; iter!=iter_end; ++iter )
	{
		const SMAPNODE &sNODE = (*iter).second;

		//std::strstream strName;
		//strName << "[" << sNODE.sNativeID.wMainID << "/" << sNODE.sNativeID.wSubID << "] ";
		//strName << sNODE.strMapName.c_str() << std::ends;

		_snprintf( szName, 128, "[%03u/%03u] %s", sNODE.GetId().wMainID, sNODE.GetId().wSubID, sNODE.m_MapID.c_str() );

		int nIndex = m_listMAP.AddString ( szName );
		m_listMAP.SetItemData ( nIndex, sNODE.GetId().dwID );

		//strName.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}

    SelectItem( m_nidMAP );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgMap::OnBnClickedOk()
{
	int nIndex = m_listMAP.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The item was not selected.", "Caution", MB_OK );
		return;
	}

	m_nidMAP.dwID = (DWORD) m_listMAP.GetItemData ( nIndex );

	OnOK();
}

void CDlgMap::OnBnClickedCancel()
{
	m_nidMAP = SNATIVEID(false);

	OnCancel();
}

void CDlgMap::SelectItem( SNATIVEID sID )
{
    if ( sID == NATIVEID_NULL() ) return;

	int nSize = m_listMAP.GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) m_listMAP.GetItemData( i );

		if ( sID.wMainID == sNativeID.wMainID && sID.wSubID == sNativeID.wSubID )
		{
			m_listMAP.SetCurSel( i );
			return;
		}
	}
}

void CDlgMap::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

    SelectItem( SNATIVEID( nMainID, nSubID ) );
}
