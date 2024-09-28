#include "pch.h"
#include "../../../../SigmaCore/String/StringUtils.h"
#include "../../../../SigmaCore/Util/Util.h"
#include "../../../../SigmaCore/File/SerialFile.h"

#include "../../../DxEffect/Char/DxEffChar.h"
#include "../../../DxTools/RENDERPARAM.h"
#include "../../../DxTools/DxMethods.h"
#include "../../../DxTools/PrivateResource.h"
#include "../../../DxTools/DebugSet.h"
#include "../../../Common/FileSystem.h"
#include "../DxSkeletonMaintain.h"
#include "../DxSkeletonManager.h"
#include "../DxSkinMeshManager.h"
//#include "./DxSkinMesh9_CPU.h"
#include "./DxSkinMesh9_HLSL.h"
#include "./DxSkinMesh9_NORMAL.h"
#include "./DxSkinMesh9_Material.h"
#include "./DxSkinMeshContainer9.h"

#include "DxSkinMesh9.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define V(x)           { hr = x; }

void EFFCHAR_PROPERTY_OVERLAY_TEXTURE::SaveFile( sc::BaseStream &SFile )
{
    SFile << m_bOverlay;

    // 기존에 sizeof( EFFCHAR_PROPERTY_OVERLAY_TEXTURE ) 사용해서 읽기/쓰기를 했었다
    // 문제는 pragma pack (4) 기준으로 저장되어 있어서 바이트 수를 맞춰줘야한다 ㅠ
    BYTE bPragmaPack = 0;
    SFile << bPragmaPack;
    SFile << bPragmaPack;
    SFile << bPragmaPack;

    SFile.WriteBuffer( &m_vLightPos, sizeof( m_vLightPos ) );
    SFile.WriteBuffer( &m_vEyePos, sizeof( m_vEyePos ) );
    SFile.WriteBuffer( &m_matWorld, sizeof( m_matWorld ) );
    SFile.WriteBuffer( &m_matView, sizeof( m_matView ) );
    SFile.WriteBuffer( &m_matProj, sizeof( m_matProj ) );
    SFile.WriteBuffer( &m_matWorldInverse, sizeof( m_matWorldInverse ) );
    SFile.WriteBuffer( &m_matWorldInvTrans, sizeof( m_matWorldInvTrans ) );
    SFile.WriteBuffer( &m_matViewInverse, sizeof( m_matViewInverse ) );
    SFile.WriteBuffer( &m_vColor1, sizeof( m_vColor1 ) );
    SFile.WriteBuffer( &m_vColor2, sizeof( m_vColor2 ) );
    SFile.WriteBuffer( &m_vAmbient, sizeof( m_vAmbient ) );
    SFile.WriteBuffer( &m_vDiffuse, sizeof( m_vDiffuse ) );
    SFile.WriteBuffer( &m_vSpecular, sizeof( m_vSpecular ) );
    SFile.WriteBuffer( &m_szOverlayTex, sizeof( char ) * MAX_PATH );

    SFile << DWORD( 1 );

    SFile << m_bUseSpecular;
    SFile << m_bUseGlowTex;

    SFile << bPragmaPack;
    SFile << bPragmaPack;

    SFile.WriteBuffer( &m_szGlowTex, sizeof( char ) * MAX_PATH );

    SFile << DWORD( 1 );

    SFile << m_fGlowBright;
    SFile << m_fGlowAlpha;
    SFile << m_bUseReflectTex;

    SFile << bPragmaPack;
    SFile << bPragmaPack;
    SFile << bPragmaPack;

    SFile.WriteBuffer( &m_szReflectTex, sizeof( char ) * MAX_PATH );

    SFile << DWORD( 1 );

    SFile << m_fReflectActivity;
    SFile << m_bUseAlphaReflectTex;

    SFile << bPragmaPack;
    SFile << bPragmaPack;
    SFile << bPragmaPack;

    SFile.WriteBuffer( &m_szReflectAlphaTex, sizeof( char ) * MAX_PATH );

    SFile << DWORD( 1 );

    SFile << m_bUseDirLight;

    SFile << bPragmaPack;
    SFile << bPragmaPack;
    SFile << bPragmaPack;

    SFile << m_fSpecularPower;
    SFile << m_fSpecularLevel;
    SFile << m_fAlphaRef;
    SFile.WriteBuffer( &m_cDiffuse, sizeof( m_cDiffuse ) );
    SFile.WriteBuffer( &m_cAmbient, sizeof( m_cAmbient ) );
    SFile.WriteBuffer( &m_matWVP, sizeof( m_matWVP ) );
    SFile.WriteBuffer( &m_matVP, sizeof( m_matVP ) );
}

void EFFCHAR_PROPERTY_OVERLAY_TEXTURE::LoadFile( sc::BaseStream &SFile )
{
    SFile >> m_bOverlay;

    // 기존에 sizeof( EFFCHAR_PROPERTY_OVERLAY_TEXTURE ) 사용해서 읽기/쓰기를 했었다
    // 문제는 pragma pack (4) 기준으로 저장되어 있어서 바이트 수를 맞춰줘야한다 ㅠ
    BYTE bPragmaPack = 0;
    SFile >> bPragmaPack;
    SFile >> bPragmaPack;
    SFile >> bPragmaPack;

    SFile.ReadBuffer( &m_vLightPos, sizeof( m_vLightPos ) );
    SFile.ReadBuffer( &m_vEyePos, sizeof( m_vEyePos ) );
    SFile.ReadBuffer( &m_matWorld, sizeof( m_matWorld ) );
    SFile.ReadBuffer( &m_matView, sizeof( m_matView ) );
    SFile.ReadBuffer( &m_matProj, sizeof( m_matProj ) );
    SFile.ReadBuffer( &m_matWorldInverse, sizeof( m_matWorldInverse ) );
    SFile.ReadBuffer( &m_matWorldInvTrans, sizeof( m_matWorldInvTrans ) );
    SFile.ReadBuffer( &m_matViewInverse, sizeof( m_matViewInverse ) );
    SFile.ReadBuffer( &m_vColor1, sizeof( m_vColor1 ) );
    SFile.ReadBuffer( &m_vColor2, sizeof( m_vColor2 ) );
    SFile.ReadBuffer( &m_vAmbient, sizeof( m_vAmbient ) );
    SFile.ReadBuffer( &m_vDiffuse, sizeof( m_vDiffuse ) );
    SFile.ReadBuffer( &m_vSpecular, sizeof( m_vSpecular ) );
    SFile.ReadBuffer( &m_szOverlayTex, sizeof( char ) * MAX_PATH );

    DWORD nTemp;
    SFile >> nTemp;

    SFile >> m_bUseSpecular;
    SFile >> m_bUseGlowTex;

    SFile >> bPragmaPack;
    SFile >> bPragmaPack;

    SFile.ReadBuffer( &m_szGlowTex, sizeof( char ) * MAX_PATH );

    SFile >> nTemp;

    SFile >> m_fGlowBright;
    SFile >> m_fGlowAlpha;
    SFile >> m_bUseReflectTex;

    SFile >> bPragmaPack;
    SFile >> bPragmaPack;
    SFile >> bPragmaPack;

    SFile.ReadBuffer( &m_szReflectTex, sizeof( char ) * MAX_PATH );

    SFile >> nTemp;

    SFile >> m_fReflectActivity;
    SFile >> m_bUseAlphaReflectTex;

    SFile >> bPragmaPack;
    SFile >> bPragmaPack;
    SFile >> bPragmaPack;

    SFile.ReadBuffer( &m_szReflectAlphaTex, sizeof( char ) * MAX_PATH );

    SFile >> nTemp;

    SFile >> m_bUseDirLight;

    SFile >> bPragmaPack;
    SFile >> bPragmaPack;
    SFile >> bPragmaPack;

    SFile >> m_fSpecularPower;
    SFile >> m_fSpecularLevel;
    SFile >> m_fAlphaRef;
    SFile.ReadBuffer( &m_cDiffuse, sizeof( m_cDiffuse ) );
    SFile.ReadBuffer( &m_cAmbient, sizeof( m_cAmbient ) );
    SFile.ReadBuffer( &m_matWVP, sizeof( m_matWVP ) );
    SFile.ReadBuffer( &m_matVP, sizeof( m_matVP ) );
}

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

