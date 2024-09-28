#pragma once

#include <string>

#include "../../DxTools/TextureManager.h"

enum EMBOARD_TYPE
{
	EMBOARD_FIGHTER_M	= 0,
	EMBOARD_ARMS_M		= 1,
	EMBOARD_ARCHER_W	= 2,
	EMBOARD_SPIRIT_W	= 3,

	EMBOARD_EXTREME_M	= 4,	
	EMBOARD_EXTREME_W	= 5,

	EMBOARD_FIGHTER_W	= 6,
	EMBOARD_ARMS_W		= 7,
	EMBOARD_ARCHER_M	= 8,
	EMBOARD_SPIRIT_M	= 9,

	EMBOARD_SCIENTIST_M = 10,
	EMBOARD_SCIENTIST_W = 11,

    EMBOARD_ASSASSIN_M  = 12,
    EMBOARD_ASSASSIN_W  = 13,

	
	// 변술부 변수가 없어서 다른 GLChar Define 을 참조해서 추가했습니다.
	// 문제가 있을경우 수정 hjKown

	EMBOARD_TRICKER_M = 14,
	EMBOARD_TRICKER_W = 15,

	EMBOARD_ETC_M       = 16,
	EMBOARD_ETC_W       = 17,

	EMBOARD_ACTOR_M = 18,
	EMBOARD_ACTOR_W = 19,

	EMBOARD_SIZE		= 20,
};

struct SBillboadCharInfo
{
	EMBOARD_TYPE emType;
	D3DXVECTOR3  vPos;	
};

struct SBillboardSort
{
public:
	bool operator() ( const SBillboadCharInfo &a, const SBillboadCharInfo &b );
};

class DxEffBillboardChar
{
public:
	DxEffBillboardChar(void);
	~DxEffBillboardChar(void);

	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadBillboardTex ( const std::string *pstrCharBoard );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	void	FrameMove ( float fTime, float fElapsedTime );
	void	RenderBillboardChar( EMBOARD_TYPE type, const D3DXVECTOR3 &vPos );
	void	Render();

	VOID	AddBillboardChar( EMBOARD_TYPE type, const D3DXVECTOR3 &vPos );



private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	TextureResource	m_textureRes[EMBOARD_SIZE];
	std::string				m_strTexture[EMBOARD_SIZE];
	std::vector<SBillboadCharInfo> m_vecBoardCharinfo;

public:
	static DxEffBillboardChar& GetInstance();

};
