// DlgSkill.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../QuestEdit.h"
#include "./DlgSkill.h"

//#include <strstream>
#include "../../RanLogic/Skill/GLSkill.h"
#include "./DlgSkill.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"


// CDlgSkill 대화 상자입니다.

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


// CDlgSkill 메시지 처리기입니다.

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

		//strName.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
