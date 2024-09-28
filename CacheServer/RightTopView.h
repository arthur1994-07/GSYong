#pragma once


// RightTopView 뷰입니다.

class RightTopView : public CEditView
{
	DECLARE_DYNCREATE(RightTopView)

protected:
	RightTopView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~RightTopView();

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
    void ClearMessage();
    afx_msg void OnEnMaxtext();
};


