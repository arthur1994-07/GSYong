#pragma once


// RightView 뷰입니다.

class RightView : public CEditView
{
	DECLARE_DYNCREATE(RightView)

protected:
	RightView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~RightView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEnMaxtext();
    virtual void OnInitialUpdate();
};


