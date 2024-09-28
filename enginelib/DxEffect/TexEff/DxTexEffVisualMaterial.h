//-------------------------------------------------------------------------------------------------------------------
//                                              shhan
//-------------------------------------------------------------------------------------------------------------------
//                                          VisualMaterial ����
//-------------------------------------------------------------------------------------------------------------------
//                Type        Type     �� ��� ���         Vertex Type
// VisualMaterial PSF      - Tiling - RealPointLight    - VERTEXNORCOLORTEX
//                         - Base   - RealPointLight    - VERTEXNORCOLORTEX
//                WLD      - Tiling - Color�� �� ���   - VERTEXNORCOLORTEX
//                         - Base   - Color�� �� ���   - VERTEXNORCOLORTEX
//                LightMap - Tiling - Texture�� �� ��� - VERTEXNORTEX2
//                         - Base   - Texture�� �� ��� - VERTEXNORTEX2
//-------------------------------------------------------------------------------------------------------------------


#pragma once

#include <boost/tr1/memory.hpp>
#include <string>

#include "../../../VisualMaterialLib/Define/DxVMCommandDef.h"

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../DxVisualMaterialDef.h"
#include "DxTexEff.h"

class sc::SerialFile;
class VisualMaterialFX;
class DxTangentMesh;
struct DxMeshes;

#define	TEXEFFFLOWUV_NEWUV			0x0001
#define	TEXEFFFLOWUV_RANDOMSPEED	0x0002
#define	TEXEFFFLOWUV_MODULATE2X		0x0004

struct TEXEFF_VISUAL_MATERIAL_PROPERTY
{
	TSTRING	                    m_strVisualMaterial;
    vm::VEC_TEXTURE_RESOURCE    m_vecTextureResource;   // Dest
    MAP_PARAMETER_NAME_DATA2    m_mapParameter;     
    std::tr1::shared_ptr<VisualMaterialFX> m_spVMFX;       // NS_VM_MANAGER���� ������.
    std::tr1::shared_ptr<VisualMaterialFX> m_spVMFX_LM;    // NS_VM_MANAGER���� ������.
    std::tr1::shared_ptr<VisualMaterialFX> m_spVMFX_COLOR_LM;  // NS_VM_MANAGER���� ������.

	void Save( sc::SerialFile& SFile );
	BOOL Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
    BOOL Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
    BOOL Load_101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

    void CloneDummy( const TEXEFF_VISUAL_MATERIAL_PROPERTY& sSrc );

    void CleanUp();
    void CleanUp_NoneClearTexture();

	TEXEFF_VISUAL_MATERIAL_PROPERTY();
	~TEXEFF_VISUAL_MATERIAL_PROPERTY();
};

class DxTexEffVisualMaterial : public DxTexEffBase
{
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static char	NAME[];

private:
	enum RENDER_FX_INDEX
	{
	RENDER_FX_INDEX_LIGHTMAP = 0,
	RENDER_FX_INDEX_REALPL = 1,
	RENDER_FX_INDEX_STARGLOW = 2,
	};

protected:
    float       m_fTime;
    D3DXVECTOR3 m_vLightDir;
	TEXEFF_VISUAL_MATERIAL_PROPERTY	m_sProp;

    // FrameMesh ��忡�� �������� ���ؼ� �߰���.
private:
    DxDynamicVB::BIGVB*  m_pDynamicVB;

public:
	virtual DWORD GetTypeID()				{ return TYPEID; }
	virtual const char* GetName()			{ return NAME; }
	virtual void GetProperty( PBYTE &pProp ) { pProp = (PBYTE)&m_sProp; }
	virtual void SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp );

public:
    virtual void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual void FrameMove( const float fElapsedTime );

	// Note : FrameMesh Mode
