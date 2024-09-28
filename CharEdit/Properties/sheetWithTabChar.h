#pragma once

#ifndef DEF_SKIN_SYNC
#include "CharPage.h"
#include "SkinPiecePage.h"
#include "ambientMng.h"
#include "SkinObjPage.h"
#include "EffSinglePage.h"
#include "EffBlurPage.h"
#include "EffCloneBlurPage.h"
#include "EffDustPage.h"
#include "EffShockPage.h"
#include "EffAttributePage.h"
#include "EffArrowPage.h"
#include "EffSpecularPage.h"
#include "EffSpecularPage2.h"
#include "EffEmitPage.h"
#include "EffAlphaPage.h"
#include "EffNeonPage.h"
#include "EffMarkPage.h"
#include "EffNoAlphaPage.h"
#include "EffReflection2Page.h"
#include "EffAmbientPage.h"
#include "EffDot3Page.h"
#include "EffMultiTexPage.h"
#include "EffParticlePage.h"
#include "EffBonePosEffPage.h"
#include "EffBoneListEffPage.h"
#include "MaterialPage.h"
#include "EffGhostingPage.h"
#include "EffLevelPage.h"
#include "EffAniSinglePage.h"
#include "EffAniGhostingPage.h"
#include "EffAniTracePage.h"
#include "EffAniFaceOffPage.h"
#include "EffToonPage.h"
#include "EffTexDiffPage.h"
#include "EffUserColorPage.h"
#include "EffNormalMapPage.h"
#include "EffKeepManPage.h"
#include "EffKeepCTEdgePage.h"
#include "EffKeepCTShadePage.h"
#include "EffKeepCTColorPage.h"
#include "EffLine2BonePage.h"
#include "EffLine2BoneDPage.h"
#include "AniBoneScalePage.h"
#include "AniJiggleBlendPage.h"
#include "AroundEffPage.h"
#include "AttBonePage.h"
#include "AttachLinkPage.h"
#include "VehiclePage.h"
#include "EffOverlayPage.h"
#include "EffTotalShaderPage.h"
#include "EffVisualMaterialPage.h"
#include "./Effects/EffSplineSingleEffPage.h"
#include "BoneSettingPage.h"
#include "JiggleBonePage.h"
#include "JiggleBoneCollPage.h"
#include "ClothCollPage.h"
#include "FootCollPage.h"
#include "SkinInfoPage.h"
#include "EffWorldBattleFlagPage.h"
#include "EffNonePage.h"
#include "FlyCameraEditPage.h"
#else
#include "CharPage.h"
#include "SkinPiecePage.h"
#include "SkinObjPage.h"
#include "EffKeepManPage.h"
#include "AttBonePage.h"
#include "AttachLinkPage.h"
#include "VehiclePage.h"

#include "EffAniSinglePage.h"
#include "EffAniGhostingPage.h"
#include "EffAniTracePage.h"
#include "EffAniFaceOffPage.h"

#include "EffKeepCTEdgePage.h"
#include "EffKeepCTShadePage.h"
#include "EffKeepCTColorPage.h"

#include "AniBoneScalePage.h"
#include "AniJiggleBlendPage.h"

#include "BoneSettingPage.h"
#include "JiggleBonePage.h"
#include "JiggleBoneCollPage.h"
#include "ClothCollPage.h"
#include "FootCollPage.h"
#include "SkinInfoPage.h"
#endif

#include "WeaponScalePage.h"
#include "BillboardPage.h"

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTabChar

enum CHAR_TAB_PAGE
{
#ifndef DEF_SKIN_SYNC
	CHARTAB_CHARFILE			= 0,
	CHARTAB_CHARPIECE			= 1,
	CHARTAB_CHAROBJ				= 2,
	CHARTAB_CHAREFF				= 3,
//	CHARTAB_AMBIENT				= 4,
	CHARTAB_ATTBONEFILE			= 4,
	CHARTAB_ATTACHLINK			= 5,
	CHARTAB_VEHICLE				= 6,
	CHARTAB_FLYCAMERA			= 7,

	CHARTAB_EFFECT_SINGLE		,
	CHARTAB_EFFECT_BLUR			,
	CHARTAB_EFFECT_CLONEBLUR	,
	CHARTAB_EFFECT_DUST			,
	CHARTAB_EFFECT_SHOCK		,
	CHARTAB_EFFECT_ATTRIBUTE	,
	CHARTAB_EFFECT_ARROW		,
	CHARTAB_EFFECT_SPECULAR		,
	CHARTAB_EFFECT_SPECULAR2	,
	CHARTAB_EFFECT_EMIT			,
	CHARTAB_EFFECT_ALPHA		,
	CHARTAB_EFFECT_NEON			,
	CHARTAB_EFFECT_MARK			,
	CHARTAB_EFFECT_NOALPHA		,
	CHARTAB_EFFECT_REFLECTION2	,
	CHARTAB_EFFECT_AMBIENT		,
	CHARTAB_EFFECT_DOT3			,
	CHARTAB_EFFECT_MULTITEX		,
	CHARTAB_EFFECT_PARTICLE		,
	CHARTAB_EFFECT_BONEPOSEFF	,
	CHARTAB_EFFECT_BONELISTEFF	,
	CHARTAB_EFFECT_USERCOLOR	,
	CHARTAB_EFFECT_LINE2BONE	,
	CHARTAB_EFFECT_LINE2BONED	,

