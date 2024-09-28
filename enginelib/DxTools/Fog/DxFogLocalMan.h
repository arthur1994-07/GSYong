#pragma once

#include <vector>

class DxSkyBoxMan;

namespace CORE_COMMON
{
	class CSerialFile;
}

struct DxFogLocal
{
	enum
	{
		VERSION = 0x0105,
	};

	D3DXMATRIX	m_matWorld;
	float		m_fRange1;
	float		m_fRange2;
	float		m_fFogStart;
	float		m_fFogEnd;
	D3DXCOLOR	m_cFogColor;

	BOOL		m_bActiveLight;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cAmbient;

	DWORD		m_dwWeatherFlags;		// ISRAIN, ISSNOW, ISLEAVES, ISSPORE

	DxSkyBoxMan*	m_pSkyBoxMan;

	BOOL IsWeatherRain();
	BOOL IsWeatherLeaves();

	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pmatReflect );
	void Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice );

	void Save( sc::SerialFile &SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile );
	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxFogLocal* pSrc );

	DxFogLocal();
	~DxFogLocal();
};

class DxFogLocalMan
{
private:
	enum
	{
		VERSION = 0x0100,
	};

public:
	typedef std::vector<DxFogLocal*>	VEC_FOG_LOCAL;
	typedef VEC_FOG_LOCAL::iterator		VEC_FOG_LOCAL_ITER;

	VEC_FOG_LOCAL	m_vecFogLocal;

private:
	float		m_fRainRate;		// 비 렌더링 비율

	// Game
public:
	void FrameMove( float fElapsedTime,
					const D3DXVECTOR3& vPos, 
					float fFogStart, 
					float fFogEnd, 
					const D3DXCOLOR& cFogColor, 
					float& fFogStartOUT, 
					float& fFogEndOUT, 
					D3DXCOLOR& cFogColorOUT,
					BOOL ChangeDirectionLight );		// ChangeDirectionLight = Light셋팅이나 Fog의 Local 셋팅 때만 TRUE이다.

	void CalculateLight( const D3DXVECTOR3& vPos, D3DXVECTOR3& vDiffuseINOUT, D3DXVECTOR3& vAmbientINOUT );
	void CalculateLight( const D3DXVECTOR3& vPos, D3DXCOLOR& cDiffuseINOUT, D3DXCOLOR& cAmbientINOUT );
	void CalculateLightRate( const D3DXVECTOR3& vPos, FLOAT& fRateMax );

	void Render( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pmatReflect );
	void Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice );

	float GetRainRate()			{ return m_fRainRate; }

	// Edit
public:
	void Insert( const D3DXVECTOR3& vPos );
	void Modify( int nIndex, const D3DXVECTOR3& vPos, float fRange1, float fRange2, float fStart, float fEnd, const D3DXCOLOR& cColor );
	void Delete( int nIndex );
	void SetObjRotate90( const D3DXMATRIX& matRotate );

	// Edit Render
public:
	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice );

	// Save, Load
public:
	void Save( sc::SerialFile &SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile );
	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxFogLocalMan* pSrc );
	void CleanUp();

public:
	DxFogLocalMan();
	~DxFogLocalMan();
};

namespace NSFogLocalMan
{
	void SetFogLocalMan( DxFogLocalMan* pFogLocalMan );
	void CalculateLight( const D3DXVECTOR3& vPos, D3DXVECTOR3& vDiffuseINOUT, D3DXVECTOR3& vAmbientINOUT );
	void CleanUp();
}