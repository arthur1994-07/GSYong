#pragma once

#include "DxEffChar.h"

struct EFFCHAR_PROPERTY_WORLD_BATTLE_FLAG : public EFFCHAR_PROPERTY
{
	DWORD		m_dwMaterials;

	EFFCHAR_PROPERTY_WORLD_BATTLE_FLAG ()
		: m_dwMaterials(0L)
	{
	}
};

class DxEffCharWorldBattleFlag : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	DWORD GetTypeID () override		{ return TYPEID; }
	DWORD GetFlag () override		{ return EMECF_WB_FLAG; }
	const char* GetName () override	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_WORLD_BATTLE_FLAG m_Property;
		};
		struct
		{
			DWORD		m_dwMaterials;
		};
	};

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProperty ) override
	{
		m_Property = *((EFFCHAR_PROPERTY_WORLD_BATTLE_FLAG*)pProperty);
	}

	EFFCHAR_PROPERTY* GetProperty () override
	{
		return &m_Property;
	}

private:
	DXMATERIAL_CHAR_EFF*	m_pMaterials;

private:
	LPDIRECT3DDEVICEQ   m_pd3dDevice;
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;
	TextureResource		m_textureRes;

public:
	void SetMaterials( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials ) override;
	DXMATERIAL_CHAR_EFF*		GetMaterials () override { return m_pMaterials; }

	// 이곳에서 텍스쳐가 만들어지면, 렌더링 되는 BaseTexture 를 사용하지 않고 이곳에서 만들어진 것을 사용하게 된다.
	void SetWorldBattleFlag( LPDIRECT3DTEXTUREQ apOverlayTexOUT[], const TCHAR* pWorldBattleFlag ) override;

private:
	//void CreateBakeTexWB( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ& pResultTex );
	BOOL IsExist_Copy_pmcMesh();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime ) override;

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) override;
	HRESULT DeleteDeviceObjects () override;

public:
	DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum ) override;

	HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice ) override;
	HRESULT SaveFile ( sc::BaseStream &SFile ) override;

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffCharWorldBattleFlag(void);
	virtual ~DxEffCharWorldBattleFlag(void);
};
