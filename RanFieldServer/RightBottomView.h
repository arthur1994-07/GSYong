#pragma once


// RightBottomView ���Դϴ�.

class RightBottomView : public CEditView
{
	DECLARE_DYNCREATE(RightBottomView)

protected:
	RightBottomView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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


