#pragma once

#include "afxcmn.h"
#include "../Resource.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

class	CsheetWithTab;

// CTaxiStationPage 대화 상자입니다.

class CTaxiStationPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTaxiStationPage)

public:
	CTaxiStationPage( LOGFONT logfont );
	virtual ~CTaxiStationPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TAXISTATIONPAGE };

protected:
	CsheetWithTab*	m_pSheetTab;
	CFont*			m_pFont;

public:
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }

	void	Init();
	void	UpdateItems ();
	void	InverseUpdateItems();

	bool	NewFile();
	bool	LoadFile();
	bool	LoadFileOld();
	bool	SaveFile();

	bool	SaveCsv();
	bool	LoadCsv();

	bool	IsEdit()	{ return m_bEdit; }
	void	FindItem();

public:
	bool	m_bEdit;

	int			m_nFindIndexMap;
	int			m_nFindIndexNpc;
	SNATIVEID	m_sFindID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ListTaxi;
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnNMDblclkListTaxistation(NMHDR *pNMHDR, LRESULT *pResult);
};
