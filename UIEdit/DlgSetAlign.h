#pragma once

#include "Resource.h"
#include "afxwin.h"


class CDlgSetAlign : public CDialog
{
public:
	enum { IDD = IDD_DIALOG_ALIGN };

public:
	CDlgSetAlign(CWnd* pParent = NULL);
	virtual ~CDlgSetAlign();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_DYNAMIC(CDlgSetAlign)
	DECLARE_MESSAGE_MAP()

public:
	int m_nSelectPosX;
	int m_nSelectPosY;
	int m_nSelectSizeX;
	int m_nSelectSizeY;
	BOOL m_nRadioPosX;
	BOOL m_nRadioPosY;
	BOOL m_nRadioSizeX;
	BOOL m_nRadioSizeY;
	BOOL m_nCheckBG;
	BOOL m_nCheckWide;
	BOOL m_nCheckBasicProp;	

public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckBg();
	afx_msg void OnBnClickedCheckWide();
	afx_msg void OnBnClickedCheckBasicProp();
	afx_msg void OnBnClickedRadioLeft();
	afx_msg void OnBnClickedRadioRight();
	afx_msg void OnBnClickedRadioCenterX();
	afx_msg void OnBnClickedRadioPropX();
	afx_msg void OnBnClickedRadioTop();
	afx_msg void OnBnClickedRadioBottom();
	afx_msg void OnBnClickedRadioCenterY();
	afx_msg void OnBnClickedRadioPropY();
	afx_msg void OnBnClickedRadioSizeX();
	afx_msg void OnBnClickedRadioPropSizeX();
	afx_msg void OnBnClickedRadioSizeY();
	afx_msg void OnBnClickedRadioPropSizeY();
};