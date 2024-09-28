#pragma once

#include "afxcmn.h"
#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include <vector>

// CDlgQuestAdd 대화 상자입니다.

class CDlgQuestAdd : public CDialog
{
	DECLARE_DYNAMIC(CDlgQuestAdd)

public:
	CDlgQuestAdd(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgQuestAdd();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_QUEST_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    void InitListHead();
    void SetData();    

    int m_nSelected;;
    std::vector<int> m_vSelected;
    CListCtrl m_ListQuestAdd;
    std::vector<GLQUEST> m_vQuest;

    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnNMDblclkListQuestAdd(NMHDR *pNMHDR, LRESULT *pResult);
};
