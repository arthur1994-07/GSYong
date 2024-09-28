#pragma once

#include "../../DxTools/TextureManager.h"
#include "./DxEffSingle.h"

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct SCREEN_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_SCREENEFFECT, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY
	{
		char			m_szEffectName[MAX_PATH];

		PROPERTY()
		{
			memset( m_szEffectName, 0, sizeof(char)*MAX_PATH );
		};
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};
		struct
		{
			char			m_szEffectName[MAX_PATH];
		};
	};

	//	Note : 타입 마다 존재하는 리소스.
	//
	TextureResource m_textureRes;
	BOOL			m_bBackUpCache;

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup );
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache);
	virtual HRESULT DeleteDeviceObjects();

	SCREEN_PROPERTY () 
		: EFF_PROPERTY()
		, m_bBackUpCache(FALSE)
	{
		StringCchCopy( m_szEffectName, 256, "" );
	}
    virtual ~SCREEN_PROPERTY()
    {
    }
};

class DxEffectScreen : public DxEffSingle
{
public:
	const static DWORD	TYPEID;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

public:
	DxEffectScreen ();
	virtual ~DxEffectScreen ();

public:
	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha );

	virtual HRESULT FinalCleanup();

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	TSTRING				m_strEffectName;
	TextureResource		m_textureRes;

public:
	friend struct SCREEN_PROPERTY;
};