#pragma once
#include "afxcmn.h"

#include "../../EngineLib/DxEffect/TexEff/DxTexEffVisualMaterial.h"

// CTexVisualMaterialDlg 대화 상자입니다.

class CTexVisualMaterialDlg : public CDialog
{
	DECLARE_DYNAMIC(CTexVisualMaterialDlg)

public:
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

public:
    TSTRING                     m_strFX;
    TSTRING                     m_strTextureSRC[MAX_USE_TEXTURE_VM];
    TSTRING                     m_strTextureDEST[MAX_USE_TEXTURE_VM];
    DWORD                       m_dwUsedTexture;
    MAP_PARAMETER_NAME_DATA2    m_mapParameter;

private:
    void ShowWindowDlg();
    void OnBnClickedButtonOpenTexture( DWORD i );
    void OnBnClickedButtonResetTexture( DWORD i );
    void UpdateLisCtrl( int nSelectIndex );
    void GetPARAMETER_NAME_DATA_IntoDialog( TSTRING& strParameterName, ParameterData& sData );

public:
	CTexVisualMaterialDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTexVisualMaterialDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TEX_VISUAL_MATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonVmFile();
    virtual BOOL OnInitDialog();
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
    afx_msg void OnLvnItemchangedListParameter(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonParameterInsert();
    afx_msg void OnBnClickedButtonParameterModify();
    afx_msg void OnBnClickedButtonParameterDelete();
    afx_msg void OnBnClickedCheckNight();
    CListCtrl m_ctrlParameter;
};
