#pragma once

#include "../../DxTools/TextureManager.h"

// texture 의 dds 이름은 chf 의 이름을 기준으로 사용된다.
class BILLBOARD_CHF_DATA
{
public:
	enum
	{
		VERSION = 0x100,
	};

private:
	float	m_fWidth;
	float	m_fHeight;
	float	m_fRightOffset;
	float	m_fUpOffset;
	DWORD	m_nTexSizeIndexWidth;	// 0-1, 1-2, 2-4, 3-8, 4-16, 5-32, 6-64, 7-128, 8-256, 9-512
	DWORD	m_nTexSizeIndexHeight;	// 0-1, 1-2, 2-4, 3-8, 4-16, 5-32, 6-64, 7-128, 8-256, 9-512

public:
	void Save( const char* szFile );
	BOOL Load( const char* szFile );

	friend class BILLBOARD_CHF;
	friend class CBillboardPage;

public:
	BILLBOARD_CHF_DATA();
};

class BILLBOARD_CHF
{
private:
	float	m_fWidth;
	float	m_fHeight;
	float	m_fRightOffset;
	float	m_fUpOffset;
	DWORD	m_dwTexSizeX;
	DWORD	m_dwTexSizeY;

	TSTRING m_strTexName;

	TextureResource				m_textureRes;

public:
	void Render( const D3DXVECTOR3 &vPos, float fBillboardAlpha );

	friend class DxSkinChar;

public:
	BILLBOARD_CHF( const BILLBOARD_CHF_DATA& sSrc, const TSTRING &strTexName );
};
