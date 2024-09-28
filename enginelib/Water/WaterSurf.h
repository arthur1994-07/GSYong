#pragma once

#include <vector>

class DxSetLandMan;
class WaterSurfPointMan;
struct WATERSURFVERTEX;

namespace sc
{
	class SerialFile;
};

/**
 * \class	WaterSurf
 * \date	2008/02/23
 * \see		
 * \todo	
 */
class WaterSurf
{
private:
	enum { VERSION = 0x0100 };

private:
	typedef std::vector<WaterSurfPointMan*>	VEC_WATERSURFPOINTMAN;
	typedef VEC_WATERSURFPOINTMAN::iterator	VEC_WATERSURFPOINTMAN_ITER;

    LPDIRECT3DDEVICEQ m_pd3dDevice;
    LPDIRECT3DSTATEBLOCK9 m_pSavedSB;
	LPDIRECT3DSTATEBLOCK9 m_pDrawSB;

	VEC_WATERSURFPOINTMAN m_vecWaterSurfPointMan;
    D3DXMATRIX m_matIdentity;

	int m_nSELECT_INDEX;

public:
	friend class CWaterSurfPage;

public:
	void CreateSurf( LPDIRECT3DDEVICEQ pd3dDevice, DxSetLandMan* pSetLandMan, const D3DXVECTOR3& vPos, float fRadius, BOOL bLeft );
	void DeleteSurf( int nIndex );
	void MoveCamera( LPDIRECT3DDEVICEQ pd3dDevice, int nIndex );
	void ClearIndex();
    void SetObjRotate90( const D3DXMATRIX& matRotate );
    VEC_WATERSURFPOINTMAN& GetWaterPointMan() { return m_vecWaterSurfPointMan; }

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnDestroyDevice( LPDIRECT3DDEVICEQ pd3dDevice );
    void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
    void OnLostDevice();
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Save( sc::SerialFile &SFile );
	void Load( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	void Import( LPDIRECT3DDEVICEQ pd3dDevice, WaterSurf* pWaterSurf );

public:
	void CleanUp();

public:
	WaterSurf();
	~WaterSurf();
};
