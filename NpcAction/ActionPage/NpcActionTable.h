#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"

// CNpcActionTable 대화 상자입니다.
class CsheetWithTab;

class CNpcActionTable : public CPropertyPage
{
	DECLARE_DYNAMIC(CNpcActionTable)

public:
	CNpcActionTable(LOGFONT logfont);
	virtual ~CNpcActionTable();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCACTIONTABLE };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	BOOL			m_bDlgInit;
	CFont*			m_pFont;

	spNpcDialogSet m_spDialogueSet;
    int m_SelectedTalkFileIndex;

public:
	void SetCallPage(int CallPage) { m_CallPage = CallPage; }
	void SetSheetTab(CsheetWithTab* pSheetTab) { m_pSheetTab = pSheetTab; }
	spNpcDialogSet GetDialogueSet();

public:
	void UpdateItems();
	void SetReadMode();
    void UpdateTalkFileList(const std::vector<std::string>& TalkList);
    void TalkFileSelected(int Item);
	void DialogClear();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlActionTable;
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedCsvimportButton();
	afx_msg void OnNMDblclkListAction(NMHDR *pNMHDR, LRESULT *pResult);
    CListCtrl m_TalkFileList;
    afx_msg void OnLvnItemchangedListTalkFile(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonAddTalkFile();
    afx_msg void OnBnClickedButtonDeleteTalkFile();
	afx_msg void OnNMRClickNTKList(NMHDR *pNMHDR, LRESULT *pResult);
};