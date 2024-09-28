#pragma once

#include <string>
#include "ClrButton.h"

// CTexMultiDlg ��ȭ �����Դϴ�.

class CTexMultiDlg : public CDialog
{
	DECLARE_DYNAMIC(CTexMultiDlg)
	
public:
	BOOL		m_bNEWUV;
	BOOL		m_bRandomSpeed;
	BOOL		m_bModulate2X;
	float		m_fAlpha;
	float		m_fSpeedX;
	float		m_fSpeedY;
	float		m_fUVScale;
	std::string m_strBaseTex;
	std::string m_strAlphaTex;
	std::string m_strFlowTex;

public:
	CTexMultiDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTexMultiDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TEX_MULTI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSrctex();
	afx_msg void OnBnClickedButtonFlowtex();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckNewuv();
	afx_msg void OnBnClickedCheckRandomSpeed();
	afx_msg void OnBnClickedCheckModulate2x();
};
