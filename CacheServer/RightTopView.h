#pragma once


// RightTopView ���Դϴ�.

class RightTopView : public CEditView
{
	DECLARE_DYNCREATE(RightTopView)

protected:
	RightTopView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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


