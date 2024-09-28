// C:\tfs\RanAS\Ran\GMCharEdit\MainTabs\ItemTransfer.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <process.h>
#include "../GMCharEdit.h"
#include "ItemTransfer.h"


#include "../DoCharEdit/PropertySheetCharEdit.h"
#include "../SubDialogs/DlgInvenAdd.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Util/CGmCfg.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../Util/CConsoleMessage.h"
#include "../Util/CLogFile.h"
#include "../../RanLogicServer/DxServerInstance.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


static unsigned int WINAPI ItemTransferThread( void* p )
{
	if ( NULL == p )
	{
		return 0;
	}
	else
	{
		CItemTransfer* pDlg = reinterpret_cast< CItemTransfer* >( p );
		pDlg->ItemTransferProc();
		return 0;
	}
}

// CItemTransfer

IMPLEMENT_DYNCREATE(CItemTransfer, CFormView)

CItemTransfer::CItemTransfer()
	: CFormView(CItemTransfer::IDD)
	, m_hThread( NULL )
	, m_bInit( FALSE )
	, m_bRunning( FALSE )
	, m_nCheck( 0 )
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();
}

CItemTransfer::~CItemTransfer()
{
}

void CItemTransfer::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_ITEMTRANSFER_PROGRESS, m_Progress );
	DDX_Control( pDX, IDC_EDIT_ITEMTRANSFER_CONSOLE, m_EditItemTransferConsole );
	DDX_Check( pDX, IDC_CHECK_CLUBITEM_TRANSFER, m_nCheck );
}

BEGIN_MESSAGE_MAP(CItemTransfer, CFormView)
	ON_BN_CLICKED(IDC_BTN_ITEMTRANSFER_START, OnBnClickedBtnItemTransferStart)
	ON_BN_CLICKED(IDC_BTN_ITEMTRANSFER_STOP, OnBnClickedBtnItemTransferStop)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_EN_MAXTEXT(IDC_EDIT_ITEMTRANSFER_CONSOLE, OnEnMaxtextEditItemTransferConsole)
	ON_BN_CLICKED(IDC_CHECK_CLUBITEM_TRANSFER, &CItemTransfer::OnBnClickedCheckClubitemTransfer)
END_MESSAGE_MAP()


// CItemTransfer �����Դϴ�.

#ifdef _DEBUG
void CItemTransfer::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CItemTransfer::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CGMItemSearch �޽��� ó�����Դϴ�.
void CItemTransfer::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.	
	m_bInit = TRUE;
}

// ������ �˻�����
void CItemTransfer::OnBnClickedBtnItemTransferStart()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	if( pFrame->m_nUserLevel < 40 )
	{
		MessageBox( _T( "Permission Denied" ) );
		return;
	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	if ( m_bRunning == TRUE ) // �۾���
	{
		MessageBox( _T( "Already running, First Stop Process" ) );
		return;
	}
	else // �۾�����
	{
		UpdateData( TRUE );
		DWORD dwThreadID = 119;
		m_hThread = ( HANDLE ) ::_beginthreadex( 
			NULL, 
			0, 
			ItemTransferThread, 
			this, 
			0, 
			( unsigned int* ) &dwThreadID );
		if ( NULL == m_hThread )
		{
			// ������ ������ �����Ͽ����ϴ�.
			MessageBox( _T( "Thread Failed" ) );
		}
		else
		{
			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "Item transfer";
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog( nGmNum, strGmCmd, strUserIP );
			strGmCmd.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			///////////////////////////////////////////////////////////////////////////
		}
	}
}

// ������ �˻�����
void CItemTransfer::OnBnClickedBtnItemTransferStop()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if ( m_bRunning ) 
	{
		m_bRunning = FALSE;
	}
	else
	{
		return;
	}
}

void CItemTransfer::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if ( m_bRunning ) 
	{
		m_bRunning = FALSE;
	}
	else
	{
		return;
	}
	CFormView::OnClose();
}

void CItemTransfer::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_bInit)
	{
		int nWidth  = cx - 5;
		int nHeight = cy - 110;

		if (nWidth < 0)  cx = 0;
		if (nHeight < 0) cy = 0;

		m_EditItemTransferConsole.MoveWindow(0,
			100,
			nWidth,
			nHeight);
	}
}

void CItemTransfer::OnEnMaxtextEditItemTransferConsole()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GetDlgItem( IDC_EDIT_ITEMTRANSFER_CONSOLE )->SetWindowText("");
}

