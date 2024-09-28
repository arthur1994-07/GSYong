#pragma once
#include "afxwin.h"

#include "ClrButton.h"

#include <string>

class CsheetWithTab;
class DxTextureEffMan;
class DxTexEffBase;

struct TEXEFF_DIFFUSE_PROPERTY;
struct TEXEFF_FLOWUV_PROPERTY;
struct TEXEFF_ROTATE_PROPERTY;
struct TEXEFF_SPECULAR_PROPERTY;
struct TEXEFF_VISUAL_MATERIAL_PROPERTY;

// CPieceTexEffEdit 대화 상자입니다.

class CPieceTexEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CPieceTexEffEdit)

protected:
	CClrButton	cClrSpecular;
	CClrButton	cClrReflect;
	CClrButton	cClrBump;
	CClrButton	cClrGlow;
	CClrButton	cClrMultiTex;
	CClrButton	cClrDiffuseTex;

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
    void SelectTexEffList( const TSTRING& strName );

protected:
	DWORD	m_dwTexEffFlag;
	DWORD	m_dwTexDXTFlag;

protected:
	void ReSetListCtrl();
	void ReSetListEffects();
	void ChangeListCtrlDXT( int nIndex, DWORD dwFlag );
	void SetDXTFlag( DWORD dwFlag );
	DxTextureEffMan* GetTextureEffMan();
	BOOL GetCheckTexView( std::string& strName );

	void CreateSettingDialog( const DWORD dwType );
	void CreateDiffuseDlg( const DWORD dwType );
	void CreateDiffuse( const DWORD dwType, const char* pName, const TEXEFF_DIFFUSE_PROPERTY& sProp );
	void CreateFlowUVDlg( const DWORD dwType );
	void CreateFlowUV( const DWORD dwType, const char* pName, const TEXEFF_FLOWUV_PROPERTY& sProp );
	void CreateRotateDlg( const DWORD dwType );
	void CreateRotate( const DWORD dwType, const char* pName, const TEXEFF_ROTATE_PROPERTY& sProp );
	void CreateSpecularDlg( const DWORD dwType );
	void CreateSpecular( const DWORD dwType, const char* pName, const TEXEFF_SPECULAR_PROPERTY& sProp );
    void CreateVisualMaterialDlg( const DWORD dwType );
	void CreateVisualMaterial( const DWORD dwType, const char* pName, const TEXEFF_VISUAL_MATERIAL_PROPERTY& sProp );

	void StateSettingDialog( DxTexEffBase* pTexEff );
	void StateDiffuseDlg( const DWORD dwType, DxTexEffBase* pTexEff );
	void StateFlowUVDlg( const DWORD dwType, DxTexEffBase* pTexEff );
	void StateRotateDlg( const DWORD dwType, DxTexEffBase* pTexEff );
	void StateSpecularDlg( const DWORD dwType, DxTexEffBase* pTexEff );
    void StateVisualMaterialDlg( const DWORD dwType, DxTexEffBase* pTexEff );

public:
	CPieceTexEffEdit();
	virtual ~CPieceTexEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PIECE_TEXEFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioDefault();
	afx_msg void OnBnClickedRadioOpacity();
	afx_msg void OnBnClickedRadioHardAlpha();
	afx_msg void OnBnClickedRadioSoftAlpha();
	afx_msg void OnBnClickedRadioSoftAlpha2();
	afx_msg void OnBnClickedRadioSoftAlpha3();
	CListCtrl m_listTexView;
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnSetActive();
	afx_msg void OnLvnItemchangedListTexview(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPower();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonState();
	afx_msg void OnBnClickedButtonDelete();
	CComboBox m_comboTexEff;
	CListBox m_listTexEff;
    afx_msg void OnBnClickedCheckDmslm();
	afx_msg void OnBnClickedCheckDontLmSCast();
};
