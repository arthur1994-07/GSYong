#include "pch.h"

#include "../../../../RanThirdParty/PhysX_3_2/include/cloth/PxCloth.h"

#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_Material.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMeshContainer9.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/DxSkinDefine.h"
#include "../../PhysX/NSPhysX.h"

#include "../Material/NSMaterialManager.h"
#include "../CubeMap/NSCubeMapUtil.h"
#include "../Light/DxLightMan.h"
#include "../DxSurfaceTex.h"
#include "../RENDERPARAM.h"
#include "../DxSurfaceTex.h"
#include "../DxShadowMap.h"
#include "../DxViewPort.h"
#include "../Collision.h"

#include "./NSMaterialSkinManager.h"
#include "./MaterialSkinDef.h"

#include "MaterialSkinMesh.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


MaterialSkinMeshForCloth::MaterialSkinMeshForCloth()
	: m_rCloth(NULL)
	, m_pClothMesh(NULL)
{

}

MaterialSkinMeshForCloth::~MaterialSkinMeshForCloth()
{
	CleanUp();
}

void MaterialSkinMeshForCloth::CleanUp()
{
	SAFE_RELEASE ( m_pClothMesh );

	// CharEdit 에서 셋팅하면서 이전에 만들었을 수도 있기 때문에 삭제를 한다.
	NSPhysX::DeleteCloth( &m_rCloth );
}

BOOL MaterialSkinMeshForCloth::IsValid() const
{ 
	return m_rCloth ? TRUE : FALSE; 
}

BOOL MaterialSkinMeshForCloth::CheckVersionCreateCloth( LPDIRECT3DDEVICEQ pd3dDevice, 
														const D3DXMATRIX* arrayBoneMatrices,
														DWORD dwNumInfl,
														DWORD dwNumPaletteEntries,
														const SMATERIAL_PIECE& sMaterialSystem,
														const MaterialSmall& sMaterialSmall,
														const float* arrayInvWeight,
														const D3DXMATRIX* pmatAttParentBone,
														const DxClothColl* pClothColl,
														const D3DVERTEXELEMENT9* pELMT,
														const LPD3DXMESH pMesh,
														std::vector<LockParticle>& vecLockParticle,
														float fAttParentBoneScale )
{
	if ( !arrayInvWeight )
	{
		AfxMessageBox( _T("Cloth 가 될 수 없는 Mesh 입니다. Diffuse 로 가중치 셋팅이 필요합니다.") );
		return FALSE;
	}

	// AttachBone 이 아니면 이건 NULL이 들어옴
	if ( !pmatAttParentBone )
		return FALSE;

	// PhysX 와 연결하는 작업을 한다.
	NSPhysX::DeleteCloth( &m_rCloth );
	m_rCloth = NSPhysX::CreateCloth( pMesh, 
									arrayBoneMatrices, 
									dwNumInfl, 
									dwNumPaletteEntries, 
									sMaterialSystem,
									sMaterialSmall.m_pELMT, 
									fAttParentBoneScale, 
									*pmatAttParentBone, 
									arrayInvWeight, 
									vecLockParticle, 
									pClothColl );
	if ( !m_rCloth )
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	// VB write가 속도가 빠른 Mesh 를 만든다.
	DWORD _D3DXMESHOPT_FULL = 0x7f000000;
	DWORD dwOption = pMesh->GetOptions() & _D3DXMESHOPT_FULL;
	dwOption |= D3DXMESH_VB_SYSTEMMEM;
	dwOption |= D3DXMESH_VB_WRITEONLY;
	dwOption |= D3DXMESH_VB_DYNAMIC;
	dwOption |= D3DXMESH_IB_MANAGED;

	// Mesh 생성.
	HRESULT hr = pMesh->CloneMesh( dwOption, pELMT, pd3dDevice, &m_pClothMesh );
	if( FAILED(hr) )
	{
		CleanUp();
		return FALSE;
	}

	return TRUE;
}




void MaterialSkinMeshForCloth::UpdateCloth( const MATERIAL_SKIN_DATA& sMaterialData,
											DWORD dwNormalOffset,
											const std::vector<WORD>& vecClothIB,
											const D3DXMATRIX* pmatAttParentBone,
											const float* arrayInvWeight,
											const std::vector<LockParticle>& vecLockParticle,
											const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres )
{
	if ( !m_rCloth )
		return;

	NSPhysX::UpdateCloth
	( 
		m_pClothMesh, 
		sMaterialData.m_rMaterial->m_sMaterialObject.m_pELMT,
		dwNormalOffset,
		&vecClothIB[0], 
		m_rCloth, 
		*pmatAttParentBone,
		arrayInvWeight,
		vecLockParticle,
		pvecClothCollSpheres
	);
}


//------------------------------------------------------------------------------
//							MaterialSkinMesh
MaterialSkinMesh::MaterialSkinMesh()
	: m_dwVer(-1)
	, m_pMesh(NULL)
	, m_rMaterialEX(NULL)
	, m_fTexDiffColor(1.f)
	, m_rTexUpDownTexture(NULL)
	, m_dwNormalOffset(0)
{
}

MaterialSkinMesh::~MaterialSkinMesh()
{
	SAFE_RELEASE ( m_pMesh );
	m_rMaterialEX = NULL;
}

