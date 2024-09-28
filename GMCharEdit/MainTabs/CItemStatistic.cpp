// CItemStatistic.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include <process.h>
#include "../../RanLogic/GLogicDataMan.h"
#include "../MainFrm.h"
#include "../GMCharEdit.h"
#include "CItemStatistic.h"

#include "../Util/CGmCfg.h"

#include "../../RanLogicServer/DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CItemStatistic

static unsigned int WINAPI ItemStatsticThread( void* p )
{
	if ( p == NULL )
	{
		return 0;
	}
	else
	{
		CItemStatistic* pDlg = reinterpret_cast< CItemStatistic* >( p );
		pDlg->StatisticProc();
		return 0;
	}
}

IMPLEMENT_DYNCREATE(CItemStatistic, CFormView)

CItemStatistic::CItemStatistic()
	: CFormView(CItemStatistic::IDD)
	, m_hThread(NULL)
	, m_bRunning(FALSE)
	, m_bInit(FALSE)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();
}

CItemStatistic::~CItemStatistic()
{
	if ( m_bRunning == TRUE )
	{
		m_bRunning = FALSE; // �ߴܽ�Ű��
		Sleep( 1000 ); // 1 �ʴ��
	}	
}

void CItemStatistic::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_PROGRESS_STAT, m_StaticProgress);
	DDX_Control(pDX, IDC_EDIT_ITEM_STATISTIC_CONSOLE, m_EditItemStatConsole);	
}

BEGIN_MESSAGE_MAP(CItemStatistic, CFormView)
	ON_EN_MAXTEXT(IDC_EDIT_ITEM_STATISTIC_CONSOLE, OnEnMaxtextEditItemChangeConsole)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_STAT_START, OnBnClickedBtnStatStart)
	ON_BN_CLICKED(IDC_BTN_STAT_STOP, OnBnClickedBtnStatStop)
END_MESSAGE_MAP()

// CItemStatistic �����Դϴ�.

#ifdef _DEBUG
void CItemStatistic::AssertValid() const
{
	CFormView::AssertValid();
}

void CItemStatistic::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


void CItemStatistic::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_bInit = TRUE;

}

void CItemStatistic::OnEnMaxtextEditItemChangeConsole()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GetDlgItem(IDC_EDIT_ITEM_STATISTIC_CONSOLE)->SetWindowText("");
}

void CItemStatistic::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_bInit)
	{
		int nWidth  = cx - 5;
		int nHeight = cy - 110;

		if (nWidth < 0)  cx = 0;
		if (nHeight < 0) cy = 0;
		
		m_EditItemStatConsole.MoveWindow(0,
		                                 100,
							             nWidth,
							             nHeight);
	}
}

// CItemStatistic �޽��� ó�����Դϴ�.
void CItemStatistic::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if ( m_bRunning == TRUE )
	{
		m_bRunning = FALSE;
		Sleep( 1000 );
	}
	else
	{
		CFormView::OnClose();
	}
}

// ������
void CItemStatistic::OnBnClickedBtnStatStart()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	if( pFrame->m_nUserLevel < 40 )
	{
		MessageBox( _T( "Permission Denied" ) );
		return;
	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bRunning == TRUE) // �۾���
	{
		MessageBox(_T("Already running, First Stop Process"));
		return;
	}
	else // �۾�����
	{
		UpdateData(TRUE);
		DWORD dwThreadID = 109;

		m_hThread = (HANDLE) ::_beginthreadex(
									NULL,
							 		0,
							 		ItemStatsticThread,
									this,
									0,
									(unsigned int*) &dwThreadID );
		if (m_hThread == NULL)
		{
			// ������ ������ �����Ͽ����ϴ�.
			MessageBox(_T("Thread Failed"));
		}
		else
		{
			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "Item Statistic";
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
			strGmCmd.freeze( false );
			///////////////////////////////////////////////////////////////////////////
		}
	}
}

// �������
void CItemStatistic::OnBnClickedBtnStatStop()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bRunning) 
	{
		m_bRunning = FALSE;
	}
	else
	{
		return;
	}
}

void CItemStatistic::StatisticProc()
{
	int nMaxSize   = 0;
	int nTotalSize = 0;
	int nCurrent   = 0;
	int	nFindCount = 0;
	int nPercent   = 0;
	
	m_bRunning = TRUE;

	m_StaticProgress.SetRange(0, 100);
	
	CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_ITEM_STATISTIC_CONSOLE)->m_hWnd);

	std::vector< ITEM_STATISTIC > result;
	if ( DB_ERROR == m_pDBMan->ItemStatistic( result ) )
	{
		return;
	}

	CLogFile cFile( "ItemStat" );
	cFile.Write( "mid,sid,name,count" );
	int ItemSize = static_cast< int >( result.size() );
	nTotalSize = ItemSize;
	for ( int loop = 0; loop < ItemSize; loop++ )
	{
		ITEM_STATISTIC& Item = result[loop];

		nCurrent++;
		nPercent = ( int )( nCurrent * 100 ) / nTotalSize;
		m_StaticProgress.SetPos(nPercent);

		CString strTemp;
		strTemp.Format("%d/%d (%d%%)", nCurrent, nTotalSize, nPercent);
		SetDlgItemText(IDC_STATIC, strTemp);

		CConsoleMessage::GetInstance()->Write( "mid: %d sid: %d count: %d", Item.ItemMid, Item.ItemSid, Item.Count );

		SITEM* pItemData = GLogicData::GetInstance().GetItem( static_cast< WORD >( Item.ItemMid ), static_cast< WORD >( Item.ItemSid ) );
		if ( pItemData != NULL )
		{				
			cFile.Write("%d,%d,\"%s\",%d", Item.ItemMid, Item.ItemSid, pItemData->GetName(), Item.Count );
		}
	}
	
	// ���Ϸ�����
	CConsoleMessage::GetInstance()->Write( "---- Statistic end ----" );
	CConsoleMessage::GetInstance()->Write( "---- Write to file ----" );
	CConsoleMessage::GetInstance()->Write( "---- Write to file end ----" );
}
