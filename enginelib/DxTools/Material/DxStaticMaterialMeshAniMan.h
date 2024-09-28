// DxStaticMaterialMeshAniMan.h
//
//	class DxStaticMaterialMeshAniMan : DxStaticMaterialMeshAni 를 vector 형식으로 가지고 있음.
//
//	Name : Sung-Hwan Han
//	Begin :2013/07/09
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../../../SigmaCore/File/SerialFile.h"

struct DxMeshes;
struct CLIPVOLUME;
struct PieceParameter;
class CMaterialSetting;
class DxStaticMaterialMeshAni;


class DxStaticMaterialMeshAniMan
{
private:
	enum
	{
		VERSION = 0x0100,           // 기본적인 Version
	};

private:
	typedef std::vector<std::tr1::shared_ptr<DxStaticMaterialMeshAni>>	VEC_SPSTATICMATERIALMESHANI;
	typedef VEC_SPSTATICMATERIALMESHANI::iterator						VEC_SPSTATICMATERIALMESHANI_ITER;

	VEC_SPSTATICMATERIALMESHANI		m_vecspStaticMaterialMeshAni;


public:
	// Material 을 구한다.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );


public:
	DWORD Create( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, const CMaterialSetting *pMaterialSetting, D3DXMATRIX *pmatCombined, float fCellSize );

	BOOL IsExist() const		{ return m_vecspStaticMaterialMeshAni.empty() ? FALSE : TRUE; }

	void SetLinkMatrix( DWORD dwID, const D3DXMATRIX* pmatCombined );

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

	void CleanUp();

public:
	void Save ( sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );
	void Load ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bPiece, boost::function<void (const TCHAR*)> pArchive );

public:
	DxStaticMaterialMeshAniMan();
	~DxStaticMaterialMeshAniMan();
};