#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSimpleEdit 대화 상자입니다.

class CSimpleEdit : public CDialog
{
	DECLARE_DYNAMIC(CSimpleEdit)

public:
	CSimpleEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSimpleEdit();

	std::tr1::shared_ptr<CNpcDialogueSet> m_spDialogueSet;
	std::tr1::shared_ptr<CNpcDialogue> m_spNpcDialogue;

	void SetNpcDialogueSet(std::tr1::shared_ptr<CNpcDialogueSet> spDialogueSet) { m_spDialogueSet = spDialogueSet; };
	void SetSimpleData();
	void SetNPCData(std::tr1::shared_ptr<CNpcDialogue> spNpcDialogue);
	void UpdateAnswerList( DWORD dwCaseIndex );
	void UpdateAnswerData( int nIndex );
	void ApplyNPCData();
	void ApplyAnswerData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SIMPLEEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	

public:
	CSliderCtrl m_Basic_Slider;
	CEdit m_Basic_Edit;
	CEdit m_BasicConv_Edit;
	CListCtrl m_Answer_List;
	CEdit m_Text_Edit;
	int	  m_OldGetPos;
	int	  m_OldTextGetPos;

	DWORD m_dwSelectCase;

	DECLARE_MESSAGE_MAP()


	afx_msg void OnNMCustomdrawBasicSlider(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMClickAnswerList(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_Answer_Edit;
	CEdit m_AnswerID_Edit;
	CEdit m_AnswerType_Edit;
	CEdit m_AnswerData_Edit;
//	afx_msg void OnBnClickedApplyButton();
	afx_msg void OnBnClickedAnswerapplyButton();
	afx_msg void OnBnClickedTextapplyButton();
	afx_msg void OnNMCustomdrawTextSlider(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_Text_Slider;
	CEdit m_TextId_Edit;
};
