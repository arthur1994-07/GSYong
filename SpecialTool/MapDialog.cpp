// MapDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "SpecialTool.h"
#include "MapDialog.h"

#include "../SigmaCore/DebugInclude.h"

// Dialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CMapDialog, CDialog)
CMapDialog::CMapDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMapDialog::IDD, pParent)
	, m_sMapID ( false )
{
}

CMapDialog::~CMapDialog()
{
}

void CMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAP, m_cListMap);
}


BEGIN_MESSAGE_MAP(CMapDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CMapDialog �޽��� ó�����Դϴ�.

BOOL CMapDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

    m_sMapList.LoadMapList(); // "mapslist.luc"

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	TCHAR szName[128] = {0};

	GLMapList::FIELDMAP_ITER iter = m_sMapList.GetMapList().begin();
	GLMapList::FIELDMAP_ITER iter_end = m_sMapList.GetMapList().end();
	for ( ; iter!=iter_end; ++iter )
	{
		const SMAPNODE &sNODE = (*iter).second;

		_snprintf( szName, 128, "[%03u/%03u] %s", sNODE.m_NativeID.wMainID, sNODE.m_NativeID.wSubID, sNODE.m_MapID.c_str() );

		int nIndex = m_cListMap.AddString ( szName );
		m_cListMap.SetItemData ( nIndex, sNODE.m_NativeID.dwID );		
	}

	SetWin_Num_int( this, IDC_EDIT_FINDMID, m_sMapID.wMainID );
	SetWin_Num_int( this, IDC_EDIT_FINDSID, m_sMapID.wSubID );

	OnBnClickedButtonFind();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CMapDialog::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	int nSize = m_cListMap.GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) m_cListMap.GetItemData( i );

		if ( nMainID == sNativeID.wMainID && nSubID == sNativeID.wSubID )
		{
			m_cListMap.SetCurSel( i );
			return;
		}
	}
}

void CMapDialog::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_cListMap.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The Map was not selected.", "Caution", MB_OK );
		return;
	}

	m_sMapID.dwID = (DWORD) m_cListMap.GetItemData ( nIndex );

	OnOK();
}
