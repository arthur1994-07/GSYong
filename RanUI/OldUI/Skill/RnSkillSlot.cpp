#include "StdAfx.h"
#include "RnSkillSlot.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../Util/RnButton.h"
#include "../Skill/SkillSlot.h"
#include "../Skill/SkillImage.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    // 임시 스트링 값
    const char* ID2GAMEWORD_RNSKILL_NEED_LEVEL;
    const char* ID2GAMEWORD_RNSKILL_NEED_SKILL;
    const char* ID2GAMEWORD_RNSKILL_NEED_POINT;
    const char* ID2GAMEWORD_RNSKILL_NEED_GOLD;

}


CRnSkillSlot::CRnSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
    , m_pSkillImage( NULL )
    , m_pShadow( NULL )
    , m_pTextBoxLine0( NULL )
    , m_pTextBoxLine1( NULL )
    , m_pRnButtonPlus( NULL )
    , m_pRnButtonMinus( NULL )
    , m_pSkillInfo( NULL )
{
    ID2GAMEWORD_RNSKILL_NEED_LEVEL = ID2GAMEWORD("RNSKILLWINDOW_SLOT", 0 );
    ID2GAMEWORD_RNSKILL_NEED_SKILL = ID2GAMEWORD("RNSKILLWINDOW_SLOT", 1 );
    ID2GAMEWORD_RNSKILL_NEED_POINT = ID2GAMEWORD("RNSKILLWINDOW_SLOT", 2 );
    ID2GAMEWORD_RNSKILL_NEED_GOLD = ID2GAMEWORD("RNSKILLWINDOW_SLOT", 3 ); 
}


void CRnSkillSlot::CreateSubControl()
{
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    //테두리와 배경
    CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
    pLinBoxSmart->CreateSub( this, "RNSKILL_SLOT_SIZE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_SLOT );
    pLinBoxSmart->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );
    RegisterControl( pLinBoxSmart );

    CUIControl* pControl = new CUIControl( m_pEngineDevice );
    pControl->CreateSub( this, "RNSKILL_SLOT_ACTIVE" );
    RegisterControl ( pControl );
    m_pShadow = pControl;
    m_pShadow->SetDiffuseAlpha( 80 );

    { 
        // +버튼
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );
        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////
        arg.pFont = pFont10Shadow;
        arg.dwColor = NS_UITEXTCOLOR::WHITE;

        arg.text = "+";
        m_pRnButtonPlus = new RnButton(m_pEngineDevice);
        m_pRnButtonPlus->CreateSub(this, "RNSKILL_SKILL_SLOT_BUTTON_PLUS", UI_FLAG_DEFAULT, ID_SLOT_BUTTON_PLUS );
        m_pRnButtonPlus->CreateSubControl(arg);
        RegisterControl(m_pRnButtonPlus);

        arg.text = "-";
        m_pRnButtonMinus = new RnButton(m_pEngineDevice);
        m_pRnButtonMinus->CreateSub(this, "RNSKILL_SKILL_SLOT_BUTTON_MINUS", UI_FLAG_DEFAULT,ID_SLOT_BUTTON_MINUS );
        m_pRnButtonMinus->CreateSubControl(arg);
        RegisterControl(m_pRnButtonMinus);

    }

    //스킬 아이콘 이미지
    {
        CSkillImage* pSkillImage = new CSkillImage ( m_pGaeaClient, m_pEngineDevice );
        pSkillImage->CreateSub( this, "RNSKILL_SKILL_SLOT_IMAGE", UI_FLAG_DEFAULT, ID_SLOT_IMAGE );
        pSkillImage->CreateSubControl();
        pSkillImage->SetUseRender( TRUE );
        RegisterControl( pSkillImage );
        m_pSkillImage = pSkillImage;
    }

    //텍스트 첫 번째 라인
    {
        m_pTextBoxLine0 = new CBasicTextBoxEx(m_pEngineDevice);
        m_pTextBoxLine0->CreateSub( this, "RNSKILL_SKILL_SLOT_TEXT0" );
        m_pTextBoxLine0->SetFont( pFont9 );
        m_pTextBoxLine0->SetLineInterval( 4.0f );
        RegisterControl( m_pTextBoxLine0 );
    }

    //텍스트 두 번째 라인
    {
        m_pTextBoxLine1 = new CBasicTextBoxEx(m_pEngineDevice);
        m_pTextBoxLine1->CreateSub( this, "RNSKILL_SKILL_SLOT_TEXT1" );
        m_pTextBoxLine1->SetFont( pFont9 );
        m_pTextBoxLine1->SetLineInterval( 4.0f );
        RegisterControl( m_pTextBoxLine1 );
    }
    
}


