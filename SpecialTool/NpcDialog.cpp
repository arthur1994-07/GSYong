// NpcDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "SpecialTool.h"
#include "NpcDialog.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"


// CNpcDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CNpcDialog, CDialog)
CNpcDialog::CNpcDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNpcDialog::IDD, pParent)
	, m_sNpcID ( false )
	, m_bMob ( false )
{
}

CNpcDialog::~CNpcDialog()
{
}

void CNpcDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NPC, m_cNpcList);
}


BEGIN_MESSAGE_MAP(CNpcDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CNpcDialog �޽��� ó�����Դϴ�.

BOOL CNpcDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	TCHAR szName[128] = {0};

	for ( WORD i=0; i<GLCrowDataMan::MAX_CROW_MID; ++i )
	for ( WORD j=0; j<GLCrowDataMan::MAX_CROW_SID; ++j )
	{
		PCROWDATA pDATA = GLCrowDataMan::GetInstance().GetCrowData ( i, j );
		if ( !pDATA )	continue;

		if ( m_bMob && pDATA->m_emCrow!=CROW_MOB )	continue;
		if ( !m_bMob && pDATA->m_emCrow==CROW_MOB )	continue;

		_snprintf( szName, 128, "[%03u/%03u] %s", i, j, pDATA->GetName() );

		int nIndex = m_cNpcList.AddString ( szName );

		m_cNpcList.SetItemData ( nIndex, pDATA->sNativeID.dwID );		
	}


	SetWin_Num_int( this, IDC_EDIT_FINDMID, m_sNpcID.wMainID );
	SetWin_Num_int( this, IDC_EDIT_FINDSID, m_sNpcID.wSubID );

	OnBnClickedButtonFind();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CNpcDialog::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	int nSize = m_cNpcList.GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) m_cNpcList.GetItemData( i );

		if ( nMainID == sNativeID.wMainID && nSubID == sNativeID.wSubID )
		{
			m_cNpcList.SetCurSel( i );
			return;
		}
	}
}

void CNpcDialog::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_cNpcList.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The Crow was not selected.", "Caution", MB_OK );
		return;
	}

	m_sNpcID.dwID = (DWORD) m_cNpcList.GetItemData ( nIndex );

	OnOK();
}

