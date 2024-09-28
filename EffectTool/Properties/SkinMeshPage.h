#pragma once

#include "../resource.h"

// CSkinMeshPage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct SKINMESH_PROPERTY;

class CSkinMeshPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkinMeshPage)

protected:
	CsheetWithTab*			m_pSheetTab;
	SKINMESH_PROPERTY*		m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CSkinMeshPage();
	virtual ~CSkinMeshPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFSKINMESH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonRead();
};
