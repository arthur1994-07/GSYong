#include "pch.h"

#include "../../../SigmaCore/Util/SystemInfo.h"


#include "../../DxMeshs/SkinMesh/_new/DxSkinMeshContainer9.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/DxCharPart.h"

#include "../Material/NSMaterialManager.h"
#include "../Material/MaterialDef.h"
#include "../Collision.h"
#include "./MaterialSkinMesh.h"

#include "DxMultiMaterialSkinMesh.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// �ܺο��� ������ ������ �����ϰ� �Ѵ�.
extern TSTRING g_strGameForceExit;

//------------------------------------------------------------------------------
//							DxMultiMaterialSkinMesh
DxMultiMaterialSkinMesh::DxMultiMaterialSkinMesh()
{
}

DxMultiMaterialSkinMesh::~DxMultiMaterialSkinMesh()
{
	CleanUp();
}

void DxMultiMaterialSkinMesh::CleanUp()
{
	m_vecType.clear();
	m_vecMesh.clear();
}

//////////////////////////////////////////////////////////////////////////
// CAllocateHierarchy::CreateMeshContainer
void DxMultiMaterialSkinMesh::Import_First( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrc, DXMATERIALEX* pMaterialEx, LPD3DXBONECOMBINATION pBoneComb )
{
	CleanUp();

	HRESULT hr(S_OK);

	//////////////////////////////////////////////////////////////////////////
	//						Attribute ������.
	DWORD dwNumAttributeGroups(0L);
	hr = pSrc->GetAttributeTable(NULL, &dwNumAttributeGroups);
    if (FAILED(hr))
    {
        return;
	}

	D3DXATTRIBUTERANGE* pAttributeTable(NULL);
    pAttributeTable = new D3DXATTRIBUTERANGE[dwNumAttributeGroups];
    if (pAttributeTable == NULL)
    {
        hr = E_OUTOFMEMORY;
        return;
    }

    hr = pSrc->GetAttributeTable(pAttributeTable, NULL);
    if (FAILED(hr))
    {
        return;
	}


	//////////////////////////////////////////////////////////////////////////
	// �����۾�
	DWORD dwFVF = pSrc->GetFVF();
	UINT nSIZE = D3DXGetFVFVertexSize( dwFVF );
	D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
	pSrc->GetDeclaration( pDecl );


	//////////////////////////////////////////////////////////////////////////
	//						Mesh �и� �۾��� ���۵ȴ�.
	for ( DWORD i=0; i<dwNumAttributeGroups; i++ )
	{
		//////////////////////////////////////////////////////////////////////////
		// �����۾�
		LPD3DXMESH pDest(NULL);
		hr = D3DXCreateMeshFVF ( pAttributeTable[i].FaceCount, pAttributeTable[i].VertexCount, pSrc->GetOptions(), pSrc->GetFVF(), pd3dDevice, &pDest );
		if (FAILED(hr))
		{
			// [shhan][2015.02.23] �����ð� �����ϰ� �޸� ����ȭ �������� ���� ���� ���������� �߻��Ͽ� �޸� ���������� ������.
			//						�뷮 ������ �����ϴ� ������ ������ Ȯ���� ũ��. �׷��� �̰����� ��.
			sc::heapCompact();

			// �� ����
			hr = D3DXCreateMeshFVF ( pAttributeTable[i].FaceCount, pAttributeTable[i].VertexCount, pSrc->GetOptions(), pSrc->GetFVF(), pd3dDevice, &pDest );
			if (FAILED(hr))
			{
				// �ȵǸ� ��������.
				//AfxMessageBox("Low memory error. Please restart the game");
				//exit(1);
				g_strGameForceExit = _T("Low memory error. Please restart the game");
				return;
			}
		}



		//////////////////////////////////////////////////////////////////////////
		// VB ����
		BYTE*	pVerticesSRC;
		BYTE*	pVerticesDEST;
		pSrc->LockVertexBuffer ( 0L, (VOID**)&pVerticesSRC );
		pDest->LockVertexBuffer ( 0L, (VOID**)&pVerticesDEST );
		memcpy( pVerticesDEST, pVerticesSRC + (nSIZE*pAttributeTable[i].VertexStart), nSIZE*pAttributeTable[i].VertexCount );
		pDest->UnlockVertexBuffer();
		pSrc->UnlockVertexBuffer();



		//////////////////////////////////////////////////////////////////////////
		// IB ����
		WORD*	pIndicesSRC;
		WORD*	pIndicesDEST;
		pSrc->LockIndexBuffer ( 0L, (VOID**)&pIndicesSRC );
		pDest->LockIndexBuffer ( 0L, (VOID**)&pIndicesDEST );
		for ( DWORD z=0; z<pAttributeTable[i].FaceCount*3; ++z )
		{
			pIndicesDEST[z] = pIndicesSRC[z+(pAttributeTable[i].FaceStart*3)] - static_cast<WORD>( pAttributeTable[i].VertexStart );
		}
		pDest->UnlockIndexBuffer();
		pSrc->UnlockIndexBuffer();



		//////////////////////////////////////////////////////////////////////////
		// Attribute
		{
			D3DXATTRIBUTERANGE* pAttribTableNEW = new D3DXATTRIBUTERANGE [ 1 ];
			pAttribTableNEW[ 0 ].AttribId = 0;
			pAttribTableNEW[ 0 ].FaceStart = 0;
			pAttribTableNEW[ 0 ].FaceCount = pAttributeTable[i].FaceCount;
			pAttribTableNEW[ 0 ].VertexStart = 0;
			pAttribTableNEW[ 0 ].VertexCount = pAttributeTable[i].VertexCount;

			pDest->SetAttributeTable( pAttribTableNEW, 1 );

			SAFE_DELETE_ARRAY ( pAttribTableNEW );
		}



		//////////////////////////////////////////////////////////////////////////
		// Insert

		// AttribID �� ���Ѵ�.
		DWORD dwThisAttribId = pAttributeTable[i].AttribId;

		// Bone �� ���� ���� Bone�� ���õ� ���� ����ϵ��� �Ѵ�.
		if ( pBoneComb )
		{
			dwThisAttribId = pBoneComb[i].AttribId;
		}

		// Mesh ���� �� m_vecMesh �� ����.
		std::tr1::shared_ptr<MaterialSkinMesh> spNew = std::tr1::shared_ptr<MaterialSkinMesh>(new MaterialSkinMesh);
		spNew->Insert( pd3dDevice, pDest, nSIZE, pDecl, pMaterialEx[dwThisAttribId] );
		m_vecMesh.push_back( spNew );

		// m_vecType ����
		if ( pMaterialEx )
		{
			SetTexType( pMaterialEx[dwThisAttribId] );
		}
		else
		{
			m_vecType.push_back( TEX_TYPE_UNKNOWN );
		}



		//////////////////////////////////////////////////////////////////////////
		// ����
		SAFE_RELEASE ( pDest );
	}

	//////////////////////////////////////////////////////////////////////////
	//								������
	SAFE_DELETE_ARRAY(pAttributeTable);
}

