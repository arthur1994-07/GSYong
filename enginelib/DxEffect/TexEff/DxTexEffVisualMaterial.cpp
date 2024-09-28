#include "pch.h"

#include "../../../VisualMaterialLib/Manager/VisualMaterialFX.h"
#include "../../../VisualMaterialLib/Manager/DxVMManager.h"
#include "../../../VisualMaterialLib/Manager/NSVMRender.h"
#include "../../../VisualMaterialLib/Define/DxVMDefine.h"
#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"
#include "../../../VisualMaterialLib/Util/DxTangentMesh.h"

#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/PostProcessing/NSGlow.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxFogMan.h"
#include "../../G-Logic/GLDefine.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"

#include "DxTexEffVisualMaterial.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define V(x)           { hr = x; }

const DWORD DxTexEffVisualMaterial::TYPEID = DEF_TEXEFF_VISUAL_MATERIAL;
const DWORD	DxTexEffVisualMaterial::VERSION = 0x00000102;
const char DxTexEffVisualMaterial::NAME[] = "[ VisualMaterial ]";

extern BOOL    g_bWIREFRAME_BLACK;
extern BOOL    g_bWIREFRAME_VIEW;

//-----------------------------------------------------------------------------------------------------------
//                                  TEXEFF_VISUAL_MATERIAL_PROPERTY
TEXEFF_VISUAL_MATERIAL_PROPERTY::TEXEFF_VISUAL_MATERIAL_PROPERTY()
{
	m_strVisualMaterial = _T("");
}

TEXEFF_VISUAL_MATERIAL_PROPERTY::~TEXEFF_VISUAL_MATERIAL_PROPERTY()
{
    CleanUp();
}

void TEXEFF_VISUAL_MATERIAL_PROPERTY::CleanUp()
{
    m_vecTextureResource.clear();
    m_mapParameter.clear();
}

void TEXEFF_VISUAL_MATERIAL_PROPERTY::CloneDummy( const TEXEFF_VISUAL_MATERIAL_PROPERTY& sSrc )
{
    CleanUp();

    m_mapParameter      = sSrc.m_mapParameter;
    m_strVisualMaterial	= sSrc.m_strVisualMaterial;

    for ( DWORD i=0; i<sSrc.m_vecTextureResource.size(); ++i )
    {
        vm::NAME_TEXTURE sNameTexture;
        sNameTexture.m_strName = sSrc.m_vecTextureResource[i].m_strName;
        m_vecTextureResource.push_back( sNameTexture );
    }

    m_spVMFX = sSrc.m_spVMFX;
    m_spVMFX_LM = sSrc.m_spVMFX_LM;
    m_spVMFX_COLOR_LM = sSrc.m_spVMFX_COLOR_LM;
}

void TEXEFF_VISUAL_MATERIAL_PROPERTY::Save( sc::SerialFile& SFile )
{
	SFile << m_strVisualMaterial;

    DWORD dwSize = static_cast<DWORD> (m_vecTextureResource.size());
    SFile << dwSize;

    SFile.BeginBlock();
    {
        for ( DWORD i=0; i<dwSize; ++i )
        {
            SFile << m_vecTextureResource[i].m_strName;
        }
    }
    SFile.EndBlock();

    SFile.BeginBlock();
    {
        dwSize = static_cast<DWORD> (m_mapParameter.size());
        SFile << dwSize;

        MAP_PARAMETER_NAME_DATA2_ITER iter = m_mapParameter.begin();
        for ( ; iter!=m_mapParameter.end(); ++iter )
        {
            SFile << (*iter).first;
            SFile << (*iter).second.m_bNight;
            SFile << (*iter).second.m_vParameter;
            SFile << (*iter).second.m_vNight;
        }
    }
    SFile.EndBlock();
}

BOOL TEXEFF_VISUAL_MATERIAL_PROPERTY::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    CleanUp();

	SFile >> m_strVisualMaterial;

    m_spVMFX            = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR );
    m_spVMFX_LM         = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT );
    m_spVMFX_COLOR_LM   = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR_LM );

    DWORD dwSize(0), dwBufferSizeMINI(0);
    SFile >> dwSize;
    SFile >> dwBufferSizeMINI;

    if ( dwSize == m_spVMFX->GetTextureSize() )
    {
        for ( DWORD i=0; i<dwSize; ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            SFile >> sNameTexture.m_strName;

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
		    false,
		    FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }
    else
    {
        // 정확한 위치로 이동
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSizeMINI );

        // 새로운 데이터로 정보를 셋팅한다.
        for ( DWORD i=0; i<m_spVMFX->GetTextureSize(); ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            sNameTexture.m_strName = m_spVMFX->GetTextureName( i );

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
				false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }

    SFile >> dwBufferSizeMINI;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        TSTRING strName;
        ParameterData sData;
        SFile >> strName;
        SFile >> sData.m_bNight;
        SFile >> sData.m_vParameter;
        SFile >> sData.m_vNight;

        m_mapParameter.insert( std::make_pair( strName.c_str(), sData ) );
    }

    // 파일이 없습니다.
    if ( !m_spVMFX->GetFX() )
    {
        std::string ErrorMsg(
            sc::string::format(
                "VisualMaterial file not exist. : %1%", m_strVisualMaterial));
        CDebugSet::ToLogFile(ErrorMsg);

		return FALSE;
    }

	return TRUE;
}

