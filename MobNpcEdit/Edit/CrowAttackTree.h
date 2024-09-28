#pragma once

#include "afxcmn.h"

#include "../../RanLogic/Crow/GLCrowData.h"

class CsheetWithTab;

class CCrowAttackTree : public CPropertyPage
{
	DECLARE_DYNAMIC(CCrowAttackTree)

public:
	CCrowAttackTree( LOGFONT logfont );
	virtual ~CCrowAttackTree();

	enum { IDD = IDD_CROWATTACKTREE_DIALOG };

protected:
	CsheetWithTab*	m_pSheetTab;			// SheetTab;
	int				m_CallPage;				// 페이지;

	SCROWDATA*		m_pDummyCrowData;		// Dummy Crow Data;
	SCROWDATA*		m_pCrowData;			// Crow Data;

	BOOL			m_bDlgInit;				// 초기화 여부;
	HTREEITEM		m_TreeRoot;				// Tree Root;

	SNATIVEID		m_sSelectPosition;		// 현재 선택된 위치;
	//CROWATTACK_VEC*	m_pItemNode;			// 현재 선택된 데이터 값;

	void	SetReadMode( CMenu* pMenu );

public:
	void	InitAllItem ();
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetCrowData ( SCROWDATA* pCrowData );

	void	InitDefaultCtrls ();
	void	UpdateItems ();
	void	CleanAllItem ();

private:
	void SetSelectItem ( const SNATIVEID& sSelectPosition );
	const SNATIVEID& GetSelectItem ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CTreeCtrl m_ctrlItemTree;
	virtual BOOL OnInitDialog();	
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
};