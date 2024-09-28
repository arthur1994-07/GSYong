#pragma once

#include "../Resource.h"
#include "EffCharPage.h"

// CUserBackSlotPage 대화 상자입니다.

class CUserBackSlotPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CUserBackSlotPage)

private:
	D3DXMATRIX	m_matWorld;

private:
	void CheckCurserMRS();

public:
	void MouseStateLeftDOWN();

	virtual void SetProperty(EFFCHAR_PROPERTY *pProp = NULL)	{}
	virtual void UpdateKeyBoardMouse();

public:
	CUserBackSlotPage();
	virtual ~CUserBackSlotPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_USER_BACK_SLOT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGetbonename();
	afx_msg void OnBnClickedCheckPosMove();
	afx_msg void OnBnClickedCheckKeepSetting();
	afx_msg void OnBnClickedButtonSettingSave();
	afx_msg void OnBnClickedButtonSettingLoad();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedButtonOk();
};
