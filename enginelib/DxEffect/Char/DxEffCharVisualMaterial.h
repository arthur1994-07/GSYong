#pragma once

#include <boost/tr1/memory.hpp>

#include "../../../VisualMaterialLib/Define/DxVMCommandDef.h"

#include "../DxVisualMaterialDef.h"
#include "./DxEffChar.h"

class VisualMaterialFX;


struct EFFCHAR_PROPERTY_VISUAL_MATERIAL
{
    TSTRING	                    m_strVisualMaterial;
    vm::VEC_TEXTURE_RESOURCE    m_vecTextureResource;   // Dest
    MAP_PARAMETER_NAME_DATA2    m_mapParameter;     
    std::tr1::shared_ptr<VisualMaterialFX> m_spVMFX;       // NS_VM_MANAGER에서 관리됨.

    void CloneDummy( const EFFCHAR_PROPERTY_VISUAL_MATERIAL& sSrc );
    void Save( sc::BaseStream& SFile );
    void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::BaseStream& SFile, BOOL bSkinMesh );
    void Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::BaseStream& SFile, BOOL bSkinMesh );

    void CleanUp();

    EFFCHAR_PROPERTY_VISUAL_MATERIAL();
    ~EFFCHAR_PROPERTY_VISUAL_MATERIAL();
};

typedef std::vector<std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>>    VEC_PROPERTY;
typedef VEC_PROPERTY::iterator                                              VEC_PROPERTY_ITER;

class DxEffCharVisualMaterial : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID () override		{ return TYPEID; }
	virtual DWORD GetFlag () override;
	virtual const char* GetName () override	{ return NAME; }

protected:
    VEC_PROPERTY    m_vecProperty;

    EFFCHAR_PROPERTY m_sProperty;

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty );
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_sProperty;
	}

protected:
    LPDIRECT3DDEVICEQ   m_pd3dDevice;
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;
    D3DXMATRIX          m_matVP;
	BOOL				m_bPossibleEffect;
    BOOL                m_bSkinMesh;    
    float               m_fTime;
    D3DXVECTOR3         m_vUserColor1;
    D3DXVECTOR3         m_vUserColor2;
    D3DXVECTOR3         m_vLightDir;
    const SKINEFFDATA*  m_rSKINEFFDATA;     // 참조

public:
    void SetPropertyNEW( LPDIRECT3DDEVICEQ pd3dDevice, const VEC_PROPERTY& vecSrc );
    const D3DXVECTOR3& GetUserColor1()  { return m_vUserColor1; }
    const D3DXVECTOR3& GetUserColor2()  { return m_vUserColor2; }

private:
    HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX, LPD3DXBONECOMBINATION pBoneCur );
    HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX, const D3DXATTRIBUTERANGE& sAttributeTable );
    void PreSettingFX( std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>& spProperty, ID3DXEffect* pFX, float fAlpha );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	RenderVM( LPDIRECT3DDEVICEQ pd3dDevice, const SKINEFFDATA* pSKINEFFDATA, TEXTURE_TYPE_FLAG emTexType ) OVERRIDE;
    virtual void    RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIXA16* pBoneMatrices, LPD3DXBONECOMBINATION pBoneCur, int nPaletteEntries, int nInfl, TEXTURE_TYPE_FLAG emTexType );
    virtual void    RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwAttrib, TEXTURE_TYPE_FLAG emTexType, const D3DXMATRIX& matStaticLinkBone );
    virtual void    ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

private:
    HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
    friend class CEffVisualMaterialPage;

public:
	DxEffCharVisualMaterial(void);
	virtual ~DxEffCharVisualMaterial(void);
};
