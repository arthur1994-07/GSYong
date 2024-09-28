#pragma once

namespace sc
{
	class SerialFile;
};

struct FOG_PROPERTY_100_2
{
	BOOL			m_bFogEnable;
	D3DCOLOR		m_cFogColor_Day;
	float			m_fFogStart;
	float			m_fFogEnd;
	float			m_fFogDensity;
};

struct FOG_PROPERTY_103_4					// Ver. 103, 104
{
	BOOL			m_bFogEnable;
	BOOL			m_bFogDayNight;
	D3DCOLOR		m_cFogColor_Day;
	float			m_fFogStart;
	float			m_fFogEnd;
	float			m_fFogDensity;
	D3DCOLOR		m_cFogColor_Night;
	float			m_fFogStart_Night;
	float			m_fFogEnd_Night;
	float			m_fFogDensity_Night;
};

struct FOG_PROPERTY_105
{
	BOOL			m_bSkyFogEnable;

	D3DCOLOR		m_cFogColor;
	float			m_fFogStart;
	float			m_fFogEnd;

	D3DCOLOR		m_cFogColor_Day;
	float			m_fFogStart_Day;
	float			m_fFogEnd_Day;
	D3DCOLOR		m_cFogColor_Night;
	float			m_fFogStart_Night;
	float			m_fFogEnd_Night;
};

struct FOG_PROPERTY_106
{
	BOOL			m_bSkyFogEnable;
	BOOL			m_bFogStaticRange;	// 정적 가시거리 활성화

	float			m_fFogStart;
	float			m_fFogEnd;

	D3DCOLOR		m_cFogColor;
	D3DCOLOR		m_cFogColor_Day;
	D3DCOLOR		m_cFogColor_Night;
};

struct FOG_PROPERTY
{
	enum { VERSION = 0x0109, };

	BOOL			m_bNewRange;		// 107 부터는 최대가시거리를 늘인다.
	BOOL			m_bSkyFogEnable;

	D3DCOLOR		m_cFogColor;
	D3DCOLOR		m_cFogColor_Day;
	D3DCOLOR		m_cFogColor_Night;

	BOOL			m_bFogStaticRange;	// 정적 가시거리 활성화

	float			m_fFogStart;
	float			m_fFogEnd;

	FOG_PROPERTY () :
		m_bNewRange(FALSE),
		m_bSkyFogEnable(FALSE),
		m_bFogStaticRange(FALSE),
		m_fFogStart(720.0f),
		m_fFogEnd(790.0f),
		m_cFogColor(D3DCOLOR_XRGB(0,0,0)),
		m_cFogColor_Day(D3DCOLOR_XRGB(89,135,179)),
		m_cFogColor_Night(D3DCOLOR_XRGB(60,60,94))
	{
	}

	void	SaveSet ( sc::SerialFile &SFile );
	void	LoadSet ( sc::SerialFile &SFile );

	void	LoadSet_0100 ( sc::SerialFile &SFile );
	void	LoadSet_0101 ( sc::SerialFile &SFile );
	void	LoadSet_0102 ( sc::SerialFile &SFile );
	void	LoadSet_0103 ( sc::SerialFile &SFile );
	void	LoadSet_0104 ( sc::SerialFile &SFile );
	void	LoadSet_0105 ( sc::SerialFile &SFile );
	void	LoadSet_0106 ( sc::SerialFile &SFile );
	void	LoadSet_0107 ( sc::SerialFile &SFile );
	void	LoadSet_0108 ( sc::SerialFile &SFile );	
	void	LoadSet_0109 ( sc::SerialFile &SFile );	//	데이터 포맷 변경 진행
};

enum FOGRANGE
{
	EMFR_NSIZE	= 4,

	EMFR_OFF	= 0,
	EMFR_LOW	= 1,
	EMFR_MIDDLE = 2,
	EMFR_HIGH	= 3,
};

class DxFogMan
{
public:
	void SetProperty ( FOG_PROPERTY& Property );
	FOG_PROPERTY& GetProperty () { return m_Property; }

protected:
	union
	{
		struct
		{
			FOG_PROPERTY	m_Property;
		};
		struct
		{
			BOOL			m_bNewRange;
			BOOL			m_bSkyFogEnable;

			D3DCOLOR		m_cFogColor;
			D3DCOLOR		m_cFogColor_Day;
			D3DCOLOR		m_cFogColor_Night;

			BOOL			m_bFogStaticRange;	// 가시거리 활성화

			float			m_fFogStart;
			float			m_fFogEnd;
		};
	};

protected:
	DWORD		m_dwSavedSB;
	DWORD		m_dwEffectSB;

	D3DCOLOR	m_cDayColor;
	D3DCOLOR	m_cFogColor_Game;
	D3DCOLOR	m_cFogColor_White;
	D3DCOLOR	m_cFogColor_Dark;
	D3DCOLOR	m_cFogColor_Object;
	BOOL		m_bFrameMode_Fog;

protected:
	float		m_fRangeLow_Near;
	float		m_fRangeLow_Far;
	float		m_fRangeMiddle_Near;
	float		m_fRangeMiddle_Far;
	float		m_fRangeHigh_Near;
	float		m_fRangeHigh_Far;

    float		m_fFogStart_RealTime;   // 실제 게임에서 적용되는 수치
    float		m_fFogEnd_RealTime;     // 실제 게임에서 적용되는 수치

protected:
	BOOL		m_bFogEnable;

protected:
	FOGRANGE	m_emFogRange;		// 포그 범위에 대한 옵션

public:
	void		FrameMove();
	void		RenderFogSB ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFORCE_FOG_OFF );

	D3DCOLOR	GetFogWhiteColor ()	{ return m_cFogColor_White; }
	D3DCOLOR	GetFogDarkColor ()	{ return m_cFogColor_Dark; }
	D3DCOLOR	GetFogColor ()		{ return m_cFogColor_Game; }
	D3DCOLOR	GetDayColor ()		{ return m_cDayColor; }

	void		SetFrameMode_Fog ( BOOL bUse )	{ m_bFrameMode_Fog = bUse; }
	BOOL		GetFrameMode_Fog ()				{ return m_bFrameMode_Fog; }

	FOGRANGE	GetFogRange ()						{ return m_emFogRange; }
	void		SetFogRange ( FOGRANGE emFogRange )	{ m_emFogRange = emFogRange; }	// 시야 셋팅

	float		GetFogRangeFar();

    float       GetFogStart()   { return m_fFogStart; }
    float       GetFogEnd()     { return m_fFogEnd; }

	// 게임에서 최대 멀리 보이는 거리
	float       GetGameFogHighEnd( const FOG_PROPERTY& sProperty );

	void		SetFogStartFogEnd( float fFogStart, float fFogEnd )		{ m_fFogStart_RealTime=fFogStart;	m_fFogEnd_RealTime=fFogEnd; }
    float       GetFogStart_RealTime()   { return m_fFogStart_RealTime; }
    float       GetFogEnd_RealTime()     { return m_fFogEnd_RealTime; }



	//////////////////////////////////////////////////////////////////////////
	//							PMarket
private:
	BOOL		m_bPMarketON_OnlyUI;			// ResizeWindow 를 하면 m_emFogRange 값이 원래대로 되돌아가버려서 강제 셋팅을 하도록 한다.

public:
	void BackUpFogRangeForPMarket();
	void RollBackFogRangeForPMarket();
	//							PMarket
	//////////////////////////////////////////////////////////////////////////



public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxFogMan();

public:
	virtual ~DxFogMan();

public:
	static DxFogMan& GetInstance();
};
