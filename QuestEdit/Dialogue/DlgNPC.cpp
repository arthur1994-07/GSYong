// DlgNPC.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../QuestEdit.h"
#include "./DlgNPC.h"

//#include <strstream>
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "./DlgNPC.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"


// CDlgNPC ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgNPC, CDialog)
CDlgNPC::CDlgNPC(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNPC::IDD, pParent),
	m_bMob(false),
	m_nidNPC(false)
{
}

CDlgNPC::~CDlgNPC()
{
}

void CDlgNPC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NPC, m_listNPC);
}


BEGIN_MESSAGE_MAP(CDlgNPC, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CDlgNPC �޽��� ó�����Դϴ�.

BOOL CDlgNPC::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szName[128] = {0};

	for ( WORD i=0; i<GLCrowDataMan::MAX_CROW_MID; ++i )
	for ( WORD j=0; j<GLCrowDataMan::MAX_CROW_SID; ++j )
	{
		PCROWDATA pDATA = GLCrowDataMan::GetInstance().GetCrowData ( i, j );
		if ( !pDATA )	continue;

		if ( m_bMob && pDATA->m_emCrow!=CROW_MOB )	continue;
		if ( !m_bMob && pDATA->m_emCrow==CROW_MOB )	continue;

		//std::strstream strName;
		//strName << "[" << i << "/" << j << "] ";
		//strName << pDATA->GetName() << std::ends;

		_snprintf( szName, 128, "[%03u/%03u] %s", i, j, pDATA->GetName() );

		int nIndex = m_listNPC.AddString ( szName );

		m_listNPC.SetItemData ( nIndex, pDATA->sNativeID.dwID );

		//strName.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgNPC::OnBnClickedOk()
{
	int nIndex = m_listNPC.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The item was not selected.", "Caution", MB_OK );
		return;
	}

	m_nidNPC.dwID = (DWORD) m_listNPC.GetItemData ( nIndex );

	OnOK();
}

void CDlgNPC::OnBnClickedCancel()
{
	m_nidNPC = SNATIVEID(false);

	OnCancel();
}

void CDlgNPC::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	int nSize = m_listNPC.GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) m_listNPC.GetItemData( i );

		if ( nMainID == sNativeID.wMainID && nSubID == sNativeID.wSubID )
		{
			m_listNPC.SetCurSel( i );
			return;
		}
	}
}
