#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Skill/GLSkill.h"

// CSkillTree 대화 상자입니다.
class	CsheetWithTab;
struct GLCHARLOGIC;

class CSkillTree : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkillTree)

public:
	CSkillTree( LOGFONT logfont );   // 표준 생성자입니다.
	virtual ~CSkillTree();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SKILLTREE_DIALOG };

protected:
	CsheetWithTab*	m_pSheetTab;
//	CWnd*			m_pComboBox;
	BOOL			m_bDlgInit;
	CFont*			m_pFont;
	HTREEITEM		m_TreeItemList[EMSKILLCLASS_NSIZE][GLSkillMan::MAX_CLASSSKILL];	

public:
	bool	ExportExcelDetail_CheckItem ( std::string& strSaveFileName, boost::function<void (int)> PercentCallbackFunc=NULL );
	std::string GetSkillInfoText ( GLSKILL* pSkill, WORD wLevel );
	BOOL	UpdateTreeItem ();

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
//	void	SetComboBoxHandle ( CWnd* pWnd ) { m_pComboBox = pWnd; }
	void	CleanAllItem ();

protected:
	DWORD	m_ItemNodeID;

	BOOL	m_bDummyHasItem;
	DWORD	m_DummyItemNodeID;

	GLCHARLOGIC*	m_pDefaultCharacter;

public:
	void	UpdateName ();
	void	SetReadMode( CMenu* pMenu );

protected:
	BOOL	AddItem ();	
	BOOL	DelItem ();
	BOOL	ModifyItem ();

protected:
	BOOL	CopyItem ();
	BOOL	PasteItemNew ();
	BOOL	PasteItemValue ();

	BOOL	ISROOT ();

protected:
	int		GetSelectMenu ();

private:
	void	CheckChildItems ( HTREEITEM hItem );
	void	UnCheckChildItems ( HTREEITEM hItem );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlItemTree;	
	virtual BOOL OnInitDialog();
	afx_msg void OnTVNLclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult);

};