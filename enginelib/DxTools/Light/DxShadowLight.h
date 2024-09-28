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
	float			m_fAlpha;				// Point 가 되는 세기.
	D3DLIGHTQ		m_sLight;				// Light 정보
	D3DXMATRIX		m_matWorld;				// m_Light에 정보(위치,방향)가 있지만 미리 Matrix 를 만들어 놓는다.
	D3DXVECTOR3		m_vPositionViewSpace;	// m_Light에 정보(위치,방향)가 있지만 미리 ViewSpace기준 위치정보를 만들어놓는다.
	D3DXVECTOR3		m_vDirectionViewSpace;	// m_Light에 정보(위치,방향)가 있지만 미리 ViewSpace기준 위치정보를 만들어놓는다.
	D3DXMATRIX		m_matCamera;			// CameraMatrix를 미리 만들어 놓는다.

	float			m_fIndexCoolTime;	// 
	DWORD			m_dwIndexCUR;
	std::vector<SPOT_COLOR>	m_vecColor;	// Color List
	float			m_fSinTime;
	float			m_fSinCycleTime;	// Save, Load
	float			m_fSinPower;		// Save, Load
	float			m_fSinAdd;			// Save, Load

	// Edit
	TSTRING			m_strName;		// 이름을 적을 수 있도록 한다.
	int             m_nAttenuation0;
	int             m_nAttenuation1;
	int             m_nAttenuation2;
	float			m_fAttEndLineValue;		// 끝라인을 부드럽게 하기위한 수치.
	float			m_fOverLighting;		// 몇배까지 밝게 해줄것인가~?

	// Game의 Mesh들 렌더링 전에 호출하도록 한다.
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
	DxShadowSpotLight* m_rShadowSpotLight;		// DxShadowLight 의 m_vecShadowSpotLight 을 참조만 한다.

	DxShadowLightNode*	m_pNext;	// Tree 를 만들기 위한 작업.

public:
	// NSShadowLight 의 g_vecShadowSpotLight 와 g_pTreeHead 의 데이터를 연결해준다.
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
