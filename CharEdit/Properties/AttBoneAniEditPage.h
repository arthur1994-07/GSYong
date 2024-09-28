#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAttBoneAniEditPage 대화 상자입니다.

class CAttBoneAniEditPage : public CDialog
{
	DECLARE_DYNCREATE(CAttBoneAniEditPage)

public:
	CAttBoneAniEditPage(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CAttBoneAniEditPage(
        const DWORD dwObjType,
        const DWORD dwPieceType,
        const SAttachBoneAnimInfo& sAttachBoneAnimInfo, 
        const CString& sAnimName,
        DxSkinAniControl *pSkinAniCtrl,
        CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAttBoneAniEditPage();


	SAttachBoneAnimInfo GetAttachBoneAnimInfo() { return m_sAttachBoneAnimInfo; }

	void UpdateDialog();
	
protected:
	DWORD				m_dwObjType;
	DWORD				m_dwPieceType;
	SAttachBoneAnimInfo m_sAttachBoneAnimInfo;
	CString				m_sAnimName;
	DxSkinAniControl*	m_pSkinAniContrl;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ATTACHANIEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboType;
	afx_msg void OnBnClickedCheckStrike();
	CListCtrl m_list_AttStrikeAni;
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnNMDBLClickListAttstrikeani(NMHDR *pNMHDR, LRESULT *pResult);
};
