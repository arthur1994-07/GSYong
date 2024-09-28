#include "StdAfx.h"
#include "LunchBoxTimeDisplay.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "SkillTimeUnit.h"
#include "../../../RanLogic/Skill/GLSkill.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CLunchBoxTimeDisplay::CLunchBoxTimeDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_NEWID ( LUNCHBOXTIME_UNIT )
{
}

CLunchBoxTimeDisplay::~CLunchBoxTimeDisplay ()
{
}

void CLunchBoxTimeDisplay::CreateSubControl ()
{	
	const CString strKeyword = "LUNCHBOX_TIME_UNIT";
	CString strCombine;

	for ( int i=0; i<SKILLFACT_SIZE_LUNCHBOX + SKILLFACT_SIZE_EXP_LUNCHBOX; i++ )
	{
		strCombine.Format ( "%s%d", strKeyword, i );
		CUIControl* pDummyControl = new CUIControl(m_pEngineDevice);
		pDummyControl->CreateSub ( this, strCombine.GetString(), UI_FLAG_DEFAULT, LUNCHBOXTIME_DUMMY_BASE + i );
		RegisterControl ( pDummyControl );

		m_pLunchBoxUnitDummy[i] = pDummyControl;
	}
}

void CLunchBoxTimeDisplay::SetEnableAll ( const BOOL bEnable )
{
	LUNCHBOXTIME_DISPLAY_LIST_ITER iter     = m_LunchboxPlayList.begin ();
	LUNCHBOXTIME_DISPLAY_LIST_ITER iter_end = m_LunchboxPlayList.end ();

	for ( ; iter != iter_end; ++iter )
	{
		const LUNCHBOXTIME_PAIR& skill_pair = (*iter);
		const DWORD& dwID = skill_pair.first;
		const UIGUID& cID = skill_pair.second;			

		CSkillTimeUnit* pControl = (CSkillTimeUnit*) FindControl ( cID );
		if ( !pControl )
		{
			GASSERT ( 0 && "컨트롤 찾기 실패" );
			continue;
		}

		pControl->SetEnable ( bEnable );
	}
}

UIGUID CLunchBoxTimeDisplay::GET_EXIST_PLAY_LUNCHBOX_CONTROL ( const DWORD& dwSkillID )
{
	LUNCHBOXTIME_DISPLAY_LIST_ITER iter     = m_LunchboxPlayList.begin ();
	LUNCHBOXTIME_DISPLAY_LIST_ITER iter_end = m_LunchboxPlayList.end ();
	for ( ; iter != iter_end; ++iter )
	{
		const LUNCHBOXTIME_PAIR& pair = (*iter);
		const DWORD& dwID = pair.first;
		const UIGUID& cID = pair.second;
		if ( dwID == dwSkillID ) return cID;
	}

	return NO_ID;
}

void CLunchBoxTimeDisplay::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	// Blank
}

void CLunchBoxTimeDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter ) return;

	//	삭제
	if ( !m_LunchboxPlayList.empty () )
	{
		LUNCHBOXTIME_DISPLAY_LIST_ITER iter     = m_LunchboxPlayList.begin ();
		LUNCHBOXTIME_DISPLAY_LIST_ITER iter_end = m_LunchboxPlayList.end ();

		for ( ; iter != iter_end; )
		{
			const LUNCHBOXTIME_PAIR& lunchbox_pair = (*iter);
			const DWORD& dwID = lunchbox_pair.first;
			const UIGUID& cID = lunchbox_pair.second;

			//	찾기			
			SSKILLFACT sSKILLFACT;
			if ( !GET_PLAY_LUNCHBOX ( dwID, sSKILLFACT ) )
			{				
				DeleteControl ( cID, 0 );
				iter = m_LunchboxPlayList.erase ( iter );
			}
			else ++iter;
		}
	}

	//	등록
	{		
		for (size_t i=0; i<SKILLFACT_SIZE_LUNCHBOX + SKILLFACT_SIZE_EXP_LUNCHBOX; ++i)
		{
			size_t nIndex = i+SKILLFACT_INDEX_LUNCHBOX_START;
            if (nIndex >= SKILLFACT_SIZE)
                continue;

			const SSKILLFACT& sSKILLFACT = pCharacter->m_sSKILLFACT[nIndex];
			const DWORD& dwID = sSKILLFACT.sNATIVEID.dwID;
			//sSKILLFACT.wLEVEL;

			if (dwID == NATIVEID_NULL().dwID)
                continue;

			UIGUID cID = GET_EXIST_PLAY_LUNCHBOX_CONTROL ( dwID );
			if (cID == NO_ID)
                NEW_PLAY_LUNCHBOX ( dwID, sSKILLFACT.wLEVEL, static_cast< WORD >( nIndex ) );
		}
	}

	if ( m_LunchboxPlayList.empty () ) return ;

	//	업데이트
	int nPLAY_SKILL = 0;
	LUNCHBOXTIME_DISPLAY_LIST_ITER iter     = m_LunchboxPlayList.begin ();
	LUNCHBOXTIME_DISPLAY_LIST_ITER iter_end = m_LunchboxPlayList.end ();

	for ( ; iter != iter_end; ++iter )
	{
		const LUNCHBOXTIME_PAIR& skill_pair = (*iter);
		const DWORD& dwID = skill_pair.first;
		const UIGUID& cID = skill_pair.second;			

		CSkillTimeUnit* pControl = (CSkillTimeUnit*) FindControl ( cID );
		if ( !pControl )
		{
			GASSERT ( 0 && "컨트롤 찾기 실패" );
			continue;
		}

		//	찾기			
		SSKILLFACT sSKILLFACT;
		if ( !GET_PLAY_LUNCHBOX ( dwID, sSKILLFACT ) )
		{
			GASSERT ( 0 && "스킬 데이타 찾기 실패" );
			continue;
		}

		const float& fAGE    = sSKILLFACT.fAGE;
		const BOOL&  bEnable = sSKILLFACT.bEnable;

		pControl->SetGlobalPos     ( m_pLunchBoxUnitDummy[nPLAY_SKILL]->GetGlobalPos () );
		pControl->SetLeftTime      ( fAGE );
		pControl->SetEnable        ( bEnable );
		pControl->SetDetailVisible ( TRUE );

		nPLAY_SKILL++;
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

bool CLunchBoxTimeDisplay::GET_PLAY_LUNCHBOX ( const DWORD& dwSkillID, SSKILLFACT& sSKILLFACT )
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter ) return false;

	for (size_t i=0; i<SKILLFACT_SIZE_LUNCHBOX + SKILLFACT_SIZE_EXP_LUNCHBOX; ++i)
	{
		size_t nIndex = i+SKILLFACT_INDEX_LUNCHBOX_START;
        if (nIndex >= SKILLFACT_SIZE)
            continue;

		const SSKILLFACT& sSkillFactIter = pCharacter->m_sSKILLFACT[nIndex];
		if ( sSkillFactIter.sNATIVEID == dwSkillID )
		{
			sSKILLFACT = sSkillFactIter;
			return true;
		}
	}

	return false;
}

HRESULT CLunchBoxTimeDisplay::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

bool CLunchBoxTimeDisplay::NEW_PLAY_LUNCHBOX ( const DWORD& dwSkillID, const WORD wLEVEL, const WORD wSlot )
{
	m_NEWID++;
	if ( LUNCHBOXTIME_UNIT_END <= m_NEWID )
		m_NEWID = LUNCHBOXTIME_UNIT;

	float fLifeTime = 0.0f;
	if ( !GET_LUNCHBOX_LIFE_TIME ( dwSkillID, wLEVEL, fLifeTime ) ) return false;

	SNATIVEID sNativeID ( dwSkillID );
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( !pSkill ) return false;

	CSkillTimeUnit* pSkillTimeUnit = new CSkillTimeUnit(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pSkillTimeUnit->CreateSub ( this, "LUNCHBOX_TIME_UNIT", UI_FLAG_DEFAULT, m_NEWID );
	pSkillTimeUnit->CreateSubControl ();
	pSkillTimeUnit->InitDeviceObjects ( m_pd3dDevice );
	pSkillTimeUnit->RestoreDeviceObjects ( m_pd3dDevice );	
	pSkillTimeUnit->SetSkill ( dwSkillID, wLEVEL, wSlot, fLifeTime, pSkill->GetName() );
	RegisterControl ( pSkillTimeUnit );	

	pSkillTimeUnit->SetEnable        ( TRUE );
	pSkillTimeUnit->SetDetailVisible ( TRUE );

	LUNCHBOXTIME_PAIR pair (dwSkillID,m_NEWID);
	m_LunchboxPlayList.push_back ( pair );

	return true;
}

bool CLunchBoxTimeDisplay::GET_LUNCHBOX_LIFE_TIME ( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime )
{
	SNATIVEID sNativeID ( dwSkillID );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( !pSkill ) return false;

	//	최대 지연시간
	fLifeTime = pSkill->m_sAPPLY.sDATA_LVL[wLEVEL].fLIFE;
	fLifeTime = (fLifeTime)?fLifeTime:1.0f;

	return true;
}

MyLunchBoxTimeDisplay::MyLunchBoxTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CLunchBoxTimeDisplay( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyLunchBoxTimeDisplay::CreateUIWindowAndRegisterOwnership()
{
    CLunchBoxTimeDisplay::Create( LUNCHBOX_TIME_DISPLAY, "LUNCHBOX_TIME_DISPLAY", UI_FLAG_RIGHT );
    CLunchBoxTimeDisplay::CreateSubControl();
    CLunchBoxTimeDisplay::m_pInterface->UiRegisterControl( this );
    CLunchBoxTimeDisplay::m_pInterface->UiShowGroupBottom( LUNCHBOX_TIME_DISPLAY );
}