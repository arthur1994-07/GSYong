#pragma once

#include "../resource.h"
#include "afxwin.h"

class CsheetWithTab;
class DxPrefab;

// CPrefabEdit ��ȭ �����Դϴ�.

class CPrefabEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CPrefabEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	BOOL m_bInitDlg;
	D3DXMATRIX* m_rmatPiece;
	void ReSetListCtrl();
	void SetPositionEdit();

public:
	void KeyBoard_Enter();
	void KeyBoard_KeyUp_Delete();

	void UpdateKeyBoardMouse();

public:
	void UpdatePage();					// ���λ����� ������ ���� List �� ����
	void UpdatePageClickUP();

	void SelectPrefabList( DxPrefab* pPrefab );

private:
	void SetFocus( CListCtrl& listReplace, int nIndex );

public:
	CPrefabEdit();
	virtual ~CPrefabEdit();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PREFAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

public:
	CListCtrl m_listReplace;

public:
	afx_msg void OnNMClickListReplace(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRotateInit();
	afx_msg void OnBnClickedButtonRotateRandom();
	afx_msg void OnBnClickedButtonRotateL90();
	afx_msg void OnBnClickedButtonRotateL15();
	afx_msg void OnBnClickedButtonRotateR15();
	afx_msg void OnBnClickedButtonRotateR90();

	afx_msg void OnBnClickedButtonHeightPick();
	afx_msg void OnBnClickedButtonHeightUp10();
	afx_msg void OnBnClickedButtonHeightUp1();
	afx_msg void OnBnClickedButtonHeightDown1();
	afx_msg void OnBnClickedButtonHeightDown10();
	afx_msg void OnBnClickedButtonHeightResetAll();
	afx_msg void OnBnClickedButtonFloatZero();
	afx_msg void OnBnClickedButtonScale();
};