//	D3DNONINDEXED	D3DINDEXEDHLSLVS
//METHOD	DxSkinMesh9::m_SkinningMethod =	D3DINDEXEDHLSLVS;	//D3DNONINDEXED;	//D3DINDEXEDHLSLVS;
DWORD			DxSkinMesh9::m_dwBehaviorFlags = NULL;
D3DXMATRIXA16*	DxSkinMesh9::g_pBoneMatrices	= NULL;
DWORD           DxSkinMesh9::g_NumBoneMatricesReserve = 0;
DWORD			DxSkinMesh9::g_NumBoneMatricesMax = 0;
D3DXMATRIXA16	DxSkinMesh9::g_matIdentity;
//
//ID3DXEffect*					DxSkinMesh9::g_pShadowFX = NULL;
//ID3DXEffect*					DxSkinMesh9::g_pShadowSkinFX = NULL;
//LPDIRECT3DVERTEXDECLARATION9	DxSkinMesh9::g_pShadowDCRT = NULL;
//LPDIRECT3DVERTEXDECLARATION9	DxSkinMesh9::g_pShadowSkinDCRT = NULL;
//
//static const char* pHLSL_Shadow =
//{
//	//
//	// Skinned Mesh Effect file 
//	// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
//	//
//	"float4x4    mWorldViewProj : VIEWPROJECTION;\r\n"
//	"float       g_fDepthOffset = 0.001f; \r\n"
//
//	///////////////////////////////////////////////////////
//	"struct VS_INPUT\r\n"
//	"{\r\n"
//	"	float4  Pos     : POSITION;\r\n"
//	"	float3  vNor    : NORMAL;\r\n"
//	"	float2  vTex    : TEXCOORD0;\r\n"
//	"};\r\n"
//
//	"struct VS_OUTPUT\r\n"
//	"{\r\n"
//	"	float4  Pos     : POSITION;\r\n"
//	"	float2  Depth    : TEXCOORD0;\r\n"
//	"};\r\n"
//
//	"VS_OUTPUT VShade(VS_INPUT i)\r\n"
//	"{\r\n"
//	"	VS_OUTPUT   o;\r\n"
//
//	// transform position from world space into view and then projection space
//	"	o.Pos = mul(i.Pos, mWorldViewProj);\r\n"
//
//	// copy the input texture coordinate through
//	"	o.Depth.xy = o.Pos.zw;\r\n"
//
//	"	return o;\r\n"
//	"}\r\n"
//
//	//-----------------------------------------------------------------------------
//	// Pixel Shader: PixShadow
//	// Desc: Process pixel for the shadow map
//	//-----------------------------------------------------------------------------
//	"float4 PS( VS_OUTPUT In ) : COLOR0 \r\n"
//	"{\r\n"
//	//
//	// Depth is z / w
//	//
//	// 캐릭터는 약간만 앞쪽에 있다고 셋팅하자.
//	// -0.001f 이 안쓰였을 시에는 발쪽 부분의 그림자가 안나오는 문제가 생겼었음.
//	"	float4 Color = (In.Depth.x-g_fDepthOffset) / In.Depth.y;\r\n"
//	" return Color;\r\n"
//	"}\r\n"
//
//	//////////////////////////////////////
//	// Techniques specs follow
//	//////////////////////////////////////
//	"technique t0\r\n"
//	"{\r\n"
//	"	pass p0\r\n"
//	"	{\r\n"
//	"		VertexShader = compile vs_2_0 VShade();\r\n"
//	"		PixelShader = compile ps_2_0 PS();\r\n"
//	"	}\r\n"
//	"}\r\n"
//};
//
//static const char* pHLSL_Shadow_Skin =
//{
//	//
//	// Skinned Mesh Effect file 
//	// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
//	//
//
//	// Matrix Pallette
//	"static const int MAX_MATRICES = 60;\r\n"
//	"float4x3    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"
//	"float4x4    mViewProj : VIEWPROJECTION;\r\n"
//	"float       g_fDepthOffset = 0.001f; \r\n"
//
//	///////////////////////////////////////////////////////
//	"struct VS_INPUT\r\n"
//	"{\r\n"
//	"	float4  Pos             : POSITION;\r\n"
//	"	float4  BlendWeights    : BLENDWEIGHT;\r\n"
//	"	float4  BlendIndices    : BLENDINDICES;\r\n"
//	"};\r\n"
//
//	"struct VS_OUTPUT\r\n"
//	"{\r\n"
//	"	float4  Pos     : POSITION;\r\n"
//	"	float2  Depth    : TEXCOORD0;\r\n"
//	"};\r\n"
//
//	"VS_OUTPUT VShade(VS_INPUT i, uniform int NumBones)\r\n"
//	"{\r\n"
//	"	VS_OUTPUT   o;\r\n"
//	"	float3      Pos = 0.0f;\r\n"
//	"	float3      Normal = 0.0f;    \r\n"
//	"	float       LastWeight = 0.0f;\r\n"
//
//	// Compensate for lack of UBYTE4 on Geforce3
//	"	int4 IndexVector = D3DCOLORtoUBYTE4(i.BlendIndices);\r\n"
//
//	// cast the vectors to arrays for use in the for loop below
//	"	float BlendWeightsArray[4] = (float[4])i.BlendWeights;\r\n"
//	"	int   IndexArray[4]        = (int[4])IndexVector;\r\n"
//
//	// calculate the pos/normal using the "normal" weights 
//	//        and accumulate the weights to calculate the last weight
//	"	for (int iBone = 0; iBone < NumBones-1; iBone++)\r\n"
//	"	{\r\n"
//	"		LastWeight = LastWeight + BlendWeightsArray[iBone];\r\n"
//
//	"		Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
//	"	}\r\n"
//	"	LastWeight = 1.0f - LastWeight; \r\n"
//
//	// Now that we have the calculated weight, add in the final influence
//	"	Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);\r\n"
//
//	// transform position from world space into view and then projection space
//	"	o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);\r\n"
//
//	// copy the input texture coordinate through
//	"	o.Depth.xy = o.Pos.zw;\r\n"
//
//	"	return o;\r\n"
//	"}\r\n"
//
//	//-----------------------------------------------------------------------------
//	// Pixel Shader: PixShadow
//	// Desc: Process pixel for the shadow map
//	//-----------------------------------------------------------------------------
//	"float4 PS( VS_OUTPUT In ) : COLOR0 \r\n"
//	"{\r\n"
//	//
//	// Depth is z / w
//	//
//	// 캐릭터는 약간만 앞쪽에 있다고 셋팅하자.
//	// -0.001f 이 안쓰였을 시에는 발쪽 부분의 그림자가 안나오는 문제가 생겼었음.
//	"	float4 Color = (In.Depth.x-g_fDepthOffset) / In.Depth.y;\r\n"
//	" return Color;\r\n"
//	"}\r\n"
//
//	"int CurNumBones = 2;\r\n"
//	"VertexShader vsArray[4] = { compile vs_1_1 VShade(1), \r\n"
//	"compile vs_1_1 VShade(2),\r\n"
//	"compile vs_1_1 VShade(3),\r\n"
//	"compile vs_1_1 VShade(4)\r\n"
//	"};\r\n"
//
//
//	//////////////////////////////////////
//	// Techniques specs follow
//	//////////////////////////////////////
//	"technique t0\r\n"
//	"{\r\n"
//	"	pass p0\r\n"
//	"	{\r\n"
//	"		VertexShader = (vsArray[CurNumBones]);\r\n"
//	"		PixelShader = compile ps_2_0 PS();\r\n"
//	"	}\r\n"
//	"}\r\n"
//};

DxSkinMesh9::DxSkinMesh9 () :
	m_pFrameRoot(NULL),
	//m_pSkeleton(0),
	m_vObjectCenter(0,0,0),
	m_fObjectRadius(0),
	m_pAnimController(NULL),

	m_pMeshContainerHead(NULL),

	m_bOriginDraw(TRUE),
	m_bAlpha(FALSE),
	m_bShadow(FALSE),
	m_bAlphaTex(FALSE),
	m_bLevel(FALSE)

    , m_pSrcData(NULL)
	, m_nSrcDataSize(0)
	, m_dwFlagsEX(0L)
{
	Init_Detail_Texture ();
}

