#pragma once
#include "Resource.h"
#include "afxcmn.h"


// DlgUIEdit 대화 상자입니다.
class CUIEditGroup;

class CDlgUIEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgUIEdit)

public:
	CDlgUIEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgUIEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MAIN };
	enum { LIST_NO = 0, LIST_NAME, LIST_PARENT, LIST_VISIBLE};

protected:
	int			m_nSelectedItem;
	int			m_nPrevSelectedItem;

public:
	CListCtrl	m_CtrlList;
	CButton		m_CheckZoom;
	CButton		m_CheckVisible;
	CButton		m_CheckViewCtrl;
	CButton		m_CheckShowTip;
	CButton		m_CheckViewLine;

public:
	BOOL isEdit();
	void ResetList();
	void SetSelectedUIInfoMain(CUIEditGroup* UIEditGroup);
	void SetSelectedUIInfoPos(int nX,int nY);
	void SetSelectedUITextureInfo(CString strTexName);
	void SetSelectedUIButtonInfo(CString Btn1, CString Btn2, CString Btn3);
	void SetSelectedUIButtonIn(CString strInName);
	void SetSelectedUIButtonClick(CString strClickName);
	void SetSelectedUIInfo(int nItem);
	void SetSelectedUIOptionInfo(int nItem);
	void ClearUIInfo();

	CString GetControlTypetoString(int nType);

	void SetSelectedItem(int nItem);
	int  GetSelectedItem() {return m_nSelectedItem;}

public:
	afx_msg void OnPaint();
	//create
	afx_msg void OnBnClickedButtonPic();
	afx_msg void OnBnClickedButtonButton();
	afx_msg void OnBnClickedButtonStatic();
	afx_msg void OnBnClickedButtonAni();
	afx_msg void OnBnClickedButtonText();
	afx_msg void OnBnClickedButtonScroll();
	//info
	afx_msg void OnBnClickedButtonAlign();
	afx_msg void OnBnClickedButtonOk();
	//texture
	afx_msg void OnBnClickedButtonTextureset();
	afx_msg void OnBnClickedButtonInflip();
	afx_msg void OnBnClickedButtonClickflip();
	//list
	afx_msg void OnLvnItemchangedListControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusListControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonPaste();
	afx_msg void OnBnClickedButtonSetParent();
	//file menu
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonMerge();
	afx_msg void OnBnClickedButtonSave();
	//graph
	afx_msg void OnBnClickedButtonSetInterval();
	afx_msg void OnBnClickedButtonSetColor();
	//option
	afx_msg void OnBnClickedCheckVisible();
	afx_msg void OnBnClickedCheckViewCtrl();
	afx_msg void OnBnClickedCheckViewLine();
	afx_msg void OnBnClickedCheckShowTip();
	afx_msg void OnBnClickedCheckZoom();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
