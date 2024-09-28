#ifndef GLMAPAXISINFO_H_
#define GLMAPAXISINFO_H_

#pragma once

#include <string>

#include "../../SigmaCore/gassert.h"

class GLMapAxisInfo
{
protected:
	static std::string		m_strRootPath;

public:
	static void SetPath ( const char* szPath );

protected:
	std::string		m_strMapTexture;

	int				m_nMapSizeX, m_nMapSizeY;
	int				m_nMapStartX, m_nMapStartY;
	
	D3DXVECTOR2		m_vecTextureSize;
	float			m_fTextureMapPos[4];

	//	x:left, y:right, z:sizex, w:sizey
	D3DXVECTOR4		m_vecTexturePos;

public:
	bool LoadFile ( const char *szFile );

public:
	
	// 기존 퀘스트 코드에서 쓰는데  내용이 이해가 안감 
	void Convert2MapPos ( float fPosX, float fPosY, int &nMapPosX, int &nMapPosY ) const;
	void MapPos2MiniPos ( int nMapPosX, int nMapPosY, float &fPosX, float &fPosZ ) const;

	// 미니맵(예전LargeMap) 에서 위치값 얻어오는 함수 다시 구현(위 함수와 동일해보이지만, 구현은 다름)
	void ConvertWorldToMap(const D3DXVECTOR3& vPos, int& nPosX, int& nPosY );
	void ConvertMapToWorld(int nPosX,int nPosY,D3DXVECTOR3& vPos );


public:
	int GetMapSizeX () const				{ return m_nMapSizeX; }
	int GetMapSizeY () const				{ return m_nMapSizeY; }

	int GetMapStartX () const				{ return m_nMapStartX; }
	int GetMapStartY () const				{ return m_nMapStartY; }

	const char* GetMinMapTex () const		{ return m_strMapTexture.c_str(); }
	D3DXVECTOR2 GetMiniMapTexSize () const	{ return m_vecTextureSize; }
	D3DXVECTOR4 GetMiniMapTexPos () const	{ return m_vecTexturePos; }

public:
	GLMapAxisInfo ( GLMapAxisInfo &sVALUE )
	{
		GASSERT(0&&"GLMapAxisInfo ( GLMapAxisInfo &)");
	}

	GLMapAxisInfo& operator= (const GLMapAxisInfo &sVALUE )
	{
//		GASSERT(0&&"GLMapAxisInfo& operator= ( GLMapAxisInfo &)");
		m_strMapTexture		= sVALUE.m_strMapTexture;
		m_nMapSizeX			= sVALUE.m_nMapSizeX;
		m_nMapSizeY			= sVALUE.m_nMapSizeY;
		m_nMapStartX		= sVALUE.m_nMapStartX;
		m_nMapStartY		= sVALUE.m_nMapStartY;
		m_vecTexturePos		= sVALUE.m_vecTexturePos;
		m_vecTextureSize	= sVALUE.m_vecTextureSize;
		m_fTextureMapPos[0]	= sVALUE.m_fTextureMapPos[0];
		m_fTextureMapPos[1]	= sVALUE.m_fTextureMapPos[1];
		m_fTextureMapPos[2]	= sVALUE.m_fTextureMapPos[2];
		m_fTextureMapPos[3]	= sVALUE.m_fTextureMapPos[3];

		return *this;
	}

public:
	GLMapAxisInfo(void);
	~GLMapAxisInfo(void);
};

#endif // GLMAPAXISINFO_H_