void DxMultiMaterialSkinMesh::SetTexType( DXMATERIALEX& sMaterialEX )
{
	if ( !sMaterialEX.m_textureRes.IsValid() )
	{
		m_vecType.push_back( TEX_TYPE_UNKNOWN );
		return;
	}

	if ( sMaterialEX.m_textureRes.GetTexture() )
	{
		switch ( sMaterialEX.m_textureRes.GetAlphaType() )
		{
		case EMTT_NORMAL:
			m_vecType.push_back( TEX_TYPE_OPAQUE );
			break;

		case EMTT_ALPHA_HARD:
			m_vecType.push_back( TEX_TYPE_HARDALPHA );
			break;

		case EMTT_ALPHA_SOFT:
		case EMTT_ALPHA_SOFT01:
		case EMTT_ALPHA_SOFT02:
			m_vecType.push_back( TEX_TYPE_SOFTALPHA );
			break;

		default:
			m_vecType.push_back( TEX_TYPE_UNKNOWN );
			break;
		}
	}
	else
	{
		m_vecType.push_back( TEX_TYPE_UNKNOWN );
		return;
	}
}

void DxMultiMaterialSkinMesh::ChangeTexType( size_t unIndex, TextureResource& sTextureResource )
{
	if ( unIndex >= m_vecType.size() )
		return;

	if ( sTextureResource.GetTexture() )
	{
		switch ( sTextureResource.GetAlphaType() )
		{
		case EMTT_NORMAL:
			m_vecType[unIndex] = TEX_TYPE_OPAQUE;
			break;

		case EMTT_ALPHA_HARD:
			m_vecType[unIndex] = TEX_TYPE_HARDALPHA;
			break;

		case EMTT_ALPHA_SOFT:
		case EMTT_ALPHA_SOFT01:
		case EMTT_ALPHA_SOFT02:
			m_vecType[unIndex] = TEX_TYPE_SOFTALPHA;
			break;

		default:
			return;
			break;
		}
	}
}

