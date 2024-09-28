#include "StdAfx.h"
#include "SystemBuffTimeDisplay.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "SkillTimeUnit.h"
#include "../../../RanLogic/Skill/GLSkill.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSystemBuffTimeDisplay::CSystemBuffTimeDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_NEWID ( SYSTEMBUFFTIME_UNIT )
{
}

CSystemBuffTimeDisplay::~CSystemBuffTimeDisplay ()
{
}

void CSystemBuffTimeDisplay::CreateSubControl ()
{	
    const CString strKeyword = "SYSTEMBUFF_TIME_UNIT";
    CString strCombine;

    for ( int i=0; i<SKILLFACT_SIZE_SYSTEM; i++ )
    {
        strCombine.Format ( "%s%d", strKeyword, i );
        CUIControl* pDummyControl = new CUIControl(m_pEngineDevice);
        pDummyControl->CreateSub ( this, strCombine.GetString(), UI_FLAG_DEFAULT, SYSTEMBUFFTIME_DUMMY_BASE + i );
        RegisterControl ( pDummyControl );

        m_pSystemBuffUnitDummy[i] = pDummyControl;
    }
}

void CSystemBuffTimeDisplay::SetEnableAll ( const BOOL bEnable )
{
    SYSTEMBUFFTIME_DISPLAY_LIST_ITER iter     = m_SystemBuffPlayList.begin ();
    SYSTEMBUFFTIME_DISPLAY_LIST_ITER iter_end = m_SystemBuffPlayList.end ();

    for ( ; iter != iter_end; ++iter )
    {
        const SYSTEMBUFFTIME_PAIR& skill_pair = (*iter);
        const DWORD& dwID = skill_pair.first.first;
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

UIGUID CSystemBuffTimeDisplay::GET_EXIST_PLAY_SYSTEMBUFF_CONTROL ( const DWORD& dwSkillID )
{
    SYSTEMBUFFTIME_DISPLAY_LIST_ITER iter     = m_SystemBuffPlayList.begin ();
    SYSTEMBUFFTIME_DISPLAY_LIST_ITER iter_end = m_SystemBuffPlayList.end ();
    for ( ; iter != iter_end; ++iter )
    {
        const SYSTEMBUFFTIME_PAIR& pair = (*iter);
        const DWORD& dwID = pair.first.first;
        const UIGUID& cID = pair.second;
        if ( dwID == dwSkillID ) return cID;
    }

    return NO_ID;
}

void CSystemBuffTimeDisplay::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    // Blank
}

void CSystemBuffTimeDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible () ) return ;

    GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
    if ( !pCharacter ) return;

    //	삭제
    if ( !m_SystemBuffPlayList.empty () )
    {
        SYSTEMBUFFTIME_DISPLAY_LIST_ITER iter     = m_SystemBuffPlayList.begin ();
        SYSTEMBUFFTIME_DISPLAY_LIST_ITER iter_end = m_SystemBuffPlayList.end ();

        for ( ; iter != iter_end; )
        {
            const SYSTEMBUFFTIME_PAIR& SystemBuff_pair = (*iter);
            const DWORD& dwID = SystemBuff_pair.first.first;
            const WORD& wLEVEL = SystemBuff_pair.first.second;
            const UIGUID& cID = SystemBuff_pair.second;

            //	찾기			
            SSKILLFACT sSKILLFACT;
            if ( !GET_PLAY_SYSTEMBUFF ( dwID, wLEVEL, sSKILLFACT ) )
            {				
                DeleteControl ( cID, 0 );
                iter = m_SystemBuffPlayList.erase ( iter );
            }
            else ++iter;
        }
    }

    //	등록
    {		
        for (size_t i=0; i<SKILLFACT_SIZE_SYSTEM; ++i)
        {
            size_t nIndex = i+SKILLFACT_INDEX_SYSTEM_START;
            if (nIndex >= SKILLFACT_SIZE)
                continue;

            const SSKILLFACT& sSKILLFACT = pCharacter->m_sSKILLFACT[nIndex];
            const DWORD& dwID = sSKILLFACT.sNATIVEID.dwID;
            if (dwID == NATIVEID_NULL().dwID)
                continue;

            UIGUID cID = GET_EXIST_PLAY_SYSTEMBUFF_CONTROL ( dwID );
            if (cID == NO_ID)
                NEW_PLAY_SYSTEMBUFF ( dwID, sSKILLFACT.wLEVEL, static_cast< WORD >( nIndex ) );
        }
    }

    if ( m_SystemBuffPlayList.empty () ) return ;

    //	업데이트
    int nPLAY_SKILL = 0;
    SYSTEMBUFFTIME_DISPLAY_LIST_ITER iter     = m_SystemBuffPlayList.begin ();
    SYSTEMBUFFTIME_DISPLAY_LIST_ITER iter_end = m_SystemBuffPlayList.end ();

    for ( ; iter != iter_end; ++iter )
    {
        const SYSTEMBUFFTIME_PAIR& skill_pair = (*iter);
        const DWORD& dwID = skill_pair.first.first;
        const WORD& wLEVEL = skill_pair.first.second;
        const UIGUID& cID = skill_pair.second;			

        CSkillTimeUnit* pControl = (CSkillTimeUnit*) FindControl ( cID );
        if ( !pControl )
        {
            GASSERT ( 0 && "컨트롤 찾기 실패" );
            continue;
        }

        //	찾기			
        SSKILLFACT sSKILLFACT;
        if ( !GET_PLAY_SYSTEMBUFF ( dwID, wLEVEL, sSKILLFACT ) )
        {
            GASSERT ( 0 && "스킬 데이타 찾기 실패" );
            continue;
        }

        const float& fAGE    = sSKILLFACT.fAGE;
        const BOOL&  bEnable = sSKILLFACT.bEnable;

        pControl->SetGlobalPos     ( m_pSystemBuffUnitDummy[nPLAY_SKILL]->GetGlobalPos () );
        pControl->SetLeftTime      ( fAGE );
        pControl->SetEnable        ( bEnable );
        pControl->SetDetailVisible ( TRUE );

        nPLAY_SKILL++;
    }

    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

