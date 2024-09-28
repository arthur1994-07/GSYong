#pragma once

#include "../../RanLogic/Item/GLItem.h"
//#include "../../RanLogic/Item/GLItemMix.h"
#include "../../RanLogic/Product/GLProductRecipe.h"

// CItemMixData 대화 상자입니다.

class CItemMixData : public CDialog
{
	DECLARE_DYNAMIC(CItemMixData)
	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_ITEMMIXDATA };

public:
	SProductRecipe m_sProductRecipe;

public:
	CItemMixData(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemMixData();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual void OnOK();

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonMat01Edit();
	afx_msg void OnBnClickedButtonMat02Edit();
	afx_msg void OnBnClickedButtonMat03Edit();
	afx_msg void OnBnClickedButtonMat04Edit();
	afx_msg void OnBnClickedButtonMat05Edit();
	afx_msg void OnBnClickedButtonResultEdit();
	afx_msg void OnBnClickedButtonWspEdit();
};