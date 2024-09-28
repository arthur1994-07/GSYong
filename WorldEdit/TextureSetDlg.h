#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CMaterialSetting;

// CTextureSetDlg 대화 상자입니다.

class CTextureSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CTextureSetDlg)

public:
	CTextureSetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTextureSetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TEXTURE_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

private:
	CMaterialSetting*	m_pMaterialSetting;

private:
	void SetFocus( CListCtrl& listReplace, int nIndex );

public:
	void SetMaterial( CMaterialSetting* pMaterialSetting ) { m_pMaterialSetting = pMaterialSetting; }

protected:
	void ReSetListCtrl();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_LCtrlTexDXT;
	afx_msg void OnBnClickedCheckCastShadow();
	afx_msg void OnNMClickListTexdxt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckCameraCollision();
	afx_msg void OnBnClickedCheckDecal();
	afx_msg void OnBnClickedRadioDecalNull();
	afx_msg void OnBnClickedRadioDecalAlphablend();
	afx_msg void OnBnClickedRadioDecalMultiply();
	afx_msg void OnBnClickedRadioDecalAdd();
	afx_msg void OnBnClickedRadioDecalOrder1();
	afx_msg void OnBnClickedRadioDecalOrder2();
	afx_msg void OnBnClickedRadioDecalOrder3();
	afx_msg void OnBnClickedRadioDecalAlphatest();
	afx_msg void OnBnClickedRadioLightMap();
	afx_msg void OnBnClickedRadioAmbientCube();
	afx_msg void OnBnClickedRadioVoxelColor();
	afx_msg void OnBnClickedCheckCubemap();
	afx_msg void OnEnChangeEditCubemapValue();
	afx_msg void OnBnClickedCheckMaskCubeMapSpecularMap();
	afx_msg void OnEnChangeEditSpotSpecPower();
	afx_msg void OnEnChangeEditSpotSpecMulti();
	afx_msg void OnBnClickedCheckNormalMap();
	afx_msg void OnBnClickedCheckDecalNormal();
};
