#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxEffect/Single/DxEffSinglePropGMan.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/DxLoadShader.h"
#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/editmeshs.h"
#include "./DxEffCharAmbient.h"
#include "./DxEffCharHLSL.h"

#include "dxeffcharoverlay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharOverlay::TYPEID			= EMEFFCHAR_OVERLAY;
const DWORD	DxEffCharOverlay::VERSION			= 0x0104;
char		DxEffCharOverlay::NAME[MAX_PATH]	= "0.Overlay";

extern BOOL		g_bCHAR_EDIT_RUN;


//-------------------------------------------------------------
// 정점선언
//-------------------------------------------------------------
namespace dxeffcharoverlay
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
} // namespace dxeffcharoverlay

static const char* pOverlayHLSL = 
{
	"texture m_ColorTexture;         \r\n"
	"texture m_MeshTexture;          \r\n"
	"texture m_ReflectTexture;       \r\n"
	"texture m_GlowTex;				 \r\n"
	"texture m_AlphaReflectTex;		 \r\n"

	// 좌표변환 월드 행렬
	"float4x4 mWorld : WORLD;	\r\n"
	// 좌표변환 프로젝션 행렬
	"float4x4 mProj  : PROJECTION;		\r\n"
	// 좌표변환 뷰 행렬
	"float4x4 mView  : VIEW;		\r\n"

	// 카메라위치(로컬좌표계)
	"float4   m_vEyePos;		\r\n"
	//"float3   m_vEyePos;		\r\n"

	"bool	  m_bUseSpecular;\r\n"
	"float4   m_fvInputColor1;\r\n"
	"float4   m_fvInputColor2;\r\n"
	"float4   m_fvLightDir;\r\n"

	// 광원의 밝기
	"float4 m_fvAmbient;    // 주변광\r\n"
	"float4 m_fvDiffuse;    // 확산광\r\n"
	"float4 m_fvSpecular;    // 반사광\r\n"

	"bool	m_bUseDirectionLight;\r\n"
	"bool   m_bUseGlowTex;\r\n"
	"float  m_fAniGlowAlpha;\r\n"
	"float  m_fGlowBright;\r\n"
	"bool   m_bUseReflect;\r\n"
	"bool   m_bUseAlphaReflect;\r\n"
	"float  m_fReflectivity;\r\n"

	"float  m_fSpecularLevel;    // 반사광\r\n"
	"float  m_fSpecularPower;    // 반사광\r\n"
	"float	m_fAlphaRef;		 // Alpha Ref\r\n"
	"static const int MAX_MATRICES = 60;\r\n"
	"float4x3    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"
	"float4x4 worldInverse;\r\n" // by luxes.
	"float4x4 worldInverseTrans;\r\n" // by luxes.
	"float4x4 viewInverse;\r\n" // by luxes.

	//--------------------------------------------------------------------------------------
	// Texture samplers
	//--------------------------------------------------------------------------------------
	"sampler ColorTextureSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <m_ColorTexture>;\r\n"
	"	MipFilter = LINEAR;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"};\r\n"

	"sampler MeshTextureSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <m_MeshTexture>;\r\n"
	"	MipFilter = LINEAR;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"};\r\n"

	"sampler ReflectTextureSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <m_ReflectTexture>;\r\n"
	"	MipFilter = LINEAR;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"};\r\n"


	"sampler AlphaReflectTextureSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <m_AlphaReflectTex>;\r\n"
	"	MipFilter = LINEAR;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"};\r\n"
	
	// -------------------------------------------------------------
	// 글로우 텍스처
	"sampler GlowSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture = <m_GlowTex>;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"	MipFilter = LINEAR;\r\n"

	/*"	AddressU = Clamp;\r\n"
	"	AddressV = Clamp;\r\n"*/
	"};\r\n"

	// -------------------------------------------------------------
	// 정점셰이더에서 받아오는 데이터
	// -------------------------------------------------------------
	"struct VS_INPUT\r\n"
	"{\r\n"
	"	float4 Position	: POSITION0; // 로컬 좌표\r\n"
	"	float4 BlendWeights    : BLENDWEIGHT;\r\n"
	"	float4 BlendIndices    : BLENDINDICES;\r\n"	
	"	float3 Normal	: NORMAL0; // 법선 벡터\r\n"
	"	float3 Tangent	: TANGENT0; // 접선 벡터\r\n"
	"	float2 Texcoord : TEXCOORD0; // 텍스쳐 좌표  \r\n"  
	"	float3 Binormal : BINORMAL0;  \r\n"  
	"};\r\n"

	//--------------------------------------------------------------------------------------
	// Vertex shader output structure
	//--------------------------------------------------------------------------------------
	"struct VS_OUTPUT\r\n"
	"{\r\n"
	"	float4 Position			: POSITION;   // vertex position \r\n"
	"	float4 Diffuse			: COLOR0;\r\n"
	"	float4 Specular			: COLOR1;\r\n"
	"	float2 TextureUV		: TEXCOORD0;  // vertex texture coords \r\n"
	"	float  SpecularPower	: TEXCOORD1; // by luxes. \r\n"
	"	float  SpecLevel		: TEXCOORD2; // by luxes. \r\n"
	"	float3 Reflection		: TEXCOORD3; // by luxes. \r\n"
	"	float3 ViewDirection	: TEXCOORD4; // by luxes. \r\n"
	"	float3 LightDirection	: TEXCOORD5; // by luxes. \r\n"
	"};\r\n"


	//--------------------------------------------------------------------------------------
	// This shader computes standard transform and lighting
	//--------------------------------------------------------------------------------------
	"VS_OUTPUT RenderSceneVS( VS_INPUT Input, uniform int NumBones)"
	"{\r\n"
	"	VS_OUTPUT Output = (VS_OUTPUT)0;\r\n"


	"		float3      Pos = 0.0f;\r\n"
	"		float3      Normal = 0.0f;\r\n"
	"		float3      Binormal = 0.0f;\r\n"

	"		float       LastWeight = 0.0f;\r\n"

	"		int4 IndexVector = D3DCOLORtoUBYTE4(Input.BlendIndices);\r\n"
	"		float BlendWeightsArray[4] = (float[4])Input.BlendWeights;\r\n"
	"		int   IndexArray[4]        = (int[4])IndexVector;\r\n"

	"		for (int iBone = 0; iBone < NumBones-1; iBone++)\r\n"
	"		{\r\n"
	"			LastWeight = LastWeight + BlendWeightsArray[iBone];\r\n"

	"			Pos += mul(Input.Position, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
	"			Normal += mul(Input.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
	"			Binormal += mul(Input.Binormal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
	"		}\r\n"
	"		LastWeight = 1.0f - LastWeight; \r\n"

	"		Pos += (mul(Input.Position, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);\r\n"
	"		Normal += (mul(Input.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); \r\n"
	"		Binormal += (mul(Input.Binormal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); \r\n"

	"		Output.Position = mul(float4(Pos.xyz, 1.0f), mProj);\r\n"

	"		float3 fvObjectPosition = mul( Input.Position, mWorld );\r\n"

	// 반사광(specular). by luxes.
	"	float fSpecPower = m_fSpecularPower * 0.1f; \r\n"
	"	float fSpecLevel = ( (m_fSpecularLevel*0.1) );\r\n"
	"	Output.SpecularPower = fSpecPower;\r\n"
	"	Output.SpecLevel = fSpecLevel;\r\n"

		// Just copy the texture coordinate through
	"	Output.TextureUV = Input.Texcoord; \r\n"

	//	Note : 광원 벡터와 시선 벡터의 계산. by luxes.
	"	float3x3 objTangent;\r\n"
	"	objTangent[ 0 ] = Input.Binormal.xyz;\r\n"
	"	objTangent[ 1 ] = -Input.Tangent.xyz;\r\n"
	"	objTangent[ 2 ] = Input.Normal.xyz;\r\n"

	"	if( !m_bUseDirectionLight )\r\n"
	"		m_fvLightDir.xyz = m_fvLightDir.xyz - fvObjectPosition;		// 광원벡터\r\n"

	"	float3 lightVec = mul( objTangent, ( mul( m_fvLightDir, worldInverse ).xyz ) );\r\n"
	//"	float3 lightVec = mul( objTangent, ( mul( ( m_fvLightDir - fvObjectPosition ), worldInverse ).xyz ) );\r\n"
	"	Output.LightDirection = lightVec;\r\n"
	"	Output.LightDirection = normalize( Output.LightDirection );\r\n"

	"	float4 osPos = mul ( viewInverse[ 3 ], worldInverse );\r\n"
	"	float3 osVec = osPos.xyz - Input.Position.xyz;\r\n"
	"	Output.ViewDirection = mul( objTangent, osVec );\r\n"
	"	Output.ViewDirection = normalize( Output.ViewDirection );\r\n"

	//	Note : 반사 벡터 좌표 계산. by luxes.
	"	float3 viewDir = mul( Input.Position, mWorld ) - viewInverse[ 3 ];\r\n"

	"	float3 worldNormal = normalize( mul( Input.Normal, worldInverseTrans ).xyz );\r\n"
	"	float3 worldBinormal = normalize( mul( Input.Binormal, worldInverseTrans ).xyz );\r\n"
	"	float3 worldTangent = normalize( mul( Input.Tangent, worldInverseTrans ).xyz );\r\n"

	"	float3 fvNormal = float3( 0.0, 0.0, 1.0 );\r\n"

	"	float3 WorldXformNormal = ( worldNormal * fvNormal.z ) + ( fvNormal.x * worldBinormal ) + ( fvNormal.y * -worldTangent );\r\n"
	"	float3 Reflect = reflect( viewDir, WorldXformNormal );\r\n"
	"	float3 Reflection = Reflect.xyz;\r\n"
	"	Reflection.xyz = Reflection.xzy;\r\n"
	"	Output.Reflection = Reflection;\r\n"

	"	return Output;    \r\n"
	"}\r\n"




	//--------------------------------------------------------------------------------------
	// This shader outputs the pixel's color by modulating the texture's
	// color with diffuse material color
	//--------------------------------------------------------------------------------------
	"float4 RenderScenePS( VS_OUTPUT In )  : COLOR0\r\n"
	"{ \r\n"

	"	float4 AreaColor = (tex2D(ColorTextureSampler, In.TextureUV));    \r\n"
	"	float4 MeshColor = (tex2D(MeshTextureSampler, In.TextureUV));    \r\n"
	"	float3 Color1, Color2;       \r\n"

	"	Color1 = ( 2 * MeshColor.rgb ) * m_fvInputColor1.rgb;\r\n"
	"	Color2 = ( 2 * MeshColor.rgb ) * m_fvInputColor2.rgb;\r\n"

	"	if ( MeshColor.r > 0.5 )\r\n"
	"		Color1.r = 1 - ( 1 - ( 2 * (MeshColor.r-0.5) ) ) * (1-m_fvInputColor1.r);\r\n"

	"	if ( MeshColor.g > 0.5 )\r\n"
	"		Color1.g = 1 - ( 1 - ( 2 * (MeshColor.g-0.5) ) ) * (1-m_fvInputColor1.g);\r\n"

	"	if ( MeshColor.b > 0.5 )\r\n"
	"		Color1.b = 1 - ( 1 - ( 2 * (MeshColor.b-0.5) ) ) * (1-m_fvInputColor1.b);\r\n"

	"	if ( MeshColor.r > 0.5 )\r\n"
	"		Color2.r = 1 - ( 1 - ( 2 * (MeshColor.r-0.5) ) ) * (1-m_fvInputColor2.r);\r\n"

	"	if ( MeshColor.g > 0.5 )\r\n"
	"		Color2.g = 1 - ( 1 - ( 2 * (MeshColor.g-0.5) ) ) * (1-m_fvInputColor2.g);\r\n"

	"	if ( MeshColor.b > 0.5 )\r\n"
	"		Color2.b = 1 - ( 1 - ( 2 * (MeshColor.b-0.5) ) ) * (1-m_fvInputColor2.b);\r\n"

	"		float4 fvGlowColor =  float4(0.0,0.0,0.0,0.0);\r\n"
	"		if( m_bUseGlowTex )\r\n"
	"		{\r\n"
	"			fvGlowColor		= tex2D( GlowSamp, In.TextureUV).rgba;\r\n"
	"			fvGlowColor     -= (1.0f - m_fAniGlowAlpha );\r\n"
	"			fvGlowColor		= max(0.0f, fvGlowColor);\r\n"
	"			fvGlowColor		*= m_fGlowBright;\r\n"
	"		}\r\n"

	"	float3 DestColor = MeshColor.rgb - ( ( MeshColor.rgb * AreaColor.r ) + ( MeshColor.rgb * AreaColor.b ) );\r\n"

	"	float3 Overlay = ( Color1.rgb * AreaColor.r ) + ( Color2.rgb * AreaColor.b );\r\n"

	"	float4 Ambient;\r\n"
	"	Ambient.rgb = DestColor.rgb + Overlay.rgb;\r\n"
	"	Ambient.a = 1.0f;    	\r\n"

	"	float4 Color, Specular;\r\n"

	//	Note : diffuse와 specular 계산. by luxes.
	"	float3 N = float3( 0.0, 0.0, 1.0 );\r\n"
	"	float NDotL = dot( N, In.LightDirection );\r\n"
	"	float3 H = normalize( In.LightDirection + In.ViewDirection );\r\n"
	"	float NdotH = saturate( dot( N, H ) );\r\n"

	"	float4 diffuse = m_fvAmbient + m_fvDiffuse * saturate( NDotL );\r\n"

	"	if ( m_bUseSpecular )\r\n"
	"	{\r\n"
	"		Specular = m_fvSpecular * pow( NdotH, In.SpecularPower ) * AreaColor.g;\r\n"
	"	}\r\n"
	"	else\r\n"
	"		Specular = m_fvSpecular * pow( NdotH, In.SpecularPower ) * In.SpecLevel;\r\n"

	"	Color =  Ambient * diffuse + Specular + fvGlowColor;\r\n"

	"	float4 OutputColor = Color;\r\n"

	"   if( m_bUseReflect )	  \r\n"
	"	{\r\n"
	"		float fReflectAlpha = 1.0f;	  \r\n"	
	"		if( m_bUseAlphaReflect )	  \r\n"	
	"		{\r\n"	
	"			fReflectAlpha = tex2D(AlphaReflectTextureSampler, In.TextureUV).a;    \r\n"
	"		}\r\n"
	"		if ( fReflectAlpha > 0.5 )\r\n"
	"		{\r\n"
	"			float3 ReflectColor = texCUBE(ReflectTextureSampler, In.Reflection).xyz * fReflectAlpha;    \r\n"
	"			OutputColor.xyz = lerp( ReflectColor.rgb, Color.rgb, 1-m_fReflectivity );\r\n"
	"		}\r\n"
	"	}\r\n"
	"	if ( MeshColor.a > m_fAlphaRef / 255.0f )\r\n"
	"		OutputColor.a = 1.0f;\r\n"
	"	else\r\n"
	"		OutputColor.a = 0.0f;\r\n"
	"	return OutputColor;\r\n"
	"}\r\n"

	"	int CurNumBones = 2;\r\n"
	"	VertexShader vsArray[4] = { compile vs_2_0 RenderSceneVS(1), \r\n"
	"								compile vs_2_0 RenderSceneVS(2),\r\n"
	"								compile vs_2_0 RenderSceneVS(3),\r\n"
	"								compile vs_2_0 RenderSceneVS(4)\r\n"
	"	};\r\n"

	//--------------------------------------------------------------------------------------
	// Renders scene 
	//--------------------------------------------------------------------------------------
	"technique RenderScene\r\n"
	"{\r\n"
	"	pass P0\r\n"
	"	{          \r\n"	
	"		VertexShader = (vsArray[CurNumBones]);\r\n"
	"		PixelShader  = compile ps_2_0 RenderScenePS();\r\n"

	"		ZEnable = true;\r\n"
	"		ZWriteEnable = true;\r\n"
	"		ShadeMode = Gouraud;\r\n"
	"		SrcBlend = SrcAlpha;\r\n"
	"		DestBlend = InvSrcAlpha;\r\n"
	"		AlphaBlendEnable = true;\r\n"
	"		AlphaTestEnable = true;\r\n"
	//"		AlphaTestEnable = false;\r\n"
	//"		AlphaBlendEnable = FALSE;\r\n"
	//"		AlphaRef = m_fAlphaRef;\r\n"
	//"		AlphaFunc = GreaterEqual;\r\n"
	"	}\r\n"
	"}\r\n"

};

