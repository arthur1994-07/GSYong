// InspectionRewardPolicy.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServerManager.h"
#include "InspectionRewardPolicy.h"
#include "ServerManagerDlg.h"
#include "s_CNetCtrl.h"

#include "../RanLogic/Msg/ServerManagerMsg.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Util/DateTime.h"

// InspectionRewardPolicy ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(InspectionRewardPolicy, CDialog)

InspectionRewardPolicy::InspectionRewardPolicy(CWnd* pParent /*=NULL*/)
	: CDialog(InspectionRewardPolicy::IDD, pParent)
	, m_nSelectServerGroup(-1)
{
	m_pDlg = reinterpret_cast <CServerManagerDlg*> (pParent);

}

InspectionRewardPolicy::~InspectionRewardPolicy()
{

}

void InspectionRewardPolicy::SetServerInfo( const std::vector<G_SERVER_INFO_DLG*>& vecInfo )
{
    m_vecServerList = vecInfo;
}

BOOL InspectionRewardPolicy::OnInitDialog()
{
    CDialog::OnInitDialog();

//     // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
//     SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
//     ShowWindow(SW_SHOW || SW_RESTORE);

    ServerGroupListInit();

    return TRUE;  // return TRUE unless you set the focus to a control
}

void InspectionRewardPolicy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER_GROUP, m_pServerGroupList);
	DDX_Control(pDX, IDC_EDIT3, Edit_Interval);
	DDX_Control(pDX, IDC_EDIT2, Edit_Reword);
	DDX_Control(pDX, IDC_EDIT4, Edit_TimeView);
	DDX_Control(pDX, IDC_BUTTON1, Btn_Commit);
	DDX_Control(pDX, IDC_EDIT5, Edit_RewordView);
	DDX_Control(pDX, IDC_BUTTON5, Btn_Send);
}

BEGIN_MESSAGE_MAP(InspectionRewardPolicy, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &InspectionRewardPolicy::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &InspectionRewardPolicy::OnBnClickedButton2)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT3, &InspectionRewardPolicy::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT2, &InspectionRewardPolicy::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON3, &InspectionRewardPolicy::OnBnClickedButton3)
	ON_EN_SETFOCUS(IDC_EDIT3, &InspectionRewardPolicy::OnEnSetfocusEdit3)
	ON_EN_SETFOCUS(IDC_EDIT2, &InspectionRewardPolicy::OnEnSetfocusEdit2)
	ON_BN_CLICKED(IDC_BUTTON5, &InspectionRewardPolicy::OnBnClickedButton5)
END_MESSAGE_MAP()

void InspectionRewardPolicy::ServerGroupListInit()
{
    m_pServerGroupList.SetExtendedStyle(LVS_EX_FULLROWSELECT); 

    LVCOLUMN Col;

    Col.mask		= LVCF_TEXT | LVCF_WIDTH;
    Col.cx			= 75;
    Col.fmt			= LVCFMT_LEFT;	
    Col.pszText		= _T("ServerGroup");
    Col.iSubItem	= 0;
    Col.iImage		= 0;
    m_pServerGroupList.InsertColumn(0, &Col);

    Col.mask		= LVCF_TEXT | LVCF_WIDTH;
    Col.cx			= 300;
    Col.fmt			= LVCFMT_LEFT;	
    Col.pszText		= _T("IP");
    Col.iSubItem	= 0;
    Col.iImage		= 0;
    m_pServerGroupList.InsertColumn(1, &Col);


    int nIndex = 0;
    for( unsigned int i=0 ; i < m_vecServerList.size() ; i++)
    {
        if( m_vecServerList[i] == NULL )
            continue;

        if( m_vecServerList[i]->gsi.ServerType != net::SERVER_CACHE )
            continue;

        CString strTemp;
        strTemp.Format( "%d", m_vecServerList[i]->gsi.nServerGroup );

        LV_ITEM lvItem;	
        lvItem.mask		= LVIF_TEXT; // �̹��� �Ӽ� ����
        lvItem.iItem	= nIndex;
        lvItem.iSubItem = 0;	
        lvItem.pszText	= (LPTSTR)strTemp.GetString();

        m_pServerGroupList.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.
        m_pServerGroupList.SetItemText(nIndex, 1, m_vecServerList[i]->gsi.szServerIP);

        ++nIndex;
    }
}


void InspectionRewardPolicy::Reset()
{

	m_tStart	= 0;
	m_tEnd		= 0;
	m_nSelectServerGroup =-1;
	m_nReward = 0;

	Btn_Commit.EnableWindow(FALSE);
	Edit_TimeView.SetWindowText("");
	Edit_RewordView.SetWindowText("");
	Edit_Interval.SetWindowText("");
	Edit_Reword.SetWindowText("");
}


// InspectionRewardPolicy �޽��� ó�����Դϴ�.

