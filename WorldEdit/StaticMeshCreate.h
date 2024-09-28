#pragma once

#include "afxwin.h"
#include "afxcmn.h"

#include "ClrButton.h"

#include "../EngineLib/DxEffect/Mesh/DxStaticPisMan.h"

class CsheetWithTab;
struct DxFrame;

// CStaticMeshCreate 대화 상자입니다.

class CStaticMeshCreate : public CPropertyPage
{
	DECLARE_DYNAMIC(CStaticMeshCreate)

protected:
	CClrButton	cClrFrame;
	CClrButton	cClrStatic;

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	void SetActiveEffectPage( DWORD TypeID, DxEffectBase* pEffectBase );
	void AddTreeFrameItem ( DxFrame *pframeCur, HTREEITEM hParent );
	void ListEffects( DxFrame *pDxFrame );

public:
	void SetTreeFrame( DxFrame *pFrameRoot );
	void SelectChangedTreeFrame();

public:
	void UpdatePage ();

protected:
	void ResetButton ();

public:
	CStaticMeshCreate();
	virtual ~CStaticMeshCreate();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MBE_CREATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonXfileLoad();
	afx_msg void OnBnClickedButtonEffState();
	afx_msg void OnBnClickedButtonEffDelete();
	afx_msg void OnBnClickedButtonEffInsert();
	afx_msg void OnBnClickedButtonSingleEff();
	CTreeCtrl m_TreeFrame;
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonFrameRender();
	afx_msg void OnBnClickedButtonStaticRender();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnBnClickedButtonTextureEff();
	CListBox m_listEff;
	CComboBox m_comboEff;
	afx_msg void OnBnClickedButtonPefLoad();
	afx_msg void OnBnClickedButtonPefSave();
	afx_msg void OnBnClickedButtonPieLoad();
	afx_msg void OnBnClickedButtonPieSave();
	afx_msg void OnBnClickedButtonMorph();
    afx_msg void OnBnClickedCheckAlphaBlend();
	afx_msg void OnBnClickedCheckAniPlayOnce();
	afx_msg void OnBnClickedCheckUnbakedLightmap();
	afx_msg void OnBnClickedButtonMeshState();
	afx_msg void OnBnClickedCheckRan10();
	afx_msg void OnBnClickedButtonResetRadius();
	afx_msg void OnEnChangeEditVisibleRadius();
	afx_msg void OnEnChangeEditInvisibleRadius();
	afx_msg void OnBnClickedButtonMaterialSetting();
};