ID3DXEffect*	DxEffCharOverlay::s_pEffect = NULL;	// D3DX effect interface

DxEffCharOverlay& DxEffCharOverlay::GetInstance()
{
	static DxEffCharOverlay Instance;
	return Instance;
}

DxEffCharOverlay::DxEffCharOverlay(void)
    : DxEffChar()
    , m_cColor1(D3DCOLOR_ARGB(255,128,128,128))
    , m_cColor2(D3DCOLOR_ARGB(255,128,128,128))
    , m_cDiffuse(D3DCOLOR_ARGB(0,255,255,255))
    , m_cSpecular(D3DCOLOR_ARGB(0,255,255,255))
    , m_cAmbient(D3DCOLOR_ARGB(0,255,255,255))
    , m_bPossibleEffect(TRUE)
    , m_hTechnique(NULL)	
    , m_hmWVP(NULL)			
    , m_htColorTex(NULL)
    , m_hvLightDir(NULL)	
    , m_pDecl(NULL)
    , m_hvInputColor1(NULL)
    , m_hfSpecularPower(NULL)
    , m_hfSpecularLevel(NULL)
    , m_hfAlphaRef(NULL)
    , m_hvInputColor2(NULL)
    , m_hvAmbient(NULL)
    , m_hvDiffuse(NULL)
    , m_hvSpecular(NULL)
    , m_htWorld(NULL)
    , m_htProj(NULL)
    , m_htView(NULL)
    , m_htGlowTex(NULL)
    , m_htGlowAlpha(NULL)
    , m_htReflectTex(NULL)
    , m_htAlphaReflectTex(NULL)
    , m_hbUseDirectionLight(NULL)
    , m_hvEyePos(NULL)
    , m_hvWorldInv(NULL)
    , m_hvWorldInvTrans(NULL)
    , m_hvViewInv(NULL)
    , m_fAniGlowAlpha(0.0f)
    , m_fAniValue(0.0f)
    , m_vAddLight(0.0f,0.0f,0.0f)
    , m_pSkinMesh(NULL)
    , m_pmcMesh(NULL)
	, m_bNewMaterialMode(TRUE)
{
	ZeroMemory( m_szOverlayTex, MAX_PATH );
}

