#pragma once
#include "afxwin.h"


// DlgTalkFileAdd ��ȭ �����Դϴ�.

class DlgTalkFileAdd : public CDialog
{
	DECLARE_DYNAMIC(DlgTalkFileAdd)

public:
	DlgTalkFileAdd(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgTalkFileAdd();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TALK_FILE_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:    
    CString m_TalkName;
};
