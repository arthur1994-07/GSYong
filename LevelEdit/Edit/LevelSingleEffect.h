#pragma once

#include "../resource.h"
// CLevelSingleEffect 대화 상자입니다.
class CsheetWithTab;
struct DXLANDEFF;

class CLevelSingleEffect : public CPropertyPage
{
	DECLARE_DYNAMIC(CLevelSingleEffect)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	BOOL			m_bDlgInit;
	DXLANDEFF*		m_pLandEff;

public:
	static BOOL				g_bCheckViewNameEFFECT;

	// Ctrl + x, Ctrl + c, Ctrl + v
private:
	static DXLANDEFF*		g_pLandEff_Ctrl_CV;

public:
	void CutMobSch();
	void CopyMobSch();
	void PasteMobSch();

public:
	static void FinalCleanup_STATIC();

private:
	BOOL	m_bLayer;
	UINT	GetLayerIndex();

public:
	void ListtingLandEff ();
	void UpdateContrl ();
	void ViewContrls ( BOOL bView, BOOL bClear=FALSE );

	void ActiveSingleEffectIndex();
	void ActiveSingleEffectLayerIndex();

public:
	CLevelSingleEffect();
	virtual ~CLevelSingleEffect();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SINGLEEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CListBox m_EffListBox;
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonEffapply();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckViewName();
	virtual BOOL OnSetActive();
};
