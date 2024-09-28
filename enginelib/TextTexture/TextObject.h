/**
 * \date	2005/10/06
 * \author	Jun-Hyuk Choi
 */
#pragma once

class CD3DFontX;
class CTextPart;
class CTextTexture;

class CTextObject
{
protected:
	INT				m_iLength;
	TCHAR*			m_strText;

	CTextPart*		m_pPart;
	CTextTexture*	m_pTexture;

	DWORD			m_dwLastDraw;
	BOOL			m_bDelete;

public:
	CTextObject();
	virtual ~CTextObject();

	VOID CreateTextPart( const TCHAR* strText, CD3DFontX* pFont );
	VOID FrameMove();
	VOID DrawText( D3DCOLOR dwColor, FLOAT fX, FLOAT fY );
    VOID DrawText( D3DCOLOR dwColor, FLOAT fX, FLOAT fY, FLOAT fZoom );

public:
	VOID SetLength( INT iLength )				{ m_iLength = iLength; }

	VOID SetTexture( CTextTexture* pTexture )	{ m_pTexture = pTexture; }

public:
	INT GetLength()								{ return m_iLength; }
	TCHAR* GetText()							{ return m_strText; }

	CTextPart* GetTextPart()					{ return m_pPart; }

	BOOL GetDelete()							{ return m_bDelete; }
};