#pragma once

#include "../DxTools/TextureManager.h"
#include "../DxTools/DxDynamicVB.h"
#include "./WaterSurfDef.h"

class DxSetLandMan;
class WaterSurfPlaneGroup;


class WaterSurfPointMan
{
private:
	enum { VERSION = 0x0100 };

private:
	TSTRING				m_strName;
	VEC_VECTOR3			m_vecWaterSurfPos[2];

    TextureResource     m_textureRes;

    UINT m_nVertexMax;
    DxDynamicVB::BIGVB m_sVertexBuffer;

private:
	WaterSurfPlaneGroup*	m_pWaterSurfPlaneGroup[2];

private:
	union
	{
		struct
		{
			WATERSURF_PROPERTY m_Property;
		};
		struct
		{
			BOOL	m_bPlaneInverse;			// 
			float	m_fPlaneWidth;				//	
			float	m_fPlaneHeight;				//	
			float	m_fSurfVelocityINIT;		// 초기 물살 이동 스피드
			float	m_fEdit_SurfAcceleration;	// [Edit] m_fSurfVelocity 줄어드는 수치
			float	m_fSurfSleepTimeINIT;		// 물살 잠복 시간
			float	m_fAlphaAddition;			// 알파 추가 스피드
			float	m_fAlphaSubtractionTime;	// 알파 삭제 시간
			float	m_fAlphaSubtraction;		// 알파 삭제 스피드
			float	m_fSideLineInterval;		// 파도의 옆라인이 나오는 시간간격
		};
	};

public:
	friend class CWaterSurfPage;

public:
	void Create( DxSetLandMan* pSetLandMan, D3DXVECTOR3 vPos, float fRadius, BOOL bLeft );
	void Create( DxSetLandMan* pSetLandMan, BOOL bFirst, VEC_VECTOR3& vecWaterSurfPos, D3DXVECTOR3* pBasePos, const D3DXVECTOR3& vPos, D3DXVECTOR3 vDir, float fRadius );
	void PlaneRemake( DxSetLandMan* pSetLandMan );
	void ResetData();
	void AddHeight( float fHeight );
	void MoveCamera( LPDIRECT3DDEVICEQ pd3dDevice );
    void SetObjRotate90( const D3DXMATRIX& matRotate );
    void CountVertexMax();
    TSTRING GetName() { return m_strName; }

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnDestroyDevice();
    void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
    void OnLostDevice();
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, float fSize, DWORD dwColor );

public:
	void Save( sc::SerialFile &SFile );
	void Load( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	void Import( LPDIRECT3DDEVICEQ pd3dDevice, const WaterSurfPointMan* pSrc );

private:
	void CleanUp();

public:
	WaterSurfPointMan();
	~WaterSurfPointMan();
};