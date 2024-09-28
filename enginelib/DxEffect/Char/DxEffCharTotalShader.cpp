#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxEffect/DxLoadShader.h"
#include "../../DxTools/DxRenderStates.h"

#include "../../DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxTools/RENDERPARAM.h"
#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharHLSL.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"

#include "../../DxTools/editmeshs.h"
#include "../../DxTools/DxInputDevice.h"
#include "DxEffCharTotalShaderHLSL.h"

#include ".\dxeffchartotalshader.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharTotalShader::TYPEID			= EMEFFCHAR_TOTALSHADER;
const DWORD	DxEffCharTotalShader::VERSION			= 0x0100;
char		DxEffCharTotalShader::NAME[MAX_PATH]	= "3.Total Shader";

extern BOOL		g_bCHAR_EDIT_RUN;

#if defined(_RELEASED) 

#ifndef GETHANDLE
#define GETHANDLE(handle,x)           { handle = x; GASSERT(handle); }
#endif

#else

#ifndef GETHANDLE
#define GETHANDLE(handle,x)           { handle = x; }
#endif

#endif

namespace dxeffchartotalshader
{
    const D3DVERTEXELEMENT9 VertexDecl[] =
    {
	    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	    { 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
	    { 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
	    { 0, 64, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
	    { 0, 76, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },

	    D3DDECL_END()
    };
} // namespace dxeffchartotalshader

DxEffCharTotalShader::DxEffCharTotalShader(void)
    : DxEffChar()
    , m_pEffect(NULL)
    , m_bPossibleEffect(TRUE)
    , m_pNormalDCRT(NULL)

    , m_pSkinMesh(NULL)
    , m_pmcMesh(NULL)

    , m_hTechnique(NULL)
    , m_hvLightDir(NULL)
    , m_hvEyePos(NULL)
    , m_htNormalMap(NULL)
    , m_htSpecularTex(NULL)
    , m_htGlossinessTex(NULL)

    , m_hmWorld(NULL)
    , m_hmView(NULL)
    , m_hmProj(NULL)

    , m_htAmbient(NULL)
    , m_htDiffuse(NULL)
    , m_htSpecular(NULL)
    , m_hbUseDirectionLight(NULL)

    , m_htTexUpDownTex(NULL)
    , m_hfTexUpDownAlpha(NULL)

    , m_htOverlayTex(NULL)
    , m_hvInputColor1(NULL)
    , m_hvInputColor2(NULL)
    , m_hfSpecularLevel(NULL)

    , m_htReflectTex(NULL)
    , m_htReflectAlphaTex(NULL)

    , m_vAddLight( 0.0f, 0.0f, 0.0f )
    , m_fAniTexUpDownAlpha(0.0f)
    , m_fAniValue(0.0f)
{
}

DxEffCharTotalShader::~DxEffCharTotalShader(void)
{
	CleanUp ();
}


DxEffChar* DxEffCharTotalShader::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		return NULL;
	}

	HRESULT hr;
	DxEffCharTotalShader *pEffChar = new DxEffCharTotalShader;
	pEffChar->SetFileName ( GetFileName() );
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );	

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}