void InspectionRewardPolicy::OnDestroy()
{
	CDialog::OnNcDestroy();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	CServerManagerDlg *pDlg;
	pDlg = reinterpret_cast <CServerManagerDlg*> (m_pDlg);
	pDlg->m_pPolicyDlg = NULL;
	delete this;
}

void InspectionRewardPolicy::OnBnClickedButton1()
{
	Btn_Send.EnableWindow(TRUE);
	//Reset();
}

void InspectionRewardPolicy::OnBnClickedButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int nSelectedItem = m_pServerGroupList.GetNextItem( -1, LVNI_SELECTED );
	
	int nServerGroup = _tstoi(m_pServerGroupList.GetItemText(nSelectedItem, 0));
	CString strOut = m_pServerGroupList.GetItemText(nSelectedItem, 1);

	if( strOut.IsEmpty() )
	{
		CString temp;
		temp.Format("must select Server!");
		::MessageBox( GetSafeHwnd(), temp, "Error", MB_OK );
		return;
	}

	CString strInterval;
	Edit_Interval.GetWindowText( strInterval );

	CString strReword;
	Edit_Reword.GetWindowText( strReword );

	
	int nInterval = _tstoi(strInterval);
	int nReword = _tstoi(strReword);

	if ( nInterval <= 0 )
	{
		CString temp;
		temp.Format("IntervalTime [%d].", nInterval);
		::MessageBox( GetSafeHwnd(), temp, "Error", MB_OK );
		return;
	}
	else if ( 1440 <= nInterval )
	{
		CString temp;
		temp.Format("IntervalTime too high", nInterval);
		::MessageBox( GetSafeHwnd(), temp, "Error", MB_OK );
		return;
	}

	if ( nReword <= 0 )
	{
		CString temp;
		temp.Format("RewordTIme [%d].", nReword);
		::MessageBox( GetSafeHwnd(), temp, "Error", MB_OK );
		return;
	}


	CTime tStartTime, tEndTime;
	tStartTime = tEndTime = CTime::GetCurrentTime();

	CTimeSpan tTimeSpan( 0, nInterval/60, nInterval%60, 0);
	tEndTime += tTimeSpan;

	CString strTimeViewText;
	strTimeViewText.Format( "[%d] Server [%02d-%02d-%02d %02d:%02d] ~ [%02d:%02d]"
		, nServerGroup

		, tStartTime.GetYear()%100
		, tStartTime.GetMonth()
		, tStartTime.GetDay()
		, tStartTime.GetHour()
		, tStartTime.GetMinute()

		, tEndTime.GetHour()
		, tEndTime.GetMinute());

	CString strRewordViewText;
	strRewordViewText.Format( "[%3d] Minute", nReword );

	Edit_TimeView.SetWindowText( strTimeViewText );
	Edit_RewordView.SetWindowText( strRewordViewText );
	Btn_Commit.EnableWindow( TRUE );

	m_tStart	= tStartTime.GetTime();
	m_tEnd		= tEndTime.GetTime();
	m_nSelectServerGroup = nServerGroup;
	m_nReward	= nReword;
}

void InspectionRewardPolicy::OnEnChangeEdit3()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void InspectionRewardPolicy::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void InspectionRewardPolicy::OnBnClickedButton3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	Reset();
}

void InspectionRewardPolicy::OnEnSetfocusEdit3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_tStart	= 0;
	m_tEnd		= 0;
	m_nSelectServerGroup =-1;
	m_nReward = 0;

	Btn_Commit.EnableWindow(FALSE);
	Edit_TimeView.SetWindowText("");
	Edit_RewordView.SetWindowText("");
}

void InspectionRewardPolicy::OnEnSetfocusEdit2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_tStart	= 0;
	m_tEnd		= 0;
	m_nSelectServerGroup =-1;
	m_nReward = 0;

	Btn_Commit.EnableWindow(FALSE);
	Edit_TimeView.SetWindowText("");
	Edit_RewordView.SetWindowText("");
}

void InspectionRewardPolicy::OnBnClickedButton5()
{


	OnOK();

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GLMSG::NET_INSPECTION_REWARD_SET_REQ NetMsg(m_nSelectServerGroup, m_tStart, m_tEnd, m_nReward);
	m_pDlg->m_pNetClient->Send( (char*)&NetMsg, NetMsg.Size() );

	sc::writeLogInfo( sc::string::format("Inspection Reward Server Group:[%d] ", NetMsg.nServerGroup ));
	sc::writeLogInfo( sc::string::format("Inspection Reward Start Time:[%s] ", sc::time::DateTimeFormat(NetMsg.tStart, true, true) ));
	sc::writeLogInfo( sc::string::format("Inspection Reward End Time:[%s] ", sc::time::DateTimeFormat(NetMsg.tEnd, true, true) ));
	sc::writeLogInfo( sc::string::format("Inspection Reward Minte:[%d] ", NetMsg.nRewardTime ));

}