BOOL TEXEFF_VISUAL_MATERIAL_PROPERTY::Load_101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    CleanUp();

	SFile >> m_strVisualMaterial;

    m_spVMFX            = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR );
    m_spVMFX_LM         = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT );
    m_spVMFX_COLOR_LM   = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR_LM );

    DWORD dwSize(0), dwBufferSizeMINI(0);
    SFile >> dwSize;
    SFile >> dwBufferSizeMINI;

    if ( dwSize == m_spVMFX->GetTextureSize() )
    {
        for ( DWORD i=0; i<dwSize; ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            SFile >> sNameTexture.m_strName;

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
				false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }
    else
    {
        // 정확한 위치로 이동
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSizeMINI );

        // 새로운 데이터로 정보를 셋팅한다.
        for ( DWORD i=0; i<m_spVMFX->GetTextureSize(); ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            sNameTexture.m_strName = m_spVMFX->GetTextureName( i );

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
				false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }

    SFile >> dwBufferSizeMINI;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        TSTRING strName;
        ParameterData sData;
        SFile >> strName;
        SFile >> sData.m_bNight;
        SFile >> sData.m_vParameter;
        SFile >> sData.m_vNight;

        m_mapParameter.insert( std::make_pair( strName.c_str(), sData ) );
    }

    // 데이터를 변경시킴
    MAP_PARAMETER_NAME_DATA2_ITER iter = m_mapParameter.begin();
    for ( ; iter!=m_mapParameter.end(); ++iter )
    {
        (*iter).second.m_vNight.y = (*iter).second.m_vNight.z = (*iter).second.m_vNight.x;
        (*iter).second.m_vParameter.y = (*iter).second.m_vParameter.z = (*iter).second.m_vParameter.x;
    }

    // 파일이 없습니다.
    if ( !m_spVMFX->GetFX() )
    {
        std::string ErrorMsg(
            sc::string::format(
                "VisualMaterial file not exist. %1%",
                m_strVisualMaterial));
        CDebugSet::ToLogFile(ErrorMsg);

		return FALSE;
    }

	return TRUE;
}

BOOL TEXEFF_VISUAL_MATERIAL_PROPERTY::Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    CleanUp();

	SFile >> m_strVisualMaterial;

    m_spVMFX            = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR );
    m_spVMFX_LM         = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT );
    m_spVMFX_COLOR_LM   = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR_LM );

    DWORD dwSize(0), dwBufferSizeMINI(0);
    SFile >> dwSize;
    SFile >> dwBufferSizeMINI;

    if ( dwSize == m_spVMFX->GetTextureSize() )
    {
        for ( DWORD i=0; i<dwSize; ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            SFile >> sNameTexture.m_strName;

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
				false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }
    else
    {
        // 정확한 위치로 이동
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSizeMINI );

        // 새로운 데이터로 정보를 셋팅한다.
        for ( DWORD i=0; i<m_spVMFX->GetTextureSize(); ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            sNameTexture.m_strName = m_spVMFX->GetTextureName( i );

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
				false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }

    // 파일이 없습니다.
    if ( !m_spVMFX->GetFX() )
    {
        std::string ErrorMsg(
            sc::string::format(
                "VisualMaterial file not exist. %1%",
                m_strVisualMaterial));
        CDebugSet::ToLogFile(ErrorMsg);

		return FALSE;
    }

	return TRUE;
}

//-----------------------------------------------------------------------------------------------------------
//                                  DxTexEffVisualMaterial
DxTexEffVisualMaterial::DxTexEffVisualMaterial()
    : m_fTime(0.f)
    , m_vLightDir(1.f,1.f,1.f)
    , m_pDynamicVB(NULL)
{
}

DxTexEffVisualMaterial::~DxTexEffVisualMaterial()
{
}

void DxTexEffVisualMaterial::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
}

void DxTexEffVisualMaterial::OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( m_pDynamicVB )
        m_pDynamicVB->ReleaseVB();
    SAFE_DELETE( m_pDynamicVB );
}

void DxTexEffVisualMaterial::SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp )
{
    TEXEFF_VISUAL_MATERIAL_PROPERTY* pTemp = (TEXEFF_VISUAL_MATERIAL_PROPERTY*)pProp;
    m_sProp.CleanUp();
    m_sProp.m_mapParameter      = pTemp->m_mapParameter;
    m_sProp.m_strVisualMaterial	= pTemp->m_strVisualMaterial;

    for ( DWORD i=0; i<pTemp->m_vecTextureResource.size(); ++i )
    {
        vm::NAME_TEXTURE sNameTexture;
        sNameTexture.m_strName = pTemp->m_vecTextureResource[i].m_strName;

        sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
            sNameTexture.m_strName,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_VISUALMATERIAL,
			false,
			FALSE );

        m_sProp.m_vecTextureResource.push_back( sNameTexture );
    }

    m_sProp.m_spVMFX            = NS_VM_MANAGER::Load( pd3dDevice, m_sProp.m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR );
    m_sProp.m_spVMFX_LM         = NS_VM_MANAGER::Load( pd3dDevice, m_sProp.m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT );
    m_sProp.m_spVMFX_COLOR_LM   = NS_VM_MANAGER::Load( pd3dDevice, m_sProp.m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR_LM );
}

