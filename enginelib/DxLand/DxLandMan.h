#ifndef DXLANDMAN_H_
#define DXLANDMAN_H_

#pragma once

//	[class DxLandMan]
//
//	Need : ���ĸ� �׸���ÿ� ����ȭ ������ ����. (������), JDH
//
//	Modify : (2002.12.03), JDH, ExportProgress::SetStateString() ���� ��� while()������
//		{}�� �������� ������ ���ѷ����� ������ ���� �߻�. Sleep() �߰���.

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
	boost::shared_ptr<DxStaticMeshLightMap>			m_spStaticMeshLightMap;			// mat �� �ƴѰ�.
	boost::shared_ptr<DxStaticMaterialMeshLightMap>	m_spStaticMaterialMeshLightMap;	// mat �� ����

protected:
	DxAnimationMan*	m_pAniManHead;
	OBJAABBNode*	m_pAniManTree;

protected:
	//	Note : ġȯ ���� ����.
	DXPIECEOBJ*		m_pPieceObjHead;
	DXPIECEOBJNODE*	m_pPieceObjTree;


protected:
	DxEffectBase*	m_pEffectFrameList;	//	Frame ���� ȿ�� ����.
	DxAfterRender	m_AfterRender;		//	Frame ���� ȿ���� ���߿� �ѷ����°� ����.

	//	Note : Frame ���� ����Ʈ�� (_EFF_REPLACE) �Ӽ��� �ִ� ȿ����.
	//
	DWORD			m_dwEffectCount;
	DxEffectBase*	m_pEffectList;
	OBJAABBNode*	m_pEffectTree;

	//	Note : Frame ���� ����Ʈ�� (_EFF_REPLACE) �Ӽ��� �ִ� ȿ����.
	//
	DWORD			m_dwEffectCount_AFTER;
	DxEffectBase*	m_pEffectList_AFTER;
	OBJAABBNode*	m_pEffectTree_AFTER;

	//	Note : Frame ���� ����Ʈ�� (_EFF_REPLACE) �Ӽ��� �ִ� ȿ����.
	//
	DWORD			m_dwEffectCount_AFTER_1;
	DxEffectBase*	m_pEffectList_AFTER_1;
	OBJAABBNode*	m_pEffectTree_AFTER_1;

	//	Note : Frame ���� ����Ʈ�� <Weather> �Ӽ��� �ִ� ȿ����. <�׸���/��/�ݻ� ��> ���� ���߿� �ѷ��� �Ѵ�.
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

	BOOL			m_bUsedMaterialSystem;		// Material System �� ����ϴ� �����ΰ�~? �����̸� Render Pass �� ����Ȱ��� ����Ѵ�.
												// Load �ȰŴ� �ƴ� �͵� ���� ���̴�.
												// �׷� ���� Render Pass�� ���� �������� CPU ����� ���� ���̴�. GPU ���ϰ� �ִ°� �ƴϴ�.

	BOOL			m_bUsedMaterialSystemPS_3_0;// Material System �� ����ϴ� �����̴�.
												// ���� ps.3.0 ������ �����ϴ� ���̴�.

	DWORD			m_dwRAD_Pass;				// ���õ� ��ġ�� ����� ���´�.
	float			m_fRAD_Reflectance;			// ���õ� ��ġ�� ����� ���´�.
	float			m_fRAD_FogBlend;			// ���õ� ��ġ�� ����� ���´�.
	DWORD			m_dwRAD_FogLength;			// ���õ� ��ġ�� ����� ���´�.

	// �ǽð� �׸��ڸ� ������ �� �ִ� SpotLight ������
	boost::scoped_ptr<ShadowLightData>	m_scpShadowLightData;

	// ī�޶� �浹 üũ
	bool			m_bCamCollision;

	// ���� List
	// Thread �󿡼� �����Ѵ�.
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
	// ���� Navi �� ������ �� m_sNaviMeshCapsule ���� �Ѵ�.
	// �̰� m_pNaviMesh ���� ����� ���� Src �� ���� ��.
	//
	// Server ������ ���� NavigationMesh �� ���� ������ �ִ�.
	// ����� Client ���� �۾��� �Ѵٰ� ���� �ȴ�.
	//
