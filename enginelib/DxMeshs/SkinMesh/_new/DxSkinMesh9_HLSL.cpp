#include "pch.h"


#include "../../../Common/SubPath.h"
#include "../../../../SigmaCore/File/SerialFile.h"
#include "../../../../SigmaCore/Util/Util.h"
#include "../DxMaterial.h"
#include "../../../DxTools/DxMethods.h"

#include "../../../DxTools/Light/DxLightMan.h"
#include "../../../DxTools/DxRenderStates.h"
#include "../../../DxEffect/Single/DxEffSinglePropGMan.h"
#include "../../../DxEffect/Char/DxEffCharToon.h"
#include "../../../DxEffect/Char/DxEffCharSpecular2.h"

#include "../../../DxEffect/Char/DxEffCharHLSL.h"
#include "DxSkinVB_ATT.h"
#include "../DxSkinMeshManager.h"
#include "../DxSkinPieceContainer.h"
#include "DxSkinMeshContainer9.h"

#include "DxSkinMesh9_HLSL.h"

// ----------------------------------------------------------------------------
//#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD DxSkinMesh9_HLSL::VERSION = 0x0100;

char g_szShaderSource[4][30] =
{
	"skinmesh1.vsh",
	"skinmesh2.vsh",
	"skinmesh3.vsh",
	"skinmesh4.vsh"
};

ID3DXEffect*	DxSkinMesh9_HLSL::m_pFX = NULL;
ID3DXEffect*	DxSkinMesh9_HLSL::m_pEffect = NULL;	// D3DX effect interface
LPDIRECT3DVERTEXDECLARATION9	DxSkinMesh9_HLSL::m_pNormalDCRT = NULL;

extern DWORD	SKINDRAWCOUNT;
//extern BOOL		g_bCHAR_EDIT_PIECE_RENDER;

static const char* pHLSL =
{
	//
	// Skinned Mesh Effect file 
	// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
	//

	"float4 lhtDir = {0.0f, 0.0f, -1.0f, 1.0f};    //light Direction\r\n"
		"float4 lightDiffuse = {0.6f, 0.6f, 0.6f, 1.0f}; // Light Diffuse\r\n"
		"float4 MaterialAmbient : MATERIALAMBIENT = {0.1f, 0.1f, 0.1f, 1.0f};\r\n"
		"float4 MaterialDiffuse : MATERIALDIFFUSE = {0.8f, 0.8f, 0.8f, 1.0f};\r\n"

		// Matrix Pallette
		"static const int MAX_MATRICES = 26;\r\n"
		"float4x3    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;\r\n"
		"float4x4    mViewProj : VIEWPROJECTION;\r\n"

		///////////////////////////////////////////////////////
		"struct VS_INPUT\r\n"
		"{\r\n"
		"	float4  Pos             : POSITION;\r\n"
		"	float4  BlendWeights    : BLENDWEIGHT;\r\n"
		"	float4  BlendIndices    : BLENDINDICES;\r\n"
		"	float3  Normal          : NORMAL;\r\n"
		"	float3  Tex0            : TEXCOORD0;\r\n"
		"};\r\n"

		"struct VS_OUTPUT\r\n"
		"{\r\n"
		"	float4  Pos     : POSITION;\r\n"
		"	float4  Diffuse : COLOR;\r\n"
		"	float2  Tex0    : TEXCOORD0;\r\n"
		"};\r\n"


		"float3 Diffuse(float3 Normal)\r\n"
		"{\r\n"
		"	float CosTheta;\r\n"

		// N.L Clamped
		"	CosTheta = max(0.0f, dot(Normal, lhtDir.xyz));\r\n"

		// propogate scalar result to vector
		"	return (CosTheta);\r\n"
		"}\r\n"


		"VS_OUTPUT VShade(VS_INPUT i, uniform int NumBones)\r\n"
		"{\r\n"
		"	VS_OUTPUT   o;\r\n"
		"	float3      Pos = 0.0f;\r\n"
		"	float3      Normal = 0.0f;    \r\n"
		"	float       LastWeight = 0.0f;\r\n"

		// Compensate for lack of UBYTE4 on Geforce3
		"	int4 IndexVector = D3DCOLORtoUBYTE4(i.BlendIndices);\r\n"

		// cast the vectors to arrays for use in the for loop below
		"	float BlendWeightsArray[4] = (float[4])i.BlendWeights;\r\n"
		"	int   IndexArray[4]        = (int[4])IndexVector;\r\n"

		// calculate the pos/normal using the "normal" weights 
		//        and accumulate the weights to calculate the last weight
		"	for (int iBone = 0; iBone < NumBones-1; iBone++)\r\n"
		"	{\r\n"
		"		LastWeight = LastWeight + BlendWeightsArray[iBone];\r\n"

		"		Pos += mul(i.Pos, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
		"		Normal += mul(i.Normal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];\r\n"
		"	}\r\n"
		"	LastWeight = 1.0f - LastWeight; \r\n"

		// Now that we have the calculated weight, add in the final influence
		"	Pos += (mul(i.Pos, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight);\r\n"
		"	Normal += (mul(i.Normal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); \r\n"

		// transform position from world space into view and then projection space
		"	o.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);\r\n"

		// normalize normals
		"	Normal = normalize(Normal);\r\n"

		// Shade (Ambient + etc.)
		"	o.Diffuse.xyz = MaterialAmbient.xyz + Diffuse(Normal) * MaterialDiffuse.xyz;\r\n"
		"	o.Diffuse.w = 1.0f;\r\n"

		// copy the input texture coordinate through
		"	o.Tex0  = i.Tex0.xy;\r\n"

		"	return o;\r\n"
		"}\r\n"

		"int CurNumBones = 2;\r\n"
		"VertexShader vsArray[4] = { compile vs_1_1 VShade(1), \r\n"
		"compile vs_1_1 VShade(2),\r\n"
		"compile vs_1_1 VShade(3),\r\n"
		"compile vs_1_1 VShade(4)\r\n"
		"};\r\n"


		//////////////////////////////////////
		// Techniques specs follow
		//////////////////////////////////////
		"technique t0\r\n"
		"{\r\n"
		"	pass p0\r\n"
		"	{\r\n"
		"		VertexShader = (vsArray[CurNumBones]);\r\n"
		"	}\r\n"
		"}\r\n"
};

