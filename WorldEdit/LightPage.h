#pragma once
#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

#include "../MfcExLib/ExLibs/CJFlatComboBox.h"
#include "../MfcExLib/ExLibs/FlatEdit.h"

#include "../EngineLib/DxTools/Light/DxLightMan.h"
#include "afxcmn.h"

// CLightPage 대화 상자입니다.
class CsheetWithTab;

class CLightPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLightPage)

private:
	CsheetWithTab	*m_pSheetTab;
    float           m_fAttMax0;
    float           m_fAttMax1;
    float           m_fAttMax2;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void UpdateContrl ();
	void KeyBoard_Enter();
	void UpdatePageClickUP();
	void UpdatePageClickDOWN();

protected:
	void SetLightType ( D3DLIGHTTYPE Type );
	void ModifyLightData ( const DXLIGHT &light );
    void SetEndLineValue( const DXLIGHT* pLight );

protected:
	void ResetDialog( CString Text );
	void ResetDialog();
	DXLIGHT* GetCurLight();
	void ListingLights();

protected:
	BOOL			m_bDlgInit;
public:
	CLightPage();
	virtual ~CLightPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedCheckLightingenable();
	afx_msg void OnBnClickedCheckEnablelight();
	afx_msg void OnCbnSelchangeComboLighttype();
	afx_msg void OnBnClickedButtonDir();
	afx_msg void OnBnClickedButtonDiffuse();
	afx_msg void OnBnClickedButtonAmbient();
	afx_msg void OnBnClickedButtonSpecular();

	CSeparator m_ctrLightEditSep;
	CCJFlatComboBox m_ComboLightType;
	CListBox m_ListLight;
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckRangeRend();
	afx_msg void OnBnClickedCheckNightandday();
	afx_msg void OnBnClickedButtonToday();
	afx_msg void OnBnClickedButtonTonight();
	afx_msg void OnBnClickedCheckDaypointoff();
	afx_msg void OnBnClickedButtonTimeset();
	afx_msg void OnLbnSelchangeListLight();
	afx_msg void OnBnClickedCheckDspName();
	virtual BOOL OnSetActive();
    afx_msg void OnNMCustomdrawSliderAtt0(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawSliderAtt1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawSliderAtt2(NMHDR *pNMHDR, LRESULT *pResult);
    CSliderCtrl m_ctrlATT0;
    CSliderCtrl m_ctrlATT1;
    CSliderCtrl m_ctrlATT2;
    afx_msg void OnEnChangeEditRange();
    afx_msg void OnBnClickedButtonDiffuseLsc();
    afx_msg void OnBnClickedButtonAmbientLsc();
    afx_msg void OnEnChangeEditLmDdp();
    afx_msg void OnBnClickedCheckEnablePlNight();
	afx_msg void OnEnChangeEditOverLighting();
};
