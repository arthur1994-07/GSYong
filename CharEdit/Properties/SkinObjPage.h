#pragma once


// CSkinObjPage ��ȭ �����Դϴ�.
class CsheetWithTabChar;
class CsheetWithTabAnim;

class CSkinObjPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkinObjPage)

protected:
	CsheetWithTabChar*	m_pSheetTabParent;
	
public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabParent = pSheetTab; }

protected:
	CsheetWithTabAnim*	m_pSheetTabAnim;

public:
	CsheetWithTabAnim*	GetAnimationTab ()	{	return m_pSheetTabAnim; }
	BOOL LoadSkinObject( const char *pszFileName );

public:
	CSkinObjPage();
	virtual ~CSkinObjPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SKINOBJPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonNew();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonSave();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedButtonSkinfile();
	afx_msg void OnBnClickedButtonBonefile();
	afx_msg void OnBnClickedButtonMakeobj();
};