DxSkinMesh9_HLSL::DxSkinMesh9_HLSL()
{
	StringCchCopy( m_szFileName, 32, _T("") );
	StringCchCopy( m_szSkinName, 32, _T("") );
	StringCchCopy( m_szSkeleton, 32, _T("") );
}

DxSkinMesh9_HLSL::~DxSkinMesh9_HLSL()
{
}

void DxSkinMesh9_HLSL::CleanUp()
{
}

HRESULT DxSkinMesh9_HLSL::StaticCreate (IDirect3DDevice9* pd3dDevice )
{
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	LPD3DXBUFFER pErr = NULL;

	UINT size = (UINT)strlen(pHLSL);
	if( FAILED(D3DXCreateEffect( pd3dDevice, pHLSL, size, NULL, NULL, dwShaderFlags, NULL, &m_pEffect, &pErr ) ) )
	{
		CDebugSet::ToListView ( "[ERROR]D3DXCreateEffect FAILED ERROR: %s", pErr->GetBufferPointer() );

#ifdef _RELEASED
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
#endif

		SAFE_RELEASE(pErr);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT DxSkinMesh9_HLSL::StaticResetDevice( IDirect3DDevice9* pd3dDevice )
{
	HRESULT hr(S_OK);

	if( m_pEffect )
	{
		V_RETURN( m_pEffect->OnResetDevice() );
	}

	const D3DVERTEXELEMENT9 VertexDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
		{ 0, 44, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 60, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

		D3DDECL_END()
	};

	hr = pd3dDevice->CreateVertexDeclaration( VertexDecl,  &m_pNormalDCRT );
	if( FAILED(hr) )
	{
		CDebugSet::ToListView ( "[ERROR] CreateVertexDeclaration _ Reflect River FAILED" );
		return E_FAIL;
	}

	return S_OK;
}

void DxSkinMesh9_HLSL::StaticLost()
{
	if( m_pEffect )
		m_pEffect->OnLostDevice();

	SAFE_RELEASE( m_pNormalDCRT );
}

void DxSkinMesh9_HLSL::StaticDestroy ()
{
	SAFE_RELEASE( m_pEffect );

	//SAFE_DELETE_ARRAY( g_pBoneMatrices );
}
//
//void DxSkinMesh9_HLSL::DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, const CHARSETTING& sSetting, const DxSkeletonMaintain* pSkeleton, const D3DXMATRIX& matStaticLinkBone )
//{
//	//HRESULT hr;
//	//SMeshContainer *pMeshContainer = (SMeshContainer*)sSetting.pMeshContainerBase;
//
//	//DWORD dwNumInfl(0);
//	//UINT m_dwNumAttributeGroups(0);
//	//DWORD dwThisAttribId(0);
//	//DXMATERIALEX* pMaterialEx(NULL);
//	//UINT iMaterial;
//	//UINT iAttrib;
//	//LPD3DXBONECOMBINATION pBoneComb;
//
//	//UINT iMatrixIndex;
//	//UINT iPaletteEntry;
//	//D3DXMATRIXA16 matTemp, matBoneMatrix_UP;
//
//	//BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
//	//if( m_fHeight!=0.f)	bHeight = TRUE;
//
//	//// set material color properties 
//	//D3DXVECTOR4 vVector;
//	//vVector.x = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse.r;
//	//vVector.y = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse.g;
//	//vVector.z = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse.b;
//	//vVector.w = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse.a;
//	//V( m_pEffect->SetVector( "MaterialDiffuse", &vVector ) );
//
//	//vVector.x = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.r;
//	//vVector.y = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.g;
//	//vVector.z = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.b;
//	//vVector.w = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.a;
//	//V( m_pEffect->SetVector( "MaterialAmbient", &vVector ) );
//
//	//// first check for skinning
//	//if (pMeshContainer->pSkinInfo != NULL)
//	//{
//	//	D3DXMATRIX matViewProj, matView, matProj;
//	//	pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
//	//	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
//	//	D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
//	//	m_pEffect->SetMatrix( "mViewProj", &matViewProj );
//
//	//	// Set Light for vertex shader
//	//	D3DXVECTOR4 vLightDir( 0.0f, 1.0f, -1.0f, 0.0f );
//	//	D3DXVec4Normalize( &vLightDir, &vLightDir );
//	//	V( m_pEffect->SetVector( "lhtDir", &vLightDir) );
//
//	//	if (pMeshContainer->UseSoftwareVP)
//	//	{
//	//		// If hw or pure hw vertex processing is forced, we can't render the
//	//		// mesh, so just exit out.  Typical applications should create
//	//		// a device with appropriate vertex processing capability for this
//	//		// skinning method.
//	//		if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
//	//			return;
//
//	//		V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
//	//	}
//
//	//	pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
//	//	for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
//	//	{ 
//	//		if( bHeight )
//	//		{
//	//			// first calculate all the world matrices
//	//			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
//	//			{
//	//				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
//	//				if (iMatrixIndex != UINT_MAX)
//	//				{
//	//					//	char shadow map - 높이 조절.
//	//					matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
//	//					matBoneMatrix_UP._42 += m_fHeight;
//
//	//					D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
//	//					//D3DXMatrixMultiply(&m_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
//	//				}
//	//			}
//	//		}
//	//		else
//	//		{
//	//			// first calculate all the world matrices
//	//			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
//	//			{
//	//				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
//	//				if (iMatrixIndex != UINT_MAX)
//	//				{
//	//					D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
//	//				}
//	//			}
//	//		}
//	//		V( m_pEffect->SetMatrixArray( "mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->NumPaletteEntries) );
//
//	//		//// Sum of all ambient and emissive contribution
//	//		//D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
//	//		//D3DXCOLOR color2(.25, .25, .25, 1.0);
//	//		//D3DXCOLOR ambEmm;
//	//		//D3DXColorModulate(&ambEmm, &color1, &color2);
//	//		//ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);
//
//	//		// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
//	//		//		V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[pBoneComb[iAttrib].AttribId].pTexture ) );
//
//	//		// Set CurNumBones to select the correct vertex shader for the number of bones
//	//		V( m_pEffect->SetInt( "CurNumBones", pMeshContainer->NumInfl -1) );
//
//	//		// Start the effect now all parameters have been updated
//	//		UINT numPasses;
//	//		V( m_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE ) ); 
//	//		for( UINT iPass = 0; iPass < numPasses; iPass++ )
//	//		{
//	//			V( m_pEffect->BeginPass( iPass ) );
//
//	//			// draw the subset with the current world matrix palette and material state
//	//			//   V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
//
//	//			//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
//	//			//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
//	//			DWORD dwThisAttribId = pBoneComb[iAttrib].AttribId;
//	//			BOOL bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && !sSetting.pmtrlSpecular[dwThisAttribId].IsType( EMCET_NULL ) );
//	//			if ( bDraw )
//	//			{
//	//				++SKINDRAWCOUNT;
//	//				RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, sSetting.pmtrlPiece, sSetting.pmtrlSpecular );
//	//			}
//
//	//			V( m_pEffect->EndPass() );
//	//		}
//
//	//		V( m_pEffect->End() );
//
//	//		V( pd3dDevice->SetVertexShader(NULL) );
//	//	}
//
//	//	// remember to reset back to hw vertex processing if software was required
//	//	if (pMeshContainer->UseSoftwareVP)
//	//	{
//	//		V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
//	//	}
//	//}
//	//else  // standard mesh, just draw it after setting material properties
//	//{
//	//	if( g_bCHAR_EDIT_PIECE_RENDER )
//	//	{
//	//		D3DXMATRIX matIdentity;
//	//		D3DXMatrixIdentity( &matIdentity );
//	//		V( pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity) );
//	//	}
//	//	else
//	//	{
//	//		D3DXMATRIX matCombined;
//	//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
//	//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );
//
//	//		matCombined._42 += m_fHeight;
//	//		m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.
//
//	//		V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );
//	//	}
//
//	//	for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
//	//	{
//	//		DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
//	//		if( !pMaterialEx )	continue;
//
//	//		// Note : 텍스쳐가 있을 경우와 없을 경우
//	//		if( !pMaterialEx->m_textureRes.IsValid() )
//	//		{
//	//			// Note : 먼저 로드가 되었는지 살펴본다.
//	//			CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
//	//		}
//
//	//		V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
//	//		//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );
//
//	//		//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
//	//		//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
//	//		BOOL bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && !sSetting.pmtrlSpecular[iMaterial].IsType( EMCET_NULL ) );
//	//		if ( bDraw )
//	//		{
//	//			RenderDefault ( pd3dDevice, pMeshContainer, iMaterial, iMaterial, sSetting.pmtrlPiece, sSetting.pmtrlSpecular );
//	//		}
//	//	}
//	//}
//
//	//// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
//	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
//	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
//	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
//	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
//}

void DxSkinMesh9_HLSL::DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, 
										DXMATERIAL_CHAR_EFF* pmtrlSpecular, BOOL bWorldIdentity )
{
	//HRESULT hr;
	//SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

	//DWORD dwNumInfl(0);
	//UINT m_dwNumAttributeGroups(0);
	//DWORD dwThisAttribId(0);
	//DXMATERIALEX* pMaterialEx(NULL);
	//UINT iMaterial;
	//UINT iAttrib;
	//LPD3DXBONECOMBINATION pBoneComb;

	//UINT iMatrixIndex;
	//UINT iPaletteEntry;
	//D3DXMATRIXA16 matTemp, matBoneMatrix_UP;

	//BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	//if( m_fHeight!=0.f)	bHeight = TRUE;

	//// first check for skinning
	//if (pMeshContainer->pSkinInfo != NULL)
	//{
	//	D3DXMATRIX matViewProj, matView, matProj;
	//	pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	//	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	//	D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
	//	m_pFX->SetMatrix( "mViewProj", &matViewProj );

	//	if (pMeshContainer->UseSoftwareVP)
	//	{
	//		// If hw or pure hw vertex processing is forced, we can't render the
	//		// mesh, so just exit out.  Typical applications should create
	//		// a device with appropriate vertex processing capability for this
	//		// skinning method.
	//		if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
	//			return;

	//		V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
	//	}

	//	pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
	//	for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
	//	{ 
	//		if( bHeight )
	//		{
	//			// first calculate all the world matrices
	//			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
	//			{
	//				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
	//				if (iMatrixIndex != UINT_MAX)
	//				{
	//					//	char shadow map - 높이 조절.
	//					matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
	//					matBoneMatrix_UP._42 += m_fHeight;

	//					D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
	//					//D3DXMatrixMultiply(&m_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
	//				}
	//			}
	//		}
	//		else
	//		{
	//			// first calculate all the world matrices
	//			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
	//			{
	//				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
	//				if (iMatrixIndex != UINT_MAX)
	//				{
	//					D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
	//				}
	//			}
	//		}
	//		V( m_pFX->SetMatrixArray( "mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->NumPaletteEntries) );

	//		// Set CurNumBones to select the correct vertex shader for the number of bones
	//		V( m_pFX->SetInt( "CurNumBones", pMeshContainer->NumInfl -1) );

	//		// Start the effect now all parameters have been updated
	//		UINT numPasses;
	//		V( m_pFX->Begin( &numPasses, D3DXFX_DONOTSAVESTATE ) ); 
	//		for( UINT iPass = 0; iPass < numPasses; iPass++ )
	//		{
	//			V( m_pFX->BeginPass( iPass ) );

	//			//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
	//			//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
	//			DWORD dwThisAttribId = pBoneComb[iAttrib].AttribId;
	//			BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && !pmtrlSpecular[dwThisAttribId].IsType( EMCET_NULL ) );
	//			if ( bDraw )
	//			{
	//				++SKINDRAWCOUNT;
	//				RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
	//			}

	//			V( m_pFX->EndPass() );
	//		}

	//		V( m_pFX->End() );

	//		V( pd3dDevice->SetVertexShader(NULL) );
	//	}

	//	// remember to reset back to hw vertex processing if software was required
	//	if (pMeshContainer->UseSoftwareVP)
	//	{
	//		V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
	//	}
	//}
	//else  // standard mesh, just draw it after setting material properties
	//{
	//	if( bWorldIdentity )
	//	{
	//		D3DXMATRIX matIdentity;
	//		D3DXMatrixIdentity( &matIdentity );
	//		V( pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity) );
	//	}
	//	else
	//	{
	//		D3DXMATRIX matCombined;
	//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
	//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );

	//		matCombined._42 += m_fHeight;
	//		m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.

	//		V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );
	//	}

	//	for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
	//	{
	//		DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
	//		if( !pMaterialEx )	continue;

	//		// Note : 텍스쳐가 있을 경우와 없을 경우
	//		if( !pMaterialEx->m_textureRes.IsValid() )
	//		{
	//			// Note : 먼저 로드가 되었는지 살펴본다.
	//			CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
	//		}

	//		V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );

	//		//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
	//		//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
	//		BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && !pmtrlSpecular[iMaterial].IsType( EMCET_NULL ) );
	//		if ( bDraw )
	//		{
	//			RenderDefault ( pd3dDevice, pMeshContainer, iMaterial, iMaterial, pmtrlPiece, pmtrlSpecular );
	//		}
	//	}
	//}

	//// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
}

//void DxSkinMesh9_HLSL::DrawMeshCartoon( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, RENDER_OPT emCTOP )
//{
//	HRESULT hr;
//	SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;
//
//	if( !pMeshContainer->m_pEdgeMeshHLSL )				return;
//	if( !pMeshContainer->m_pEdgeMeshHLSL->m_pScaleVB )	return;
//
//	DWORD dwNumInfl(0);
//	UINT m_dwNumAttributeGroups(0);
//	DWORD dwThisAttribId(0);
//	DXMATERIALEX* pMaterialEx(NULL);
//	UINT iMaterial;
//	UINT iAttrib;
//	LPD3DXBONECOMBINATION pBoneComb;
//
//	UINT iMatrixIndex;
//	UINT iPaletteEntry;
//	D3DXMATRIXA16 matTemp, matBoneMatrix_UP;
//
//	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
//	if( m_fHeight!=0.f)	bHeight = TRUE;
//
//	// Note : 일단 미리 해 놓는다.
//	LPDIRECT3DINDEXBUFFER9	pIB;
//	pMeshContainer->MeshData.pMesh->GetIndexBuffer( &pIB );
//	pd3dDevice->SetIndices( pIB );
//
//
//	// first check for skinning
//	if (pMeshContainer->pSkinInfo != NULL)
//	{
//		D3DXMATRIX matViewProj, matView, matProj;
//		pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
//		pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
//		D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
//		m_pFX->SetMatrix( "mViewProj", &matViewProj );
//
//		if (pMeshContainer->UseSoftwareVP)
//		{
//			// If hw or pure hw vertex processing is forced, we can't render the
//			// mesh, so just exit out.  Typical applications should create
//			// a device with appropriate vertex processing capability for this
//			// skinning method.
//			if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
//				return;
//
//			V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
//		}
//
//		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
//		for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
//		{ 
//			if( bHeight )
//			{
//				// first calculate all the world matrices
//				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
//				{
//					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
//					if (iMatrixIndex != UINT_MAX)
//					{
//						//	char shadow map - 높이 조절.
//						matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
//						matBoneMatrix_UP._42 += m_fHeight;
//
//						D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
//					}
//				}
//			}
//			else
//			{
//				// first calculate all the world matrices
//				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
//				{
//					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
//					if (iMatrixIndex != UINT_MAX)
//					{
//						D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
//					}
//				}
//			}
//			V( m_pFX->SetMatrixArray( "mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->NumPaletteEntries) );
//
//			// Set CurNumBones to select the correct vertex shader for the number of bones
//			V( m_pFX->SetInt( "CurNumBones", pMeshContainer->NumInfl -1) );
//
//			// Start the effect now all parameters have been updated
//			UINT numPasses;
//			V( m_pFX->Begin( &numPasses, D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESHADERSTATE ) );//D3DXFX_DONOTSAVESTATE ) ); 
//			for( UINT iPass = 0; iPass < numPasses; iPass++ )
//			{
//				V( m_pFX->BeginPass( iPass ) );
//
//				//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
//				//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
//				DWORD dwThisAttribId = pBoneComb[iAttrib].AttribId;
//
//				switch( emCTOP )
//				{
//				case CTOP_SHADOW:
//					RenderCartoon( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece );
//					break;
//				case CTOP_EDGE:
//					RenderEdge( pd3dDevice, pMeshContainer, iAttrib );
//					break;
//				case CTOP_NEWTEX:
//					break;
//				}
//
//				V( m_pFX->EndPass() );
//			}
//
//			V( m_pFX->End() );
//		}
//
//		// remember to reset back to hw vertex processing if software was required
//		if (pMeshContainer->UseSoftwareVP)
//		{
//			V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
//		}
//	}
//	else  // standard mesh, just draw it after setting material properties
//	{
//		D3DXMATRIX matCombined;
//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );
//
//		matCombined._42 += m_fHeight;
//		m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.
//
//		V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );
//
//		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
//		{
//			DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
//			if( !pMaterialEx )	continue;
//
//			// Note : 텍스쳐가 있을 경우와 없을 경우
//			if( !pMaterialEx->m_textureRes.IsValid() )
//			{
//				// Note : 먼저 로드가 되었는지 살펴본다.
//				CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
//			}
//
//			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
//			//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );
//
//			switch( emCTOP )
//			{
//			case CTOP_SHADOW:
//				RenderCartoon( pd3dDevice, pMeshContainer, iMaterial, iMaterial, pmtrlPiece );
//				break;
//			case CTOP_EDGE:
//				RenderEdge( pd3dDevice, pMeshContainer, iMaterial );
//				break;
//			case CTOP_NEWTEX:
//				break;
//			}
//		}
//	}
//
//	switch( emCTOP )
//	{
//	case CTOP_SHADOW:
//		V( pd3dDevice->SetVertexShader(NULL) );
//		V( pd3dDevice->SetPixelShader(NULL) );
//		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );	// 값을 되돌린다.
//		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );	// 값을 되돌린다.
//		break;
//
//	case CTOP_EDGE:
//		V( pd3dDevice->SetVertexShader(NULL) );
//		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_CCW );					// 값을 되돌린다.
//		break;
//
//	case CTOP_NEWTEX:
//		break;
//	}
//}
//
//void DxSkinMesh9_HLSL::RenderCartoon( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
//									 SMATERIAL_PIECE *pmtrlPiece )
//{
//	LPDIRECT3DTEXTUREQ	pTexture(NULL);
//	if( pmtrlPiece )
//	{
//		if ( pmtrlPiece[dwThisAttribId].m_textureRes.GetTexture() )
//            pTexture = pmtrlPiece[dwThisAttribId].m_textureRes.GetTexture();
//	}
//	if( !pTexture )
//        pTexture = pmcMesh->pMaterialEx[dwThisAttribId].m_textureRes.GetTexture();
//
//	pd3dDevice->SetTexture( 0, pTexture );
//
//	pmcMesh->MeshData.pMesh->DrawSubset( ipattr );
//}
//
//void DxSkinMesh9_HLSL::RenderEdge( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr )
//{
//	//pd3dDevice->SetFVF( pmcMesh->m_pEdgeMeshHLSL->m_dwScaleFVF );
//	pd3dDevice->SetStreamSource( 0, pmcMesh->m_pEdgeMeshHLSL->m_pScaleVB, 0, pmcMesh->m_pEdgeMeshHLSL->m_dwScaleSizeFVF );
//
//	const D3DXATTRIBUTERANGE* pAttribTable = pmcMesh->m_pEdgeMeshHLSL->m_pAttribTable;
//	const D3DXATTRIBUTERANGE& sAttribTable = pAttribTable[ pAttribTable[ipattr].AttribId ];
//
//	pd3dDevice->DrawIndexedPrimitive
//		(
//		D3DPT_TRIANGLELIST, 
//		0, 
//		sAttribTable.VertexStart, 
//		sAttribTable.VertexCount, 
//		sAttribTable.FaceStart*3, 
//		sAttribTable.FaceCount 
//		);
//}


void DxSkinMesh9_HLSL::Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice )
{
	CleanUp();

	sc::SerialFile	SFile;
	std::string strName = DxSkinMeshManager::GetInstance().GetPath();
	strName += pName;

	strName = sc::util::ChangeExtName( strName.c_str(), _T("ehs") );

	BOOL bOpened = SFile.OpenFile( FOT_READ, strName.c_str() );
	if( bOpened )
	{
		DWORD dwVer;
		SFile >> dwVer;

		Load( SFile, pd3dDevice );

		SFile.CloseFile();
	}



	strName = DxSkinMeshManager::GetInstance().GetPath();
	strName += pName;

	strName = sc::util::ChangeExtName( strName.c_str(), _T("nor") );

	bOpened = SFile.OpenFile( FOT_READ, strName.c_str() );
	if( bOpened )
	{
		DWORD dwVer;
		SFile >> dwVer;

		LoadNormalMap( SFile, pd3dDevice );

		SFile.CloseFile();
	}
}

