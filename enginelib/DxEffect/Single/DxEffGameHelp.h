#pragma once

#include "./DxEffSingle.h"

enum GAMEHELP_EFFECTTYPE
{
	EMTARGET_EFFECT_NULL = 0,
	EMTARGET_EFFECT_DEFAULT,
	EMTARGET_EFFECT_ENEMY,
	EMTARGET_EFFECT_NPC,
	EMTARGET_EFFECT_ALLIANCE, 
	EMOVER_EFFECT,
	EMCLICK_EFFECT,
	EMMINE_EFFECT,
	EMSCOPE_CIRCLE_EFFECT,
	EMSCOPE_FANWISE_EFFECT,
};

struct FANWISE_PROPERTY;
class DxEffFanwise;
typedef std::tr1::shared_ptr<DxEffFanwise> SPDXEFFFANWISE;

struct PLANE_PROPERTY;
class DxEffPlane;
typedef std::tr1::shared_ptr<DxEffPlane> SPDXEFFPLANE;

struct CIRCLE_PROPERTY;
class DxEffCircle;
typedef std::tr1::shared_ptr<DxEffCircle> SPDXEFFCIRCLE;

class DxEffGameHelp
{
public:
	struct EFFECTFILENAMECAPS
	{
		std::string	strTargetEff_Default;
		std::string	strTargetEff_Circle_Enemy;
		std::string	strTargetEff_Circle_Npc;
		std::string	strTargetEff_Circle_Alliance;
		std::string	strTargetEff_Arrow_Enemy;
		std::string	strTargetEff_Arrow_Npc;
		std::string	strTargetEff_Arrow_Alliance;
		std::string strOverEff;
		std::string	strClickEff;
		std::string	strMineEff;
	};

	struct TEXTUREFILENAMECAPS
	{
		std::string	strScopeCircle;
		std::string	strScopeFanwise;
	};

private:
	typedef std::map<STARGETID,SPDXEFFSINGLEGROUP> MAP_MINEEFF;
	typedef MAP_MINEEFF::iterator				  MAP_MINEEFF_ITER;

public:
	DxEffGameHelp(void);
	~DxEffGameHelp(void);

	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	VOID	SetEffectFileName( const EFFECTFILENAMECAPS& sEffectFilenameCaps, 
		const TEXTUREFILENAMECAPS& sTextrueFilenameCaps ) 
	{ 
		m_sEffectFilenameCaps  = sEffectFilenameCaps;
		m_sTextrueFilenameCaps = sTextrueFilenameCaps;
	}

	void    RemoveAllEff();

	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	void	ResetOverEffect() { if( m_pOverEff ) m_pOverEff->ReStartEff(); }
	VOID	ResetClickEffect() { if( m_pClickEff ) m_pClickEff->ReStartEff(); }

	HRESULT UpdateGameHelpEffect(
        const GAMEHELP_EFFECTTYPE emEffectType,
        const D3DXMATRIX& matTrans,
        const STARGETID& sTargetID,
        const D3DXVECTOR3& vTargetSize = D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	HRESULT BlindEff ( const GAMEHELP_EFFECTTYPE emEffectType );
	HRESULT BlindEff ( const GAMEHELP_EFFECTTYPE emEffectType, const STARGETID &sTargetID );
	HRESULT BlindTargetEff ();
	HRESULT BlindScopeEff ();

	SPDXEFFSINGLEGROUP NewEffGroup ( DxEffSinglePropGroupResource spPropGroup, const D3DXMATRIX &matTrans, const STARGETID* pTargetID );
	SPDXEFFSINGLEGROUP NewEffGroup ( const char* szFileName, const D3DXMATRIX &matTrans, const STARGETID* pTargetID, BOOL bThread );

	SPDXEFFFANWISE NewFanwiseEff ( const FANWISE_PROPERTY &sProperty );
	SPDXEFFPLANE   NewPlaneEff ( const PLANE_PROPERTY &sProperty );
	SPDXEFFCIRCLE  NewCircleEff ( const CIRCLE_PROPERTY &sProperty );

	HRESULT UpdateTargetEffect( 
		const GAMEHELP_EFFECTTYPE emEffectType, 
		const STARGETID& sTargetID, 
		const D3DXMATRIX &matCircleTrans, 
		const D3DXMATRIX &matArrowTrans,
		const D3DXVECTOR3& vTargetSize = D3DXVECTOR3(1.0f, 1.0f, 1.0f));

	HRESULT UpdateScopePlaneEffect( const PLANE_PROPERTY &sProperty, const CIRCLE_PROPERTY &sPropertyOutLine, const D3DXMATRIX &matTrans );
	HRESULT UpdateFanwiseEffect( const FANWISE_PROPERTY &sProperty, const D3DXMATRIX &matTrans );

private:
	LPDIRECT3DSTATEBLOCK9 m_pScopeRenderStats;

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	SPDXEFFSINGLEGROUP  m_pTargetEff_Default;
	SPDXEFFSINGLEGROUP  m_pTargetEffCircle_Enemy;
	SPDXEFFSINGLEGROUP  m_pTargetEffArrow_Enemy;
	SPDXEFFSINGLEGROUP  m_pTargetEffCircle_Npc;
	SPDXEFFSINGLEGROUP  m_pTargetEffArrow_Npc;
	SPDXEFFSINGLEGROUP  m_pTargetEffCircle_Alliance;
	SPDXEFFSINGLEGROUP  m_pTargetEffArrow_Alliance;
	SPDXEFFSINGLEGROUP  m_pOverEff;
	SPDXEFFSINGLEGROUP  m_pClickEff;
	MAP_MINEEFF		    m_mapMineEff;

	SPDXEFFPLANE		m_pScopeEffCircle;
	SPDXEFFCIRCLE		m_pScopeEffCircle_OutLine;
	SPDXEFFFANWISE		m_pScopeEffFanwise;

	GAMEHELP_EFFECTTYPE	m_emTargetEffRender;
	BOOL				m_bOverEffRender;
	BOOL				m_bClickEffRender;
	BOOL				m_bScopeEffCircle;
	BOOL				m_bScopeEffFanwise;

	D3DXVECTOR3			m_vSelectTargetSize;
	D3DXMATRIX			m_matScopeCircle;
	D3DXMATRIX			m_matScopeFanswise;

	EFFECTFILENAMECAPS  m_sEffectFilenameCaps; 
	TEXTUREFILENAMECAPS m_sTextrueFilenameCaps;

public:
	static DxEffGameHelp& GetInstance();

};