void CRnSkillSlot::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    if( m_pSkillInfo == NULL )
        return;

    switch( ControlID )
    {
    case ID_SLOT_IMAGE:
        if( CHECK_MOUSE_IN ( dwMsg ) )
        {
            if( m_pSkillInfo->pSkill->GetId() != NATIVEID_NULL() )
                m_pInterface->SHOW_SKILL_INFO( m_pSkillInfo->pSkill->GetId(), FALSE, static_cast<WORD>( m_pSkillInfo->dwTempSkillLevel ) );

            if ( CHECK_LB_DOWN_LIKE( dwMsg ) )
            {
                PGLSKILL pSkill = m_pSkillInfo->pSkill;
                if ( !pSkill )
                {
                    GASSERT ( 0 && "찾을 수 없는 스킬입니다." );
                    return ;
                }
                // 스킬 Lv.0 일 경우 들 수 없다.
                if ( m_pSkillInfo->dwLevel <= 0) 
                    return;

                //	패시브 스킬은 들 수 없다.
                if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ) 
                    return;

                ISkillWindowToTray* pSkillWindowToTray = m_pInterface->GetSkillWindowToTray();
                if ( !pSkillWindowToTray ) 
                    return;

                pSkillWindowToTray->SetSkill( pSkill->GetId() );
                m_pInterface->SkilSlotTrayOpen();
            }

			if ( CHECK_RB_UP_LIKE( dwMsg ) )
			{
				PGLSKILL pSkill = m_pSkillInfo->pSkill;
				if ( !pSkill )
				{
					GASSERT ( 0 && "찾을 수 없는 스킬입니다." );
					return ;
				}
				// 스킬 Lv.0 일 경우 들 수 없다.
				if ( m_pSkillInfo->dwLevel <= 0) 
					return;

				//	패시브 스킬은 들 수 없다.
				if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ) 
					return;

				//빈슬롯을 찾기위해서 NATIVEID_NULL()과 비교를 한다
				WORD wQUICKSKILLSLOT = m_pInterface->GetSkillTrayTab()->FindSkillSlot(NATIVEID_NULL());
				m_pGaeaClient->GetCharacter()->ReqSkillQuickSet( m_pSkillInfo->pSkill->GetId(), wQUICKSKILLSLOT );
				m_pInterface->SkillWindowToTrayResetSkill();
			}
        }
        break;

    case ID_SLOT_BUTTON_PLUS:
        if ( CHECK_MOUSE_IN ( dwMsg ) )
        {
            const bool bNextLevel = !m_pSkillInfo->bMaster; //임시 스킬이 마스터레벨일 경우는 현재레벨 툴팁을 보여줌
            if( m_pSkillInfo->pSkill->GetId() != NATIVEID_NULL() )
                m_pInterface->SHOW_SKILL_INFO( m_pSkillInfo->pSkill->GetId(), bNextLevel, static_cast<WORD>( m_pSkillInfo->dwTempSkillLevel ) );
            if( CHECK_LB_UP_LIKE ( dwMsg ))
                AddMessageEx( UIMSG_RNSKILL_UP_CLICK );
            else if( CHECK_RB_UP_LIKE ( dwMsg ))
                AddMessageEx( UIMSG_RNSKILL_DOWN_CLICK );
        }
        break;

    case ID_SLOT_BUTTON_MINUS:
        if ( CHECK_MOUSE_IN ( dwMsg ) )
        {
            if( CHECK_LB_UP_LIKE ( dwMsg ))
                AddMessageEx( UIMSG_RNSKILL_DOWN_CLICK );
            else if( CHECK_RB_UP_LIKE ( dwMsg ))
                AddMessageEx( UIMSG_RNSKILL_UP_CLICK );
        }
        break;

    }    
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

}