void DxMultiMaterialSkinMesh::SetMaterialSetting( LPDIRECT3DDEVICEQ pd3dDevice, const EffectMaterialData* pEffectMaterialData, int ipattr, DWORD dwThisAttribId )
{
	m_vecMesh[ipattr]->SetMaterialSetting( pd3dDevice, pEffectMaterialData, dwThisAttribId );
}

void DxMultiMaterialSkinMesh::SetMaterialSettingGhosting( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecMesh.size(); ++i )
	{
		m_vecMesh[i]->SetMaterialSettingGhosting( pd3dDevice );
	}
}

void DxMultiMaterialSkinMesh::SetMaterialSettingGlow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecMesh.size(); ++i )
	{
		m_vecMesh[i]->SetMaterialSettingGlow( pd3dDevice );
	}
}

void DxMultiMaterialSkinMesh::SetMaterialSettingShadow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecMesh.size(); ++i )
	{
		m_vecMesh[i]->SetMaterialSettingShadow( pd3dDevice );
	}
}


//////////////////////////////////////////////////////////////////////////
// Render Skinned
void DxMultiMaterialSkinMesh::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
										   int ipattr,
										   const D3DXMATRIX* arrayBoneMatrices,
										   SMeshContainer *pMeshContainer,
										   //DWORD dwNumPaletteEntries,
										   //DWORD dwNumInfl,
										   DWORD dwThisAttribId,
										   SMATERIAL_PIECE& sMaterialSystem,
										   const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
										   const EffectMaterialData* pEffectMaterialData,
										   LPDIRECT3DTEXTUREQ pOverlayTex,
										   const D3DXMATRIX* pmatAttParentBone,
										   const DxClothColl* pClothColl,
										   const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
										   float fAttParentBoneScale,
										   float fAlpha,
										   DxPhysXCloth* pPhysXCloth )
{
	// HardAlpha, SoftAlpha�� ����ؼ� Opaque���� �ѹ��� �Ѵ�.
	m_vecMesh[ipattr]->CheckVersionSkin_UpdateCloth
	( 
		pd3dDevice,
		arrayBoneMatrices,
		pMeshContainer->m_dwNumInfl,
		pMeshContainer->m_dwNumPaletteEntries,
		sMaterialSystem,		
		pMeshContainer->m_arrayfInvWeight,
		pmatAttParentBone,
		pClothColl,
		pvecClothCollSpheres,
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth(ipattr) : NULL,
		fAttParentBoneScale
	);

	// �ؽ��� �ε��� �Ϸ� �Ǿ��ٸ� ������ �������ش�.
	if ( m_vecType[ ipattr ] == TEX_TYPE_UNKNOWN )
	{
		ChangeTexType( ipattr, sMaterialSystem.m_textureRes );
	}

	if ( m_vecType[ ipattr ] == TEX_TYPE_HARDALPHA || m_vecType[ ipattr ] == TEX_TYPE_SOFTALPHA )
		return;

	if ( sMaterialSystem.m_bBaseRenderOff )
		return;

	m_vecMesh[ipattr]->Render
	( 
		pd3dDevice, 
		arrayBoneMatrices, 
		pMeshContainer, 
		sMaterialSystem,
		pEffectMaterialData, 
		&sSkinInfo,
		pOverlayTex,
		fAlpha,
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL
	);
}

