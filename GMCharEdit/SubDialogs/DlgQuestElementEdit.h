#pragma once

#include "../../RanLogic/Quest/GLQuest.h"
#include "afxcmn.h"

// CDlgQuestElementEdit 대화 상자입니다.

class CDlgQuestElementEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgQuestElementEdit)

public:
	CDlgQuestElementEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgQuestElementEdit();

public:
    GLQUESTPROG* m_pQuestProg;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_QUEST_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    void InitListHead();
    void SetData(GLQUEST_STEP* pStep, CString strStatus, const int nStepNumber);

    virtual BOOL OnInitDialog();
    CString m_strQuestName;
    CString m_strQuestComment;
    CListCtrl m_ListQuestStep;
};
