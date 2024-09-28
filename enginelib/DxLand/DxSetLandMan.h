#ifndef DXSETLANDMAN_H_
#define DXSETLANDMAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxLandDef.h"
#include "./DxLandEff.h"
#include "./DxLandGateMan.h"
#include "./DxLandSkinObj.h"
#include "./CollisionMap.h"

#include "../DxTools/DxCamAniMan.h"
#include "../DxTools/DxFogMan.h"
#include "../DxTools/Light/DxLightMan.h"
#include "../DxTools/DxSkyMan.h"
#include "../DxTools/DxWeatherMan.h"
#include "..//DxSound/BgmSoundData.h"
#include "..//DxSound/StaticSoundMan.h"
#include "../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../NaviMesh/NavigationMesh.h"
#include "./DxPiece/DxPieceDef.h"

class DxPieceManagerPSF;
class DxPieceView;
class DxPrefabMan;
struct ShadowLightData;


class DxSetLandMan
{
public:
	enum
	{
		VERSION			= 0x00000033,
		VERSION_PREFAB	= 0x100,
	};

protected:
	BOOL			m_bPrevSaveFile_120712;	// 120712 이전에 저장된 Save 파일
	TSTRING			m_strFileName;			// 현재는 prefab 에서만 사용되고 있음.
	SNATIVEID		m_MapID;
	char			m_szMapName[MAXLANDNAME];
    TSTRING         m_strLightMapName;  // LightMap Name
    float           m_fLightMapDetail;  // LightMap Detail

	DxFrameMesh*	m_pFrameMesh;

	LPDXFRAME		m_pNaviFrame;
	NavigationMesh	m_NaviMesh;

	CStaticSoundMan m_StaticSoundMan;

	DWORD			m_dwNumLandEff;
	PLANDEFF		m_pLandEffList;
	OBJAABBNode*	m_pLandEffTree;

	DWORD			m_dwNumLandSkinObj;
	PLANDSKINOBJ	m_pLandSkinObjList;
	OBJAABBNode*	m_pLandSkinObjTree;

	DxLightMan*		m_pLightMan;
	DxWeatherMan	m_WeatherMan;
	DxCamAniMan		m_CameraAniMan;
	DxLandGateMan	m_LandGateMan;

	CCollisionMap	m_CollisionMap;

	FOG_PROPERTY	m_FOG_PROPERTY;
	SKY_PROPERTY	m_SKY_PROPERTY;
	SBGMDATA		m_BGMDATA;

	DxPieceManagerPSF*	m_pPieceManager;
    DxPieceView*		m_pPieceView;

	boost::scoped_ptr<ShadowLightData>	m_scpShadowLightData;

	// Prefab
private:
	boost::scoped_ptr<DxPrefabMan>	m_scpPrefabMan;
	BOOL			m_bPrefab;

public:
	BOOL IsPrefab()									{ return m_bPrefab; }
	const ShadowLightData* GetShadowLightData()		{ return m_scpShadowLightData.get(); }
	ShadowLightData* GetShadowLightData_NoneConst()	{ return m_scpShadowLightData.get(); }

	// Etc
public:
	void SetFogProperty ( FOG_PROPERTY& Property );
	FOG_PROPERTY& GetFogProperty ()		{ return m_FOG_PROPERTY; }

	void SetSkyProperty ( SKY_PROPERTY& Property );
	SKY_PROPERTY& GetSkyProperty ()		{ return m_SKY_PROPERTY; }

public:
	void SetNaviFrame ( DxFrame *pNaviMesh );
	void GetNaviFrame ( DxFrame **pNaviMesh ) { *pNaviMesh = m_pNaviFrame; }

