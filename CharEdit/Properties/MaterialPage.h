#pragma once

#include "PieceRelativePage.h"

// CMaterialPage 대화 상자입니다.
struct DxSkinPiece;

class CMaterialPage : public CPieceRelativePage
{
	DECLARE_DYNAMIC(CMaterialPage)

protected:
	struct MATERIAL
	{
		std::string		strTexture;
		std::string		strTextureSel;

		DWORD			m_dwFlags;
		DWORD			dwZBias;
		DWORD			m_dwBlendFlag;

		MATERIAL ()
			: m_dwFlags(NULL)
			, dwZBias(0)
			, m_dwBlendFlag(0)
		{
		}
	};

	int			m_nMaterialSel;
	DWORD		m_dwMaterialNum;
	MATERIAL	*m_pMaterials;
	DxSkinPiece *m_pPiece;

public:
	void SetProperty ( DxSkinPiece *pPiece );
	void ReSetProperty ();

private:
	void SetTypeShowWindow();

public:
	CMaterialPage();
	virtual ~CMaterialPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CListBox m_listboxMaterial;
	afx_msg void OnLbnSelchangeListMaterialList();
	virtual BOOL OnInitDialog();

	// 사용 안하는 부분.
	CComboBox m_comboZBias;
	afx_msg void OnCbnSelchangeComboZbias();
	afx_msg void OnBnClickedButtonTextureSel();
	afx_msg void OnEnChangeEditTexture();
	afx_msg void OnBnClickedCheckShadow();
	afx_msg void OnBnClickedCheckAlphaBlendSrcDestAlpha();

	// Material System 으로 새로 사용되는 부분.
	afx_msg void OnBnClickedCubeMap();
	afx_msg void OnBnClickedNormalMap();
	afx_msg void OnBnClickedRadioBase();
	afx_msg void OnBnClickedRadioHair();
	afx_msg void OnBnClickedRadioCloth();
	afx_msg void OnBnClickedButtonHairSpecularColorDirection();
	afx_msg void OnBnClickedButtonHairSpecularColor();
	afx_msg void OnEnChangeHairShiftU();
	afx_msg void OnEnChangeHairShiftV();
	afx_msg void OnEnChangeHairSpecDirectionIntensity();
	afx_msg void OnEnChangeHairSpecExpDirection();
	afx_msg void OnEnChangeHairSpecExpCamera();
	afx_msg void OnBnClickedClothApply();
};
