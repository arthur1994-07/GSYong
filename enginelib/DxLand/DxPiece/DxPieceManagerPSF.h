#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <vector>
#include <map>
#include <string>

#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxTools/DxCustomTypes.h"
#include "../../DxTools/DxVertexFVF.h"
#include "./DxPieceManager.h"
#include "./DxPieceDef.h"

struct DxFrame;
class DxFrameMesh;
class DxStaticMeshLightMap;
class DxPiece;
class DxPieceAABB;
struct DxPieceNode;
class DxPieceEdit;
class DxPieceQuickSort;
class DxPrefabMan;
class DxSetLandMan;
class DxStaticMeshCollBase;

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		M	a	n	a	g	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
class DxPieceManagerPSF : public DxPieceManager
{
private:
	static const DWORD VERSION_PSF;

public:
    enum EDITMODE
	{
		MODE_INSERT = 0,
		MODE_EDIT = 1,
	};	

	static EDITMODE	g_emEDITMODE;

protected:
	float m_fTime;
	float m_fElapsedTime;

public:
	struct MATRIXPISNAME
	{
		enum
		{
			VERSION = 0x0101,
		};

		D3DXMATRIX	m_matFrame;		// 자신의 Matrix	// FramaName, Piece 에서 필요한 것. m_mapChangePis 에서만 쓰이고 m_vecPointPis 에서는 쓰이지 않는다.
		char*		m_pPisName;
		DxPiece*	m_pPiece;

		void Save( sc::SerialFile& SFile );
		void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
		void Load_Prev( sc::SerialFile& SFile );

		void CleanUp();

		MATRIXPISNAME();
		~MATRIXPISNAME();
	};

protected:
	// FrameName, Piece
    typedef std::tr1::unordered_map<std::string, MATRIXPISNAME*> MAPCHANGEPIS;
	typedef MAPCHANGEPIS::iterator                               MAPCHANGEPIS_ITER;
    typedef MAPCHANGEPIS::value_type                             MAPCHANGEPIS_VALUE;

	MAPCHANGEPIS	m_mapChangePis;

	// Point, Piece
	typedef std::vector<MATRIXPISNAME*>		VECPOINTPIS;
	typedef VECPOINTPIS::iterator			VECPOINTPIS_ITER;
	VECPOINTPIS	m_vecPointPis;

	// PieceName, PieceAABB
    typedef std::tr1::unordered_map<std::string, DxPieceAABB*> MAPNAMEPIECE;
	typedef MAPNAMEPIECE::iterator				               MAPNAMEPIECE_ITER;
    typedef MAPNAMEPIECE::value_type			               MAPNAMEPIECE_VALUE;
	MAPNAMEPIECE	m_mapNamePiece;

protected:
	DxPiece*	m_pPiecePick;	// Pick Piece

	BOOL		m_bUseLightMap;	// LightMap 으로 작동되는가~?

	bool		m_bFrameMoveOnce;	// frameMove 는 한번만 되도록 ...

public:
	DWORD GetSize()		{ return (DWORD)(m_mapChangePis.size()+m_vecPointPis.size()); }
	BOOL IsGetName( DWORD nNUM, LPCSTR& pPieceName, LPCSTR& pFrameName );
	void SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh, const char* szFrameName, const char* szPisName );
	void DeletePiece( DxFrameMesh* pFrameMesh, const char* szFrameName );
	BOOL SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, const char* szPisName, const D3DXMATRIX& matWorld, DxSetLandMan* pSetLandMan, BOOL bSlope, float fSlopeRate );
	BOOL SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, const char* szPisName, const D3DXMATRIX& matWorld );
	void DeletePiece( DWORD dwNumber );
	D3DXMATRIX* MoveCamera( DxFrame* pFrameRoot, const char* szFrameName );
	D3DXMATRIX* MoveCamera( DWORD dwNumber );
    DxPiece* GetPiece( DWORD dwNumber );                                // m_vecPointPis 여기서 
    DxPieceManagerPSF::MATRIXPISNAME*	GetMATRIXPISNAME( DWORD dwNumber );
    DWORD GetPointPieceSize()    { return static_cast< DWORD >( m_vecPointPis.size() ); }	
	DxPieceManagerPSF::MATRIXPISNAME*	GetChangeMATRIXPISNAME( DWORD dwNumber );
	DWORD GetChangePieceSize()    { return static_cast< DWORD >( m_mapChangePis.size() ); }
	void SortPointPis();

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision ) const;

    BOOL IsCollisionLine2( const D3DXVECTOR3& vStart, 
						      D3DXVECTOR3& vEnd, 
							  D3DXVECTOR3& vNor,
						      D3DXVECTOR3& vCollision, 
						      DxPiece** rrPiece,
                              EMCOLLMODE emCullMode ) const;						// 형상 - <Edit> Piece를 얻기위함.

    // Note : 충돌이 되면 바로 리턴한다.
    virtual BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

    void ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

    int GetIndex( DxPiece* pPiece );

    void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

	void Clone_MouseShift( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX& pMatrix );

	void ChangeNamePIE15();

	void ResetAnimationTime_DisableLoopType();

	// Prefab
public:
	void MakeTree( const D3DXMATRIX& matWorld );
	void SetPrefabMatrix( const D3DXMATRIX& matPrefab );
	void Import( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceManagerPSF* pSrc, DxPrefabMan* pPrefabMan );
	void ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece );

	// Etc
protected:
	void MakeTree();		// Note : Render를 하기위해 구조 변환 및 Tree 생성\

private:
	friend class DxPieceManagerWLD;
    friend class CPieceManagerEdit;
    friend class CPiecePointEdit;
    friend class CWorldEditView;

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bPSF, float fLightMapDetail, BOOL bFrameMove );
	void RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render_DecalPass2_WaterCustomSoftAlpha();
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh , BOOL bPSF, float fLightMapDetail );
	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF, BOOL bFrameMove );
	void RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice );

	//void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );

    HRESULT DrawWireFrame ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV );        //	editer 용. 

	void CheckPieceForRadiosity( VEC_TSTRING& vecErrorPiece );

	// TriangleLinkLight
public:
	// Piece의 TriangleLinkLight 를 생성하도록 한다.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh,
									SET_DWORD& setEraseLightID,
									BOOL bDirectionLight );

	//  현재 라이트와 연관된 삼각형들의 특정 슬롯에 자리가 남는지 확인.
	BOOL IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const;

	// 현재 라이트와 연관된 삼각형들의 특정 슬롯에 현재 LightID 를 셋팅.
	void InsertSlotLightID( DWORD dwLightID, DWORD dwSlot );

	// Triangle Link Light - Light가 3개 이상인 것 찾기 및 Light 갯수 2개로 제한
	void EraseLightID4LightOver3( SET_DWORD& setEraseLightID );

	// setEraseLightID 을 바탕으로 Piece 쪽 LightID도 삭제 하도록 한다.
	void EraseLightID4TriangleLinkLight( const SET_DWORD& setEraseLightID );

	// Piece 의 PS_3_0의 렌더타입을 설정하도록 한다.
	void SetType_PS_3_0();

private:
	// 특정 LightID 를 삭제하도록 한다.
	void EraseLightID( DWORD dwLightID );

public:
	void SavePSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh );
	void LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh );
	void LoadPSF_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh );

	void CleanUp();

public:
    void Archive(boost::function<void (const TCHAR*)> fun);

public:
	DxPieceManagerPSF();
	virtual ~DxPieceManagerPSF();
};