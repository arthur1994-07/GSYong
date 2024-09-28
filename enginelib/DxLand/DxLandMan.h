#ifndef DXLANDMAN_H_
#define DXLANDMAN_H_

#pragma once

//	[class DxLandMan]
//
//	Need : 알파멥 그리기시에 최적화 여지가 있음. (수정요), JDH
//
//	Modify : (2002.12.03), JDH, ExportProgress::SetStateString() 에서 대기 while()문에서
//		{}가 공백으로 있을때 무한루프에 빠지는 버그 발생. Sleep() 추가함.

#include "../../SigmaCore/File/BaseStream.h"

#include "./CollisionMap.h"
#include "./NSLandThreadDef.h"
#include "./DxLandDef.h"
#include "./DxLandEff.h"
#include "./DxLandGateMan.h"
#include "./DxLandSkinObj.h"
#include "./DxOctree/DxOctree.h"

#include "../DxTools/Collision.h"
#include "../DxTools/DxMethods.h"
#include "../DxTools/Light/DxLightMan.h"
#include "../DxTools/DxWeatherMan.h"
#include "../DxTools/DxCamAniMan.h"
#include "../DxTools/DxFogMan.h"
#include "../DxTools/DxSkyMan.h"
#include "..//DxSound/BgmSoundData.h"
#include "../DxEffect/Frame/DxEffectRender.h"
#include "../G-Logic/GLDefine.h"
#include "../DxMeshs/FrameMesh/DxAnimationManager.h"

struct	DXPIECEOBJ;
struct	DXPIECEOBJNODE;
struct	ShadowLightData;
class	DxDecal;
class	SUBPATH;
class	NavigationMesh;
class	DxSetLandMan;
class	CStaticSoundMan;
class   DxTextureEffMan;
class   DxPieceManagerWLD;
class   DxStaticMesh;
class	DxStaticMeshLightMap;
class	DxStaticMaterialMeshLightMap;
class	EntityRenderListSwap;
class	DxLandSkinManager;


namespace	ExportProgress
{
	extern	int		EndPos;
	extern	int		CurPos;
	extern	BOOL	bValid;	
	extern	char	szState[MAX_PATH];

	void InitialCritical ();
	void DeleteCritical ();

	void GetStateString ( char *szString );
	void SetStateString ( const char *szString );
}

