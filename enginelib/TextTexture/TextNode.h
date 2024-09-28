/**
 * \date	2005/10/06
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "../GUInterface/UIDataType.h"

class CD3DFontX;

class CTextPart
{
public:
	BOOL		m_bValid;
	INT			m_iLength;
	TCHAR*		m_strText;
	INT			m_iWidth;

	BOOL		m_bTemp;
	BOOL		m_bDelete;
	BOOL		m_bCreate;

	RECT		m_rtTBound;
	UIVERTEX	m_stVertex[ 4 ];

	CD3DFontX*	m_pFont;

public:
	CTextPart*	m_pLeft;
	CTextPart*	m_pRight;

public:
	CTextPart();
	virtual ~CTextPart();

	VOID CreateText( const TCHAR* strText, CD3DFontX* pFont );
	VOID DrawText( HDC hDC, DWORD dwFlags );
	VOID DeleteText();
	VOID SetTextureUV();

	VOID DrawText( LPDIRECT3DTEXTUREQ pTexture, D3DCOLOR dwColor, FLOAT& fX, FLOAT fY );
	VOID DrawText( D3DCOLOR dwColor, FLOAT& fX, FLOAT fY );
    VOID DrawText( D3DCOLOR dwColor, FLOAT& fX, FLOAT fY, FLOAT fZoom );

	DWORD GetFontFlags();
};