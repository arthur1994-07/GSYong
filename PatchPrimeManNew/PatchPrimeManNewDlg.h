
// PatchPrimeManNewDlg.h : header file
//

#pragma once

#include <vector>
#include "./Define.h"

class PatchThread;
class SUBPATH;
class CHttpPatch;

struct DOWNLOAD_INFO
{
	std::string strFileName;
	std::string strMD5;
	std::string strPath;
	bool NeedVersionUpdate;

	DOWNLOAD_INFO()
		: NeedVersionUpdate( false )
	{
	}
};

// CPatchPrimeManNewDlg dialog
class CPatchPrimeManNewDlg : public CDialog
{
// Construction
public:
	CPatchPrimeManNewDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPatchPrimeManNewDlg();

// Dialog Data
	enum { IDD = IDD_PATCHPRIMEMANNEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


public:
	enum
	{
		E_PATCH_LIST	= 3,
		E_START_PATCH	= 4,
		E_FORCE_EXIT	= 5,
	};

	BOOL LoadCountryFile();
	BOOL LoadPatchListFile();
	BOOL LoadParamFile();
	BOOL VersionUp();

	BOOL StartPatch();
	BOOL EndPatch( bool bError = false );

	void EndPatchThread();

	void ListAddString( const CString& strLog );
	void ListAddString( UINT nIDS );

	BOOL FileListUpdate();
	BOOL LauncherFileListUpdate();

protected:
	void UpdateProgressControls();
	void AddDownloadFile( const std::string& strFileName, const std::string& strMD5, const std::string& strPath, bool NeedVersionUpdate );
	void SetInternetOption();
	BOOL CreateLauncherProcess();
	void ForceDownloadFile( const std::string& strFileName );
	void DeleteNeedlessFile();
	HANDLE GetProcessList( LPCTSTR szFilename );
	BOOL SafeTerminateProcess( HANDLE hProcess, UINT uExitCode );
	void KillLauncher();
	BOOL LoadServerFileList();
	BOOL LoadClientFileList();
	BOOL LoadClientLauncherPatchList();

// Implementation
protected:
	HICON m_hIcon;

	CProgressCtrl m_DownloadProgress;
	CListBox m_ListBox;
	CStatic m_stcInfo;
	PatchThread* m_pPatch;
	SUBPATH* m_pPath;
	int m_GameVersion;
	int m_PatchVersion;
	std::vector< DOWNLOAD_INFO > m_vecPatchFile;
	std::vector< std::string > m_vecDeleteFile;
	CHttpPatch*	m_pHttpPatch;

	CPatchSet	m_ServerFileTree;
	CPatchSet	m_ClientFileTree;
	CPatchSet	m_ClientLauncherPatchTree;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