std::tr1::shared_ptr<VisualMaterialFX>& DxTexEffVisualMaterial::GetVMFX_COLOR()
{
    return m_sProp.m_spVMFX;
}

void DxTexEffVisualMaterial::FrameMove( const float fElapsedTime )
{
    m_fTime += fElapsedTime;
}

void DxTexEffVisualMaterial::Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                                    DxTangentMesh* pTangentMesh, 
                                    const D3DXMATRIX& matWorld, 
                                    LPDIRECT3DTEXTUREQ pTextureDay, 
                                    LPDIRECT3DTEXTUREQ pTextureNight,
                                    LPDIRECT3DTEXTUREQ pTextureAvgDay,
                                    LPDIRECT3DTEXTUREQ pTextureAvgNight,
                                    const D3DXVECTOR2& vLightMapUV_Offset,
                                    float fAlpha )
{
	// StarGlow 가 활성화 되어 있나면, 기록해둔 후 나중에 렌더링 하도록 하자.
	if ( m_sProp.m_spVMFX->IsActiveStarGlow() )
	{
		NSGlow::SetActiveGlow();

		NSVMRender::g_sStarGlow.Insert
		( 
			NSVMRender::RT_LIGHTMAP,
			reinterpret_cast<BYTE*>( this ), 
			pTangentMesh, 
			0, 
			FALSE, 
			matWorld, 
			pTextureDay, 
			pTextureNight, 
			pTextureAvgDay, 
			pTextureAvgNight,
			vLightMapUV_Offset,
			fAlpha
		);
	}

    // Translucent, Additive, Modulate 라면 나중에 렌더링 되어야 한다.
    switch ( m_sProp.m_spVMFX_LM->GetBlendMode() )
    {
    case VM_BLENDMODE_TRANSLUCENT:
    case VM_BLENDMODE_ADDITIVE:
    case VM_BLENDMODE_MODULATE:
        {
            NSVMRender::g_sTranslucent.Insert
            ( 
				NSVMRender::RT_LIGHTMAP,
                reinterpret_cast<BYTE*>( this ), 
                pTangentMesh, 
                0, 
                FALSE, 
                matWorld, 
                pTextureDay, 
                pTextureNight, 
                pTextureAvgDay, 
                pTextureAvgNight,
                vLightMapUV_Offset,
                fAlpha
            );
            return;
        }
        break;
    };

    RenderDetail( pd3dDevice, pTangentMesh, 0, matWorld, pTextureDay, pTextureNight, pTextureAvgDay, pTextureAvgNight, vLightMapUV_Offset, fAlpha, FALSE );
}

void DxTexEffVisualMaterial::RenderColorLM( LPDIRECT3DDEVICEQ pd3dDevice, 
                                            DxTangentMesh* pTangentMesh, 
                                            const D3DXMATRIX& matWorld, 
                                            LPDIRECT3DTEXTUREQ pTextureDay, 
                                            LPDIRECT3DTEXTUREQ pTextureNight,
                                            LPDIRECT3DTEXTUREQ pTextureAvgDay,
                                            LPDIRECT3DTEXTUREQ pTextureAvgNight,
                                            const D3DXVECTOR2& vLightMapUV_Offset,
                                            float fAlpha )
{
	// StarGlow 가 활성화 되어 있나면, 기록해둔 후 나중에 렌더링 하도록 하자.
	if ( m_sProp.m_spVMFX->IsActiveStarGlow() )
	{
		NSGlow::SetActiveGlow();

		NSVMRender::g_sStarGlow.Insert
		( 
			NSVMRender::RT_COLOR_LIGHTMAP,
			reinterpret_cast<BYTE*>( this ), 
			pTangentMesh, 
			0, 
			FALSE, 
			matWorld, 
			pTextureDay, 
			pTextureNight, 
			pTextureAvgDay, 
			pTextureAvgNight,
			vLightMapUV_Offset,
			fAlpha
		);
	}

    // Translucent, Additive, Modulate 라면 나중에 렌더링 되어야 한다.
    switch ( m_sProp.m_spVMFX_COLOR_LM->GetBlendMode() )
    {
    case VM_BLENDMODE_TRANSLUCENT:
    case VM_BLENDMODE_ADDITIVE:
    case VM_BLENDMODE_MODULATE:
        {
            NSVMRender::g_sTranslucent.Insert
            ( 
				NSVMRender::RT_COLOR_LIGHTMAP,
                reinterpret_cast<BYTE*>( this ), 
                pTangentMesh, 
                0, 
                FALSE, 
                matWorld, 
                pTextureDay, 
                pTextureNight, 
                pTextureAvgDay, 
                pTextureAvgNight,
                vLightMapUV_Offset,
                fAlpha
            );
            return;
        }
        break;
    };

    RenderColorDetailLM( pd3dDevice, pTangentMesh, 0, matWorld, pTextureDay, pTextureNight, pTextureAvgDay, pTextureAvgNight, vLightMapUV_Offset, fAlpha, FALSE );
}