DxEffCharOverlay::~DxEffCharOverlay(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharOverlay::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		return NULL;
	}

	HRESULT hr;
	DxEffCharOverlay *pEffChar = new DxEffCharOverlay;
	pEffChar->SetFileName ( GetFileName() );
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );
	for ( int i = 0; i < MAX_MATERIAL_OVERLAY; i++ )
	{
		pEffChar->SetOverlayProperty( &m_PropertyOverlay[ i ], i );
	}

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	pEffChar->m_bNewMaterialMode = m_bNewMaterialMode;

	return pEffChar;
}

HRESULT DxEffCharOverlay::StaticInitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	LPD3DXBUFFER pErr = NULL;	
    HRESULT hr = D3DXCreateEffect(
        pd3dDevice,
        pOverlayHLSL,
        (UINT)strlen( pOverlayHLSL ),
        NULL,
        NULL,
        D3DXFX_NOT_CLONEABLE,
        NULL,
        &s_pEffect,
        &pErr );

	if( FAILED( hr ) )
	{
		CDebugSet::ToListView(
            "[ERROR]D3DXCreateEffect FAILED ERROR: %s",
            pErr->GetBufferPointer() );

#ifdef _RELEASED
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
#endif

		SAFE_RELEASE( pErr );
		return E_FAIL;
	}

    return hr;
}

HRESULT DxEffCharOverlay::StaticRestoreDeviceObjects()
{
	if( s_pEffect )
        s_pEffect->OnResetDevice();

    return S_OK;
}

HRESULT DxEffCharOverlay::StaticInvalidateDeviceObjects()
{
    if( s_pEffect )
        s_pEffect->OnLostDevice();

    return S_OK;
}

HRESULT DxEffCharOverlay::StaticDeleteDeviceObjects()
{
    SAFE_RELEASE( s_pEffect );

    return S_OK;
}

HRESULT DxEffCharOverlay::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

    if( !s_pEffect )
        return S_OK;

	if ( !DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader() )	// PS.2.0 하드웨어 가속이 가능 할 경우만 로드
	{
		m_bPossibleEffect = FALSE;
		return E_FAIL;
	}

	// 정점선언 오브젝트 생성
    V_RETURN( pd3dDevice->CreateVertexDeclaration( dxeffcharoverlay::VertexDecl, &m_pDecl ) );

	m_hTechnique	= s_pEffect->GetTechniqueByName( "TShader" );
	m_hmWVP			= s_pEffect->GetParameterByName( NULL, "m_mWorldViewProjection");
	m_htColorTex	= s_pEffect->GetParameterByName( NULL, "m_ColorTexture" );
	m_hvLightDir	= s_pEffect->GetParameterByName( NULL, "m_fvLightDir" );

	m_hfSpecularLevel= s_pEffect->GetParameterByName( NULL, "m_fSpecularLevel" );
	m_hfSpecularPower= s_pEffect->GetParameterByName( NULL, "m_fSpecularPower" );
	m_hfAlphaRef	 = s_pEffect->GetParameterByName( NULL, "m_fAlphaRef" );

	m_hvInputColor1 = s_pEffect->GetParameterByName( NULL, "m_fvInputColor1" );
	m_hvInputColor2 = s_pEffect->GetParameterByName( NULL, "m_fvInputColor2" );

	m_htWorld		= s_pEffect->GetParameterByName( NULL, "mWorld" );
	m_htProj		= s_pEffect->GetParameterByName( NULL, "mProj" );
	m_htView		= s_pEffect->GetParameterByName( NULL, "mView" );

	m_hvAmbient		= s_pEffect->GetParameterByName( NULL, "m_fvAmbient" );
	m_hvDiffuse		= s_pEffect->GetParameterByName( NULL, "m_fvDiffuse" );
	m_hvSpecular	= s_pEffect->GetParameterByName( NULL, "m_fvSpecular" );

	m_htGlowTex		= s_pEffect->GetParameterByName( NULL, "m_GlowTex" );
	m_htGlowAlpha	= s_pEffect->GetParameterByName( NULL, "m_fAniGlowAlpha" );

	m_hvEyePos		= s_pEffect->GetParameterByName( NULL, "m_vEyePos" );

	m_htReflectTex		= s_pEffect->GetParameterByName( NULL, "m_ReflectTexture" );
	m_htAlphaReflectTex	= s_pEffect->GetParameterByName( NULL, "m_AlphaReflectTex" );

	m_hbUseDirectionLight = s_pEffect->GetParameterByName( NULL, "m_bUseDirectionLight" );

	m_hvWorldInv = s_pEffect->GetParameterByName( NULL, "worldInverse" );
	m_hvWorldInvTrans = s_pEffect->GetParameterByName( NULL, "worldInverseTrans" );
	m_hvViewInv = s_pEffect->GetParameterByName( NULL, "viewInverse" );

	s_pEffect->SetBool( "m_bUseSpecular", m_bUseSpecular ); // by luxes.

	StringCchCopy( m_PropertyOverlay[ m_dwSelectMaterial ].m_szOverlayTex, MAX_PATH, m_szOverlayTex ); // overlay value setting. by luxes.
	StringCchCopy( m_PropertyOverlay[ m_dwSelectMaterial ].m_szGlowTex, MAX_PATH, m_szGlowTex ); // overlay value setting. by luxes.
	StringCchCopy( m_PropertyOverlay[ m_dwSelectMaterial ].m_szReflectTex, MAX_PATH, m_szReflectTex ); // overlay value setting. by luxes.
	StringCchCopy( m_PropertyOverlay[ m_dwSelectMaterial ].m_szReflectAlphaTex, MAX_PATH, m_szReflectAlphaTex ); // overlay value setting. by luxes.

	m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseGlowTex = ( m_Property.m_bUseGlowTex == 0 ? FALSE : TRUE );
	m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseReflectTex = ( m_Property.m_bUseReflect == 0 ? FALSE : TRUE );
	m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseAlphaReflectTex = ( m_Property.m_bUseAlphaReflect == 0 ? FALSE : TRUE );

	


	for ( int i = 0; i < MAX_MATERIAL_OVERLAY; i++ )
	{
		if( strlen( m_PropertyOverlay[ i ].m_szGlowTex ) != 0 && m_PropertyOverlay[ i ].m_bUseGlowTex )
		{
			m_PropertyOverlay[ i ].m_textureResGlow = TextureManager::GetInstance().LoadTexture(
				m_PropertyOverlay[ i ].m_szGlowTex,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				EMMM_CHAR_MIPMAP,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_CHAR,
				true,
				FALSE );

			s_pEffect->SetFloat( "m_fGlowBright", m_fGlowBright );
			s_pEffect->SetFloat( m_htGlowAlpha, m_fAniGlowAlpha );
		}
		else
		{
			m_PropertyOverlay[ i ].m_textureResGlow.Clear();
		}
		s_pEffect->SetBool( "m_bUseGlowTex", m_bUseGlowTex );

		if( strlen( m_PropertyOverlay[ i ].m_szReflectTex ) != 0 && m_PropertyOverlay[ i ].m_bUseReflectTex )
		{
			m_PropertyOverlay[ i ].m_textureResReflect = TextureManager::GetInstance().LoadTexture(
				m_PropertyOverlay[ i ].m_szReflectTex,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE,
				0,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_CHAR,
				true,
				FALSE );

			s_pEffect->SetFloat( "m_fReflectivity", m_fReflectActivity );
		}
		else
		{
			m_PropertyOverlay[ i ].m_textureResReflect.Clear();
		}
		s_pEffect->SetBool( "m_bUseReflect", m_bUseReflect );


		LoadOverlayMaskTexture( i );


		if( strlen( m_PropertyOverlay[ i ].m_szReflectAlphaTex ) != 0 && m_PropertyOverlay[ i ].m_bUseAlphaReflectTex )
		{
			m_PropertyOverlay[ i ].m_textureResAlphaReflect = TextureManager::GetInstance().LoadTexture(
				m_PropertyOverlay[ i ].m_szReflectAlphaTex,
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
			m_PropertyOverlay[ i ].m_textureResAlphaReflect.Clear();
		}
		s_pEffect->SetBool( "m_bUseAlphaReflect", m_bUseAlphaReflect );
	}

	for ( int i = 0; i < MAX_MATERIAL_OVERLAY; i++ )
	{
		m_PropertyOverlay[ i ].m_bOverlay = ( m_Property.m_bUseOverlay[ i ] == 0 ? FALSE : TRUE );
	}



	return S_OK;
}

