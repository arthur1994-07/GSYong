#ifndef __SHEETWITHTAB_H_
#define __SHEETWITHTAB_H_

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

#include "EffGroupPage.h"
#include "ParticleSysPage.h"
#include "DirectionEdit.h"
#include "GroundPage.h"
#include "MeshEffectPage.h"
#include "SequencePage.h"
#include "GroupPropPage.h"
#include "DecalPage.h"
#include "CameraPage.h"
#include "EffectSoundEdit.h"
#include "BlurSysPage.h"
#include "LightingPage.h"
#include "MoveRotatePage.h"
#include "TargetInfluencePage.h"
#include "SkinMeshPage.h"
#include "WavePage.h"
#include "PointLightPage.h"
#include "MoveTargetPage.h"
#include "ScreenEffectPage.h"

enum EDITSHEET
{
	GROUPPAGEINDEX		= 0,
	PARTICLEPAGEINDEX	= 1,
	DIRECTIONPAGEINDEX	= 2,
	GROUNDPAGEINDEX		= 3,
	MESHPAGEINDEX		= 4,
	SEQUENCEPAGEINDEX	= 5,
	GROUPPROPINDEX		= 6,
	DECALINDEX			= 7,
	CAMERAINDEX			= 8,
	SOUNDINDEX			= 9,
	BLURSYSINDEX		= 10,
	LIGHTINGINDEX		= 11,
	MOVEROTATEINDEX		= 12,
	SKINMESHINDEX		= 13,
	WAVEINDEX			= 14,
	POINTLIGHTINDEX		= 15,
	MOVETARGETINDEX		= 16,
	TARGETINFLUENCEINDEX = 17,
	SCREENEFFECTINDEX	= 18,
};

extern CString g_strEditSheet[30];

class CsheetWithTab : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTab)

public:
	CWnd*				m_pWndParent;

public:
	CParticleSysPage		m_ParticleSysPage;
	CEffGroupPage			m_EffGroupPage;
	DirectionEdit			m_DirectPage;
	CGroundPage				m_GroundPage;
	CMeshEffectPage			m_MeshPage;
	CSequencePage			m_SequencePage;
	CGroupPropPage			m_GroupPropPage;
	CDecalPage				m_DecalPage;
	CCameraEffPage			m_CameraPage;
	CEffectSoundEdit		m_SoundPage;
	CBlurSysPage			m_BlurSysPage;
	CLightingPage			m_LightingPage;
	CMoveRotatePage			m_MoveRotatePage;
	CSkinMeshPage			m_SkinMeshPage;
	CWavePage				m_WavePage;
	CPointLightPage			m_PointLightPage;
	CMoveTargetPage			m_MoveTargetPage;
	CTargetInfluencePage	m_TargetInfluencePage;
	CScreenEffectPage		m_ScreenEffectPage;

public:
	void ActiveEffGroupPage ()
	{
		SetActivePage ( GROUPPAGEINDEX );
	}

	void ActiveEffParticleSysPage ( EFF_PROPERTY* pProp )
	{
		SetActivePage ( PARTICLEPAGEINDEX );
		m_ParticleSysPage.SetProperty ( pProp );
	}

	void ActiveDirectionPage ( D3DXVECTOR3* pvDirection, float fScale, DWORD dwIndex )
	{
		m_DirectPage.m_dwOldPageIndex = dwIndex;
		m_DirectPage.m_fScale = fScale;
		m_DirectPage.m_pvDirection = pvDirection;

		SetActivePage ( DIRECTIONPAGEINDEX );
		m_DirectPage.UpdateContrl ();
	}

	void ActiveEffGroundPage ( EFF_PROPERTY* pProp )
	{
		SetActivePage ( GROUNDPAGEINDEX );
		m_GroundPage.SetProperty ( pProp );
	}

	void ActiveEffMeshPage ( EFF_PROPERTY* pProp )
	{
		SetActivePage ( MESHPAGEINDEX );
		m_MeshPage.SetProperty ( pProp );
	}

	void ActiveEffSequencePage ( EFF_PROPERTY* pProp )
	{
		SetActivePage ( SEQUENCEPAGEINDEX );
		m_SequencePage.SetProperty ( pProp );
	}

	void ActiveEffGroupPropPage ( EFF_PROPGROUP* pGroupProp )
	{
		SetActivePage ( GROUPPROPINDEX );
		m_GroupPropPage.SetProperty ( pGroupProp );
	}

	void ActiveEffDecalPage ( EFF_PROPERTY* pDecal )
	{
		SetActivePage ( DECALINDEX );
		m_DecalPage.SetProperty ( pDecal );
	}

	void ActiveEffCameraPage ( EFF_PROPERTY* pCamera )
	{
		SetActivePage ( CAMERAINDEX );
		m_CameraPage.SetProperty ( pCamera );
	}

	void ActiveEffSoundPage ( SMovSound *pMovSound, DWORD dwIndex )
	{
		SetActivePage ( SOUNDINDEX );
		m_SoundPage.SetProperty ( pMovSound, dwIndex );
	}

	void ActiveEffBlurSysPage ( EFF_PROPERTY* pBlurSys )
	{
		SetActivePage ( BLURSYSINDEX );
		m_BlurSysPage.SetProperty ( pBlurSys );
	}

	void ActiveEffLightingPage ( EFF_PROPERTY* pLighting )
	{
		SetActivePage ( LIGHTINGINDEX );
		m_LightingPage.SetProperty ( pLighting );
	}

	void ActiveEffMoveRotatePage ( EFF_PROPERTY* pMoveRotate )
	{
		SetActivePage ( MOVEROTATEINDEX );
		m_MoveRotatePage.SetProperty ( pMoveRotate );
	}

	void ActiveEffTargetInfluencePage ( EFF_PROPERTY* pTargetInfluence )
	{
		SetActivePage ( TARGETINFLUENCEINDEX );
		m_TargetInfluencePage.SetProperty ( pTargetInfluence );
	}

	void ActiveEffSkinMeshPage ( EFF_PROPERTY* pProperty )
	{
		SetActivePage ( SKINMESHINDEX );
		m_SkinMeshPage.SetProperty ( pProperty );
	}

	void ActiveEffWavePage ( EFF_PROPERTY* pProperty )
	{
		SetActivePage ( WAVEINDEX );
		m_WavePage.SetProperty ( pProperty );
	}

	void ActiveEffPointLightPage ( EFF_PROPERTY* pProperty )
	{
		SetActivePage ( POINTLIGHTINDEX );
		m_PointLightPage.SetProperty ( pProperty );
	}

	void ActiveEffMoveTargetPage ( EFF_PROPERTY* pProperty )
	{
		SetActivePage ( MOVETARGETINDEX );
		m_MoveTargetPage.SetProperty ( pProperty );
	}

	void ActiveEffScreenEffectPage ( EFF_PROPERTY* pProperty )
	{
		SetActivePage ( SCREENEFFECTINDEX );
		m_ScreenEffectPage.SetProperty ( pProperty );
	}

// Construction
public:
	CsheetWithTab(CWnd* pParentWnd = NULL);
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