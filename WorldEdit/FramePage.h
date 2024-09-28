#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "../MfcExLib/ExLibs/CJFlatComboBox.h"

// CFramePage 대화 상자입니다.
struct DxFrame;
class CsheetWithTab;
class DxEffectBase;

class CFramePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFramePage)

protected:
	CsheetWithTab	*m_pSheetTab;

	BOOL			m_bDlgInit;

public:
	void ResetContent ();
	void UpdateContrl ();
	void UpdateCamColl();

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	void AddTreeFrameItem ( DxFrame *pframeCur, HTREEITEM hParent );
	void SetLightMapXY();

public:
	void SetTreeFrame ( DxFrame *pFrameRoot );
	void SelectChangedTreeFrame ();

public:
	void SetActiveEffectPage ( DWORD TypeID, DxEffectBase* pEffectBase );

	void ListEffects ( DxFrame *pDxFrame );
	bool LoadXFile ( const TCHAR* pFileName );

public:
	CFramePage();
	virtual ~CFramePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CTreeCtrl		m_TreeFrame;
	CCJFlatComboBox	m_ComboEffectTypes;
	CListBox		m_ListBoxEffect;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonXfileopen();
	afx_msg void OnNMRclickTreeFrame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonEffectchangeadd();
	afx_msg void OnTvnSelchangedTreeFrame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonEffectstate();
	afx_msg void OnBnClickedButtonEffectdelete();
	afx_msg void OnBnClickedButtonFramestate();
	afx_msg void OnBnClickedButtonReplaceSet();	
	afx_msg void OnEnChangeEditMapmainid();
	afx_msg void OnEnChangeEditMapsubid();
	afx_msg void OnBnClickedCheckNaviview();
	afx_msg void OnBnClickedButtonTextureSetting();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonTexeff();
	afx_msg void OnBnClickedSnowCheck();
	afx_msg void OnBnClickedRainCheck();
	afx_msg void OnBnClickedLeaveCheck();
	afx_msg void OnBnClickedCheckCamcoll();
protected:
    virtual void PostNcDestroy();
public:
    afx_msg void OnBnClickedButtonPieceTexVmView();
	afx_msg void OnEnChangeEditLightmapDetail();
	afx_msg void OnNMClickTreeFrame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonMinimap();
	afx_msg void OnBnClickedButtonOcclusionCulling();
	afx_msg void OnBnClickedButtonCubeMap();
};
