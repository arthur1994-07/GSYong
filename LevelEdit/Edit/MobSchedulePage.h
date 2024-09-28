#pragma once

#include "../resource.h"
#include "../../RanLogic/Crow/GLMobSchedule.h"

#include "afxwin.h"

// CMobSchedulePage 대화 상자입니다.
class CsheetWithTab;

class CMobSchedulePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMobSchedulePage)

protected:
	CsheetWithTab*	m_pSheetTab;	

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	ActiveMobScheDuleEdit ( BOOL bShow, BOOL bNew );
	void	ListingUpdate ();

protected:
	DXAFFINEMATRIX	m_AffineMatrix;

protected:
	BOOL					m_bInit;

	MOBACTIONLIST*			m_pMobActList;
	SMOBACTION*				m_pMobAction;

public:
	void SetSchedule(sc::CGLLIST<SMOBACTION>* pMobActList);

public:
	void UpdatePage ();

public:
	CMobSchedulePage();
	virtual ~CMobSchedulePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MOBSCHEDULEPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDelete();	
	afx_msg void OnBnClickedButtonOk();
	CComboBox m_ctrlAction;
	CString m_valTime;
	CListBox m_ctrlSchedule;
	afx_msg void OnCbnSelchangeComboAction();
	afx_msg void OnLbnDblclkListSchedule();
	afx_msg void OnBnClickedButtonMove();
};
