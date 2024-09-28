#pragma once

#include "../Resource.h"

class DxViewPortPreview;

/////////////////////////////////////////////////////////////////////////////
// CMaterialExpressionsWnd dialog

class CMaterialExpressionsWnd : public CTreeCtrl
{
public:
    CMaterialExpressionsWnd();   // standard constructor

// Construction
public:
    virtual ~CMaterialExpressionsWnd();

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};