void CRnSkillSlot::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void CRnSkillSlot::SetSkillInfo( SKILLINFO* pSkillInfo, BOOL bRollover )
{
    m_pSkillInfo = pSkillInfo;

    if( m_pSkillInfo == NULL)
        return;

    if( !m_pSkillImage )
        return;

    if( !m_pTextBoxLine0 && !m_pTextBoxLine1 )
        return;

    m_pSkillImage->ResetSkill();


    m_pSkillImage->SetSkill( m_pSkillInfo->pSkill->GetId() );

    SetSlotState();         // UI상태를 기록하고
    RollOver( bRollover );  // 롤오버 여부에 따라서 내용을 출력한다.
}


const SKILLINFO& CRnSkillSlot::GetSkillInfo()
{
    return *m_pSkillInfo;
}


void CRnSkillSlot::SetSlotState()
{
    switch( m_pSkillInfo->nState )
    {
    case STATE_IMPOSSIBLE:
        m_pShadow->SetVisibleSingle( FALSE );
        m_pRnButtonPlus->SetVisibleSingle( FALSE );
        m_pRnButtonMinus->SetVisibleSingle( FALSE );
        m_pSkillImage->SetDiffuseAlpha( 128 );

		if( 0 < m_pSkillInfo->dwLevel )
		{
			m_pSkillImage->SetDiffuseAlpha( 255 );

			if( m_pSkillInfo->bMaster != TRUE)
			{
				m_pRnButtonPlus->SetVisibleSingle( TRUE );
				m_pRnButtonMinus->SetVisibleSingle( TRUE );
				m_pRnButtonPlus->SetDiffuseAlpha( 64 );
				m_pRnButtonMinus->SetDiffuseAlpha( 64 );
				break;
			}
		}

        if( 0 < m_pSkillInfo->dwTempSkillLevel )
        {
            m_pRnButtonPlus->SetVisibleSingle( TRUE );
            m_pRnButtonMinus->SetVisibleSingle( TRUE );
        }

        m_pRnButtonPlus->SetDiffuseAlpha( 64 );
        m_pRnButtonMinus->SetDiffuseAlpha( 255 );
        break;
    case STATE_DEACTIVATE:
        m_pShadow->SetVisibleSingle( TRUE );
        m_pRnButtonPlus->SetVisibleSingle( TRUE );
        m_pRnButtonMinus->SetVisibleSingle( TRUE );

        m_pSkillImage->SetDiffuseAlpha( 128 );
        m_pRnButtonPlus->SetDiffuseAlpha( 255 );
        m_pRnButtonMinus->SetDiffuseAlpha( 64 );

        break;
    case STATE_ACTIVATE:
        m_pShadow->SetVisibleSingle( TRUE );
        m_pRnButtonPlus->SetVisibleSingle( TRUE );
        m_pRnButtonMinus->SetVisibleSingle( TRUE );

        m_pSkillImage->SetDiffuseAlpha( 255 );
        m_pRnButtonPlus->SetDiffuseAlpha( 255 );
        m_pRnButtonMinus->SetDiffuseAlpha( 64 );        

        if( m_pSkillInfo->bMaster == TRUE )
        {
            m_pRnButtonPlus->SetVisibleSingle( FALSE );
            m_pRnButtonMinus->SetVisibleSingle( FALSE );
        } 
        break;
    case STATE_ACTIVATE_TEMP:
    case STATE_DEACTIVATE_TEMP:
        m_pShadow->SetVisibleSingle( TRUE );
        m_pRnButtonPlus->SetVisibleSingle( TRUE );
        m_pRnButtonMinus->SetVisibleSingle( TRUE );

        m_pSkillImage->SetDiffuseAlpha( 255 );
        m_pRnButtonPlus->SetDiffuseAlpha( 255 );
        m_pRnButtonMinus->SetDiffuseAlpha( 255 ); 

        if( m_pSkillInfo->bMaster == TRUE )
        {
            m_pRnButtonPlus->SetVisibleSingle( FALSE );    //마스터는 +버튼 미출력
        } 
        break;
    }

}


