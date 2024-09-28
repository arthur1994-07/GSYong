#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectLightMap.h"

// CLightMapEffEdit ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_FOG };		// �ӽô�.. �Ⱦ��� ������ �̷��� �־���.

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
