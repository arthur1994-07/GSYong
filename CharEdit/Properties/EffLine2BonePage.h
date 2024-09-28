#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharLine2BoneEff.h"
#include "EffCharPage.h"

// CEffLine2BonePage 대화 상자입니다.

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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_LINE2BONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
