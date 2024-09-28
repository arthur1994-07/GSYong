#pragma once
#include "afxcmn.h"
#include "../resource.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

// CSkillTreePage ��ȭ �����Դϴ�.
class	CsheetWithTab;
class CSkillTreePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkillTreePage)

public:
	CSkillTreePage();
	virtual ~CSkillTreePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SKILLTREE };

protected:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;	

public:
	BOOL	UpdateTreeItem ();

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	SetPropertyData ( int CalledPage, SNATIVEID NativeID );
	void	CleanAllItem ();

protected:
	BOOL		m_bNativeIDInit;
	int			m_CalledPage;	
	SNATIVEID	m_NativeID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlItemTree;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
