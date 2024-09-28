#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Quest/GLQuestMan.h"


// CQuestTree ��ȭ �����Դϴ�.
class	CsheetWithTab;
typedef GLQuestMan::SNODE QUESTNODE;
typedef QUESTNODE*		  PQUESTNODE;
class CQuestTree : public CPropertyPage
{
	DECLARE_DYNAMIC(CQuestTree)

public:
	CQuestTree( LOGFONT logfont );
	virtual ~CQuestTree();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_QUESTTREE_DIALOG };
private:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;
	HTREEITEM		m_TreeRoot;
	CFont*			m_pFont;

public:
	BOOL	UpdateTree ();
	void	UpdateName ();

private:
	BOOL	UpdateItem ( PQUESTNODE pQuestNode, HTREEITEM hMainItem );
	const char*	GetFormatName ( PQUESTNODE pQuestNode );

public:
	void SetSheetTab(CsheetWithTab* pSheetTab) { m_pSheetTab = pSheetTab; }
	void CleanAllQuest();

private:
	PQUESTNODE	m_pQuestNode;
	BOOL		m_bDummyHasQuest;
	PQUESTNODE	m_pDummyQuestNode;

private:
	BOOL	AddQuest();
	BOOL	AddQuestSub ();    
	
	BOOL	DelQuest ();

	BOOL	CopyQuest ();
	BOOL	PasteQuestNew ();
	BOOL	PasteQuestValue ();

private:
	int		GetSelectMenu ( POINT pt );
	void	ForceSelectOnRightClick ( POINT pt );
	void	RunSelectMenu ( int nSelect );

	void		SetSelectItem ( PQUESTNODE pQuestNode );
	PQUESTNODE	GetSelectItem ();

	void SetReadMode(CMenu* pMenu);
    bool ClassFilter(GLQUEST* pQuest);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlQuestTree;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedRadioQuestSchool();
    int m_SchoolRadioButton;
    afx_msg void OnBnClickedRadio2();
    afx_msg void OnBnClickedRadio3();
    afx_msg void OnBnClickedRadio4();
    afx_msg void OnBnClickedCheckClassFight();
    afx_msg void OnBnClickedCheckClassArcher();
    afx_msg void OnBnClickedCheckClassFencing();
    afx_msg void OnBnClickedCheckClassSpirit();
    afx_msg void OnBnClickedCheckClassExtream();
    afx_msg void OnBnClickedCheckClassScientist();
    afx_msg void OnBnClickedCheckClassAssassin();
    afx_msg void OnBnClickedCheckClassEtc();
	afx_msg void OnBnClickedCheckClassActor();
};
