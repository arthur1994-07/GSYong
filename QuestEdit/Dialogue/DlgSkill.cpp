// DlgSkill.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../QuestEdit.h"
#include "./DlgSkill.h"

//#include <strstream>
#include "../../RanLogic/Skill/GLSkill.h"
#include "./DlgSkill.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"


// CDlgSkill ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgSkill, CDialog)
CDlgSkill::CDlgSkill(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSkill::IDD, pParent)
{
}

CDlgSkill::~CDlgSkill()
{
}

void CDlgSkill::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SKILL, m_listSKILL);
}


BEGIN_MESSAGE_MAP(CDlgSkill, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CDlgSkill �޽��� ó�����Դϴ�.

BOOL CDlgSkill::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szName[128] = {0};

	for ( WORD i=0; i<EMSKILLCLASS_NSIZE; ++i )
	for ( WORD j=0; j<GLSkillMan::MAX_CLASSSKILL; ++j )
	{
		PGLSKILL pDATA = GLSkillMan::GetInstance().GetData ( i, j );
		if ( !pDATA )	continue;

		//std::strstream strName;
		//strName << "[" << i << "/" << j << "] ";
		//strName << pDATA->GetName() << std::ends;

		_snprintf( szName, 128, "[%03u/%03u] %s", i, j, pDATA->GetName() );

		int nIndex = m_listSKILL.AddString ( szName );

		m_listSKILL.SetItemData ( nIndex, pDATA->m_sBASIC.sNATIVEID.dwID );

		//strName.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgSkill::OnBnClickedOk()
{
	int nIndex = m_listSKILL.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The item was not selected.", "Caution", MB_OK );
		return;
	}

	m_nidSKILL.dwID = (DWORD) m_listSKILL.GetItemData ( nIndex );

	OnOK();
}

void CDlgSkill::OnBnClickedCancel()
{
	m_nidSKILL = SNATIVEID(false);

	OnCancel();
}

void CDlgSkill::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	int nSize = m_listSKILL.GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) m_listSKILL.GetItemData( i );

		if ( nMainID == sNativeID.wMainID && nSubID == sNativeID.wSubID )
		{
			m_listSKILL.SetCurSel( i );
			return;
		}
	}
}