void DxTexEffVisualMaterial::RenderColor( LPDIRECT3DDEVICEQ pd3dDevice, DxTangentMesh* pTangentMesh, const DWORD dwAttrib, BOOL bRealPL, const D3DXMATRIX& matWorld, float fAlpha )
{
	// StarGlow 가 활성화 되어 있나면, 기록해둔 후 나중에 렌더링 하도록 하자.
	if ( m_sProp.m_spVMFX->IsActiveStarGlow() )
	{
		NSGlow::SetActiveGlow();

		NSVMRender::g_sStarGlow.Insert( NSVMRender::RT_COLOR, reinterpret_cast<BYTE*>( this ), pTangentMesh, dwAttrib, bRealPL, matWorld, NULL, NULL, NULL, NULL, D3DXVECTOR2(0.f,0.f), fAlpha );
	}

    // Translucent, Additive, Modulate 라면 나중에 렌더링 되어야 한다.
    switch ( m_sProp.m_spVMFX->GetBlendMode() )
    {
    case VM_BLENDMODE_TRANSLUCENT:
    case VM_BLENDMODE_ADDITIVE:
    case VM_BLENDMODE_MODULATE:
        {
			NSVMRender::g_sTranslucent.Insert( NSVMRender::RT_COLOR, reinterpret_cast<BYTE*>( this ), pTangentMesh, dwAttrib, bRealPL, matWorld, NULL, NULL, NULL, NULL, D3DXVECTOR2(0.f,0.f), fAlpha );
            return;
        }
        break;
    };

    RenderColorDetail( pd3dDevice, pTangentMesh, dwAttrib, bRealPL, matWorld, fAlpha, FALSE );
}

void DxTexEffVisualMaterial::RenderDetail( LPDIRECT3DDEVICEQ pd3dDevice, 
                                            DxTangentMesh* pTangentMesh, 
                                            const DWORD dwAttrib, 
                                            const D3DXMATRIX& matWorld, 
                                            LPDIRECT3DTEXTUREQ pTextureDay, 
                                            LPDIRECT3DTEXTUREQ pTextureNight,
                                            LPDIRECT3DTEXTUREQ pTextureAvgDay,
                                            LPDIRECT3DTEXTUREQ pTextureAvgNight,
                                            const D3DXVECTOR2& vLightMapUV_Offset,
                                            float fAlpha,
											BOOL bStarGlow )
{
    DWORD dwFVF(0L);
    pd3dDevice->GetFVF( &dwFVF );
    pd3dDevice->SetFVF( 0 );
    pd3dDevice->SetVertexDeclaration( NS_VM_MANAGER::g_pDCRT_PIECE );

    pd3dDevice->SetStreamSource( 0, pTangentMesh->GetVB(), 0, sizeof(vm::VERTEXNORTANGENTTEX2_COMPRESS) );
	pd3dDevice->SetIndices ( pTangentMesh->GetIB() );

	if ( !NSLightMapFX::RenderTest( pd3dDevice, FALSE, pTangentMesh, dwAttrib ) )	// LightTestFX 가 동작되게 되면, 실제 렌더링 되야할 것은 작동시키지 않는다.
	{
		// 실제 렌더링
		PreSettingFX( m_sProp.m_spVMFX_LM, matWorld, pTextureDay, pTextureNight, pTextureAvgDay, pTextureAvgNight, vLightMapUV_Offset, fAlpha );
		RenderDetail2( pd3dDevice, pTangentMesh, m_sProp.m_spVMFX_LM, dwAttrib, bStarGlow ? RENDER_FX_INDEX_STARGLOW : RENDER_FX_INDEX_LIGHTMAP );
	}

    // 정보 복원
    pd3dDevice->SetFVF( dwFVF );
    pd3dDevice->SetVertexShader( NULL );
    pd3dDevice->SetPixelShader( NULL );
}

void DxTexEffVisualMaterial::RenderColorDetailLM( LPDIRECT3DDEVICEQ pd3dDevice, 
                                                    DxTangentMesh* pTangentMesh, 
                                                    const DWORD dwAttrib, 
                                                    const D3DXMATRIX& matWorld, 
                                                    LPDIRECT3DTEXTUREQ pTextureDay, 
                                                    LPDIRECT3DTEXTUREQ pTextureNight,
                                                    LPDIRECT3DTEXTUREQ pTextureAvgDay,
                                                    LPDIRECT3DTEXTUREQ pTextureAvgNight,
                                                    const D3DXVECTOR2& vLightMapUV_Offset,
													float fAlpha,
													BOOL bStarGlow )
{
    DWORD dwFVF(0L);
    pd3dDevice->GetFVF( &dwFVF );
    pd3dDevice->SetFVF( 0 );
    pd3dDevice->SetVertexDeclaration( NS_VM_MANAGER::g_pDCRT_PIECE_COLOR_LM );

    pd3dDevice->SetStreamSource( 0, pTangentMesh->GetVB(), 0, sizeof(vm::VERTEXNORCOLORTANGENTTEX2_COMPRESS) );
	pd3dDevice->SetIndices ( pTangentMesh->GetIB() );

	if ( !NSLightMapFX::RenderTest( pd3dDevice, TRUE, pTangentMesh, dwAttrib ) )	// LightTestFX 가 동작되게 되면, 실제 렌더링 되야할 것은 작동시키지 않는다.
	{
		PreSettingFX( m_sProp.m_spVMFX_COLOR_LM, matWorld, pTextureDay, pTextureNight, pTextureAvgDay, pTextureAvgNight, vLightMapUV_Offset, fAlpha );
		RenderDetail2( pd3dDevice, pTangentMesh, m_sProp.m_spVMFX_COLOR_LM, dwAttrib, bStarGlow ? RENDER_FX_INDEX_STARGLOW : RENDER_FX_INDEX_LIGHTMAP );
	}

    // 정보 복원
    pd3dDevice->SetFVF( dwFVF );
    pd3dDevice->SetVertexShader( NULL );
    pd3dDevice->SetPixelShader( NULL );
}

