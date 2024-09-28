#pragma once
#include "afxwin.h"


// CSkinPiecePage ��ȭ �����Դϴ�.

#include "../MfcExLib/ExLibs/Separator.h"
#include "afxcmn.h"
class CsheetWithTabChar;

// CAttachLinkPage ��ȭ �����Դϴ�.

class CAttachLinkPage  : public CPropertyPage
{
	DECLARE_DYNAMIC(CAttachLinkPage )


protected:
	CsheetWithTabChar	*m_pSheetTabParent;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabParent = pSheetTab; }

public:
	CAttachLinkPage ();
	virtual ~CAttachLinkPage ();

	BOOL LoadAblFile( const char *pszFileName );
	BOOL LoadABFFile ( const char* szFile );
	void AnimationListing ();
	void UpdateEditWindow();

	void SetParentObjTypeContrl ();
	void SetMeshSetContrl ();
	void SetComboWeaponWhereBack();
	void UpdateValueData( bool bSet = TRUE );

	void UpdateObjTypeComboBox ();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ATTACHLINKPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoadabf();
	afx_msg void OnBnClickedButtonLoadbone();
	afx_msg void OnBnClickedButtonGetbone();
	afx_msg void OnBnClickedButtonLoadcps();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckUsecps();
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonSave();
	CListBox m_list_Animation;
	afx_msg void OnLbnDblclkListAnimation();
	afx_msg void OnBnClickedRadioPosition();
	afx_msg void OnBnClickedRadioRotation();
	afx_msg void OnBnClickedButtonReset1();
	afx_msg void OnBnClickedRadioScale();

	CComboBox m_comboObjType;
	CComboBox m_comboSkinMeshType;
	CComboBox m_comboWhereBack;
	afx_msg void OnCbnSelchangeComboObjtype();
	afx_msg void OnCbnSelchangeComboSkinmeshtype();
	afx_msg void OnCbnSelchangeComboWhereback();
	afx_msg void OnDeltaposSpinValue1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinValue2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinValue3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditValue1();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedCheckAtkmode();
	afx_msg void OnBnClickedButtonLinkInverseMatrix();
	afx_msg void OnBnClickedButtonLinkMatrix();
};
