#include "StdAfx.h"
#include "SkillTimeDisplay.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "SkillTimeUnit.h"
#include "../../../RanLogic/Skill/GLSkill.h"

#include "../../InnerInterface.h"

//#include "../../RanLogic/Character/GLCharData.h" //봉주

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSkillTimeDisplay::CSkillTimeDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
    , m_NEWID ( SKILLTIME_UNIT )
{
}

CSkillTimeDisplay::~CSkillTimeDisplay ()
{
}

void CSkillTimeDisplay::CreateSubControl ()
{	
	const CString strKeyword = "SKILL_TIME_UNIT";
	CString strCombine;

	for ( int i=0; i<SKILLFACT_SIZE_NORMAL_INC_ADDITIONAL; i++ )
	{
		strCombine.Format ( "%s%d", strKeyword, i );
		CUIControl* pDummyControl = new CUIControl(m_pEngineDevice);
		pDummyControl->CreateSub ( this, strCombine.GetString(), UI_FLAG_DEFAULT, SKILLTIME_DUMMY_BASE + i );
		RegisterControl ( pDummyControl );

		m_pSkillUnitDummy[i] = pDummyControl;
	}
}

UIGUID CSkillTimeDisplay::GET_EXIST_PLAY_SKILL_CONTROL ( const DWORD& dwSkillID )
{
	SKILLTIME_DISPLAY_LIST_ITER iter = m_SkillPlayList.begin ();
	SKILLTIME_DISPLAY_LIST_ITER iter_end = m_SkillPlayList.end ();
	for ( ; iter != iter_end; ++iter )
	{
		const SKILLTIME_PAIR& pair = (*iter);
		const DWORD& dwID = pair.first;
		const UIGUID& cID = pair.second;
		if ( dwID == dwSkillID ) return cID;
	}

	return NO_ID;
}

void CSkillTimeDisplay::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	// Blank
}

void CSkillTimeDisplay::RegisterPlaySkill( EMSKILLFACT_TYPE emType )
{
	GLCharacter*	pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return;

	if( emType == EMSKILLFACT_TYPE_NORMAL )
	{
		for( int i = 0; i < SKILLFACT_SIZE_NORMAL; ++i )
		{
			int		nIndex = i + SKILLFACT_INDEX_NORMAL_START;
			if( nIndex >= SKILLFACT_SIZE )
				continue;

			const SSKILLFACT&	sSKILLFACT	= pCharacter->m_sSKILLFACT[nIndex];
			const DWORD&		dwID		= sSKILLFACT.sNATIVEID.dwID;

			if( dwID == NATIVEID_NULL().dwID )
				continue;

			UIGUID		cID = GET_EXIST_PLAY_SKILL_CONTROL( dwID );
			if( cID == NO_ID )
				NEW_PLAY_SKILL( dwID, sSKILLFACT.wLEVEL, static_cast< WORD >( nIndex ) );
		} //for
	}
	else if( emType == EMSKILLFACT_TYPE_ADDITIONAL )
	{
		for( int i = 0; i < SKILLFACT_SIZE_ADDITIONAL; ++i )
		{
			int		nIndex = i + SKILLFACT_INDEX_ADDITIONAL_START;
			if( nIndex >= SKILLFACT_SIZE )
				continue;

			const SSKILLFACT&	sSKILLFACT	= pCharacter->m_sSKILLFACT[nIndex];
			const DWORD&		dwOption	= sSKILLFACT.sNATIVEID.dwID;

			if( dwOption == NATIVEID_NULL().dwID )
				continue;

			UIGUID		cID = GET_EXIST_PLAY_SKILL_CONTROL( dwOption );
			if( cID == NO_ID )
				NEW_PLAY_ADDITIONAL( dwOption, sSKILLFACT.fAGE );
		} //for
	}
	else //doesn't support
	{
		return;
	} //if..else if..
} //CSkillTimeDisplay::RegisterPlaySkill

void CSkillTimeDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if( !IsVisible () )
		return ;

	if( !m_pGaeaClient->GetCharacter() )
		return;

	//	삭제
	if( !m_SkillPlayList.empty () )
	{
		SKILLTIME_DISPLAY_LIST_ITER iter = m_SkillPlayList.begin ();
		SKILLTIME_DISPLAY_LIST_ITER iter_end = m_SkillPlayList.end ();
		while( iter != iter_end )
		{
			const SKILLTIME_PAIR&	skill_pair	= (*iter);
			const DWORD&			dwID		= skill_pair.first;
			const UIGUID&			cID			= skill_pair.second;

			//	찾기			
			SSKILLFACT sSKILLFACT;
			if( !GET_PLAY_SKILLFACT( dwID, sSKILLFACT ) )
			{	
				DeleteControl( cID, 0 );
				iter = m_SkillPlayList.erase( iter );
				continue;
			} //if

			++iter;
		} //while
	} //if

	//	등록
	RegisterPlaySkill( EMSKILLFACT_TYPE_NORMAL );
	RegisterPlaySkill( EMSKILLFACT_TYPE_ADDITIONAL );

	if( m_SkillPlayList.empty () )
		return ;

	//	업데이트
	int nPLAY_SKILL = 0;
	SKILLTIME_DISPLAY_LIST_ITER iter		= m_SkillPlayList.begin();
	SKILLTIME_DISPLAY_LIST_ITER iter_end	= m_SkillPlayList.end();

	// 소환가능 패시브가 있을 경우;
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return;

	if ( pCharacter->m_sSummonable.bSummonable )
	{
		nPLAY_SKILL = 1;
	}

	for( ; iter != iter_end; ++iter )
	{
		const SKILLTIME_PAIR&	skill_pair	= (*iter);
		const DWORD&			dwID		= skill_pair.first;
		const UIGUID&			cID			= skill_pair.second;			
		CSkillTimeUnit*			pControl	= (CSkillTimeUnit*) FindControl ( cID );
		if( !pControl )
		{
			GASSERT ( 0 && "컨트롤 찾기 실패" );
			continue;
		}

		//	찾기			
		SSKILLFACT sSKILLFACT;
		if( !GET_PLAY_SKILLFACT ( dwID, sSKILLFACT ) )
		{
			GASSERT ( 0 && "스킬 데이타 찾기 실패" );
			continue;
		} //if

		const float& fAGE    = sSKILLFACT.fAGE;
        const float& fLIFE   = sSKILLFACT.fLIFE;
		const BOOL&  bEnable = sSKILLFACT.bEnable;

		pControl->SetGlobalPos     ( m_pSkillUnitDummy[nPLAY_SKILL]->GetGlobalPos () );
		pControl->SetLeftTime      ( fAGE );
        pControl->SetLifeTime      ( fLIFE );
		pControl->SetEnable        ( bEnable );
		pControl->SetDetailVisible ( FALSE );

		nPLAY_SKILL++;
	} //for

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

bool CSkillTimeDisplay::GET_PLAY_SKILLFACT ( const DWORD& dwSkillID, SSKILLFACT& sSKILLFACT )
{
	GLCharacter* const		pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter )
		return false;

	DWORD		dwSize, dwStart;
	if( IS_ADDITIONAL_ID( dwSkillID ) )
	{
		dwSize	= SKILLFACT_SIZE_ADDITIONAL;
		dwStart	= SKILLFACT_INDEX_ADDITIONAL_START;
	}
	else
	{
		dwSize	= SKILLFACT_SIZE_NORMAL;
		dwStart	= SKILLFACT_INDEX_NORMAL_START;
	} //if..else

	for( size_t i = 0; i < dwSize; ++i )
	{
		size_t nIndex = i + dwStart;
        if( nIndex >= SKILLFACT_SIZE )
            continue;

		const SSKILLFACT& sSkillFactIter = pCharacter->m_sSKILLFACT[nIndex];
		if( sSkillFactIter.sNATIVEID == dwSkillID )
		{
			sSKILLFACT = sSkillFactIter;
			return true;
		} //if
	} //for

	return false;
}

