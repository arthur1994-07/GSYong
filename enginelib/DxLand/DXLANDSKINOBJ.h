#pragma once
#include "../DxTools/DxCustomTypes.h"
#include "../DxTools/Collision.h"

#include "../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../DxMeshs/SkinMesh/DxSkinCharData.h"

#include "./LightMap/LightMapBoxColor.h"
#include "./Entity.h"

class sc::SerialFile;
class LightMapBoxColor;
class LightMapBoxColorFullData;
class DxStaticMeshCollBase;
class DxPieceManager;

class DXLANDSKINOBJ : public OBJAABB, public Entity
{
public:
	enum
	{
		VERSION				= 0x0104,
		LANDSKINOBJ_MAXSZ	= 33,
	};

public:
	static float		m_fTime;
	static float		m_fElapsedTime;

	BOOL				m_bRender;

	static HRESULT FrameMove ( float fTime, float fElapsedTime )
	{
		m_fTime = fTime;
		m_fElapsedTime = fElapsedTime;
		return S_OK;
	}

public:
	BOOL				m_bOnlyFixedForMaterial;		// ���� Export �Ǵ� ���� SkinObject�� ������ Fixed �θ� ���۵Ǵ°�~?
														// MAP �� MaterialSystem �� �Ϸ�Ǿ�����, SkinObject �� �̿ϼ��̶� �̰� �ʿ���.

	BOOL				m_bLodBlending;	// Lod Blending �����ϴ°�~?

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	char				m_szName[LANDSKINOBJ_MAXSZ];
	char				m_szFileName[MAX_PATH];			//	

	float				m_fRadius;
	D3DXVECTOR3			m_vCenter;

	D3DXVECTOR3			m_vMax;							//	AABB�� ���.
	D3DXVECTOR3			m_vMaxOrg;						//	AABB�� ���.
	
	D3DXVECTOR3			m_vMin;							//	AABB�� ���.
	D3DXVECTOR3			m_vMinOrg;						//	AABB�� ���.

	D3DXMATRIX			m_matWorld;

	DxSkinCharData*		m_pSkinCharData;				//	��Ų ĳ���� Data.
	DxSkinChar*			m_pSkinChar;					//	��Ų ĳ����.

	DXLANDSKINOBJ*		m_pNext;

	// LightMapBoxColor
private:
	D3DXVECTOR4					m_vVoxelColorUsed;				// ���Ǵ� VoxelColor
	D3DXVECTOR4					m_avAmbientCube[LightMapBoxColor::TEST_COUNT];
	LightMapBoxColor*			m_pLightMapBoxColor;	// ����Ʈ�� or Radiosity�� ���� �� ��� ���̰� �����.
	LightMapBoxColorFullData*	m_pLightMapBoxColorFullData;	// ���� ������ ������ ����.

	// LOD ���� ������
private:	
	float               m_fVisibleRadius;   // Piece�� alpha�� 1�� �Ǵ� �ݰ�.
	float               m_fInvisibleRadius; // Piece�� alpha�� 0�� �Ǵ� �ݰ�.

public:
	friend class CLandSkinObj;

	// LightMapBoxColor
public:
	void LightMapCalculationBoxColor( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshCollBase* pStaticMesh, DxPieceManager* pPieceManager );
	void AddRadiosityColorForVoxel( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, LPDIRECT3DTEXTUREQ pTextureSystem, float fReflectance, float fFogBlend, DWORD dwFogLength );

public:
	void SetPosition( const D3DXVECTOR3& vPos )	{ m_matWorld._41 = vPos.x; m_matWorld._42 = vPos.y; m_matWorld._43 = vPos.z; }
	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );

protected:
	HRESULT PlaySkinChar ( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetAABBBOX( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	HRESULT Import ( const DXLANDSKINOBJ* pSrc, LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT SetSkinChar ( const char* szName, const D3DXMATRIX& matWorld, const char* szFile, LPDIRECT3DDEVICE9 pd3dDevice );
	DxSkinChar* GetSkinChar ()		{ return m_pSkinChar; }

public:
	HRESULT Load ( sc::SerialFile	&SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Save ( sc::SerialFile	&SFile );

	//	Note : OBJAABB
	//
public:
	void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		vMax = m_vMax; vMin = m_vMin;
	}

	BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
		return COLLISION::IsWithInPoint( vDivMax, vDivMin, vPos );
	}

	OBJAABB* GetNext ()		{ return m_pNext; }

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();

public:
	virtual void InsertData ( const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength );
	virtual void InsertDataForShadow();
	virtual void	FrameMove( const float fElapsedTime );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, BOOL bReflect, float fReflectHeight );
	HRESULT RenderName ( LPDIRECT3DDEVICEQ pd3dDevice );
	void ExportXFile( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strFrameName, const TSTRING& strTexName );

private:
	void Render2( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight, BOOL bPointLightRT );

	// virtual Set
public:

	// virtual Get
public:
	virtual float				GetRadius() const		{ return m_fRadius; }
	virtual const D3DXVECTOR3&	GetCenter() const		{ return m_vCenter; }
	virtual const D3DXVECTOR3&	GetStaticMax() const	{ return m_vMax; }
	virtual const D3DXVECTOR3&	GetStaticMin() const	{ return m_vMin; }

public:
	virtual float GetVisibleRadius()		{ return m_fVisibleRadius; }
	virtual float GetInvisibleRadius()		{ return m_fInvisibleRadius; }

public:
	void FrameMoveEntity( float fElapsedTime) override;
	void RenderSkinObject( BOOL bReflect, float fReflectHeight ) override;

public:
	DXLANDSKINOBJ( LPDIRECT3DDEVICE9 pd3dDevice );
	~DXLANDSKINOBJ(void);
};

typedef DXLANDSKINOBJ* PLANDSKINOBJ;







class DxLandSkinManager
{
private:
	//	Note : Skin Object
	//
	DWORD			m_dwNumLandSkinObj;
	PLANDSKINOBJ	m_pLandSkinObjList;
	OBJAABBNode*	m_pLandSkinObjTree;

public:
	//void FrameMove( float fElapsedTime, const CLIPVOLUME &cv, float fRealTimeWaterHeight );

	//void RenderAABBTree( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );
	//void RenderSkinObject( BOOL bReflect, float fReflectHeight, BOOL bFrameMove );
	//void RenderAABBTreeReflect( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, float fRealTimeWaterHeight );
	//void Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );
	//void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );

	//////////////////////////////////////////////////////////////////////////
	// ��ü �� ȣ��ȴ�.
	void InsertQuickSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend, BOOL bReflect );
	void InsertQuickSortForShadow( const CLIPVOLUME &sCV );
	//void InsertQuickSortPSF( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend );
	// ��ü �� ȣ��ȴ�.
	//////////////////////////////////////////////////////////////////////////

public:
	DWORD GetNumSkinObj ()				{ return m_dwNumLandSkinObj; }
	PLANDSKINOBJ GetLandSkinObjList ()	{ return m_pLandSkinObjList; }

	void AddLandSkinObj ( PLANDSKINOBJ pLandSkinObj );
	void DelLandSkinObj ( PLANDSKINOBJ pLandSkinObj );
	PLANDSKINOBJ FindLandSkinObj ( char* szName );
	PLANDSKINOBJ FindLandSkinObjByFileName ( const char* szFileName );

	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );

	void BuildSkinObjTree ();

	void Import( LPDIRECT3DDEVICEQ pd3dDevice, const DXLANDSKINOBJ* pCurSkinObj );

	void ExportXFile( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strFrameName, const TSTRING& strTexName );

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	void SaveFile( sc::SerialFile& SFile );
	void LoadFile( sc::SerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	void CleanUp();

public:
	DxLandSkinManager();
	~DxLandSkinManager();
};