void CRnSkillSlot::RollOver( BOOL bRollover )
{
    if( m_pTextBoxLine0 == NULL || m_pTextBoxLine1 == NULL || m_pSkillInfo == NULL )
        return;

    CString strLine0;
    CString strLine1;
    DWORD dwColorLine0 = NS_UITEXTCOLOR::WHITE;
    DWORD dwColorLine1 = NS_UITEXTCOLOR::WHITE;

    SKILL_NEED& sNeed = m_pSkillInfo->sNeed;
    std::vector<CString> vecString;
    CString strTemp;

    // 임시 스킬에 사용된 스텟 계산
    switch( m_pSkillInfo->nState )
    {
    case STATE_ACTIVATE_TEMP:
    case STATE_DEACTIVATE_TEMP:
        // 기본 출력
        dwColorLine0 = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
        dwColorLine1 = D3DCOLOR_ARGB(0xFF,0xFF,0xCC,0x00);
        m_pShadow->SetDiffuseAlpha( 80 );   
        strLine1.Format("%d%s Lv. %d (%d)"
            , m_pSkillInfo->pSkill->m_sBASIC.dwGRADE
            , ID2GAMEWORD( "SKILL_SLOT", 0 )
            , m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel
            , m_pSkillInfo->dwUsingSkillPoint );

        // 마우스가 올라갈경우 출력이 다르다.
        if( bRollover == TRUE )
        {
            m_pShadow->SetDiffuseAlpha( 255 );   
            unsigned nNextLevel = m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel;
            if( m_pSkillInfo->bMaster == FALSE )
            {
                strLine1.Format("Lv. %d - Lv. %d (%d)"
                    , m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel
                    , m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel + 1
                    , m_pSkillInfo->pSkill->m_sLEARN.sLVL_STEP[ nNextLevel ].dwSKP );   
                //, m_pSkillInfo->dwUsingSkillPoint );
            }
            else
            {
                strLine1.Format("Lv. %d", m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel );
            }
        }
        
        // 마스터라면 + 버튼을 알파를 떨어뜨린다.
        if( m_pSkillInfo->bMaster == TRUE )
            m_pRnButtonPlus->SetDiffuseAlpha( 64 );

        break;

    case STATE_IMPOSSIBLE:
        if( 0 < sNeed.dwLevel )
        {
            strTemp.Format( "%s %d", ID2GAMEWORD_RNSKILL_NEED_LEVEL, sNeed.dwLevel );
            vecString.push_back( strTemp );
        }
        if( sNeed.bPrecedentSkill == TRUE )
        {    
            strTemp.Format( "%s", ID2GAMEWORD_RNSKILL_NEED_SKILL );
            vecString.push_back( strTemp );
        }
        if( sNeed.bUseScrool )
        {
            strTemp.Format( "%s", ID2GAMEWORD("RNSKILL_SLOT_NEED",0) );
            vecString.push_back( strTemp );
        }
        if( 0 < sNeed.dwSkillPoint )
        {
            strTemp.Format( "%s %d", ID2GAMEWORD_RNSKILL_NEED_POINT, sNeed.dwSkillPoint );
            vecString.push_back( strTemp );
        }
        if( 0 < sNeed.dwPow )
        {
            strTemp.Format( "%s %d", ID2GAMEWORD("SKILL_ADVANCED_INFO_STATS",0), sNeed.dwPow );
            vecString.push_back( strTemp );
        }
        if( 0 < sNeed.dwDex )
        {
            strTemp.Format( "%s %d", ID2GAMEWORD("SKILL_ADVANCED_INFO_STATS",3), sNeed.dwDex );
            vecString.push_back( strTemp );
        }
        if( 0 < sNeed.dwSpi )
        {
            strTemp.Format( "%s %d", ID2GAMEWORD("SKILL_ADVANCED_INFO_STATS",2), sNeed.dwSpi );
            vecString.push_back( strTemp );
        }
        if( 0 < sNeed.dwStr )
        {
            strTemp.Format( "%s %d", ID2GAMEWORD("SKILL_ADVANCED_INFO_STATS",1), sNeed.dwStr );
            vecString.push_back( strTemp );
        }
        if( 0 < sNeed.dwSta )
        {
            strTemp.Format( "%s %d", ID2GAMEWORD("SKILL_ADVANCED_INFO_STATS",5), sNeed.dwSta );
            vecString.push_back( strTemp );
        }

//         if( 0 < sNeed.dwGold )
//         {
//             strTemp.Format( "%s", ID2GAMEWORD_RNSKILL_NEED_GOLD , sNeed.dwGold );
//             vecString.push_back( strTemp );
//         }

        if( 0 < m_pSkillInfo->dwTempSkillLevel )
        {
            if( 1 == vecString.size() )
                strLine1.Format( "%s (%d)", vecString[0], m_pSkillInfo->dwUsingSkillPoint );
            else if( 2 <= vecString.size() )
                strLine1.Format( "%s / %s (%d)", vecString[0], vecString[1], m_pSkillInfo->dwUsingSkillPoint );
            else
                strLine1.Format( " " );
        }
        else
        {
            // 마우스가 올라갈경우 출력이 다르다.
            if( bRollover == TRUE )
            {
                if( 1 == vecString.size() )
                    strLine1.Format( "%s (%d)", vecString[0], m_pSkillInfo->dwUsingSkillPoint );
                else if( 2 <= vecString.size() )
                    strLine1.Format( "%s / %s (%d)", vecString[0], vecString[1], m_pSkillInfo->dwUsingSkillPoint );
            }
            else
            {
                if( 1 == vecString.size() )
                    strLine1.Format( "%s", vecString[0]);
                else if( 2 <= vecString.size() )
                    strLine1.Format( "%s / %s", vecString[0], vecString[1]);
            }


        }


        dwColorLine0 = D3DCOLOR_ARGB(0x80,0xFF,0xFF,0xFF);
        dwColorLine1 = D3DCOLOR_ARGB(0x80,0xFF,0x00,0x00);
        break;

    case STATE_DEACTIVATE:
        strLine1.Format("%d%s Lv. %d"
            , m_pSkillInfo->pSkill->m_sBASIC.dwGRADE
            , ID2GAMEWORD( "SKILL_SLOT", 0 )
            , m_pSkillInfo->dwLevel );
        dwColorLine0 = D3DCOLOR_ARGB(0x80,0xFF,0xFF,0xFF);
        dwColorLine1 = D3DCOLOR_ARGB(0x80,0xFF,0xFF,0xFF);
        if( bRollover == TRUE )
            dwColorLine0 = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
        break;

    case STATE_ACTIVATE:
        dwColorLine0 = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
        dwColorLine1 = D3DCOLOR_ARGB(0x80,0xFF,0xFF,0xFF);
        m_pShadow->SetDiffuseAlpha( 80 );   
        strLine1.Format("%d%s Lv. %d"
            , m_pSkillInfo->pSkill->m_sBASIC.dwGRADE
            , ID2GAMEWORD( "SKILL_SLOT", 0 )
            , m_pSkillInfo->dwLevel
            , m_pSkillInfo->dwTempSkillLevel );


        if( bRollover == TRUE )
        {
            dwColorLine1 = D3DCOLOR_ARGB(0xFF,0x65,0xB3,0xEE);
            m_pShadow->SetDiffuseAlpha( 255 );   

            if( m_pSkillInfo->bMaster == TRUE )
                strLine1.Format("Lv. %d (M)", m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel );   
            else
            {
                unsigned nNextLevel = m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel;
                strLine1.Format("Lv. %d - Lv. %d (%d)"
                    , m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel
                    , m_pSkillInfo->dwLevel + m_pSkillInfo->dwTempSkillLevel + 1
                    , m_pSkillInfo->dwUsingSkillPoint );   
            }
        }
        break;

    }
    strLine0.Format("%s",  m_pSkillInfo->pSkill->GetName() );
    m_pTextBoxLine0->ClearText();
    m_pTextBoxLine0->AddTextNoSplit( strLine0.GetString(), dwColorLine0 );

    m_pTextBoxLine1->ClearText();
    m_pTextBoxLine1->AddTextNoSplit( strLine1.GetString(), dwColorLine1 );

}