public:
    // Translucent ���� �˻��Ѵ�. TRUE ��� ���߿� ������ �ǰ�, FALSE ��� ��� ������ �ȴ�.
    virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                            DxTangentMesh* pTangentMesh, 
                            const D3DXMATRIX& matWorld, 
                            LPDIRECT3DTEXTUREQ pTextureDay, 
                            LPDIRECT3DTEXTUREQ pTextureNight,
                            LPDIRECT3DTEXTUREQ pTextureAvgDay,
                            LPDIRECT3DTEXTUREQ pTextureAvgNight,
                            const D3DXVECTOR2& vLightMapUV_Offset,
                            float fAlpha );

    virtual void RenderColorLM( LPDIRECT3DDEVICEQ pd3dDevice, 
                                DxTangentMesh* pTangentMesh, 
                                const D3DXMATRIX& matWorld, 
                                LPDIRECT3DTEXTUREQ pTextureDay, 
                                LPDIRECT3DTEXTUREQ pTextureNight,
                                LPDIRECT3DTEXTUREQ pTextureAvgDay,
                                LPDIRECT3DTEXTUREQ pTextureAvgNight,
                                const D3DXVECTOR2& vLightMapUV_Offset,
                                float fAlpha );

    virtual void RenderColor( LPDIRECT3DDEVICEQ pd3dDevice, DxTangentMesh* pTangentMesh, const DWORD dwAttrib, BOOL bRealPL, const D3DXMATRIX& matWorld, float fAlpha );

    virtual void ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual BOOL IsVM2Side() const;                     // VM �� ����ϸ鼭 2Side Rendering�� ����ߴ°�~?
    virtual void GetFileNameVM( TSTRING& strVM ) const; // VM �� ����Ѵٸ� ����ϴ� VM ���� �̸���~?

public:
    void RenderDetail( LPDIRECT3DDEVICEQ pd3dDevice, 
                        DxTangentMesh* pTangentMesh, 
                        const DWORD dwAttrib, 
                        const D3DXMATRIX& matWorld, 
                        LPDIRECT3DTEXTUREQ pTextureDay, 
                        LPDIRECT3DTEXTUREQ pTextureNight,
                        LPDIRECT3DTEXTUREQ pTextureAvgDay,
                        LPDIRECT3DTEXTUREQ pTextureAvgNight,
                        const D3DXVECTOR2& vLightMapUV_Offset,
						float fAlpha,
						BOOL bStarGlow );

    void RenderColorDetailLM( LPDIRECT3DDEVICEQ pd3dDevice, 
                                DxTangentMesh* pTangentMesh, 
                                const DWORD dwAttrib, 
                                const D3DXMATRIX& matWorld, 
                                LPDIRECT3DTEXTUREQ pTextureDay, 
                                LPDIRECT3DTEXTUREQ pTextureNight,
                                LPDIRECT3DTEXTUREQ pTextureAvgDay,
                                LPDIRECT3DTEXTUREQ pTextureAvgNight,
                                const D3DXVECTOR2& vLightMapUV_Offset,
								float fAlpha,
								BOOL bStarGlow );

	void RenderColorDetail( LPDIRECT3DDEVICEQ pd3dDevice,
							DxTangentMesh* pTangentMesh, 
							const DWORD dwAttrib, 
							BOOL bRealPL, 
							const D3DXMATRIX& matWorld, 
							float fAlpha,
							BOOL bStarGlow );

private:
    void RenderDetail2( LPDIRECT3DDEVICEQ pd3dDevice, DxTangentMesh* pTangentMesh, std::tr1::shared_ptr<VisualMaterialFX>& spVMFX, const DWORD dwAttrib, RENDER_FX_INDEX emRenderFxIndex );
    HRESULT Render_DrawIndexedPrimitive( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX, D3DXATTRIBUTERANGE& sAttrib, RENDER_FX_INDEX emRenderFxIndex );
    HRESULT Render_DrawIndexedPrimitiveUP( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX );

public:
    void PreSettingFX( std::tr1::shared_ptr<VisualMaterialFX>& spVMFX, 
                        const D3DXMATRIX& matWorld, 
                        LPDIRECT3DTEXTUREQ pTextureDay, 
                        LPDIRECT3DTEXTUREQ pTextureNight, 
                        LPDIRECT3DTEXTUREQ pTextureAvgDay,
                        LPDIRECT3DTEXTUREQ pTextureAvgNight,
                        const D3DXVECTOR2& vLightMapUV_Offset,
                        float fAlpha );

    std::tr1::shared_ptr<VisualMaterialFX>& GetVMFX_COLOR();
    void InsertColorTriangle( LPDIRECT3DDEVICEQ pd3dDevice, BYTE* pVertices );
    void RenderColorTriangle( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	TEXEFF_VISUAL_MATERIAL_PROPERTY* GetProperty()	{ return &m_sProp; }

public:
	virtual void SavePSF( sc::SerialFile& SFile );
	virtual BOOL LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

public:
	DxTexEffVisualMaterial();
	virtual ~DxTexEffVisualMaterial();
};


