#pragma once

class CsheetWithTabTS;
// CEffTS_TexUpDown ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFTS_TEXUPDOWN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTextureGlowtex();
};