class DxLandMan
{
public:
	const static DWORD	OLDVERSION;
	const static DWORD	VERSION;
	const static char	FILEMARK[128];
	static char			m_szPath[MAX_PATH];

public:
	static void		SetPath ( char* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	static char*	GetPath ()					{ return m_szPath; }

private:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

private:
	TSTRING			m_strNameWLD;
	TSTRING			m_strMakeTimeWLD;

public:
	const TSTRING&	GetNameWLD()		{ return m_strNameWLD; }
	const TSTRING&	GetMakeTimeWLD()	{ return m_strMakeTimeWLD; }

protected:
	SNATIVEID		m_MapID;
	char			m_szMapName[MAXLANDNAME];

	SLAND_FILEMARK	m_sFILEMARK;

public:
	SNATIVEID GetMapID ()	{ return m_MapID; }
	void	  SetMapID ( SNATIVEID mapID ) { m_MapID = mapID; }

protected:
	DxOctree		m_DxOctree;

protected:
	boost::shared_ptr<DxStaticMeshLightMap>			m_spStaticMeshLightMap;			// mat 이 아닌것.
	boost::shared_ptr<DxStaticMaterialMeshLightMap>	m_spStaticMaterialMeshLightMap;	// mat 을 쓴것

protected:
	DxAnimationMan*	m_pAniManHead;
	OBJAABBNode*	m_pAniManTree;

protected:
	//	Note : 치환 조각 관리.
	DXPIECEOBJ*		m_pPieceObjHead;
	DXPIECEOBJNODE*	m_pPieceObjTree;


protected:
	DxEffectBase*	m_pEffectFrameList;	//	Frame 종속 효과 관리.
	DxAfterRender	m_AfterRender;		//	Frame 종속 효과중 나중에 뿌려지는것 관리.

	//	Note : Frame 접합 이펙트중 (_EFF_REPLACE) 속성이 있는 효과들.
	//
	DWORD			m_dwEffectCount;
	DxEffectBase*	m_pEffectList;
	OBJAABBNode*	m_pEffectTree;

	//	Note : Frame 접합 이펙트중 (_EFF_REPLACE) 속성이 있는 효과들.
	//
	DWORD			m_dwEffectCount_AFTER;
	DxEffectBase*	m_pEffectList_AFTER;
	OBJAABBNode*	m_pEffectTree_AFTER;

	//	Note : Frame 접합 이펙트중 (_EFF_REPLACE) 속성이 있는 효과들.
	//
	DWORD			m_dwEffectCount_AFTER_1;
	DxEffectBase*	m_pEffectList_AFTER_1;
	OBJAABBNode*	m_pEffectTree_AFTER_1;

	//	Note : Frame 접합 이펙트중 <Weather> 속성이 있는 효과들. <그림자/물/반사 등> 보다 나중에 뿌려야 한다.
	DWORD			m_dwCount_Weather;
	DxEffectBase*	m_pList_Weather;
	OBJAABBNode*	m_pTree_Weather;
	
	DxLightMan*		m_pLightMan;
	DxWeatherMan	m_WeatherMan;
	DxCamAniMan		m_CameraAniMan;
	DxLandGateMan	m_LandGateMan;

	CCollisionMap	m_CollisionMap;

	FOG_PROPERTY	m_FOG_PROPERTY;
	SKY_PROPERTY	m_SKY_PROPERTY;
	SBGMDATA		m_BGMDATA;

	boost::shared_ptr<DxPieceManagerWLD>	m_spPieceManager;
	boost::shared_ptr<DxLandSkinManager>	m_spLandSkinManager;

	BOOL			m_bUsedMaterialSystem;		// Material System 을 사용하는 버젼인가~? 버젼이면 Render Pass 가 변경된것을 사용한다.
												// Load 된거는 아닌 것도 있을 것이다.
												// 그런 것은 Render Pass로 인해 쓸데없는 CPU 계산이 있을 것이다. GPU 부하가 있는건 아니다.

	BOOL			m_bUsedMaterialSystemPS_3_0;// Material System 을 사용하는 버젼이다.
												// 또한 ps.3.0 버젼을 지원하는 맵이다.

	DWORD			m_dwRAD_Pass;				// 셋팅된 수치를 기록해 놓는다.
	float			m_fRAD_Reflectance;			// 셋팅된 수치를 기록해 놓는다.
	float			m_fRAD_FogBlend;			// 셋팅된 수치를 기록해 놓는다.
	DWORD			m_dwRAD_FogLength;			// 셋팅된 수치를 기록해 놓는다.

	// 실시간 그림자를 생성할 수 있는 SpotLight 정보들
	boost::scoped_ptr<ShadowLightData>	m_scpShadowLightData;

	// 카메라 충돌 체크
	bool			m_bCamCollision;

	// 렌더 List
	// Thread 상에서 공유한다.
	boost::shared_ptr<EntityRenderListSwap>	m_spEntityRenderListSwap[NSLandThread::TYPE_CULL_MODE_SIZE];

public:
	void SetFogProperty ( FOG_PROPERTY& Property );
	FOG_PROPERTY& GetFogProperty ()		{ return m_FOG_PROPERTY; }

	void SetSkyProperty ( SKY_PROPERTY& Property );
	SKY_PROPERTY& GetSkyProperty ()		{ return m_SKY_PROPERTY; }

public:
	DxCamAniMan*	GetCamAniMan ()		{ return &m_CameraAniMan; }
	DxLandGateMan*	GetLandGateMan ()	{ return &m_LandGateMan; }
	DxAfterRender*	GetAfterRender ()	{ return &m_AfterRender; }
    DXPIECEOBJ*	    GetPieceObject ()	{ return m_pPieceObjHead; }
    CStaticSoundMan* GetStaticSoundMan () { return m_pStaticSoundMan; }    
    DxLightMan*     GetLightMan ()		{ return m_pLightMan; }

	void SetCamCollision( const bool bColl )	{ m_bCamCollision = bColl; }
	bool GetCamCollision()						{ return m_bCamCollision; }
	BOOL GetUsedMaterialSystem() const			{ return m_bUsedMaterialSystem; }
	BOOL GetUsedMaterialSystemPS_3_0() const	{ return m_bUsedMaterialSystemPS_3_0; }
	DWORD GetRAD_Pass() const					{ return m_dwRAD_Pass; }
	float GetRAD_Reflectance() const			{ return m_fRAD_Reflectance; }
	float GetRAD_FogBlend() const				{ return m_fRAD_FogBlend; }
	DWORD GetRAD_FogLength() const				{ return m_dwRAD_FogLength; }

	//////////////////////////////////////////////////////////////////////////
	// 실제 Navi 를 돌리는 건 m_sNaviMeshCapsule 에서 한다.
	// 이건 m_pNaviMesh 값을 만들기 위한 Src 로 보면 됨.
	//
	// Server 에서는 따로 NavigationMesh 의 값을 가지고 있다.
	// 여기는 Client 관련 작업만 한다고 보면 된다.
	//
private:
	class NavigationMeshCapsule
	{
	private:
		// wld 에 포함된 Navi
		// 이걸 사용하지는 않음.
		// 실제 길찾기에 쓰이는 건 m_pNaviMesh 를 사용함.
		boost::shared_ptr<NavigationMesh>			m_spNaviMeshInWld;

		// 네비변경 기능이 들어가면서 추가되었음.
		// 이걸 사용하지는 않음.
		// 실제 길찾기에 쓰이는 건 m_pNaviMesh 를 사용함.
		boost::shared_ptr<NavigationMesh>			m_spNaviMeshBypass;

		//	Note : 네비게이션 메시 무결성
		BOOL					m_bIntegrity;

	public:
		void CreateNaviMesh( DxSetLandMan* pSetLandMan, LPDIRECT3DDEVICEQ pd3dDevice, char *szDebugFullDir );
		void RenderMiniMapMode( LPDIRECT3DDEVICEQ pd3dDevice );	// 미니맵 그릴때 사용
		void RenderNavigationForEditVerySlow( LPDIRECT3DDEVICEQ pd3dDevice );
		boost::shared_ptr<NavigationMesh>	GetNaviMesh();
		BOOL IsNaviMeshIntegrity();
		void CleanUp();

		// 기본적으로 wld 파일을 읽으면 호출되는 함수. m_spNaviMeshInWld 만 Save, Load 함.
	public:
		void	SaveForNaviMeshInWld( sc::SerialFile& SFile );
		void	LoadForNaviMeshInWld( sc::SerialFile& SFile );

		// Level Edit 에서 셋팅하기 위한 함수
		// Bypass 의 Save 는 psf 상의 DxSetLandMan::SaveSetNavi 로 직접한다. ( WorldEdit )
	public:
		void	LoadNaviBypassFile( const char *szFile );	// Level 및 Game 에서 호출된다.
		void	ClearNaviBypassFile();

	public:
		NavigationMeshCapsule()
			: m_bIntegrity(TRUE)
		{
		}
		~NavigationMeshCapsule()	{}
	};
	//
	//////////////////////////////////////////////////////////////////////////

private:
	NavigationMeshCapsule	m_sNaviMeshCapsule;		// 길에 대한 정보를 가진 메쉬


protected:
	//	Note : 맵 사운드
	CStaticSoundMan*m_pStaticSoundMan;
	
protected:
	//	Note : 다이나믹 로딩용
	sc::SerialFile		m_SFileMap;

public:
	sc::SerialFile&	GetSerialFile()		{ return m_SFileMap; }

protected:
	//	Note : Single Effect
	//
	DWORD			m_dwNumLandEff;
	PLANDEFF		m_pLandEffList;
	OBJAABBNode*	m_pLandEffTree;

public:
	void SetBgmFile ( CString strBGM )	{ m_BGMDATA.SetFile ( strBGM ); }
	const CString& GetBgmFile ()		{ return m_BGMDATA.strFileName; }

public:
	DWORD GetNumLandEff ()				{ return m_dwNumLandEff; }
	PLANDEFF GetLandEffList ()			{ return m_pLandEffList; }

	void AddLandEff ( PLANDEFF pLandEff );
	void DelLandEff ( PLANDEFF pLandEff );
	PLANDEFF FindLandEff ( char* szName );

	void BuildSingleEffTree ();

public:
	DxOctree*							GetOctree    ()			{ return &m_DxOctree;   }
	DxStaticMeshLightMap*				GetStaticMesh()			{ return m_spStaticMeshLightMap.get(); }
	DxStaticMaterialMeshLightMap*		GetStaticMaterialMesh()	{ return m_spStaticMaterialMeshLightMap.get(); }
	//DxStaticMesh*						GetStaticMesh()			{ return m_pStaticMesh; }
	DxPieceManagerWLD*					GetPieceManager()		{ return m_spPieceManager.get(); }
	boost::shared_ptr<NavigationMesh>	GetNaviMesh();

protected:
	BOOL AddPieceObj ( DXPIECEOBJ *pNewPieceObj );
	void AddEffectFrameList ( DxEffectBase* pEffect );

protected:
	HRESULT EffectAdaptToFrame ( DxOctree &OcNode, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
					PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc, DxTextureEffMan* pTexEff );

	HRESULT EffectAdaptToList ( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
					PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc, 
                    DxTextureEffMan* pTexEff, const TSTRING& strLightMapName, VEC_LIGHTMAPUV_MAIN& vecLightMapUV_MAIN );

	void SwitchWeatherEffect( PDXEFFECTBASE& pSrc, DWORD& dwSrc, PDXEFFECTBASE& pDest, DWORD& dwDest );

public:
	static HRESULT EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, PDXEFFECTBASE &pEffList, BOOL bDummy=FALSE );
	static HRESULT EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, PDXEFFECTBASE &pEffList, PDXEFFECTBASE &pEffList_AFTER, 
									DWORD& dwCount, DWORD& dwCount_AFTER, BOOL bDummy );

