#pragma once


// RightBottomView 뷰입니다.

class RightBottomView : public CEditView
{
	DECLARE_DYNCREATE(RightBottomView)

protected:
	RightBottomView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~RightBottomView();

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
};


