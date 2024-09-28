#include "StdAfx.h"
#include "ReferCharSkillTab.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Skill/GLSkill.h"

#include "../../../RanLogicClient/ReferChar/ReferChar.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Display/SkillInforTooltip.h"

#include "ReferSkillSlot.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CReferCharSkillTab::CReferCharSkillTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroupHelper(pEngineDevice)

	, m_pInterface( pInterface )
    , m_pGaeaClient( pGaeaClient )

	, m_bInit( false )
	, m_pTarget( NULL )

	, m_pScrollBar( NULL )

	, m_nMouseX( 0 )
	, m_nMouseY( 0 )

	, m_nViewPos( 0 )
{
	for( int i=0; i<TAB_COUNT; ++i )
		m_pSkillPageText[ i ] = NULL;

	for( int i=0; i<SKILLSLOT_COUNT; ++i )
		m_SkillSlot[ i ] = NULL;
}

CReferCharSkillTab::~CReferCharSkillTab()
{
}

void CReferCharSkillTab::CreateSubControl()
{
	m_bInit = false;

    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
    CreateLineBox( "ANOTHER_SKILL_REGION", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );

	// 20칸 영역 만들기
    for( int i=0; i<SKILLSLOT_COUNT; ++i )
    {
        CString strSlotName;
        strSlotName.Format( "ANOTHER_SKILL_SLOT_%d", i );

        CReferSkillSlot* pSkillSlot = new CReferSkillSlot( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pSkillSlot->CreateSub ( this, strSlotName, UI_FLAG_DEFAULT, ID_SKILL_SLOT_RECT_CONTROL_START + i );
        pSkillSlot->CreateSubControl();
        pSkillSlot->SetVisibleSingle( TRUE );
        RegisterControl( pSkillSlot );

        m_SkillSlot[i] = pSkillSlot;
    }

    {  
        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "ANOTHER_SKILL_TAB_0_TEXT" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pStaticText );
        m_pSkillPageText[0] = pStaticText;

        // 버튼
        DEFAULT_RADIO_BUTTON* pRadio1 = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
        pRadio1->CreateSub( this, "ANOTHER_SKILL_TAB_0", UI_FLAG_DEFAULT, ID_SKILL_TAB_0 );
        pRadio1->CreateSubControl( " "
			,TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y
            ,"CHARACTER_WINDOW_HISTORY_RADIO_BUTTON_TEXT"
            ,NS_UITEXTUTIL::BLACK
            ,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX"
            ,"CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO"
            ,"CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO" );

        RegisterTapButton( pRadio1 );
        RegisterControl ( pRadio1 );
    }

    {  
        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "ANOTHER_SKILL_TAB_1_TEXT" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pStaticText );
        m_pSkillPageText[1] = pStaticText;

        // 버튼
        DEFAULT_RADIO_BUTTON* pRadio2 = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
        pRadio2->CreateSub( this, "ANOTHER_SKILL_TAB_1", UI_FLAG_DEFAULT, ID_SKILL_TAB_1 );
        pRadio2->CreateSubControl( " "
			,TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y
            ,"CHARACTER_WINDOW_HISTORY_RADIO_BUTTON_TEXT"
            ,NS_UITEXTUTIL::BLACK
            ,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX"
            ,"CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO"
            ,"CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO" );

        RegisterTapButton( pRadio2 );
        RegisterControl ( pRadio2 );
    }

    {  
        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "ANOTHER_SKILL_TAB_2_TEXT" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pStaticText );
        m_pSkillPageText[2] = pStaticText;

        // 버튼
        DEFAULT_RADIO_BUTTON* pRadio3 = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
        pRadio3->CreateSub( this, "ANOTHER_SKILL_TAB_2", UI_FLAG_DEFAULT, ID_SKILL_TAB_2 );
        pRadio3->CreateSubControl( " "
			,TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y
            ,"CHARACTER_WINDOW_HISTORY_RADIO_BUTTON_TEXT"
            ,NS_UITEXTUTIL::BLACK
            ,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX"
            ,"CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO"
            ,"CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO" );

        RegisterTapButton( pRadio3 );
        RegisterControl ( pRadio3 );
    }

    {
        //텍스트
        CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
        pStaticText->CreateSub( this, "ANOTHER_SKILL_TAB_3_TEXT" );
        pStaticText->SetFont( pFont9 );
        pStaticText->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pStaticText );
        m_pSkillPageText[3] = pStaticText;

        // 버튼 ( 공통 버튼 )
        DEFAULT_RADIO_BUTTON* pRadio4 = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
        pRadio4->CreateSub( this, "ANOTHER_SKILL_TAB_3", UI_FLAG_DEFAULT, ID_SKILL_TAB_3 );
        pRadio4->CreateSubControl( " "
			,TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y
            ,"CHARACTER_WINDOW_HISTORY_RADIO_BUTTON_TEXT"
            ,NS_UITEXTUTIL::BLACK
            ,"CHARACTER_WINDOW_RADIO_BUTTON_CHECK_BOX"
            ,"CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO"
            ,"CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO" );

        RegisterTapButton( pRadio4 );
        RegisterControl ( pRadio4 );
    }

	// 스크롤바
    {
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, ID_SKILL_SCROOLBAR );
        pScrollBar->CreateBaseScrollBar( "ANOTHER_SKILL_SLOT_SCROOL" );
        RegisterControl ( pScrollBar );
        m_pScrollBar = pScrollBar;
    }

	m_bInit = true;

	EventSelectedTap( ID_SKILL_TAB_0 );
}

