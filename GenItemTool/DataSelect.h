#pragma once
#include "./resource.h"
#include "../RanLogic/GenItemTable/GenItemTableDefine.h"


// DataSelect ��ȭ �����Դϴ�.

class DataSelect : public CDialog
{
	DECLARE_DYNAMIC(DataSelect)

public:
	DataSelect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DataSelect();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SELECT_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	BOOL OnInitDialog();
	void UpDateItems();
	void SetCase(GenItem::FindFromWordCase _Case) 
		{ m_FindCase = _Case; }
	std::string GetCompleteStr()
		{	return m_CompleteWord;	}
	void SetCompleteStr(std::string _Word)
	{
		m_CompleteWord = _Word;
	}

private:
		CListCtrl m_ListFindWord;
		CListCtrl m_ListSelectData;

		std::vector<std::string> m_VecDataName;
		std::vector<std::string> m_VecSelectName;
		
		GenItem::FindFromWordCase m_FindCase;
		std::string m_CompleteWord;
		int m_CountSelectLine;

public:
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnEnChangeEditInputName();
	afx_msg void OnBnClickedButtonComplete();

	afx_msg void OnNMDblclkListFindword(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListSelectData(NMHDR *pNMHDR, LRESULT *pResult);
};