BOOL MaterialSkinMesh::Insert( LPDIRECT3DDEVICEQ pd3dDevice, 
							  const LPD3DXMESH pMeshSRC, 
							  DWORD dwVertexSize,
							  LPD3DVERTEXELEMENT9 pDecl,
							  DXMATERIALEX& sMaterialEX )
{
	//////////////////////////////////////////////////////////////////////////
	// 초기 버젼을 셋팅한다.
	m_dwVer = 0L;


	//////////////////////////////////////////////////////////////////////////
	// CloneMeshFVF
	SAFE_RELEASE ( m_pMesh );
	HRESULT hr = pMeshSRC->CloneMeshFVF( pMeshSRC->GetOptions(), pMeshSRC->GetFVF(), pd3dDevice, &m_pMesh );
	if ( hr != S_OK)
		return FALSE;


	////////////////////////////////////////////////////////////////////////////
	//// VB 생성
	//BYTE*	pVerticesSRC;
	//BYTE*	pVerticesDEST;
	//pMeshSRC->LockVertexBuffer ( 0L, (VOID**)&pVerticesSRC );
	//m_pMesh->LockVertexBuffer ( 0L, (VOID**)&pVerticesDEST );
	//memcpy( pVerticesDEST, pVerticesSRC, dwVertexSize * pMeshSRC->GetNumVertices() );
	//m_pMesh->UnlockVertexBuffer();
	//pMeshSRC->UnlockVertexBuffer();


	//////////////////////////////////////////////////////////////////////////
	// SkinMesh를 HLSL로 렌더링한다면, CloneMeshFVF 후에 꼭 작업해 줘야하는 것.
	// 원 소스는 SMeshContainer::GenerateSkinnedMeshMaterial 쪽에서 참조해보면 된다.
	{
		hr = m_pMesh->UpdateSemantics( pDecl );
		if (FAILED(hr))
		{
			SAFE_RELEASE ( m_pMesh );
			return FALSE;
		}
	}

	m_rMaterialEX = &sMaterialEX;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// int nRenderMode,
//////////////////////////////////////////////////////////////////////////
// 0-Default, 
// 1-CubeMap
// 2-Multi
// 3-TexUpDown
// 4-CubeMap+Multi
// 5-CubeMap+TexUpDown
// 6-Multi+TexUpDown
// 7-CubeMap+Multi+TexUpDown
//
// 8-Level, 
// 9-Level+CubeMap
// 10-Level+Multi
// 11-Level+TexUpDown
// 12-Level+CubeMap+Multi
// 13-Level+CubeMap+TexUpDown
// 14-Level+Multi+TexUpDown
// 15-Level+CubeMap+Multi+TexUpDown
//
// 16-Level+Flow, 
// 17-Level+Flow+CubeMap
// 18-Level+Flow+Multi
// 19-Level+Flow+TexUpDown
// 20-Level+Flow+CubeMap+Multi
// 21-Level+Flow+CubeMap+TexUpDown
// 22-Level+Flow+Multi+TexUpDown
// 23-Level+Flow+CubeMap+Multi+TexUpDown
//////////////////////////////////////////////////////////////////////////

void MaterialSkinMesh::SetMaterialSetting( LPDIRECT3DDEVICEQ pd3dDevice,
										  const EffectMaterialData* pEffectMaterialData, 
										  DWORD dwThisAttribId )
{
	int nRenderMode(0);

	// 참조값이기 때문에 이전 값이 들어있을 수도 있고, 삭제가 진행된 후 일 수도 있다.
	m_rTexUpDownTexture = NULL; 
	if ( pEffectMaterialData->m_bCubeBlend && pEffectMaterialData->m_vecEnableCubeBlend[dwThisAttribId] )
	{
		if ( pEffectMaterialData->m_bMultiTex && pEffectMaterialData->m_vecEnableMultiTex[dwThisAttribId] )
		{
			if ( pEffectMaterialData->m_bTexDiffColor && pEffectMaterialData->m_vecTexUpDownTexture[dwThisAttribId] )
			{
				nRenderMode = 7;
				m_fTexDiffColor = pEffectMaterialData->m_vecTexDiffColor[dwThisAttribId];
				m_rTexUpDownTexture = pEffectMaterialData->m_vecTexUpDownTexture[dwThisAttribId];
			}
			else
			{
				nRenderMode = 4;
			}
		}
		else if ( pEffectMaterialData->m_bTexDiffColor && pEffectMaterialData->m_vecTexUpDownTexture[dwThisAttribId] )
		{
			nRenderMode = 5;
			m_fTexDiffColor = pEffectMaterialData->m_vecTexDiffColor[dwThisAttribId];
			m_rTexUpDownTexture = pEffectMaterialData->m_vecTexUpDownTexture[dwThisAttribId];
		}
		else
		{
			nRenderMode = 1;
		}
	}
	else if ( pEffectMaterialData->m_bMultiTex && pEffectMaterialData->m_vecEnableMultiTex[dwThisAttribId] )
	{
		if ( pEffectMaterialData->m_bTexDiffColor && pEffectMaterialData->m_vecTexUpDownTexture[dwThisAttribId] )
		{
			nRenderMode = 6;
			m_fTexDiffColor = pEffectMaterialData->m_vecTexDiffColor[dwThisAttribId];
			m_rTexUpDownTexture = pEffectMaterialData->m_vecTexUpDownTexture[dwThisAttribId];
		}
		else
		{
			nRenderMode = 2;
		}
	}
	else if ( pEffectMaterialData->m_bTexDiffColor && pEffectMaterialData->m_vecTexUpDownTexture[dwThisAttribId] )
	{
		nRenderMode = 3;
		m_fTexDiffColor = pEffectMaterialData->m_vecTexDiffColor[dwThisAttribId];
		m_rTexUpDownTexture = pEffectMaterialData->m_vecTexUpDownTexture[dwThisAttribId];
	}

	if ( pEffectMaterialData->m_rLevelData )
	{
		if ( pEffectMaterialData->m_rLevelData->m_bLevel )
		{
			nRenderMode += 8;

			if ( pEffectMaterialData->m_rLevelData->m_bLevelFlow )
			{
				nRenderMode += 8;
			}
		}
	}

	BOOL bNormalMap(FALSE);
	if ( dwThisAttribId < pEffectMaterialData->m_vecEnableNormalMap.size() )
	{
		bNormalMap = pEffectMaterialData->m_vecEnableNormalMap[dwThisAttribId];
	}

	m_sMaterialData.Init();

	if ( pEffectMaterialData->m_nEnableHairIndex == dwThisAttribId )
	{
		NSMaterialSkinManager::Active_MATERIAL_DATA( pd3dDevice, _T(""), m_sMaterialData, FALSE, nRenderMode, bNormalMap, 4 );
	}
	else
	{
		NSMaterialSkinManager::Active_MATERIAL_DATA( pd3dDevice, _T(""), m_sMaterialData, FALSE, nRenderMode, bNormalMap, 0 );
	}
}

void MaterialSkinMesh::SetMaterialSettingGhosting( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sMaterialData.Init();
	NSMaterialSkinManager::Active_MATERIAL_DATA( pd3dDevice, _T(""), m_sMaterialData, FALSE, 0, FALSE, 5 );
}

void MaterialSkinMesh::SetMaterialSettingGlow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sMaterialData.Init();
	NSMaterialSkinManager::Active_MATERIAL_DATA( pd3dDevice, _T(""), m_sMaterialData, FALSE, 0, FALSE, 1 );
}

void MaterialSkinMesh::SetMaterialSettingShadow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	int nRenderType(2);
//	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM || RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
//		nRenderType = 3;

	m_sMaterialData.Init();
	NSMaterialSkinManager::Active_MATERIAL_DATA( pd3dDevice, _T(""), m_sMaterialData, FALSE, 0, FALSE, nRenderType );
}


//////////////////////////////////////////////////////////////////////////
//
BOOL MaterialSkinMesh::CheckVersion_UpdateCloth( LPDIRECT3DDEVICEQ pd3dDevice, 
												const D3DXMATRIX* arrayBoneMatrices,
												DWORD dwNumInfl,
												DWORD dwNumPaletteEntries,
												const SMATERIAL_PIECE& sMaterialSystem,
												const MaterialSmall& sMaterialSmall,
												const float* arrayInvWeight,
												const D3DXMATRIX* pmatAttParentBone,
												const DxClothColl* pClothColl,				// 고정되는 위치. 및 BaseBone 정보.
												const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
												MaterialSkinMeshForCloth* pMeshForCloth,
												float fAttParentBoneScale )
{
	if ( !m_pMesh )
		return FALSE;

	if ( m_dwVer != sMaterialSmall.m_dwVer )
	{
		m_dwVer = sMaterialSmall.m_dwVer;

		if ( !NSMATERIALSKINMESH::CreateMesh( pd3dDevice, &sMaterialSmall, m_pMesh ) )
			return FALSE;
	}

	if ( !pMeshForCloth )
		return TRUE;

	//////////////////////////////////////////////////////////////////////////
	// 천 시뮬레이션일 경우 새로 만들도록 한다.
	// (sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH) && pClothColl 값이 있으면 Cloth 동작이다.
	// 아니면 Cloth 동작 못한다.
	if ( (sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH) && pClothColl )
	{
		// PhysX Cloth 가 없을 때만 새로 생성한다.
		if ( !pMeshForCloth->IsValid() )
		{
			if ( !pMeshForCloth->CheckVersionCreateCloth( pd3dDevice, 
				arrayBoneMatrices,
				dwNumInfl,
				dwNumPaletteEntries,
				sMaterialSystem,
				sMaterialSmall,
				arrayInvWeight,
				pmatAttParentBone,
				pClothColl,
				m_sMaterialData.m_rMaterial->m_sMaterialObject.m_pELMT,
				m_pMesh,
				m_vecLockParticle,
				fAttParentBoneScale ) )
			{
				pMeshForCloth->CleanUp();
				return FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			// IB 를 다른 Buffer 에 복사해놓는다. Lock 안하기 위함.
			m_vecClothIB.resize( m_pMesh->GetNumFaces()*3 );

			BYTE* pBufferIB(NULL);
			m_pMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pBufferIB );
			memcpy( &m_vecClothIB[0], pBufferIB, sizeof(WORD)*m_pMesh->GetNumFaces()*3 );
			m_pMesh->UnlockIndexBuffer();


			//////////////////////////////////////////////////////////////////////////
			// Normal Offset 을 미리 구해놓는다.
			const D3DVERTEXELEMENT9 *pDecl = m_sMaterialData.m_rMaterial->m_sMaterialObject.m_pELMT;
			if ( pDecl )
			{
				while ( pDecl->Stream != 0xff )
				{
					if ( pDecl->Usage == D3DDECLUSAGE_NORMAL )
					{
						m_dwNormalOffset = pDecl->Offset;
						break;
					}

					++pDecl;
				}
			}
		}
	}
	else
	{
		// CharEdit 에서 셋팅하면서 이전에 만들었을 수도 있기 때문에 삭제를 한다.
		pMeshForCloth->CleanUp();
		return TRUE;
	}

	// UpdateCloth
	{
		pMeshForCloth->UpdateCloth
		( 
			m_sMaterialData,
			m_dwNormalOffset,
			m_vecClothIB, 
			pmatAttParentBone,
			arrayInvWeight,
			m_vecLockParticle,
			pvecClothCollSpheres
		);
	}

	return TRUE;
}

