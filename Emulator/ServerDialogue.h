#pragma once

#include "./Resource.h"

// CAgentDialogue ��ȭ �����Դϴ�.

class CServerDialogue : public CDialog
{
	DECLARE_DYNAMIC(CServerDialogue)

public:
	CServerDialogue(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CServerDialogue();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_AGENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnMessageLogConsole(WPARAM wParam, LPARAM lParam);
};
