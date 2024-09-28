#pragma once

#include <boost/tr1/memory.hpp>
#include <vector>

#include "../../DxTools/DxVertexFVF.h"

namespace NSLIGHTMAPDEF
{
	enum LIGHTMAP_RENDERTYPE
	{
		LMRT_TEXTURE_LIGHTMAP	= 0,
		LMRT_LIGHTMAP			= 1,
		LMRT_LIGHTMAP_POINT		= 2,
		LMRT_CHECKER			= 3,
	};
};

class LightMapObject;

typedef std::vector<std::tr1::shared_ptr<LightMapObject>>	VEC_LIGHTMAPOBJ;
typedef VEC_LIGHTMAPOBJ::iterator							VEC_LIGHTMAPOBJ_ITER;


class SVEC_LIGHTMAPOBJ;
typedef std::vector<std::tr1::shared_ptr<SVEC_LIGHTMAPOBJ>>    VEC_SVEC_LIGHTMAPOBJ;
typedef VEC_SVEC_LIGHTMAPOBJ::iterator							VEC_SVEC_LIGHTMAPOBJ_ITER;


struct LIGHTMAPUV
{
	WORDWORD				 m_wwTextureSize;
	WORDWORD				 m_wwBaseTexelXY;
	std::vector<D3DXVECTOR2> m_vecUV;
};
typedef std::map<TSTRING,std::tr1::shared_ptr<LIGHTMAPUV>>	MAP_LIGHTMAPUV;
typedef MAP_LIGHTMAPUV::iterator							MAP_LIGHTMAPUV_ITER;
typedef MAP_LIGHTMAPUV::const_iterator						MAP_LIGHTMAPUV_CITER;
typedef MAP_LIGHTMAPUV::value_type							MAP_LIGHTMAPUV_VTYPE;

struct LIGHTMAPUV_MAIN
{
	MAP_LIGHTMAPUV	m_mapLightMapUV;
	DWORD			m_dwTextureWidth;
	DWORD			m_dwTextureHeight;
	DWORD			m_dwTextureID;

	LIGHTMAPUV_MAIN()
		: m_dwTextureWidth(0)
		, m_dwTextureHeight(0)
		, m_dwTextureID(0)
	{
	}
};

typedef std::tr1::shared_ptr<LIGHTMAPUV_MAIN>	SP_LIGHTMAPUV_MAIN;

typedef std::vector<SP_LIGHTMAPUV_MAIN>			VEC_LIGHTMAPUV_MAIN;
typedef VEC_LIGHTMAPUV_MAIN::iterator			VEC_LIGHTMAPUV_MAIN_ITER;
typedef VEC_LIGHTMAPUV_MAIN::const_iterator		VEC_LIGHTMAPUV_MAIN_CITER;
typedef VEC_LIGHTMAPUV_MAIN::value_type			VEC_LIGHTMAPUV_MAIN_VTYPE;


struct POSITION_TRIANGLE_LIGHT
{
	std::vector<D3DXVECTOR3>		m_vecPosition;
	std::vector<std::vector<DWORD>>	m_vecTriangle_vecLight;
};

typedef std::vector<POSITION_TRIANGLE_LIGHT>	VEC_POSITION_TRIANGLE_LIGHT;
typedef std::vector<POSITION_TRIANGLE_LIGHT>	VEC_POSITION_TRIANGLE_LIGHT_ITER;