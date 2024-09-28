#pragma once

#include "DxEffAni.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxTools/TextureManager.h"

#include <list>

class DxSkinChar;

struct EFF_PROPGROUP;
class DxEffSingleGroup;

struct EFFANI_PROPERTY_FACEOFF : public EFFANI_PROPERTY
{
	TCHAR		m_szTexture[32];	// 텍스쳐 종류.

	EFFANI_PROPERTY_FACEOFF ()
	{
		StringCchCopy( m_szTexture, 32, _T("") );
	}
};

class DxEffAniData_FaceOff : public DxEffAniData
{
public:
	static DWORD		VERSION;
	static DWORD		TYPEID;
	static TCHAR		NAME[64];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const TCHAR* GetName ()	{ return NAME; }

public:
	union
	{
		struct
		{
			EFFANI_PROPERTY_FACEOFF		m_Property;
		};

		struct
		{
			TCHAR		m_szTexture[32];	// 텍스쳐 종류.
		};
	};

public:
	BOOL				m_bCreated;
	void	SetCreated ( BOOL bCreated )	{ m_bCreated = bCreated; }

public:
	BOOL CheckEff ( DWORD dwCurKeyTime, DWORD dwPrevFrame );

public:
	virtual void SetProperty ( EFFANI_PROPERTY *pProperty )
	{
		m_Property = *((EFFANI_PROPERTY_FACEOFF*)pProperty);
	}
	
	virtual EFFANI_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

public:
	virtual DxEffAni* NEWOBJ( DxSkinAnimation* pAnimContainer );		//	Note : 이팩트 생성자.
	virtual HRESULT LoadFile(sc::BaseStream& SFile, LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT SaveFile(sc::SerialFile& SFile);

public:
	DxEffAniData_FaceOff ()
        : DxEffAniData()
        , m_bCreated( FALSE )
	{
	}

    virtual ~DxEffAniData_FaceOff()
    {
    }
};

class DxEffAniFaceOff : public DxEffAni
{
public:
	TCHAR		m_szTexture[32];	// 텍스쳐 종류.

private:
	TextureResource m_textureRes;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	LPDIRECT3DSTATEBLOCK9	m_pSavedBaseSB;
	LPDIRECT3DSTATEBLOCK9	m_pEffectBaseSB;

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
	virtual BOOL IsHeadDrawChild();

public:
	DxEffAniFaceOff(void);
	virtual ~DxEffAniFaceOff(void);
};

