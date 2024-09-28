// DxStaticMaterialMeshAni.h
//
//	class DxStaticMaterialMeshAni : DxStaticMaterialMesh ������ Matrix ���� ������ �ִ�.
//									Matrix ���� DxStaticMaterialMeshFrameAni �� Matrix ���� Pointer �� �����Ѵ�.
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
		VERSION = 0x0100,           // �⺻���� Version
	};

	boost::scoped_ptr<DxStaticMaterialMesh>		m_scpStaticMaterialMesh;	// �������� �ʿ��� ��.
	const D3DXMATRIX							*m_rmatCombined;			// ������ ��
	D3DXMATRIX									m_matCombined_Mirror;		// �ݻ縦 ���� ��

public:
	// Material �� ���Ѵ�.
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