BOOL MaterialSkinMesh::CheckVersionSkin_UpdateCloth( LPDIRECT3DDEVICEQ pd3dDevice, 
													const D3DXMATRIX* arrayBoneMatrices,
													DWORD dwNumInfl,
													DWORD dwNumPaletteEntries,
													const SMATERIAL_PIECE& sMaterialSystem,
													const float* arrayInvWeight,
													const D3DXMATRIX* pmatAttParentBone,
													const DxClothColl* pClothColl,
													const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
													MaterialSkinMeshForCloth* pMeshForCloth,
													float fAttParentBoneScale )

{
	return CheckVersion_UpdateCloth( pd3dDevice, 
									arrayBoneMatrices,
									dwNumInfl,
									dwNumPaletteEntries,
									sMaterialSystem,
									m_sMaterialData.m_rMaterial->m_sMaterialSkin,
									arrayInvWeight,
									pmatAttParentBone,
									pClothColl,
									pvecClothCollSpheres,
									pMeshForCloth,
									fAttParentBoneScale );
}

BOOL MaterialSkinMesh::CheckVersionObject_UpdateCloth( LPDIRECT3DDEVICEQ pd3dDevice, 
													  const SMATERIAL_PIECE& sMaterialSystem,
													  const float* arrayInvWeight,
													  const D3DXMATRIX* pmatAttParentBone,
													  const DxClothColl* pClothColl,
													  const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
													  MaterialSkinMeshForCloth* pMeshForCloth,
													  float fAttParentBoneScale )
{
	return CheckVersion_UpdateCloth( pd3dDevice, 
									NULL,
									0,
									0,
									sMaterialSystem,
									m_sMaterialData.m_rMaterial->m_sMaterialObject,
									arrayInvWeight,
									pmatAttParentBone,
									pClothColl,
									pvecClothCollSpheres,
									pMeshForCloth,
									fAttParentBoneScale );
}



//////////////////////////////////////////////////////////////////////////
//
BOOL MaterialSkinMesh::Render( LPDIRECT3DDEVICEQ pd3dDevice, 
								const D3DXMATRIX* arrayBoneMatrices,
								SMeshContainer *pMeshContainer,
								//DWORD dwNumPaletteEntries,
								//DWORD dwNumInfl, 
								SMATERIAL_PIECE& sMaterialSystem,
								const EffectMaterialData* pEffectMaterialData,
								const SKIN_SPEC_SSS_INFO_REFERENCE* pSkinInfo,
								LPDIRECT3DTEXTUREQ pOverlayTex,
								float fAlpha,
								const MaterialSkinMeshForCloth* pMeshForCloth )
{
	if ( !m_pMesh )
		return FALSE;

	ID3DXEffect* pFX = SetCommonFX( pd3dDevice, sMaterialSystem, pEffectMaterialData );
	if ( !pFX )
		return FALSE;

	DWORD dwNumInfl = pMeshContainer->m_dwNumInfl;
	DWORD dwNumPaletteEntries = pMeshContainer->m_dwNumPaletteEntries;

	pFX->SetFloat( "g_fAlpha", fAlpha );

	if ( pOverlayTex )
	{
		pFX->SetTexture( "g_BaseTexture", pOverlayTex );
	}

	if ( pSkinInfo )
	{
		pFX->SetVector ( "g_vSSS_Color", pSkinInfo->m_rSSS_Color );
		pFX->SetVector ( "g_vSpecColor_Power", pSkinInfo->m_rSpecColor_Power );

		// 사용안함.
		//pFX->SetVector ( "g_vSpec2Color_Power", pSkinInfo->m_rSpec2Color_Power );
	}

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
	{
		// [shhan][2014.08.06] 씨름부원 폴리곤 보이는 현상 때문에 0.0012f 보다 수치를 내릴수가 없다.
		//D3DXVECTOR4 vShadow( 1.f/DxSurfaceTex::CASCADESHADOWSIZE.x, 0.5f, 0.f, 0.f );	// x-SIZE(해상도)
		D3DXVECTOR4 vShadow( 1.f/DxSurfaceTex::CASCADESHADOWSIZE.x, 0.0012f, 0.f, 0.f );	// x-SIZE(해상도)
		pFX->SetVector( "g_vShadow", &vShadow );
		pFX->SetTexture( "g_ShadowTexture_PCF",	DxSurfaceTex::GetInstance().m_pShadowTexR32F_CSM );
	}
	else if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
	{
		// [shhan][2014.08.06] 씨름부원 폴리곤 보이는 현상 때문에 0.0012f 보다 수치를 내릴수가 없다.
		D3DXVECTOR4 vShadow( 1.f/DxSurfaceTex::CASCADESHADOWSIZE.z, 0.0012f, 0.f, 0.f );	// x-SIZE(해상도)
		pFX->SetVector( "g_vShadow", &vShadow );
		pFX->SetTexture( "g_ShadowTexture_PCF",	DxSurfaceTex::GetInstance().m_pShadowTexR32F );
	}

	pFX->SetVector( "g_vLightPosWorldSpace", &NSMaterialManager::g_vLightPosWorldSpace );

	D3DXMATRIX matLightViewProjClip2Tex;
	DxShadowMap::GetInstance().GetMultiply_Clip2Tex( matLightViewProjClip2Tex, DxShadowMap::GetInstance().GetMatrix_D() );
	pFX->SetMatrixTranspose( "g_matShadow_Clip2Tex",	&matLightViewProjClip2Tex );

	pFX->SetMatrixArray( "g_matWorldMatrixArray", arrayBoneMatrices, dwNumPaletteEntries );

	// Set CurNumBones to select the correct vertex shader for the number of bones
	pFX->SetInt( "g_nCurNumBones", dwNumInfl -1 );

	
	//////////////////////////////////////////////////////////////////////////
	//
	// [shhan][2014.10.21] PhysX 에서 넘어온 Normal 이 Inverse 된 상태인거 같다.
	//
	// 천 시뮬레이션일 경우 Vertex 의 위치를 재 설정해준다.
	if ( pMeshForCloth && pMeshForCloth->IsValid() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pFX->SetMatrix( "g_matWorld", &matIdentity );

		HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Object );
		if ( hr != D3D_OK )
		{
			return FALSE;
		}

		// Cloth 는 강도를 약하게 한다.
		//NSMaterialSkinManager::SetFX_LightParameter( pFX, pEffectMaterialData, 0.3f );
		//pFX->SetFloat( "g_fInverseLightIntensity",	0.1f );

		DWORD dwCullMode(0L);
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_CW );
		RenderCommon( pFX, pMeshForCloth->GetClothMesh() );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );

		{
			pFX->SetFloat( "g_fInverseNormal", -1.f );
			RenderCommon( pFX, pMeshForCloth->GetClothMesh() );
			pFX->SetFloat( "g_fInverseNormal", 1.f );
		}
		//pFX->SetFloat( "g_fInverseLightIntensity",	0.3f );
	}
	else
	{
		//if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		//{
		//	// Cloth 는 강도를 약하게 한다.
		//	NSMaterialSkinManager::SetFX_LightParameter( pFX, pEffectMaterialData, 0.3f );
		//	pFX->SetFloat( "g_fInverseLightIntensity",	0.1f );
		//}

		HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Skin );
		if ( hr != D3D_OK )
		{
			return FALSE;
		}

		RenderCommon( pFX, m_pMesh );

		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pFX->SetFloat( "g_fInverseNormal", -1.f );

			DWORD dwCullMode(0L);
			pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_CW );
			RenderCommon( pFX, m_pMesh );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );

			pFX->SetFloat( "g_fInverseNormal", 1.f );


			//pFX->SetFloat( "g_fInverseLightIntensity",	0.3f );
		}
	}

	return TRUE;
}

