#pragma once


// CEffEditBar ��ȭ �����Դϴ�.
class CsheetWithTab;
class CEffEditBar : public CDialog
{
	DECLARE_DYNAMIC(CEffEditBar)

public:
	CsheetWithTab*	m_psheetWithTab;

public:
	void SelectEffProp ( DWORD dwProp );

public:
	CEffEditBar(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CEffEditBar();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void PostNcDestroy();
};
