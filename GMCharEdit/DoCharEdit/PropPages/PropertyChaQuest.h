#pragma once
#include "afxcmn.h"
#include <vector>

namespace gmce
{
	class IDBManager;
}


class CPropertyChaQuest : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyChaQuest)

public:
	CPropertyChaQuest();
	virtual ~CPropertyChaQuest();

	enum { IDD = IDD_PROPERTY_CHAR_QUEST };

	gmce::IDBManager *m_pDBMan;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

    void InitListHead();
    void SetData(SCHARDATA2* pChar);
    
    CListCtrl m_ListQuest;
    
    enum { PROGRESS, COMPLETED };

    struct QuestProg
    {
        GLQUESTPROG* pQuestProg;
        int nStatus;
    };
    
    std::vector<QuestProg> m_vQuestProg;

    afx_msg void OnBnClickedButton1();    
    afx_msg void OnBnClickedBtnQuestSave();
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBtnDeleteQuest();
	virtual void OnOK();
	afx_msg void OnBnClickedBtnQuestExportToCsv();
};
