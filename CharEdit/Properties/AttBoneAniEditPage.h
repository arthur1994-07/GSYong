#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAttBoneAniEditPage ��ȭ �����Դϴ�.

class CAttBoneAniEditPage : public CDialog
{
	DECLARE_DYNCREATE(CAttBoneAniEditPage)

public:
	CAttBoneAniEditPage(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	CAttBoneAniEditPage(
        const DWORD dwObjType,
        const DWORD dwPieceType,
        const SAttachBoneAnimInfo& sAttachBoneAnimInfo, 
        const CString& sAnimName,
        DxSkinAniControl *pSkinAniCtrl,
        CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAttBoneAniEditPage();


	SAttachBoneAnimInfo GetAttachBoneAnimInfo() { return m_sAttachBoneAnimInfo; }

	void UpdateDialog();
	
protected:
	DWORD				m_dwObjType;
	DWORD				m_dwPieceType;
	SAttachBoneAnimInfo m_sAttachBoneAnimInfo;
	CString				m_sAnimName;
	DxSkinAniControl*	m_pSkinAniContrl;


// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ATTACHANIEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
