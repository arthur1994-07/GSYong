#include "pch.h"

#include "../../DxLand/DxPiece/DxPieceDef.h"
#include "./DxStaticMaterialMeshAni.h"

#include "DxStaticMaterialMeshAniMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	t	a	t	i	c		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMaterialMeshAniMan::DxStaticMaterialMeshAniMan()
{
}

DxStaticMaterialMeshAniMan::~DxStaticMaterialMeshAniMan()
{
	CleanUp();
}

void DxStaticMaterialMeshAniMan::CleanUp()
{
	m_vecspStaticMaterialMeshAni.clear();
}

DWORD DxStaticMaterialMeshAniMan::Create( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, const CMaterialSetting *pMaterialSetting, D3DXMATRIX *pmatCombined, float fCellSize )
{
	std::tr1::shared_ptr<DxStaticMaterialMeshAni> spStaticMaterialMeshAni = std::tr1::shared_ptr<DxStaticMaterialMeshAni>(new DxStaticMaterialMeshAni);
	spStaticMaterialMeshAni->Import( pd3dDevice, pmsMeshs, pMaterialSetting, pmatCombined, fCellSize );

	m_vecspStaticMaterialMeshAni.push_back( spStaticMaterialMeshAni );

	return m_vecspStaticMaterialMeshAni.size() - 1;
}

void DxStaticMaterialMeshAniMan::SetLinkMatrix( DWORD dwID, const D3DXMATRIX* pmatCombined )
{
	if ( dwID >= m_vecspStaticMaterialMeshAni.size() )
		return;

	m_vecspStaticMaterialMeshAni[dwID]->SetLinkMatrix( pmatCombined );
}

void DxStaticMaterialMeshAniMan::RenderOrderSettingPiece( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool, DWORD& dwTexType )
{
	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->RenderOrderSettingPiece( pd3dDevice, bTool, dwTexType );
	}
}

void DxStaticMaterialMeshAniMan::Render_PIECE_( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	const D3DXMATRIX *pmatWorldPrev = sParameter.m_rmatWorld;

	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_( sParameter, bMPShading, bBlend );
	}

	sParameter.m_rmatWorld = pmatWorldPrev;
}

void DxStaticMaterialMeshAniMan::Render_PIECE_Alpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend )
{
	const D3DXMATRIX *pmatWorldPrev = sParameter.m_rmatWorld;

	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_Alpha( sParameter, bMPShading, bBlend );
	}

	sParameter.m_rmatWorld = pmatWorldPrev;
}

void DxStaticMaterialMeshAniMan::Render_PIECE_Decal( PieceParameter& sParameter )
{
	const D3DXMATRIX *pmatWorldPrev = sParameter.m_rmatWorld;

	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_Decal( sParameter );
	}

	sParameter.m_rmatWorld = pmatWorldPrev;
}

void DxStaticMaterialMeshAniMan::Render_PIECE_SoftAlpha( PieceParameter& sParameter, BOOL bMPShading )
{
	const D3DXMATRIX *pmatWorldPrev = sParameter.m_rmatWorld;

	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_SoftAlpha( sParameter, bMPShading );
	}

	sParameter.m_rmatWorld = pmatWorldPrev;
}

void DxStaticMaterialMeshAniMan::Render_PIECE_SoftAlphaForPSF( PieceParameter& sParameter )
{
	const D3DXMATRIX *pmatWorldPrev = sParameter.m_rmatWorld;

	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_SoftAlphaForPSF( sParameter );
	}

	sParameter.m_rmatWorld = pmatWorldPrev;
}

void DxStaticMaterialMeshAniMan::Render_PIECE_DecalPass2( PieceParameter& sParameter )
{
	const D3DXMATRIX *pmatWorldPrev = sParameter.m_rmatWorld;

	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_DecalPass2( sParameter );
	}

	sParameter.m_rmatWorld = pmatWorldPrev;
}

void DxStaticMaterialMeshAniMan::Render_PIECE_Custom_UNLIT_UNFOG( PieceParameter& sParameter )
{
	const D3DXMATRIX *pmatWorldPrev = sParameter.m_rmatWorld;

	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_Custom_UNLIT_UNFOG( sParameter );
	}

	sParameter.m_rmatWorld = pmatWorldPrev;
}

void DxStaticMaterialMeshAniMan::Render_PIECE_Water_CustomSoftAlpha( PieceParameter& sParameter )
{
	const D3DXMATRIX *pmatWorldPrev = sParameter.m_rmatWorld;

	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_Water_CustomSoftAlpha( sParameter );
	}

	sParameter.m_rmatWorld = pmatWorldPrev;
}

void DxStaticMaterialMeshAniMan::Render_PIECE_CascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_CascadeShadowOpaque( pd3dDevice );
	}
}

void DxStaticMaterialMeshAniMan::Render_PIECE_CascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Render_PIECE_CascadeShadowHardAlpha( pd3dDevice );
	}
}

void DxStaticMaterialMeshAniMan::Save ( sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile << static_cast<DWORD>( m_vecspStaticMaterialMeshAni.size() );
		for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
		{
			m_vecspStaticMaterialMeshAni[i]->Save( SFile, bPiece, pArchive );
		}
	}
	SFile.EndBlock();
}

void DxStaticMaterialMeshAniMan::Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive )
{
	DWORD dwVer, dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		std::tr1::shared_ptr<DxStaticMaterialMeshAni> spStaticMaterialMeshAni = std::tr1::shared_ptr<DxStaticMaterialMeshAni>(new DxStaticMaterialMeshAni);
		spStaticMaterialMeshAni->Load( pd3dDevice, SFile, bPiece, pArchive );
		m_vecspStaticMaterialMeshAni.push_back( spStaticMaterialMeshAni );
	}
}

// Material 을 구한다.
void DxStaticMaterialMeshAniMan::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	for ( DWORD i=0; i<m_vecspStaticMaterialMeshAni.size(); ++i )
	{
		m_vecspStaticMaterialMeshAni[i]->Active_MATERIAL_DATA( pd3dDevice, bTool );
	}
}