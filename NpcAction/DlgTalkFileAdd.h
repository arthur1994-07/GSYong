#pragma once
#include "afxwin.h"


// DlgTalkFileAdd 대화 상자입니다.

class DlgTalkFileAdd : public CDialog
{
	DECLARE_DYNAMIC(DlgTalkFileAdd)

public:
	DlgTalkFileAdd(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgTalkFileAdd();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TALK_FILE_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:    
    CString m_TalkName;
};
