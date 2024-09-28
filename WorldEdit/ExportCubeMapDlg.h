#pragma once

struct	SExportParam
{
	int m_nSumulationDetailLength;
	int m_nOctreeBoxLength;
	DWORD m_dwCubeMapSize;

	SExportParam()
		: m_nSumulationDetailLength(80)
		, m_nOctreeBoxLength(80)
		, m_dwCubeMapSize(32)
	{
	}
};

// CExportCubeMapDlg 대화 상자입니다.

class CExportCubeMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CExportCubeMapDlg)

public:
	SExportParam	m_SParam;
	CTime m_sStartTime;
	int m_nCount;
	std::stringstream m_strStringStreamET;
	std::stringstream m_strStringStreamRT;

public:
	CExportCubeMapDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CExportCubeMapDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EXPORT_MINI };
	CButton	m_ctrlButtonStart;
	CProgressCtrl	m_ctrlProgress;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};
