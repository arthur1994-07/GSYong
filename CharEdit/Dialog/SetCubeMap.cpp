// SetCubeMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "SetCubeMap.h"


// CSetCubeMap 대화 상자입니다.

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


// CSetCubeMap 메시지 처리기입니다.