void DxMultiMaterialSkinMesh::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
											  int ipattr,
											  const D3DXMATRIX* arrayBoneMatrices,
											  SMeshContainer *pMeshContainer,
											  //DWORD dwNumPaletteEntries,
											  //DWORD dwNumInfl,
											  SMATERIAL_PIECE& sMaterialSystem,
											  const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
											  const EffectMaterialData* pEffectMaterialData,
											  LPDIRECT3DTEXTUREQ pOverlayTex,
											  float fAlpha,
											  const DxPhysXCloth* pPhysXCloth )
{
	if ( m_vecType[ ipattr ] != TEX_TYPE_HARDALPHA  )
		return;

	if ( sMaterialSystem.m_bBaseRenderOff )
		return;

	m_vecMesh[ipattr]->Render
	( 
		pd3dDevice, 
		arrayBoneMatrices, 
		pMeshContainer, 
		sMaterialSystem,
		pEffectMaterialData, 
		&sSkinInfo,
		pOverlayTex,
		fAlpha,
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL
	);
}

void DxMultiMaterialSkinMesh::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
												int ipattr,
												const D3DXMATRIX* arrayBoneMatrices,
												SMeshContainer *pMeshContainer,
												SMATERIAL_PIECE& sMaterialSystem,
												const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
												const EffectMaterialData* pEffectMaterialData,
												LPDIRECT3DTEXTUREQ pOverlayTex,
												float fAlpha,
												const DxPhysXCloth* pPhysXCloth )
{
	if ( m_vecType[ ipattr ] != TEX_TYPE_SOFTALPHA  )
		return;

	if ( sMaterialSystem.m_bBaseRenderOff )
		return;

	m_vecMesh[ipattr]->Render
	( 
		pd3dDevice, 
		arrayBoneMatrices, 
		pMeshContainer, 
		sMaterialSystem,
		pEffectMaterialData, 
		&sSkinInfo,
		pOverlayTex,
		fAlpha,
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL
	);
}

BOOL DxMultiMaterialSkinMesh::RenderGhosting( LPDIRECT3DDEVICEQ pd3dDevice, 
											  int ipattr,
											  const D3DXMATRIX* arrayBoneMatrices,
											  SMeshContainer *pMeshContainer,
											  SMATERIAL_PIECE& sMaterialSystem,
											  const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
											  const EffectMaterialData* pEffectMaterialData,
											  LPDIRECT3DTEXTUREQ pOverlayTex,
											  float fAlpha,
											  const DxPhysXCloth* pPhysXCloth )
{
	if ( sMaterialSystem.m_bBaseRenderOff )
		return FALSE;

	// Ghosting ������ õ�� ������ ���� �ʴ´�.
	if ( pPhysXCloth )
	{
		if ( pPhysXCloth->IsValid_PxCloth( ipattr ) )
			return TRUE;
	}

	return m_vecMesh[ipattr]->RenderGhosting_Skin
	( 
		pd3dDevice, 
		arrayBoneMatrices, 
		pMeshContainer, 
		sMaterialSystem,
		pEffectMaterialData, 
		&sSkinInfo,
		pOverlayTex,
		fAlpha
	);
}

void DxMultiMaterialSkinMesh::RenderGlow( LPDIRECT3DDEVICEQ pd3dDevice, 
										  int ipattr,
										  const D3DXMATRIX* arrayBoneMatrices,
										  DWORD dwNumPaletteEntries,
										  DWORD dwNumInfl,
										  DWORD dwThisAttribId, 
										  SMATERIAL_PIECE& sMaterialSystem,
										  LPDIRECT3DTEXTUREQ pGlowTex,
										  const D3DXVECTOR4* pGlowColor,
										  UINT unPass,
										  const DxPhysXCloth* pPhysXCloth )
{
	m_vecMesh[ipattr]->RenderGlow_Skin
	( 
		pd3dDevice, 
		arrayBoneMatrices, 
		dwNumPaletteEntries, 
		dwNumInfl, 
		sMaterialSystem,
		NULL, 
		NULL,
		pGlowTex,
		pGlowColor,
		unPass,
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL
	);
}