void DxTexEffVisualMaterial::RenderColorDetail( LPDIRECT3DDEVICEQ pd3dDevice,
											   DxTangentMesh* pTangentMesh, 
											   const DWORD dwAttrib, 
											   BOOL bRealPL, 
											   const D3DXMATRIX& matWorld, 
											   float fAlpha,
											   BOOL bStarGlow )
{
    DWORD dwFVF(0L);
    pd3dDevice->GetFVF( &dwFVF );
    pd3dDevice->SetFVF( 0 );
    pd3dDevice->SetVertexDeclaration( NS_VM_MANAGER::g_pDCRT_PIECE_COLOR );

    pd3dDevice->SetStreamSource( 0, pTangentMesh->GetVB(), 0, sizeof(vm::VERTEXNORCOLORTEXTANGENT_COMPRESS) );
	pd3dDevice->SetIndices ( pTangentMesh->GetIB() );

	PreSettingFX( m_sProp.m_spVMFX, matWorld, NULL, NULL, NULL, NULL, D3DXVECTOR2(0.f,0.f), fAlpha );
	RenderDetail2( pd3dDevice, pTangentMesh, m_sProp.m_spVMFX, dwAttrib, bStarGlow ? RENDER_FX_INDEX_STARGLOW : (bRealPL ? RENDER_FX_INDEX_REALPL : RENDER_FX_INDEX_LIGHTMAP) );

    // 정보 복원
    pd3dDevice->SetFVF( dwFVF );
    pd3dDevice->SetVertexShader( NULL );
    pd3dDevice->SetPixelShader( NULL );
}

void DxTexEffVisualMaterial::InsertColorTriangle( LPDIRECT3DDEVICEQ pd3dDevice, BYTE* pVertices )
{
    if ( !m_pDynamicVB )
    {
        m_pDynamicVB = new DxDynamicVB::BIGVB;
        m_pDynamicVB->dwFVF = vm::VERTEXNORCOLORTEXTANGENT_COMPRESS::FVF;
        m_pDynamicVB->nVertexSize = sizeof(vm::VERTEXNORCOLORTEXTANGENT_COMPRESS);
	    m_pDynamicVB->nFullByte = m_pDynamicVB->nVertexSize * 3000;
        m_pDynamicVB->CreateVB( pd3dDevice, TRUE );
    }

    // DynamicVB 셋팅.
    {
	    DWORD dwVertexSizeFULL = 3*sizeof(vm::VERTEXNORCOLORTEXTANGENT_COMPRESS);
        if( !m_pDynamicVB->IsValidSize( dwVertexSizeFULL ) )
            return;

        if( !m_pDynamicVB->IsValidCopy( dwVertexSizeFULL ) )
		    RenderColorTriangle( pd3dDevice );

        m_pDynamicVB->CopyToVB(
		    pVertices,
		    dwVertexSizeFULL,
		    3 );
    }
}

void DxTexEffVisualMaterial::RenderColorTriangle( LPDIRECT3DDEVICEQ pd3dDevice )
{
    DWORD dwFVF(0L);
    pd3dDevice->GetFVF( &dwFVF );
    pd3dDevice->SetFVF( 0 );
    pd3dDevice->SetVertexDeclaration( NS_VM_MANAGER::g_pDCRT_PIECE_COLOR );

    RenderDetail2( pd3dDevice, NULL, m_sProp.m_spVMFX, 0, RENDER_FX_INDEX_LIGHTMAP );

    // 정보 복원
    pd3dDevice->SetFVF( dwFVF );
    pd3dDevice->SetVertexShader( NULL );
    pd3dDevice->SetPixelShader( NULL );
}

