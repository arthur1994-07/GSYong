#pragma once
#include "afxwin.h"

#include "../MfcExLib/ExLibs/reportctrl.h"
#include "../MfcExLib/ExLibs/Separator.h"

#include "../MfcExLib/ExLibs/CJFlatComboBox.h"
#include "../MfcExLib/ExLibs/FlatEdit.h"

#include "afxcmn.h"

// CShadowLightPage 대화 상자입니다.
class CsheetWithTab;
struct DxShadowSpotLight;

class CShadowLightPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CShadowLightPage)

private:
	CsheetWithTab	*m_pSheetTab;
    float           m_fAttMax0;
    float           m_fAttMax1;
    float           m_fAttMax2;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void UpdateContrl ();
	//void UpdatePageClickUP();
	//void UpdatePageClickDOWN();
	void UpdateKeyBoardMouse();

	// Light를 회전할 수가 있기 때문에 Direction 을 알맞게 변경해준다.
	void UpdateSpotLightDirection();

	// 새로생성된 것으로 인한 List 재 정리
	void UpdatePage_List_AddLastItem();

protected:
	void ModifyLightData ( const DxShadowSpotLight* pSpotLight );
    void SetEndLineValue( const D3DLIGHTQ* pLight );
	void UpdateColorList();

protected:
	void ResetDialog( DxShadowSpotLight* pSpotLight );
	void ResetDialog();
	DxShadowSpotLight* GetCurLight();
	void ListingLights();

protected:
	BOOL			m_bDlgInit;
public:
	CShadowLightPage();
	virtual ~CShadowLightPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SHADOW_LIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CReportCtrl	m_wndList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedCheckDirectionLight();
	afx_msg void OnBnClickedButtonDiffuse();

	CListBox m_ListLight;
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckRangeRend();
	afx_msg void OnLbnSelchangeListLight();
	virtual BOOL OnSetActive();
    afx_msg void OnNMCustomdrawSliderAtt0(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawSliderAtt1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawSliderAtt2(NMHDR *pNMHDR, LRESULT *pResult);
    CSliderCtrl m_ctrlATT0;
    CSliderCtrl m_ctrlATT1;
    CSliderCtrl m_ctrlATT2;
    afx_msg void OnEnChangeEditRange();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnEnChangeEditLightname();
	afx_msg void OnEnChangeEditTheta();
	afx_msg void OnEnChangeEditPhi();
	afx_msg void OnEnChangeEditFallOff();

	afx_msg void OnBnClickedButtonColorInsert();
	afx_msg void OnBnClickedButtonColorState();
	afx_msg void OnBnClickedButtonColorDelete();
	afx_msg void OnBnClickedButtonColorTimeApply();
	afx_msg void OnNMDblclkListColor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckFlash();
	afx_msg void OnBnClickedButtonSinApply();
	afx_msg void OnEnChangeEditPLEndLineLimit();
	afx_msg void OnEnChangeEditOverLighting();
	afx_msg void OnBnClickedButtonDirectionLightDocument();
};