void DxMultiMaterialSkinMesh::RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, 
											 int ipattr,
											 const D3DXMATRIX* arrayBoneMatrices,
											 DWORD dwNumPaletteEntries,
											 DWORD dwNumInfl, 
											 const D3DXMATRIX& matViewProj,
											 SMATERIAL_PIECE& sMaterialSystem,
											 BOOL bMaterialSystemWLD,
											 const DxPhysXCloth* pPhysXCloth )
{
	DWORD dwAlphaTestEnable(0L);
	if ( m_vecType[ ipattr ] == TEX_TYPE_HARDALPHA || m_vecType[ ipattr ] == TEX_TYPE_SOFTALPHA )
	{
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	}
	//else if ( m_vecType[ ipattr ] == TEX_TYPE_SOFTALPHA )
	//{
	//	return;
	//}

	m_vecMesh[ipattr]->RenderShadow_Skin
	( 
		pd3dDevice, 
		arrayBoneMatrices, 
		dwNumPaletteEntries, 
		dwNumInfl,
		matViewProj,
		sMaterialSystem,
		bMaterialSystemWLD, 
		m_vecType[ ipattr ] == TEX_TYPE_OPAQUE,
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL
	);

	if ( m_vecType[ ipattr ] == TEX_TYPE_HARDALPHA || m_vecType[ ipattr ] == TEX_TYPE_SOFTALPHA )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
	}
}



//////////////////////////////////////////////////////////////////////////
// Render Object
void DxMultiMaterialSkinMesh::RenderObjectOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
												   int ipattr,
												   const D3DXMATRIX& matWorld, 
												   SMeshContainer *pMeshContainer,
												   SMATERIAL_PIECE& sMaterialSystem,
												   const EffectMaterialData* pEffectMaterialData,
												   LPDIRECT3DTEXTUREQ pOverlayTex,
												   const float* arrayInvWeight,
												   const D3DXMATRIX* pmatClothParentBone,
												   const DxClothColl* pClothColl,
												   const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
												   float fAttParentBoneScale,
												   float fAlpha,
												   DxPhysXCloth* pPhysXCloth )
{
	// HardAlpha, SoftAlpha�� ����ؼ� Opaque���� �ѹ��� �Ѵ�.
	m_vecMesh[ipattr]->CheckVersionObject_UpdateCloth
	( 
		pd3dDevice, 
		sMaterialSystem,
		arrayInvWeight,
		pmatClothParentBone,
		pClothColl,
		pvecClothCollSpheres,
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth(ipattr) : NULL,
		fAttParentBoneScale
	);

	// �ؽ��� �ε��� �Ϸ� �Ǿ��ٸ� ������ �������ش�.
	if ( m_vecType[ ipattr ] == TEX_TYPE_UNKNOWN )
	{
		ChangeTexType( ipattr, sMaterialSystem.m_textureRes );
	}

	if ( m_vecType[ ipattr ] == TEX_TYPE_HARDALPHA || m_vecType[ ipattr ] == TEX_TYPE_SOFTALPHA )
		return;

	if ( sMaterialSystem.m_bBaseRenderOff )
		return;

	m_vecMesh[ipattr]->RenderObject( pd3dDevice, matWorld, sMaterialSystem, pEffectMaterialData, pOverlayTex, fAlpha, pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL );
}

void DxMultiMaterialSkinMesh::RenderObjectHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
													  int ipattr,
													  const D3DXMATRIX& matWorld, 
													  SMATERIAL_PIECE& sMaterialSystem,
													  const EffectMaterialData* pEffectMaterialData,
													  LPDIRECT3DTEXTUREQ pOverlayTex,
													  float fAlpha,
													  const DxPhysXCloth* pPhysXCloth )
{
	if ( m_vecType[ ipattr ] != TEX_TYPE_HARDALPHA  )
		return;

	if ( sMaterialSystem.m_bBaseRenderOff )
		return;

	m_vecMesh[ipattr]->RenderObject( pd3dDevice,  matWorld, sMaterialSystem, pEffectMaterialData, pOverlayTex, fAlpha, pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL );
}

