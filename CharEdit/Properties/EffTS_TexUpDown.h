#pragma once

class CsheetWithTabTS;
// CEffTS_TexUpDown 대화 상자입니다.

class CEffTS_TexUpDown : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffTS_TexUpDown)

protected:
	CsheetWithTabTS				 *m_pSheetTab;
	EFFCHAR_PROPERTY_TS_TEXUPDOWN m_PropTexUpDown;

public:
	void SetSheetTab ( CsheetWithTabTS *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void SetProperty( EFFCHAR_PROPERTY_TS_TEXUPDOWN Property );
	EFFCHAR_PROPERTY_TS_TEXUPDOWN GetProperty() { return m_PropTexUpDown; }
	void Apply();

public:
	CEffTS_TexUpDown();
	virtual ~CEffTS_TexUpDown();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFTS_TEXUPDOWN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTextureGlowtex();
};
