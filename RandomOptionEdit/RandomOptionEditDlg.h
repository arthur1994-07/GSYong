
// RandomOptionEditDlg.h : 헤더 파일
//

#pragma once

#include "./RandomOptionConvertDlg.h"

// CRandomOptionEditDlg 대화 상자
class CRandomOptionEditDlg : public CDialog
{
// 생성입니다.
public:
	CRandomOptionEditDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_RANDOMOPTIONEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CListBox	m_RandomOptionListBox;
	CListBox	m_OptionValueListBox;
	CListCtrl	m_RandomOptionData;
	CEdit		m_SerchEditBox;
	CEdit		m_NameEditBox;

	CEdit		m_TypeEditBox;
	CEdit		m_RateEditBox;
	CEdit		m_LowEditBox;
	CEdit		m_HighEditBox;

	CRandomOptionConvertDlg m_ConverDlg;
	void UpdateRandomOptionList();
	void UpdateRandomOptionData();

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();	
	virtual void OnOK(void) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnEdChangeValue(void);
	afx_msg void OnPaint(void);
	afx_msg HCURSOR OnQueryDragIcon(void);

	afx_msg void OnBnClickedBtnLoad(void);
	afx_msg void OnBnClickedBtnSave(void);
	afx_msg void OnBnClickedBtnCsvSave(void);
	afx_msg void OnBnClickedBtnCsvLoad(void);
	afx_msg void OnBnClickedBtnOldLoad(void);
	afx_msg void OnBnClickedBtnAddEntity(void);
	afx_msg void OnBnClickedBtnRemoveEntity(void);
	afx_msg void OnBnClickedBtnApplyEntity(void);
	afx_msg void OnBnClickedBtnRemoveValue(void);
	afx_msg void OnBnClickedBtnAddValue(void);
	afx_msg void OnBnSelchangeRandomOption(void);
	afx_msg void OnBnSelchangeEntity(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChangeValue();
	DECLARE_MESSAGE_MAP()
};
