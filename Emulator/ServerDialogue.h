#pragma once

#include "./Resource.h"

// CAgentDialogue 대화 상자입니다.

class CServerDialogue : public CDialog
{
	DECLARE_DYNAMIC(CServerDialogue)

public:
	CServerDialogue(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServerDialogue();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_AGENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnMessageLogConsole(WPARAM wParam, LPARAM lParam);
};