bool CSystemBuffTimeDisplay::GET_PLAY_SYSTEMBUFF ( const DWORD& dwSkillID, const WORD& wLevel, SSKILLFACT& sSKILLFACT )
{
    GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
    if ( !pCharacter ) return false;

    for (size_t i=0; i<SKILLFACT_SIZE_SYSTEM; ++i)
    {
        size_t nIndex = i+SKILLFACT_INDEX_SYSTEM_START;
        if (nIndex >= SKILLFACT_SIZE)
            continue;

        const SSKILLFACT& sSkillFactIter = pCharacter->m_sSKILLFACT[nIndex];
        if ( (sSkillFactIter.sNATIVEID == dwSkillID) && (sSkillFactIter.wLEVEL == wLevel) )
        {
            sSKILLFACT = sSkillFactIter;
            return true;
        }
    }

    return false;
}

HRESULT CSystemBuffTimeDisplay::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;
    return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

bool CSystemBuffTimeDisplay::NEW_PLAY_SYSTEMBUFF ( const DWORD& dwSkillID, const WORD wLEVEL, const WORD wSlot )
{
    m_NEWID++;
    if ( SYSTEMBUFFTIME_UNIT_END <= m_NEWID ) m_NEWID = SYSTEMBUFFTIME_UNIT;

    float fLifeTime = 0.0f;
    if ( !GET_SYSTEMBUFF_LIFE_TIME ( dwSkillID, wLEVEL, fLifeTime ) ) return false;

    SNATIVEID sNativeID ( dwSkillID );
    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
    if ( !pSkill ) return false;

    CSkillTimeUnit* pSkillTimeUnit = new CSkillTimeUnit(m_pGaeaClient, m_pInterface, m_pEngineDevice);
    pSkillTimeUnit->CreateSub ( this, "SYSTEMBUFF_TIME_UNIT", UI_FLAG_DEFAULT, m_NEWID );
    pSkillTimeUnit->CreateSubControl ();
    pSkillTimeUnit->InitDeviceObjects ( m_pd3dDevice );
    pSkillTimeUnit->RestoreDeviceObjects ( m_pd3dDevice );	
    pSkillTimeUnit->SetSkill ( dwSkillID, wLEVEL, wSlot, fLifeTime, pSkill->GetName() );
    RegisterControl ( pSkillTimeUnit );	

    pSkillTimeUnit->SetEnable        ( TRUE );
    pSkillTimeUnit->SetDetailVisible ( TRUE );

    SYSTEMBUFFTIME_PAIR pair ( std::make_pair(dwSkillID, wLEVEL),m_NEWID);
    m_SystemBuffPlayList.push_back ( pair );

    return true;
}

bool CSystemBuffTimeDisplay::GET_SYSTEMBUFF_LIFE_TIME ( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime )
{
    SNATIVEID sNativeID ( dwSkillID );

    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );
    if ( !pSkill ) return false;

    //	최대 지연시간
    fLifeTime = pSkill->m_sAPPLY.sDATA_LVL[wLEVEL].fLIFE;
    fLifeTime = (fLifeTime)?fLifeTime:1.0f;

    return true;
}

MySystemBuffTimeDisplay::MySystemBuffTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CSystemBuffTimeDisplay( pGaeaClient, pInterface, pEngineDevice )
{
}

void MySystemBuffTimeDisplay::CreateUIWindowAndRegisterOwnership()
{
    CSystemBuffTimeDisplay::Create( SYSTEMBUFF_TIME_DISPLAY, "SYSTEMBUFF_TIME_DISPLAY", UI_FLAG_RIGHT );
    CSystemBuffTimeDisplay::CreateSubControl();
    CSystemBuffTimeDisplay::m_pInterface->UiRegisterControl( this );
    CSystemBuffTimeDisplay::m_pInterface->UiShowGroupBottom( SYSTEMBUFF_TIME_DISPLAY );
}