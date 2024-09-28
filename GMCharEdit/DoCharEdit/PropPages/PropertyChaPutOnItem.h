#pragma once

#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../SubDialogs/DlgItemDisguise.h"
#include "afxwin.h"

namespace gmce
{
	class IDBManager;
}

class CPropertyChaPutOnItem : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyChaPutOnItem)

public:
	CPropertyChaPutOnItem();
	virtual ~CPropertyChaPutOnItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPERTY_CHAR_PUTONITEM };

	gmce::IDBManager *m_pDBMan;

public:
	void GetData(GLCHARLOGIC* pChar);
	void SetData( SITEMCUSTOM sITEM = SITEMCUSTOM(NATIVEID_NULL()) );
    std::vector<SITEMCUSTOM> m_vPutOnItem;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();
	afx_msg void OnBnClickedRadio8();
	afx_msg void OnBnClickedRadio9();	
	afx_msg void OnBnClickedRadio10();
	afx_msg void OnBnClickedRadio11();	
	afx_msg void OnBnClickedRadio12();
	afx_msg void OnBnClickedRadio13();
	afx_msg void OnBnClickedRadio14();
	afx_msg void OnBnClickedRadio15();
	afx_msg void OnBnClickedRadio16();
	afx_msg void OnBnClickedRadio17();
	afx_msg void OnBnClickedRadio18();
	afx_msg void OnBnClickedRadio19();
	afx_msg void OnBnClickedBtnPutonDelete();
	afx_msg void OnBnClickedBtnPutonChange();
	afx_msg void OnBnClickedBtnPutonitemSave();
	afx_msg void OnBnClickedBtnDisguise();

	int m_nAvoidRate;
	int m_nDamage;
	int m_nDefense;
	int m_nHitRate;

	GLPADATA m_sDamageStat;
	WORD m_wDefenceStat;
	
	int m_nElectric;
	int m_nFire;
	int m_nIce;
	int m_nPoison;
	int m_nSpirit;

	DWORD m_wReModel;
	DWORD m_wUsedNum;	
	int m_nRadio;
	int m_nMainID;
	int m_nSubID;
	CString m_strName;	
	CEdit m_editTIME;

	CString m_strGenID;

	DWORD m_dwCOSTUME_MID;
	DWORD m_dwCOSTUME_SID;

	CComboBox m_comboLinkSkillTarget;
	SNATIVEID m_sLinkSkillID;
	int m_wLinkSkillLevel;
	float m_fLinkSkillOccurRate;

	CComboBox m_comboAddOpt1;
	CComboBox m_comboAddOpt2;
	CComboBox m_comboAddOpt3;
	CComboBox m_comboAddOpt4;
	CComboBox m_comboAddOpt5;
	CComboBox m_comboAddOpt6;

	SITEMCUSTOM m_NewItem;
	CComboBox m_comboRandomOpt1;
	CComboBox m_comboRandomOpt2;
	CComboBox m_comboRandomOpt3;
	CComboBox m_comboRandomOpt4;
	CComboBox m_comboRandomOpt5;
// 	short m_nRandomOpt1;
// 	short m_nRandomOpt2;
// 	short m_nRandomOpt3;
// 	short m_nRandomOpt4;
	CString m_strCosTime;

	CDlgItemDisguise m_dlgItemDisguise;

	std::vector<SITEMCUSTOM> m_vecWriteLogItems;
	afx_msg void OnBnClickedBtnPutonApply();
};
