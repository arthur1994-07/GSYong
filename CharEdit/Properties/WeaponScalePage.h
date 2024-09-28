#pragma once

#include "../Resource.h"
#include "afxwin.h"

class CsheetWithTabChar;

// CWeaponScalePage ��ȭ �����Դϴ�.

class CWeaponScalePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CWeaponScalePage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab )	{ m_pSheetTabChar = pSheetTab; }

private:
	void UpdateWeaponScaleCTRL();

public:
	CWeaponScalePage();
	virtual ~CWeaponScalePage();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_WEAPON_SCALE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedOk();
	CListCtrl m_ctrlWeaponScale;
	afx_msg void OnNMClickListWeaponScale(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditScale();
	afx_msg void OnBnClickedButtonDocument();
};
