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
	BOOL			m_bFogStaticRange;	// ���� ���ðŸ� Ȱ��ȭ

	float			m_fFogStart;
	float			m_fFogEnd;

	D3DCOLOR		m_cFogColor;
	D3DCOLOR		m_cFogColor_Day;
	D3DCOLOR		m_cFogColor_Night;
};

struct FOG_PROPERTY
{
	enum { VERSION = 0x0109, };

	BOOL			m_bNewRange;		// 107 ���ʹ� �ִ밡�ðŸ��� ���δ�.
	BOOL			m_bSkyFogEnable;

	D3DCOLOR		m_cFogColor;
	D3DCOLOR		m_cFogColor_Day;
	D3DCOLOR		m_cFogColor_Night;

	BOOL			m_bFogStaticRange;	// ���� ���ðŸ� Ȱ��ȭ

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
	void	LoadSet_0109 ( sc::SerialFile &SFile );	//	������ ���� ���� ����
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

			BOOL			m_bFogStaticRange;	// ���ðŸ� Ȱ��ȭ

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

    float		m_fFogStart_RealTime;   // ���� ���ӿ��� ����Ǵ� ��ġ
    float		m_fFogEnd_RealTime;     // ���� ���ӿ��� ����Ǵ� ��ġ

protected:
	BOOL		m_bFogEnable;

protected:
	FOGRANGE	m_emFogRange;		// ���� ������ ���� �ɼ�

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
	void		SetFogRange ( FOGRANGE emFogRange )	{ m_emFogRange = emFogRange; }	// �þ� ����

	float		GetFogRangeFar();

    float       GetFogStart()   { return m_fFogStart; }
    float       GetFogEnd()     { return m_fFogEnd; }

	// ���ӿ��� �ִ� �ָ� ���̴� �Ÿ�
	float       GetGameFogHighEnd( const FOG_PROPERTY& sProperty );

	void		SetFogStartFogEnd( float fFogStart, float fFogEnd )		{ m_fFogStart_RealTime=fFogStart;	m_fFogEnd_RealTime=fFogEnd; }
    float       GetFogStart_RealTime()   { return m_fFogStart_RealTime; }
    float       GetFogEnd_RealTime()     { return m_fFogEnd_RealTime; }



	//////////////////////////////////////////////////////////////////////////
	//							PMarket
private:
	BOOL		m_bPMarketON_OnlyUI;			// ResizeWindow �� �ϸ� m_emFogRange ���� ������� �ǵ��ư������� ���� ������ �ϵ��� �Ѵ�.

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