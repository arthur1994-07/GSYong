// AutoPatchManDlg.h : ��� ����
//
#pragma once

#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

//#include "../RanLogic/s_NetGlobal.h"
#include "../RanLogicClient/Network/s_NetClient.h"
#include "./explorer1.h"
#include "./GlobalVariable.h"

#include "TransparentHelper.h"
#include "BitComboBox.h"
#include "StaticEx.h"
#include "BtnST.h"
#include "SkinProgress.h"
#include "TransparentListBox.h"
#include "HyperLink.h"

class CLPatchThread;
class CHttpPatch;
class CPatch;

class CAutoPatchManDlg : public CDialog
{
// ����
public:
	CAutoPatchManDlg(CWnd* pParent = NULL);	// ǥ�� ������
// ��ȭ ���� ������	
	enum { IDD = IDD_AUTOPATCHMAN_DIALOG };


private:
	

	enum EM_VERSION_STATE
	{
		E_VERSION_CUR	= 1,
		E_VERSION_UP	= 2,
	};

	enum //	Ÿ�̸� ID
	{
		E_LOGIN_CONNECT = 1,
		E_CHECK_VERSION = 2,
		E_FTP_CONNECT	= 3,
		E_START_PATCH	= 4,
		E_FORCE_EXIT	= 5,
		E_WEB_BROWSER	= 6,
		E_CHECK_HASH	= 7
	};

	enum
	{			
		E_MSG_FAILED	=-2,	//	����
		E_MSG_SUCCEED	= 1,	//	����
		E_CHK_VER_NOT_READY = -1,		//	���� ������ ���� �ȵ�
		E_CHK_VER_SERVER_UPDATE = -100,	//	���� ������Ʈ��
        E_CHK_VER_NEED_REINSTALL = -200,//	�缳ġ �ʿ� ( ���� ���̰� �ʹ�ŭ )
	};

	enum //	��ó ��ġ ���� �޽���
	{
		E_MSG_DOWNLOAD_FAILED = E_MSG_FAILED - 1,	//	�ٿ�ε� ����
		E_MSG_EXTRACT_FAILED  = E_MSG_FAILED - 2,	//	��ġ ����
	};

	enum // Thread ����
	{
		E_THREAD_WEB = 1,
		E_THREAD_LOGIN = 2,
		E_THREAD_PATCH = 3,
		E_THREAD_HASH = 4
	};

private:
	static const UINT n500_MILISEC;
	static const UINT n1000_MILISEC;
	static const UINT nLOGIN_CONNECT_TIME_ELAPSE;		//	�α��� Ÿ�̸�
	static const UINT nCHECK_VERSION_TIME_ELAPSE;		//	������� Ÿ�̸�
	static const UINT nFTP_CONNECT_TIME_ELAPSE;			//	FTP Ŀ��Ʈ Ÿ�̸�
	static const UINT nFORCE_TERMINATE_TIME_ELAPSE;		//	�������� Ÿ�̸�
	static const UINT nWEB_BROWSER_START;				//	�� ������ ���� �ð�

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

private:
	//	NOTE
	//		���� ��� �����̱� ����
	BOOL	m_bLBDown;
	CPoint	m_WindowPos;
	CPoint	m_MouseClickPos;

private:
	//	NOTE
	//		
	CNetClient* m_pNetClient;

private:
	//int		m_nFtpTry;			//	FTP �õ�
	int		m_nLoginTry;		//	�α��� �õ�
	BOOL	m_bForceTerminate;	//	���� ���� �õ�

private:
	int	m_cPatchVer;
	int	m_sPatchVer;
	int	m_cGameVer;
	int m_sGameVer;

	//	NOTE
	//		��ġ ( HTTP, FTP )
private:	
	CHttpPatch*	m_pHttpPatch;
	//CPatch*		m_pFtpPatch;
	bool		m_bUseHttp;