void CReferCharSkillTab::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if( !m_pTarget )
		return;

    EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
    TapSelectorUIMessage( ControlID, dwMsg, emResult );
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

    if( ID_SKILL_SLOT_RECT_CONTROL_START <= ControlID &&
		ControlID <= ID_SKILL_SLOT_RECT_CONTROL_END )
    {
        if( CHECK_MOUSE_IN ( dwMsg ) )
        {
			UINT nIndex = ControlID - ID_SKILL_SLOT_RECT_CONTROL;
			SNATIVEID sNativeID = m_SkillSlot[ nIndex ]->GetNativeSkillID();
			if( sNativeID != NATIVEID_NULL() )
				m_pInterface->SHOW_SKILL_INFO( sNativeID, FALSE );
        }

		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
		{
			UINT nIndex = ControlID - ID_SKILL_SLOT_RECT_CONTROL;
			SNATIVEID sNativeID = m_SkillSlot[ nIndex ]->GetNativeSkillID();
			if( sNativeID != NATIVEID_NULL() )
				m_pInterface->ShowSkillToolTipMulti( m_pTarget, sNativeID, m_nMouseX, m_nMouseY );
		}
    }
}

void CReferCharSkillTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_nMouseX = x;
	m_nMouseY = y;

    UINT nTotal = static_cast< UINT >( m_vecCurrentSkill.size() );
    int nVisibleLine = SKILLSLOT_COUNT;

    CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();

    const int nMovableLine = nTotal - nVisibleLine;
    float fPercent = pThumbFrame->GetPercent();
    m_nViewPos = (int)floor( fPercent * nMovableLine );
    
    ( m_nViewPos%2 != 0 ? m_nViewPos++ : m_nViewPos );

    if( m_nViewPos < 0 )
		m_nViewPos = 0;

    for( int i=0; i<nVisibleLine; ++i )
    {
        UINT nViewDataPos = m_nViewPos + i;
        if( m_pTarget && nViewDataPos < nTotal )
        {
			SNATIVEID sSkillID = m_vecCurrentSkill[ nViewDataPos ]->GetId();
			SCHARSKILL* pCharSkill = m_pTarget->GETLEARNED_SKILL( sSkillID );

			WORD nLevel = 0;
			BOOL bMaster = FALSE;

			if( pCharSkill )
			{
				nLevel = pCharSkill->wLevel + 1;
				bMaster = m_pTarget->ISMASTER_SKILL( sSkillID );
			}

            m_SkillSlot[ i ]->SetNativeSkillID(	sSkillID, nLevel, bMaster );

			m_SkillSlot[ i ]->SetVisibleSingle( TRUE );
        } 
        else
        {
            m_SkillSlot[ i ]->SetVisibleSingle( FALSE );
        }
    }
}

