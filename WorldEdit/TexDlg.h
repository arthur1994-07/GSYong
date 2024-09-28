#pragma once

#include <string>
#include "ClrButton.h"

// CTexDlg 대화 상자입니다.
enum EMTEXEFFMODE
{
	EMTESPECULAR = 0,
	EMTEDIFFUSE = 1,
};

class CTexDlg : public CDialog
{
	DECLARE_DYNAMIC(CTexDlg)

public:
	EMTEXEFFMODE	m_emMode;
	
public:
	float		m_fAlpha;
	BOOL		m_bSpeed;
	float		m_fSpeed;
	std::string m_strBaseTex;
	std::string m_strAlphaTex;
	std::string m_strFlowTex;
	BOOL		m_bNightLight;

public:
	CTexDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTexDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TEX_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSrctex();
	afx_msg void OnBnClickedButtonFlowtex();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckSpeed();
	afx_msg void OnBnClickedCheckNightlight();
};
