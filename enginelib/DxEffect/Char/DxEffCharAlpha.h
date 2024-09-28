#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

#define	USETRANSLUCENT		0x0001		// 반투명 옵션을 켠다.

struct EFFCHAR_PROPERTY_ALPHA_100
{
	float	m_fPlayTime;
};

struct EFFCHAR_PROPERTY_ALPHA_101
{
	float	m_fWaitTime;
	float	m_fPlayTime;
	float	m_fHeight;	
};

struct EFFCHAR_PROPERTY_ALPHA_102	//	데이터 포맷 변경 진행	
{
	DWORD	m_dwFlags;
	float	m_fWaitTime;
	float	m_fPlayTime;
	float	m_fHeight;
	float	m_fAlpha;	
};

struct EFFCHAR_PROPERTY_ALPHA : public EFFCHAR_PROPERTY
{
	DWORD	m_dwFlags;
	float	m_fHeight;
	float	m_fAlpha;	
	float	m_fWaitTime;
	float	m_fPlayTime;

	EFFCHAR_PROPERTY_ALPHA () :
		m_fWaitTime(2.f),
		m_fPlayTime(1.f),
		m_fHeight(50.f),
		m_fAlpha(0.f)
	{
		m_dwFlags = 0L;
	}
};

class DxEffCharAlpha : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID() override { return TYPEID; }
	virtual DWORD GetFlag() override   { return EMECF_ALPHA; }
	virtual const char* GetName() override { return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_ALPHA m_Property;
		};
		struct
		{
			DWORD	m_dwFlags;
			float	m_fHeight;
			float	m_fAlpha;
			float	m_fWaitTime;
			float	m_fPlayTime;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_ALPHA*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	DWORD m_dwZWriteEnable;
	DWORD m_dwFogEnable;
	DWORD m_dwAlphaBlendEnable;
	DWORD m_dwSecBlend;
	DWORD m_dwDestBlend;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

protected:
	float	m_fElapsedTime;
	float	m_fTimeSum;

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_ALPHA; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행	

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh();

public:
	DxEffCharAlpha(void);
	virtual ~DxEffCharAlpha(void);
};
