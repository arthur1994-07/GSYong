#pragma once


class CsheetWithTabTS;
struct EFFCHAR_PROPERTY_TS_NORMAL;
// CEffTS_NormalMap ��ȭ �����Դϴ�.

class CEffTS_NormalMap : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffTS_NormalMap)

protected:
	CsheetWithTabTS					*m_pSheetTab;
	EFFCHAR_PROPERTY_TS_NORMAL		m_PropNormalMap;

public:
	void SetSheetTab ( CsheetWithTabTS *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void SetProperty( EFFCHAR_PROPERTY_TS_NORMAL Property );
	EFFCHAR_PROPERTY_TS_NORMAL GetProperty() { return m_PropNormalMap; }
	void Apply();

public:
	CEffTS_NormalMap();
	virtual ~CEffTS_NormalMap();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFTS_NORMALMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonTextureNormal();
	afx_msg void OnBnClickedButtonTextureSpecular();
	afx_msg void OnBnClickedButtonTextureSpecular2();
	afx_msg void OnBnClickedCheckSpecular();
	afx_msg void OnBnClickedCheckSpecular2();
	afx_msg void OnBnClickedCheckGlonormap();
};
