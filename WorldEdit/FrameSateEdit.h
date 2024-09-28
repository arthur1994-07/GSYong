#pragma once
#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

class CsheetWithTab;
struct DxFrame;

// CFrameSateEdit 대화 상자입니다.

class CFrameSateEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CFrameSateEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void SetInstance ( DxFrame *pFrame );

protected:
	DWORD		m_ActiveSubSet;
	DxFrame		*m_pThisFrame;

protected:
	void MeshStateEdit ( BOOL bEdit );

public:
	CFrameSateEdit();
	virtual ~CFrameSateEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FRAMESTATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CSeparator m_stListLine;
	CSeparator m_stMeshLine;
	CListBox m_ListMeshAttribs;
	CComboBox m_ComboMeshRendTypes;
	CSeparator m_stFrameStateLine;

	afx_msg void OnBnClickedButtonMeshstate();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonMeshstateOk();
	afx_msg void OnLbnSelchangeListMesh();
	virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboMeshstate();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonReplace();
	afx_msg void OnBnClickedCheckReplaceframe();
	CComboBox m_ComboObjType;
	CComboBox m_Combo_AniType;
    afx_msg void OnEnChangeEditLightmapUvIndex();
	afx_msg void OnEnChangeEditLmTextureSizeX();
	afx_msg void OnEnChangeEditLmTextureSizeY();
	afx_msg void OnBnClickedCheckEnableLightmap();
};
