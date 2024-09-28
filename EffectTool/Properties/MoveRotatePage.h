#pragma once
#include "afxwin.h"
#include "../resource.h"

// CMoveRotatePage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct MOVEROTATE_PROPERTY;

// CMoveRotatePage ��ȭ �����Դϴ�.

class CMoveRotatePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMoveRotatePage)

protected:
	CsheetWithTab*			m_pSheetTab;
	MOVEROTATE_PROPERTY*		m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CMoveRotatePage();
	virtual ~CMoveRotatePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_MOVEROTATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonMove();
	afx_msg void OnBnClickedCheckInitrand();
	afx_msg void OnBnClickedCheckGotocenter();
	//afx_msg void OnBnClickedCheckParentRotate();
};