DxSkinMesh9::~DxSkinMesh9()
{
    OnSkinDestroyDevice ();
	OnSkinCleanUp ();
}

void DxSkinMesh9::CreateBoneMatrices()
{
	if( g_NumBoneMatricesReserve <= 0 )
		return;

	DestroyBoneMatrices();

	g_NumBoneMatricesMax = g_NumBoneMatricesReserve;
	g_NumBoneMatricesReserve = 0;

	g_pBoneMatrices = new D3DXMATRIXA16[ g_NumBoneMatricesMax ];

	D3DXMatrixIdentity( &g_matIdentity );
}

void DxSkinMesh9::DestroyBoneMatrices()
{
	SAFE_DELETE_ARRAY( g_pBoneMatrices );

	g_NumBoneMatricesMax = 0;
}

void DxSkinMesh9::DataExchange(DxSkinMesh9& rhs)
{
	if (this != &rhs)
	{
		std::swap(m_pFrameRoot, rhs.m_pFrameRoot);
		std::swap(m_pAnimController, rhs.m_pAnimController);

		m_pMeshContainerHead	= rhs.m_pMeshContainerHead;			

		m_vObjectCenter	= rhs.m_vObjectCenter;
		m_fObjectRadius	= rhs.m_fObjectRadius;			

		m_bOriginDraw	= rhs.m_bOriginDraw;
		m_bAlpha		= rhs.m_bAlpha;
		m_bShadow		= rhs.m_bShadow;
		m_bAlphaTex		= rhs.m_bAlphaTex;
		m_bLevel		= rhs.m_bLevel;

		m_pSrcData		= rhs.m_pSrcData;
		m_nSrcDataSize	= rhs.m_nSrcDataSize;
	}
}

//--------------------------------------------------------------------------------------
// Called to setup the pointers for a given bone to its transformation matrix
//--------------------------------------------------------------------------------------
HRESULT DxSkinMesh9::SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
	if ( !m_skeletonRes.IsValid() )	
		return E_FAIL;

    UINT iBone, cBones;
	DxBoneTrans* pBoneTrans;

    SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

    // if there is a skinmesh, then setup the bone matrices
    if (pMeshContainer->pSkinInfo != NULL)
    {
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//pMeshContainer->m_ppBoneMatrixPtrs = new D3DXMATRIX*[ cBones ];
  //      if (pMeshContainer->m_ppBoneMatrixPtrs == NULL)
  //          return E_OUTOFMEMORY;

		pMeshContainer->m_arrayBoneIndex = new DWORD[ cBones ];
		if (pMeshContainer->m_arrayBoneIndex == NULL)
			return E_OUTOFMEMORY;

		// [shhan][2015.01.05] 일단 초기화를 하여 return E_FAIL; 이 되더라도 문제가 클라이언트가 종료되는 문제는 없도록 하자.
		//						잘못된 값이 들어가서 CharPartBoneMatrices::CaculateBoneMatrices 상에 
		//						sSkeleton.FindBone_Index( sMeshContainer.m_arrayBoneIndex[iMatrixIndex] )->m_matCombined 에서 문제가 발생할 수 있다.
		//						그래서 초기화를 해주고 알맞는 정보가 없을 경우 return E_FAIL; 를 해준다.
		//						
		//						원래대로라면 E_FAIL 시 렌더링을 안하게 해야 하는게 맞는데, 기존에 잘못되었지만 렌더링 되었기 때문에,
		//						클라가 종료 안되고 렌더링은 되도록 하기 위해(비정상적) 초기값 0 값을 주었다.
		//
		//						비정상적인 위치에 렌더링 될꺼기 때문에 추후 데이터 수정을 하면 된다.
		//
		//						추리 : Bone이 많은 상태로 Skin이 제작이 되었다가 Bone 을 줄이면서 그 값이 없어질 수도 있다.
		//
		for (iBone = 0; iBone < cBones; iBone++)
		{
			pMeshContainer->m_arrayBoneIndex[ iBone ] = 0;
		}

		for (iBone = 0; iBone < cBones; iBone++)
		{
			pBoneTrans = m_skeletonRes.get()->FindBone_t( pMeshContainer->pSkinInfo->GetBoneName(iBone) );
			if ( pBoneTrans == NULL )
			{
				// [shhan][2014.10.22] skin.x 에는 Bone 이 있지만 Bone.x 에는 그 Bone 이 없는 경우이다.
				//						경고 Msg 를 띄우는게 좋겠다.
				TSTRING strName(_T("BoneFile : "));
				strName += m_strName_BONE + _T(" , SkinFile : ") + m_strName_SKIN + _T(" , Error Bone Name : ");
				strName += static_cast<TSTRING>( pMeshContainer->pSkinInfo->GetBoneName(iBone) );

				CDebugSet::ToLogFile( strName );

				GASSERT( 0 && _T("SetupBoneMatrixPointersOnMesh - Skin.x 에 특정 Bone이 Bone.x에 없습니다. log를 참조해주세요.") );

				return E_FAIL;
			}

			//pMeshContainer->m_ppBoneMatrixPtrs[ iBone ] = &pBoneTrans->matCombined;
			pMeshContainer->m_arrayBoneIndex[ iBone ] = m_skeletonRes.get()->GetReferenceIndex_t( pMeshContainer->pSkinInfo->GetBoneName(iBone) );
		}
    }
	else
	{
		SFrame* pFrame = (SFrame*)pFrameBase;

		size_t nNameSize(0);
		if ( pFrame->Name )
		{			
			StringCchLength( pFrame->Name, MAX_PATH, &nNameSize );
		}

		if ( nNameSize > 0 )
		{
			// [shhan][2014.12.15] 이름이 있을 경우.
			//						바로 연결된 본이 나온다. ( 새로운 Exporter )
			SFrame* pBoneFrame = FindParentFrame ( pFrame );
			if( pBoneFrame && pBoneFrame->Name )
			{
				pMeshContainer->m_strBoneName = pBoneFrame->Name;

				pMeshContainer->m_dwNumBoneMatrix = cBones = SMeshContainer::g_dwStaticBoneMatrixNum;
				pMeshContainer->m_ppStaticMatrixPtrs = new D3DXMATRIX*[ cBones ];

				pMeshContainer->m_ppStaticMatrixPtrs[0] = &pFrame->TransformationMatrix;			//[LOCAL]
				//pMeshContainer->m_ppStaticMatrixPtrs[1] = &pBoneFrame->TransformationMatrix;	//[LOCAL] parent	// 이 값이 필요없어졌는데 값을 안넣을 수는 없고..
				pMeshContainer->m_ppStaticMatrixPtrs[1] = &g_matIdentity;

				pMeshContainer->m_arrayBoneIndex = new DWORD[ 1 ];
				pMeshContainer->m_arrayBoneIndex[ 0 ] = m_skeletonRes.get()->GetReferenceIndex_t( pBoneFrame->Name );

				// [shhan][2015.01.05] 원래대로라면 E_FAIL 시 렌더링을 안하게 해야 하는게 맞는데, 기존에 잘못되었지만 렌더링 되었기 때문에,
				//						클라가 종료 안되고 렌더링은 되도록 하기 위해(비정상적) 초기값 0 값을 주었다.
				//						비정상적인 위치에 렌더링 될꺼기 때문에 추후 데이터 수정을 하면 된다.
				//
				if ( pMeshContainer->m_arrayBoneIndex[ 0 ] == -1 )
				{
					// [shhan][2015.01.05] skin.x 에는 Bone 이 있지만 Bone.x 에는 그 Bone 이 없는 경우이다.
					//						경고 Msg 를 띄우는게 좋겠다.
					TSTRING strName(_T("BoneFile : "));
					strName += m_strName_BONE + _T(" , SkinFile : ") + m_strName_SKIN + _T(" , Error Bone Name : ");
					strName += pBoneFrame->Name;

					CDebugSet::ToLogFile( strName );

					GASSERT( 0 && _T("SetupBoneMatrixPointersOnMesh - 부모 Bone.x에 Bone이 없습니다. log를 참조해주세요.") );

					pMeshContainer->m_arrayBoneIndex[ 0 ] = 0;

					return E_FAIL;
				}
			}
		}
		else	
		{
			// [shhan][2014.12.15] 이름이 없을 경우.
			//						예전에는 한단계를 더 거쳐야 연결된 본이 나왔다. ( 구형 Exporter )
			SFrame* pframeParent = FindParentFrame ( pFrame );
			if ( pframeParent )
			{
				SFrame* pBoneFrame = FindParentFrame ( pframeParent );
				if( pBoneFrame && pBoneFrame->Name )
				{
					pMeshContainer->m_strBoneName = pBoneFrame->Name;

					pMeshContainer->m_dwNumBoneMatrix = cBones = SMeshContainer::g_dwStaticBoneMatrixNum;
					pMeshContainer->m_ppStaticMatrixPtrs = new D3DXMATRIX*[ cBones ];

					pMeshContainer->m_ppStaticMatrixPtrs[0] = &pFrame->TransformationMatrix;			//[LOCAL]
					pMeshContainer->m_ppStaticMatrixPtrs[1] = &pframeParent->TransformationMatrix;	//[LOCAL] parent

					// [shhan][2014.09.29] Max 에서 Scale 마이너스 값 들어있는 문제를 해결하기 위한 작업.
					//						Skin 쪽 구형 Export 사용시 이 조건으로 들어갈 수 있다.
					{
						D3DXVECTOR3 vScale;
						D3DXVECTOR3 vPos;
						D3DXQUATERNION sQuat;
						D3DXMatrixToSQT( vScale, sQuat, vPos, pBoneFrame->TransformationMatrix );
						if ( vScale.x < 0.f )
						{
							D3DXMATRIX matScaling;
							D3DXMatrixScaling( &matScaling, -1.f, -1.f, -1.f );
							D3DXMatrixMultiply( pMeshContainer->m_ppStaticMatrixPtrs[1], pMeshContainer->m_ppStaticMatrixPtrs[1], &matScaling );
						}
					}

					//DxBoneTrans* pBoneTrans = m_skeletonRes.get()->FindBone_t( pBoneFrame->Name );
					//if ( pBoneTrans )
					//{
					//	pMeshContainer->m_ppBoneMatrixPtrs[2] = &pBoneTrans->matCombined;				//[BONE]
					//}

					pMeshContainer->m_arrayBoneIndex = new DWORD[ 1 ];
					pMeshContainer->m_arrayBoneIndex[ 0 ] = m_skeletonRes.get()->GetReferenceIndex_t( pBoneFrame->Name );

					// [shhan][2015.01.05] 원래대로라면 E_FAIL 시 렌더링을 안하게 해야 하는게 맞는데, 기존에 잘못되었지만 렌더링 되었기 때문에,
					//						클라가 종료 안되고 렌더링은 되도록 하기 위해(비정상적) 초기값 0 값을 주었다.
					//						비정상적인 위치에 렌더링 될꺼기 때문에 추후 데이터 수정을 하면 된다.
					//
					if ( pMeshContainer->m_arrayBoneIndex[ 0 ] == -1 )
					{
						// [shhan][2015.01.05] skin.x 에는 Bone 이 있지만 Bone.x 에는 그 Bone 이 없는 경우이다.
						//						경고 Msg 를 띄우는게 좋겠다.
						TSTRING strName(_T("BoneFile : "));
						strName += m_strName_BONE + _T(" , SkinFile : ") + m_strName_SKIN + _T(" , Error Bone Name : ");
						strName += pBoneFrame->Name;

						CDebugSet::ToLogFile( strName );

						GASSERT( 0 && _T("SetupBoneMatrixPointersOnMesh - 부모 Bone.x에 Bone이 없습니다. log를 참조해주세요.") );

						pMeshContainer->m_arrayBoneIndex[ 0 ] = 0;

						return E_FAIL;
					}
				}
			}
		}
	}


    return S_OK;
}