BOOL MaterialSkinMesh::RenderObject( LPDIRECT3DDEVICEQ pd3dDevice,
									  const D3DXMATRIX& matWorld, 
									  SMATERIAL_PIECE& sMaterialSystem,
									  const EffectMaterialData* pEffectMaterialData,
									  LPDIRECT3DTEXTUREQ pOverlayTex,
									  float fAlpha,
									  const MaterialSkinMeshForCloth* pMeshForCloth )
{
	if ( !m_pMesh )
		return FALSE;

	ID3DXEffect* pFX = SetCommonFX( pd3dDevice, sMaterialSystem, pEffectMaterialData );
	if ( !pFX )
		return FALSE;

	pFX->SetFloat( "g_fAlpha", fAlpha );

	if ( pOverlayTex )
	{
		pFX->SetTexture( "g_BaseTexture", pOverlayTex );
	}

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
	{
		// [shhan][2014.08.06] 씨름부원 폴리곤 보이는 현상 때문에 0.0012f 보다 수치를 내릴수가 없다.
		//D3DXVECTOR4 vShadow( 1.f/DxSurfaceTex::CASCADESHADOWSIZE.x, 0.5f, 0.f, 0.f );	// x-SIZE(해상도)
		D3DXVECTOR4 vShadow( 1.f/DxSurfaceTex::CASCADESHADOWSIZE.x, 0.0012f, 0.f, 0.f );	// x-SIZE(해상도)
		pFX->SetVector( "g_vShadow", &vShadow );
		pFX->SetTexture( "g_ShadowTexture_PCF",	DxSurfaceTex::GetInstance().m_pShadowTexR32F_CSM );
	}
	else if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
	{
		// [shhan][2014.08.06] 씨름부원 폴리곤 보이는 현상 때문에 0.0012f 보다 수치를 내릴수가 없다.
		D3DXVECTOR4 vShadow( 1.f/DxSurfaceTex::CASCADESHADOWSIZE.z, 0.0012f, 0.f, 0.f );	// x-SIZE(해상도)
		pFX->SetVector( "g_vShadow", &vShadow );
		pFX->SetTexture( "g_ShadowTexture_PCF",	DxSurfaceTex::GetInstance().m_pShadowTexR32F );
	}

	pFX->SetVector( "g_vLightPosWorldSpace", &NSMaterialManager::g_vLightPosWorldSpace );

	D3DXMATRIX matLightViewProjClip2Tex;
	DxShadowMap::GetInstance().GetMultiply_Clip2Tex( matLightViewProjClip2Tex, DxShadowMap::GetInstance().GetMatrix_D() );
	pFX->SetMatrixTranspose( "g_matShadow_Clip2Tex",	&matLightViewProjClip2Tex );

	HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Object );
	if ( hr != D3D_OK )
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// [shhan][2014.10.21] PhysX 에서 넘어온 Normal 이 Inverse 된 상태인거 같다.
	//
	// 천 시뮬레이션일 경우 Vertex 의 위치를 재 설정해준다.
	if ( pMeshForCloth && pMeshForCloth->IsValid() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pFX->SetMatrix( "g_matWorld", &matIdentity );

		// Cloth 는 강도를 약하게 한다.
		//NSMaterialSkinManager::SetFX_LightParameter( pFX, pEffectMaterialData, 0.3f );
		//pFX->SetFloat( "g_fInverseLightIntensity",	0.1f );

		DWORD dwCullMode(0L);
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_CW );

		RenderCommon( pFX, pMeshForCloth->GetClothMesh() );

		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );

		{
			pFX->SetFloat( "g_fInverseNormal", -1.f );
			RenderCommon( pFX, pMeshForCloth->GetClothMesh() );
			pFX->SetFloat( "g_fInverseNormal", 1.f );
		}
		//pFX->SetFloat( "g_fInverseLightIntensity",	0.3f );
	}
	else
	{
		//if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		//{
		//	// Cloth 는 강도를 약하게 한다.
		//	NSMaterialSkinManager::SetFX_LightParameter( pFX, pEffectMaterialData, 0.3f );
		//	pFX->SetFloat( "g_fInverseLightIntensity",	0.1f );
		//}

		pFX->SetMatrix( "g_matWorld", &matWorld );

		RenderCommon( pFX, m_pMesh );

		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pFX->SetFloat( "g_fInverseNormal", -1.f );

			DWORD dwCullMode(0L);
			pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_CW );
			RenderCommon( pFX, m_pMesh );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );

			pFX->SetFloat( "g_fInverseNormal", 1.f );


			//pFX->SetFloat( "g_fInverseLightIntensity",	0.3f );
		}
	}

	return TRUE;
}

// Ghosting 에서 망토는 렌더링 안한다.
BOOL MaterialSkinMesh::RenderGhosting_Skin( LPDIRECT3DDEVICEQ pd3dDevice, 
											  const D3DXMATRIX* arrayBoneMatrices,
											  SMeshContainer *pMeshContainer,
											  SMATERIAL_PIECE& sMaterialSystem,
											  const EffectMaterialData* pEffectMaterialData,
											  const SKIN_SPEC_SSS_INFO_REFERENCE* pSkinInfo,
											  LPDIRECT3DTEXTUREQ pOverlayTex,
											  float fAlpha )
{
	if ( !m_pMesh )
		return FALSE;

	ID3DXEffect* pFX = SetCommonFX( pd3dDevice, sMaterialSystem, pEffectMaterialData );
	if ( !pFX )
		return FALSE;

	DWORD dwNumInfl = pMeshContainer->m_dwNumInfl;
	DWORD dwNumPaletteEntries = pMeshContainer->m_dwNumPaletteEntries;

	pFX->SetFloat( "g_fAlpha", fAlpha );

	if ( pOverlayTex )
	{
		pFX->SetTexture( "g_BaseTexture", pOverlayTex );
	}

	pFX->SetMatrixArray( "g_matWorldMatrixArray", arrayBoneMatrices, dwNumPaletteEntries );

	// Set CurNumBones to select the correct vertex shader for the number of bones
	pFX->SetInt( "g_nCurNumBones", dwNumInfl -1 );

	// TnL_CHAR_PS_1_1 로 동작된다.
	{
		HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Skin );
		if ( hr != D3D_OK )
		{
			return FALSE;
		}

		RenderCommonPass( pFX, m_pMesh, 0 );
	}

	return TRUE;
}

