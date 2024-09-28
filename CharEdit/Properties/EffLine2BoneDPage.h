#pragma once

#include "PieceRelativePage.h"
#include "../EngineLib/DxEffect/Char/DxEffCharLine2BoneEff.h"

// CEffLine2BoneDPage 대화 상자입니다.

class CEffLine2BoneDPage : public CPieceRelativePage
{
	DECLARE_DYNAMIC(CEffLine2BoneDPage)

protected:
	DxEffCharLine2BoneEff*			m_rEffChar;
	EFFCHAR_PROPERTY_LINE2BONEEFF*	m_rProperty;

public:
	void SetProperty( DxEffChar* pEffChar, EFFCHAR_PROPERTY *pProp );

public:
	CEffLine2BoneDPage();
	virtual ~CEffLine2BoneDPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_LINE2BONE_D };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonBack();
	afx_msg void OnBnClickedCheckSequence();
	afx_msg void OnBnClickedButtonColor3();
	afx_msg void OnBnClickedButtonTextureFind();
	afx_msg void OnBnClickedButtonColorOther();
	afx_msg void OnBnClickedRadioStartend();
	afx_msg void OnBnClickedRadioBlur();
	afx_msg void OnBnClickedRadioEach();
	afx_msg void OnBnClickedRadioNone();
	afx_msg void OnBnClickedButtonTextureFind2();
	afx_msg void OnBnClickedCheckCammove();
    afx_msg void OnBnClickedCheckAbsolute();
};