HRESULT DxEffCharOverlay::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bPossibleEffect )
        return S_OK;

	//if( m_pmcMesh )
	//{
	//	if ( !m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh )
	//	{
	//		m_pmcMesh->GenerateSkinnedMeshNormalMap( pd3dDevice );

	//		LPD3DXMESH pMesh;
	//		m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->CloneMesh( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->GetOptions(), dxeffcharoverlay::VertexDecl, pd3dDevice, &pMesh );
	//		D3DXComputeTangent( pMesh, 0, 0, 0, TRUE, NULL );

	//		SAFE_RELEASE( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh );
	//		m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh = pMesh;
	//	}
	//}	

	return S_OK;
}

HRESULT DxEffCharOverlay::InvalidateDeviceObjects ()
{
	if( !m_bPossibleEffect )
        return S_OK;

	return S_OK;
}

HRESULT DxEffCharOverlay::DeleteDeviceObjects ()
{
	if( !m_bPossibleEffect )
        return S_OK;

	SAFE_RELEASE( m_pDecl );

    m_textureResOverlay.Clear();

	return S_OK;
}


HRESULT DxEffCharOverlay::FrameMove ( float fTime, float fElapsedTime )
{
	if( !m_bPossibleEffect )
        return S_OK;

    HRESULT hr = S_OK;

	if( m_fGlowAniSpeed == 0.0f )
	{
		m_fAniGlowAlpha = m_fGlowAlpha;
	}else{
		m_fAniValue += m_fGlowAniSpeed * fElapsedTime;

		if( m_fAniValue >= 360 ) m_fAniValue = 0.0f;
		m_fAniGlowAlpha = fabs( cosf(m_fAniValue) / m_fGlowAlpha );
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

void DxEffCharOverlay::PrepareOverlayValues( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwSelectMaterial )
{
	if( !m_bPossibleEffect ) return;
	if( !m_pSkinMesh )		return;
	if( !s_pEffect )		return;

	if( !m_PropertyOverlay[ dwSelectMaterial ].m_textureResOverlay.GetTexture() )	
		return;

	if ( !m_PropertyOverlay[ dwSelectMaterial ].m_bOverlay ) 
		return;

	m_Property.Assign( m_PropertyOverlay[ dwSelectMaterial ], dwSelectMaterial );

	HRESULT hr;

	D3DXMATRIX matView, matProj, mat;
	pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

	//mat = m_matWorld * matView * matProj;

	D3DXVECTOR4 v;
	D3DLIGHT9 light;
	pd3dDevice->GetLight( 0, &light );
	mat = m_matWorld * matView * matProj;
	D3DXVECTOR4 light_pos = D3DXVECTOR4( light.Direction.x + m_vAddLight.x, light.Direction.y + m_vAddLight.y, light.Direction.z + m_vAddLight.z, 0);
	D3DXMatrixInverse( &mat, NULL, &m_matWorld);
	D3DXVec4Transform( &v, &light_pos, &mat );
	D3DXVec3Normalize( (D3DXVECTOR3 *)&v, (D3DXVECTOR3 *)&v );

	s_pEffect->SetBool( "m_bUseSpecular", m_PropertyOverlay[ dwSelectMaterial ].m_bUseSpecular ); // by luxes.
	s_pEffect->SetFloat( "m_fGlowBright", m_PropertyOverlay[ dwSelectMaterial ].m_fGlowBright );
	s_pEffect->SetBool( "m_bUseGlowTex", m_PropertyOverlay[ dwSelectMaterial ].m_bUseGlowTex );
	s_pEffect->SetFloat( "m_fReflectivity", m_PropertyOverlay[ dwSelectMaterial ].m_fReflectActivity );
	s_pEffect->SetBool( "m_bUseReflect", m_PropertyOverlay[ dwSelectMaterial ].m_bUseReflectTex );
	s_pEffect->SetBool( "m_bUseAlphaReflect", m_PropertyOverlay[ dwSelectMaterial ].m_bUseAlphaReflectTex );


	// 정점 선언
	pd3dDevice->SetVertexDeclaration( m_pDecl);	


	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )			pMaterialPiece = m_pCharPart->GetSkinPiece()->m_arrayMaterialPiece;
	else if ( m_pSkinPiece )	pMaterialPiece = m_pSkinPiece->m_arrayMaterialPiece;

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;		
	sCharSetting.pmtrlSpecular		= NULL;
	sCharSetting.bWorldIdentity		= TRUE;
	sCharSetting.emRDOP				= CTOP_BASE;


	if( !g_bCHAR_EDIT_RUN )
	{
		if ( m_nSelectOverlay == dwSelectMaterial )
		{
			m_cColor1  = m_pCharPart->GetHairColor();
			m_cColor2  = m_pCharPart->GetSubColor();

			m_PropertyOverlay[ dwSelectMaterial ].m_vColor1 = D3DXVECTOR4(m_cColor1.r,m_cColor1.g,m_cColor1.b,m_cColor1.a);
			m_PropertyOverlay[ dwSelectMaterial ].m_vColor2 = D3DXVECTOR4(m_cColor2.r,m_cColor2.g,m_cColor2.b,m_cColor2.a);
		}
	}

	D3DXVECTOR4 vColor1 = D3DXVECTOR4(m_cColor1.r,m_cColor1.g,m_cColor1.b,m_cColor1.a);
	D3DXVECTOR4 vColor2 = D3DXVECTOR4(m_cColor2.r,m_cColor2.g,m_cColor2.b,m_cColor2.a);


	D3DXVECTOR4 vAmbient = D3DXVECTOR4(m_cAmbient.r,m_cAmbient.g,m_cAmbient.b,m_cAmbient.a);
	D3DXVECTOR4 vDiffuse = D3DXVECTOR4(m_cDiffuse.r,m_cDiffuse.g,m_cDiffuse.b,m_cDiffuse.a);
	D3DXVECTOR4 vSpecular = D3DXVECTOR4(m_cSpecular.r,m_cSpecular.g,m_cSpecular.b,m_cSpecular.a);


	s_pEffect->SetTechnique( m_hTechnique );

	if( m_PropertyOverlay[ dwSelectMaterial ].m_bUseGlowTex )
	{
		s_pEffect->SetTexture( m_htGlowTex, m_PropertyOverlay[ dwSelectMaterial ].m_textureResGlow.GetTexture() );
		s_pEffect->SetFloat( m_htGlowAlpha, m_fAniGlowAlpha );
	}

	if( m_PropertyOverlay[ dwSelectMaterial ].m_bUseReflectTex )
	{
		s_pEffect->SetTexture( m_htReflectTex, m_PropertyOverlay[ dwSelectMaterial ].m_textureResReflect.GetCubeTexture() );
	}

	if( m_PropertyOverlay[ dwSelectMaterial ].m_bUseAlphaReflectTex )
	{
		s_pEffect->SetTexture( m_htAlphaReflectTex, m_PropertyOverlay[ dwSelectMaterial ].m_textureResAlphaReflect.GetTexture()  );
	}

	D3DXVECTOR4 vInputLightPos;

	if( g_bCHAR_EDIT_RUN )
	{
		s_pEffect->SetVector( m_hvAmbient, &m_PropertyOverlay[ dwSelectMaterial ].m_vAmbient );
		s_pEffect->SetVector( m_hvDiffuse, &m_PropertyOverlay[ dwSelectMaterial ].m_vDiffuse );
		s_pEffect->SetVector( m_hvSpecular, &m_PropertyOverlay[ dwSelectMaterial ].m_vSpecular );
	}
	else
	{
		D3DXVECTOR4 vLightAmbient  = D3DXVECTOR4( light.Ambient.r, light.Ambient.g, light.Ambient.b, light.Ambient.a );
		D3DXVECTOR4 vLightDiffuse  = D3DXVECTOR4( light.Diffuse.r, light.Diffuse.g, light.Diffuse.b, light.Diffuse.a );
		D3DXVECTOR4 vLightSpecular = D3DXVECTOR4( light.Specular.r, light.Specular.g, light.Specular.b, light.Specular.a );

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
			}	
		}

		D3DXVECTOR4 vAmbient  = D3DXVECTOR4(m_PropertyOverlay[ dwSelectMaterial ].m_vAmbient.x
			,m_PropertyOverlay[ dwSelectMaterial ].m_vAmbient.y,m_PropertyOverlay[ dwSelectMaterial ].m_vAmbient.z
			,m_PropertyOverlay[ dwSelectMaterial ].m_vAmbient.w);
		D3DXVECTOR4 vDiffuse  = ( D3DXVECTOR4(m_PropertyOverlay[ dwSelectMaterial ].m_vDiffuse.x
			,m_PropertyOverlay[ dwSelectMaterial ].m_vDiffuse.y,m_PropertyOverlay[ dwSelectMaterial ].m_vDiffuse.z
			,m_PropertyOverlay[ dwSelectMaterial ].m_vDiffuse.w) ) * 0.7f + vLightDiffuse * 0.3f;
		D3DXVECTOR4 vSpecular = ( D3DXVECTOR4(m_PropertyOverlay[ dwSelectMaterial ].m_vSpecular.x
			,m_PropertyOverlay[ dwSelectMaterial ].m_vSpecular.y,m_PropertyOverlay[ dwSelectMaterial ].m_vSpecular.z
			,m_PropertyOverlay[ dwSelectMaterial ].m_vSpecular.w) + vLightSpecular ) / 2.0f;

		//D3DXVECTOR4 vAmbient  = ( D3DXVECTOR4(m_cAmbient.r,m_cAmbient.g,m_cAmbient.b,m_cAmbient.a) + vLightAmbient ) / 2.0f;
		//D3DXVECTOR4 vDiffuse  = ( D3DXVECTOR4(m_cDiffuse.r,m_cDiffuse.g,m_cDiffuse.b,m_cDiffuse.a) + vLightDiffuse ) / 2.0f;
		//D3DXVECTOR4 vSpecular = ( D3DXVECTOR4(m_cSpecular.r,m_cSpecular.g,m_cSpecular.b,m_cSpecular.a) + vLightSpecular ) / 2.0f;

        // Ambient 효과가 CharPart에 적용될 경우.. Ambient 값 변경.
        // 효과가 적용 안되어도 디폴트값 m_pCharPart->m_cEffAmbientType == USETEXCOLOR
        //                              m_pCharPart->m_cEffAmbientColor = 1.f,1.f,1.f,1.f 
        //                              이라서 원본을 훼손하지 않는다.
        if ( m_pCharPart )
        {
            if ( m_pCharPart->m_cEffAmbientType == USETEXCOLOR )
            {
                vAmbient.x *= m_pCharPart->m_cEffAmbientColor.r;
                vAmbient.y *= m_pCharPart->m_cEffAmbientColor.g;
                vAmbient.z *= m_pCharPart->m_cEffAmbientColor.b;
            }
            else
            {
                vAmbient.x = m_pCharPart->m_cEffAmbientColor.r;
                vAmbient.y = m_pCharPart->m_cEffAmbientColor.g;
                vAmbient.z = m_pCharPart->m_cEffAmbientColor.b; 
            }
        }

		// 라이트 끈 효과를 주기 위해
		DWORD dwLighting(0);
		pd3dDevice->GetRenderState( D3DRS_LIGHTING, &dwLighting );
		if ( dwLighting == 0 )
		{
			vDiffuse = D3DXVECTOR4( 0.f, 0.f, 0.f, 0.f );
			vAmbient = D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );
			vSpecular = D3DXVECTOR4( 0.f, 0.f, 0.f, 0.f );
		}

		s_pEffect->SetVector( m_hvAmbient, &vAmbient );
		s_pEffect->SetVector( m_hvDiffuse, &vDiffuse);
		s_pEffect->SetVector( m_hvSpecular, &vSpecular );

	}

	V( s_pEffect->SetTexture( m_htColorTex, m_PropertyOverlay[ dwSelectMaterial ].m_textureResOverlay.GetTexture() ) );
	V( s_pEffect->SetVector( m_hvInputColor1, &m_PropertyOverlay[ dwSelectMaterial ].m_vColor1 ) );
	V( s_pEffect->SetVector( m_hvInputColor2, &m_PropertyOverlay[ dwSelectMaterial ].m_vColor2 ) );
	V( s_pEffect->SetFloat( m_hfSpecularPower, m_PropertyOverlay[ dwSelectMaterial ].m_fSpecularPower ) );
	V( s_pEffect->SetFloat( m_hfSpecularLevel, m_PropertyOverlay[ dwSelectMaterial ].m_fSpecularLevel ) );
	V( s_pEffect->SetFloat( m_hfAlphaRef, m_PropertyOverlay[ dwSelectMaterial ].m_fAlphaRef) );
}

