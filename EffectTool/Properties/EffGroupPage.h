#pragma once
#include "afxwin.h"
#include "../resource.h"
#include "../../EngineLib/DxEffect/Single/DxEffSingle.h"
#include "afxcmn.h"

// CEffGroupPage 대화 상자입니다.
class CsheetWithTab;

class CEffGroupPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffGroupPage)

protected:
	CsheetWithTab*		m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	static EFF_PROPGROUP		m_EffPropGroup;
	static D3DXMATRIX			m_TargetMatrix;

public:
	EFF_PROPERTY*				m_SaveProp;

protected:
	void			AddTreeEffGroupItem		( EFF_PROPERTY* pEffProp, HTREEITEM hParent );
	EFF_PROPERTY*	GetTreeEffGroupParent	( EFF_PROPERTY* pTargetProp, EFF_PROPERTY* pParentProp );
	EFF_PROPERTY*	GetTreeEffGroupSibling	( EFF_PROPERTY* pTargetProp, EFF_PROPERTY* pParentProp );

public:
	void SetCharAnimList ();
	void SetTreeEffGroup ();
	void SelectEffProp ( EFF_PROPERTY* pEffProp );
	void ActivePrppertyPage ( EFF_PROPERTY* pProp );
	void LoadEffPropGroup ( CString strPathName );

public:
	CEffGroupPage();
	virtual ~CEffGroupPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFGROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_ComboEffectTypes;
	CTreeCtrl m_treeEffectGroup;
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonEffstate();
	afx_msg void OnBnClickedButtonEffdel();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonMoveObj();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonPaste();
	afx_msg void OnBnClickedButtonTestTarget();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonAabbClear();
	afx_msg void OnBnClickedButtonEfftotexStart();
	afx_msg void OnBnClickedButtonEfftotexEnd();
	afx_msg void OnBnClickedCheckAutoAabbbox();
	afx_msg void OnBnClickedButtonRhApply();
	CListBox m_ListCharAnim;
	afx_msg void OnBnClickedButtonAnimSelect();
	afx_msg void OnBnClickedButtonChfLoad();
	afx_msg void OnNMRclickTreeEffgroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeEffgroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonTestAbort();
};
