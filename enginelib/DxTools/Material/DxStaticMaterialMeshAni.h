// DxStaticMaterialMeshAni.h
//
//	class DxStaticMaterialMeshAni : DxStaticMaterialMesh 정보과 Matrix 값을 가지고 있다.
//									Matrix 값은 DxStaticMaterialMeshFrameAni 의 Matrix 값을 Pointer 로 참조한다.
//
//	Name : Sung-Hwan Han
//	Begin :2013/07/09
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <boost/scoped_ptr.hpp>

#include "../../../SigmaCore/File/SerialFile.h"

struct DxMeshes;
struct CLIPVOLUME;
struct PieceParameter;
class CMaterialSetting;
class DxStaticMaterialMesh;


class DxStaticMaterialMeshAni
{
private:
	enum
	{
		VERSION = 0x0100,           // 기본적인 Version
	};

	boost::scoped_ptr<DxStaticMaterialMesh>		m_scpStaticMaterialMesh;	// 렌더링에 필요한 값.
	const D3DXMATRIX							*m_rmatCombined;			// 참조한 값
	D3DXMATRIX									m_matCombined_Mirror;		// 반사를 위한 값

public:
	// Material 을 구한다.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

public:
	void Import( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, const CMaterialSetting *pMaterialSetting, const D3DXMATRIX *pmatCombined, float fCellSize );

	void SetLinkMatrix( const D3DXMATRIX* pmatCombined );

	void RenderOrderSettingPiece( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool, DWORD& dwTexType );
	void Render_PIECE_( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void Render_PIECE_Alpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void Render_PIECE_Decal( PieceParameter& sParameter );
	void Render_PIECE_SoftAlpha( PieceParameter& sParameter, BOOL bMPShading );
	void Render_PIECE_SoftAlphaForPSF( PieceParameter& sParameter );
	void Render_PIECE_DecalPass2( PieceParameter& sParameter );
	void Render_PIECE_Custom_UNLIT_UNFOG( PieceParameter& sParameter );
	void Render_PIECE_Water_CustomSoftAlpha( PieceParameter& sParameter );
	void Render_PIECE_CascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void Render_PIECE_CascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Save ( sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );

public:
	DxStaticMaterialMeshAni();
	~DxStaticMaterialMeshAni();
};