void DxEffCharOverlay::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	if( !m_bPossibleEffect ) return;
	if( !m_pSkinMesh )		return;
	if( !s_pEffect )		return;

	// m_pmcMesh 관리.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	//bool bOverlayTex = false;
	//for ( int i = 0; i < static_cast<int>(m_pmcMesh->GetNumMaterials()) && i < MAX_MATERIAL_OVERLAY; i++ )
	//{
	//	if ( m_PropertyOverlay[ i ].m_textureResOverlay.GetTexture() )
	//		bOverlayTex = true;
	//}
	//if ( !bOverlayTex )	return;

	// 렌더하지 않는게 아니라 흰색으로 그릴 수 있도록 한다.
	//for ( int i = 0; i < static_cast<int>(m_pmcMesh->GetNumMaterials()) && i < MAX_MATERIAL_OVERLAY; i++ )
	//{
	//	if( strlen( m_PropertyOverlay[ i ].m_szGlowTex ) != 0 && m_PropertyOverlay[ i ].m_bUseGlowTex )
	//	{
	//		if ( !m_PropertyOverlay[ i ].m_textureResGlow.Update() )
	//			return;
	//	}
	//	if( strlen( m_PropertyOverlay[ i ].m_szReflectTex ) != 0 && m_PropertyOverlay[ i ].m_bUseReflectTex )
	//	{
	//		if ( !m_PropertyOverlay[ i ].m_textureResReflect.Update() )
	//			return;
	//	}

	//	if( strlen( m_PropertyOverlay[ i ].m_szOverlayTex ) != 0 )
	//	{
	//		if ( !m_PropertyOverlay[ i ].m_textureResOverlay.Update() )
	//			return;
	//	}

	//	if( strlen( m_PropertyOverlay[ i ].m_szReflectAlphaTex ) != 0 && m_PropertyOverlay[ i ].m_bUseAlphaReflectTex )
	//	{
	//		if ( !m_PropertyOverlay[ i ].m_textureResAlphaReflect.Update() )
	//			return;
	//	}
	//}

	HRESULT hr;

	D3DXMATRIX matView, matProj, mat;
	pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

	mat = m_matWorld * matView * matProj;
	s_pEffect->SetMatrix( m_hmWVP, &mat );

	D3DXVECTOR4 v;
	D3DLIGHT9 light;
	pd3dDevice->GetLight( 0, &light );
	mat = m_matWorld * matView * matProj;
	D3DXVECTOR4 light_pos = D3DXVECTOR4( light.Direction.x + m_vAddLight.x, light.Direction.y + m_vAddLight.y, light.Direction.z + m_vAddLight.z, 0);
	D3DXMatrixInverse( &mat, NULL, &m_matWorld);
	D3DXVec4Transform( &v, &light_pos, &mat );
	D3DXVec3Normalize( (D3DXVECTOR3 *)&v, (D3DXVECTOR3 *)&v );

	m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseSpecular = ( m_bUseSpecular == 0 ? FALSE : TRUE );
	m_PropertyOverlay[ m_dwSelectMaterial ].m_fGlowBright = m_fGlowBright;
	m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseGlowTex = ( m_bUseGlowTex == 0 ? FALSE : TRUE );
	m_PropertyOverlay[ m_dwSelectMaterial ].m_fReflectActivity = m_fReflectActivity;
	m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseReflectTex = ( m_bUseReflect == 0 ? FALSE : TRUE );
	m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseAlphaReflectTex = ( m_bUseAlphaReflect == 0 ? FALSE : TRUE );


	// 정점 선언
	pd3dDevice->SetVertexDeclaration( m_pDecl);	

	
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

	
	if( g_bCHAR_EDIT_RUN )
	{
		D3DXVECTOR4 vInputLightPos = D3DXVECTOR4( light_pos.x, light_pos.y, light_pos.z, 0.0f );
		V( s_pEffect->SetVector( m_hvLightDir, &vInputLightPos ) );
	}
	else
	{
		D3DXVECTOR4 vInputLightPos = D3DXVECTOR4( -light_pos.x, -light_pos.y, -light_pos.z, 0.0f );
		//D3DXVECTOR4 vInputLightPos = D3DXVECTOR4( -light_pos.x, light_pos.y, -light_pos.z, 0.0f );
		V( s_pEffect->SetVector( m_hvLightDir, &vInputLightPos ) );
	}

	// 시전(로컬 좌표계)
	mat = m_matWorld * matView;
	D3DXMatrixInverse( &mat, NULL, &mat );
	v = D3DXVECTOR4( matView._41, matView._42, matView._43, 1.0f );
	//v = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
	//D3DXVec4Transform( &v, &v, &mat );
	s_pEffect->SetVector( m_hvEyePos, &v );

	// 좌표계 계산을 위한 행렬. by luxes.
	D3DXMATRIX hvMatWorldInv, hvWorldInvTrans;
	D3DXMatrixInverse( &hvMatWorldInv, 0, &m_matWorld );
	s_pEffect->SetMatrix( m_hvWorldInv, &hvMatWorldInv );

	D3DXMatrixInverse( &hvWorldInvTrans, 0, &m_matWorld );
	D3DXMatrixTranspose( &hvWorldInvTrans, &hvWorldInvTrans );
	s_pEffect->SetMatrix( m_hvWorldInvTrans, &hvWorldInvTrans );

	D3DXMATRIX hvMatViewInv;
	D3DXMatrixInverse( &hvMatViewInv, 0, &matView );
	s_pEffect->SetMatrix( m_hvViewInv, &hvMatViewInv );

	D3DXVECTOR4 vColor1 = D3DXVECTOR4(m_cColor1.r,m_cColor1.g,m_cColor1.b,m_cColor1.a);
	D3DXVECTOR4 vColor2 = D3DXVECTOR4(m_cColor2.r,m_cColor2.g,m_cColor2.b,m_cColor2.a);


	D3DXVECTOR4 vAmbient = D3DXVECTOR4(m_cAmbient.r,m_cAmbient.g,m_cAmbient.b,m_cAmbient.a);
	D3DXVECTOR4 vDiffuse = D3DXVECTOR4(m_cDiffuse.r,m_cDiffuse.g,m_cDiffuse.b,m_cDiffuse.a);
	D3DXVECTOR4 vSpecular = D3DXVECTOR4(m_cSpecular.r,m_cSpecular.g,m_cSpecular.b,m_cSpecular.a);


	s_pEffect->SetTechnique( m_hTechnique );

	if( m_bUseGlowTex )
	{
		//s_pEffect->SetTexture( m_htGlowTex, m_PropertyOverlay[ m_dwSelectMaterial ].m_textureResGlow.GetTexture() );
		//s_pEffect->SetFloat( m_htGlowAlpha, m_fAniGlowAlpha );
	}

	if( m_bUseReflect )
	{
		//s_pEffect->SetTexture( m_htReflectTex, m_textureResReflect.GetCubeTexture() );
	}

	if( m_bUseAlphaReflect )
	{
		//s_pEffect->SetTexture( m_htAlphaReflectTex, m_textureResAlphaReflect.GetTexture() );
	}

	D3DXVECTOR4 vInputLightPos;

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

		m_PropertyOverlay[ m_dwSelectMaterial ].m_vAmbient = vAmbient; // overlay value setting. by luxes.
		m_PropertyOverlay[ m_dwSelectMaterial ].m_vDiffuse = vDiffuse; // overlay value setting. by luxes.
		m_PropertyOverlay[ m_dwSelectMaterial ].m_vSpecular = vSpecular; // overlay value setting. by luxes.

		s_pEffect->SetVector( m_hvLightDir, &vInputLightPos );

	}
	else
	{
		vInputLightPos = D3DXVECTOR4( -light_pos.x, -light_pos.y, -light_pos.z, 0.0f );
		//vInputLightPos = D3DXVECTOR4( -light_pos.x, light_pos.y, -light_pos.z, 0.0f );

		s_pEffect->SetBool( m_hbUseDirectionLight, TRUE );

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

				s_pEffect->SetBool( m_hbUseDirectionLight, FALSE );
			}	
		}

		s_pEffect->SetVector( m_hvLightDir, &vInputLightPos );
	}

	m_PropertyOverlay[ m_dwSelectMaterial ].m_vColor1 = vColor1; // overlay value setting. by luxes.
	m_PropertyOverlay[ m_dwSelectMaterial ].m_vColor2 = vColor2; // overlay value setting. by luxes.
	m_PropertyOverlay[ m_dwSelectMaterial ].m_fSpecularPower = m_fSpecularPower; // overlay value setting. by luxes.
	m_PropertyOverlay[ m_dwSelectMaterial ].m_fSpecularLevel = m_fSpecularLevel; // overlay value setting. by luxes.
	m_PropertyOverlay[ m_dwSelectMaterial].m_fAlphaRef = m_fAlphaRef; // overlay value setting. by luxes.

	// 로컬 - 투영 변환 행렬
	s_pEffect->SetMatrix( m_htWorld, &m_matWorld );
	s_pEffect->SetMatrix( m_htProj, &matProj );
	s_pEffect->SetMatrix( m_htView, &matView );

	// 오버레이 적용 받지 않는 메터리얼의 이펙트 세팅
	ID3DXEffect* pSkinnedEffect = DxSkinMesh9_HLSL::GetEffect();	// D3DX effect interface

	if ( pSkinnedEffect )
	{
		D3DXVECTOR4 vVector;
		D3DCOLORVALUE cDiffuse = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse;
		D3DCOLORVALUE cAmbient = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient;

		vVector.x = cDiffuse.r;
		vVector.y = cDiffuse.g;
		vVector.z = cDiffuse.b;
		vVector.w = cDiffuse.a;
		V( pSkinnedEffect->SetVector( "MaterialDiffuse", &vVector ) );

		vVector.x = cAmbient.r;
		vVector.y = cAmbient.g;
		vVector.z = cAmbient.b;
		vVector.w = cAmbient.a;
		V( pSkinnedEffect->SetVector( "MaterialAmbient", &vVector ) );

		D3DXMATRIX matViewProj, matView, matProj;
		pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
		pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
		D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
		pSkinnedEffect->SetMatrix( "mViewProj", &matViewProj );

		// Set Light for vertex shader
		D3DXVECTOR4 vLightDir( 0.0f, 1.0f, -1.0f, 0.0f );
		D3DXVec4Normalize( &vLightDir, &vLightDir );
		V( pSkinnedEffect->SetVector( "lhtDir", &vLightDir) );
	}

	for ( int i = 0; i < static_cast<int>(m_pmcMesh->GetNumMaterials()) && i < MAX_MATERIAL_OVERLAY; i++ )
	{
		PrepareOverlayValues( pd3dDevice, i );
		m_pSkinMesh->DrawMeshOVERLAY( pd3dDevice, pBoneMatrices, sCharSetting, m_PropertyOverlay, s_pEffect, i );
	}
}


