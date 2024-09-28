#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include "FramePage.h"
#include "LightPage.h"
#include "DirectionEdit.h"
#include "WeatherPage.h"
#include "FogPage.h"
#include "MapEditPage.h"
#include "StaticMeshCreate.h"

#include "TilingEffEdit.h"
#include "ClothEffEdit.h"
#include "Dot3EffEdit.h"
#include "RiverEffEdit.h"
#include "ShadowEffEdit.h"
#include "FurEffEdit.h"
#include "RainPointEffEdit.h"
#include "GrassEffEdit.h"
#include "FrameSateEdit.h"
#include "ReplaceFramePage.h"
#include "NatureEffEdit.h"
#include "WaterLightEffEdit.h"
#include "RenderStateEffEdit.h"
#include "CameraPage.h"
#include "WorldSoundPage.h"
#include "SingleEffPage.h"
#include "NaturePage.h"
#include "SporeEffEdit.h"
#include "LandSkinObj.h"
#include "LandGatePage.h"
#include "SpecularEffEdit.h"
#include "Specular2EffEdit.h"
#include "ReflectEffEdit.h"
#include "BgmEdit.h"
#include "MultiTexEffEdit.h"
#include "NeonEffEdit.h"
#include "SpecReflectEffEdit.h"
#include "LightMapEffEdit.h"
#include "GlowEffEdit.h"
#include "PieceSingleEffEdit.h"
#include "PieceTexEffEdit.h"
#include "PieceManagerEdit.h"
#include "PieceMorphEdit.h"
#include "./PiecePage/PiecePointEdit.h"
#include "./PiecePage/PieceMeshStateEdit.h"
#include "CamcollPage.h"
#include "./EditPage/SkyBoxManPage.h"
#include "./OcclusionPage/OcclusionPage.h"
#include "WaterSurfPage.h"
#include "MiniMapPage.h"
#include "./PiecePage/PrefabEdit.h"
#include "./CubeMapPage/CubeMapPage.h"
#include "ShadowLightPage.h"
#include "ClothForMaterialEffEdit.h"

class DxPiece;


enum EDITSHEET
{
	FRAMEPAGEINDEX			= 0,
	TILINGPAGEINDEX			= 1,
	CLOTHPAGEINDEX			= 2,
	LIGHTPAGEINDEX			= 3,
	DOT3PAGEINDEX			= 4,
	RIVERPAGEINDEX			= 5,
	SHADOWPAGEINDEX			= 6,
	FURPAGEINDEX			= 7,
	SPECULARPAGEINDEX		= 8,
	RAINPOINTPAGEINDEX		= 9,
	GRASSPAGEINDEX			= 10,
	SPECULAR2PAGEINDEX		= 11,
	DIRECTIONPAGEINDEX		= 12,
	FRAMESTATEPAGEINDEX		= 13,
	REPLACEFRAMEPAGEINDEX	= 14,
	WEATHERPAGEINDEX		= 15,
	NATUREPAGEINDEX			= 16,
	WATERLIGHTPAGEINDEX		= 17,
	RENDERSTATEPAGEINDEX	= 18,
	CAMERAPAGEINDEX			= 19,
	SOUNDPAGEINDEX			= 20,
	SINGLEEFFPAGEINDEX		= 21,
	NATUREINDEX				= 22,
	SPOREPAGEINDEX			= 23,
	SKINOBJPAGEINDEX		= 24,
	LANDGATEPAGEINDEX		= 25,
	RELFECTPAGEINDEX		= 26,
	BGMEDITPAGEINDEX		= 27,
	MULTITEXPAGEINDEX		= 28,
	NEONPAGEINDEX			= 29,
	SPECREFLECTPAGEINDEX	= 30,
	LIGHTMAPPAGEINDEX		= 31,
	GLOWPAGEINDEX			= 32,
	FOGPAGEINDEX			= 33,
	MAPEDITPAGEINDEX		= 34,
	STATICMESHCREATEINDEX	= 35,
	PIECESINGLEEFFINDEX		= 36,
	PIECETEXEFFINDEX		= 37,
	PIECEMANAGERINDEX		= 38,
	PIECEMORPHINDEX			= 39,
    PIECEPOINTPAGEINDEX     = 40,
	PIECEMESHSTATEPAGEINDEX = 41,
	CAMCOLLPAGEINDEX	    = 42,
    SKYBOXMANINDEX			= 43,
	WATERSURFPAGEINDEX		= 44,
	MINIMAPPAGEINDEX		= 45,
	OCCLUSIONPAGEINDEX		= 46,
	PREFABPAGEINDEX			= 47,
	CUBEMAPPAGEINDEX		= 48,
	SHADOWLIGHTINDEX		= 49,
	CLOTH4MATERIALINDEX	= 50,

