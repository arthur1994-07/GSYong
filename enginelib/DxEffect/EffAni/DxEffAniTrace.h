#pragma once

#include "DxEffAni.h"

class DxSkinChar;
class DxEffSingleGroup;

struct EFFANI_PROPERTY_TRACE_100
{
	float		m_fScale;
	char        m_szTexName[28];
};

struct EFFANI_PROPERTY_TRACE : public EFFANI_PROPERTY
{
	float		m_fScale;

	EFFANI_PROPERTY_TRACE () :
		m_fScale(1.f)
	{
	}
};

class DxEffAniData_Trace : public DxEffAniData
{
public:
	static DWORD		VERSION;
	static DWORD		TYPEID;
	static char			NAME[64];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

public:
	union
	{
		struct
		{
			EFFANI_PROPERTY_TRACE		m_Property;
		};

		struct
		{
			float		m_fScale;
		};
	};

public:
	virtual void SetProperty ( EFFANI_PROPERTY *pProperty )
	{
		m_Property = *((EFFANI_PROPERTY_TRACE*)pProperty);
	}
	
	virtual EFFANI_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

public:
	virtual void SetEffAniData ( DxEffAniData*	pData );

public:
	virtual DxEffAni* NEWOBJ ( DxSkinAnimation* pAnimContainer );	//	Note : 이팩트 생성자.
	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	DxEffAniData_Trace ()
        : DxEffAniData()
	{
	}

    virtual ~DxEffAniData_Trace()
    {
    }
};

class DxEffAniTrace : public DxEffAni
{
public:
	float		m_fScale;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit () override;
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) override;
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) override;
	virtual HRESULT InvalidateDeviceObjects () override;
	virtual HRESULT DeleteDeviceObjects () override;
	virtual HRESULT FinalCleanup () override;

public:
	virtual HRESULT FrameMove ( DxSkinCharEffAniBase* pSkinCharBase, float fTime, float fElapsedTime ) override;
	virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, const D3DXMATRIX& matCurPos, float fAlpha ) override;
	//virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIX& matCurPos, float fAlpha ) override;
	//virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIX& matCurPos, float fAlpha ) override;

public:
	DxEffAniTrace(void);
	virtual ~DxEffAniTrace(void);
};

