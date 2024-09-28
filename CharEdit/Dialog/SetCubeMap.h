#pragma once


// CSetCubeMap 대화 상자입니다.

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
	CSetCubeMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetCubeMap();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUBEMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