// Ghosting 에서 망토는 렌더링 안한다.
BOOL MaterialSkinMesh::RenderGhosting_Object( LPDIRECT3DDEVICEQ pd3dDevice,
												const D3DXMATRIX& matWorld, 
												SMATERIAL_PIECE& sMaterialSystem,
												const EffectMaterialData* pEffectMaterialData,
												LPDIRECT3DTEXTUREQ pOverlayTex,
												float fAlpha )
{
	if ( !m_pMesh )
		return FALSE;

	ID3DXEffect* pFX = SetCommonFX( pd3dDevice, sMaterialSystem, pEffectMaterialData );
	if ( !pFX )
		return FALSE;

	pFX->SetFloat( "g_fAlpha", fAlpha );

	if ( pOverlayTex )
	{
		pFX->SetTexture( "g_BaseTexture", pOverlayTex );
	}	

	HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Object );
	if ( hr != D3D_OK )
	{
		return FALSE;
	}

	// TnL_CHAR_PS_1_1 로 동작된다.
	{
		pFX->SetMatrix( "g_matWorld", &matWorld );

		RenderCommonPass( pFX, m_pMesh, 0 );
	}

	return TRUE;
}

BOOL MaterialSkinMesh::RenderGlow_Skin( LPDIRECT3DDEVICEQ pd3dDevice, 
										 const D3DXMATRIX* arrayBoneMatrices,
										 DWORD dwNumPaletteEntries,
										 DWORD dwNumInfl, 
										 SMATERIAL_PIECE& sMaterialSystem,
										 const EffectMaterialData* pEffectMaterialData,
										 const SKIN_SPEC_SSS_INFO_REFERENCE* pSkinInfo,
										 LPDIRECT3DTEXTUREQ pGlowTex,
										 const D3DXVECTOR4* pGlowColor,
										 UINT unPass,
										 const MaterialSkinMeshForCloth* pMeshForCloth )
{
	if ( !m_pMesh )
		return FALSE;

	ID3DXEffect* pFX = SetCommonFX( pd3dDevice, sMaterialSystem, pEffectMaterialData );
	if ( !pFX )
		return FALSE;

	pFX->SetTexture( "g_BaseTexture", pGlowTex );
	pFX->SetVector( "g_vGlowColor", pGlowColor );

	pFX->SetMatrixArray( "g_matWorldMatrixArray", arrayBoneMatrices, dwNumPaletteEntries );

	// Set CurNumBones to select the correct vertex shader for the number of bones
	pFX->SetInt( "g_nCurNumBones", dwNumInfl -1 );


	//////////////////////////////////////////////////////////////////////////
	// 천 시뮬레이션일 경우 Vertex 의 위치를 재 설정해준다.
	if ( pMeshForCloth && pMeshForCloth->IsValid() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pFX->SetMatrix( "g_matWorld", &matIdentity );

		HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Object );
		if ( hr != D3D_OK )
		{
			return FALSE;
		}

		DWORD dwCullMode(0L);
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
		RenderCommonPass( pFX, pMeshForCloth->GetClothMesh(), unPass );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );
	}
	else
	{
		HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Skin );
		if ( hr != D3D_OK )
		{
			return FALSE;
		}

		DWORD dwCullMode(0L);
		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
		}

		RenderCommonPass( pFX, m_pMesh, unPass );

		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );
		}
	}

	return TRUE;
}

BOOL MaterialSkinMesh::RenderGlow_Object( LPDIRECT3DDEVICEQ pd3dDevice,
										   const D3DXMATRIX& matWorld, 
										   SMATERIAL_PIECE& sMaterialSystem,
										   const EffectMaterialData* pEffectMaterialData,
										   LPDIRECT3DTEXTUREQ pGlowTex,
										   const D3DXVECTOR4* pGlowColor,
										   UINT unPass,
										   const MaterialSkinMeshForCloth* pMeshForCloth )
{
	if ( !m_pMesh )
		return FALSE;

	ID3DXEffect* pFX = SetCommonFX( pd3dDevice, sMaterialSystem, pEffectMaterialData );
	if ( !pFX )
		return FALSE;

	pFX->SetTexture( "g_BaseTexture", pGlowTex );
	pFX->SetVector( "g_vGlowColor", pGlowColor );

	pFX->SetMatrix( "g_matWorld", &matWorld );

	HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Object );
	if ( hr != D3D_OK )
	{
		return FALSE;
	}


	//////////////////////////////////////////////////////////////////////////
	// 천 시뮬레이션일 경우 Vertex 의 위치를 재 설정해준다.
	if ( pMeshForCloth && pMeshForCloth->IsValid() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pFX->SetMatrix( "g_matWorld", &matIdentity );

		DWORD dwCullMode(0L);
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
		RenderCommonPass( pFX, pMeshForCloth->GetClothMesh(), unPass );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );
	}
	else
	{
		DWORD dwCullMode(0L);
		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
		}

		RenderCommonPass( pFX, m_pMesh, unPass );

		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );
		}
	}

	return TRUE;
}

BOOL MaterialSkinMesh::RenderShadow_Skin( LPDIRECT3DDEVICEQ pd3dDevice, 
										   const D3DXMATRIX* arrayBoneMatrices,
										   DWORD dwNumPaletteEntries,
										   DWORD dwNumInfl, 
										   const D3DXMATRIX& matViewProj,
										   SMATERIAL_PIECE& sMaterialSystem,
										   BOOL bMaterialSystemWLD,
										   BOOL bOpaqueTex,
										   const MaterialSkinMeshForCloth* pMeshForCloth )
{
	if ( !m_pMesh )
		return FALSE;

	if ( !m_sMaterialData.m_rMaterial )
		return FALSE;

	ID3DXEffect* pFX = m_sMaterialData.m_rMaterial->m_pFX;
	if ( !pFX )
		return FALSE;

	// 텍스쳐 정보.
	if ( m_sMaterialData.m_bDefaultMaterial )
	{
		pFX->SetTexture( "g_BaseTexture", sMaterialSystem.m_textureRes.GetTexture() );
	}

	pFX->SetMatrixArray( "g_matWorldMatrixArray", arrayBoneMatrices, dwNumPaletteEntries );
	pFX->SetMatrix( "g_matViewProj",	&matViewProj );

	// Set CurNumBones to select the correct vertex shader for the number of bones
	pFX->SetInt( "g_nCurNumBones", dwNumInfl -1 );

	// [shhan][2013.10.18] CRM#1497 - [Map]Mob 그림자 관련 확인요청
	//									처음엔 0.001f 로 하였다.
	//									그런데 TnL_PS_3_0_CSM 에서 캐릭터 발쪽 그림자가 안나오는 문제가 생겨서, 10.f 했더니 잘 나왔다.
	//									그렇지만 TnL_PS_3_0_NM 이하 에서는 그림자가 나오지 않아야될 벽쪽으로도 생성이 되었다.
	//									일단 그래서 옵션별로 셋팅을 나눠 놓았지만 TnL_PS_3_0_CSM 쪽이 이상하긴 하다.
	//									다른 그래픽카드에서 문제가 있다면 0.001f 으로 하는게 맞을것이다.
	//
	// [shhan][2014.07.11] Self-Shadow 때문에 값을 10.f 로 줄수가 없다.
	//
	//if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
	//{
	//	pFX->SetFloat( "g_fDepthOffset", 10.f );
	//}
	//else
	{
		pFX->SetFloat( "g_fDepthOffset", 0.001f );
	}

	// 위치값
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
	if ( pLight )
	{
		D3DXVECTOR4 vPos4;
		vPos4.x = DxViewPort::GetInstance().GetLookatPt().x - pLight->m_Light.Direction.x*1500.f;
		vPos4.y = DxViewPort::GetInstance().GetLookatPt().y - pLight->m_Light.Direction.y*1500.f;
		vPos4.z = DxViewPort::GetInstance().GetLookatPt().z - pLight->m_Light.Direction.z*1500.f;
		vPos4.w = 1.f;
		pFX->SetVector( "g_vLightPosWorldSpace", &vPos4 );
	}

	UINT unPass(0);
	if ( bMaterialSystemWLD )
	{
		unPass = 1;
	}

	if ( !bOpaqueTex )
	{
		unPass += 2;
	}

	//////////////////////////////////////////////////////////////////////////
	// 천 시뮬레이션일 경우 Vertex 의 위치를 재 설정해준다.
	if ( pMeshForCloth && pMeshForCloth->IsValid() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pFX->SetMatrix( "g_matWorld", &matIdentity );

		HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Object );
		if ( hr != D3D_OK )
		{
			return FALSE;
		}

		DWORD dwCullMode(0L);
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
		RenderCommonPass( pFX, pMeshForCloth->GetClothMesh(), unPass );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );
	}
	else
	{
		HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Skin );
		if ( hr != D3D_OK )
		{
			return FALSE;
		}

		DWORD dwCullMode(0L);
		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
		}

		RenderCommonPass( pFX, m_pMesh, unPass );

		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );
		}
	}

	return TRUE;
}

