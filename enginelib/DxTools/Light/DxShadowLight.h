#pragma once

#include <vector>

#include "../Collision.h"

struct SPOT_COLOR
{
	DWORD m_dwR;
	DWORD m_dwG;
	DWORD m_dwB;
	float m_fTime;
	BOOL m_bFlash;
};

struct DxShadowSpotLight
{
	enum
	{
		VERSION = 0x105,
	};

	// Game
	DWORD			m_dwID;					// LightID
	float			m_fAlpha;				// Point �� �Ǵ� ����.
	D3DLIGHTQ		m_sLight;				// Light ����
	D3DXMATRIX		m_matWorld;				// m_Light�� ����(��ġ,����)�� ������ �̸� Matrix �� ����� ���´�.
	D3DXVECTOR3		m_vPositionViewSpace;	// m_Light�� ����(��ġ,����)�� ������ �̸� ViewSpace���� ��ġ������ �������´�.
	D3DXVECTOR3		m_vDirectionViewSpace;	// m_Light�� ����(��ġ,����)�� ������ �̸� ViewSpace���� ��ġ������ �������´�.
	D3DXMATRIX		m_matCamera;			// CameraMatrix�� �̸� ����� ���´�.

	float			m_fIndexCoolTime;	// 
	DWORD			m_dwIndexCUR;
	std::vector<SPOT_COLOR>	m_vecColor;	// Color List
	float			m_fSinTime;
	float			m_fSinCycleTime;	// Save, Load
	float			m_fSinPower;		// Save, Load
	float			m_fSinAdd;			// Save, Load

	// Edit
	TSTRING			m_strName;		// �̸��� ���� �� �ֵ��� �Ѵ�.
	int             m_nAttenuation0;
	int             m_nAttenuation1;
	int             m_nAttenuation2;
	float			m_fAttEndLineValue;		// �������� �ε巴�� �ϱ����� ��ġ.
	float			m_fOverLighting;		// ������ ��� ���ٰ��ΰ�~?

	// Game�� Mesh�� ������ ���� ȣ���ϵ��� �Ѵ�.
	void PreRender( const D3DXMATRIX& matView );

	// Game
	void FrameMove( float fElapsedTime );

	// Edit
	BOOL			m_bEditCheck;	// Don`t Save, Load

	void SetDefault( const D3DXVECTOR3& vPos );
	void InsertColor( DWORD dwR, DWORD dwG, DWORD dwB );
	void DeleteColor( int nIndex );
	void ChageColor( int nIndex, DWORD dwR, DWORD dwG, DWORD dwB );
	void SetColorTime( int nIndex, float fTime );
	void SetFlash( int nIndex, BOOL bFlash );

	// Save & Load
	void	Save ( sc::SerialFile &SFile );
	void	Load ( sc::SerialFile &SFile, DWORD dwID );

	DxShadowSpotLight();
};

class DxShadowLightNode : public OBJAABB
{
private:
	D3DXVECTOR3 m_vMax;
	D3DXVECTOR3 m_vMin;
	DxShadowSpotLight* m_rShadowSpotLight;		// DxShadowLight �� m_vecShadowSpotLight �� ������ �Ѵ�.

	DxShadowLightNode*	m_pNext;	// Tree �� ����� ���� �۾�.

public:
	// NSShadowLight �� g_vecShadowSpotLight �� g_pTreeHead �� �����͸� �������ش�.
	void SetData( DxShadowSpotLight* pShadowSpotLight, DxShadowLightNode** ppTreeHead );

public:
	void	FrameMove( const float fElapsedTime ) override;

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
	virtual OBJAABB* GetNext ();

public:
	DxShadowLightNode();
	virtual ~DxShadowLightNode();
};
