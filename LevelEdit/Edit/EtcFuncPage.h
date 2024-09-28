#pragma once

#include "../resource.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

#include "afxwin.h"
#include "afxcmn.h"

// CEtcFuncPage 대화 상자입니다.
class CsheetWithTab;
class CEtcFuncPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEtcFuncPage)

protected:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bInit;

	

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

	D3DXVECTOR3		m_vMinLandPos;
	D3DXVECTOR3		m_vMaxLandPos;
	bool			m_bUseCircle;
	VEC_LANDEFF		m_vecLandEffect;

	BYTE			m_dwSelectNum;

public:
	CEtcFuncPage();   // 표준 생성자입니다.
	virtual ~CEtcFuncPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ETCFUNC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckLandeff();
	afx_msg void OnBnClickedCheckLimittime();
	afx_msg void OnBnClickedCheckConcam();
	virtual BOOL OnInitDialog();	
	afx_msg void OnBnClickedButtonConfirm();
	afx_msg void OnBnClickedButtonConcam();
	CComboBox m_comob_LandEff;
	CListCtrl m_list_LandEff;
	afx_msg void OnBnClickedButtonLandeff1();
	afx_msg void OnBnClickedButtonLandeff2();

public:
	void UpdatePage();
	void UpdateLandEffectList();
	void SetEtcFuncData();
	void SetEnableFunction( DWORD dwEtcFunc, BOOL bEnable );
	afx_msg void OnNMClickListLandeff(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListLandeff(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckCircle();
	afx_msg void OnBnClickedCheckReentrylimit();
};