BOOL MaterialSkinMesh::RenderShadow_Object( LPDIRECT3DDEVICEQ pd3dDevice,
											const D3DXMATRIX& matWorld, 
											const D3DXMATRIX& matViewProj,
											SMATERIAL_PIECE& sMaterialSystem,
											BOOL bMaterialSystemWLD,
											BOOL bOpaqueTex,
											const MaterialSkinMeshForCloth* pMeshForCloth )
{
	if ( !m_pMesh )
		return FALSE;

	if ( !m_sMaterialData.m_rMaterial )
		return FALSE;

	ID3DXEffect* pFX = m_sMaterialData.m_rMaterial->m_pFX;
	if ( !pFX )
		return FALSE;

	// 텍스쳐 정보.
	if ( m_sMaterialData.m_bDefaultMaterial )
	{
		pFX->SetTexture( "g_BaseTexture", sMaterialSystem.m_textureRes.GetTexture() );
	}

	pFX->SetMatrix( "g_matWorld", &matWorld );
	pFX->SetMatrix( "g_matViewProj",	&matViewProj );

	// [shhan][2013.10.18] CRM#1497 - [Map]Mob 그림자 관련 확인요청
	//									처음엔 0.001f 로 하였다.
	//									그런데 TnL_PS_3_0_CSM 에서 캐릭터 발쪽 그림자가 안나오는 문제가 생겨서, 10.f 했더니 잘 나왔다.
	//									그렇지만 TnL_PS_3_0_NM 이하 에서는 그림자가 나오지 않아야될 벽쪽으로도 생성이 되었다.
	//									일단 그래서 옵션별로 셋팅을 나눠 놓았지만 TnL_PS_3_0_CSM 쪽이 이상하긴 하다.
	//									다른 그래픽카드에서 문제가 있다면 0.001f 으로 하는게 맞을것이다.
	//
	// [shhan][2014.07.11] Self-Shadow 때문에 값을 10.f 로 줄수가 없다.
	//
	//if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
	//{
	//	pFX->SetFloat( "g_fDepthOffset", 10.f );
	//}
	//else
	{
		pFX->SetFloat( "g_fDepthOffset", 0.001f );
	}

	// 위치값
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
	if ( pLight )
	{
		D3DXVECTOR4 vPos4;
		vPos4.x = DxViewPort::GetInstance().GetLookatPt().x - pLight->m_Light.Direction.x*1500.f;
		vPos4.y = DxViewPort::GetInstance().GetLookatPt().y - pLight->m_Light.Direction.y*1500.f;
		vPos4.z = DxViewPort::GetInstance().GetLookatPt().z - pLight->m_Light.Direction.z*1500.f;
		vPos4.w = 1.f;
		pFX->SetVector( "g_vLightPosWorldSpace", &vPos4 );
	}

	HRESULT hr = pFX->SetTechnique ( m_sMaterialData.m_rMaterial->m_hTechnique_runtime_Object );
	if ( hr != D3D_OK )
	{
		return FALSE;
	}

	UINT unPass(0);
	if ( bMaterialSystemWLD )
	{
		unPass = 1;
	}

	if ( !bOpaqueTex )
	{
		unPass += 2;
	}

	//////////////////////////////////////////////////////////////////////////
	// 천 시뮬레이션일 경우 Vertex 의 위치를 재 설정해준다.
	if ( pMeshForCloth && pMeshForCloth->IsValid() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pFX->SetMatrix( "g_matWorld", &matIdentity );

		DWORD dwCullMode(0L);
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
		RenderCommonPass( pFX, pMeshForCloth->GetClothMesh(), unPass );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );
	}
	else
	{
		DWORD dwCullMode(0L);
		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pd3dDevice->GetRenderState( D3DRS_CULLMODE,	&dwCullMode );
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
		}

		RenderCommonPass( pFX, m_pMesh, unPass );

		if ( sMaterialSystem.m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CLOTH )
		{
			pd3dDevice->SetRenderState( D3DRS_CULLMODE,	dwCullMode );
		}
	}

	return TRUE;
}

// pEffectMaterialData 값이 NULL 이 올 수 있다.
ID3DXEffect* MaterialSkinMesh::SetCommonFX( LPDIRECT3DDEVICEQ pd3dDevice,
										   SMATERIAL_PIECE& sMaterialSystem, 
										   const EffectMaterialData* pEffectMaterialData )
{
	if ( !m_pMesh )
		return NULL;

	if ( !m_sMaterialData.m_rMaterial )
		return NULL;

	ID3DXEffect* pFX = m_sMaterialData.m_rMaterial->m_pFX;
	if ( !pFX )
		return NULL;

	NSMaterialSkinManager::SetFX_CommonParameter( pFX );

	NSMaterialSkinManager::SetFX_LightParameter( pFX, pEffectMaterialData );

	// 텍스쳐 정보.
	if ( m_sMaterialData.m_bDefaultMaterial )
	{
		pFX->SetTexture( "g_BaseTexture", sMaterialSystem.m_textureRes.GetTexture() );

		if ( pEffectMaterialData && pEffectMaterialData->m_nEnableHairIndex >= 0 )
		{
			pFX->SetVector( "g_vHairShiftUV_SpecExp_DL_CL", &sMaterialSystem.m_vHairShiftUV_SpecExp_DL_CL );
			pFX->SetVector( "g_vHairSpecColor_DirectionIntensity", &sMaterialSystem.m_vHairSpecColor_DirectionIntensity );

			if ( sMaterialSystem.m_textureRes_HairMask.GetTexture() )
			{
				pFX->SetTexture( "g_MaskTexture", sMaterialSystem.m_textureRes_HairMask.GetTexture() );
			}
		}
		else
		{
			if ( sMaterialSystem.m_textureRes_mask.GetTexture() )
			{
				pFX->SetTexture( "g_MaskTexture", sMaterialSystem.m_textureRes_mask.GetTexture() );
			}
			else if ( sMaterialSystem.m_pBakeMaskTex )
			{
				pFX->SetTexture( "g_MaskTexture", sMaterialSystem.m_pBakeMaskTex );
			}
			else
			{
				pFX->SetTexture( "g_MaskTexture", NSMaterialManager::GetBlackTexture() );
			}

			if ( sMaterialSystem.m_textureRes_mask2.GetTexture() )
			{
				pFX->SetTexture( "g_Mask2Texture", sMaterialSystem.m_textureRes_mask2.GetTexture() );
			}
			else if ( sMaterialSystem.m_pBakeMask2Tex )
			{
				pFX->SetTexture( "g_Mask2Texture", sMaterialSystem.m_pBakeMask2Tex );
			}
			else
			{
				pFX->SetTexture( "g_Mask2Texture", NSMaterialManager::GetBlackTexture() );
			}
		}
	}

	if ( pEffectMaterialData )
	{
		// 레벨정보 셋팅.
		if ( pEffectMaterialData->m_rLevelData )
		{
			if ( pEffectMaterialData->m_rLevelData->m_bLevel )
			{
				pFX->SetVector( "g_vLightLevel", &pEffectMaterialData->m_rLevelData->m_vLevelColor );
			}

			if ( pEffectMaterialData->m_rLevelData->m_bLevelFlow )
			{
				pFX->SetVector( "g_vLevelFlowColor", &pEffectMaterialData->m_rLevelData->m_vLevelFlowColor );
				pFX->SetTexture( "g_LevelFlowTexture", pEffectMaterialData->m_rLevelData->m_rLevelFlowTex );
			}		
		}

		// 멀티텍스쳐 정보 셋팅.
		if ( pEffectMaterialData->m_bMultiTex )
		{
			pFX->SetInt( "g_nMulti_ADD", pEffectMaterialData->m_nMultiTex_ADD );
			pFX->SetTexture( "g_MultiTexture", pEffectMaterialData->m_rMultiTex );
		}

		// CubeBlend 정보 셋팅.
		if ( pEffectMaterialData->m_bCubeBlend )
		{
			// Global CubeMap
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
			{
				pFX->SetTexture( "g_GlobalCubeTexture", NSCubeMapUtil::NSCubeMapBlend::GetCubeMapTexture() );
			}
			else
			{
				if ( pEffectMaterialData->m_pCubeMapTexture )
				{
					pFX->SetTexture( "g_GlobalCubeTexture", pEffectMaterialData->m_pCubeMapTexture );
				}
				else
				{
					pFX->SetTexture( "g_GlobalCubeTexture", NSCubeMapUtil::GetTempCubeMapTexture() );
				}
			}
		}

		// Tex UpDown 셋팅.
		if ( !pEffectMaterialData->m_vecTexDiffColor.empty() )
		{
			
			pFX->SetFloat( "g_fTexDiffColor", m_fTexDiffColor );
			pFX->SetTexture( "g_pTexUpDownTexture", m_rTexUpDownTexture );
		}
	}

	return pFX;
}