HRESULT CSkillTimeDisplay::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

bool CSkillTimeDisplay::NEW_PLAY_SKILL ( const DWORD& dwSkillID, const WORD wLEVEL, const WORD wSlot )
{
	m_NEWID++;
	if( SKILLTIME_UNIT_END <= m_NEWID )
		m_NEWID = SKILLTIME_UNIT;

	float fLifeTime = 0.0f;
	if( !GET_SKILL_LIFE_TIME( dwSkillID, wLEVEL, fLifeTime ) )
		return false;

	SNATIVEID	sNativeID ( dwSkillID );
	PGLSKILL	pSkill = GLSkillMan::GetInstance().GetData( sNativeID );
	if( !pSkill )
		return false;

	CSkillTimeUnit*		pSkillTimeUnit = new CSkillTimeUnit( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pSkillTimeUnit->CreateSub( this, "SKILLTIME_UNIT", UI_FLAG_DEFAULT, m_NEWID );
	pSkillTimeUnit->CreateSubControl();
	pSkillTimeUnit->InitDeviceObjects( m_pd3dDevice );
	pSkillTimeUnit->RestoreDeviceObjects( m_pd3dDevice );	
	pSkillTimeUnit->SetSkill( dwSkillID, wLEVEL, wSlot, fLifeTime, pSkill->GetName() );
	RegisterControl( pSkillTimeUnit );		

	pSkillTimeUnit->SetEnable( TRUE  );
	pSkillTimeUnit->SetDetailVisible( FALSE );

	SKILLTIME_PAIR	pair(dwSkillID,m_NEWID);
	m_SkillPlayList.push_back( pair );

	return true;
}

bool CSkillTimeDisplay::NEW_PLAY_ADDITIONAL( const DWORD dwOption, const float fTimeLength )
{
	m_NEWID++;
	if( SKILLTIME_UNIT_END <= m_NEWID )
		m_NEWID = SKILLTIME_UNIT;

	CSkillTimeUnit*		pSkillTimeUnit = new CSkillTimeUnit( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pSkillTimeUnit->CreateSub( this, "SKILLTIME_UNIT", UI_FLAG_DEFAULT, m_NEWID );
	pSkillTimeUnit->CreateSubControl();
	pSkillTimeUnit->InitDeviceObjects( m_pd3dDevice );
	pSkillTimeUnit->RestoreDeviceObjects( m_pd3dDevice );	
	pSkillTimeUnit->SetAdditional( SKILL_TO_ADDITIONAL( dwOption ), fTimeLength );
	RegisterControl( pSkillTimeUnit );	

	pSkillTimeUnit->SetEnable( TRUE );
	pSkillTimeUnit->SetDetailVisible( FALSE );

	SKILLTIME_PAIR		pair( dwOption, m_NEWID );
	m_SkillPlayList.push_back( pair );

	return true;
} //CSkillTimeDisplay::NEW_PLAY_ADDITIONAL

bool CSkillTimeDisplay::GET_SKILL_LIFE_TIME ( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime )
{
	SNATIVEID sNativeID ( dwSkillID );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
	if ( !pSkill )
		return false;

    GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter(); //봉주
    if ( !pCharacter )
		return false;

    for( int i = 0; i < SKILLFACT_SIZE; ++i) //봉주
    {
        const SSKILLFACT& sSKILLEF = pCharacter->GetSkillFact(i);
        if ( sNativeID == sSKILLEF.sNATIVEID )
            fLifeTime = sSKILLEF.fLIFE;         
    }

	//	최대 지연시간
	//fLifeTime = pSkill->m_sAPPLY.sDATA_LVL[wLEVEL].fLIFE;

	fLifeTime = (fLifeTime)?fLifeTime:1.0f;

	return true;
}
