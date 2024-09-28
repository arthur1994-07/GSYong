#pragma once

struct PieceParameter;
class DxStaticAniFrame;
class DxPieceInstancingForShadow;

//class CPieceInstancing
//{
//private:
//	typedef std::map< DxStaticAniFrame*, std::tr1::shared_ptr<DxPieceInstancingForShadow> >	MAP_PIECE_INSTANCING;
//	typedef MAP_PIECE_INSTANCING::iterator											MAP_PIECE_INSTANCING_ITER;
//
//	MAP_PIECE_INSTANCING	m_mapPieceInstancing;
//
//public:
//	void CleanUp();
//
//	void Init( DxStaticAniFrame* pStaticAniFrame, const PieceParameter* pParameter );
//
//	void Insert( DxStaticAniFrame* pStaticAniFrame, const PieceParameter* pParameter );
//	void CreateVB( LPDIRECT3DDEVICEQ pd3dDevice );
//
//	void Reset();
//	void InsertDataToVB();
//	void RenderCascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
//	void RenderCascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
//	void RenderCascadeShadowOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice );
//	void RenderCascadeShadowHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice );
//
//	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
//	void OnLostDevice();
//
//public:
//	CPieceInstancing();
//	~CPieceInstancing();
//};