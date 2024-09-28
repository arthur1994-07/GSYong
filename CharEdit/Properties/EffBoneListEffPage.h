#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharBoneListEff.h"
#include "EffCharPage.h"

// CEffBoneListEffPage 대화 상자입니다.

class CEffBoneListEffPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffBoneListEffPage)

private:
	D3DXMATRIX	m_matWorld;

private:
	void ResetEditBox();			// Enable, Disable    Check!!
	void ResetBoneList( int nCur );
	void CheckCurserMRS();

protected:
	EFFCHAR_PROPERTY_BONELISTEFF	m_Property; 

public:
	void SetProperty( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffBoneListEffPage();
	virtual ~CEffBoneListEffPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_BONELISTEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	CListBox m_listBone;
	afx_msg void OnBnClickedCheckPosMove();
	afx_msg void OnBnClickedCheckSequence2();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonTextureFind();
	virtual BOOL OnKillActive();
	afx_msg void OnLbnSelchangeListBone();
	afx_msg void OnBnClickedCheckRotate();
	afx_msg void OnBnClickedRadioAbsoluteno2();
	afx_msg void OnBnClickedRadioAbsolute2();
	afx_msg void OnBnClickedCheckRemain();
    afx_msg void OnBnClickedCheckCreateRandomDontMove();
};
