#pragma once

#include "../../RanLogic/Item/GLItem.h"
#include "DlgItemDisguise.h"
#include "afxwin.h"

// CDlgInvenElementEdit 대화 상자입니다.

class CDlgInvenElementEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgInvenElementEdit)

public:
	CDlgInvenElementEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInvenElementEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_INVEN_ELEMENT_EDIT };

public:
	SINVENITEM m_sItem;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnDisguise();

	int m_nDamage;
	int m_nDefense;

	GLPADATA m_sDamageStat;
	WORD m_wDefenceStat;

	int m_nElectric;
	int m_nFire;
	int m_nIce;
	int m_nPoison;
	int m_nSpirit;

	DWORD m_wTurnNum;

	int m_nMainID;
	int m_nSubID;

	CString m_strName;

	afx_msg void OnBnClickedOk();
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

	CComboBox m_comboRandomOpt1;
	CComboBox m_comboRandomOpt2;
	CComboBox m_comboRandomOpt3;
	CComboBox m_comboRandomOpt4;
	CComboBox m_comboRandomOpt5;
	short m_nRandomOpt1;
	short m_nRandomOpt2;
	short m_nRandomOpt3;
	short m_nRandomOpt4;
	short m_nRandomOpt5;
	CString m_strCosTime;

	CDlgItemDisguise m_dlgItemDisguise;
};