	BOOL CreateNaviMesh ( LPDIRECT3DDEVICE9 pd3dDevice );

public:
	void Clone_MouseShift( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX& pMatrix );
	void SetObjRotate90();

public:
	SNATIVEID GetMapID ()				{ return m_MapID; }
	char* GetMapName ()					{ return m_szMapName; }
	const TSTRING& GetFileName()		{ return m_strFileName; }
	DxFrameMesh* GetFrameMesh ()		{ return m_pFrameMesh; }
	CStaticSoundMan* GetStaticSoundMan(){ return &m_StaticSoundMan; }
	DxLightMan* GetLightMan ()			{ return m_pLightMan; }
	DxWeatherMan* GetWeatherMan ()		{ return &m_WeatherMan; }
	DxCamAniMan* GetCameraAniMan ()		{ return &m_CameraAniMan; }
	DxLandGateMan* GetLandGateMan ()	{ return &m_LandGateMan; }
	NavigationMesh* GetNaviMesh ()		{ return &m_NaviMesh; }
	CCollisionMap* GetCollisionMap ()	{ return &m_CollisionMap; }
	SBGMDATA* GetBGMDATA ()				{ return &m_BGMDATA; }
	DxPieceManagerPSF*	GetPieceManager()	{ return m_pPieceManager; }
    DxPieceView*	GetPieceView()			{ return m_pPieceView; }
	DxPrefabMan*	GetPrefabMan()			{ return m_scpPrefabMan.get(); }

public:
	void SetMapID ( SNATIVEID &MapID )	{ m_MapID = MapID; }

public:
	DWORD GetNumLandEff ()				{ return m_dwNumLandEff; }
	PLANDEFF GetLandEffList ()			{ return m_pLandEffList; }

	void AddLandEff ( PLANDEFF pLandEff );
	void DelLandEff ( PLANDEFF pLandEff );
	PLANDEFF FindLandEff( char* szName );
	PLANDEFF FindLandEff( D3DXVECTOR3& vCenter, D3DXVECTOR3& vTarget );
	void ReverseListLandEff();

	void BuildSingleEffTree ();

public:
	DWORD GetNumSkinObj ()				{ return m_dwNumLandSkinObj; }
	PLANDSKINOBJ GetLandSkinObjList ()	{ return m_pLandSkinObjList; }

	void AddLandSkinObj ( PLANDSKINOBJ pLandSkinObj );
	void DelLandSkinObj ( PLANDSKINOBJ pLandSkinObj );
	PLANDSKINOBJ FindLandSkinObj ( char* szName );

	void BuildSkinObjTree ();

    void SetLightMapInfo( const TSTRING& strLightMapName )
    {
        m_strLightMapName = strLightMapName;
    }

    const TSTRING&  GetLightMapName()   { return m_strLightMapName; }
    float           GetLightMapDetail() { return m_fLightMapDetail; }

public:
	HRESULT ActiveMap ();

public:
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	virtual HRESULT CleanUp( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		InvalidateDeviceObjects( pd3dDevice );

		DeleteDeviceObjects();

		FinalCleanup();

		return S_OK;
	}

public:
	friend class CFramePage;

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pmatPrefab, BOOL bFrameMove );
	void	RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice );
	void	RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderSoftAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pmatPrefab, BOOL bFrameMove, BOOL bInverseOrder );
	HRESULT Render_DecalPass2_WaterCustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFrameMove );
	void	RenderStarGlow( LPDIRECT3DDEVICEQ pd3dDevice );
    void    Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT DrawWireFrame ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    // Effect만 검사한다.
	BOOL IsCollisionLineEffect( 
		const D3DXVECTOR3& vStart, 
		D3DXVECTOR3& vEnd,
		POBJAABB& pOBJ );

	// 충돌체크
	BOOL IsCollisionLine_NoneConst( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vNor, EMCOLLMODE emCullMode );

	// 충돌체크
	BOOL IsCollisionLineExceptPiece( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vNor, EMCOLLMODE emCullMode, DxPiece* pExceptPiece );

public:
	void LoadSet ( LPDIRECT3DDEVICEQ pd3dDevice, const char	*pFileName, const SUBPATH* pPath );
	void SaveSet ( LPDIRECT3DDEVICEQ pd3dDevice, const char	*pFileName, boost::function<void (const TCHAR*)> pArchive = 0 );    

	void LoadSetPrefab( const TCHAR	*pFileName, LPDIRECT3DDEVICEQ pd3dDevice );
	void SaveSetPrefab( const TCHAR	*pFileName, LPDIRECT3DDEVICEQ pd3dDevice );

	void SaveSetNavi( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR *pFileName, const TCHAR* szDebugFullDir );

	void ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece );

	void CheckPieceForRadiosity( VEC_TSTRING& vecErrorPiece );

	void ChangeNamePIE15();

public:
	DxSetLandMan( BOOL bPrefab );
	~DxSetLandMan(void);
};

#endif // DXSETLANDMAN_H_