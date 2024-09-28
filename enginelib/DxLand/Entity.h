#pragma once

#include <boost/scoped_ptr.hpp>

#include "../DxTools\DxVertexFVF.h"

class DxPiece;
class DxFrameMesh;
class DxStaticMeshLightMap;
struct CLIPVOLUME;

static float DEFAULT_VISIBLE_RADIUS = 900.f;
static float DEFAULT_INVISIBLE_RADIUS = 1000.f;

class Entity
{
protected:
	float				m_fAlpha;		// ��ŷ�� �Ǿ��� �� Alpha �� ��ȭ�Ѵ�.		// �Ź� ���ϴ� ��.

public:
	void	SetAlpha( float fAlpha );
	float	GetAlpha() const;

public:
	// �ʼ������� �־�� �� �͵�.
	virtual float				GetRadius() const		= 0;
	virtual const D3DXVECTOR3&	GetCenter() const		= 0;
	virtual const D3DXVECTOR3&	GetStaticMax() const	= 0;
	virtual const D3DXVECTOR3&	GetStaticMin() const	= 0;

	// �־ �ǰ� ��� �Ǵ� �͵�.
	virtual float GetVisibleRadius()		{ return DEFAULT_VISIBLE_RADIUS; }
	virtual float GetInvisibleRadius()		{ return DEFAULT_INVISIBLE_RADIUS; }

	virtual void FrameMoveTHREAD( const float fElapsedTime )			{}
	virtual void FrameMoveTOOL( const float fTime, const float fElapsedTime )			{}
	virtual void FrameMoveMAT( const float fTime, const float fElapsedTime )			{}
	virtual void FrameMoveMAT_OnlyStaticAniFrame( float fElapsedTime )					{}
	virtual void FrameMoveMAT_OnlyPieceEff( const float fTime, const float fElapsedTime )	{}

	// ������ DF �� �����ȴٸ� �׿� �´� �ɼǰ����� ��ȭ��Ų��.
	virtual void SetData_Force_TnL_PS_3_0_DF()												{}

	//virtual void FrameMoveRAD( BOOL bDay )												{}

	virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )											{}
	virtual void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )									{}
	virtual void RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )									{}
	virtual void RenderMS_Opaque( BOOL bMPShading, BOOL bReflect, float fHeight, BOOL bBlend )				{}
	virtual void RenderMS_Opaque_Animation( BOOL bMPShading, BOOL bReflect, float fHeight, BOOL bBlend )	{}
	virtual void RenderMS_Fixed()																			{}
	virtual void RenderMS_HardAlpha( BOOL bMPShading, BOOL bBlend )										{}
	virtual void RenderMS_HardAlpha_Animation( BOOL bMPShading, BOOL bBlend )								{}
	virtual void RenderEffFrame()																			{}
	virtual void RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect )									{}
	virtual void RenderDecal()																				{}
	virtual void RenderMS_SoftAlpha( BOOL bMPShading )														{}
	virtual void RenderMS_SoftAlphaForDeffered()															{}
	virtual void RenderMS_SoftAlphaForPSF()																	{}
	virtual void RenderMS_DecalPass2()																		{}
	virtual void Render_PIECE_Custom_UNLIT_UNFOG()															{}
	virtual void Render_PIECE_Water_CustomSoftAlpha()														{}
	virtual void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF )		{}
	virtual void RenderCascadeShadowOpaque()	{}
	virtual void RenderCascadeShadowHardAlpha()	{}
	virtual void RenderCascadeShadowReplace()	{}

	virtual void Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )	{}
	virtual void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )			{}

	virtual void SetBlendAlpha( BOOL bBlend )	{}
	virtual BOOL IsBlendAlpha() const			{ return TRUE; }
	virtual BOOL GetPickAlpha() const			{ return FALSE; }
	virtual BOOL IsVoxelColor() const			{ return TRUE; }
	virtual BOOL IsMaterialMesh() const			{ return TRUE; }

	// Piece �� ������� ��.
	virtual BOOL CheckOcclusionCulling( const VEC_DWORD* pvecDWORD ) const	{ return TRUE; }
	virtual void SetAddAlpha( float fTime, BOOL bAddAlpha )	{}
	virtual BOOL IsAddAlpha()								{ return TRUE; }

	// SkinObject �� ������� ��
	virtual void FrameMoveEntity( float fElapsedTime)							{}
	virtual void RenderSkinObject( BOOL bReflect, float fReflectHeight )		{}

public:
	Entity();
	virtual ~Entity();
};