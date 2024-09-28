#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharLine2BoneEff.h"
#include "EffCharPage.h"

// CEffLine2BonePage ��ȭ �����Դϴ�.

class CEffLine2BonePage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffLine2BonePage)

protected:
	EFFCHAR_PROPERTY_LINE2BONEEFF	m_Property; 

public:
	void SetProperty( EFFCHAR_PROPERTY *pProp = NULL );

	void UpdateLineLIST();

public:
	CEffLine2BonePage();
	virtual ~CEffLine2BonePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_LINE2BONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonGet1();
	afx_msg void OnBnClickedButtonGet2();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	CListCtrl m_ctrlLINE;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListLinebone(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSetting();
};
