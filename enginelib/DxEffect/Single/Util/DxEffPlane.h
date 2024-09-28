#ifndef __DXEFFECTPLANE_H_
#define __DXEFFECTPLANE_H_

#pragma once

#include "../../../DxTools/DxVertexFVF.h"
#include "../DxEffSingle.h"

struct PLANE_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_PLANE, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY		// Note : Ver.100
	{
		DWORD		m_dwColor;

		float		m_fWidth;
		float		m_fHeight;
		D3DXVECTOR3 m_vNormal;

		char		m_szTexture[256];
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};

		struct
		{
			DWORD			m_dwColor;

			float			m_fWidth;
			float			m_fHeight;
			D3DXVECTOR3		m_vNormal;

			char			m_szTexture[256];
		};
	};

	//	Note : 타입 마다 존재하는 리소스.
	//
	TextureResource m_textureRes;

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup );
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache);
	virtual HRESULT DeleteDeviceObjects();

	PLANE_PROPERTY () : EFF_PROPERTY(),
		m_dwColor(0xFFFFFFFF),
		m_fWidth(1.0f),
		m_fHeight(1.0f),
		m_vNormal(0.0f,1.0f,0.0f)
	{
		memset( m_szTexture, 0, sizeof(char)*256 );
	}

	virtual ~PLANE_PROPERTY()
	{
	}

	const bool ISREBUILD ( const PLANE_PROPERTY& rhs ) const
	{
		if ( m_vNormal != rhs.m_vNormal )
		{
			return true;
		}

		if ( m_fWidth != rhs.m_fWidth )
		{
			return true;
		}

		if ( m_fHeight != rhs.m_fWidth )
		{
			return true;
		}

		if ( m_dwColor != rhs.m_dwColor )
		{
			return true;
		}

		return false;
	}
};

class DxEffPlane : public DxEffSingle
{
public:
	const static DWORD	TYPEID;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	//! : 공통 리소스 자원들 관리.
protected:
	static BOOL	VAILEDDEVICE;

protected:
	static LPDIRECT3DSTATEBLOCK9 m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9 m_pDrawRenderStats;


public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha );

	virtual HRESULT FinalCleanup();

public:
	void UpdateProperty( const bool bTexReload=false, const bool bRebuild=false );
	void UpdateProperty( const PLANE_PROPERTY& cProperty );

public:
	const DWORD CalculateVerticesNum();
	const DWORD CalculateFaceNum();
	void BuildPlane();

public:
	DxEffPlane(void);
	virtual ~DxEffPlane(void);

public:
	LPDIRECT3DDEVICEQ m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFERQ m_pVB;

protected:
	DWORD m_dwVerSize;

public:
	PLANE_PROPERTY m_Property;

};

#endif //__DXEFFECTPLANE_H_