void CReferCharSkillTab::Refresh()
{
	if( !m_bInit )
		return;

    ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
    if( !pReferChar )
        return;

    m_pTarget = pReferChar->GetCompleteData();

    EventSelectedTap( GetTapID() );
}

void CReferCharSkillTab::EventSelectedTap( UIGUID controlID )
{
	if( !m_bInit )
		return;

    if( ID_SKILL_TAB_0 <= controlID &&  controlID <= ID_SKILL_TAB_3 )
    {
        UINT nPage = controlID - ID_SKILL_TAB_0;
        InitSkillPage( nPage );
    }
}

void GetSkillIndexRange( EMCHARCLASS eClass, int& nBegin, int& nEnd )
{	
   switch( eClass )
   {
   case GLCC_FIGHTER_M:
   case GLCC_FIGHTER_W:
       {
           nBegin = EMSKILL_FIGHTER_01;
           nEnd = EMSKILL_FIGHTER_04;
       }
       break;

   case GLCC_ARMS_M:
   case GLCC_ARMS_W:
       {
           nBegin = EMSKILL_ARMS_01;
           nEnd = EMSKILL_ARMS_04;
       }
       break;

   case GLCC_ARCHER_M:
   case GLCC_ARCHER_W:
       {
           nBegin = EMSKILL_ARCHER_01;
           nEnd = EMSKILL_ARCHER_04;
       }
       break;

   case GLCC_SPIRIT_M:
   case GLCC_SPIRIT_W:
       {
           nBegin = EMSKILL_SPIRIT_01;
           nEnd = EMSKILL_SPIRIT_04;
       }
       break;	

   case GLCC_EXTREME_M:
   case GLCC_EXTREME_W:
       {
           nBegin = EMSKILL_EXTREME_01;
           nEnd = EMSKILL_EXTREME_04;
       }
       break;
   case GLCC_SCIENTIST_M:
   case GLCC_SCIENTIST_W:
       {
           nBegin = EMSKILL_SCIENTIST_01;
           nEnd = EMSKILL_SCIENTIST_04;			
       }
       break;
   case GLCC_ASSASSIN_M:
   case GLCC_ASSASSIN_W:
       {
           nBegin = EMSKILL_ASSASSIN_01;
           nEnd = EMSKILL_ASSASSIN_04;			
       }
       break;	   
   case GLCC_TRICKER_M:
   case GLCC_TRICKER_W:
	   {
		   nBegin = EMSKILL_TRICKER_01;
		   nEnd = EMSKILL_TRICKER_04;			
	   }
	   break;
   case GLCC_ETC_M:
   case GLCC_ETC_W:
       {
           nBegin = EMSKILL_ETC_01;
           nEnd = EMSKILL_ETC_04;			
       }
       break;
   }
}