void DxTexEffVisualMaterial::PreSettingFX( std::tr1::shared_ptr<VisualMaterialFX>& spVMFX, 
                                          const D3DXMATRIX& matWorld, 
                                          LPDIRECT3DTEXTUREQ pTextureDay, 
                                          LPDIRECT3DTEXTUREQ pTextureNight,
                                          LPDIRECT3DTEXTUREQ pTextureAvgDay,
                                          LPDIRECT3DTEXTUREQ pTextureAvgNight,
                                          const D3DXVECTOR2& vLightMapUV_Offset,
                                          float fAlpha )
{
    ID3DXEffect* pFX = spVMFX->GetFX();
    if ( !pFX )
        return;

    const D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView();
    const D3DXMATRIX& matProj = DxViewPort::GetInstance().GetMatProj();

    D3DXMATRIX matWVP;
    D3DXMatrixMultiply( &matWVP, &matWorld, &matView );
    D3DXMatrixMultiply( &matWVP, &matWVP, &matProj );

    pFX->SetMatrixTranspose( "g_matWorld",	&matWorld );
    pFX->SetMatrixTranspose( "g_matWVP",	&matWVP );

    DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
    const D3DLIGHT9& sLight = pLight->m_Light;

    D3DXVECTOR3 vLightDir3;
    D3DXVECTOR4 vLightDir;
    D3DXVECTOR3 vHalfDir3;
    D3DXVECTOR4 vHalfDir4;
    D3DXVECTOR4 vCameraFrom4;
    {
        D3DXVECTOR4 vLightMapUV_Offset4(vLightMapUV_Offset.x,vLightMapUV_Offset.y,1.f,1.f);
        D3DXVECTOR4 vDayNightWeight4(GLPeriod::GetInstance().GetBlendFact(),1.f-GLPeriod::GetInstance().GetBlendFact(),1.f,1.f);
        D3DXVECTOR4 vLightDiffuse(sLight.Diffuse.r,sLight.Diffuse.g,sLight.Diffuse.b,fAlpha);
        D3DXVECTOR4 vLightAmbient(sLight.Ambient.r,sLight.Ambient.g,sLight.Ambient.b,1.f);
        D3DXVECTOR4 vLightSpecular(sLight.Specular.r,sLight.Specular.g,sLight.Specular.b,1.f);
        m_vLightDir.x = -sLight.Direction.x;
        m_vLightDir.y = -sLight.Direction.y;
        m_vLightDir.z = -sLight.Direction.z;
        D3DXVec3Normalize( &m_vLightDir, &m_vLightDir );
        vLightDir.x = m_vLightDir.x;
        vLightDir.y = m_vLightDir.y;
        vLightDir.z = m_vLightDir.z;
        vLightDir.w = 1.f;

        D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
        vCameraFrom4.x = vFromPt.x;
        vCameraFrom4.y = vFromPt.y;
        vCameraFrom4.z = vFromPt.z;
        vCameraFrom4.w = 1.f;

        // Diffuser Color Power 적용
        {
            float fLM_DDPower = (DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower()-1.f)*GLPeriod::GetInstance().GetBlendFact();
            vLightDiffuse += vLightDiffuse*fLM_DDPower;
        }

        if ( g_bWIREFRAME_BLACK )
        {
            D3DXVECTOR4 vBlack(0.f,0.f,0.f,1.f);
            pFX->SetVector( "g_vLightDiffuse", &vBlack );
		    pFX->SetVector( "g_vLightAmbient", &vBlack );
            pFX->SetVector( "g_vLightSpecular", &vBlack );
        }
        else
        {
            pFX->SetVector( "g_vLightDiffuse", &vLightDiffuse );
		    pFX->SetVector( "g_vLightAmbient", &vLightAmbient );
            pFX->SetVector( "g_vLightSpecular", &vLightSpecular );
        }
		
        pFX->SetVector( "g_vLightDir", &vLightDir );
        pFX->SetVector( "g_vCameraFrom", &vCameraFrom4 );
        pFX->SetVector( "g_vDayNightWeight", &vDayNightWeight4 );
        pFX->SetVector( "g_vLightMapUV_Offset", &vLightMapUV_Offset4 );

        pFX->SetTexture( "ShininessTex", NS_VM_MANAGER::g_textureResShininess.GetTexture() );
        if ( pTextureDay )
        {
            pFX->SetTexture( "LightMapDayTex", pTextureDay );
        }
        if ( pTextureNight )
        {
            pFX->SetTexture( "LightMapNightTex", pTextureNight );
        }
        if ( pTextureAvgDay )
        {
            pFX->SetTexture( "LightMapAvgTex", pTextureAvgDay );
        }
        if ( pTextureAvgNight )
        {
            pFX->SetTexture( "LightMapAvgTex", pTextureAvgNight );
        }

        // Fog
		if ( g_bWIREFRAME_VIEW )
		{
			D3DXVECTOR4 vFog
			(
				2, 
				1,
				1.f,
				1.f
			);
			pFX->SetVector( "g_vFog", &vFog );
		}
		else
		{
			D3DXVECTOR4 vFog
			(
				DxFogMan::GetInstance().GetFogEnd_RealTime(), 
				DxFogMan::GetInstance().GetFogEnd_RealTime() - DxFogMan::GetInstance().GetFogStart_RealTime(),
				1.f,
				1.f
			);
			pFX->SetVector( "g_vFog", &vFog );
		}

        // Light 설정
        {
            // Piece에서는 최대 7개까지 PointLight에 연관된다.
            #define POINT_LIGHT_MAX 7

            D3DXVECTOR4 vPos_Range_PL[ POINT_LIGHT_MAX ];
            D3DXVECTOR4 vDiff_PL[ POINT_LIGHT_MAX ];
            D3DXVECTOR4 vAtt_PL[ POINT_LIGHT_MAX ];
            DWORD dwSize = NSNearPointLight::GetPointLightDataSize();
            if ( dwSize > POINT_LIGHT_MAX )
                dwSize = POINT_LIGHT_MAX;

            for ( DWORD i=0; i<dwSize && i<POINT_LIGHT_MAX; ++i )
            {
                const NSNearPointLight::PointLightData* pPointLight = NSNearPointLight::GetPointLightData(i);
                vDiff_PL[i] = D3DXVECTOR4( pPointLight->m_cDiffuse.r, pPointLight->m_cDiffuse.g, pPointLight->m_cDiffuse.b, 1.f );
                vPos_Range_PL[i] = D3DXVECTOR4( pPointLight->m_vPosition.x, pPointLight->m_vPosition.y, pPointLight->m_vPosition.z, pPointLight->m_fRange );
                vAtt_PL[i] = D3DXVECTOR4( pPointLight->m_vAttenuation.x, pPointLight->m_vAttenuation.y, pPointLight->m_vAttenuation.z, 1.f );
            }

            pFX->SetInt( "g_nPointLightNum", dwSize );
            pFX->SetVectorArray( "g_vPos_Range_PL", &vPos_Range_PL[0], POINT_LIGHT_MAX );
            pFX->SetVectorArray( "g_vDiff_PL", &vDiff_PL[0], POINT_LIGHT_MAX );
            pFX->SetVectorArray( "g_vAtt_PL", &vAtt_PL[0], POINT_LIGHT_MAX );
        }

        // Texture_0 ... 설정
        {
            char c1='0';
            std::string strTextureORIG = "Texture_";
            std::string strTexture = "";
            for ( size_t i=0; i<m_sProp.m_vecTextureResource.size(); ++i )
            {
                strTexture = strTextureORIG + static_cast<char>( c1 + i );
                pFX->SetTexture( strTexture.c_str(), m_sProp.m_vecTextureResource[i].m_textureRes.GetTexture() );
            }
        }

        // TextureCube_0 ... 설정
        {
            char c1='0';
            std::string strTextureCubeORIG = "CubeTexture_";
            std::string strTextureCube = "";
            for ( size_t i=0; i<spVMFX->GetCubeTextureSize(); ++i )
            {
                strTextureCube = strTextureCubeORIG + static_cast<char>( c1 + i );
                pFX->SetTexture( strTextureCube.c_str(), spVMFX->GetCubeTexture(i) );
            }
        }

        // g_fUniformPixelScalar_0 ... 설정.
        {
            vm::MAP_PARAMETER_NAME_DATA mapParamaterNameData;
            MAP_PARAMETER_NAME_DATA2_CITER iter = m_sProp.m_mapParameter.begin();
            for ( ; iter!=m_sProp.m_mapParameter.end(); ++iter )
            {
                // 밤
				if ( DxLightMan::GetInstance()->IsNightAndDay() == TRUE &&
					(!GLPeriod::GetInstance().IsOffLight() && (*iter).second.m_bNight) )
                {
                    // 밤
                    mapParamaterNameData.insert( std::make_pair( (*iter).first, (*iter).second.m_vNight ) );
                }
                else
                {
                    // 낮
                    mapParamaterNameData.insert( std::make_pair( (*iter).first, (*iter).second.m_vParameter ) );
                }                
            }

            spVMFX->FrameMove( mapParamaterNameData, m_fTime );
        }
    }
}