	CString m_strNewsURL;
	CString m_strImageURL;

//private:
	//BOOL	ConnectFtpServer ( CString strFtpAddress );
	//BOOL	DisconnectFtpServer ();

private:
	void	InitDlgCtrl ();
	void	SetAppPath ();
	void	InitDlgText();

private:
	void	UpdateControls();

private:
	void	UpdateLoginConnect ();
	void	UpdateCheckVersion ();
	//void	UpdateFtpConnect ();

private:
	void	BEGIN_LOGIN_THREAD ();
	//void	BEGIN_FTP_THREAD ();	

private:
	EM_VERSION_STATE	m_emPatchState;
	EM_VERSION_STATE	m_emGameState;

	int		CheckVersion ();

private:
	void	BEGIN_PATCH ();
	void	END_PATCH ();

	void	END_LAUNCHER_PATCH ();
	int		BEGIN_LAUNCHER_PATCH ();

private:
	BOOL CREATEPROCESS_PATCHPRIME(void);
	BOOL GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir = NS_GLOBAL_VAR::strDownloadTemp );
	void FILEOUT( const CString& strLogFile );

private:
	//	NOTE
	//		milisecond�� �����Ѵ�.
	void SET_TIME ( const UINT nTIME )					{ m_nLIFE_TIME = nTIME; }
	const UINT GET_TIME () const						{ return m_nLIFE_TIME; }
	void UPDATE_TIME ( const UINT nElapsedTime );

public:
	void ListAddString( const CString & strLog );
	void ListAddString( UINT nIDS );
	void ListAddString( const PCHAR szMessage );

protected: // ������ ����
	CLPatchThread * m_pWebBrowserThread;
	CLPatchThread * m_pLoginThread;
	CLPatchThread * m_pPatchThread;
	CLPatchThread * m_pHashThread;

protected: // ������ ���� �Լ�
	typedef CLPatchThread* LPCLPATCHTHREAD;

	void BeginThread(INT nThreadType, LPCLPATCHTHREAD & pThread, LPVOID pParam, int LoginPort, const CString & strParam = "");
	void EndThread( LPCLPATCHTHREAD & pThread );
	void ForceKillThread( LPCLPATCHTHREAD & pThread );

private:
	UINT	m_nLIFE_TIME;

// ����
protected:
	HICON m_hIcon;
	
	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

	CSkinProgress*	m_ctrlProgressCur;
	CSkinProgress*	m_ctrlProgressAll;

	CStaticEx m_ctrlDownLoad;
	CStaticEx m_ctrlDownLoad2;


	CExplorer1  m_ctrlTitleImage;
	CExplorer1	m_ctrlNotify;

	CButtonST m_ctrlOption;
	CButtonST m_ctrlStart;
	CButtonST m_ctrlUpdate;
	CButtonST m_ctrlRetry;
	CButtonST m_ctrlExit;
	CButtonST m_ctrlCheckBox;

	CButtonST m_ctrlItem;
	CButtonST m_ctrlManualPatch;

	CTransparentListBox m_ctrlListBox;


	CHyperLink m_ctrlHomePage;

	CBitComboBox	m_comboNation;
	CStaticEx		m_stNATION;

public:
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedButtonStart();

    void GameStart();
    void GameStartIndenesia();

	void GetProcessByFileName( CString strFilename, std::vector< HANDLE >& Handles );
	BOOL SafeTerminateProcess( HANDLE hProcess, UINT uExitCode );

    afx_msg void OnBnClickedButtonOption();
	afx_msg void OnBnClickedButtonRetry();
	afx_msg void OnCbnSelchangeCombo();
	afx_msg LRESULT OnCustomControlSiteMsg(WPARAM wParam, LPARAM lParam);

protected:
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	afx_msg void OnBnClickedButtonPatch();
	afx_msg void OnLbnDblclkListLog();

public:
	CDC* m_pMemDC;
	CBitmap* m_pOldBitmap;
	CBitmap* m_pBitmapMem;

	LRESULT OnTransaprentBk( WPARAM wParam, LPARAM lParam);
	void BuildBkDC();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonItem();
	afx_msg void OnBnClickedButtonEvent();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnCnbSelChangeNationCombo();
};