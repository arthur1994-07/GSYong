#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "./GLMapAxisInfo.h"

#include "../../EngineLib/Common/GLTexFile.h"
#include "../../EngineLib/G-Logic/GLogic.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

std::string GLMapAxisInfo::m_strRootPath;

void GLMapAxisInfo::SetPath ( const char* szPath )
{
	m_strRootPath = szPath;
}

GLMapAxisInfo::GLMapAxisInfo(void)
{
}

GLMapAxisInfo::~GLMapAxisInfo(void)
{
}

bool GLMapAxisInfo::LoadFile ( const char *szFile )
{
	//미니맵 설정 파일 확장자로 변경
	CString	MapName;
	STRUTIL::ChangeExt(szFile,MapName,".mmp");
	
	std::string strFullPath = m_strRootPath + MapName.GetString();

	gltexfile cFILE;
	cFILE.reg_sep("\t ,");

	if (GLOGIC::UseEngineZipFile())
		cFILE.SetZipFile( GLOGIC::strMAP_ZIPFILE );

	if (!cFILE.open(strFullPath, false))
	{
		sc::writeLogError(
            sc::string::format(
                "GLMapAxisInfo::LoadFile %1%", MapName.GetString()));
		return false;
	}

	cFILE.getflag ( "MINIMAPNAME", 1, 1, m_strMapTexture, true );

	cFILE.getflag ( "MAPSIZE_X", 1, 2, m_nMapSizeX, true );
	cFILE.getflag ( "MAPSIZE_X", 2, 2, m_nMapStartX, true );

	cFILE.getflag ( "MAPSIZE_Y", 1, 2, m_nMapSizeY, true );
	cFILE.getflag ( "MAPSIZE_Y", 2, 2, m_nMapStartY, true );

	cFILE.getflag ( "TEXTURE_SIZE", 1, 2, m_vecTextureSize.x, true );
	cFILE.getflag ( "TEXTURE_SIZE", 2, 2, m_vecTextureSize.y, true );

	cFILE.getflag ( "TEXTURE_POS", 1, 4, m_fTextureMapPos[0], true );
	cFILE.getflag ( "TEXTURE_POS", 2, 4, m_fTextureMapPos[1], true );
	cFILE.getflag ( "TEXTURE_POS", 3, 4, m_fTextureMapPos[2], true );
	cFILE.getflag ( "TEXTURE_POS", 4, 4, m_fTextureMapPos[3], true );

	m_vecTexturePos = D3DXVECTOR4 ( m_fTextureMapPos[0]/m_vecTextureSize.x, m_fTextureMapPos[1]/m_vecTextureSize.y,
		m_fTextureMapPos[2]/m_vecTextureSize.x, m_fTextureMapPos[3]/m_vecTextureSize.y );

	return true;
}

void GLMapAxisInfo::Convert2MapPos ( float fPosX, float fPosY, int &nMapPosX, int &nMapPosY ) const
{
	int nOriginPosX = (int) floor(fPosX);
	int nOriginPosY = (int) floor(fPosY);

	// 월드 좌표를 컨버트
	int nCur_X = nOriginPosX - m_nMapStartX; 
	int nCur_Y = nOriginPosY - m_nMapStartY;

	nMapPosX = nCur_X / 50;
	nMapPosY = nCur_Y / 50;
}

void GLMapAxisInfo::MapPos2MiniPos ( int nMapPosX, int nMapPosY, float &fPosX, float &fPosZ ) const
{
	fPosX = nMapPosX * 50.0f + m_nMapStartX;
	fPosZ = nMapPosY * 50.0f + m_nMapStartY;
}

void GLMapAxisInfo::ConvertWorldToMap( const D3DXVECTOR3& vPos, int& nPosX, int& nPosY )
{
	//////////////////////////////////////////////////////////////////////////
	int worldStartX = GetMapStartX();
	int worldStartY = GetMapStartY();
	int worldSizeX =  GetMapSizeX();
	int worldSizeY =  GetMapSizeY();

	const D3DXVECTOR2 vMinimapTexSize = GetMiniMapTexSize();
	int mapTextureSizeX = static_cast<int>(vMinimapTexSize.x);
	int mapTextureSizeY = static_cast<int>(vMinimapTexSize.y);

	mapTextureSizeX = (mapTextureSizeX) ? mapTextureSizeX : 1;
	mapTextureSizeY = (mapTextureSizeY) ? mapTextureSizeY : 1;

	worldSizeX = (worldSizeX) ? worldSizeX : 1;
	worldSizeY = (worldSizeY) ? worldSizeY : 1;
	//////////////////////////////////////////////////////////////////////////


	//	NOTE
	//		월드 좌표의 기준을 재설정, Y값이 계산되는 것은 캐릭터가 진행하는 방향과
	//		맵이 진행하는 방향이 반대로 되어 있기 때문임.
	int nCur_X = static_cast<int>(vPos.x) - worldStartX;
	int nCur_Y = (worldSizeY-static_cast<int>(vPos.z)) + worldStartY;

	//	NOTE
	//		재정의된 월드 좌표를 맵 포지션으로 바꿈
	nPosX = static_cast<int>(mapTextureSizeX * nCur_X / worldSizeX);
	nPosY = static_cast<int>(mapTextureSizeY * nCur_Y / worldSizeY);
}

void GLMapAxisInfo::ConvertMapToWorld( int nPosX,int nPosY,D3DXVECTOR3& vPos )
{
	//////////////////////////////////////////////////////////////////////////
	int worldStartX = GetMapStartX();
	int worldStartY = GetMapStartY();
	int worldSizeX =  GetMapSizeX();
	int worldSizeY =  GetMapSizeY();

	const D3DXVECTOR2 vMinimapTexSize = GetMiniMapTexSize();
	int mapTextureSizeX = static_cast<int>(vMinimapTexSize.x);
	int mapTextureSizeY = static_cast<int>(vMinimapTexSize.y);

	mapTextureSizeX = (mapTextureSizeX) ? mapTextureSizeX : 1;
	mapTextureSizeY = (mapTextureSizeY) ? mapTextureSizeY : 1;

	worldSizeX = (worldSizeX) ? worldSizeX : 1;
	worldSizeY = (worldSizeY) ? worldSizeY : 1;
	//////////////////////////////////////////////////////////////////////////

	float fPosX = worldSizeX / mapTextureSizeX * (float)nPosX;
	float fPosY = worldSizeY / mapTextureSizeY  * (float)nPosY;

	vPos.x = fPosX + worldStartX;
	vPos.z = worldSizeY - ( fPosY - worldStartY );
}
