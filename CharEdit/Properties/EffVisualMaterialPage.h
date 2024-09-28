#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharVisualMaterial.h"
#include "EffCharPage.h"

// CEffVisualMaterialPage 대화 상자입니다.

class CEffVisualMaterialPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffVisualMaterialPage)

private:
    enum
    {
        MAX_USE_TEXTURE_VM = 7,
    };

private:
    DWORD m_dwStatic[MAX_USE_TEXTURE_VM];
    DWORD m_dwSrcTexture[MAX_USE_TEXTURE_VM];
    DWORD m_dwDestTexture[MAX_USE_TEXTURE_VM];
    DWORD m_dwOpenTexture[MAX_USE_TEXTURE_VM];
    DWORD m_dwResetTexture[MAX_USE_TEXTURE_VM];

protected:
	EFFCHAR_PROPERTY            m_Property;
    VEC_PROPERTY                m_vecProperty;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );
    void ResetDialog() { VmfxReload_DialogReset(); }
    void VmfxReload_DialogReset();

    virtual BOOL OnSetActive();
    virtual BOOL OnKillActive();

private:
    void OnBnClickedButtonOpenTexture( DWORD i );
    void OnBnClickedButtonResetTexture( DWORD i );
    void ShowWindowDlg( const TCHAR* pNameFX, const vm::VEC_TEXTURE_RESOURCE& vecTextureResource );
    void ShowWindowDlg( int nIndex, BOOL bShowWindow, const TCHAR* pTextureSRC, const TCHAR* pTextureDEST );
    void SelectMaterial( int nIndex );
    void UpdateLisCtrl( int nSelectIndex );
    void GetPARAMETER_NAME_DATA_IntoDialog( TSTRING& strParameterName, ParameterData& sData );
	int GetCurSel_ComboMatrials();

public:
	CEffVisualMaterialPage();
	virtual ~CEffVisualMaterialPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_Eff_VISUAL_MATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonVmFile();
    afx_msg void OnBnClickedButtonColor1();
    afx_msg void OnBnClickedButtonColor2();
    afx_msg void OnBnClickedButtonOpenTexture0();
    afx_msg void OnBnClickedButtonOpenTexture1();
    afx_msg void OnBnClickedButtonOpenTexture2();
    afx_msg void OnBnClickedButtonOpenTexture3();
    afx_msg void OnBnClickedButtonOpenTexture4();
    afx_msg void OnBnClickedButtonOpenTexture5();
    afx_msg void OnBnClickedButtonOpenTexture6();
    afx_msg void OnBnClickedButtonResetTexture0();
    afx_msg void OnBnClickedButtonResetTexture1();
    afx_msg void OnBnClickedButtonResetTexture2();
    afx_msg void OnBnClickedButtonResetTexture3();
    afx_msg void OnBnClickedButtonResetTexture4();
    afx_msg void OnBnClickedButtonResetTexture5();
    afx_msg void OnBnClickedButtonResetTexture6();
    virtual BOOL OnInitDialog();
    afx_msg void OnCbnSelchangeComboMatrial();
    CComboBox m_ComboMatrials;
    CListCtrl m_ctrlParameter;
    afx_msg void OnLvnItemchangedListParameter(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonParameterInsert();
    afx_msg void OnBnClickedButtonParameterModify();
    afx_msg void OnBnClickedButtonParameterDelete();
    afx_msg void OnBnClickedCheckNight();
};
