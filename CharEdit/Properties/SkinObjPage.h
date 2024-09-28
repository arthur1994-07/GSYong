#pragma once


// CSkinObjPage 대화 상자입니다.
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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SKINOBJPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