	EDITSHEET_NUM,
};

extern CString g_strEditSheet[EDITSHEET_NUM];

//class DxEffectWater;
//class DxEffectWater2;

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*			m_pWndParent;

public:
	CFramePage			m_FramePage;
	CTilingEffEdit		m_TilingPage;
	CClothEffEdit		m_ClothPage;
	CLightPage			m_LightPage;
	CDot3EffEdit		m_Dot3Page;
	CRiverEffEdit		m_RiverPage;
	CShadowEffEdit		m_ShadowPage;
	CFurEffEdit			m_FurPage;
	CSpecularEffEdit	m_SpecularPage;
	CRainPointEffEdit	m_RainPointPage;
	CGrassEffEdit		m_GrassPage;
	CSpecular2EffEdit	m_Specular2Page;
	DirectionEdit		m_DirectPage;
	CFrameSateEdit		m_FrameStatePage;
	CReplaceFramePage	m_ReplaceFramePage;
	CWeatherPage		m_WeatherPage;
	CNatureEffEdit		m_NaturePage;
	CWaterLightEffEdit	m_WaterLightPage;
	CRenderStateEffEdit	m_RenderStatePage;
	CCameraPage			m_CameraPage;
	CWorldSoundPage		m_SoundPage;
	CSingleEffPage		m_SingleEffPage;
	CNaturePage			m_NaturePageS;
	CSporeEffEdit		m_SporePage;
	CLandSkinObj		m_LandSkinObj;
	CLandGatePage		m_LandGatePage;
	CReflectEffEdit		m_ReflectPage;
	CBgmEdit			m_BgmEditPage;
	CMultiTexEffEdit	m_MultiTexPage;
	CNeonEffEdit		m_NeonPage;
	CSpecReflectEffEdit	m_SpecReflectPage;
	CLightMapEffEdit	m_LightMapPage;
	CGlowEffEdit		m_GlowPage;
	CFogPage			m_FogPage;
	CMapEditPage		m_MapEditPage;
	CStaticMeshCreate	m_StaticMeshCreate;
	CPieceSingleEffEdit	m_PieceSingleEffPage;
	CPieceTexEffEdit	m_PieceTexEffPage;
	CPieceManagerEdit	m_PieceManagerPage;
	CPieceMorphEdit		m_PieceMorphPage;
    CPiecePointEdit     m_PiecePointPage;
	CPieceMeshStateEdit	m_PieceMeshStatePage;
	CCamcollPage        m_CamcollPage;
    CSkyBoxManPage		m_SkyBoxManPage;
	CWaterSurfPage		m_WaterSurfPage;
	CMiniMapPage		m_MiniMapPage;
	COcclusionPage		m_OcclusionPage;
	CPrefabEdit			m_PrefabPage;
	CCubeMapPage		m_CubeMapPage;
	CShadowLightPage	m_ShadowLightPage;
	CClothForMaterialEffEdit	m_ClothForMaterialPage;

public:
	EDITSHEET	m_PrevPage;
	void SetActivePageEX( int nNewPage, int BackUp );
	void SetActivePageEX( int nNewPage );
	void SetActiveBackUpPage( int nCurPage );