HRESULT DxEffCharTotalShader::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_rSkinMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_skinMeshRes.get();
	}

	if ( !DxEffectMan::GetInstance().IsPixelShader_3_0() || DxEffectMan::GetInstance().GetUseSwShader() )	// PS.2.0 하드웨어 가속이 가능 할 경우만 로드
	{
		m_bPossibleEffect = FALSE;
		return E_FAIL;
	}

	/*if( !m_bUseNormalMap && !m_bUseTexUpDown && !m_bUseReflect && !m_bUseOverlay ) return S_OK;
	if( m_bUseNormalMap && strlen( m_ShaderNormalMap.szNormalMap ) == 0 ) return S_OK;
	if( m_bUseTexUpDown && strlen( m_ShaderTexUpDown.szTexUpDownTex ) == 0 ) return S_OK;
	if( m_bUseReflect   && strlen( m_ShaderReflection.szReflectTex ) == 0 ) return S_OK;
	if( m_bUseOverlay   && strlen( m_ShaderOverlay.szOverlayTex ) == 0 ) return S_OK;*/


	if( !m_pEffect )
	{

		// 정점 선언 오브젝트 생성
        if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( dxeffchartotalshader::VertexDecl, &m_pNormalDCRT ) ) )
			return E_FAIL;

		// 셰이더 읽기
		LPD3DXBUFFER pErr = NULL;	
		UINT size = (UINT)strlen(pTotalHLSL);
		if( FAILED(D3DXCreateEffect( pd3dDevice, pTotalHLSL, size, NULL, NULL, 0, NULL, &m_pEffect, &pErr ) ) )
		{
            std::string ErrorMsg(
                sc::string::format(
                    "D3DXCreateEffect FAILED ERROR: %1%",
                    pErr->GetBufferPointer()));
			CDebugSet::ToLogFile(ErrorMsg);

#ifdef _RELEASED
			MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
#endif

			SAFE_RELEASE(pErr);
			return E_FAIL;
		}

		GETHANDLE( m_hTechnique, m_pEffect->GetTechniqueByName( "TShader" ) );	 	 			
		GETHANDLE( m_hvLightDir, m_pEffect->GetParameterByName( NULL, "m_vLightDir" ) ); 			
		GETHANDLE( m_hvEyePos, m_pEffect->GetParameterByName( NULL, "m_vEyePos" ) );   				

		GETHANDLE( m_htNormalMap, m_pEffect->GetParameterByName( NULL, "m_NormalMap" ) ); 			
		GETHANDLE( m_htSpecularTex, m_pEffect->GetParameterByName( NULL, "m_SpecularTex" ) );			
		GETHANDLE( m_htGlossinessTex, m_pEffect->GetParameterByName( NULL, "m_GlossinessTex" ) );		

		GETHANDLE( m_hmWorld, m_pEffect->GetParameterByName( NULL, "m_mWorld" ) );				
		GETHANDLE( m_hmView, m_pEffect->GetParameterByName( NULL, "m_mView" ) );				
		GETHANDLE( m_hmProj, m_pEffect->GetParameterByName( NULL, "m_mProj" ) );				

		GETHANDLE( m_htAmbient, m_pEffect->GetParameterByName( NULL, "m_fvAmbient" ) );				
		GETHANDLE( m_htDiffuse, m_pEffect->GetParameterByName( NULL, "m_fvDiffuse" ) );				
		GETHANDLE( m_htSpecular, m_pEffect->GetParameterByName( NULL, "m_fvSpecular" ) );			
		GETHANDLE( m_hbUseDirectionLight, m_pEffect->GetParameterByName( NULL, "m_bUseDirectionLight" ) );  

		GETHANDLE( m_htTexUpDownTex, m_pEffect->GetParameterByName( NULL, "m_TexUpDownTex" ) );
		GETHANDLE( m_hfTexUpDownAlpha, m_pEffect->GetParameterByName( NULL, "m_fAniTexUpDownAlpha" ) );

		GETHANDLE( m_htOverlayTex, m_pEffect->GetParameterByName( NULL, "m_OverlayTexture" ) );
		GETHANDLE( m_hvInputColor1, m_pEffect->GetParameterByName( NULL, "m_fvInputColor1" ) );
		GETHANDLE( m_hvInputColor2, m_pEffect->GetParameterByName( NULL, "m_fvInputColor2" ) );

		GETHANDLE( m_htReflectTex, m_pEffect->GetParameterByName( NULL, "m_ReflectTexture" ) );
		GETHANDLE( m_htReflectAlphaTex, m_pEffect->GetParameterByName( NULL, "m_ReflectAlphaTex" ) );

		SAFE_RELEASE(pErr);
	}
	m_pEffect->SetBool( "m_bUseNormalMap", m_bUseNormalMap );
	m_pEffect->SetBool( "m_bUseTexUpDown", m_bUseTexUpDown );
	m_pEffect->SetBool( "m_bUseOverlay",   m_bUseOverlay );
	m_pEffect->SetBool( "m_bUseReflect",   m_bUseReflect );

	m_pEffect->SetFloat( "m_fSpecularPower", m_fSpecularPower );


	//////////////////////////////////////////////////////////////////////////
	// 노말맵 데이터 로드
	//////////////////////////////////////////////////////////////////////////
	// 노말맵 텍스쳐 읽기
	if( m_bUseNormalMap )
	{
        m_textureResNormal = TextureManager::GetInstance().LoadTexture(
            m_ShaderNormalMap.szNormalMap,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            EMMM_CHAR_MIPMAP,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_CHAR,
			true,
			FALSE );

        // 스펙큘라 맵 읽기
		if( strlen( m_ShaderNormalMap.szSpecularTex ) != 0 && m_ShaderNormalMap.bUseSpecularTex )
		{
            m_textureResSpecular = TextureManager::GetInstance().LoadTexture(
                m_ShaderNormalMap.szSpecularTex,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                EMMM_CHAR_MIPMAP,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_CHAR,
				true,
				FALSE );
		}
        else
        {
			m_textureResSpecular.Clear();
		}
		m_pEffect->SetBool( "m_bUseSpecularTex", m_ShaderNormalMap.bUseSpecularTex );

		if( strlen( m_ShaderNormalMap.szGlossinessTex ) != 0 && m_ShaderNormalMap.bUseGlossinessTex )
		{
            m_textureResGlossiness = TextureManager::GetInstance().LoadTexture(
                m_ShaderNormalMap.szGlossinessTex,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                EMMM_CHAR_MIPMAP,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_CHAR,
				true,
				FALSE );
		}
        else
        {
			m_textureResGlossiness.Clear();
		}
		m_pEffect->SetBool( "m_bUseGlossinessTex", m_ShaderNormalMap.bUseGlossinessTex );
		m_pEffect->SetBool( "m_bUseGlossNormalAlpha", m_ShaderNormalMap.bUseGlossNormalAlpha );
	}
	//////////////////////////////////////////////////////////////////////////
	// 텍스쳐 업다운 데이터 로드
	//////////////////////////////////////////////////////////////////////////
	if( strlen( m_ShaderTexUpDown.szTexUpDownTex ) != 0 && m_bUseTexUpDown )
	{
        m_textureResUpDown = TextureManager::GetInstance().LoadTexture(
            m_ShaderTexUpDown.szTexUpDownTex,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            EMMM_CHAR_MIPMAP,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_CHAR,
			true,
			FALSE );

		m_pEffect->SetFloat( "m_fTexUpDownBright", m_ShaderTexUpDown.fTexUpDownBright );
	}
	//////////////////////////////////////////////////////////////////////////
	// 오버레이 데이터 로드
	//////////////////////////////////////////////////////////////////////////
	if( strlen( m_ShaderOverlay.szOverlayTex ) != 0 && m_bUseOverlay )
	{
        m_textureResOverlay = TextureManager::GetInstance().LoadTexture(
            m_ShaderOverlay.szOverlayTex,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            EMMM_CHAR_MIPMAP,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_CHAR,
			true,
			FALSE );

		m_pEffect->SetFloat( "m_fSpecularLevel", m_ShaderOverlay.fSpecularLevel );
	}
	//////////////////////////////////////////////////////////////////////////
	// 리플렉션
	//////////////////////////////////////////////////////////////////////////
	if( strlen( m_ShaderReflection.szReflectTex ) != 0 && m_bUseReflect )
	{
        m_textureResReflect = TextureManager::GetInstance().LoadTexture(
            m_ShaderReflection.szReflectTex,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_CHAR,
			true,
			FALSE );

        if( strlen( m_ShaderReflection.szReflectAlphaTex ) != 0 && m_ShaderReflection.bUseReflectAlpha )
		{
            m_textureResReflectAlpha = TextureManager::GetInstance().LoadTexture(
                m_ShaderReflection.szReflectAlphaTex,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                EMMM_CHAR_MIPMAP,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_CHAR,
				true,
				FALSE );
		}
		m_pEffect->SetFloat( "m_fReflectivity", m_ShaderReflection.fReflectActivity );
		m_pEffect->SetBool( "m_bUseReflectAlpha", m_ShaderReflection.bUseReflectAlpha );
	}

	// m_pmcMesh
	IsExist_Copy_pmcMesh( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharTotalShader::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bPossibleEffect ) return S_OK;
	// 이펙트
	if(m_pEffect) m_pEffect->OnResetDevice();

	if( m_pmcMesh )
	{
		if ( !m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh )
		{
			m_pmcMesh->GenerateSkinnedMeshNormalMap( pd3dDevice );

			LPD3DXMESH pMesh;
			m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->CloneMesh( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->GetOptions(), dxeffchartotalshader::VertexDecl, pd3dDevice, &pMesh );
			D3DXComputeTangent( pMesh, 0, 0, 0, TRUE, NULL );

			SAFE_RELEASE( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh );
			m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh = pMesh;
		}
	}	

	return S_OK;
}

