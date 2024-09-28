#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Item/GLItem.h"
#include "../resource.h"

class	CsheetWithTab;

class CItemMixPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CItemMixPage)
	DECLARE_MESSAGE_MAP()

public:
	CItemMixPage( LOGFONT logfont );
	virtual ~CItemMixPage();

	enum { IDD = IDD_ITEMMIXPAGE };

protected:
	CsheetWithTab*	m_pSheetTab;
	CFont*			m_pFont;

public:
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }

    void	Init();
	void	UpdateItems ();
	void	FindItem();

	bool	NewFile();
	bool	LoadFile();
	bool	LoadFileOld();
	bool	SaveFile();

	bool	SaveCsv();
	bool	LoadCsv();

	bool	IsEdit()	{ return m_bEdit; }

public:
	int			m_nFindIndex;
	SNATIVEID	m_sFindID;

	bool		m_bEdit;

	CListCtrl m_ListItemMix;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnNMDblclkListItemmix(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonNext();
};