public:
	void ActiveDirectionPage ( D3DXVECTOR3* pvDirection, DWORD dwIndex )
	{
		m_DirectPage.m_dwOldPageIndex = dwIndex;
		m_DirectPage.m_pvDirection = pvDirection;

		SetActivePageEX( DIRECTIONPAGEINDEX );
		m_DirectPage.UpdateContrl ();
	}

	void ActivePieceSingleEffPage()
	{
		SetActivePageEX( PIECESINGLEEFFINDEX );
	}

	void ActivePieceTexEffPage()
	{
		SetActivePageEX( PIECETEXEFFINDEX );
	}

	void ActivePieceManagerPage()
	{
		SetActivePageEX( PIECEMANAGERINDEX );
	}

	void ActivePieceMorphPage()
	{
		SetActivePageEX( PIECEMORPHINDEX );
	}

	void ActiveReplaceFramePage ()
	{
		SetActivePageEX( REPLACEFRAMEPAGEINDEX );
	}

	void ActiveSoundPage ()
	{
		SetActivePageEX( SOUNDPAGEINDEX );
	}

	void ActiveEffectPage ()
	{
		SetActivePageEX( SINGLEEFFPAGEINDEX );
	}

	void ActiveNaturePage ()
	{
		SetActivePageEX( NATUREINDEX );
	}

	void ActiveSkinObjPage ()
	{
		SetActivePageEX( SKINOBJPAGEINDEX );
	}

	void ActiveBgmEditPage ()
	{
		SetActivePageEX( BGMEDITPAGEINDEX );
		m_BgmEditPage.UpdateContrl ();
	}

	void ActiveFogPage ()
	{
		SetActivePageEX( FOGPAGEINDEX );
	}

	void ActiveMapEditPage ()
	{
		SetActivePageEX( MAPEDITPAGEINDEX );
	}

	void ActiveEffectTilingPage ( DxEffectTiling *pTiling )
	{
		SetActivePageEX( TILINGPAGEINDEX );
		m_TilingPage.SetInstance ( pTiling );
	}

	void ActiveEffectClothPage ( DxEffectCloth *pCloth )
	{
		SetActivePageEX( CLOTHPAGEINDEX );
		m_ClothPage.SetInstance ( pCloth );
	}

	void ActiveEffectClothForMaterialPage ( DxEffectClothForMaterial *pCloth )
	{
		SetActivePageEX( CLOTH4MATERIALINDEX );
		m_ClothForMaterialPage.SetInstance ( pCloth );
	}

	void ActiveEffectDot3Page ( DxEffectDot3 *pDot3 )
	{
		SetActivePageEX( DOT3PAGEINDEX );
		m_Dot3Page.SetInstance ( pDot3 );
	}

	void ActiveEffectRiverPage ( DxEffectRiver *pRiver )
	{
		SetActivePageEX( RIVERPAGEINDEX );
		m_RiverPage.SetInstance ( pRiver );
	}

	void ActiveEffectShadowPage ( DxEffectShadow *pShadow )
	{
		SetActivePageEX( SHADOWPAGEINDEX );
		m_ShadowPage.SetInstance ( pShadow );
	}

	void ActiveEffectFurPage ( DxEffectFur *pFur )
	{
		SetActivePageEX( FURPAGEINDEX );
		m_FurPage.SetInstance ( pFur );
	}

	void ActiveEffectSpecularPage ( DxEffectSpecular *pSpecular )
	{
		SetActivePageEX( SPECULARPAGEINDEX );
		m_SpecularPage.SetInstance ( pSpecular );
	}

	void ActiveEffectRainPointPage ( DxEffectRainPoint *pRainPoint )
	{
		SetActivePageEX( RAINPOINTPAGEINDEX );
		m_RainPointPage.SetInstance ( pRainPoint );
	}

	void ActiveEffectGrassPage ( DxEffectGrass *pGrass )
	{
		SetActivePageEX( GRASSPAGEINDEX );
		m_GrassPage.SetInstance ( pGrass );
	}

	void ActiveEffectSpecular2Page ( DxEffectSpecular2 *pSpecular2 )
	{
		SetActivePageEX( SPECULAR2PAGEINDEX );
		m_Specular2Page.SetInstance ( pSpecular2 );
	}

	void ActiveFrameStatePage ( DxFrame* pFrame )
	{
		SetActivePageEX( FRAMESTATEPAGEINDEX );
		m_FrameStatePage.SetInstance ( pFrame );
	}

	void ActivePieceMeshStatePage ( DxFrame* pFrame )
	{
		SetActivePageEX( PIECEMESHSTATEPAGEINDEX );
		m_PieceMeshStatePage.SetInstance ( pFrame );
	}

	void ActiveEffectNaturePage ( DxEffectNature *pNature )
	{
		SetActivePageEX( NATUREPAGEINDEX );
		m_NaturePage.SetInstance ( pNature );
	}

	void ActiveEffectWaterLightPage ( DxEffectWaterLight *pWaterLight )
	{
		SetActivePageEX( WATERLIGHTPAGEINDEX );
		m_WaterLightPage.SetInstance ( pWaterLight );
	}

	void ActiveEffectRenderStatePage ( DxEffectRenderState *pRenderState )
	{
		SetActivePageEX( RENDERSTATEPAGEINDEX );
		m_RenderStatePage.SetInstance ( pRenderState );
	}

	void ActiveEffectSporePage ( DxEffectSpore *pSpore )
	{
		SetActivePageEX( SPOREPAGEINDEX );
		m_SporePage.SetInstance ( pSpore );
	}

	void ActiveEffectReflectPage ( DxEffectReflect *pReflect )
	{
		SetActivePageEX( RELFECTPAGEINDEX );
		m_ReflectPage.SetInstance ( pReflect );
	}

	void ActiveEffectMultiTexPage ( DxEffectMultiTex *pMultiTex )
	{
		SetActivePageEX( MULTITEXPAGEINDEX );
		m_MultiTexPage.SetInstance ( pMultiTex );
	}

	void ActiveEffectNeonPage ( DxEffectNeon *pNeon )
	{
		SetActivePageEX( NEONPAGEINDEX );
		m_NeonPage.SetInstance ( pNeon );
	}

	void ActiveEffectSpecReflectPage ( DxEffectSpecReflect *pSpecReflect )
	{
		SetActivePageEX( SPECREFLECTPAGEINDEX );
		m_SpecReflectPage.SetInstance ( pSpecReflect );
	}

	void ActiveEffectLightMapPage ( DxEffectLightMap *pLightMap )
	{
		SetActivePageEX( LIGHTMAPPAGEINDEX );
		m_LightMapPage.SetInstance ( pLightMap );
	}

	void ActiveEffectGlowPage ( DxEffectGlow *pEff )
	{
		SetActivePageEX( GLOWPAGEINDEX );
		m_GlowPage.SetInstance ( pEff );
	}

    void ActiveSkyBoxManPage( DxSkyBoxMan* pSkyBoxMan, DxSkyBoxMan* pSkyBoxManSRC, DxFogLocalMan* pFogLocalMan, int nBackUp )
	{
		m_SkyBoxManPage.SetInstance( pSkyBoxMan, pSkyBoxManSRC, pFogLocalMan );
		SetActivePageEX( SKYBOXMANINDEX, nBackUp );
	}

	void ActivePrefabPage()
	{
		SetActivePageEX( PREFABPAGEINDEX );
	}

	void ActiveCubeMapPage()
	{
		SetActivePageEX( CUBEMAPPAGEINDEX );
	}

	void ActiveShadowLightPage()
	{
		SetActivePageEX( SHADOWLIGHTINDEX );
	}

public:
    void SelectPieceList( DxPiece* pPiece, int nCurType );
    void SelectTexEffList( const TSTRING& strName );
	void UpdatePage_Always();
	void UpdatePage_Event( BOOL bUp );
	void UpdatePage_ListChange();
	void UpdatePage_List_AddLastItem();
	void KeyBoard_Enter();
    void UpdateKeyBoardMouse();

// Construction
public:
	CsheetWithTab(CRect& rect, CWnd* pParentWnd = NULL);
	virtual ~CsheetWithTab();

// Attributes
public:
   CRect      m_Rect;

   BOOL       m_bNoTabs;
   int        m_iDistanceFromTop;

// Get/Set
public:
   void NoTabs() { m_bNoTabs = TRUE; }
   void SetDistanceFromTop( int iDistance ) { m_iDistanceFromTop = iDistance; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsheetWithTab)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
protected:
	//{{AFX_MSG(CsheetWithTab)
   afx_msg HBRUSH CtlColor(CDC* pDc, UINT uCtlColor);	
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __SHEETWITHTAB_H_