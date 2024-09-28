#pragma once

#include "./Resource.h"
#include "afxwin.h"

class CDlgMainView : public CDialog
{
	DECLARE_DYNAMIC( CDlgMainView )

public:
	enum
	{
		IDD = IDD_MAIN_VIEW_DLG,
		DLG_SIZE = 512,
	};

protected:
	FLOAT	m_fMagnify;

public:
	CDlgMainView( CWnd* pParent = NULL );
	virtual ~CDlgMainView();

protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg VOID OnTimer( UINT_PTR nIDEvent );
	afx_msg VOID OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );

public:
	VOID Render();

protected:
	virtual void OnCancel();
};