#pragma once

#include "DxEffSingle.h"

#define USEPARENTROTATE					0x00040000	// 부모 회전 영향받음;

struct TARGETINFLUENCE_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_TARGETINFLUENCE, };
	static const char		NAME[];
	const static DWORD		VERSION;

	virtual inline DWORD GetTypeID ()		{ return TYPEID; }
	virtual inline const char* GetName ()	{ return NAME; }
	
	struct PROPERTY
	{
		DWORD	m_dwFLAG;
	};

	union
	{
		struct
		{
			PROPERTY	m_Property;
		};

		struct
		{
			DWORD	m_dwFLAG;
		};
	};

	// Effect Creator
	virtual DxEffSingle* NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup );
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache);
	virtual HRESULT DeleteDeviceObjects();

	TARGETINFLUENCE_PROPERTY() : EFF_PROPERTY(), m_dwFLAG(0)	{}
	virtual ~TARGETINFLUENCE_PROPERTY()							{}
};

class DxEffectTargetInfluence : public DxEffSingle
{
private:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	D3DXMATRIX				m_matWorld;

public:
	const static DWORD TYPEID;
	const static DWORD FLAG;
	const static char NAME[];
	DWORD m_dwFLAG;

public:
	DxEffectTargetInfluence(void);
	virtual ~DxEffectTargetInfluence(void);

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();
	
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha );
	virtual HRESULT FinalCleanup();

	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual inline DWORD GetTypeID ()		{ return TYPEID; }
	virtual inline DWORD GetFlag ()			{ return FLAG; }
	virtual inline const char* GetName ()	{ return NAME; }
};