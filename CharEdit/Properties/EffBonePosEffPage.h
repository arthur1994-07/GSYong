#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharBonePosEff.h"
#include "EffCharPage.h"

// CEffBonePosEffPage ��ȭ �����Դϴ�.

class CEffBonePosEffPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffBonePosEffPage)

private:
	EFFCHAR_PROPERTY_BONEPOSEFF	m_Property;

private:
	D3DXMATRIX	m_matWorld;

private:
	void ResetEditBox();	// Enable, Disable    Check!!
	void CheckCurserMRS();	// Ŀ���� Move, Rotate, Scale ��.

public:
	void SetProperty( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffBonePosEffPage();
	virtual ~CEffBonePosEffPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_BONEPOSEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonGetbonename();
	afx_msg void OnBnClickedCheckPosMove();
	afx_msg void OnBnClickedCheckSequence2();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonTextureFind();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckRotate2();
	afx_msg void OnBnClickedCheckRemain2();
};