void CItemTransfer::ItemTransferProc()
{
// 	MessageBox( _T( "Not in service." ) );
// 	return;

	m_bRunning = TRUE;

	GetDlgItem( IDC_CHECK_CLUBITEM_TRANSFER )->EnableWindow( FALSE );

	size_t nTotalSize = 0;
	size_t nCurrent   = 0;
	size_t nPercent   = 0;		

	CString strTemp;

	CConsoleMessage::GetInstance()->SetControl( GetDlgItem( IDC_EDIT_ITEMTRANSFER_CONSOLE )->m_hWnd );

	// Set Range Of progress bar
	m_Progress.SetRange( 0, 100 );

	{
		time_t ltime;
		struct tm today;

		// Use time structure to build a customized time string. 
		time( &ltime );
		localtime_s( &today, &ltime );

		// Use strftime to build a customized time string. 
		char szTime[100];
		strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

		CString strLog( szTime );
		strLog += "---- Start ----";

		CConsoleMessage::GetInstance()->Write( strLog.GetString() );
	}

	if ( m_nCheck )
	{
		// Ŭ����Ŀ �����۸� ��ȯ�Ѵ�.
		std::vector< DWORD > vecClubNum;
		m_pDBMan->GetClubNumToTransfer( vecClubNum );
		size_t ClubNumSize = vecClubNum.size();

		{
			time_t ltime;
			struct tm today;

			// Use time structure to build a customized time string. 
			time( &ltime );
			localtime_s( &today, &ltime );

			// Use strftime to build a customized time string. 
			char szTime[100];
			strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

			CString strLog( szTime );
			strLog += "ClubLocker count %d";

			CConsoleMessage::GetInstance()->Write( strLog.GetString(), ClubNumSize );
		}

		nTotalSize = ClubNumSize;

		if ( 0 == nTotalSize )
		{
			{
				time_t ltime;
				struct tm today;

				// Use time structure to build a customized time string. 
				time( &ltime );
				localtime_s( &today, &ltime );

				// Use strftime to build a customized time string. 
				char szTime[100];
				strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

				CString strLog( szTime );
				strLog += "TotalSize is 0";

				CConsoleMessage::GetInstance()->Write( strLog.GetString() );
			}

			m_bRunning = FALSE;
			GetDlgItem( IDC_CHECK_CLUBITEM_TRANSFER )->EnableWindow( TRUE );
			return;
		}

		nPercent = ( nCurrent * 100 ) / nTotalSize;
		m_Progress.SetPos( nPercent );

		{
			time_t ltime;
			struct tm today;

			// Use time structure to build a customized time string. 
			time( &ltime );
			localtime_s( &today, &ltime );

			// Use strftime to build a customized time string. 
			char szTime[100];
			strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

			CString strLog( szTime );
			strLog += "ClubLocker transfer start";

			CConsoleMessage::GetInstance()->Write( strLog.GetString() );
		}

		for ( size_t loop = 0; loop < ClubNumSize; loop++ )
		{
			m_pDBMan->TransferClubLockerItems( vecClubNum[loop] );
			nCurrent++;
			nPercent = ( nCurrent * 100 ) / nTotalSize;
			m_Progress.SetPos( nPercent );

			strTemp.Format( "%d/%d (%d%%)", nCurrent, nTotalSize, nPercent );
			SetDlgItemText( IDC_STATIC_ITEMTRANSFER, strTemp );
		}

		{
			time_t ltime;
			struct tm today;

			// Use time structure to build a customized time string. 
			time( &ltime );
			localtime_s( &today, &ltime );

			// Use strftime to build a customized time string. 
			char szTime[100];
			strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

			CString strLog( szTime );
			strLog += "ClubLocker transfer end";

			CConsoleMessage::GetInstance()->Write( strLog.GetString() );
		}
	}
	else
	{
		// ��ü ��ȯ ��� ���� ��������
		size_t TotalUserNum = 0;
		std::vector< DWORD > vecUserNum;
		m_pDBMan->GetUserNumToTransfer( vecUserNum );
		TotalUserNum = vecUserNum.size();

		if ( 0 == TotalUserNum )
		{
			{
				time_t ltime;
				struct tm today;

				// Use time structure to build a customized time string. 
				time( &ltime );
				localtime_s( &today, &ltime );

				// Use strftime to build a customized time string. 
				char szTime[100];
				strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

				CString strLog( szTime );
				strLog += "TotalSize is 0";

				CConsoleMessage::GetInstance()->Write( strLog.GetString() );
			}

			m_bRunning = FALSE;
			GetDlgItem( IDC_CHECK_CLUBITEM_TRANSFER )->EnableWindow( TRUE );
			return;
		}

		{
			time_t ltime;
			struct tm today;

			// Use time structure to build a customized time string. 
			time( &ltime );
			localtime_s( &today, &ltime );

			// Use strftime to build a customized time string. 
			char szTime[100];
			strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

			CString strLog( szTime );
			strLog += "Total itemTransfer target user %d";

			CConsoleMessage::GetInstance()->Write( strLog.GetString(), TotalUserNum );
		}

		nTotalSize = TotalUserNum;

		nPercent = ( nCurrent * 100 ) / nTotalSize;
		m_Progress.SetPos( nPercent );

		{
			time_t ltime;
			struct tm today;

			// Use time structure to build a customized time string. 
			time( &ltime );
			localtime_s( &today, &ltime );

			// Use strftime to build a customized time string. 
			char szTime[100];
			strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

			CString strLog( szTime );
			strLog += "transfer start";

			CConsoleMessage::GetInstance()->Write( strLog.GetString() );
		}

		for ( size_t loop = 0; loop < TotalUserNum; loop++ )
		{
			if ( FALSE == m_bRunning )
			{
				{
					time_t ltime;
					struct tm today;

					// Use time structure to build a customized time string. 
					time( &ltime );
					localtime_s( &today, &ltime );

					// Use strftime to build a customized time string. 
					char szTime[100];
					strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

					CString strLog( szTime );
					strLog += "transfer canceled";

					CConsoleMessage::GetInstance()->Write( strLog.GetString() );
				}
				break;
			}

			// ���Ӽ������� �������� ���ϵ��� ���°� �����Ѵ�.
			int Result = m_pDBMan->PrepareItemTransfer( vecUserNum[loop] );
			if ( 0 != vecUserNum[loop] && 0 == Result )
			{
				m_pDBMan->TransferLockerItems( vecUserNum[loop] );

				std::vector< DWORD > vecChaNum;
				m_pDBMan->GetCharacterItemTransferTarget( vecUserNum[loop], vecChaNum );
				size_t ChaNumSize = vecChaNum.size();
				for ( size_t loop2 = 0; loop2 < ChaNumSize; loop2++ )
				{
					if ( 0 == vecChaNum[loop2] )
					{
						continue;
					}

					m_pDBMan->TransferCharacterItems( vecChaNum[loop2] );
				}

				m_pDBMan->CompleteItemTransfer( vecUserNum[loop] );
			}

			nCurrent++;
			nPercent = ( nCurrent * 100 ) / nTotalSize;
			m_Progress.SetPos( nPercent );

			strTemp.Format( "%d/%d (%d%%)", nCurrent, nTotalSize, nPercent );
			SetDlgItemText( IDC_STATIC_ITEMTRANSFER, strTemp );
		}

		{
			time_t ltime;
			struct tm today;

			// Use time structure to build a customized time string. 
			time( &ltime );
			localtime_s( &today, &ltime );

			// Use strftime to build a customized time string. 
			char szTime[100];
			strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

			CString strLog( szTime );
			strLog += "transfer end";

			CConsoleMessage::GetInstance()->Write( strLog.GetString() );
		}

		/*
		std::vector< DWORD > vecUserNum;
		m_pDBMan->GetUserNumToTransfer( vecUserNum );
		size_t UserNumSize = vecUserNum.size();
		CConsoleMessage::GetInstance()->Write( "UserLocker count %d", UserNumSize );

		std::vector< DWORD > vecChaNum;
		m_pDBMan->GetChaNumToTransfer( vecChaNum );
		size_t ChaNumSize = vecChaNum.size();
		CConsoleMessage::GetInstance()->Write( "Character count %d", ChaNumSize );

		nTotalSize = UserNumSize + ChaNumSize;
		if ( 0 == nTotalSize )
		{
			CConsoleMessage::GetInstance()->Write( "TotalSize is 0" );
			m_bRunning = FALSE;
			return;
		}

		nPercent = ( nCurrent * 100 ) / nTotalSize;
		m_Progress.SetPos( nPercent );

		CConsoleMessage::GetInstance()->Write( "UserLocker transfer start" );
		for ( size_t loop = 0; loop < UserNumSize; loop++ )
		{
			m_pDBMan->TransferLockerItems( vecUserNum[loop] );
			nCurrent++;
			nPercent = ( nCurrent * 100 ) / nTotalSize;
			m_Progress.SetPos( nPercent );

			strTemp.Format( "%d/%d (%d%%)", nCurrent, nTotalSize, nPercent );
			SetDlgItemText( IDC_STATIC_ITEMTRANSFER, strTemp );
		}
		CConsoleMessage::GetInstance()->Write( "UserLocker transfer end" );

		CConsoleMessage::GetInstance()->Write( "Character transfer start" );
		for ( size_t loop = 0; loop < ChaNumSize; loop++ )
		{
			m_pDBMan->TransferCharacterItems( vecChaNum[loop] );
			nCurrent++;
			nPercent = ( nCurrent * 100 ) / nTotalSize;
			m_Progress.SetPos( nPercent );

			strTemp.Format( "%d/%d (%d%%)", nCurrent, nTotalSize, nPercent );
			SetDlgItemText( IDC_STATIC_ITEMTRANSFER, strTemp );
		}
		CConsoleMessage::GetInstance()->Write( "Character transfer end" );
		*/
	}

	{
		time_t ltime;
		struct tm today;

		// Use time structure to build a customized time string. 
		time( &ltime );
		localtime_s( &today, &ltime );

		// Use strftime to build a customized time string. 
		char szTime[100];
		strftime(szTime, 100, "[%Y-%m-%d %H:%M:%S] ", &today);

		CString strLog( szTime );
		strLog += "---- End ----";

		CConsoleMessage::GetInstance()->Write( strLog.GetString() );
	}

	m_bRunning = FALSE;
	GetDlgItem( IDC_CHECK_CLUBITEM_TRANSFER )->EnableWindow( TRUE );
}

void CItemTransfer::OnBnClickedCheckClubitemTransfer()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if ( ( ( CButton* )GetDlgItem( IDC_CHECK_CLUBITEM_TRANSFER ) )->GetCheck() )
	{
		m_nCheck = 1;
	}
	else
	{
		m_nCheck = 0;
	}
}
