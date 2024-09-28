#pragma once


// CRenderLight ��ȭ �����Դϴ�.

class CRenderLight : public CDialog
{
	DECLARE_DYNAMIC(CRenderLight)
public:
	BOOL	m_bNearLight;
	float	m_fSpeed;
	float	m_fColorPower;
	int		m_nRate;

public:
	CRenderLight(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRenderLight();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RENDERLIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditSpeed();
	afx_msg void OnEnChangeEditRate();
	afx_msg void OnEnChangeEditColorpower();
	afx_msg void OnBnClickedCheckNearlight();
};