public:
	void MakeMiniMap( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxSetLandMan* pSetLandMan,
						D3DXVECTOR2& vMin, 
						D3DXVECTOR2& vMax, 
						DWORD dwTexSize, 
						const TCHAR* pName );

	HRESULT Import_Ran_1_0( int MaxTriangles, 
							int MaxSubdivisions, 
							DxSetLandMan *pSetLandMan, 
							LPDIRECT3DDEVICEQ pd3dDevice, 
							char *szDebugFullDir, 
							BOOL bCreatLightMap,
							const TSTRING& strLightMapName,
							float fLightMapDetail,
							const SUBPATH* pPath );

	HRESULT Import( int MaxTriangles, 
					BOOL bOneCell, 
					DxSetLandMan *pSetLandMan, 
					LPDIRECT3DDEVICEQ pd3dDevice, 
					char *szDebugFullDir, 
					const SUBPATH* pPath,
					BOOL bCreatLightMap,
					const TSTRING& strLightMapName,
					BOOL bRadiosity,
					int nRadiosityPass,
					float fRadiosityReflectance,
					float fRadiosityFogBlend,
					DWORD dwRadiosityFogLength );

private:
	void ActiveMapIn ();

public:
	HRESULT ActiveMap ( boost::shared_ptr<DxLandMan>& spDxLandManMe );
	HRESULT ActiveMap_Tool( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<DxLandMan>& spDxLandManMe );

