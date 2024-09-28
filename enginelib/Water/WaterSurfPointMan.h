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
			float	m_fSurfVelocityINIT;		// �ʱ� ���� �̵� ���ǵ�
			float	m_fEdit_SurfAcceleration;	// [Edit] m_fSurfVelocity �پ��� ��ġ
			float	m_fSurfSleepTimeINIT;		// ���� �ẹ �ð�
			float	m_fAlphaAddition;			// ���� �߰� ���ǵ�
			float	m_fAlphaSubtractionTime;	// ���� ���� �ð�
			float	m_fAlphaSubtraction;		// ���� ���� ���ǵ�
			float	m_fSideLineInterval;		// �ĵ��� �������� ������ �ð�����
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