HRESULT	DxEffCharOverlay::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

        for( DWORD i=0; i<MAX_MATERIAL_OVERLAY; ++i )
            m_PropertyOverlay[ i ].SaveFile( SFile );
	}
	SFile.EndBlock();

	return S_OK;
}

HRESULT DxEffCharOverlay::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	EFFCHAR_PROPERTY_OVERLAY_100 Property;
	SFile.ReadBuffer ( &Property, sizeof(Property) );
	m_Property.Assign(Property);

	for ( int i = 0; i < MAX_MATERIAL_OVERLAY; i++ )
		OldDataSync( m_PropertyOverlay[ i ], m_Property );

	m_bNewMaterialMode = FALSE;

	return S_OK;
}

HRESULT DxEffCharOverlay::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	EFFCHAR_PROPERTY_OVERLAY_101 Property;
	SFile.ReadBuffer ( &Property, sizeof(Property) );
	m_Property.Assign(Property);

	for ( int i = 0; i < MAX_MATERIAL_OVERLAY; i++ )
		OldDataSync( m_PropertyOverlay[ i ], m_Property );

	m_bNewMaterialMode = FALSE;

	return S_OK;
}

HRESULT DxEffCharOverlay::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	EFFCHAR_PROPERTY_OVERLAY_102 Property;
	SFile.ReadBuffer ( &Property, sizeof(Property) );

    for( DWORD i=0; i<MAX_MATERIAL_OVERLAY; ++i )
        m_PropertyOverlay[ i ].LoadFile( SFile );

	m_bNewMaterialMode = FALSE;

	m_Property.Assign(Property);

	return S_OK;
}

