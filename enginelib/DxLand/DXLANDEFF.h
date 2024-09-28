#ifndef DXLANDEFF_H_
#define DXLANDEFF_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../SigmaCore/File/BaseStream.h"
#include "../DxTools/DxCustomTypes.h"
#include "../DxTools/Collision.h"
#include "../DxEffect/Single/DxEffSinglePropGMan.h"
#include "../DxEffect/Single/DxEffSingle.h"

struct DXLANDEFF : public OBJAABB
{
public:
	enum
	{
		LANDEFF_MAXSZ	= 33,
	};

public:
	const static DWORD	VERSION;
	static DWORD		SIZE;

	static float		m_fTime;
	static float		m_fElapsedTime;

	static HRESULT FrameMove ( float fTime, float fElapsedTime )
	{
		m_fTime = fTime;
		m_fElapsedTime = fElapsedTime;
		return S_OK;
	}

public:
	char				m_szFileName[MAX_PATH];	//	
	char				m_szName[LANDEFF_MAXSZ];
	D3DXMATRIX			m_matWorld;				//
	D3DXVECTOR3			m_vMax;					//	AABB에 사용.
	D3DXVECTOR3			m_vMin;					//	AABB에 사용.
    float               m_fColorPowerDay;       // 낮 밝기
    float               m_fColorPowerNight;     // 밤 밝기
    bool                m_bRender;


	DxEffSinglePropGroupResource	m_sPropGroupRes;		//	
	SPDXEFFSINGLEGROUP				m_spSingleGroup;	//	

	DXLANDEFF*			m_pNext;

protected:
	HRESULT PlaySingleGroup ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	const char* GetEffName()	{ return m_szFileName; }
    const char* GetEffectName() { return m_szName; }
	void SetPosition( const D3DXVECTOR3& vPos )	{ m_matWorld._41 = vPos.x; m_matWorld._42 = vPos.y; m_matWorld._43 = vPos.z; }
	void ReSetAABBBox();

public:
	//! 승리한 클럽의 국기를 맵에 셋팅한다.
	//void SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert );

	// WorldBattle ( 난사군도 )
public:
	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );

	// Edit 에서만 쓰임.
public:
	HRESULT SetEffect(
        char* szName,
        const D3DXMATRIX &matWorld,
        const char* szFile,
        float fColorPowerDay,
        float fColorPowerNight,
        LPDIRECT3DDEVICEQ pd3dDevice);

public:
	HRESULT Load ( sc::BaseStream	&SFile );
	HRESULT Save ( sc::SerialFile	&SFile );

	HRESULT Load_0100 ( sc::BaseStream	&SFile );
	HRESULT Load_0101 ( sc::BaseStream	&SFile );
	HRESULT Load_0102 ( sc::BaseStream	&SFile );	//	데이터 포맷 변경 진행
    HRESULT Load_0103 ( sc::BaseStream	&SFile );	//	낮, 밤 Effect 세기 변경

public:
	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor ) const;

	//	Note : OBJAABB
	//
public:
	void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		vMax = m_vMax; vMin = m_vMin;
	}

	BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, vPos );
	}

	OBJAABB* GetNext ()		{ return m_pNext; }

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV );
	void RenderName( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DXLANDEFF ();
	~DXLANDEFF ();
};
typedef DXLANDEFF* PLANDEFF;

#endif // DXLANDEFF_H_