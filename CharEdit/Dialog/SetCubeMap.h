#pragma once


// CSetCubeMap ��ȭ �����Դϴ�.

class CSetCubeMap : public CDialog
{
	DECLARE_DYNAMIC(CSetCubeMap)

public:
	char	m_szCubeTex0[MAX_PATH];
	char	m_szCubeTex1[MAX_PATH];
	char	m_szCubeTex2[MAX_PATH];
	char	m_szCubeTex3[MAX_PATH];
	char	m_szCubeTex4[MAX_PATH];
	char	m_szCubeTex5[MAX_PATH];

public:
	CSetCubeMap(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSetCubeMap();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CUBEMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