	CHARTAB_MATERIAL			,

	CHARTAB_EFFECT_GHOSTING		,
	CHARTAB_EFFECT_LEVEL		,
	CHARTAB_EFFECT_TOON			,
	CHARTAB_EFFECT_TEXDIFF		,
	CHARTAB_EFFECT_NORMALMAP	,

	CHARTAB_EFFECT_NONE,

	CHARTAB_EFFANI_SINGLE,
	CHARTAB_EFFANI_GHOSTING,
	CHARTAB_EFFANI_TRACE,
	CHARTAB_EFFANI_FACEOFF,

	CHARTAB_EFFKEEP_CTEDGE,
	CHARTAB_EFFKEEP_CTSHADE,
	CHARTAB_EFFKEEP_CTCOLOR,

	CHARTAB_ANIBONESCALE,
	CHARTAB_ANIJIGGLEBLEND,

	CHARTAB_AROUNDEFFECT,

	CHARTAB_OVERLAY,
	CHARTAB_TOTALSHADER,
    CHARTAB_VISUAL_MATERIAL,
    CHARTAB_SPLINE_SINGLE_EFF,

	CHARTAB_BONE_SETTING,
	CHARTAB_JIGGLE_BONE,
	CHARTAB_JIGGLE_BONE_COLL,

	CHARTAB_CLOTH_COLL,

	CHARTAB_EFFECT_WB_FLAG,

	CHARTAB_FOOT_COLL,

	CHARTAB_SKIN_INFO,

	CHARTAB_WEAPON_SCALE,
	CHARTAB_BILLBOARD,

	CHARTAB_SIZE,
#else
    CHARTAB_CHARFILE,
    CHARTAB_CHAREFF,
    CHARTAB_ATTBONEFILE,
    CHARTAB_ATTACHLINK,
    CHARTAB_VEHICLE,
    CHARTAB_SIZE,

    CHARTAB_EFFANI_SINGLE = CHARTAB_SIZE,
    CHARTAB_EFFANI_GHOSTING,
    CHARTAB_EFFANI_TRACE,
    CHARTAB_EFFANI_FACEOFF,

    CHARTAB_EFFKEEP_CTEDGE,
    CHARTAB_EFFKEEP_CTSHADE,
    CHARTAB_EFFKEEP_CTCOLOR,

    CHARTAB_ANIBONESCALE,
	CHARTAB_ANIJIGGLEBLEND,

	CHARTAB_BONE_SETTING,
    CHARTAB_JIGGLE_BONE,
	CHARTAB_JIGGLE_BONE_COLL,

	CHARTAB_CLOTH_COLL,

	CHARTAB_FOOT_COLL,

	CHARTAB_SKIN_INFO,

	CHARTAB_WEAPON_SCALE,
	CHARTAB_BILLBOARD,

    CHARTAB_TOTAL_SIZE,
#endif
};

class CsheetWithTabChar : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTabChar)

public:
#ifndef DEF_SKIN_SYNC
	static const char m_szCharTabPages[CHARTAB_EFFECT_SINGLE][65];
#else
    static const char m_szCharTabPages[CHARTAB_SIZE][65];
#endif

public:
	CWnd*			m_pWndParent;

// Construction
public:
	CCharPage			m_CharPage;

#ifndef DEF_SKIN_SYNC
	CSkinPiecePage		m_SkinPiecePage;
	CSkinObjPage		m_SkinObjPage;
#endif

	CEffKeepManPage		m_EffKeepManPage;
	CAttBonePage		m_AttBonePage;
	CAttachLinkPage		m_AttachLinkPage;
	CVehiclePage		m_VehiclePage;
	CFlyCameraEditPage	m_FlyCameraPage;

