// SetCubeMap.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "SetCubeMap.h"


// CSetCubeMap ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSetCubeMap, CDialog)
CSetCubeMap::CSetCubeMap(CWnd* pParent /*=NULL*/)
	: CDialog(CSetCubeMap::IDD, pParent)
{
	memset(m_szCubeTex0, 0, sizeof(char) * MAX_PATH);
	memset(m_szCubeTex1, 0, sizeof(char) * MAX_PATH);
	memset(m_szCubeTex2, 0, sizeof(char) * MAX_PATH);
	memset(m_szCubeTex3, 0, sizeof(char) * MAX_PATH);
	memset(m_szCubeTex4, 0, sizeof(char) * MAX_PATH);
	memset(m_szCubeTex5, 0, sizeof(char) * MAX_PATH);
}

CSetCubeMap::~CSetCubeMap()
{
}

void CSetCubeMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetCubeMap, CDialog)
END_MESSAGE_MAP()


// CSetCubeMap �޽��� ó�����Դϴ�.