void DxMultiMaterialSkinMesh::RenderObjectSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
													  int ipattr,
													  const D3DXMATRIX& matWorld,
													  SMATERIAL_PIECE& sMaterialSystem,
													  const EffectMaterialData* pEffectMaterialData,
													  LPDIRECT3DTEXTUREQ pOverlayTex,
													  float fAlpha,
													  const DxPhysXCloth* pPhysXCloth )
{
	if ( m_vecType[ ipattr ] != TEX_TYPE_SOFTALPHA  )
		return;

	if ( sMaterialSystem.m_bBaseRenderOff )
		return;

	m_vecMesh[ipattr]->RenderObject( pd3dDevice, matWorld, sMaterialSystem, pEffectMaterialData, pOverlayTex, fAlpha, pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL );
}

BOOL DxMultiMaterialSkinMesh::RenderObjectGhosting( LPDIRECT3DDEVICEQ pd3dDevice, 
													int ipattr,
													const D3DXMATRIX& matWorld,
													SMATERIAL_PIECE& sMaterialSystem,
													const EffectMaterialData* pEffectMaterialData,
													LPDIRECT3DTEXTUREQ pOverlayTex,
													float fAlpha,
													const DxPhysXCloth* pPhysXCloth )
{
	if ( sMaterialSystem.m_bBaseRenderOff )
		return FALSE;

	// Ghosting ������ õ�� ������ ���� �ʴ´�.
	if ( pPhysXCloth )
	{
		if ( pPhysXCloth->IsValid_PxCloth( ipattr ) )
			return TRUE;
	}

	return m_vecMesh[ipattr]->RenderGhosting_Object( pd3dDevice, matWorld, sMaterialSystem, pEffectMaterialData, pOverlayTex, fAlpha );
}

void DxMultiMaterialSkinMesh::RenderObjectGlow( LPDIRECT3DDEVICEQ pd3dDevice, 
											   int ipattr,
											   const D3DXMATRIX& matWorld,
											   SMATERIAL_PIECE& sMaterialSystem,
											   LPDIRECT3DTEXTUREQ pGlowTex,
											   const D3DXVECTOR4* pGlowColor,
											   UINT unPass,
											   const DxPhysXCloth* pPhysXCloth )
{
	m_vecMesh[ipattr]->RenderGlow_Object
	( 
		pd3dDevice, 
		matWorld, 
		sMaterialSystem, 
		NULL, 
		pGlowTex, 
		pGlowColor, 
		unPass,
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL
	);
}

void DxMultiMaterialSkinMesh::RenderObjectShadow( LPDIRECT3DDEVICEQ pd3dDevice, 
											   int ipattr,
											   const D3DXMATRIX& matWorld, 
											   const D3DXMATRIX& matViewProj,
											   SMATERIAL_PIECE& sMaterialSystem,
											   BOOL bMaterialSystemWLD,
											   const DxPhysXCloth* pPhysXCloth )
{
	DWORD dwAlphaTestEnable(0L);
	if ( m_vecType[ ipattr ] == TEX_TYPE_HARDALPHA || m_vecType[ ipattr ] == TEX_TYPE_SOFTALPHA )
	{
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	}
	//else if ( m_vecType[ ipattr ] == TEX_TYPE_SOFTALPHA )
	//{
	//	return;
	//}

	m_vecMesh[ipattr]->RenderShadow_Object
	( 
		pd3dDevice, 
		matWorld, 
		matViewProj, 
		sMaterialSystem,
		bMaterialSystemWLD, 
		m_vecType[ ipattr ] == TEX_TYPE_OPAQUE, 
		pPhysXCloth ? pPhysXCloth->GetMaterialSkinMeshForCloth_const(ipattr) : NULL
	);

	if ( m_vecType[ ipattr ] == TEX_TYPE_HARDALPHA || m_vecType[ ipattr ] == TEX_TYPE_SOFTALPHA )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
	}
}

void DxMultiMaterialSkinMesh::Export( std::vector<boost::shared_ptr<MaterialSkinMeshForCloth>>& vecMeshForCloth )
{
	vecMeshForCloth.clear();

	for ( DWORD i=0; i<m_vecMesh.size(); ++i )
	{
		boost::shared_ptr<MaterialSkinMeshForCloth> spMeshForCloth(new MaterialSkinMeshForCloth);
		vecMeshForCloth.push_back( spMeshForCloth );
	}
}