public:
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);

	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();	

protected:
	HRESULT DrawAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, BOOL bReflect=FALSE );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime, const CLIPVOLUME &cv, BOOL bNotRendering );



	//////////////////////////////////////////////////////////////////////////
	// TnL_PS_1_1 이하에서만 호출 된다.
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bFrustom = TRUE );
	HRESULT Render_LOW_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );
	HRESULT Render_LOW_SingleEff( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );

	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem );
	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight );
	// TnL_PS_1_1 이하에서만 호출 된다.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// Deffered 중,상,최상 옵션	
	HRESULT Render_Opaque_SM30DF( LPDIRECT3DDEVICEQ pd3dDevice, 
									const CLIPVOLUME &cv, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									BOOL bReflect, 
									float fReflectHeight, 
									LPDIRECT3DTEXTUREQ pColorTex_1st,
									LPDIRECT3DTEXTUREQ pColorTex_2nd, 
									LPDIRECT3DTEXTUREQ pColorTex_3rd,
									LPDIRECT3DTEXTUREQ pColorTex_4th,
									const D3DXMATRIX& matWorld, 
									const D3DXMATRIX& matView,
									const D3DXMATRIX& matProj,
									BOOL bMaterialFrameMove );

	// 이전 MaterialSystem 초기에 작업된 FrameAnimation..
	// 현재의 MaterialSystem 에서는 사용되지 않도록 하자.
	// 지금 현재는 로그인 화면 때문에 어쩔수 없이 유지되고 있다.
	void RenderMS_FrameAnimation_Will_Erase( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );

	HRESULT RenderDecal_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									BOOL bReflect, 
									float fReflectHeight, 
									const D3DXMATRIX& matWorld );

	HRESULT RenderWater_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	HRESULT Render_SoftAlpha_SM30DF( LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									BOOL bReflect, 
									float fReflectHeight, 
									const D3DXMATRIX& matWorld );

	HRESULT RenderEff_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );

	void RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
							BOOL bReflect, 
							float fReflectHeight, 
							const D3DXMATRIX& matWorld );

	void RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									BOOL bReflect, 
									float fReflectHeight, 
									const D3DXMATRIX& matWorld );

	void Render_Opaque_Spot_LBuffer( LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	void Render_SoftAlpha_Spot_LBuffer( LPDIRECT3DDEVICEQ pd3dDevice, 
										NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	void RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice, 
							NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
							BOOL bReflect );
	// Deffered 중,상,최상 옵션
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// CascadeShadowMap 및 SpotShadowMap 생성코드
	void RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderShadow_SpotLight1( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderShadow_SpotLight2( LPDIRECT3DDEVICEQ pd3dDevice );
	// CascadeShadowMap 및 SpotShadowMap 생성코드
	//////////////////////////////////////////////////////////////////////////



	// 실시간 반사 위치를 구한다.
	// 값이 FLT_MAX 일 경우는 작동되지 않는다.
	float GetRealTimeWaterHeight();

	HRESULT	Render_ReflectOLD( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &pCV, float fHeight, const D3DXMATRIX& matReflectVP );
	HRESULT	Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );

	//HRESULT Render_NOSKIN ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );

	void	RenderStarGlow( LPDIRECT3DDEVICEQ pd3dDevice );
    void    Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice );
	void	Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFieldOcclusion, BOOL bPieceOcclusion, DWORD dwCullMode );

	// Radiosity Render
	DWORD Render_Radiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
							const D3DXVECTOR3& vPos, 
							const D3DXVECTOR3& vNor, 
							LPDIRECT3DTEXTUREQ pTextureSystem, 
							float fReflectance,
							float fFogBlend,
							DWORD dwFogLength,
							BOOL bDay );

	HRESULT Render_Radiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
								CLIPVOLUME &cv,
								const D3DXMATRIX& matView,
								const D3DXMATRIX& matProj,
								BOOL bDay,
								const D3DCOLOR& colorClear,
								DWORD dwWidth,
								DWORD dwHeight,
								LPDIRECT3DSURFACEQ pColorSuf_Main,
								LPDIRECT3DSURFACEQ pColorSuf_1st,
								LPDIRECT3DSURFACEQ pColorSuf_2nd,
								LPDIRECT3DSURFACEQ pColorSuf_3rd,
								LPDIRECT3DTEXTUREQ pColorTex_1st,
								LPDIRECT3DTEXTUREQ pColorTex_2nd, 
								LPDIRECT3DTEXTUREQ pColorTex_3rd,
								BOOL bReflect,
								float fReflectHeight,
								const D3DXMATRIX& matWorld,
								BOOL bSky,
								NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType );

	void	GetClipVolumeRAD( CLIPVOLUME& sOUT, D3DXMATRIX& matViewOUT, D3DXMATRIX& matProjOUT, D3DXVECTOR3& vLookatPtOUT, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vNor );

	void	RenderNavigationForEditVerySlow( LPDIRECT3DDEVICEQ pd3dDevice );

	void	PieceSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt );
	void	CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	void	DestroyQuery();
	void	ResetOcclusionQuery();
	void	FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex, VEC_DWORD& vecPieceIndex );

	void	ClipStaticMesh( DxDecal* pDecal );