HRESULT DxEffCharTotalShader::InvalidateDeviceObjects ()
{
	if( !m_bPossibleEffect ) return S_OK;

	if( m_pEffect ) m_pEffect->OnLostDevice();

	return S_OK;
}

HRESULT DxEffCharTotalShader::DeleteDeviceObjects ()
{
	if( !m_bPossibleEffect ) return S_OK;

	SAFE_RELEASE( m_pEffect );
	SAFE_RELEASE( m_pNormalDCRT );

    m_textureResNormal.Clear();
    m_textureResSpecular.Clear();
    m_textureResGlossiness.Clear();
    m_textureResUpDown.Clear();
    m_textureResOverlay.Clear();
    m_textureResReflect.Clear();

	return S_OK;
}


HRESULT DxEffCharTotalShader::FrameMove ( float fTime, float fElapsedTime )
{
	if( !m_bPossibleEffect ) return S_OK;
	HRESULT hr = S_OK;

	if( m_ShaderTexUpDown.fTexUpDownAniSpeed == 0.0f )
	{
		m_fAniTexUpDownAlpha = m_ShaderTexUpDown.fTexUpDownAlpha;
	}else{
		m_fAniValue += m_ShaderTexUpDown.fTexUpDownAniSpeed * fElapsedTime;

		if( m_fAniValue >= 360 ) m_fAniValue = 0.0f;
		m_fAniTexUpDownAlpha = fabs( cosf(m_fAniValue) / m_ShaderTexUpDown.fTexUpDownAlpha );
	}

	if( g_bCHAR_EDIT_RUN )
	{
		if( DxInputDevice::GetInstance().GetKeyState( DIK_R ) & DXKEY_DOWNED )
		{						
			m_vAddLight.y += 1.0f;
		}else if( DxInputDevice::GetInstance().GetKeyState( DIK_F ) & DXKEY_DOWNED )
		{						
			m_vAddLight.y -= 1.0f;	
		}

		if( DxInputDevice::GetInstance().GetKeyState( DIK_A ) & DXKEY_DOWNED )
		{						
			m_vAddLight.x += 1.0f;
		}else if( DxInputDevice::GetInstance().GetKeyState( DIK_D ) & DXKEY_DOWNED )
		{						
			m_vAddLight.x -= 1.0f;
		}

		if( DxInputDevice::GetInstance().GetKeyState( DIK_W ) & DXKEY_DOWNED )
		{						
			m_vAddLight.z -= 1.0f;
		}else if( DxInputDevice::GetInstance().GetKeyState( DIK_S ) & DXKEY_DOWNED )
		{						
			m_vAddLight.z += 1.0f;
		}

		if( DxInputDevice::GetInstance().GetKeyState( DIK_Q ) & DXKEY_DOWNED )
		{						
			m_vAddLight = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}

	}

	return S_OK;
}

