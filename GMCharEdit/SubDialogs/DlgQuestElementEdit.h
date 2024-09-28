#pragma once

#include "../../RanLogic/Quest/GLQuest.h"
#include "afxcmn.h"

// CDlgQuestElementEdit ��ȭ �����Դϴ�.

class CDlgQuestElementEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgQuestElementEdit)

public:
	CDlgQuestElementEdit(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgQuestElementEdit();

public:
    GLQUESTPROG* m_pQuestProg;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_QUEST_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    void InitListHead();
    void SetData(GLQUEST_STEP* pStep, CString strStatus, const int nStepNumber);

    virtual BOOL OnInitDialog();
    CString m_strQuestName;
    CString m_strQuestComment;
    CListCtrl m_ListQuestStep;
};
