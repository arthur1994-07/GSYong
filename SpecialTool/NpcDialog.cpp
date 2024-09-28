// NpcDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "SpecialTool.h"
#include "NpcDialog.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"


// CNpcDialog 대화 상자입니다.

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


// CNpcDialog 메시지 처리기입니다.

BOOL CNpcDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CNpcDialog::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_cNpcList.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The Crow was not selected.", "Caution", MB_OK );
		return;
	}

	m_sNpcID.dwID = (DWORD) m_cNpcList.GetItemData ( nIndex );

	OnOK();
}

