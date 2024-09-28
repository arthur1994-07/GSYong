// DlgMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "./DlgSpecialActivity.h"
#include "./DlgMap.h"

//#include <strstream>
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../RanLogic/Land/GLMapList.h"


// CDlgMap 대화 상자입니다.

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


// CDlgMap 메시지 처리기입니다.

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

		//strName.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}

    SelectItem( m_nidMAP );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

    SelectItem( SNATIVEID( nMainID, nSubID ) );
}