void DxTexEffVisualMaterial::RenderDetail2( LPDIRECT3DDEVICEQ pd3dDevice, 
                                          DxTangentMesh* pTangentMesh, 
                                          std::tr1::shared_ptr<VisualMaterialFX>& spVMFX, 
                                          const DWORD dwAttrib, 
                                          RENDER_FX_INDEX emRenderFxIndex )
{
    ID3DXEffect* pFX = spVMFX->GetFX();
    if ( !pFX )
        return;

    // 기본 렌더링
    {
        if ( pTangentMesh && pTangentMesh->GetAttrib() )
        {
            Render_DrawIndexedPrimitive( pd3dDevice, pFX, pTangentMesh->GetAttrib()[dwAttrib], emRenderFxIndex );
        }
        else
        {
            Render_DrawIndexedPrimitiveUP( pd3dDevice, pFX );
        }
    }

    // LightMap 일 경우는 TwoSideSeperatePass 를 하지 않는다.
    if ( spVMFX->GetTwoSideSeperatePass() && emRenderFxIndex != RENDER_FX_INDEX_LIGHTMAP )
    {
        DWORD dwCullMode;
        pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
        pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

        D3DXVECTOR4 vLightDir;
        vLightDir.x = m_vLightDir.x;
        vLightDir.y = m_vLightDir.y;
        vLightDir.z = m_vLightDir.z;
        vLightDir.w = -1.f;
        pFX->SetVector( "g_vLightDir", &vLightDir );

        if ( pTangentMesh )
        {
            Render_DrawIndexedPrimitive( pd3dDevice, pFX, pTangentMesh->GetAttrib()[dwAttrib], emRenderFxIndex );
        }
        else
        {
            Render_DrawIndexedPrimitiveUP( pd3dDevice, pFX );
        }

        pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode );
    }
}

HRESULT DxTexEffVisualMaterial::Render_DrawIndexedPrimitive( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX, D3DXATTRIBUTERANGE& sAttrib, RENDER_FX_INDEX emRenderFxIndex )
{
    HRESULT hr(S_OK);

	
	DWORD dwPassIndex(0);
	switch ( emRenderFxIndex )
	{
	case RENDER_FX_INDEX_LIGHTMAP:	dwPassIndex = 0;	break;
	case RENDER_FX_INDEX_REALPL:	dwPassIndex = 1;	break;
	case RENDER_FX_INDEX_STARGLOW:	dwPassIndex = 2;	break;
	default: 
		return hr;
	};

    UINT numPasses;
    V( pFX->Begin( &numPasses, 0 ) ); 
    if( dwPassIndex < numPasses )
    {
        V( pFX->BeginPass( dwPassIndex ) );
        {
            pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 
									            0,
									            sAttrib.VertexStart,
									            sAttrib.VertexCount,
									            sAttrib.FaceStart*3,
									            sAttrib.FaceCount );
        }
        V( pFX->EndPass() );
    }
    V( pFX->End() );

    return hr;
}

