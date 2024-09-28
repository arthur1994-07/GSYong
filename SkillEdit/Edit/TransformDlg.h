#pragma once

#include "../../RanLogic/Skill/GLSkill.h"


// CTransformDlg 대화 상자입니다.

class CTransformDlg : public CDialog
{
	DECLARE_DYNAMIC(CTransformDlg)


public:
	CTransformDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTransformDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TRANSFORM };
	
	SKILL::SSPECIAL_SKILL m_sSPECIAL_SKILL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonMOpen1();
	afx_msg void OnBnClickedButtonMOpen2();
	afx_msg void OnBnClickedButtonMOpen3();
	afx_msg void OnBnClickedButtonMOpen4();
	afx_msg void OnBnClickedButtonMOpen5();
	afx_msg void OnBnClickedButtonMOpen6();
	afx_msg void OnBnClickedButtonMOpen7();
	afx_msg void OnBnClickedButtonMOpen8();
	afx_msg void OnBnClickedButtonWOpen1();
	afx_msg void OnBnClickedButtonWOpen2();
	afx_msg void OnBnClickedButtonWOpen3();
	afx_msg void OnBnClickedButtonWOpen4();
	afx_msg void OnBnClickedButtonWOpen5();
	afx_msg void OnBnClickedButtonWOpen6();
	afx_msg void OnBnClickedButtonWOpen7();
	afx_msg void OnBnClickedButtonWOpen8();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSelfzoneeffect4();
};
