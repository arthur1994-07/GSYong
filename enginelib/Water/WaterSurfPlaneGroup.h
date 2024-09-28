#pragma once

#include <vector>
#include "./WaterSurfDef.h"

class DxSetLandMan;
class WaterSurfPlane;


class WaterSurfPlaneGroup
{
private:
	enum { VERSION = 0x0100 };

private:
	typedef std::vector<WaterSurfPlane*>	VEC_WATERSURFPLANE;
	typedef VEC_WATERSURFPLANE::iterator	VEC_WATERSURFPLANE_ITER;

	VEC_WATERSURFPLANE	m_vecWaterSurfPlane;

public:
	void Create( DxSetLandMan* pSetLandMan, const WATERSURF_PROPERTY& sProp, const VEC_VECTOR3& vecWaterSurfPOS, int nPointNum );
	void ResetData( const WATERSURF_PROPERTY& sProp );
	void AddHeight( float fHeight );
    void SetObjRotate90( const D3DXMATRIX& matRotate );
    void CountVertexMax( UINT& nVertexMax );
    void CountVertexRender( UINT& nVertexMax );

public:
	void FrameMove( const WATERSURF_PROPERTY& sProp, float fElapsedTime );
	void Render( WATERSURFVERTEX*& pLocked, bool& bFirst );

public:
	void Save( sc::SerialFile &SFile );
	void Load( sc::SerialFile &SFile, const WATERSURF_PROPERTY& sProp );

	void Import( const WaterSurfPlaneGroup* pSrc, const WATERSURF_PROPERTY& sProp );

private:
	void CleanUp();

public:
	WaterSurfPlaneGroup();
	~WaterSurfPlaneGroup();
};
