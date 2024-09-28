#pragma once

struct	SExportParam
{
	int m_nSumulationDetailLength;
	int m_nOctreeBoxLength;
	BOOL m_bFieldOcclusion;
	BOOL m_bPieceOcclusion;
	BOOL m_bCharOcclusion;
	//int m_nOctreeBoxLength_CHAR;

	SExportParam()
		: m_nSumulationDetailLength(80)
		, m_nOctreeBoxLength(80)
		, m_bFieldOcclusion(FALSE)
		, m_bPieceOcclusion(FALSE)
		, m_bCharOcclusion(FALSE)
	//	, m_nOctreeBoxLength_CHAR(160)
	{
	}
};

// CExportMiniDlg 대화 상자입니다.

class CExportMiniDlg : public CDialog
{
	DECLARE_DYNAMIC(CExportMiniDlg)

public:
	SExportParam	m_SParam;
	CTime m_sStartTime;
	int m_nCount;
	std::stringstream m_strStringStreamET;
	std::stringstream m_strStringStreamRT;

public:
	CExportMiniDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CExportMiniDlg();

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
