#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectLightMap.h"

// CLightMapEffEdit 대화 상자입니다.

class CLightMapEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CLightMapEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectLightMap		*m_pLightMapEff;
	
	LIGHTMAP_PROPERTY		m_LightMapProperty;
	LIGHTMAP_PROPERTY		m_OldLightMapProperty;

public:
	void SetInstance ( DxEffectLightMap *pLightMapEff );

public:
	CLightMapEffEdit();
	virtual ~CLightMapEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FOG };		// 임시다.. 안쓰기 때문에 이렇게 넣었다.

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
