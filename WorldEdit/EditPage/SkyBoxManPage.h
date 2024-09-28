#pragma once
#include "afxcmn.h"

class CsheetWithTab;

class DxSkyBoxMan;
class DxFogLocalMan;

// CSkyBoxManPage 대화 상자입니다.

class CSkyBoxManPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkyBoxManPage)

private:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

private:
	DxSkyBoxMan*	m_pSkyBoxMan;
	DxSkyBoxMan*	m_pSkyBoxManSRC;
	DxFogLocalMan*	m_rFogLocalMan;

public:
	void SetInstance( DxSkyBoxMan* pSkyBoxMan, DxSkyBoxMan* pSkyBoxManSRC, DxFogLocalMan* pFogLocalMan );

public:
	void UpdateSkyBoxList();

public:
	CSkyBoxManPage();
	virtual ~CSkyBoxManPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SKYBOXMAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlSKYBOX;
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListSkybox(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedRadioBlendAdd();
	afx_msg void OnBnClickedRadioBlendAlpha();
	afx_msg void OnBnClickedRadioBlendBase();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedRadioBlendLightShafts();
	afx_msg void OnBnClickedButtonLightShaftsApply();
	afx_msg void OnBnClickedEditLightShaftsDirSet();
};