void DxEffCharTotalShader::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	if( !m_bPossibleEffect ) return;
	if( !m_pSkinMesh )		return;
	if( !m_pEffect )		return;

	if ( !IsExist_Copy_pmcMesh( pd3dDevice ) )
		return;

	if( m_bUseNormalMap )
	{
		if ( !m_textureResNormal.Update() )
			return;

		// 스펙큘라 맵 읽기
		if( strlen( m_ShaderNormalMap.szSpecularTex ) != 0 && m_ShaderNormalMap.bUseSpecularTex )
		{
			if ( !m_textureResSpecular.Update() )
				return;
		}

		if( strlen( m_ShaderNormalMap.szGlossinessTex ) != 0 && m_ShaderNormalMap.bUseGlossinessTex )
		{
			if ( !m_textureResGlossiness.Update() )
				return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 텍스쳐 업다운 데이터 로드
	//////////////////////////////////////////////////////////////////////////
	if( strlen( m_ShaderTexUpDown.szTexUpDownTex ) != 0 && m_bUseTexUpDown )
	{
		if ( !m_textureResUpDown.Update() )
			return;
	}
	//////////////////////////////////////////////////////////////////////////
	// 오버레이 데이터 로드
	//////////////////////////////////////////////////////////////////////////
	if( strlen( m_ShaderOverlay.szOverlayTex ) != 0 && m_bUseOverlay )
	{
		if ( !m_textureResOverlay.Update() )
			return;
	}
	//////////////////////////////////////////////////////////////////////////
	// 리플렉션
	//////////////////////////////////////////////////////////////////////////
	if( strlen( m_ShaderReflection.szReflectTex ) != 0 && m_bUseReflect )
	{
		if ( !m_textureResReflect.Update() )
			return;

		if( strlen( m_ShaderReflection.szReflectAlphaTex ) != 0 && m_ShaderReflection.bUseReflectAlpha )
		{
			if ( !m_textureResReflectAlpha.Update() )
				return;
		}
	}

	HRESULT hr;

	/*if( !m_bUseNormalMap && !m_bUseTexUpDown && !m_bUseReflect && !m_bUseOverlay ) return;
	if( m_bUseNormalMap && m_textureResNormal.GetTexture() == NULL ) return;
	if( m_bUseTexUpDown && strlen( m_ShaderTexUpDown.szTexUpDownTex ) == 0 ) return;
	if( m_bUseReflect   && strlen( m_ShaderReflection.szReflectTex ) == 0 ) return;
	if( m_bUseOverlay   && strlen( m_ShaderOverlay.szOverlayTex ) == 0 ) return;*/


	D3DXMATRIX matView, matProj, mat, matRotate;
	//pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	//pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	matView = NSMaterialSkinManager::GetMatrixView();
	matProj = NSMaterialSkinManager::GetMatrixProjection();

	// 정점 선언
	pd3dDevice->SetVertexDeclaration( m_pNormalDCRT );	

	V( m_pEffect->SetTechnique( m_hTechnique ) );
	// 로컬 - 투영 변환 행렬
	V( m_pEffect->SetMatrix( m_hmWorld, &m_matWorld ) );
	V( m_pEffect->SetMatrix( m_hmView, &matView ) );
	V( m_pEffect->SetMatrix( m_hmProj, &matProj ) );


	// 조명 방향(로컬 좌표계)
	D3DXVECTOR4 v;

	D3DLIGHT9 light;
	pd3dDevice->GetLight( 0, &light );

	mat = m_matWorld * matView * matProj;
	D3DXVECTOR4 light_pos = D3DXVECTOR4( light.Direction.x + m_vAddLight.x, light.Direction.y + m_vAddLight.y, light.Direction.z + m_vAddLight.z, 0);
	D3DXMatrixInverse( &mat, NULL, &m_matWorld);
	D3DXVec4Transform( &v, &light_pos, &mat );	
	D3DXVec3Normalize( (D3DXVECTOR3 *)&v, (D3DXVECTOR3 *)&v );

	D3DXVECTOR4 vAmbient, vDiffuse, vSpecular, vInputLightPos;

	if( g_bCHAR_EDIT_RUN )
	{
		D3DXVECTOR3 lightDir = D3DXVECTOR3( light_pos.x, light_pos.y, light_pos.z );
		EDITMESHS::RENDERSPHERE( pd3dDevice, lightDir, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), FALSE, NULL, 0xffff0000 );

		CDebugSet::ToView( 0, 2, "Light_pos %fx %fy %fz", light_pos.x, light_pos.y, light_pos.z );
		CDebugSet::ToView( 0, 3, "vpos %fx %fy %fz", m_matWorld._41, m_matWorld._42, m_matWorld._43 );


		vInputLightPos = D3DXVECTOR4( light_pos.x, light_pos.y, light_pos.z, 0.0f );

		vAmbient = D3DXVECTOR4(m_cAmbient.r,m_cAmbient.g,m_cAmbient.b,m_cAmbient.a);
		vDiffuse = D3DXVECTOR4(m_cDiffuse.r,m_cDiffuse.g,m_cDiffuse.b,m_cDiffuse.a);
		vSpecular = D3DXVECTOR4(m_cSpecular.r,m_cSpecular.g,m_cSpecular.b,m_cSpecular.a);


		
	}else{
		vInputLightPos = D3DXVECTOR4( -light_pos.x, light_pos.y, -light_pos.z, 0.0f );
		D3DXVECTOR4 vLightAmbient  = D3DXVECTOR4( light.Ambient.r, light.Ambient.g, light.Ambient.b, light.Ambient.a );
		D3DXVECTOR4 vLightDiffuse  = D3DXVECTOR4( light.Diffuse.r, light.Diffuse.g, light.Diffuse.b, light.Diffuse.a );
		D3DXVECTOR4 vLightSpecular = D3DXVECTOR4( light.Specular.r, light.Specular.g, light.Specular.b, light.Specular.a );

		V( m_pEffect->SetBool( m_hbUseDirectionLight, TRUE ) );

		D3DLIGHTQ	pLight;
		D3DXVECTOR3 vLength;
		float fLength, fMinLength = 1000.0f; 
		D3DXVECTOR3 vPos = D3DXVECTOR3( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
		for ( int i=0; i<7; i++ )
		{
			if ( !DxLightMan::GetInstance()->GetClosedLight(i+1) ) continue;
			pLight = DxLightMan::GetInstance()->GetClosedLight(i+1)->m_Light;
			vLength = vPos - D3DXVECTOR3( pLight.Position.x, pLight.Position.y, pLight.Position.z );
			fLength = D3DXVec3Length( &vLength );
			if( fLength <= pLight.Range && ( fMinLength == 1000.0f || fMinLength > fLength ) )
			{
				vInputLightPos = D3DXVECTOR4( pLight.Position.x, pLight.Position.y, pLight.Position.z, 0.0f );
				fMinLength = fLength;

				vLightAmbient  = D3DXVECTOR4( pLight.Ambient.r, pLight.Ambient.g, pLight.Ambient.b, pLight.Ambient.a );
				vLightDiffuse  = D3DXVECTOR4( pLight.Diffuse.r, pLight.Diffuse.g, pLight.Diffuse.b, pLight.Diffuse.a );
				vLightSpecular = D3DXVECTOR4( pLight.Specular.r, pLight.Specular.g, pLight.Specular.b, pLight.Specular.a );
				V( m_pEffect->SetBool( m_hbUseDirectionLight, FALSE ) );
			}	
		}

		m_ShaderOverlay.cColor1 = m_pCharPart->GetHairColor();
		m_ShaderOverlay.cColor2 = m_pCharPart->GetSubColor();

		vAmbient  = ( D3DXVECTOR4(m_cAmbient.r,m_cAmbient.g,m_cAmbient.b,m_cAmbient.a) + vLightAmbient ) / 2.0f;
		vDiffuse  = ( D3DXVECTOR4(m_cDiffuse.r,m_cDiffuse.g,m_cDiffuse.b,m_cDiffuse.a) + vLightDiffuse ) / 2.0f;
		vSpecular = ( D3DXVECTOR4(m_cSpecular.r,m_cSpecular.g,m_cSpecular.b,m_cSpecular.a) + vLightSpecular ) / 2.0f;
	}

	V( m_pEffect->SetVector( m_htAmbient, &vAmbient ) );
	V( m_pEffect->SetVector( m_htDiffuse, &vDiffuse ) );
	V( m_pEffect->SetVector( m_htSpecular, &vSpecular ) );	
	V( m_pEffect->SetVector( m_hvLightDir, &vInputLightPos ) );

	// 시전(로컬 좌표계)
	mat = m_matWorld * matView;
	D3DXMatrixInverse( &mat, NULL, &mat );
	v = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
	D3DXVec4Transform( &v, &v, &mat );
	V( m_pEffect->SetVector( m_hvEyePos, &v ) );


	// 노말맵
	if( m_bUseNormalMap )
	{
		V( m_pEffect->SetTexture( m_htNormalMap, m_textureResNormal.GetTexture() ) );
		// 스펙큘라 맵
		if( m_ShaderNormalMap.bUseSpecularTex )
		{
			V( m_pEffect->SetTexture( m_htSpecularTex, m_textureResSpecular.GetTexture() ) );
		}

		// 글로시니스 맵
		if( m_ShaderNormalMap.bUseGlossinessTex )
		{
			V( m_pEffect->SetTexture( m_htGlossinessTex, m_textureResGlossiness.GetTexture() ) );
		}
	}

	// 글로우 맵
	if( m_bUseTexUpDown )
	{
		V( m_pEffect->SetTexture( m_htTexUpDownTex, m_textureResUpDown.GetTexture() ) );
		V( m_pEffect->SetFloat( m_hfTexUpDownAlpha, m_fAniTexUpDownAlpha ) );
	}

	// 오버레이
	if( m_bUseOverlay )
	{

		D3DXVECTOR4 vColor1 = D3DXVECTOR4(m_ShaderOverlay.cColor1.r,m_ShaderOverlay.cColor1.g,m_ShaderOverlay.cColor1.b,m_ShaderOverlay.cColor1.a);
		D3DXVECTOR4 vColor2 = D3DXVECTOR4(m_ShaderOverlay.cColor2.r,m_ShaderOverlay.cColor2.g,m_ShaderOverlay.cColor2.b,m_ShaderOverlay.cColor2.a);

		V( m_pEffect->SetTexture( m_htOverlayTex, m_textureResOverlay.GetTexture() ) );
		V( m_pEffect->SetVector( m_hvInputColor1, &vColor1 ) );
		V( m_pEffect->SetVector( m_hvInputColor2, &vColor2 ) );
	}

	// 리플렉션
	if( m_bUseReflect )
	{
		m_pEffect->SetTexture( m_htReflectTex, m_textureResReflect.GetCubeTexture() );
		if( m_ShaderReflection.bUseReflectAlpha )
		{
			m_pEffect->SetTexture( m_htReflectAlphaTex, m_textureResReflectAlpha.GetTexture() );
		}
	}

	const D3DXMATRIXA16*	pBoneMatrices(NULL);
	SMATERIAL_PIECE*		pMaterialPiece = NULL;
	if ( m_pCharPart )
	{
		pBoneMatrices = m_pCharPart->GetBoneMatrices();
		if ( !pBoneMatrices )
			return;

		pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
	}
	else if ( m_pSkinPiece )
	{
		pBoneMatrices = DxSkinMesh9::g_pBoneMatrices;
		pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;
	}

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;		
	sCharSetting.pmtrlSpecular		= NULL;
	sCharSetting.bWorldIdentity		= TRUE;
	sCharSetting.emRDOP				= CTOP_BASE;

	int nOverlayMaterial = -1;
	if( m_bUseOverlay )
	{
		nOverlayMaterial = m_ShaderOverlay.dwSelectMaterial;
	}

	// 현재는 Fog 가 Enable 로 되어있다.
	// 하지만 DxEffCharNormal 는  제작시 Fog 고려 안하고 만들었고, 많이 사용되지 않을 예정이다. (MaterialSystem 때문에)
	// 그래서 강제로 Fog 를 끄는 방향으로 하였다.
	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	m_pSkinMesh->DrawMeshTotalShader( pd3dDevice, m_pmcMesh, pBoneMatrices, m_pEffect, sCharSetting.pmtrlPiece, nOverlayMaterial );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
}


HRESULT	DxEffCharTotalShader::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );
	}
	SFile.EndBlock();

	return S_OK;
}