// 로그로 인해 게임에 방해될 수 있으니 확인용으로 한번만 나오도록 한다.
BOOL g_bMaterialSkinMesh_FirstLog(TRUE);

void MaterialSkinMesh::RenderCommon( ID3DXEffect* pFX, LPD3DXMESH pMesh )
{
	UINT nPass(0);
	if ( RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_3_0 )
	{
		nPass = 1;	// SelfShadow - VSM 적용.
	}

	//if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
	//{
	//	// SelfShadow 가능.
	//	nPass = 1;
	//	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
	//		nPass = 2;	//VSM 적용.
	//}

	UINT numPasses;
	HRESULT hr = pFX->Begin( &numPasses, 0 );
	if ( hr == D3D_OK )
	{
		if( nPass < numPasses )
		{
			hr = pFX->BeginPass( nPass );
			if ( hr == D3D_OK )
			{
				pMesh->DrawSubset( 0 );
			}
			else if ( g_bMaterialSkinMesh_FirstLog )
			{
				g_bMaterialSkinMesh_FirstLog = FALSE;

				CDebugSet::ToLogFile( std::string("MaterialSkinMesh::RenderCommon() 쪽 pFX->BeginPass 오류 \n") );
				GASSERT( 0 && _T("MaterialSkinMesh::RenderCommon() 쪽 pFX->BeginPass 오류") );
			}
			pFX->EndPass();
		}
	}
	else if ( g_bMaterialSkinMesh_FirstLog )
	{
		g_bMaterialSkinMesh_FirstLog = FALSE;

		CDebugSet::ToLogFile( std::string("MaterialSkinMesh::RenderCommon() pFX->Begin 오류 \n") );
		GASSERT( 0 && _T("MaterialSkinMesh::RenderCommon() pFX->Begin 오류") );
	}
	pFX->End();
}

void MaterialSkinMesh::RenderCommonPass( ID3DXEffect* pFX, LPD3DXMESH pMesh, UINT unPass )
{
	UINT numPasses;
	HRESULT hr = pFX->Begin( &numPasses, 0 );
	if ( hr == D3D_OK )
	{
		if( unPass < numPasses )
		{
			hr = pFX->BeginPass( unPass );
			if ( hr == D3D_OK )
			{
				pMesh->DrawSubset( 0 );
			}
			else if ( g_bMaterialSkinMesh_FirstLog )
			{
				g_bMaterialSkinMesh_FirstLog = FALSE;

				CDebugSet::ToLogFile( std::string("MaterialSkinMesh::RenderCommonPass() 쪽 pFX->BeginPass 오류 \n") );
				GASSERT( 0 && _T("MaterialSkinMesh::RenderCommonPass() 쪽 pFX->BeginPass 오류") );
			}
			pFX->EndPass();
		}
	}
	else if ( g_bMaterialSkinMesh_FirstLog )
	{
		g_bMaterialSkinMesh_FirstLog = FALSE;

		CDebugSet::ToLogFile( std::string("MaterialSkinMesh::RenderCommonPass() pFX->Begin 오류 \n") );
		GASSERT( 0 && _T("MaterialSkinMesh::RenderCommonPass() pFX->Begin 오류") );
	}
	pFX->End();
}



//////////////////////////////////////////////////////////////////////////
//							NSMATERIALMESH
namespace NSMATERIALSKINMESH
{
	// WorldEdit 에서 확인용 데이터
	// 코딩을 잘못 했을 경우, 너무 많은 호출이 될 수 있어서, 확인용 변수
	int g_nCreateCount(0);

	//BOOL CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, 
	//					const MaterialSmall* pMaterialSmall, 
	//					const LPD3DXMESH pSrcMesh, 
	//					LPD3DXMESH& pMeshOUT )
	//{
	//	++g_nCreateCount;

	//	// Clone mesh to the new vertex format
	//	LPD3DXMESH pTempMesh(NULL);
	//	HRESULT hr = pSrcMesh->CloneMesh( pSrcMesh->GetOptions(), pMaterialSmall->m_pELMT, pd3dDevice, &pTempMesh );

	//	// 단순한 확인용
	//	//DWORD dwFVF = pSrcMesh->GetFVF();

	//	SAFE_RELEASE( pMeshOUT );
	//	pMeshOUT = pTempMesh;
	//	pTempMesh = NULL;

	//	// 단순한 확인용
	//	//dwFVF = pMeshOUT->GetFVF();

	//	// Tangent 혹은 bBinormal 가 필요하다면 생성작업한다.
	//	if ( pMaterialSmall->m_bTangent || pMaterialSmall->m_bBinormal )
	//	{
	//		if ( pMaterialSmall->m_bTangent && pMaterialSmall->m_bBinormal )
	//		{
	//			// Compute tangents, which are required for normal mapping
	//			hr = D3DXComputeTangentFrameEx( pMeshOUT, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
	//				D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_NORMAL, 0,
	//				0, NULL, -1, 0, -1, &pTempMesh, NULL );
	//		}
	//		else if ( pMaterialSmall->m_bTangent )
	//		{
	//			// Compute tangents, which are required for normal mapping
	//			hr = D3DXComputeTangentFrameEx( pMeshOUT, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
	//				D3DX_DEFAULT, 0, D3DDECLUSAGE_NORMAL, 0,
	//				0, NULL, -1, 0, -1, &pTempMesh, NULL );
	//		}
	//		else if ( pMaterialSmall->m_bBinormal )
	//		{
	//			// Compute tangents, which are required for normal mapping
	//			hr = D3DXComputeTangentFrameEx( pMeshOUT, D3DDECLUSAGE_TEXCOORD, 0, D3DX_DEFAULT, 0,
	//				D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_NORMAL, 0,
	//				0, NULL, -1, 0, -1, &pTempMesh, NULL );
	//		}
	//		else
	//		{
	//			return FALSE;
	//		}

