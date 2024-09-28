#pragma once

#include "AnimPage.h"
#include "AnimTabPage.h"
#include "AnimSetPage.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "SoundPage.h"
#include "AniDivPage.h"
#include "StrikePage.h"
#include "EffAniPage.h"
#include "EffectNamePage.h"

class DxSkinChar;
class DxSkinCharData;
class DxAttBone;
class DxAttBoneData;
class DxSkinAniControl;
class DxSkinAniControlData;

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTabAnim

enum EDITSHEET
{
	ANIMPAGEINDEX			= 0,
	ANIMSETPAGEINDEX		= 1,
	SOUNDPAGEINDEX			= 2,
	ANIDIVPAGEINDEX			= 3,
	ANISTRIKE				= 4,
	ANIEFFBONE				= 5,
	ANIEFFNAME				= 6,
};


class CsheetWithTabAnim : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTabAnim)

public:
	CWnd*				m_pWndParent;

public:
	void ApplyAniContrl ();

public:
	DxSkinAniControlData*	m_pSkinAniContrlData;
	DxSkinAniControlData*	GetSkinAniContrlData()		{	return m_pSkinAniContrlData; }

	DxSkinAniControl*		m_pSkinAniContrl;
	DxSkinAniControl*		GetSkinAniContrl()			{	return m_pSkinAniContrl; }

public:
	CAnimPage			m_AnimPage;		//	0
	CAnimTabPage		m_AnimTabPage;	//	0
	CAnimSetPage		m_AnimsetPage;	//	1
	CSoundPage			m_SoundPage;	//	2
	CAniDivPage			m_AniDivPage;	//	3
	CStrikePage			m_StrikePage;	//	4
	CEffAniPage			m_EffAniPage;	//	5
	CEffectNamePage		m_EffNamePage;	//	6

public:
	void SetPages( DxSkinAniControlData* pSkinAniContrlData, DxSkinAniControl* pSkinAniContrl );
	void ReSetPages();

public:
	void AnimSetActive ( CString sName );
	void SoundSetActive( SChaSoundData* pChaSoundData );
	void AniDivActive ( CString sName );
	void AniStrike ( CString sName );
	void AniEff ( CString sName );
	void AniEffName( BOOL bChange = TRUE );


public:
	CsheetWithTabAnim ( CWnd* pParentWnd = NULL );
	CsheetWithTabAnim ( CRect& rect, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTabAnim ();

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
	public:
	virtual BOOL OnInitDialog();

// Implementation
public:

// Generated message map functions
protected:
   afx_msg HBRUSH CtlColor(CDC* pDc, UINT uCtlColor);	

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

