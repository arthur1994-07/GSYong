#pragma once


// RightView ���Դϴ�.

class RightView : public CEditView
{
	DECLARE_DYNCREATE(RightView)

protected:
	RightView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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