//--------------------------------------------------------------------------------------
// Called to setup the pointers for a given bone to its transformation matrix
//--------------------------------------------------------------------------------------
HRESULT DxSkinMesh9::SetupBoneMatrixPointers( LPD3DXFRAME pFrame )
{
    HRESULT hr;

    if (pFrame->pMeshContainer != NULL)
    {
        hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer,pFrame);
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
// update the frame matrices
//--------------------------------------------------------------------------------------
void DxSkinMesh9::UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
{
    SFrame *pFrame = (SFrame*)pFrameBase;

    if (pParentMatrix != NULL)
        D3DXMatrixMultiply ( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

    if (pFrame->pFrameSibling != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
    }
}

//--------------------------------------------------------------------------------------
// update the skinning method
//--------------------------------------------------------------------------------------
//void DxSkinMesh9::UpdateSkinningMethod( LPD3DXFRAME pFrameBase )
//{
//    SFrame *pFrame = (SFrame*)pFrameBase;
//    SMeshContainer *pMeshContainer;
//
//    pMeshContainer = (SMeshContainer *)pFrame->pMeshContainer;
//
//    while( pMeshContainer != NULL )
//    {
//        pMeshContainer->GenerateSkinnedMesh ( DXUTGetD3DDevice() );
//
//        pMeshContainer = (SMeshContainer *)pMeshContainer->pNextMeshContainer;
//    }
//
//    if (pFrame->pFrameSibling != NULL)
//    {
//        UpdateSkinningMethod(pFrame->pFrameSibling);
//    }
//
//    if (pFrame->pFrameFirstChild != NULL)
//    {
//        UpdateSkinningMethod(pFrame->pFrameFirstChild);
//    }
//}

HRESULT DxSkinMesh9::StaticCreate (IDirect3DDevice9* pd3dDevice )
{
//	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
//
//#ifdef DEBUG_VS
//	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
//#endif
//#ifdef DEBUG_PS
//	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
//#endif
//
//	LPD3DXBUFFER pErr = NULL;
//
//	UINT size = (UINT)strlen(pHLSL_Shadow);
//	if( FAILED(D3DXCreateEffect( pd3dDevice, pHLSL_Shadow, size, NULL, NULL, dwShaderFlags, NULL, &g_pShadowFX, &pErr ) ) )
//	{
//		CDebugSet::ToListView ( "[ERROR]D3DXCreateEffect FAILED ERROR: %s", pErr->GetBufferPointer() );
//
//#ifdef _RELEASED
//		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
//#endif
//
//		SAFE_RELEASE(pErr);
//		return E_FAIL;
//	}
//
//	size = (UINT)strlen(pHLSL_Shadow_Skin);
//	if( FAILED(D3DXCreateEffect( pd3dDevice, pHLSL_Shadow_Skin, size, NULL, NULL, dwShaderFlags, NULL, &g_pShadowSkinFX, &pErr ) ) )
//	{
//		CDebugSet::ToListView ( "[ERROR]D3DXCreateEffect FAILED ERROR: %s", pErr->GetBufferPointer() );
//
//#ifdef _RELEASED
//		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
//#endif
//
//		SAFE_RELEASE(pErr);
//		return E_FAIL;
//	}

	return S_OK;
}

HRESULT DxSkinMesh9::StaticResetDevice ( IDirect3DDevice9* pd3dDevice )
{
	//if ( g_pShadowFX )
	//{
	//	g_pShadowFX->OnResetDevice();
	//}

	//if ( g_pShadowSkinFX )
	//{
	//	g_pShadowSkinFX->OnResetDevice();
	//}

	//{
	//	const D3DVERTEXELEMENT9 VertexDeclShadow[] =
	//	{
	//		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	//		{ 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	//		{ 0, 24,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

	//		D3DDECL_END()
	//	};

	//	HRESULT hr = pd3dDevice->CreateVertexDeclaration( VertexDeclShadow,  &g_pShadowDCRT );
	//	if( FAILED(hr) )
	//	{
	//		CDebugSet::ToListView ( "[ERROR] CreateVertexDeclaration _ Reflect River FAILED" );
	//		return S_OK;
	//	}
	//}

	//{
	//	const D3DVERTEXELEMENT9 VertexDeclShadowSkin[] =
	//	{
	//		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	//		{ 0, 12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
	//		{ 0, 28, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

	//		D3DDECL_END()
	//	};

	//	HRESULT hr = pd3dDevice->CreateVertexDeclaration( VertexDeclShadowSkin,  &g_pShadowSkinDCRT );
	//	if( FAILED(hr) )
	//	{
	//		CDebugSet::ToListView ( "[ERROR] CreateVertexDeclaration _ Reflect River FAILED" );
	//		return S_OK;
	//	}
	//}

	return S_OK;
}

void DxSkinMesh9::StaticLost ()
{
	//if( g_pShadowFX )
	//	g_pShadowFX->OnLostDevice();

	//if( g_pShadowSkinFX )
	//	g_pShadowSkinFX->OnLostDevice();

	//SAFE_RELEASE( g_pShadowDCRT );
	//SAFE_RELEASE( g_pShadowSkinDCRT );
}

void DxSkinMesh9::StaticDestroy ()
{
	//SAFE_RELEASE( g_pShadowFX );
	//SAFE_RELEASE( g_pShadowSkinFX );
}

void DxSkinMesh9::SetFile( const TCHAR* szSkinFile, const TCHAR *szSkeletonFile )
{
	m_strName_SKIN		= szSkinFile;
	m_strName_BONE		= szSkeletonFile;
	m_strFullName_SKIN	= DxSkinMeshManager::GetInstance().GetPath();
	m_strFullName_SKIN	+= szSkinFile;
}

HRESULT DxSkinMesh9::OnCreateSkin_Bone( IDirect3DDevice9* pd3dDevice)//, BOOL bThread )
{
	m_skeletonRes = DxSkeletonManager::GetInstance().LoadSkeleton( m_strName_BONE, TRUE );
	if ( !m_skeletonRes.IsValid() )	return E_FAIL;

	m_dwFlagsEX |= SKIN_LOADING_END_BONE;

	return S_OK;
}

HRESULT DxSkinMesh9::SetSkin_Bone(DxSkeletonResource skeletonRes)
{
	if ( !skeletonRes.IsValid() )	return E_FAIL;

	m_skeletonRes = skeletonRes;
	m_dwFlagsEX |= SKIN_LOADING_END_BONE;
	return S_OK;
}

HRESULT DxSkinMesh9::OnCreateSkin_Memory( IDirect3DDevice9* pd3dDevice,const SP_RAW_DATA& res, UINT size)
{
	if( res.get() )
	{
		m_pSrcData.GetSPMemory() = res;
		m_nSrcDataSize = size;

		m_dwFlagsEX |= SKIN_LOADING_END_MEMORY;
		return S_OK;
	}
	else
	{
		// File System 에서 메모리를 가져온다.	
		m_pSrcData = CFileSystem::getInstance().getFile( m_strFullName_SKIN.c_str(), m_nSrcDataSize );
		if (m_pSrcData == NULL)
		{
			m_dwFlagsEX |= SKIN_LOADING_END_ERROR;
			return E_FAIL;
		}
		else
		{
			m_dwFlagsEX |= SKIN_LOADING_END_MEMORY;
			return S_OK;
		}
	}
}

HRESULT DxSkinMesh9::OnCreateSkin_Skin( IDirect3DDevice9* pd3dDevice, BOOL bThreadTEX )
{
	HRESULT hr(S_OK);
	SAFE_RELEASE( m_pAnimController );

	CAllocateHierarchy Alloc(this,m_strName_SKIN,bThreadTEX);

	V_RETURN( D3DXLoadMeshHierarchyFromXInMemory(
        m_pSrcData.get(),
        (UINT) m_nSrcDataSize,					
        D3DXMESH_MANAGED,
        pd3dDevice,
        &Alloc,
        NULL,
        &m_pFrameRoot,
        &m_pAnimController ) );

	m_pSrcData = NULL;

	// Obtain the behavior flags
    D3DDEVICE_CREATION_PARAMETERS cp;
    pd3dDevice->GetCreationParameters( &cp );
	m_dwBehaviorFlags = cp.BehaviorFlags;

    // Create Matrices
    if( DxSkinMesh9::g_NumBoneMatricesReserve > DxSkinMesh9::g_NumBoneMatricesMax )
        DxSkinMesh9::CreateBoneMatrices();

	m_dwFlagsEX |= SKIN_LOADING_END_SKIN;

	return hr;
}

HRESULT DxSkinMesh9::OnCreateSkin_Final( IDirect3DDevice9* pd3dDevice, BOOL bSleep )	// Thread
{
	HRESULT hr(S_OK);

	if ( !m_pFrameRoot )
	{
		m_dwFlagsEX |= SKIN_LOADING_END_ERROR;
		return S_FALSE;
	}

	if( FAILED( hr = SetupBoneMatrixPointers( m_pFrameRoot ) ) )
	{
		m_dwFlagsEX |= SKIN_LOADING_END_ERROR;
		return hr;
	}

	if( bSleep )
	{
		Sleep(1);
	}

	if( FAILED( hr = SetupNameOnMeshContainer( m_pFrameRoot ) ) )
	{
		m_dwFlagsEX |= SKIN_LOADING_END_ERROR;
		return hr;
	}

	if( bSleep )
	{
		Sleep(1);
	}

	m_dwFlagsEX |= SKIN_LOADING_END_FINAL;
	m_dwFlagsEX |= SKIN_COMPLETED;

	return hr;
}

BOOL DxSkinMesh9::IsLoadEnd_Memory()
{
	if( m_dwFlagsEX & SKIN_LOADING_END_MEMORY )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL DxSkinMesh9::IsLoadEnd_Final()
{
	if( m_dwFlagsEX & SKIN_LOADING_END_FINAL )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL DxSkinMesh9::IsLoadEnd_Error()
{
	if( m_dwFlagsEX & SKIN_LOADING_END_ERROR )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//void DxSkinMesh9::FileReleaseMemory()
//{
//	CFileSystem::getInstance().releaseMemory( m_strFullName_SKIN );
//	m_dwFlagsEX |= SKIN_LOADING_END_SKIN;
//}

HRESULT DxSkinMesh9::OnSkinFrameMove ( double fTime, float fElapsedTime, D3DXMATRIXA16 &matWorld )
{
    if ( m_pAnimController != NULL )
        m_pAnimController->AdvanceTime ( fElapsedTime, NULL );

    UpdateFrameMatrices ( m_pFrameRoot, &matWorld );

	return S_OK;
}

//void DxSkinMesh9::OnSkinFrameRender ( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
//{
//	DrawFrame( pd3dDevice, m_pFrameRoot );
//}

//HRESULT DxSkinMesh9::OnNewSkinnedMethod ( METHOD NewSkinningMethod )
//{
//    // If the selected skinning method is different than the current one
//    //if( m_SkinningMethod != NewSkinningMethod )
//    {
//      //  m_SkinningMethod = NewSkinningMethod;
//
//        // update the meshes to the new skinning method
//        UpdateSkinningMethod( m_pFrameRoot );
//    }
//
//	return S_OK;
//}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void DxSkinMesh9::OnSkinDestroyDevice ()
{
    if( m_pFrameRoot )
    {
	    CAllocateHierarchy Alloc(NULL,_T(""),FALSE);
        D3DXFrameDestroy( m_pFrameRoot, &Alloc );
	    m_pFrameRoot = NULL;
    }

    SAFE_RELEASE( m_pAnimController );
}

void DxSkinMesh9::OnSkinCleanUp ()
{
    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.
}

SMeshContainer* DxSkinMesh9::FindMeshContainer ( LPD3DXFRAME pFrame, const char *szName )
{
	if ( !pFrame )	return NULL;

    if ( pFrame->pMeshContainer!=NULL )
    {
		if ( !strcmp(pFrame->pMeshContainer->Name,szName) )
			return (SMeshContainer*)pFrame->pMeshContainer;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        SMeshContainer* pMeshContainer = FindMeshContainer(pFrame->pFrameSibling,szName);
        if ( pMeshContainer )	return (SMeshContainer*)pMeshContainer;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        SMeshContainer* pMeshContainer = FindMeshContainer(pFrame->pFrameFirstChild,szName);
		if ( pMeshContainer )	return (SMeshContainer*)pMeshContainer;
    }

	return NULL;
}

SFrame* DxSkinMesh9::FindFrame ( char *szName )
{
	if ( m_pFrameRoot == NULL )	return NULL;
	
	return ((SFrame*)m_pFrameRoot)->FindFrame ( szName );
}

SFrame* DxSkinMesh9::FindParentFrame ( SFrame* pFrame )
{
	if ( m_pFrameRoot == NULL || pFrame == NULL )	return NULL;
	
	return ((SFrame*)m_pFrameRoot)->FindParentFrame ( pFrame );
}

// Edit 상에서만 쓰임.
HRESULT DxSkinMesh9::CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer* pmcMesh, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	HRESULT hr(S_OK);
	if ( !pmcMesh->pSkinInfo )	
		return E_FAIL;

	hr = pmcMesh->CalculateBoundingBox ( pd3dDevice, vMax, vMin );

	return hr;
}

void DxSkinMesh9::CreateCartoonMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness )
{
	//SMeshContainer* pCur = m_pMeshContainerHead;
	//while( pCur )
	//{
	//	pCur->IsCartoon_CreateMesh( pd3dDevice, fThickness );
	//	pCur = pCur->pGlobalNext;
	//}
}

void DxSkinMesh9::SaveNORMAL( TCHAR* pName )
{
	//TSTRING strFileName = DxSkinMeshManager::GetInstance().GetPath();
	//strFileName += m_strName_SKIN;
	//if( strFileName.empty() )	return;

	//strFileName = sc::util::ChangeExtName( strFileName.c_str(), _T("enm") );

	//sc::SerialFile	SFile;

	//BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFileName.c_str() );
	//if ( !bOpened )
	//{
	//	MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
	//	return;
	//}

	//SFile << DxSkinMesh9_NORMAL::VERSION;

	//SaveNORMAL( SFile );

	//SFile.CloseFile();
}

void DxSkinMesh9::SaveHLSL( TCHAR* pName )
{
	//TSTRING strFileName = DxSkinMeshManager::GetInstance().GetPath();
	//strFileName += m_strName_SKIN;
	//if( strFileName.empty() )	return;

	//strFileName = sc::util::ChangeExtName( strFileName.c_str(), _T("ehs") );

	//sc::SerialFile	SFile;

	//BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFileName.c_str() );
	//if ( !bOpened )
	//{
	//	MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
	//	return;
	//}

	//SFile << DxSkinMesh9_HLSL::VERSION;

	//SaveHLSL( SFile );

	//SFile.CloseFile();
}

void DxSkinMesh9::SaveNormalMesh( TCHAR* pName )
{
	//TSTRING strFileName = DxSkinMeshManager::GetInstance().GetPath();
	//strFileName += m_strName_SKIN;
	//if( strFileName.empty() )	return;

	//strFileName = sc::util::ChangeExtName( strFileName.c_str(), _T("nor") );

	//sc::SerialFile	SFile;

	//BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFileName.c_str() );
	//if ( !bOpened )
	//{
	//	MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
	//	return;
	//}

	//SFile << DxSkinMesh9_HLSL::VERSION;

	//SaveNormalMesh( SFile );

	//SFile.CloseFile();
}

void DxSkinMesh9::SaveLOD_SW( TCHAR* pName )
{
	//TSTRING strFileName = DxSkinMeshManager::GetInstance().GetPath();
	//strFileName += m_strName_SKIN;
	//if( strFileName.empty() )	return;

	//strFileName = sc::util::ChangeExtName( strFileName.c_str(), _T("lod_sw") );

	//sc::SerialFile	SFile;

	//BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFileName.c_str() );
	//if ( !bOpened )
	//{
	//	MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
	//	return;
	//}

	//SFile << DxSkinMesh9_CPU::VERSION;

	//SaveLOD_SW( SFile );

	//SFile.CloseFile();
}

void DxSkinMesh9::SaveNORMAL( sc::SerialFile& SFile )
{
	//SMeshContainer* pCur = m_pMeshContainerHead;
	//DWORD dwCount(0L);
	//while( pCur )
	//{
	//	++dwCount;
	//	pCur = pCur->pGlobalNext;	
	//}
	//SFile << dwCount;

	//SFile.BeginBlock();
	//{
	//	pCur = m_pMeshContainerHead;
	//	while( pCur )
	//	{
	//		pCur->SaveNORMAL( SFile );
	//		pCur = pCur->pGlobalNext;	
	//	}
	//}
	//SFile.EndBlock();
}

void DxSkinMesh9::SaveHLSL( sc::SerialFile& SFile )
{
	//SMeshContainer* pCur = m_pMeshContainerHead;
	//DWORD dwCount(0L);
	//while( pCur )
	//{
	//	++dwCount;
	//	pCur = pCur->pGlobalNext;	
	//}
	//SFile << dwCount;

	//SFile.BeginBlock();
	//{
	//	pCur = m_pMeshContainerHead;
	//	while( pCur )
	//	{
	//		pCur->SaveHLSL( SFile );
	//		pCur = pCur->pGlobalNext;	
	//	}
	//}
	//SFile.EndBlock();
}

void DxSkinMesh9::SaveNormalMesh( sc::SerialFile& SFile )
{
	//SMeshContainer* pCur = m_pMeshContainerHead;
	//DWORD dwCount(0L);
	//while( pCur )
	//{
	//	++dwCount;
	//	pCur = pCur->pGlobalNext;	
	//}
	//SFile << dwCount;

	//SFile.BeginBlock();
	//{
	//	pCur = m_pMeshContainerHead;
	//	while( pCur )
	//	{
	//		pCur->SaveNormalMesh( SFile );
	//		pCur = pCur->pGlobalNext;	
	//	}
	//}
	//SFile.EndBlock();
}

void DxSkinMesh9::SaveLOD_SW( sc::SerialFile& SFile )
{
	//SMeshContainer* pCur = m_pMeshContainerHead;
	//DWORD dwCount(0L);
	//while( pCur )
	//{
	//	++dwCount;
	//	pCur = pCur->pGlobalNext;	
	//}
	//SFile << dwCount;

	//SFile.BeginBlock();
	//{
	//	pCur = m_pMeshContainerHead;
	//	while( pCur )
	//	{
	//		pCur->SaveLOD_SW( SFile );
	//		pCur = pCur->pGlobalNext;	
	//	}
	//}
	//SFile.EndBlock();
}

void DxSkinMesh9::LoadFile( void * param1 )
{
	throw std::exception("The method or operation is not implemented.");
}

void DxSkinMesh9::DrawMeshNORMALMAP( IDirect3DDevice9 *pd3dDevice, 
									LPD3DXMESHCONTAINER pMeshContainerBase, 
									const D3DXMATRIXA16* pBoneMatrices,
									ID3DXEffect* pEffect, 
									SMATERIAL_PIECE *pmtrlPiece, 
									BOOL bWorldIdentity/* =FALSE */ )
{
	HRESULT hr;
	SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

	//	if( !pMeshContainer->m_pNormalMesh )				return;
	//	if( !pMeshContainer->m_pNormalMesh->m_pScaleVB )	return;

	DWORD dwNumInfl(0);
	DWORD dwThisAttribId(0);
	DXMATERIALEX* pMaterialEx(NULL);
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;
	LPD3DXBONECOMBINATION pBoneCur;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp, matBoneMatrix_UP;

	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	if( m_fHeight!=0.f)	bHeight = TRUE;

	// Note : 일단 미리 해 놓는다.
	LPDIRECT3DINDEXBUFFER9	pIB;
	pMeshContainer->m_sMeshDataHLSL.m_MeshData.pMesh->GetIndexBuffer( &pIB );
	pd3dDevice->SetIndices( pIB );


	pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		D3DXMATRIX matView;
		pd3dDevice->GetTransform( D3DTS_VIEW, &matView );

		//if (pMeshContainer->UseSoftwareVP)
		//{
		//	// If hw or pure hw vertex processing is forced, we can't render the
		//	// mesh, so just exit out.  Typical applications should create
		//	// a device with appropriate vertex processing capability for this
		//	// skinning method.
		//	if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		//		return;

		//	V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
		//}

		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_sMeshDataHLSL.m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_sMeshDataHLSL.m_NumAttributeGroups; iAttrib++)
		{ 
			pBoneCur = &pBoneComb[iAttrib];
			dwThisAttribId = pBoneCur->AttribId;

			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->m_dwNumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply( &g_pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries+iPaletteEntry], 
										&pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries+iPaletteEntry], 
										&matView);
				}
			}

			V( pEffect->SetMatrixArray( "mWorldMatrixArray", &g_pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries], pMeshContainer->m_dwNumPaletteEntries) );

			V( pEffect->SetInt( "CurNumBones", pMeshContainer->m_sMeshDataHLSL.m_NumInfl -1) );		


			UINT numPasses;
			//V( pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESHADERSTATE ) );//D3DXFX_DONOTSAVESTATE ) ); 
			pEffect->Begin( &numPasses, 0 );

			for( UINT iPass = 0; iPass < numPasses; iPass++ )
			{
				V( pEffect->BeginPass( iPass ) );
				RenderUseHLSL( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece );				
				V( pEffect->EndPass() );
			}

			V( pEffect->End() );

			V( pd3dDevice->SetVertexShader(NULL) );
		}

		//// remember to reset back to hw vertex processing if software was required
		//if (pMeshContainer->UseSoftwareVP)
		//{
		//	V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
		//}
	}

	V( pd3dDevice->SetVertexShader(NULL) );
	V( pd3dDevice->SetPixelShader(NULL) );

	// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
}