void CReferCharSkillTab::InitSkillPage( UINT nPage )
{
	m_vecCurrentSkill.clear();

	for( int i=0; i<TAB_COUNT; ++i )
        m_pSkillPageText[i]->ClearText();

	if( !m_pTarget )
        return;

	EMCLASSINDEX eClassIndex = CharClassToClassIndex( m_pTarget->m_emClass );

	const CString strClassKeyword[ GLCL_NUM_CLASS ] =
    {
        "SKILL_TAB_TITLE_FIGHTER",
        "SKILL_TAB_TITLE_ARMS",
        "SKILL_TAB_TITLE_ARCHER",
        "SKILL_TAB_TITLE_SPIRIT",
        "SKILL_TAB_TITLE_EXTREME_M",
        "SKILL_TAB_TITLE_SCIENTIST",
        "SKILL_TAB_TITLE_ASSASSIN",
		"SKILL_TAB_TITLE_TRICKER",
        "SKILL_TAB_TITLE_ETC",
    };

    CString strClassText = strClassKeyword[ eClassIndex ];

	DWORD nSkillPoint[ TAB_COUNT ] = { 0, 0, 0, 0 };
	DWORD nSkillPointMax = 0;

    for( UINT i=0; i<TAB_COUNT; ++i )
    {
		SkillInfoVec vecSkill;
		GetSkillList( i, vecSkill );

		nSkillPoint[ i ] = GetSkillPoint( vecSkill );
		nSkillPointMax = max( nSkillPointMax, nSkillPoint[ i ] );

		CString strTapName;
        strTapName.Format(
			"   %s(%d)",
			ID2GAMEWORD( strClassText.GetString(), i ),
			nSkillPoint[ i ] );
        m_pSkillPageText[i]->SetText( strTapName, NS_UITEXTUTIL::WHITE );

		if( i == nPage )
        {
            m_vecCurrentSkill = vecSkill;

			// 스크롤 최대 크기 설정
            int nMAX = static_cast< UINT >( m_vecCurrentSkill.size() );
            m_pScrollBar->GetThumbFrame()->SetState( nMAX, SKILLSLOT_COUNT );
        }
	}

	if( nSkillPointMax >= TAB_HIGHLIGHT )
	{
		for( UINT i=0; i<TAB_COUNT; ++i )
		{
			if( nSkillPoint[ i ] == nSkillPointMax )
			{
				const DWORD nColor = D3DCOLOR_XRGB( 0xE9, 0xCE, 0x00 );
				m_pSkillPageText[ i ]->SetTextColor( 0, nColor );
			}
		}
	}
}

void CReferCharSkillTab::GetSkillList( UINT nPage, SkillInfoVec& vecSkill )
{
	int nBegin = 0;
	int nEnd = 0;
	GetSkillIndexRange( m_pTarget->m_emClass, nBegin, nEnd );

	int nSelectPage = nBegin + nPage;
	if( nSelectPage > nEnd )
		return;

	WORD nClass = 0;
	WORD nIndex = 0;
	GLSkillMan::GetInstance().GetMaxSkill( nClass, nIndex );

	for( int i=0; i<nIndex; ++i )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( nSelectPage, i );
		if( pSkill )
			vecSkill.push_back( pSkill );
	}

	if( vecSkill.empty() )
		return;

	class CCompareSkill
	{
	public:
		bool operator() ( const PGLSKILL lvalue, const PGLSKILL rvalue )
		{
			if( lvalue->m_sBASIC.dwGRADE < rvalue->m_sBASIC.dwGRADE )
			{
				return true;
			}
			else if( lvalue->m_sBASIC.dwGRADE == rvalue->m_sBASIC.dwGRADE )
			{
				if( lvalue->m_sBASIC.sNATIVEID.wSubID == rvalue->m_sBASIC.sNATIVEID.wSubID )
					return true;
			}
			return false;
		}
	};

	std::sort( vecSkill.begin(), vecSkill.end(), CCompareSkill() );
}

DWORD CReferCharSkillTab::GetSkillPoint( SkillInfoVec& vecSkill )
{
	DWORD nPoint = 0;

	BOOST_FOREACH( PGLSKILL pSkill, vecSkill )
	{
		SCHARDATA2::SKILL_MAP_ITER it = m_pTarget->m_ExpSkills.find( pSkill->GetId().dwID );
		if( it == m_pTarget->m_ExpSkills.end() )
			continue;

		const SCHARSKILL& sSkill = it->second;

		for( WORD i=0; i<=sSkill.wLevel; ++i )
			nPoint += pSkill->m_sLEARN.sLVL_STEP[ i ].dwSKP;
	}

	return nPoint;
}