#include "pch.h"
#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

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

#include "./DxEffCharNormal.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharNormal::TYPEID			= EMEFFCHAR_NORMALMAP;
const DWORD	DxEffCharNormal::VERSION			= 0x0102;
char		DxEffCharNormal::NAME[MAX_PATH]	= "3.Normal Map";

extern BOOL		g_bCHAR_EDIT_RUN;

static const char* pNormalHLSL = 
{
	// 좌표변환 행렬
	"float4x4 mWVP;		\r\n"

	// 광원방향
	"float4 vLightDir;	\r\n"
	// 카메라위치(로컬좌표계)
	"float3 vEyePos;		\r\n"

	// 스펙큘라, 글로시니스, 글로우 텍스쳐
	"bool   bUseSpecularTex;\r\n"
	"bool   bUseGlossinessTex;\r\n"
	"bool   bUseGlowTex;\r\n"
	"bool   bUseDirectionLight;\r\n"

	// 좌표변환 월드 행렬
	"float4x4 mWorld;	\r\n"
	// 좌표변환 뷰 행렬
	"float4x4 mView;	\r\n"
	// 좌표변환 프로젝션 행렬
	"float4x4 mProj;		\r\n"

	"float4 fvAmbient;\r\n"
	"float4 fvSpecular;\r\n"
	"float4 fvDiffuse;\r\n"

	"float fSpecularPower;\r\n"
	"float fGlowBright;\r\n"
	"float fAniGlowAlpha;\r\n"

	"static const int MAX_MATRICES = 60;\r\n"
	"float4x3    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"

	// -------------------------------------------------------------
	// 텍스처
	// -------------------------------------------------------------
	// 디컬 텍스처
	"texture BaseTex;\r\n"
	"sampler BaseSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture = <BaseTex>;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"	MipFilter = LINEAR;\r\n"

	/*"	AddressU = Clamp;\r\n"
	"	AddressV = Clamp;\r\n"*/
	"};\r\n"

	// -------------------------------------------------------------
	// 스펙큘라 텍스처
	"texture SpecularTex;\r\n"
	"sampler SpecularSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture = <SpecularTex>;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"	MipFilter = LINEAR;\r\n"

	/*"	AddressU = Clamp;\r\n"
	"	AddressV = Clamp;\r\n"*/
	"};\r\n"

	// -------------------------------------------------------------
	// 스펙큘라 파워(글로시니스) 텍스처
	"texture GlossinessTex;\r\n"
	"sampler GlossinessSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture = <GlossinessTex>;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"	MipFilter = LINEAR;\r\n"

	/*"	AddressU = Clamp;\r\n"
	"	AddressV = Clamp;\r\n"*/
	"};\r\n"

	// -------------------------------------------------------------
	// 글로우 텍스처
	"texture GlowTex;\r\n"
	"sampler GlowSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture = <GlowTex>;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"	MipFilter = LINEAR;\r\n"

	/*"	AddressU = Clamp;\r\n"
	"	AddressV = Clamp;\r\n"*/
	"};\r\n"

	// -------------------------------------------------------------
	// 법선맵
	"texture NormalMap;\r\n"
	"sampler NormalSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture = <NormalMap>;\r\n"
	"	MinFilter = LINEAR;\r\n"
	"	MagFilter = LINEAR;\r\n"
	"	MipFilter = LINEAR;\r\n"

	"	AddressU = Wrap;\r\n"
	"	AddressV = Wrap;\r\n"
	"};\r\n"

	// -------------------------------------------------------------
	// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
	// -------------------------------------------------------------
	"struct VS_OUTPUT\r\n"
	"{\r\n"
	"	float4 Position		  : POSITION;\r\n"
	"	float2 Texcoord		  : TEXCOORD0;	// 디컬텍스처 좌표\r\n"
	"	float3 LightDirection : TEXCOORD1;	// 광원벡터\r\n"
	"	float3 ViewDirection  : TEXCOORD2;	// 법선벡터\r\n"
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

	//---------------------------------------------------------------
	// 장면 랜더링
	//---------------------------------------------------------------
	"VS_OUTPUT VS( VS_INPUT Input, uniform int NumBones)"
	"	{\r\n"
	"		VS_OUTPUT Output = (VS_OUTPUT)0; // 출력 데이터\r\n"

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
	"		Output.Texcoord = Input.Texcoord;\r\n"	

	"		float3 fvObjectPosition = mul( Input.Position, mWorld );\r\n"

	"		float3 fvViewDirection = vEyePos - fvObjectPosition;	// 시선벡터\r\n"
	"		float3 fvLightDirection = vLightDir;		// 광원벡터\r\n"
	"		if( !bUseDirectionLight )\r\n"
	"			fvLightDirection = vLightDir - fvObjectPosition;		// 광원벡터\r\n"

	// 좌표계 변환 기저	
	/*"		float3 T = cross(Normal,Binormal);\r\n"
	"		float3 fvTangent  = mul( T, mWorld );\r\n"
	"		float3 fvBinormal = mul( Binormal, mWorld );\r\n"
	"		float3 fvNormal	  = mul( Normal, mWorld );\r\n"*/

	"		float3 TotalNormal	 = normalize( Input.Normal );\r\n"
	"		float3 TotalBinormal = normalize( Input.Binormal );\r\n"
	"		float3 TotalTangent = cross(TotalNormal,TotalBinormal);\r\n"

	"		if( Input.Texcoord.x >= 1.0f )\r\n"
	"		{\r\n"
	"			TotalTangent = -TotalTangent;\r\n"
	//	"			fvBinormal = -fvBinormal;\r\n"
	//	"			fvNormal = -fvNormal;\r\n"
	"		}\r\n"

	"		float3 fvTangent  = mul( TotalTangent, mWorld );\r\n"
	"		float3 fvBinormal = mul( TotalBinormal, mWorld );\r\n"
	"		float3 fvNormal	  = mul( TotalNormal, mWorld );\r\n"

	"		Output.ViewDirection.y  = dot( fvTangent, fvViewDirection );\r\n"
	"		Output.ViewDirection.x  = dot( fvBinormal, fvViewDirection );\r\n"
	"		Output.ViewDirection.z  = dot( fvNormal, fvViewDirection );\r\n"

	"		Output.LightDirection.y  = dot( fvTangent, fvLightDirection );\r\n"
	"		Output.LightDirection.x  = dot( fvBinormal, fvLightDirection );\r\n"
	"		Output.LightDirection.z  = dot( fvNormal, fvLightDirection );\r\n"

	"		return Output;	\r\n"
	"	}\r\n"

	//-------------------------------------------------------------------------------------
	"	float4 PS(VS_OUTPUT Input) : COLOR0\r\n"
	"	{\r\n"


	"		float3 fvLightDirection = normalize( Input.LightDirection );\r\n"
	"		float3 fvNormal         = normalize( ( tex2D( NormalSamp, Input.Texcoord ).xyz * 2.0f ) - 1.0f );\r\n"
	"		float  fNDotL           = dot( fvNormal, fvLightDirection ); \r\n"

	"		float3 fvReflection     = normalize( ( ( 2.0f * fvNormal ) * ( fNDotL ) ) - fvLightDirection ); \r\n"
	"		float3 fvViewDirection  = normalize( Input.ViewDirection );\r\n"
	"		float  fRDotV           = max( 0.0f, dot( fvReflection, fvViewDirection ) );\r\n"

	"		float4 fvBaseColor      = tex2D( BaseSamp, Input.Texcoord );\r\n"

	"		float4 fvGlowColor =  float4(0.0,0.0,0.0,0.0);\r\n"
	"		if( bUseGlowTex )\r\n"
	"		{\r\n"
	"			fvGlowColor		= tex2D( GlowSamp, Input.Texcoord).rgba;\r\n"
	"			fvGlowColor     -= (1.0f - fAniGlowAlpha );\r\n"
	"			fvGlowColor		= max(0.0f, fvGlowColor);\r\n"
	"			fvGlowColor		*= fGlowBright;\r\n"
	"		}\r\n"

	"		float4 fvTotalAmbient   = fvAmbient * fvBaseColor; \r\n"
	"		float4 fvTotalDiffuse   = fvDiffuse * fNDotL * fvBaseColor; \r\n"

	"		float4 fSpecularLevel   = float4(1.0,1.0,1.0,1.0);\r\n"

	"		if( bUseSpecularTex )\r\n"
	"			fSpecularLevel = tex2D(SpecularSamp, Input.Texcoord);\r\n"

	"		float fSpecPowerTexLevel = 1.0f; \r\n"
	"		if( bUseGlossinessTex ) \r\n"
	"			fSpecPowerTexLevel = fSpecularPower * ( 1.0f - tex2D(GlossinessSamp, Input.Texcoord).rgb );\r\n"

	"		float4 fvTotalSpecular  = fvSpecular * pow( fRDotV, fSpecPowerTexLevel ) * fSpecularLevel;	\r\n"
	"		float4 result = saturate( fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular + fvGlowColor);	\r\n"

	"		return( result );\r\n"

	"	}\r\n"

	"	int CurNumBones = 2;\r\n"
	"	VertexShader vsArray[4] = { compile vs_2_0 VS(1), \r\n"
	"								compile vs_2_0 VS(2),\r\n"
	"								compile vs_2_0 VS(3),\r\n"
	"								compile vs_2_0 VS(4)\r\n"
	"	};\r\n"

	// -------------------------------------------------------------
	// 테크닉
	// -------------------------------------------------------------
	"	technique TShader\r\n"
	"	{\r\n"
	"		pass P0\r\n"
	"		{\r\n"
	"			VertexShader = (vsArray[CurNumBones]);\r\n"
	"			PixelShader  = compile ps_2_0 PS();\r\n"
	"		}\r\n"
	"	}\r\n"

};

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

