#pragma once

#include "../resource.h"
#include "EffSheetWithTab.h"
#include "../../EngineLib/DxEffect/Single/DxEffectMesh.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "MeshTabControl.h"


// CMeshEffectPage ��ȭ �����Դϴ�.

class CsheetWithTab;
struct MESH_PROPERTY;

class CMeshEffectPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMeshEffectPage)


protected:
	CsheetWithTab*			m_pSheetTab;
	MESH_PROPERTY*			m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CMeshEffectPage();
	virtual ~CMeshEffectPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFMESH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:	
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonEditsound();
	CMeshTabControl m_tabTYPE;
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonPosreset();
};
