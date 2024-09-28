#pragma once
#include "afxwin.h"

class DxPiece;
class CsheetWithTab;

// CPieceManagerEdit 대화 상자입니다.

class CPieceManagerEdit : public CPropertyPage
{
	enum EXOPTIONS
	{
		EMOPT_NONE	= 0,
		EMOPT_AND	= 1,
		EMOPT_NOT	= 2,
	};

	DECLARE_DYNAMIC(CPieceManagerEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void UpdatePage();
    void SelectPieceList( DxPiece* pPiece );

protected:
	BOOL m_bInitDlg;
	BOOL m_bFrameSelect;
	void ReSetListCtrl();
	void FindFrame( DxFrame *pframe, CString strFrameStr, CString strFrameStrEx, EXOPTIONS emOption );

public:
	CPieceManagerEdit();
	virtual ~CPieceManagerEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PIECE_MANAGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonSingleReplaceobj();
	afx_msg void OnBnClickedButtonSingleReplaceobjApply();
	afx_msg void OnBnClickedButtonFramesearch();
	CComboBox m_comboExSearch;
	CListCtrl m_listReplace;
	CListBox m_FrameList;
	afx_msg void OnBnClickedButtonSelectfile();
	afx_msg void OnBnClickedButtonReplaceProcessing();
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnCbnSelchangeComboExSearch();
	afx_msg void OnNMClickListReplace(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioFrame();
	afx_msg void OnBnClickedRadioPoint();
	afx_msg void OnBnClickedButtonTextexport();
};
