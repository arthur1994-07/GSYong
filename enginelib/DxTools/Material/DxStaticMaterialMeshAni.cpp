#include "pch.h"

#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../DxLand/DxPiece/DxPieceDef.h"
#include "./NSShadowRenderForMaterial.h"
#include "./DxStaticMaterialMesh.h"

#include "DxStaticMaterialMeshAni.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	t	a	t	i	c		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMaterialMeshAni::DxStaticMaterialMeshAni()
	: m_rmatCombined(NULL)
	, m_scpStaticMaterialMesh(new DxStaticMaterialMesh)
{
}

DxStaticMaterialMeshAni::~DxStaticMaterialMeshAni()
{
}

void DxStaticMaterialMeshAni::Import( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, const CMaterialSetting* pMaterialSetting, const D3DXMATRIX* pmatCombined, float fCellSize )
{
	m_rmatCombined = pmatCombined;
	std::tr1::shared_ptr<DxStaticMesh> spStaticMesh = std::tr1::shared_ptr<DxStaticMesh>(new DxStaticMesh);
	spStaticMesh->MakeAABBOCTree( pd3dDevice, pmsMeshs, pMaterialSetting, fCellSize );
	m_scpStaticMaterialMesh->Import( pd3dDevice, spStaticMesh.get(), fCellSize );
}

void DxStaticMaterialMeshAni::SetLinkMatrix( const D3DXMATRIX* pmatCombined )
{
	m_rmatCombined = pmatCombined;
}

void DxStaticMaterialMeshAni::RenderOrderSettingPiece( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool, DWORD& dwTexType )
{
	m_scpStaticMaterialMesh->RenderOrderSettingPiece( pd3dDevice, bTool, dwTexType );
}

void DxStaticMaterialMeshAni::Render_PIECE_( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	sParameter.m_rmatWorld = m_rmatCombined;
	m_scpStaticMaterialMesh->Render_PIECE_( sParameter, bMPShading, bBlend );
}

void DxStaticMaterialMeshAni::Render_PIECE_Alpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	sParameter.m_rmatWorld = m_rmatCombined;
	m_scpStaticMaterialMesh->Render_PIECE_Alpha( sParameter, bMPShading, bBlend );
}

void DxStaticMaterialMeshAni::Render_PIECE_Decal( PieceParameter& sParameter )
{
	sParameter.m_rmatWorld = m_rmatCombined;
	m_scpStaticMaterialMesh->Render_PIECE_Decal( sParameter );
}

void DxStaticMaterialMeshAni::Render_PIECE_SoftAlpha( PieceParameter& sParameter, BOOL bMPShading )
{
	sParameter.m_rmatWorld = m_rmatCombined;
	m_scpStaticMaterialMesh->Render_PIECE_SoftAlpha( sParameter, bMPShading );
}

void DxStaticMaterialMeshAni::Render_PIECE_SoftAlphaForPSF( PieceParameter& sParameter )
{
	sParameter.m_rmatWorld = m_rmatCombined;
	m_scpStaticMaterialMesh->Render_PIECE_SoftAlphaForPSF( sParameter );
}

void DxStaticMaterialMeshAni::Render_PIECE_DecalPass2( PieceParameter& sParameter )
{
	sParameter.m_rmatWorld = m_rmatCombined;
	m_scpStaticMaterialMesh->Render_PIECE_DecalPass2( sParameter );
}

void DxStaticMaterialMeshAni::Render_PIECE_Custom_UNLIT_UNFOG( PieceParameter& sParameter )
{
	sParameter.m_rmatWorld = m_rmatCombined;
	m_scpStaticMaterialMesh->Render_PIECE_Custom_UNLIT_UNFOG( sParameter );
}

void DxStaticMaterialMeshAni::Render_PIECE_Water_CustomSoftAlpha( PieceParameter& sParameter )
{
	sParameter.m_rmatWorld = m_rmatCombined;
	m_scpStaticMaterialMesh->Render_PIECE_Water_CustomSoftAlpha( sParameter );
}

void DxStaticMaterialMeshAni::Render_PIECE_CascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//sParameter.m_rmatWorld = m_rmatCombined;
	NSShadowRenderForMaterial::SetWorld( *m_rmatCombined );
	NSShadowRenderForMaterial::CommitChanges();
	m_scpStaticMaterialMesh->Render_PIECE_CascadeShadowOpaque( pd3dDevice );
}

void DxStaticMaterialMeshAni::Render_PIECE_CascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//sParameter.m_rmatWorld = m_rmatCombined;
	NSShadowRenderForMaterial::SetWorld( *m_rmatCombined );
	NSShadowRenderForMaterial::CommitChanges();
	m_scpStaticMaterialMesh->Render_PIECE_CascadeShadowHardAlpha( pd3dDevice );
}

void DxStaticMaterialMeshAni::Save ( sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		m_scpStaticMaterialMesh->Save( SFile, bPiece, pArchive );
	}
	SFile.EndBlock();
}

void DxStaticMaterialMeshAni:: Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	DWORD dwVer, dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	m_scpStaticMaterialMesh->Load( pd3dDevice, SFile, FALSE, bPiece, pArchive );
}

// Material 을 구한다.
void DxStaticMaterialMeshAni::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	m_scpStaticMaterialMesh->Active_MATERIAL_DATA( pd3dDevice, bTool );
}