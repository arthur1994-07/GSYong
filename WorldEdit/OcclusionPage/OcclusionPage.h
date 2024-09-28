#pragma once

class CsheetWithTab;

// COcclusionPage ��ȭ �����Դϴ�.

class COcclusionPage : public CPropertyPage
{
	DECLARE_DYNAMIC(COcclusionPage)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	COcclusionPage();
	virtual ~COcclusionPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_OCCLUSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedButtonViewSimulationPosition();
	afx_msg void OnBnClickedButtonBake();
	afx_msg void OnBnClickedButtonBakeThisPlace();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonViewOctreeBox();
	afx_msg void OnBnClickedCheckField();
	afx_msg void OnBnClickedCheckPiece();
	afx_msg void OnBnClickedCheckChar();
};
