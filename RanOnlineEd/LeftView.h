#pragma once

#include <string>
#include "../../RanThirdParty/XtremeToolkit/Include/XTToolkitPro.h"

// LeftView 뷰입니다.

class LeftView : public CListView
{
	DECLARE_DYNCREATE(LeftView)

protected:
	LeftView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LeftView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    enum EM_TOOL
    {
		TOOL_IDX_0 = 0,
		TOOL_IDX_1,
		TOOL_IDX_2,
        TOOL_IDX_3,
        TOOL_IDX_4,
        TOOL_COUNT,
    };

    std::string m_ToolName[TOOL_COUNT];
    CXTOutlookBar m_wndOutlookBar;
    CXTPagerCtrl  m_wndPager;

protected:
	DECLARE_MESSAGE_MAP()

public:
    void MakeFileListToExcel();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnOutbarNotify(UINT lParam, LONG wParam);
    afx_msg void OnPagerScroll(NMHDR* pNMHDR, LRESULT * pResult);
    afx_msg void OnPagerCalcSize(NMHDR* pNMHDR, LRESULT* pResult);  
};


