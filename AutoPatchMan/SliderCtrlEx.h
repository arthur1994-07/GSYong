#pragma once

#include "TransparentHelper.h"
// CSliderCtrlEx

class CSliderCtrlEx : public CSliderCtrl
{
	DECLARE_DYNAMIC(CSliderCtrlEx)

public:
	CSliderCtrlEx();
	virtual ~CSliderCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
	BOOL OnSliderDrawThumb( CDC* pDC,  CRect& rect, UINT nState);
	BOOL OnSliderDrawChannel( CDC* pDC,  CRect& rect, UINT nState);
	BOOL OnSliderDrawTics( CDC* pDC,  CRect& rect, UINT nState){ return TRUE;};

private:
	CTransparentHelper m_objTrans;
	CBitmap m_bmpDrag;
	CBitmap m_bmpBg;
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void PreSubclassWindow();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	LRESULT OnNotifyTransparent( WPARAM wParam, LPARAM lParam);

public:
	int	m_nBitIDBack;
	int m_nBitIDDrag;

	void SetBitmapID( int nBitIDBack, int nBitIDDrag );
};