void DxSkinMesh9::DrawMeshTotalShader( IDirect3DDevice9 *pd3dDevice, 
										LPD3DXMESHCONTAINER pMeshContainerBase, 
										const D3DXMATRIXA16* pBoneMatrices, 
										ID3DXEffect* pEffect, 
										SMATERIAL_PIECE *pmtrlPiece, 
										int nOverlayMatrial, 
										BOOL bWorldIdentity/* =FALSE */ )
{
	HRESULT hr;
	SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

	//	if( !pMeshContainer->m_pNormalMesh )				return;
	//	if( !pMeshContainer->m_pNormalMesh->m_pScaleVB )	return;

	DWORD dwNumInfl(0);
	DWORD dwThisAttribId(0);
	DXMATERIALEX* pMaterialEx(NULL);
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;
	LPD3DXBONECOMBINATION pBoneCur;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp, matBoneMatrix_UP;

	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	if( m_fHeight!=0.f)	bHeight = TRUE;

	// Note : 일단 미리 해 놓는다.
	LPDIRECT3DINDEXBUFFER9	pIB;
	pMeshContainer->m_sMeshDataHLSL.m_MeshData.pMesh->GetIndexBuffer( &pIB );
	pd3dDevice->SetIndices( pIB );


	pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		D3DXMATRIX matView;
		pd3dDevice->GetTransform( D3DTS_VIEW, &matView );

		//if (pMeshContainer->UseSoftwareVP)
		//{
		//	// If hw or pure hw vertex processing is forced, we can't render the
		//	// mesh, so just exit out.  Typical applications should create
		//	// a device with appropriate vertex processing capability for this
		//	// skinning method.
		//	if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		//		return;

		//	V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
		//}

		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_sMeshDataHLSL.m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_sMeshDataHLSL.m_NumAttributeGroups; iAttrib++)
		{ 
			pBoneCur = &pBoneComb[iAttrib];
			dwThisAttribId = pBoneCur->AttribId;
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->m_dwNumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply( &g_pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries+iPaletteEntry], 
										&pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries+iPaletteEntry], 
										&matView);
				}
			}

			V( pEffect->SetMatrixArray( "mWorldMatrixArray", &g_pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries], pMeshContainer->m_dwNumPaletteEntries) );

			V( pEffect->SetInt( "CurNumBones", pMeshContainer->m_sMeshDataHLSL.m_NumInfl -1) );	

			if( nOverlayMatrial == iAttrib )
			{
				pEffect->SetBool( "m_bUseOverlay", TRUE );
			}else{
				pEffect->SetBool( "m_bUseOverlay", FALSE );
			}

			UINT numPasses;
			//V( pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESHADERSTATE ) );//D3DXFX_DONOTSAVESTATE ) ); 
			pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );

			for( UINT iPass = 0; iPass < numPasses; iPass++ )
			{
				V( pEffect->BeginPass( iPass ) );				
				RenderUseHLSL( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece );				
				V( pEffect->EndPass() );
			}

			V( pEffect->End() );

			V( pd3dDevice->SetVertexShader(NULL) );
		}

		//// remember to reset back to hw vertex processing if software was required
		//if (pMeshContainer->UseSoftwareVP)
		//{
		//	V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
		//}
	}

	V( pd3dDevice->SetVertexShader(NULL) );
	V( pd3dDevice->SetPixelShader(NULL) );

	// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
}