#ifndef DEF_SKIN_SYNC
	CEffSinglePage	    	m_EffSingle;		
	CEffBlurPage	    	m_EffBlur;			
	CEffCloneBlurPage   	m_EffCloneBlur;		
	CEffDustPage	    	m_EffDust;			
	CEffShockPage	    	m_EffShock;			
	CEffAttributePage   	m_EffAttribute;		
	CEffArrowPage	    	m_EffArrow;			
	CEffSpecularPage    	m_EffSpecular;		
	CEffSpecularPage2   	m_EffSpecular2;		
	CEffEmitPage	    	m_EffEmit;			
	CEffAlphaPage	    	m_EffAlpha;			
	CEffNeonPage	    	m_EffNeon;			
	CEffMarkPage	    	m_EffMark;			
	CEffNoAlphaPage	    	m_EffNoAlpha;		
	CEffReflection2Page 	m_EffReflection2;	
	CEffAmbientPage	    	m_EffAmbient;		
	CEffDot3Page	    	m_EffDot3;			
	CEffMultiTexPage    	m_EffMultiTex;		
	CEffParticlePage    	m_EffParticle;		
	CEffBonePosEffPage  	m_EffBonePosEff;	
	CEffBoneListEffPage 	m_EffBoneListEff;	
	CEffUserColorPage   	m_EffUserColor;		
	CEffLine2BonePage   	m_EffLine2Bone;		
	CEffLine2BoneDPage  	m_EffLine2BoneD;
	CEffNonePage			m_EffNone;
#endif

#ifndef DEF_SKIN_SYNC
	CMaterialPage	    	m_SkinMaterial;		
	CEffGhostingPage    	m_EffGhosting;		
	CEffLevelPage	    	m_EffLevel;			
	CEffToonPage	    	m_EffToon;			
	CEffTexDiffPage		    m_EffTexDiff;		
	CEffNormalMapPage	    m_EffNormalMap;		
	CEffOverlayPage		    m_EffOverlay;
	CEffTotalShaderPage	    m_EffTotalShader;
    CEffVisualMaterialPage	m_EffVisualMaterial;
    CEffSplineSingleEffPage	m_EffSplineSingleEff;
#endif

	CEffAniSinglePage	m_EffAniSingle;		
	CEffAniGhostingPage	m_EffAniGhosting;	
	CEffAniTracePage	m_EffAniTrace;		
	CEffAniFaceOffPage	m_EffAniFaceOff;	

	CEffKeepCTEdgePage	m_EffKeepCTEdge;	
	CEffKeepCTShadePage	m_EffKeepCTShade;	
	CEffKeepCTColorPage	m_EffKeepCTColor;	

	CAniBoneScalePage	m_AniBoneScale;
	CAniJiggleBlendPage	m_AniJiggleBlend;

#ifndef DEF_SKIN_SYNC
	CAroundEffPage		m_AroundEff;
#endif

	CBoneSettingPage	m_BoneSetting;
	CJiggleBonePage		m_JiggleBone;
	CJiggleBoneCollPage	m_JiggleBoneColl;

	CClothCollPage		m_ClothColl;

	CFootCollPage		m_FootColl;

	CSkinInfoPage		m_sSkinInfo;

	CWeaponScalePage	m_sWeaponScale;
	CBillboardPage		m_sBillboard;

#ifndef DEF_SKIN_SYNC
	CEffWorldBattleFlagPage	m_EffWorldBattleFlag;
#endif

#ifdef DEF_SKIN_SYNC
    class CPieceContainerPage* m_pCurPieceContainer;
#endif

public:
	void SetActivePageTab ( int nPage );
	BOOL SetActivePage ( int nPage );

	// User 가 TimeLine 의 시간을 변경한다면 그와 연관된 작업을 한다.
	void ChangeTimeOfTimeLine( int nPage, DWORD dwToTime );

	// User 가 CDlgTimeLine 의 OnButtonPlay 을 누른다면 그와 연관된 작업을 한다.
	void OnButtonPlayOfCDlgTimeLine( int nPage );

	void SetSpinControl( int nPage, int nRadius, int nHeight, int nSpin3, int nSpin4, int nColorHeight );
    void ResetDialog( int nPage );
    void UpdateKeyBoardMouse( int nPage );

	void SetActivePage_JiggleBone( int nPage, DxJiggleBone* pJiggleBone )
	{
		m_JiggleBone.SetData( pJiggleBone );
		SetActivePage( nPage );
	}

	void SetActivePage_JiggleBoneColl( int nPage, DxJiggleBoneColl* pJiggleBoneColl )
	{
		m_JiggleBoneColl.SetData( pJiggleBoneColl );
		SetActivePage( nPage );
	}

	void SetActivePage_ClothColl( int nPage, DxClothColl* pClothColl )
	{
		m_ClothColl.SetData( pClothColl );
		SetActivePage( nPage );
	}

public:
	CsheetWithTabChar ( CRect& rect, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTabChar ();

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
	//{{AFX_VIRTUAL(CsheetWithTabChar)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
protected:
	//{{AFX_MSG(CsheetWithTabChar)
   afx_msg HBRUSH CtlColor(CDC* pDc, UINT uCtlColor);	
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