public:
	void CleanUp ();

public:
	BOOL IsNaviMeshIntegrity();

	void ExportXFileForLandSkin( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strSkinName, const TSTRING& strTexName );

public:
	BOOL SaveFile ( LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile, boost::function<void (const TCHAR*)> pArchive = 0 );
	BOOL SaveFileOld ( LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile, boost::function<void (const TCHAR*)> pArchive = 0 );
	//BOOL SavePieceFile ( const char *szFile );

public:
	BOOL LoadFile ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad = FALSE );
	BOOL LoadBasicPos( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vPos );
	BOOL LoadBasicPos( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );

protected:
	BOOL LoadFile_VER108 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad );
	BOOL LoadFile_VER109 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );
	BOOL LoadFile_VER110 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );
	BOOL LoadFile_VER112 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// Ver 111~112
	BOOL LoadFile_VER114 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// Ver 113~114
	BOOL LoadFile_VER115 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	//	데이터 포맷 변경 진행
	BOOL LoadFile_VER117 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	//	맵에 카메라 충돌 사용 기능 추가, 카메라 충돌 메터리얼 추가
    BOOL LoadFile_VER118 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// LightMap Piece 관련 수정함.
    BOOL LoadFile_VER119 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// PointLight 관련 수정함.
	BOOL LoadFile_VER120 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// DxStaticMeshLightMap 추가함.
	BOOL LoadFile_VER121 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// m_spStaticMaterialMeshLightMap 추가함.
	BOOL LoadFile_VER122_123 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName, BOOL bForceIndoor );	// Animation 쪽 Mesh 저장에 문제가 있어서 수정함.