void DxSkinMesh9::RenderUseHLSL( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, SMATERIAL_PIECE *pmtrlPiece )
{
	LPDIRECT3DTEXTUREQ	pTexture(NULL);
	if( pmtrlPiece )
	{
		if ( pmtrlPiece[dwThisAttribId].m_textureRes.GetTexture() )
			pTexture = pmtrlPiece[dwThisAttribId].m_textureRes.GetTexture();
	}
	if( !pTexture )
		pTexture = pmcMesh->m_pMaterialEx[dwThisAttribId].m_textureRes.GetTexture();

	pd3dDevice->SetTexture( 0, pTexture );

	pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->DrawSubset(ipattr);

	pd3dDevice->SetTexture ( 0, NULL );
}

void DxSkinMesh9::DrawMeshVisualMaterial( IDirect3DDevice9 *pd3dDevice, 
										 LPD3DXMESHCONTAINER pMeshContainerBase, 
										 const D3DXMATRIXA16* pBoneMatrices, 
										 DxEffChar* pEffChar,
										 const DxSkeletonMaintain* pSkeleton, 
										 TEXTURE_TYPE_FLAG emTexType,
										 const D3DXMATRIX& matStaticLinkBone )
{
	HRESULT hr;
	SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

	DWORD dwNumInfl(0);
	DWORD dwThisAttribId(0);
	DXMATERIALEX* pMaterialEx(NULL);
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;
	LPD3DXBONECOMBINATION pBoneCur;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp, matBoneMatrix_UP;

	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	if( m_fHeight!=0.f)	bHeight = TRUE;

	pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		// Note : 일단 미리 해 놓는다.
		LPDIRECT3DINDEXBUFFER9	pIB;
		pMeshContainer->m_sMeshDataHLSL.m_MeshData.pMesh->GetIndexBuffer( &pIB );
		pd3dDevice->SetIndices( pIB );

		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_sMeshDataHLSL.m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_sMeshDataHLSL.m_NumAttributeGroups; iAttrib++)
		{ 
			pBoneCur = &pBoneComb[iAttrib];
			dwThisAttribId = pBoneCur->AttribId;

			// VisualMaterial 특수하게 동작됨.
			const D3DXMATRIXA16* pBoneMatricesLocal(NULL);
			if ( pBoneMatrices )
			{
				pBoneMatricesLocal = &pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries];
			}
			else
			{
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->m_dwNumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						//D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->m_pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->m_ppBoneMatrixPtrs[iMatrixIndex]);

						D3DXMatrixMultiply
						(
							&g_pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries+iPaletteEntry], 
							&pMeshContainer->m_pBoneOffsetMatrices[iMatrixIndex],
							&pSkeleton->FindBone_Index( pMeshContainer->m_arrayBoneIndex[iMatrixIndex] )->m_matCombined
						);
					}
				}

				pBoneMatricesLocal = g_pBoneMatrices;
			}

			pEffChar->RenderVisualMaterial( pd3dDevice, pBoneMatricesLocal, pBoneCur, pMeshContainer->m_dwNumPaletteEntries, pMeshContainer->m_sMeshDataHLSL.m_NumInfl, emTexType );
		}
	}
	else  // standard mesh, just draw it after setting material properties
	{
		// Note : 일단 미리 해 놓는다.
		LPDIRECT3DINDEXBUFFER9	pIB;
		pMeshContainer->MeshData.pMesh->GetIndexBuffer( &pIB );
		pd3dDevice->SetIndices( pIB );

		//D3DXMATRIX matCombined;
		////D3DXMatrixMultiply ( &matCombined, pMeshContainer->m_ppBoneMatrixPtrs[1], pMeshContainer->m_ppBoneMatrixPtrs[2] );
		//D3DXMatrixMultiply ( &matCombined, pMeshContainer->m_ppStaticMatrixPtrs[1], &pSkeleton->FindBone_Index( pMeshContainer->m_arrayBoneIndex[0] )->m_matCombined );
		//D3DXMatrixMultiply ( &matCombined, pMeshContainer->m_ppStaticMatrixPtrs[0], &matCombined );

		for ( DWORD i=0; i < pMeshContainer->m_dwNumAttributeGroups; ++i )
		{
			pEffChar->RenderVisualMaterial( pd3dDevice, i, emTexType, matStaticLinkBone );
		}
	}

	V( pd3dDevice->SetVertexShader(NULL) );
	V( pd3dDevice->SetPixelShader(NULL) );

	// [shhan][2014.06.30] Material 작업되면서 주석처리가 되었다.
	//						반투명캐릭일 때 앞 Part 가 VM 을 쓰면 그 뒤부터는 반투명이 풀리는 문제가 있었다.
	//// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
}

