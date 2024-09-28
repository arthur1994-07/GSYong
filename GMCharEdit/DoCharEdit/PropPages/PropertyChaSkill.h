#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Character/GLCharData.h"

namespace gmce
{
	class IDBManager;
}

class CPropertyChaSkill : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyChaSkill)

public:
	CPropertyChaSkill();
	virtual ~CPropertyChaSkill();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPERTY_CHAR_SKILL };

	gmce::IDBManager *m_pDBMan;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
    void SetData(SCHARDATA2* pChar);
    void GetData(SCHARDATA2* pChar);
    void InitListHead();    

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

	int m_nSelectSlot;
    
    CListCtrl m_ListSkill;
    std::vector<SCHARSKILL> m_vSkill;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnNMDblclkListCharEditSkill(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedBtnSkillSave();
	afx_msg void OnBnClickedRadioCharSlotA ( void );
	afx_msg void OnBnClickedRadioCharSlotB ( void );
	afx_msg void OnBnClickedRadioCharSlotC ( void );
	afx_msg void OnBnClickedRadioCharSlotD ( void );
	afx_msg void OnBnClickedRadioCharSlotE ( void );
	virtual void OnOK();
};
