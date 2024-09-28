// MapDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "SpecialTool.h"
#include "MapDialog.h"

#include "../SigmaCore/DebugInclude.h"

// Dialog 대화 상자입니다.

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


// CMapDialog 메시지 처리기입니다.

BOOL CMapDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

    m_sMapList.LoadMapList(); // "mapslist.luc"

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapDialog::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_cListMap.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The Map was not selected.", "Caution", MB_OK );
		return;
	}

	m_sMapID.dwID = (DWORD) m_cListMap.GetItemData ( nIndex );

	OnOK();
}
