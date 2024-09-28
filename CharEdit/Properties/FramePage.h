#include "afxcmn.h"
#if !defined(AFX_FRAMEPAGE_H__0CE8C844_A8A7_4C85_A1FC_F11B281DC956__INCLUDED_)
#define AFX_FRAMEPAGE_H__0CE8C844_A8A7_4C85_A1FC_F11B281DC956__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FramePage.h : header file
//
enum EMFRAMEEDITTYPE
{
	EMFRAMEEDIT_CHAR	 = 0,
	EMFRAMEEDIT_ATTBONE  = 1,
	EMFRAMEEDIT_VEHICLE  = 2,
	EMFRAMEEDIT_SIZE
};

struct SFrame;
class CsheetWithTabFrame;
/////////////////////////////////////////////////////////////////////////////
// CFramePage dialog

class CFramePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFramePage)

protected:
	CsheetWithTabFrame	*m_pSheetTab;

	EMFRAMEEDITTYPE		m_emFrameEditType;

public:
	void SetSheetTab ( CsheetWithTabFrame *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void SetFrameEditType( EMFRAMEEDITTYPE emFrameEditType );

public:
	void ResetPage ();
	void SetPieceList ();
	void SetAttBoneList ();
	void SetVehiclePartList ();

	HRESULT InsertAttBone( const char *pszFileName );
	HRESULT InsertVehiclePart( const char *pszFileName );
	HRESULT InsertSkinPiece( const char *pszFileName );
	BOOL	InsertSkinEff( const char *pszFileName );
	BOOL	LoadSkinBone( const char *pszFileName ); 

// Construction
public:
	CFramePage();
	~CFramePage();

// Dialog Data
	//{{AFX_DATA(CFramePage)
	enum { IDD = IDD_DIALOG_FRAME };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	afx_msg void OnRclickTreeFrame(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CListCtrl m_PieceList;
#ifndef DEF_SKIN_SYNC
	afx_msg void OnBnClickedButtonPieceSelect();
#else
    afx_msg void OnBnClickedButtonAniEdit();
#endif
	afx_msg void OnBnClickedButtonPieceDeselect();
	afx_msg void OnBnClickedButtonSkinbone();
	afx_msg void OnBnClickedButtonEffSave();
	afx_msg void OnBnClickedButtonEffLoad();
	afx_msg void OnEnChangeEditSkinScale();
	afx_msg void OnBnClickedButtonScale();
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnBnClickedButtonEffName();
	float m_fSetYPos;
	afx_msg void OnBnClickedButtonSetHeight();
	afx_msg void OnBnClickedButtonEffBoardpos();
	afx_msg void OnBnClickedButtonSkinSetting();
	afx_msg void OnBnClickedButtonBoneSetting();
	//afx_msg void OnBnClickedButtonCollisionObject();
    afx_msg void OnNMClickListPiece(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDBLCLKListPiece(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonFrameDocument();
	afx_msg void OnBnClickedButtonWeaponScale2();
	afx_msg void OnBnClickedButtonBillboard();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEPAGE_H__0CE8C844_A8A7_4C85_A1FC_F11B281DC956__INCLUDED_)
