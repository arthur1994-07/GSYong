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

		D3DXMATRIX	m_matFrame;		// �ڽ��� Matrix	// FramaName, Piece ���� �ʿ��� ��. m_mapChangePis ������ ���̰� m_vecPointPis ������ ������ �ʴ´�.
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

	BOOL		m_bUseLightMap;	// LightMap ���� �۵��Ǵ°�~?

	bool		m_bFrameMoveOnce;	// frameMove �� �ѹ��� �ǵ��� ...

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
    DxPiece* GetPiece( DWORD dwNumber );                                // m_vecPointPis ���⼭ 
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
                              EMCOLLMODE emCullMode ) const;						// ���� - <Edit> Piece�� �������.

    // Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
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
	void MakeTree();		// Note : Render�� �ϱ����� ���� ��ȯ �� Tree ����\

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

    HRESULT DrawWireFrame ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV );        //	editer ��. 

	void CheckPieceForRadiosity( VEC_TSTRING& vecErrorPiece );

	// TriangleLinkLight
public:
	// Piece�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh,
									SET_DWORD& setEraseLightID,
									BOOL bDirectionLight );

	//  ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� �ڸ��� ������ Ȯ��.
	BOOL IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const;

	// ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� ���� LightID �� ����.
	void InsertSlotLightID( DWORD dwLightID, DWORD dwSlot );

	// Triangle Link Light - Light�� 3�� �̻��� �� ã�� �� Light ���� 2���� ����
	void EraseLightID4LightOver3( SET_DWORD& setEraseLightID );

	// setEraseLightID �� �������� Piece �� LightID�� ���� �ϵ��� �Ѵ�.
	void EraseLightID4TriangleLinkLight( const SET_DWORD& setEraseLightID );

	// Piece �� PS_3_0�� ����Ÿ���� �����ϵ��� �Ѵ�.
	void SetType_PS_3_0();

private:
	// Ư�� LightID �� �����ϵ��� �Ѵ�.
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