HRESULT DxEffCharTotalShader::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}



HRESULT	DxEffCharTotalShader::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case VERSION:	LoadFile_0100( SFile, pd3dDevice );	break;
	default:
		{
			//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );

			return E_FAIL;
		}
		break;
	};

	//	Note : Device 자원을 생성한다.
	HRESULT hr(S_OK);
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

BOOL DxEffCharTotalShader::IsExist_Copy_pmcMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 값이 있다면 또 작업하지 않는다.
	if ( m_pmcMesh )
		return TRUE;

	if ( m_pCharPart )
	{
		m_pmcMesh = m_pCharPart->m_rMeshContainer;
	}
	else if ( m_pSkinPiece )
	{
		m_pmcMesh = m_pSkinPiece->m_pmcMesh;
	}

	if ( !m_pmcMesh )
		return FALSE;

	if ( !m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh )
	{
		m_pmcMesh->GenerateSkinnedMeshNormalMap( pd3dDevice );

		LPD3DXMESH pMesh;
		m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->CloneMesh( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->GetOptions(), dxeffchartotalshader::VertexDecl, pd3dDevice, &pMesh );
		D3DXComputeTangent( pMesh, 0, 0, 0, TRUE, NULL );

		SAFE_RELEASE( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh );
		m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh = pMesh;
	}

	return TRUE;
}