private:
	class NavigationMeshCapsule
	{
	private:
		// wld �� ���Ե� Navi
		// �̰� ��������� ����.
		// ���� ��ã�⿡ ���̴� �� m_pNaviMesh �� �����.
		boost::shared_ptr<NavigationMesh>			m_spNaviMeshInWld;

		// �׺񺯰� ����� ���鼭 �߰��Ǿ���.
		// �̰� ��������� ����.
		// ���� ��ã�⿡ ���̴� �� m_pNaviMesh �� �����.
		boost::shared_ptr<NavigationMesh>			m_spNaviMeshBypass;

		//	Note : �׺���̼� �޽� ���Ἲ
		BOOL					m_bIntegrity;

	public:
		void CreateNaviMesh( DxSetLandMan* pSetLandMan, LPDIRECT3DDEVICEQ pd3dDevice, char *szDebugFullDir );
		void RenderMiniMapMode( LPDIRECT3DDEVICEQ pd3dDevice );	// �̴ϸ� �׸��� ���
		void RenderNavigationForEditVerySlow( LPDIRECT3DDEVICEQ pd3dDevice );
		boost::shared_ptr<NavigationMesh>	GetNaviMesh();
		BOOL IsNaviMeshIntegrity();
		void CleanUp();

		// �⺻������ wld ������ ������ ȣ��Ǵ� �Լ�. m_spNaviMeshInWld �� Save, Load ��.
	public:
		void	SaveForNaviMeshInWld( sc::SerialFile& SFile );
		void	LoadForNaviMeshInWld( sc::SerialFile& SFile );

		// Level Edit ���� �����ϱ� ���� �Լ�
		// Bypass �� Save �� psf ���� DxSetLandMan::SaveSetNavi �� �����Ѵ�. ( WorldEdit )
	public:
		void	LoadNaviBypassFile( const char *szFile );	// Level �� Game ���� ȣ��ȴ�.
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
	NavigationMeshCapsule	m_sNaviMeshCapsule;		// �濡 ���� ������ ���� �޽�


protected:
	//	Note : �� ����
	CStaticSoundMan*m_pStaticSoundMan;
	
protected:
	//	Note : ���̳��� �ε���
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
	// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bFrustom = TRUE );
	HRESULT Render_LOW_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );
	HRESULT Render_LOW_SingleEff( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv );

	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem );
	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight );
	// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// Deffered ��,��,�ֻ� �ɼ�	
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

	// ���� MaterialSystem �ʱ⿡ �۾��� FrameAnimation..
	// ������ MaterialSystem ������ ������ �ʵ��� ����.
	// ���� ����� �α��� ȭ�� ������ ��¿�� ���� �����ǰ� �ִ�.
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
	// Deffered ��,��,�ֻ� �ɼ�
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// CascadeShadowMap �� SpotShadowMap �����ڵ�
	void RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderShadow_SpotLight1( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderShadow_SpotLight2( LPDIRECT3DDEVICEQ pd3dDevice );
	// CascadeShadowMap �� SpotShadowMap �����ڵ�
	//////////////////////////////////////////////////////////////////////////



	// �ǽð� �ݻ� ��ġ�� ���Ѵ�.
	// ���� FLT_MAX �� ���� �۵����� �ʴ´�.
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
	BOOL LoadFile_VER115 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	//	������ ���� ���� ����
	BOOL LoadFile_VER117 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	//	�ʿ� ī�޶� �浹 ��� ��� �߰�, ī�޶� �浹 ���͸��� �߰�
    BOOL LoadFile_VER118 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// LightMap Piece ���� ������.
    BOOL LoadFile_VER119 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// PointLight ���� ������.
	BOOL LoadFile_VER120 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// DxStaticMeshLightMap �߰���.
	BOOL LoadFile_VER121 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// m_spStaticMaterialMeshLightMap �߰���.
	BOOL LoadFile_VER122_123 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName, BOOL bForceIndoor );	// Animation �� Mesh ���忡 ������ �־ ������.

private:
	DWORD EffectLoadToList_Dummy( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile );
	void EffectLoadToList_ModifyTileColor( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, DWORD dwEffectLoadToListStartMark );

public:
	DxLandMan ();
	~DxLandMan();		

	//	Note : �浹 ó�� �Լ���.
public:

	// �ܼ� �浹 �ߴ��� �� �ߴ��� Ȯ�ο�.
	BOOL IsCollisionSimple( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, EMCOLLMODE emCullMode ) const;

	// 
	void IsCollision( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						LPDXFRAME &pDxFrame, EMCOLLMODE emCullMode, BOOL bOnlyCamColl = FALSE ) const;

	//  ���� Octree�� �� �Լ��� ������� �� ��.  Piece �浹��.
	void IsCollisionNEW( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						EMCOLLMODE emCullMode, const BOOL bPiece, BOOL bOnlyCamColl = FALSE );

	void IsCollisionEX ( const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
							LPDXFRAME &pDxFrame, LPCSTR& szName, D3DXVECTOR3 *vNormal = NULL, BOOL bOnlyCamColl = FALSE );

	void GetCollisionLineInTriListExceptY  ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE>  &pTriList ); // �ֺ��� �߰� �޼ҵ�

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEndSRC, DWORD& dwColor );

	void CalculateActorWorldLight_Thread( D3DXVECTOR3 &vDiffuse OUT, D3DXVECTOR3 &vPointLight OUT, const D3DXVECTOR3 &vPos ) const;

	BOOL IsLightMap();
	BOOL IsStaticMesh()			{ return !(!m_spStaticMeshLightMap.get()); } // �ֺ��� �߰� �޼ҵ�
	BOOL IsStaticMaterialMesh() { return m_spStaticMaterialMeshLightMap.get() ? TRUE : FALSE; }
	//BOOL IsStaticMesh() { return !(!m_pStaticMesh); } // �ֺ��� �߰� �޼ҵ�

	// Navigation ��ȸ ��� ���� �Լ�
public:
	void LoadNaviBypassFile( const char *szFile );
	void ClearNaviBypassFile();

	// Land Thread ����
public:
	boost::shared_ptr<EntityRenderListSwap>* GetArrayEntityRenderListSwap();

	// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
	void SetNSLandThread();

	// 
public:
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );

	// WorldBattle ( ���籺�� )
public:
	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );
};

#endif // DXLANDMAN_H_