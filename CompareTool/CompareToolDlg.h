// CompareToolDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// 정렬 기준
class Align{
public:
	// 정렬 조건을 정의한다.
	bool operator() ( const DWORD& a, const DWORD& b ) const 
	{
		SNATIVEID sA = a;
		SNATIVEID sB = b;
		if( sA.wMainID != sB.wMainID )
			return  sA.wMainID < sB.wMainID ? true : false;
		else
			return  sA.wSubID < sB.wSubID ? true : false;
	}
};


struct SDefferData
{
	CString	strDifferHead;
	CString	strDifferData_A;
	CString	strDifferData_B;
};

typedef std::map<DWORD,SDefferData,Align>			 MAP_DEFFER;
typedef std::map<DWORD,SDefferData,Align>::iterator MAP_DEFFER_ITER;

class EngineDeviceMan;
class SUBPATH;

// CCompareToolDlg 대화 상자
class CCompareToolDlg : public CDialog
{
// 생성
public:
	CCompareToolDlg(CWnd* pParent = NULL);	// 표준 생성자
    virtual ~CCompareToolDlg();

// 대화 상자 데이터
	enum { IDD = IDD_COMPARETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원
	void LoadCompareFile( bool bLoadA );

// 구현
protected:
	HICON m_hIcon;
    SUBPATH* m_pPath;

	char			m_szAppPath[MAX_PATH];
	CString			m_strAppPath;

    std::string			m_strCompareFileA;
	std::string			m_strCompareFileB;
    std::set<CString>	m_setCompareTargetStr;

	MAP_DEFFER		m_mapDefferData;

    std::set<DWORD, Align>		m_setCompareID;
    EngineDeviceMan* m_pEngineDevice;

	void AddDefferData( SNATIVEID sNativedID, CString strHead, CString strData_A, CString strData_B );
	void UpdateCompareList();

	// 데이터 비교
	void CompareData();


	void DrawCursor(LPDRAWITEMSTRUCT lpDS, char parm_push_flag);
	void DrawItem(LPDRAWITEMSTRUCT lpDS, char parm_push_flag);

	// 메시지 맵 함수를 생성했습니다.
    virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoadA();
	afx_msg void OnBnClickedButtonLoadB();
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonCsvsave();
	afx_msg void OnBnClickedButtonCompare();
    afx_msg void OnBnClickedButtonCompareList();
    afx_msg void OnDestroy ();
	CListCtrl m_list_CompareResult;
	afx_msg void OnNMCustomdrawListCompareResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnBnClickedButtonCsvsave2();
	afx_msg void OnBnClickedRadioPart();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedButtonCompareTarget();
	afx_msg void OnBnClickedRadioEditSkill();
	afx_msg void OnBnClickedRadioEditMobnpc();
	afx_msg void OnBnClickedRadioEditItem();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