private:
	DWORD EffectLoadToList_Dummy( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile );
	void EffectLoadToList_ModifyTileColor( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, DWORD dwEffectLoadToListStartMark );

public:
	DxLandMan ();
	~DxLandMan();		

	//	Note : 충돌 처리 함수들.
public:

	// 단순 충돌 했는지 안 했는지 확인용.
	BOOL IsCollisionSimple( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, EMCOLLMODE emCullMode ) const;

	// 
	void IsCollision( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						LPDXFRAME &pDxFrame, EMCOLLMODE emCullMode, BOOL bOnlyCamColl = FALSE ) const;

	//  예전 Octree는 이 함수를 사용하지 못 함.  Piece 충돌함.
	void IsCollisionNEW( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						EMCOLLMODE emCullMode, const BOOL bPiece, BOOL bOnlyCamColl = FALSE );

	void IsCollisionEX ( const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
							LPDXFRAME &pDxFrame, LPCSTR& szName, D3DXVECTOR3 *vNormal = NULL, BOOL bOnlyCamColl = FALSE );

	void GetCollisionLineInTriListExceptY  ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE>  &pTriList ); // 주병진 추가 메소드

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEndSRC, DWORD& dwColor );

	void CalculateActorWorldLight_Thread( D3DXVECTOR3 &vDiffuse OUT, D3DXVECTOR3 &vPointLight OUT, const D3DXVECTOR3 &vPos ) const;

	BOOL IsLightMap();
	BOOL IsStaticMesh()			{ return !(!m_spStaticMeshLightMap.get()); } // 주병진 추가 메소드
	BOOL IsStaticMaterialMesh() { return m_spStaticMaterialMeshLightMap.get() ? TRUE : FALSE; }
	//BOOL IsStaticMesh() { return !(!m_pStaticMesh); } // 주병진 추가 메소드

	// Navigation 우회 사용 관련 함수
public:
	void LoadNaviBypassFile( const char *szFile );
	void ClearNaviBypassFile();

	// Land Thread 관련
public:
	boost::shared_ptr<EntityRenderListSwap>* GetArrayEntityRenderListSwap();

	// 로그인 화면 같은 경우 다중으로 맵이 로딩이 되어 있고, 맵 전환이 되기 때문에 NSLandThread 정보값도 변경이 되야한다.
	void SetNSLandThread();

	// 
public:
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// WorldBattle ( 난사군도 )
public:
	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );
};

#endif // DXLANDMAN_H_