void DxSkinMesh9_HLSL::Load( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice )
{
	DWORD dwCount(0L);
	SMeshContainer* pCur = m_pMeshContainerHead;
	while( pCur )
	{
		++dwCount;
		pCur = pCur->m_pGlobalNext;	
	}

	DWORD dwCountLOAD(0L);
	DWORD dwBufferSize(0L);
	SFile >> dwCountLOAD;
	SFile >> dwBufferSize;
	if( dwCount != dwCountLOAD )
	{
		SFile.SetOffSet( SFile.GetfTell()+dwBufferSize );
		return;
	}

	pCur = m_pMeshContainerHead;
	while( pCur )
	{
		pCur->LoadHLSL( pd3dDevice, SFile );
		pCur = pCur->m_pGlobalNext;	
	}
}

void DxSkinMesh9_HLSL::LoadNormalMap( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice )
{
	DWORD dwCount(0L);
	SMeshContainer* pCur = m_pMeshContainerHead;
	while( pCur )
	{
		++dwCount;
		pCur = pCur->m_pGlobalNext;	
	}

	DWORD dwCountLOAD(0L);
	DWORD dwBufferSize(0L);
	SFile >> dwCountLOAD;
	SFile >> dwBufferSize;
	if( dwCount != dwCountLOAD )
	{
		SFile.SetOffSet( SFile.GetfTell()+dwBufferSize );
		return;
	}

	pCur = m_pMeshContainerHead;
	while( pCur )
	{
		pCur->LoadNormalMesh( pd3dDevice, SFile );
		pCur = pCur->m_pGlobalNext;	
	}
}