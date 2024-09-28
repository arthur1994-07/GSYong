#pragma once

#include "resource.h"
#include <string>

// LeftView 폼 뷰입니다.

class LeftView : public CFormView
{
	DECLARE_DYNCREATE(LeftView)

protected:
	LeftView();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnBnClickedButtonClearLog();
    afx_msg void OnBnClickedButtonLogItemExchange();
	afx_msg void OnBnClickedButtonAdoTest1();
    afx_msg void OnBnClickedButtonAdoTest2();
};


