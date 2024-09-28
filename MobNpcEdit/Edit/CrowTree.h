#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Crow/GLCrowData.h"

// CCrowTree 대화 상자입니다.
class	CsheetWithTab;
class CCrowTree : public CPropertyPage
{
	DECLARE_DYNAMIC(CCrowTree)

public:
	CCrowTree( LOGFONT logfont );   // 표준 생성자입니다.
	virtual ~CCrowTree();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CROWTREE_DIALOG };

protected:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;
	HTREEITEM		m_TreeRoot;
	CFont*			m_pFont;

public:
	BOOL	UpdateTree ();
	void	UpdateName ();

	void	SetReadMode( CMenu* pMenu );

private:
	BOOL	UpdateItem ( PCROWDATA pItem, HTREEITEM hMainItem );
	const char*	GetFormatName ( PCROWDATA pItem );

public:
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	CleanAllItem ();

private:
	PCROWDATA	m_pItemNode;

	BOOL		m_bDummyHasItem;
	PCROWDATA	m_pDummyItemNode;

private:
	BOOL	AddItem ();
	BOOL	AddItemMain( HTREEITEM hMainItem );
	BOOL	AddItemSub ();    

	BOOL	DelItem ();

	BOOL	CopyItem ();
	BOOL	PasteItemNew ();
	BOOL	PasteItemValue ();

private:
	int		GetSelectMenu ( POINT pt );
	void	ForceSelectOnRightClick ( POINT pt );
	void	RunSelectMenu ( int nSelect );

	void		SetSelectItem ( PCROWDATA pItemNode );
	PCROWDATA	GetSelectItem ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlItemTree;
	virtual BOOL OnInitDialog();	
	afx_msg void OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);		
	afx_msg void OnBnClickedButtonUpdateSync();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonReloadStrtbl();
	afx_msg void OnBnClickedButtonStrtblSync();
	afx_msg void OnBnClickedBtnCsvSave();
	afx_msg void OnBnClickedBtnCsvLoad();
	afx_msg void OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnExcelStrTblImport();
	afx_msg void OnBnClickedBtnExcelStrTblExport();
	afx_msg void OnBnClickedBtnXmlConvert();
};