HRESULT DxEffCharOverlay::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

    for( DWORD i=0; i<MAX_MATERIAL_OVERLAY; ++i )
        m_PropertyOverlay[ i ].LoadFile( SFile );

	m_bNewMaterialMode = FALSE;

	return S_OK;
}

HRESULT DxEffCharOverlay::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	for( DWORD i=0; i<MAX_MATERIAL_OVERLAY; ++i )
		m_PropertyOverlay[ i ].LoadFile( SFile );

	return S_OK;
}


HRESULT	DxEffCharOverlay::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
	case 0x0101:	LoadFile_0101( SFile, pd3dDevice ); break;
	case 0x0102:	LoadFile_0102( SFile, pd3dDevice ); break;
	case 0x0103:	LoadFile_0103( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0104( SFile, pd3dDevice );	break;
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
	if ( FAILED(hr) )
        return hr;

	return S_OK;
}

void DxEffCharOverlay::OldDataSync( EFFCHAR_PROPERTY_OVERLAY_TEXTURE& val1, EFFCHAR_PROPERTY_OVERLAY& val2 )
{
	val1.m_bOverlay = true; // 현재 선택된 material에서 overlay 사용하기. by luxes.

	StringCchCopy( val1.m_szOverlayTex, MAX_PATH, val2.m_szOverlayTex );

	val1.m_bUseSpecular = ( val2.m_bUseSpecular == 0 ? FALSE : TRUE ); // by luxes.

	val1.m_vColor1 = D3DXVECTOR4( val2.m_cColor1.r, val2.m_cColor1.g
		, val2.m_cColor1.b, val2.m_cColor1.a );

	val1.m_vColor2 = D3DXVECTOR4( val2.m_cColor2.r, val2.m_cColor2.g
		, val2.m_cColor2.b, val2.m_cColor2.a );

	val1.m_vAmbient = D3DXVECTOR4( val2.m_cAmbient.r, val2.m_cAmbient.g
		, val2.m_cAmbient.b, val2.m_cAmbient.a );
	val1.m_vDiffuse = D3DXVECTOR4( val2.m_cDiffuse.r, val2.m_cDiffuse.g
		, val2.m_cDiffuse.b, val2.m_cDiffuse.a );
	val1.m_vSpecular = D3DXVECTOR4( val2.m_cSpecular.r, val2.m_cSpecular.g
		, val2.m_cSpecular.b, val2.m_cSpecular.a );

	val1.m_fSpecularPower = val2.m_fSpecularPower;
	val1.m_fSpecularLevel = val2.m_fSpecularLevel;

	val1.m_fAlphaRef = val2.m_fAlphaRef; // by luxes.

	StringCchCopy( val1.m_szGlowTex, MAX_PATH, val2.m_szGlowTex );
	val1.m_bUseGlowTex = ( val2.m_bUseGlowTex == 0 ? FALSE : TRUE );
	val1.m_fGlowBright = val2.m_fGlowBright;
	val1.m_fGlowAlpha = val2.m_fGlowAlpha;

	StringCchCopy( val1.m_szReflectTex, MAX_PATH, val2.m_szReflectTex );
	StringCchCopy( val1.m_szReflectAlphaTex, MAX_PATH, val2.m_szReflectAlphaTex );
	val1.m_bUseReflectTex = ( val2.m_bUseReflect == 0 ? FALSE : TRUE );
	val1.m_fReflectActivity = val2.m_fReflectActivity;
	val1.m_bUseAlphaReflectTex = ( val2.m_bUseAlphaReflect == 0 ? FALSE : TRUE );
}

void DxEffCharOverlay::SetOverlayProperty( EFFCHAR_PROPERTY_OVERLAY_TEXTURE* PropertyOverlay, int nIndex )
{
	m_PropertyOverlay[ nIndex ] = *PropertyOverlay;
}

void DxEffCharOverlay::SetOverlayColor( D3DXCOLOR hairColor, D3DXCOLOR subColor )
{
	if ( m_pCharPart )
	{
		m_pCharPart->SetHairColor( hairColor );
		m_pCharPart->SetHairColor( subColor );
	}
}

EFFCHAR_PROPERTY_OVERLAY_TEXTURE DxEffCharOverlay::GetPropertyOverlay( int nIndex )
{
    if (nIndex < 0 || nIndex >= MAX_MATERIAL_OVERLAY)
    {
        std::string ErrorMsg(
            sc::string::format(
                "EFFCHAR_PROPERTY_OVERLAY_TEXTURE DxEffCharOverlay::GetPropertyOverlay - %1% texture index error.",
                m_szOverlayTex));
        CDebugSet::ToLogFile(ErrorMsg);
        return m_PropertyOverlay[0];
    }
    else
    {
        return m_PropertyOverlay[nIndex];
    }
}

void DxEffCharOverlay::LoadOverlayMaskTexture( int nMaterial, BOOL bTexThread )
{
	if( strlen( m_PropertyOverlay[ nMaterial ].m_szOverlayTex ) != 0 )
	{
		m_PropertyOverlay[ nMaterial ].m_textureResOverlay = TextureManager::GetInstance().LoadTexture(
			m_PropertyOverlay[ nMaterial ].m_szOverlayTex,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			EMMM_CHAR_MIPMAP,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_CHAR,
			bTexThread ? true : false,
			FALSE );
	}
	else
	{
		m_PropertyOverlay[ nMaterial ].m_textureResOverlay.Clear();
	}
}

LPDIRECT3DTEXTUREQ DxEffCharOverlay::GetOverlayMaskTexture( int nMaterial )
{
	return m_PropertyOverlay[ nMaterial ].m_textureResOverlay.GetTexture();
}

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.

void DxEffCharOverlay::GetOverlayColor_1_2( int nMaterial, const D3DXVECTOR4** ppColor1, const D3DXVECTOR4** ppColor2 )
{
	D3DXVECTOR4 vColor1 = D3DXVECTOR4(m_cColor1.r,m_cColor1.g,m_cColor1.b,m_cColor1.a);
	D3DXVECTOR4 vColor2 = D3DXVECTOR4(m_cColor2.r,m_cColor2.g,m_cColor2.b,m_cColor2.a);
	m_PropertyOverlay[ m_dwSelectMaterial ].m_vColor1 = vColor1; // overlay value setting. by luxes.
	m_PropertyOverlay[ m_dwSelectMaterial ].m_vColor2 = vColor2; // overlay value setting. by luxes.

	(*ppColor1) = &m_PropertyOverlay[ nMaterial ].m_vColor1;
	(*ppColor2) = &m_PropertyOverlay[ nMaterial ].m_vColor2;
}

