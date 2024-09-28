#pragma once

#include "../resource.h"

// CSkinMeshPage 대화 상자입니다.
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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFSKINMESH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonRead();
};
