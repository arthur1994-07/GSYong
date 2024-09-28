// DlgQuest.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "./DlgQuest.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"

// CDlgQuest ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgQuest, CDialog)
CDlgQuest::CDlgQuest(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQuest::IDD, pParent),
	m_dwQuestID(UINT_MAX)
{
}

CDlgQuest::~CDlgQuest()
{
}

void CDlgQuest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_QUEST, m_listQuest);
}


BEGIN_MESSAGE_MAP(CDlgQuest, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgQuest �޽��� ó�����Դϴ�.
BOOL CDlgQuest::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listQuest.ResetContent();

	GLQuestMan::MAPQUEST & mapQuest = GLQuestMan::GetInstance().GetMap ();
	GLQuestMan::MAPQUEST_ITER iter = mapQuest.begin();
	GLQuestMan::MAPQUEST_ITER iter_end = mapQuest.end();
	for ( ; iter!=iter_end; ++iter )
	{
		DWORD dwQUESTID = (*iter).first;
		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );;
		if ( !pQUEST )	continue;

		CString strNAME;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
			// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
			strNAME.Format ( "[%d] %s", CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
		}
		else
		{
			strNAME.Format ( "[%d] %s", dwQUESTID, pQUEST->GetTITLE() );
		}

		int nIndex = m_listQuest.AddString ( strNAME );
		m_listQuest.SetItemData ( nIndex, pQUEST->m_sNID.dwID );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgQuest::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_listQuest.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = (DWORD) m_listQuest.GetItemData ( nIndex );
	m_dwQuestID = dwData;

	OnOK();
}

void CDlgQuest::OnBnClickedCancel()
{
	m_dwQuestID = UINT_MAX;

	OnCancel();
}