// 프로그램상 MaskMap 을 만들기 위한 작업.
void DxEffCharOverlay::ExportEffTexture( LPDIRECT3DDEVICEQ pd3dDevice, 
									  DWORD dwMaterialID, 
									  LPDIRECT3DTEXTUREQ* ppLevelTex, 
									  LPDIRECT3DTEXTUREQ* ppCubeTex,
									  bool bTexThread,
									  const TCHAR* pTextureName_NoHave )
{
	if ( m_dwSelectMaterial != dwMaterialID )
		return;

	if ( m_PropertyOverlay[ dwMaterialID ].m_bUseAlphaReflectTex )
	{
		if ( !m_PropertyOverlay[ dwMaterialID ].m_textureResAlphaReflect.GetTexture() )
		{
			m_PropertyOverlay[ dwMaterialID ].m_textureResAlphaReflect = TextureManager::GetInstance().LoadTexture(
				m_PropertyOverlay[ dwMaterialID ].m_szReflectAlphaTex,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				EMMM_CHAR_MIPMAP,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_CHAR,
				bTexThread,
				FALSE );
		}

		(*ppCubeTex) = m_PropertyOverlay[ dwMaterialID ].m_textureResAlphaReflect.GetTexture();

		// 값이 없을 때만 넣어준다.
		if ( !(*ppCubeTex) )
		{
			pTextureName_NoHave = m_PropertyOverlay[ dwMaterialID ].m_szReflectAlphaTex;
		}
	}
}

// Effect Texture 들이 다 로딩이 되었는가~?
BOOL DxEffCharOverlay::IsEnableEffTexture( BOOL bTexThread )
{
	// [shhan][2015.03.25] rm #1089 - [코스튬] 신규 코스튬 "그림리퍼" 모자 색상 오류 문제 확인 요청
	//						m_textureResOverlay 검사가 빠르게 이루어져야 한다. 
	//						안그러면 Overlay 색이 표현되지 않는다.
	//
	// Overlay 텍스쳐도 다 로딩되어 있어야 된다.
	if ( m_PropertyOverlay[ m_dwSelectMaterial ].m_bOverlay )
	{
		if ( !m_PropertyOverlay[ m_dwSelectMaterial ].m_textureResOverlay.GetTexture() )
		{
			// [shhan][2014.10.01] 텍스쳐 자체가 없기 때문에 어떻게 할수가 없다.
			//						대기 타지 말고 그냥 렌더링 하도록 한다.
			if ( !TextureManager::GetInstance().IsPathFile( m_PropertyOverlay[ m_dwSelectMaterial ].m_szOverlayTex ) )
				return TRUE;

			// [shhan][2014.10.17] 로딩이 다 되지 않았는데 m_PropertyOverlay[ m_dwSelectMaterial ].m_textureResOverlay 를 복사하면서 문제가 생겼다.
			//						되도록 TextureResource 복사는 하면 안된다.
			//
			LoadOverlayMaskTexture( m_dwSelectMaterial, bTexThread );

			return FALSE;
		}
	}

	size_t nSize(0);
	StringCchLength( m_szReflectAlphaTex, MAX_PATH, &nSize );
	if ( nSize == 0 )
		return TRUE;

	StringCchCopy( m_PropertyOverlay[ m_dwSelectMaterial ].m_szReflectAlphaTex, MAX_PATH, m_szReflectAlphaTex ); // overlay value setting. by luxes.
	m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseAlphaReflectTex = ( m_Property.m_bUseAlphaReflect == 0 ? FALSE : TRUE );

	if ( m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseAlphaReflectTex )
	{
		if ( !m_PropertyOverlay[ m_dwSelectMaterial ].m_textureResAlphaReflect.GetTexture() )
		{
			// [shhan][2014.10.01] 텍스쳐 자체가 없기 때문에 어떻게 할수가 없다.
			//						대기 타지 말고 그냥 렌더링 하도록 한다.
			if ( !TextureManager::GetInstance().IsPathFile( m_PropertyOverlay[ m_dwSelectMaterial ].m_szReflectAlphaTex ) )
				return TRUE;

			// [shhan][2014.10.08] 로딩이 다 되지 않았는데 m_PropertyOverlay[ m_dwSelectMaterial ].m_textureResAlphaReflect 를 복사하면서 문제가 생겼다.
			//						되도록 TextureResource 복사는 하면 안된다.
			//
			m_PropertyOverlay[ m_dwSelectMaterial ].m_textureResAlphaReflect = TextureManager::GetInstance().LoadTexture(
					m_PropertyOverlay[ m_dwSelectMaterial ].m_szReflectAlphaTex,
					false,
					TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
					EMMM_CHAR_MIPMAP,
					D3DFMT_UNKNOWN,
					TEXTURE_RESIZE_NONE,
					TEXTURE_CHAR,
					bTexThread,
					FALSE );

			return FALSE;
		}
	}

	return TRUE;
}

// PrecalculateForMaterialSystem의 2번 중 값만 미리 백업해 놓는거라면, 이곳에서 렌더가 된다.
void DxEffCharOverlay::PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
													   EffectMaterialData* pEffMaterialData, 
													   const D3DXMATRIX* pMatrix, 
													   const float fScale )
{
	//////////////////////////////////////////////////////////////////////////
	//		w_testserver_body_1.cps 파일에서 문제가 생긴것
	//	이전 툴상에서 Overlay 가 있으면 Effect 순서상 먼저 뿌린거는 지워지는 현상이 있음.
	//				똑같이 재현 필요함.
	// 새로이 작업된건 이런식으로 하면 안됨. 그래서 m_bNewMaterialMode 으로 분류.
	if ( !m_bNewMaterialMode )
	{
		pEffMaterialData->m_bMultiTex = FALSE;
	}

	// 성능을 최저사양으로 함.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_PS_1_1 )
		return;

	// m_pmcMesh 관리.
	if ( !IsExist_Copy_pmcMesh() )
		return;

	if ( !m_PropertyOverlay[ m_dwSelectMaterial ].m_bUseReflectTex )
		return;
	

	if ( pEffMaterialData->m_vecEnableCubeBlend.size() != m_pmcMesh->GetNumMaterials() )
	{
		pEffMaterialData->m_vecEnableCubeBlend.clear();

		for ( DWORD i=0; i<m_pmcMesh->GetNumMaterials(); ++i )
		{
			if ( m_PropertyOverlay[ i ].m_bUseReflectTex )
			{
				pEffMaterialData->m_bCubeBlend = TRUE;

				if ( m_PropertyOverlay[ i ].m_textureResReflect.GetTexture() )
				{
					pEffMaterialData->m_pCubeMapTexture = m_PropertyOverlay[ i ].m_textureResReflect.GetTexture();
				}

				pEffMaterialData->m_vecEnableCubeBlend.push_back( TRUE );
			}
			else
			{
				pEffMaterialData->m_vecEnableCubeBlend.push_back( FALSE );
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//			이전에 글로우 셋팅한 것이 있는데 그건 TexUpDown.
	//						보여주도록 한다.
	float fTexUpDownPower(1.f);
	if ( pEffMaterialData->m_vecTexUpDownTexture.size() != m_pmcMesh->GetNumMaterials() )
	{
		pEffMaterialData->m_vecTexDiffColor.clear();
		pEffMaterialData->m_vecTexUpDownTexture.clear();

		for ( DWORD i=0; i<m_pmcMesh->GetNumMaterials(); ++i )
		{
			if ( m_PropertyOverlay[ i ].m_bUseReflectTex )
			{
				pEffMaterialData->m_bTexDiffColor = TRUE;

				pEffMaterialData->m_vecTexDiffColor.push_back( m_fAniGlowAlpha*fTexUpDownPower );
				pEffMaterialData->m_vecTexUpDownTexture.push_back( m_PropertyOverlay[ i ].m_textureResGlow.GetTexture() );
			}
			else
			{
				pEffMaterialData->m_vecTexDiffColor.push_back( 0.f );
				pEffMaterialData->m_vecTexUpDownTexture.push_back( NULL );
			}
		}
	}
	else
	{
		// 다중으로 DxEffCharTexDiff 가 셋팅되어 있을 수도 있어서 이 작업이 추가됨.
		for ( DWORD i=0; i<m_pmcMesh->GetNumMaterials(); ++i )
		{
			if ( m_PropertyOverlay[ i ].m_bUseReflectTex )
			{
				pEffMaterialData->m_bTexDiffColor = TRUE;

				pEffMaterialData->m_vecTexDiffColor[i] = m_fAniGlowAlpha*fTexUpDownPower;
				pEffMaterialData->m_vecTexUpDownTexture[i] = m_PropertyOverlay[ i ].m_textureResGlow.GetTexture();
			}
			else
			{
				pEffMaterialData->m_vecTexUpDownTexture[i] = NULL;
			}

			// 이전에 셋팅된 값을 위해 m_PropertyOverlay[ i ].m_bUseReflectTex 이 FALSE 라도 아무동작도 하지 않는다.
		}
	}
}

BOOL DxEffCharOverlay::IsExist_Copy_pmcMesh()
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

	return TRUE;
}