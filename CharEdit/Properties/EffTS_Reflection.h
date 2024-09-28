#pragma once


class CsheetWithTabTS;
struct EFFCHAR_PROPERTY_TS_REFLECTION;
// CEffTS_Reflection ��ȭ �����Դϴ�.

class CEffTS_Reflection : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffTS_Reflection)

protected:
	CsheetWithTabTS					*m_pSheetTab;
	EFFCHAR_PROPERTY_TS_REFLECTION	 m_PropReflection;

public:
	void SetSheetTab ( CsheetWithTabTS *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void SetProperty( EFFCHAR_PROPERTY_TS_REFLECTION Property );
	EFFCHAR_PROPERTY_TS_REFLECTION GetProperty() { return m_PropReflection; }
	void Apply();

public:
	CEffTS_Reflection();
	virtual ~CEffTS_Reflection();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFTS_REFLECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTextureReflecttex();
	afx_msg void OnBnClickedButtonTextureReflecttex2();
};
