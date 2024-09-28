#ifndef __DXEFFECTCIRCLE_H_
#define __DXEFFECTCIRCLE_H_

#pragma once

#include "../../../DxTools/DxVertexFVF.h"
#include "../DxEffSingle.h"

struct CIRCLE_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_CIRCLE, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY		// Note : Ver.100
	{
		DWORD m_dwColor;

		float m_fDegree;
		float m_fRadius;

		float m_fVertexPerDegree;

		char  m_szTexture[256];
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
			DWORD			m_dwOutLineColor;

			float			m_fRadius;
			float			m_fVertexPerDegree;

			float			m_fOutLine;

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

	CIRCLE_PROPERTY () : EFF_PROPERTY(),
		m_dwColor(0xFFFFFFFF),
		m_dwOutLineColor(0xFF000000),
		m_fRadius(1.0f),
		m_fOutLine(0.0f),
		m_fVertexPerDegree(1.0f)
	{
		memset( m_szTexture, 0, sizeof(char)*256 );
	}

	virtual ~CIRCLE_PROPERTY()
	{
	}

	const bool ISREBUILD ( const CIRCLE_PROPERTY& rhs ) const
	{
		if ( m_dwColor != rhs.m_dwColor )
		{
			if ( m_dwColor == 0 || rhs.m_dwColor == 0 )
			{
				return true;
			}
		}

		if ( m_dwOutLineColor != rhs.m_dwOutLineColor )
		{
			if ( m_dwOutLineColor == 0 || rhs.m_dwOutLineColor == 0 )
			{
				return true;
			}
		}

		if ( m_fRadius != rhs.m_fRadius )
		{
			return true;
		}

		if ( m_fOutLine != rhs.m_fOutLine )
		{
			return true;
		}

		if ( m_fVertexPerDegree != rhs.m_fVertexPerDegree )
		{
			return true;
		}

		return false;
	}
};

class DxEffCircle : public DxEffSingle
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
	void UpdateProperty( const CIRCLE_PROPERTY& cProperty );

public:
	const DWORD CalculateInnerVerticesNum();
	const DWORD CalculateInnerFaceNum( const bool bRenderCheck=true );
	const DWORD CalculateOuterVerticesNum();
	const DWORD CalculateOuterFaceNum();
	void BuildCircle();

public:
	DxEffCircle(void);
	virtual ~DxEffCircle(void);

public:
	LPDIRECT3DDEVICEQ m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFERQ m_pVB;

protected:
	DWORD m_dwVerSize;

public:
	CIRCLE_PROPERTY m_Property;

};

#endif //__DXEFFECTCIRCLE_H_