HRESULT DxTexEffVisualMaterial::Render_DrawIndexedPrimitiveUP( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX )
{
    HRESULT hr(S_OK);

    if ( !m_pDynamicVB )
        return S_OK;

    m_pDynamicVB->UpdateVB();

    pd3dDevice->SetStreamSource ( 0, m_pDynamicVB->pVB, 0, sizeof(vm::VERTEXNORCOLORTEXTANGENT_COMPRESS) );

    // m_pDynamicVB 것을 렌더 할 때는 FrameMesh 모드일 경우이다.
    // 이 때는 실시간 PointLight를 켜주자.
    DWORD dwPassIndex(1);

    UINT numPasses;
    V( pFX->Begin( &numPasses, 0 ) ); 
    if( dwPassIndex < numPasses )
    {
        V( pFX->BeginPass( dwPassIndex ) );
        {
            pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_pDynamicVB->iStartVertex/3 );
        }
        V( pFX->EndPass() );
    }
    V( pFX->End() );

    // 초기화
    m_pDynamicVB->Clear();

    return hr;
}

void DxTexEffVisualMaterial::ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
    // 백업
    VEC_TSTRING    m_vecTextureName;
    if ( m_sProp.m_spVMFX.get() )
    {
        for ( DWORD i=0; i<m_sProp.m_spVMFX->GetTextureSize(); ++i )
        {
            m_vecTextureName.push_back( m_sProp.m_spVMFX->GetTextureName(i) );
        }
    }

    // 로딩
    m_sProp.m_spVMFX            = NS_VM_MANAGER::Load( pd3dDevice, m_sProp.m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR );
    m_sProp.m_spVMFX_LM         = NS_VM_MANAGER::Load( pd3dDevice, m_sProp.m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT );
    m_sProp.m_spVMFX_COLOR_LM   = NS_VM_MANAGER::Load( pd3dDevice, m_sProp.m_strVisualMaterial, NS_VM_MANAGER::EMRT_OBJECT_COLOR_LM );


    // 셋팅된 텍스쳐 갯수가 맞지 않으면, 텍스쳐 정보를 리셋하도록 한다.
    BOOL bTextureReset(FALSE);
    if ( m_sProp.m_vecTextureResource.size() != m_sProp.m_spVMFX->GetTextureSize() )
    {
        bTextureReset = TRUE;
    }
    else
    {
        // 원본 텍스쳐 이름이 변경되었다면, 텍스쳐 정보를 리셋하도록 한다.
        for ( DWORD i=0; i<m_sProp.m_spVMFX->GetTextureSize(); ++i )
        {
            if ( m_vecTextureName[i] != m_sProp.m_spVMFX->GetTextureName(i) &&
                m_vecTextureName[i] == m_sProp.m_vecTextureResource[i].m_strName )
            {
                bTextureReset = TRUE;
            }
        }
    }
    
    // 텍스쳐 리셋
    if ( bTextureReset )
    {
        // Reload 에서는 m_sProp.CleanUp()을 하지 않음.
        // 이 함수로 여러번 들어오는데 ReleaseTexture 때문에 이전에 참조하고 있던 m_pTexture 포인터가 망가질수 있다.
        // Tool 이라서 참조가 망가져도 놔두도록 한다.
        m_sProp.m_vecTextureResource.clear();

        DWORD dwSize = m_sProp.m_spVMFX->GetTextureSize();
        for ( DWORD i=0; i<dwSize; ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            sNameTexture.m_strName = m_sProp.m_spVMFX->GetTextureName(i);

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
				false,
				FALSE );

            m_sProp.m_vecTextureResource.push_back( sNameTexture );
        }
    }
}

// VM 을 사용하면서 2Side Rendering을 사용했는가~?
BOOL DxTexEffVisualMaterial::IsVM2Side() const
{
    if ( m_sProp.m_spVMFX->GetTwoSideSeperatePass() )
        return TRUE;

    return FALSE;
}

// VM 을 사용한다면 사용하는 VM 파일 이름은~?
void DxTexEffVisualMaterial::GetFileNameVM( TSTRING& strVM ) const
{
    strVM = m_sProp.m_strVisualMaterial;
}

void DxTexEffVisualMaterial::SavePSF( sc::SerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock();
	m_sProp.Save( SFile );
	SFile.EndBlock();
}

BOOL DxTexEffVisualMaterial::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	DWORD dwSize;
	DWORD dwVer;
	SFile >> dwVer;
	SFile >> dwSize;

	if( dwVer==VERSION )
	{
		if ( !m_sProp.Load( pd3dDevice, SFile ) )
			return FALSE;
	}
    else if( dwVer==0x00000101 )
	{
		if ( !m_sProp.Load_101( pd3dDevice, SFile ) )
			return FALSE;
	}
    else if( dwVer==0x00000100 )
	{
		if ( !m_sProp.Load_100( pd3dDevice, SFile ) )
			return FALSE;
	}
	else
	{
		DWORD dwCurBuffer = SFile.GetfTell();
		SFile.SetOffSet( dwCurBuffer+dwSize );
		return FALSE;
	}

	return TRUE;
}
