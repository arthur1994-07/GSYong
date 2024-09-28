#pragma once
#include "afxwin.h"


// CNaturePage 대화 상자입니다.
class CsheetWithTab;
//class GLMobScheduleMan;
//class GLMobSchedule;
class CNaturePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CNaturePage)

public:
	CNaturePage();
	virtual ~CNaturePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_NATUREPAGE };

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
//	static GLMobScheduleMan*	m_pMobSchMan;
//	static GLMobSchedule*		m_pSchedule;

public:
//	void	ActiveMobSetEdit ( BOOL bShow, BOOL bNew );	
//	void	ListingUpdate ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	
//public:	
//	afx_msg void OnBnClickedButtonCreate();
//	afx_msg void OnBnClickedButtonDel();
//	afx_msg void OnBnClickedButtonModify();
//	CListBox m_ctrlMobList;
//	afx_msg void OnBnClickedButtonSchedule();
//	afx_msg void OnBnClickedButtonAdd();
//	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
//	CString m_szFileName;
//	afx_msg void OnBnClickedButtonLoadfile();
//	afx_msg void OnBnClickedButtonSavefile();
//	afx_msg void OnBnClickedButtonNew();
//	CString m_valName;
//	CString m_valSkinName;
//	CString m_valReGenTime;
//	afx_msg void OnBnClickedButtonSkin();
//	afx_msg void OnBnClickedButtonTest();
	
public:
	afx_msg void OnBnClickedButtonEffectchangeadd();
	afx_msg void OnBnClickedButtonEffectstate();
	afx_msg void OnBnClickedButtonEffectdelete();
	CListBox m_pListEffect;
	CComboBox m_pComboEffect;
};

//extern GLMobScheduleMan g_Man;