	//		if( FAILED(hr) )
	//			return FALSE;

	//		// Copy the mesh and return
	//		SAFE_RELEASE( pMeshOUT );
	//		pMeshOUT = pTempMesh;
	//		pTempMesh = NULL;
	//	}

	//	return TRUE;
	//}

	BOOL CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, 
					const MaterialSmall* pMaterialSmall,
					LPD3DXMESH& pMesh_IN_OUT )
	{
		HRESULT hr(S_OK);

		++g_nCreateCount;

		// Clone mesh to the new vertex format
		LPD3DXMESH pTempMesh(NULL);

		// 단순한 확인용
		//dwFVF = pMeshOUT->GetFVF();

		// Tangent 혹은 bBinormal 가 필요하다면 생성작업한다.
		if ( pMaterialSmall->m_bTangent || pMaterialSmall->m_bBinormal )
		{

			//////////////////////////////////////////////////////////////////////////
			// [shhan][2014.05.07] 이전에 D3DDECLUSAGE_TANGENT 만 사용했을시 UV가 Mirror가 될 경우, 문제가 발생했었다.
			//						그래서 D3DDECLUSAGE_TANGENT, D3DDECLUSAGE_BINORMAL 둘다 사용하시 해결을 했는데,
			//						D3DDECLUSAGE_TANGENT 값이 이젠 정상이니, 굳이 D3DDECLUSAGE_BINORMAL 까지 가지고 갈 필요는 없어서,
			//						Binormal 은 삭제 후 Shader 에서 계산해주는걸 작업함.
			//

			DWORD dwCount(0);
			for ( DWORD i=0; i<MAXD3DDECLLENGTH; ++i )
			{
				if ( pMaterialSmall->m_pELMT[i].Stream == 0xff )
				{
					dwCount = i;
					break;
				}
			}

			D3DVERTEXELEMENT9* pVertElements = new D3DVERTEXELEMENT9[dwCount+3];

			BOOL bTangent(FALSE);
			BOOL bBinormal(FALSE);
			for ( DWORD i=0; i<dwCount; ++i )
			{
				if ( pMaterialSmall->m_pELMT[i].Usage == D3DDECLUSAGE_TANGENT )
				{
					bTangent = TRUE;
				}
				if ( pMaterialSmall->m_pELMT[i].Usage == D3DDECLUSAGE_BINORMAL )
				{
					bBinormal = TRUE;
				}
			}

			int dwEndIndex(0);
			if ( !bTangent && !bBinormal )
			{
				int nOffset(0);
				for ( DWORD i=0; i<dwCount; ++i )
				{
					pVertElements[i+2] = pMaterialSmall->m_pELMT[i];
					pVertElements[i+2].Offset += 24;
				}

				pVertElements[0].Stream			= 0;
				pVertElements[0].Offset			= 0;
				pVertElements[0].Type			= D3DDECLTYPE_FLOAT3;
				pVertElements[0].Method			= D3DDECLMETHOD_DEFAULT;
				pVertElements[0].Usage			= D3DDECLUSAGE_TANGENT;
				pVertElements[0].UsageIndex		= 0;

				pVertElements[1].Stream			= 0;
				pVertElements[1].Offset			= 12;
				pVertElements[1].Type			= D3DDECLTYPE_FLOAT3;
				pVertElements[1].Method			= D3DDECLMETHOD_DEFAULT;
				pVertElements[1].Usage			= D3DDECLUSAGE_BINORMAL;
				pVertElements[1].UsageIndex		= 0;

				dwEndIndex = dwCount+2;
			}
			else if ( !bTangent )
			{
				int nOffset(0);
				for ( DWORD i=0; i<dwCount; ++i )
				{
					pVertElements[i+1] = pMaterialSmall->m_pELMT[i];
					pVertElements[i+1].Offset += 12;
				}

				pVertElements[0].Stream			= 0;
				pVertElements[0].Offset			= 0;
				pVertElements[0].Type			= D3DDECLTYPE_FLOAT3;
				pVertElements[0].Method			= D3DDECLMETHOD_DEFAULT;
				pVertElements[0].Usage			= D3DDECLUSAGE_TANGENT;
				pVertElements[0].UsageIndex		= 0;

				dwEndIndex = dwCount+1;
			}
			else if ( !bBinormal )
			{
				int nOffset(0);
				for ( DWORD i=0; i<dwCount; ++i )
				{
					pVertElements[i+1] = pMaterialSmall->m_pELMT[i];
					pVertElements[i+1].Offset += 12;
				}

				pVertElements[0].Stream			= 0;
				pVertElements[0].Offset			= 0;
				pVertElements[0].Type			= D3DDECLTYPE_FLOAT3;
				pVertElements[0].Method			= D3DDECLMETHOD_DEFAULT;
				pVertElements[0].Usage			= D3DDECLUSAGE_BINORMAL;
				pVertElements[0].UsageIndex		= 0;

				dwEndIndex = dwCount+1;
			}
			else
			{
				for ( DWORD i=0; i<dwCount; ++i )
				{
					pVertElements[i] = pMaterialSmall->m_pELMT[i];
				}

				dwEndIndex = dwCount;
			}

			//int nOffset(0);
			//for ( DWORD i=0; i<dwCount; ++i )
			//{
			//	pVertElements[i+1] = pMaterialSmall->m_pELMT[i];
			//	pVertElements[i+1].Offset += 12;
			//}

			//pVertElements[0].Stream			= 0;
			//pVertElements[0].Offset			= 0;
			//pVertElements[0].Type			= D3DDECLTYPE_FLOAT3;
			//pVertElements[0].Method			= D3DDECLMETHOD_DEFAULT;
			//pVertElements[0].Usage			= D3DDECLUSAGE_BINORMAL;
			//pVertElements[0].UsageIndex		= 0;

			pVertElements[dwEndIndex].Stream		= 0xff;
			pVertElements[dwEndIndex].Offset		= 0;
			pVertElements[dwEndIndex].Type			= D3DDECLTYPE_UNUSED;
			pVertElements[dwEndIndex].Method		= 0;
			pVertElements[dwEndIndex].Usage			= 0;
			pVertElements[dwEndIndex].UsageIndex	= 0;


			hr = pMesh_IN_OUT->CloneMesh( pMesh_IN_OUT->GetOptions(), pVertElements, pd3dDevice, &pTempMesh );
			if( FAILED(hr) )
			{
				SAFE_DELETE_ARRAY ( pVertElements );
				return FALSE;
			}

			SAFE_DELETE_ARRAY ( pVertElements );

			{
				// Compute tangents, which are required for normal mapping
				SAFE_RELEASE( pMesh_IN_OUT );
				hr = D3DXComputeTangentFrameEx( pTempMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
					D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_NORMAL, 0,
					0, NULL, -1, 0, -1, &pMesh_IN_OUT, NULL );

				if( FAILED(hr) )
				{
					pMesh_IN_OUT = pTempMesh;
					pTempMesh = NULL;
					return FALSE;
				}
			}
			SAFE_RELEASE( pTempMesh );
		}

		hr = pMesh_IN_OUT->CloneMesh( pMesh_IN_OUT->GetOptions(), pMaterialSmall->m_pELMT, pd3dDevice, &pTempMesh );	
		if( FAILED(hr) )
		{
			return FALSE;
		}

		//// 단순한 확인용
		////DWORD dwFVF = pSrcMesh->GetFVF();

		SAFE_RELEASE( pMesh_IN_OUT );
		pMesh_IN_OUT = pTempMesh;
		pTempMesh = NULL;

		return TRUE;
	}
};