DxEffCharNormal::DxEffCharNormal(void) :
	DxEffChar(),
	m_pNormalDCRT(NULL),
	m_pEffect(NULL),
	m_hTechnique(NULL),
	m_hvLightDir(NULL),
	m_hvEyePos(NULL),
	m_htNormalMap(NULL),
	m_htSpecularTex(NULL),
	m_htGlossinessTex(NULL),
	m_htGlowTex(NULL),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),

	m_vAddLight(0.0f,0.0f,0.0f),

	m_fAniGlowAlpha(0.0f),
	m_fAniValue(0.0f),

	m_bPossibleEffect(TRUE)
{
	ZeroMemory( m_szNormalMap, MAX_PATH );
	ZeroMemory( m_szSpecularTex, MAX_PATH );
	ZeroMemory( m_szGlossinessTex, MAX_PATH );
	ZeroMemory( m_szGlowTex, MAX_PATH );
}

DxEffCharNormal::~DxEffCharNormal(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharNormal::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		return NULL;
	}

	HRESULT hr;
	DxEffCharNormal *pEffChar = new DxEffCharNormal;
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

HRESULT DxEffCharNormal::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	if ( !DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader() )	// PS.2.0 하드웨어 가속이 가능 할 경우만 로드
	{
		m_bPossibleEffect = FALSE;
		return E_FAIL;
	}
	
	if( strlen( m_szNormalMap ) == 0 ) return S_OK;


	if( !m_pEffect )
	{

		// 정점 선언 오브젝트 생성
		if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( VertexDecl, &m_pNormalDCRT ) ) )
			return E_FAIL;

		// 셰이더 읽기
		LPD3DXBUFFER pErr = NULL;	
		UINT size = (UINT)strlen(pNormalHLSL);
		if( FAILED(D3DXCreateEffect( pd3dDevice, pNormalHLSL, size, NULL, NULL, 0, NULL, &m_pEffect, &pErr ) ) )
		{
			CDebugSet::ToListView ( "[ERROR]D3DXCreateEffect FAILED ERROR: %s", pErr->GetBufferPointer() );

#ifdef _RELEASED
			MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
#endif

			SAFE_RELEASE(pErr);
			return E_FAIL;
		}

		m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
		m_hvLightDir = m_pEffect->GetParameterByName( NULL, "vLightDir" );
		m_hvEyePos	 = m_pEffect->GetParameterByName( NULL, "vEyePos" );

		m_htNormalMap		= m_pEffect->GetParameterByName( NULL, "NormalMap" );
		m_htSpecularTex		= m_pEffect->GetParameterByName( NULL, "SpecularTex" );
		m_htGlossinessTex	= m_pEffect->GetParameterByName( NULL, "GlossinessTex" );
		m_htGlowTex			= m_pEffect->GetParameterByName( NULL, "GlowTex" );

		m_htWorld		= m_pEffect->GetParameterByName( NULL, "mWorld" );
		m_htView		= m_pEffect->GetParameterByName( NULL, "mView" );
		m_htProj		= m_pEffect->GetParameterByName( NULL, "mProj" );

		m_htAmbient		= m_pEffect->GetParameterByName( NULL, "fvAmbient" );
		m_htDiffuse		= m_pEffect->GetParameterByName( NULL, "fvDiffuse" );
		m_htSpecular	= m_pEffect->GetParameterByName( NULL, "fvSpecular" );
		m_htbUseDirectionLight = m_pEffect->GetParameterByName( NULL, "bUseDirectionLight" );

		m_htGlowAlpha = m_pEffect->GetParameterByName( NULL, "fAniGlowAlpha" );


		SAFE_RELEASE(pErr);
	}

	LPDIRECT3DTEXTURE9 pHeightTexture = NULL;

	// 스펙큘라 맵 읽기
	if( strlen( m_szSpecularTex ) != 0 && m_bUseSpecularTex )
	{
        m_textureResSpecular = TextureManager::GetInstance().LoadTexture(
            m_szSpecularTex,
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
	m_pEffect->SetBool( "bUseSpecularTex", m_bUseSpecularTex );

	if( strlen( m_szGlossinessTex ) != 0 && m_bUseGlossinessTex )
	{
        m_textureResGlossiness = TextureManager::GetInstance().LoadTexture(
            m_szGlossinessTex,
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
	m_pEffect->SetBool( "bUseGlossinessTex", m_bUseGlossinessTex );

	if( strlen( m_szGlowTex ) != 0 && m_bUseGlowTex )
	{
        m_textureResGlow = TextureManager::GetInstance().LoadTexture(
            m_szGlowTex,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            EMMM_CHAR_MIPMAP,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_CHAR,
			true,
			FALSE );

		m_pEffect->SetFloat( "fGlowBright", m_fGlowBright );
		m_pEffect->SetFloat( "fAniGlowAlpha", m_fAniGlowAlpha );
	}
    else
    {
		m_textureResGlow.Clear();
	}
	m_pEffect->SetBool( "bUseGlowTex", m_bUseGlowTex );	
	m_pEffect->SetFloat( "fSpecularPower", m_fSpecularPower );

	// 높이맵 읽기
    m_textureResNormal = TextureManager::GetInstance().LoadTexture(
        m_szNormalMap,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        EMMM_CHAR_MIPMAP,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

	// m_pmcMesh
	IsExist_Copy_pmcMesh( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharNormal::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bPossibleEffect ) 
		return S_OK;

	// 이펙트
	if(m_pEffect) 
		m_pEffect->OnResetDevice();

	if( m_pmcMesh )
	{
		if ( !m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh )
		{
			m_pmcMesh->GenerateSkinnedMeshNormalMap( pd3dDevice );

			LPD3DXMESH pMesh;
			m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->CloneMesh( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->GetOptions(), VertexDecl, pd3dDevice, &pMesh );
			D3DXComputeTangent( pMesh, 0, 0, 0, TRUE, NULL );

			SAFE_RELEASE( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh );
			m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh = pMesh;
		}
	}

	return S_OK;
}

HRESULT DxEffCharNormal::InvalidateDeviceObjects ()
{
	if( !m_bPossibleEffect ) 
		return S_OK;

	if( m_pEffect ) m_pEffect->OnLostDevice();

	return S_OK;
}

HRESULT DxEffCharNormal::DeleteDeviceObjects ()
{
	if( !m_bPossibleEffect ) 
		return S_OK;

	SAFE_RELEASE( m_pEffect );
	SAFE_RELEASE( m_pNormalDCRT );

    m_textureResNormal.Clear();
    m_textureResSpecular.Clear();
    m_textureResGlossiness.Clear();
    m_textureResGlow.Clear();

	return S_OK;
}


HRESULT DxEffCharNormal::FrameMove ( float fTime, float fElapsedTime )
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

void DxEffCharNormal::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	if( !m_bPossibleEffect || !m_pSkinMesh || !m_pEffect )
        return;

	if ( !IsExist_Copy_pmcMesh( pd3dDevice ) )
		return;

    if( !m_textureResNormal.Update() )
	{
        return;
	}
	if( strlen( m_szSpecularTex ) != 0 && m_bUseSpecularTex )
	{
		if ( !m_textureResSpecular.Update() )
			return;
	}
	if( strlen( m_szGlossinessTex ) != 0 && m_bUseGlossinessTex )
	{
		if ( !m_textureResGlossiness.Update() )
			return;
	}
	if( strlen( m_szGlowTex ) != 0 && m_bUseGlowTex )
	{
		if ( !m_textureResGlow.Update() )
			return;
	}

    // Note : ReloadTexture - Loading이 안 된 것을 위한 조치.

	D3DXMATRIX matView, matProj, mat, matRotate;
	//pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	//pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	matView = NSMaterialSkinManager::GetMatrixView();
	matProj = NSMaterialSkinManager::GetMatrixProjection();



	// 로컬 - 투영 변환 행렬
	m_pEffect->SetMatrix( m_htWorld, &m_matWorld );
	m_pEffect->SetMatrix( m_htView, &matView );
	m_pEffect->SetMatrix( m_htProj, &matProj );


	// 조명 방향(로컬 좌표계)
	D3DXVECTOR4 v;

	D3DLIGHT9 light;
	pd3dDevice->GetLight( 0, &light );

	mat = m_matWorld * matView * matProj;
	D3DXVECTOR4 light_pos = D3DXVECTOR4( light.Direction.x + m_vAddLight.x, light.Direction.y + m_vAddLight.y, light.Direction.z + m_vAddLight.z, 0);
	D3DXMatrixInverse( &mat, NULL, &m_matWorld);
	D3DXVec4Transform( &v, &light_pos, &mat );
	D3DXVec3Normalize( (D3DXVECTOR3 *)&v, (D3DXVECTOR3 *)&v );


	if( g_bCHAR_EDIT_RUN )
	{
		D3DXVECTOR3 lightDir = D3DXVECTOR3( light_pos.x, light_pos.y, light_pos.z );
		EDITMESHS::RENDERSPHERE( pd3dDevice, lightDir, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), FALSE, NULL, 0xffff0000 );

		CDebugSet::ToView( 0, 2, "Light_pos %fx %fy %fz", light_pos.x, light_pos.y, light_pos.z );
		CDebugSet::ToView( 0, 3, "vpos %fx %fy %fz", m_matWorld._41, m_matWorld._42, m_matWorld._43 );


		D3DXVECTOR4 vInputLightPos = D3DXVECTOR4( light_pos.x, light_pos.y, light_pos.z, 0.0f );

		D3DXVECTOR4 vAmbient = D3DXVECTOR4(m_cAmbient.r,m_cAmbient.g,m_cAmbient.b,m_cAmbient.a);
		D3DXVECTOR4 vDiffuse = D3DXVECTOR4(m_cDiffuse.r,m_cDiffuse.g,m_cDiffuse.b,m_cDiffuse.a);
		D3DXVECTOR4 vSpecular = D3DXVECTOR4(m_cSpecular.r,m_cSpecular.g,m_cSpecular.b,m_cSpecular.a);

		m_pEffect->SetVector( m_htAmbient, &vAmbient );
		m_pEffect->SetVector( m_htDiffuse, &vDiffuse );
		m_pEffect->SetVector( m_htSpecular, &vSpecular );	
		

		m_pEffect->SetVector( m_hvLightDir, &vInputLightPos );

	}else{
		D3DXVECTOR4 vInputLightPos = D3DXVECTOR4( -light_pos.x, light_pos.y, -light_pos.z, 0.0f );
		D3DXVECTOR4 vLightAmbient  = D3DXVECTOR4( light.Ambient.r, light.Ambient.g, light.Ambient.b, light.Ambient.a );
		D3DXVECTOR4 vLightDiffuse  = D3DXVECTOR4( light.Diffuse.r, light.Diffuse.g, light.Diffuse.b, light.Diffuse.a );
		D3DXVECTOR4 vLightSpecular = D3DXVECTOR4( light.Specular.r, light.Specular.g, light.Specular.b, light.Specular.a );

		m_pEffect->SetBool( m_htbUseDirectionLight, TRUE );

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
				m_pEffect->SetBool( m_htbUseDirectionLight, FALSE );
			}	
		}

		D3DXVECTOR4 vAmbient  = ( D3DXVECTOR4(m_cAmbient.r,m_cAmbient.g,m_cAmbient.b,m_cAmbient.a) + vLightAmbient ) / 2.0f;
		D3DXVECTOR4 vDiffuse  = ( D3DXVECTOR4(m_cDiffuse.r,m_cDiffuse.g,m_cDiffuse.b,m_cDiffuse.a) + vLightDiffuse ) / 2.0f;
		D3DXVECTOR4 vSpecular = ( D3DXVECTOR4(m_cSpecular.r,m_cSpecular.g,m_cSpecular.b,m_cSpecular.a) + vLightSpecular ) / 2.0f;

		m_pEffect->SetVector( m_htAmbient, &vAmbient );
		m_pEffect->SetVector( m_htDiffuse, &vDiffuse );
		m_pEffect->SetVector( m_htSpecular, &vSpecular );	

		m_pEffect->SetVector( m_hvLightDir, &vInputLightPos );
	}

	// 시전(로컬 좌표계)
	mat = m_matWorld * matView;
	D3DXMatrixInverse( &mat, NULL, &mat );
	v = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
	D3DXVec4Transform( &v, &v, &mat );
//	v = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_pEffect->SetVector( m_hvEyePos, &v );

	// 법선 맵
	m_pEffect->SetTexture( m_htNormalMap, m_textureResNormal.GetTexture() );

    // 스펙큘라 맵
	if( m_bUseSpecularTex )
	{
		m_pEffect->SetTexture( m_htSpecularTex, m_textureResSpecular.GetTexture() );
	}

	// 글로시니스 맵
	if( m_bUseGlossinessTex )
	{
		m_pEffect->SetTexture( m_htGlossinessTex, m_textureResGlossiness.GetTexture() );
	}

	// 글로우 맵
	if( m_bUseGlowTex )
	{
		m_pEffect->SetTexture( m_htGlowTex, m_textureResGlow.GetTexture() );
		m_pEffect->SetFloat( m_htGlowAlpha, m_fAniGlowAlpha );
	}

	// 정점 선언
	pd3dDevice->SetVertexDeclaration( m_pNormalDCRT );	

	m_pEffect->SetTechnique( m_hTechnique );


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

	
	// 현재는 Fog 가 Enable 로 되어있다.
	// 하지만 DxEffCharNormal 는  제작시 Fog 고려 안하고 만들었고, 많이 사용되지 않을 예정이다. (MaterialSystem 때문에)
	// 그래서 강제로 Fog 를 끄는 방향으로 하였다.
	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	m_pSkinMesh->DrawMeshNORMALMAP( pd3dDevice, m_pmcMesh, pBoneMatrices, m_pEffect, sCharSetting.pmtrlPiece );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
}

HRESULT DxEffCharNormal::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	EFFCHAR_PROPERTY_NORMAL_100_101 Property;
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_NORMAL_100_101) );

	m_Property.Assign( Property );
	
	return S_OK;
}

HRESULT DxEffCharNormal::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	EFFCHAR_PROPERTY_NORMAL_100_101 Property;
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_NORMAL_100_101) );

	m_Property.Assign( Property );

	return S_OK;
}

HRESULT DxEffCharNormal::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 읽기.
	SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

HRESULT	DxEffCharNormal::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
	case 0x0101:	LoadFile_0101( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0102( SFile, pd3dDevice );	break;
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

HRESULT	DxEffCharNormal::SaveFile ( sc::BaseStream &SFile )
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

BOOL DxEffCharNormal::IsExist_Copy_pmcMesh( LPDIRECT3DDEVICEQ pd3dDevice )
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
		m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->CloneMesh( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->GetOptions(), VertexDecl, pd3dDevice, &pMesh );
		D3DXComputeTangent( pMesh, 0, 0, 0, TRUE, NULL );

		SAFE_RELEASE( m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh );
		m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh = pMesh;
	}

	return TRUE;
}