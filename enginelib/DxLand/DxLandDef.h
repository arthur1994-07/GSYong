#ifndef DXLANDDEF_H_
#define DXLANDDEF_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include <vector>
#include <map>
#include "../../InternalCommonLib/dxstdafx.h"

namespace sc {
    class SerialFile;
}

enum  EMLANDMAN
{
    //	MAXLANDSID		= 32, 
    //	MAXLANDMID		= 256,
    //	MAXLANDSID		= 256,

    MAXLANDNAME		= 128,	

    MAX_LAND_MID	= 4096,	// 한 서버 내에서 생성 될 수 있는 LandManager 최대 갯수;
    MAX_LAND_SID	= 16,		// LandManager가 가질 수 있는 최대 LevelFile 갯수;
};

struct SLAND_FILEMARK_000	
{
	DWORD	dwNAVI_MARK;	//	네비게이션 메시.
	DWORD	dwGATE_MARK;	//	출입구.
	DWORD	dwCOLL_MARK;	//	충돌맵.
};

struct SLAND_FILEMARK_100	//	데이터 포맷 변경 진행
{
	DWORD	dwNAVI_MARK;	//	네비게이션 메시.
	DWORD	dwGATE_MARK;	//	출입구.
	DWORD	dwCOLL_MARK;	//	충돌맵.
	DWORD	dwWEATHER_MARK;	//	날씨.
};

struct SLAND_FILEMARK
{
	enum { VERSION = 0x0101, };

	DWORD	dwNAVI_MARK;	//	네비게이션 메시.
	DWORD	dwWEATHER_MARK;	//	날씨.
	DWORD	dwGATE_MARK;	//	출입구.
	DWORD	dwCOLL_MARK;	//	충돌맵.

	SLAND_FILEMARK()
		: dwNAVI_MARK(0)
		, dwGATE_MARK(0)
		, dwCOLL_MARK(0)
		, dwWEATHER_MARK(0)
	{
	}

	void Assign(SLAND_FILEMARK_000 &sOldMark);
	void Assign(SLAND_FILEMARK_100 &sOldMark);

	BOOL LoadSet(sc::SerialFile& SFile);
	BOOL SaveSet(sc::SerialFile& SFile);
};

enum EM_LAND_TYPE
{
	EMLR_DAYNIGHT_ON = 0,	// 낮밤이 있는 옥트리
	EMLR_DAYNIGHT_OFF = 1,	// 낮밤이 없는 옥트리
	EMLR_PIECE = 2,			// Piece Object
};

enum AABB_BINARY_TREE_TYPE
{
	ABTT_OCTREE		= 0,
	ABTT_LIGHTMAP	= 1,
	ABTT_MAT		= 2,
};

struct NAME_LMID
{
	TSTRING	m_strMaterialName;	// Material 이름
	DWORD   m_dwLightMapID;		// m_strName 것과는 틀린 라이트맵 ID
	TSTRING m_strTextureName;	// 텍스쳐 이름
    
    bool operator < ( const NAME_LMID& rhs ) const
    {
        if ( std::make_pair(m_strMaterialName,m_dwLightMapID) < std::make_pair(rhs.m_strMaterialName,rhs.m_dwLightMapID) )
            return true;
        else
            return false;
    }

    bool operator != ( const NAME_LMID& rhs ) const
    {
        if ( std::make_pair(m_strMaterialName,m_dwLightMapID) != std::make_pair(rhs.m_strMaterialName,rhs.m_dwLightMapID) )
            return true;
        else
            return false;
    }

	NAME_LMID( const TSTRING& strMaterialName, DWORD dwLightMapID, const TSTRING& strTextureName )
		: m_strMaterialName(strMaterialName)
		, m_dwLightMapID(dwLightMapID)
		, m_strTextureName(strTextureName)
	{
	}
};

class DxSingleTexMesh;
typedef DxSingleTexMesh* PDXSINGLETEXMESH;
typedef std::map<NAME_LMID,DxSingleTexMesh*>					MAPSINGLETEXMESH;
typedef std::map<NAME_LMID,DxSingleTexMesh*>::iterator			MAPSINGLETEXMESH_ITER;
typedef std::map<NAME_LMID,DxSingleTexMesh*>::const_iterator	MAPSINGLETEXMESH_CITER;

class DxSingleMaterialMesh;
typedef std::map<TSTRING,DxSingleMaterialMesh*>		MAPSINGLEMATERIALMESH;
typedef MAPSINGLEMATERIALMESH::iterator				MAPSINGLEMATERIALMESH_ITER;
typedef MAPSINGLEMATERIALMESH::const_iterator		MAPSINGLEMATERIALMESH_CITER;

typedef std::multimap<TSTRING,DxSingleMaterialMesh*>	MMAPSINGLEMATERIALMESH;
typedef MMAPSINGLEMATERIALMESH::iterator				MMAPSINGLEMATERIALMESH_ITER;
typedef MMAPSINGLEMATERIALMESH::const_iterator			MMAPSINGLEMATERIALMESH_CITER;

typedef std::vector<DxSingleMaterialMesh*>			VECSINGLEMATERIALMESH;
typedef VECSINGLEMATERIALMESH::iterator				VECSINGLEMATERIALMESH_ITER;
typedef VECSINGLEMATERIALMESH::const_iterator		VECSINGLEMATERIALMESH_CITER;

class DxTexEffBase;
typedef std::vector<std::tr1::shared_ptr<DxTexEffBase>> VEC_TEXEFF;
typedef VEC_TEXEFF::iterator                            VEC_TEXEFF_ITER;
typedef VEC_TEXEFF::const_iterator                      VEC_TEXEFF_CITER;
typedef std::map<TSTRING,VEC_TEXEFF>                    MAP_VECTEXEFF;
typedef MAP_VECTEXEFF::iterator                         MAP_VECTEXEFF_ITER;
typedef MAP_VECTEXEFF::const_iterator                   MAP_VECTEXEFF_CITER;

#endif // DXLANDDEF_H_