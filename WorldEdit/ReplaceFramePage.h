#pragma once
#include "afxwin.h"

class CsheetWithTab;
struct DxFrame;

#include "../MfcExLib/ExLibs/Separator.h"

// CReplaceFramePage 대화 상자입니다.

class CReplaceFramePage : public CPropertyPage
{
	enum EXOPTIONS
	{
		EMOPT_NONE	= 0,
		EMOPT_AND	= 1,
		EMOPT_NOT	= 2,
	};

	DECLARE_DYNAMIC(CReplaceFramePage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	void FindFrame ( DxFrame *pframe, CString strFrameStr, CString strFrameStrEx, EXOPTIONS emOption );

public:
	CReplaceFramePage();
	virtual ~CReplaceFramePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_REPLACEFRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CSeparator m_stReplaceLine;
	CSeparator m_stFileLine;
	CListBox m_FrameList;

	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonFramesearch();
	afx_msg void OnLbnSelchangeListFrames();
	afx_msg void OnBnClickedButtonFrameDel();
	afx_msg void OnBnClickedButtonSelectfile();
	afx_msg void OnBnClickedButtonReplaceProcessing();
	afx_msg void OnBnClickedCheckRange();
	virtual BOOL OnInitDialog();
	CComboBox m_comboExSearch;
	afx_msg void OnBnClickedButtonClear();
};