//void DxSkinMesh9::DrawMeshForMaterialSystem( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, const D3DXMATRIX& matViewProj )
//{
//	HRESULT hr;
//	SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;
//
//	DWORD dwNumInfl(0);
//	DWORD dwThisAttribId(0);
//	DXMATERIALEX* pMaterialEx(NULL);
//	UINT iAttrib;
//	LPD3DXBONECOMBINATION pBoneComb;
//	LPD3DXBONECOMBINATION pBoneCur;
//
//	UINT iMatrixIndex;
//	UINT iPaletteEntry;
//	D3DXMATRIXA16 matTemp, matBoneMatrix_UP;
//
//	pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
//
//	// first check for skinning
//	if (pMeshContainer->pSkinInfo != NULL)
//	{
//		ID3DXEffect* pEffect = g_pShadowSkinFX;
//		if ( !pEffect )
//			return;
//
//		// [shhan][2013.10.18] CRM#1497 - [Map]Mob 그림자 관련 확인요청
//		//									처음엔 0.001f 로 하였다.
//		//									그런데 TnL_PS_3_0_CSM 에서 캐릭터 발쪽 그림자가 안나오는 문제가 생겨서, 10.f 했더니 잘 나왔다.
//		//									그렇지만 TnL_PS_3_0_NM 이하 에서는 그림자가 나오지 않아야될 벽쪽으로도 생성이 되었다.
//		//									일단 그래서 옵션별로 셋팅을 나눠 놓았지만 TnL_PS_3_0_CSM 쪽이 이상하긴 하다.
//		//									다른 그래픽카드에서 문제가 있다면 0.001f 으로 하는게 맞을것이다.
//		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
//		{
//			pEffect->SetFloat( "g_fDepthOffset", 10.f );
//		}
//		else
//		{
//			pEffect->SetFloat( "g_fDepthOffset", 0.001f );
//		}
//
//		if ( !pMeshContainer->m_sMeshDataShadow.m_MeshData.pMesh )
//			return;
//
//		pEffect->SetMatrix( "mViewProj", &matViewProj );
//
//		//if (pMeshContainer->UseSoftwareVP)
//		//{
//		//	// If hw or pure hw vertex processing is forced, we can't render the
//		//	// mesh, so just exit out.  Typical applications should create
//		//	// a device with appropriate vertex processing capability for this
//		//	// skinning method.
//		//	if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
//		//		return;
//
//		//	V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
//		//}
//
//		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_sMeshDataShadow.m_pBoneCombinationBuf->GetBufferPointer());
//		for (iAttrib = 0; iAttrib < pMeshContainer->m_sMeshDataShadow.m_NumAttributeGroups; iAttrib++)
//		{ 
//			pBoneCur = &pBoneComb[iAttrib];
//			dwThisAttribId = pBoneCur->AttribId;
//
//			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->m_dwNumPaletteEntries; ++iPaletteEntry)
//			{
//				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
//				if (iMatrixIndex != UINT_MAX)
//				{
//					D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->m_pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->m_ppBoneMatrixPtrs[iMatrixIndex]);
//				}
//			}
//
//			pd3dDevice->SetVertexDeclaration( g_pShadowSkinDCRT ); 
//
//			V( pEffect->SetMatrixArray( "mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->m_dwNumPaletteEntries) );
//
//			V( pEffect->SetInt( "CurNumBones", pMeshContainer->m_sMeshDataShadow.m_NumInfl -1) );
//
//			UINT numPasses;
//			pEffect->Begin( &numPasses, 0 );
//
//			for( UINT iPass = 0; iPass < numPasses; iPass++ )
//			{
//				V( pEffect->BeginPass( iPass ) );
//				pMeshContainer->m_sMeshDataShadow.m_MeshData.pMesh->DrawSubset( iAttrib );
//				V( pEffect->EndPass() );
//			}
//
//			V( pEffect->End() );
//
//			V( pd3dDevice->SetVertexShader(NULL) );
//			V( pd3dDevice->SetPixelShader(NULL) );
//		}
//
//		//// remember to reset back to hw vertex processing if software was required
//		//if (pMeshContainer->UseSoftwareVP)
//		//{
//		//	V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
//		//}
//	}
//	else  // standard mesh, just draw it after setting material properties
//	{
//		ID3DXEffect* pEffect = g_pShadowFX;
//		if ( !pEffect )
//			return;
//
//		// [shhan][2013.10.18] CRM#1497 - [Map]Mob 그림자 관련 확인요청
//		//									처음엔 0.001f 로 하였다.
//		//									그런데 TnL_PS_3_0_CSM 에서 캐릭터 발쪽 그림자가 안나오는 문제가 생겨서, 10.f 했더니 잘 나왔다.
//		//									그렇지만 TnL_PS_3_0_NM 이하 에서는 그림자가 나오지 않아야될 벽쪽으로도 생성이 되었다.
//		//									일단 그래서 옵션별로 셋팅을 나눠 놓았지만 TnL_PS_3_0_CSM 쪽이 이상하긴 하다.
//		//									다른 그래픽카드에서 문제가 있다면 0.001f 으로 하는게 맞을것이다.
//		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
//		{
//			pEffect->SetFloat( "g_fDepthOffset", 10.f );
//		}
//		else
//		{
//			pEffect->SetFloat( "g_fDepthOffset", 0.001f );
//		}
//
//		if ( !pMeshContainer->MeshData.pMesh )
//			return;
//
//		//if (pMeshContainer->UseSoftwareVP)
//		//{
//		//	// If hw or pure hw vertex processing is forced, we can't render the
//		//	// mesh, so just exit out.  Typical applications should create
//		//	// a device with appropriate vertex processing capability for this
//		//	// skinning method.
//		//	if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
//		//		return;
//
//		//	V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
//		//}
//
//		D3DXMATRIX matCombined;
//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->m_ppBoneMatrixPtrs[1], pMeshContainer->m_ppBoneMatrixPtrs[2] );
//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->m_ppBoneMatrixPtrs[0], &matCombined );
//		D3DXMatrixMultiply ( &matCombined, &matCombined, &matViewProj );
//
//		V( pEffect->SetMatrix( "mWorldViewProj", &matCombined ) );
//
//
//		pd3dDevice->SetVertexDeclaration( g_pShadowDCRT ); 
//
//		UINT numPasses;
//		pEffect->Begin( &numPasses, 0 );
//
//		for( UINT iPass = 0; iPass < numPasses; iPass++ )
//		{
//			V( pEffect->BeginPass( iPass ) );
//
//			for ( DWORD dwMaterial = 0; dwMaterial < pMeshContainer->NumMaterials; ++dwMaterial)
//			{
//				pMeshContainer->MeshData.pMesh->DrawSubset( dwMaterial );
//			}
//
//			V( pEffect->EndPass() );
//		}
//
//		V( pEffect->End() );
//
//		V( pd3dDevice->SetVertexShader(NULL) );
//		V( pd3dDevice->SetPixelShader(NULL) );
//
//		//// remember to reset back to hw vertex processing if software was required
//		//if (pMeshContainer->UseSoftwareVP)
//		//{
//		//	V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
//		//}
//	}
//
//	V( pd3dDevice->SetVertexShader(NULL) );
//	V( pd3dDevice->SetPixelShader(NULL) );
//
//	// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
//}