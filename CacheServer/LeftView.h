#pragma once

#include "resource.h"
#include <string>

// LeftView �� ���Դϴ�.

class LeftView : public CFormView
{
	DECLARE_DYNCREATE(LeftView)

protected:
	LeftView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~LeftView();

public:
	enum { IDD = IDD_LEFTVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
    void SetStartButtonText(const std::string& Text);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnBnClickedButtonClearLog();
    afx_msg void OnBnClickedButtonLogItemExchange();
	afx_msg void OnBnClickedButtonAdoTest1();
    afx_msg void OnBnClickedButtonAdoTest2();
};


