#pragma once
#include "TransparentHelper.h"

// CStaticEx

class CStaticEx : public CStatic
{
	DECLARE_DYNAMIC(CStaticEx)

public:
	CStaticEx();
	virtual ~CStaticEx();

protected:
	DECLARE_MESSAGE_MAP()

	CTransparentHelper m_objHelper;
	virtual void PreSubclassWindow();

public:
	DWORD m_Textcolor;

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void SetColor( COLORREF color ) { m_Textcolor = color; }
	void SetFontSize ( int nSize, bool bBold );
};


