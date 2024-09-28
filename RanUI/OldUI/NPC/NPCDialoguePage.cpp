#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../../RanLogic/NpcTalk/NpcTalkCondition.h"
#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/Quest/GLQuest.h"

#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Crow/GLCrowClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"


#include <map>

#include "../../InnerInterface.h"
#include "./NPCDialoguePage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CNPCDialoguePage::nSTARTINDEX = 1;
const int CNPCDialoguePage::nSTARTLINE = 0;
const float CNPCDialoguePage::fANSWER_PART_INTERVAL = 2.0f;

CNPCDialoguePage::CNPCDialoguePage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
    , m_pRandomTimeButton(NULL)
    , m_pRandomTimeText(NULL)
    , m_pRandomTimeBack(NULL)
    , m_pTalkBack(NULL)
    , m_pTalkText(NULL)
    , m_pTalkScrollBar(NULL)
    , m_bButtonRandom(false)
    , m_nTalkButtonTotalLine(0)
    , m_nTalkButtonStartIndex(0)
{
    for ( int i = 0; i < NUM_TALK_BUTTON; i++ )
    {
        m_pTalkButton[ i ] = NULL;
        m_pTalkButtonText[ i ] = NULL;
    }

    m_spDialogueSet = std::tr1::shared_ptr<CNpcDialogueSet> (new CNpcDialogueSet);
}

CNPCDialoguePage::~CNPCDialoguePage()
{
}

void CNPCDialoguePage::CreateSubControl()
{
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_EX_FLAG );
    CD3DFontPar* pFont11Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 11, _DEFAULT_FONT_SHADOW_EX_FLAG );

    // RandomTime
    {
        const int nBUTTONSIZE = CBasicTextButton::SIZE16;
        m_pRandomTimeButton = new CBasicTextButton(m_pEngineDevice);
        m_pRandomTimeButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, RANDOM_TIME_BUTTON );
        m_pRandomTimeButton->CreateBaseButton( "RANDOM_TIME_BUTTON", nBUTTONSIZE, CBasicButton::RADIO_FLIP, ID2GAMEWORD( "RANDOM_TIME_BUTTON",0));
        m_pRandomTimeButton->SetFlip( FALSE );
        RegisterControl( m_pRandomTimeButton );

        m_pRandomTimeBack = new CBasicLineBox(m_pEngineDevice);
        m_pRandomTimeBack->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pRandomTimeBack->CreateBaseBoxEditBox( "RANDOM_TIME_BACK" );
        RegisterControl( m_pRandomTimeBack );    


        m_pRandomTimeText = new CBasicTextBox(m_pEngineDevice);
        m_pRandomTimeText->CreateSub( this, "RANDOM_TIME_TEXT", UI_FLAG_DEFAULT, RANDOM_TIME_TEXT );
        m_pRandomTimeText->SetFont( pFont9 );
        m_pRandomTimeText->SetTextAlign( TEXT_ALIGN_CENTER_X );	
        RegisterControl ( m_pRandomTimeText );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "DIALOGUE_WINDOW_TALK_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "DIALOGUE_WINDOW_TALK_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pTalkBack = pLinBoxSmart;

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "DIALOGUE_WINDOW_TALK_TEXT", UI_FLAG_DEFAULT, DIALOGUE_TALK_TEXT );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pTalkText = pTextBox;

        int nTotalLine = m_pTalkText->GetVisibleLine();

        //	스크롤바
        m_pTalkScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        m_pTalkScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, DIALOGUE_TALK_SCROLL );
        m_pTalkScrollBar->CreateBaseScrollBar( "DIALOGUE_WINDOW_TALK_TEXT_SCROLL_BAR" );
        m_pTalkScrollBar->GetThumbFrame()->SetState( 1, nTotalLine );
        RegisterControl( m_pTalkScrollBar );

    }

    {
        char* strUIButton[ NUM_TALK_BUTTON ] =
        {
            "DIALOGUE_WINDOW_TALK_BUTTON_0"
            , "DIALOGUE_WINDOW_TALK_BUTTON_1"
            , "DIALOGUE_WINDOW_TALK_BUTTON_2"
            , "DIALOGUE_WINDOW_TALK_BUTTON_3"
            , "DIALOGUE_WINDOW_TALK_BUTTON_4"
            , "DIALOGUE_WINDOW_TALK_BUTTON_5"
        };

        char* strUIButtonText[ NUM_TALK_BUTTON ] =
        {
            "DIALOGUE_WINDOW_TALK_BUTTON_0_TEXT"
            , "DIALOGUE_WINDOW_TALK_BUTTON_1_TEXT"
            , "DIALOGUE_WINDOW_TALK_BUTTON_2_TEXT"
            , "DIALOGUE_WINDOW_TALK_BUTTON_3_TEXT"
            , "DIALOGUE_WINDOW_TALK_BUTTON_4_TEXT"
            , "DIALOGUE_WINDOW_TALK_BUTTON_5_TEXT"
        };

        for ( int i = 0; i < NUM_TALK_BUTTON; i++ )
        {
            CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
            pLinBoxSmart->CreateSub( this, strUIButton[ i ], UI_FLAG_XSIZE | UI_FLAG_YSIZE, ( DIUALOGUE_TALK_BUTTON_0 + i ) );
            pLinBoxSmart->CreateSubControl( "DIALOGUE_WINDOW_TALK_BUTTON_0_LINE" );
            RegisterControl( pLinBoxSmart );
            m_pTalkButton[ i ] = pLinBoxSmart;

            CBasicTextBoxEx* pBaiscTextBoxEx = new CBasicTextBoxEx(m_pEngineDevice);
            pBaiscTextBoxEx->CreateSub( this, strUIButtonText[ i ], UI_FLAG_DEFAULT, ( DIUALOGUE_TALK_BUTTON_TEXT_0 + i ) );
            pBaiscTextBoxEx->SetFont( pFont9Shadow );
            pBaiscTextBoxEx->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
            pBaiscTextBoxEx->SetPartInterval( fANSWER_PART_INTERVAL );
            pBaiscTextBoxEx->SetSensitive( true );
            RegisterControl( pBaiscTextBoxEx );
            m_pTalkButtonText[ i ] = pBaiscTextBoxEx;
        }

        //	스크롤바
        m_pTalkButtonScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        m_pTalkButtonScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, DIUALOGUE_TALK_BUTTON_SCROLL_BAR );
        m_pTalkButtonScrollBar->CreateBaseScrollBar( "DIALOGUE_WINDOW_TALK_BUTTON_SCROLL_BAR" );
        m_pTalkButtonScrollBar->GetThumbFrame()->SetState( 1, NUM_TALK_BUTTON );
        RegisterControl( m_pTalkButtonScrollBar );
    }
}

void CNPCDialoguePage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !CUIControlNpcActionable::IsUseable( m_pGaeaClient ) )
    {
        m_pInterface->SetDialogueWindowClose();
        return;
    }

    for ( int i = 0; i < NUM_TALK_BUTTON; i++ )
        m_pTalkButtonText[ i ]->SetUseOverColor( FALSE );

    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( bFirstControl )
    {
        {
            CBasicScrollThumbFrame* const pThumbFrame = m_pTalkScrollBar->GetThumbFrame();		
            m_pTalkText->SetCurLine( 0 );
            const int nTotalLine = m_pTalkText->GetTotalLine();
            const int nLinePerOneView = m_pTalkText->GetVisibleLine();
            if ( nLinePerOneView < nTotalLine )
            {
                const int nMovableLine = nTotalLine - nLinePerOneView;
                float fPercent = pThumbFrame->GetPercent();
                int nPos = static_cast<int>( floor(fPercent * nMovableLine) );
                if ( nPos < nSTARTLINE )
                    nPos = nSTARTLINE;
                m_pTalkText->SetCurLine( nPos );			
            }
        }

        {
            CBasicScrollThumbFrame* const pThumbFrame = m_pTalkButtonScrollBar->GetThumbFrame();		
            pThumbFrame->SetState( m_nTalkButtonTotalLine, NUM_TALK_BUTTON );
            if ( NUM_TALK_BUTTON < m_nTalkButtonTotalLine )
            {
                const int nMovableLine = m_nTalkButtonTotalLine - NUM_TALK_BUTTON;
                float fPercent = pThumbFrame->GetPercent();
                int nPos = (int)floor(fPercent * nMovableLine);
                if ( nPos != m_nTalkButtonStartIndex )
                {
//					DWORD dwPageIndex(0);
//					if ( m_spDialogue )
//						dwPageIndex = m_spDialogue->FindConditionSatisfied(m_pCharLogic, m_pGaeaClient->GetMyPartyClient().GetNMember() );

                    m_nTalkButtonStartIndex = nPos;
                    LoadButtonTalk( m_bButtonRandom, m_nCaseIdx );
                }
                else
                    m_nTalkButtonStartIndex = nPos;
            }
        }
    }
}

void CNPCDialoguePage::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case DIUALOGUE_TALK_BUTTON_TEXT_0:
    case DIUALOGUE_TALK_BUTTON_TEXT_1:
    case DIUALOGUE_TALK_BUTTON_TEXT_2:
    case DIUALOGUE_TALK_BUTTON_TEXT_3:
    case DIUALOGUE_TALK_BUTTON_TEXT_4:
    case DIUALOGUE_TALK_BUTTON_TEXT_5:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                int nButtonIndex = ControlID - DIUALOGUE_TALK_BUTTON_TEXT_0;
                if ( nButtonIndex < 0 || nButtonIndex >= NUM_TALK_BUTTON )
                    return;

                int nIndex = m_pTalkButtonText[ nButtonIndex ]->GetSelectPos();
                if ( nIndex < 0 )
                    return;

                m_pTalkButtonText[ nButtonIndex ]->SetUseOverColor( TRUE );		
                m_pTalkButtonText[ nButtonIndex ]->SetOverColor( nIndex, NS_UITEXTCOLOR::BLACK );

                if ( dwMsg & UIMSG_LB_UP )
                {
                    if ( m_nTalkButtonStartIndex + nButtonIndex + 1 == m_nTalkButtonTotalLine )
                    {
                        // 대화 종료 클릭.
                        m_pInterface->UiHideGroup( DIALOGUE_WINDOW_RENEWAL );
                        m_pInterface->ItemRepairWindowOpen( false );
                        return;
                    }
                }

                const DWORD dwTalkID = m_pTalkButtonText[ nButtonIndex ]->GetTextData( nIndex );
                if (!m_spDialogueCase)
                    return;

                std::tr1::shared_ptr<CNpcTalkControl> spNpcTalkControl = m_spDialogueCase->GetTalkControl();
                if (!spNpcTalkControl)
                    return;


                std::tr1::shared_ptr<SNpcTalk> spNpcTalk = spNpcTalkControl->GetTalk( dwTalkID );
                if (!spNpcTalk)
                    return;

                if ( dwMsg & UIMSG_LB_UP )
                {
                    TranslateTalkMessage( spNpcTalk );
                }
                else
                {
                    //	퀘스트 보상치 표현
                    if ( spNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_START )
                    {						
                        DWORD dwQuestID = spNpcTalk->m_dwQuestStartID[0];
                        GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
                        if ( !pQuest )
                        {
                            sc::writeLogError(
                                sc::string::format(
                                "quest id = %1% 의 정보를 가져오지 못하였습니다.", dwQuestID));
                            return;
                        }

                        //	보상치가 없으면 아무것도 표시하지 않음
                        if ( !(pQuest->m_lnGiftEXP || pQuest->m_dwGiftMONEY ||
                            pQuest->m_dwGiftELEMENT || pQuest->m_dwGiftLIFEPOINT ||
                            pQuest->m_dwGiftSKILLPOINT || pQuest->m_dwGiftSTATSPOINT ||
                            pQuest->m_vecGiftITEM.size () || pQuest->m_vecGiftSKILL.size ()) )
                        {
                            return ;
                        }

                        if ( m_pInterface->BEGIN_COMMON_LINEINFO_MULTI() )
                        {
                            m_pInterface->ADD_COMMON_LINEINFO_MULTI( ID2GAMEWORD("QUEST_GIFT",9), NS_UITEXTCOLOR::GREENYELLOW );

                            std::string strCombine;

                            if ( pQuest->m_lnGiftEXP )
                            {
                                strCombine = sc::string::format( "%s : %d", ID2GAMEWORD("QUEST_GIFT",0), pQuest->m_lnGiftEXP );
                                m_pInterface->ADD_COMMON_LINEINFO_MULTI( strCombine.c_str(), NS_UITEXTCOLOR::WHITE );
                            }
                            if ( pQuest->m_dwGiftMONEY )
                            {
                                strCombine = sc::string::format( "%s : %d", ID2GAMEWORD("QUEST_GIFT",1), pQuest->m_dwGiftMONEY );
                                m_pInterface->ADD_COMMON_LINEINFO_MULTI( strCombine.c_str(), NS_UITEXTCOLOR::WHITE );
                            }
                            if ( pQuest->m_dwGiftELEMENT )
                            {
                                strCombine = sc::string::format( "%s : %d", ID2GAMEWORD("QUEST_GIFT",2), pQuest->m_dwGiftELEMENT );
                                m_pInterface->ADD_COMMON_LINEINFO_MULTI( strCombine.c_str(), NS_UITEXTCOLOR::WHITE );
                            }
                            if ( pQuest->m_dwGiftLIFEPOINT )
                            {
                                strCombine = sc::string::format( "%s : %d", ID2GAMEWORD("QUEST_GIFT",3), pQuest->m_dwGiftLIFEPOINT );
                                m_pInterface->ADD_COMMON_LINEINFO_MULTI( strCombine.c_str(), NS_UITEXTCOLOR::WHITE );
                            }
                            if ( pQuest->m_dwGiftSKILLPOINT )
                            {
                                strCombine = sc::string::format( "%s : %d", ID2GAMEWORD("QUEST_GIFT",5), pQuest->m_dwGiftSKILLPOINT );
                                m_pInterface->ADD_COMMON_LINEINFO_MULTI( strCombine.c_str(), NS_UITEXTCOLOR::WHITE );
                            }
                            if ( pQuest->m_dwGiftSTATSPOINT )
                            {
                                strCombine = sc::string::format( "%s : %d", ID2GAMEWORD("QUEST_GIFT",6), pQuest->m_dwGiftSTATSPOINT );
                                m_pInterface->ADD_COMMON_LINEINFO_MULTI( strCombine.c_str(), NS_UITEXTCOLOR::WHITE );
                            }

                            if ( pQuest->m_vecGiftITEM.size() )
                            {
                                std::string strItem = ID2GAMEWORD( "QUEST_GIFT",7 );
                                strItem += " : ";
                                INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin();
                                INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end();
                                for ( ; iter != iter_end; )
                                {
                                    SITEMCUSTOM& sItemCustom = (*iter);

                                    ++iter;

                                    const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
                                    if ( !pItem )
                                        continue;

                                    strItem += sItemCustom.GETNAME();
                                    if ( iter == iter_end )
                                        break;

                                    strItem += ", ";
                                }
                                m_pInterface->ADD_COMMON_LINEINFO_MULTI( strItem.c_str(), NS_UITEXTCOLOR::WHITE );
                            }

                            if ( pQuest->m_vecGiftSKILL.size() )
                            {
                                std::string strSkill = ID2GAMEWORD("QUEST_GIFT",8);
                                strSkill += " : ";
                                DWQARRAY_ITER iter = pQuest->m_vecGiftSKILL.begin();
                                DWQARRAY_ITER iter_end = pQuest->m_vecGiftSKILL.end();
                                for ( ; iter != iter_end; )
                                {
                                    DWORD dwID = (*iter);									
                                    PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData( SNATIVEID(dwID) );
                                    if ( !pSKILL ) continue;

                                    strSkill += pSKILL->GetName();

                                    ++iter;

                                    if ( iter == iter_end )
                                        break;

                                    strSkill += ", ";
                                }
                                m_pInterface->ADD_COMMON_LINEINFO_MULTI( strSkill.c_str(), NS_UITEXTCOLOR::WHITE );
                            }

                            m_pInterface->END_COMMON_LINEINFO_MULTI();
                        }
                    }
                }
            }
        }
        break;
    };
}
void CNPCDialoguePage::TranslateTalkMessage( std::tr1::shared_ptr<SNpcTalk> spNpcTalk )
{
    m_nTalkButtonStartIndex = 0;

    if ( !spNpcTalk )
        return;

    switch ( spNpcTalk->m_nACTION )
    {
    case SNpcTalk::EM_DO_NOTHING:
        {
        }
        break;
    case SNpcTalk::EM_PAGE_MOVE:
        {
            LoadNode( spNpcTalk->m_dwACTION_NO );
        }
        break;
    case SNpcTalk::EM_BASIC:
        {
            TranslateBasicMessage( spNpcTalk );
        }
        break;
    case SNpcTalk::EM_QUEST_START:
        {
            LoadNode( spNpcTalk->m_dwACTION_NO );

            TranslateQuestStartMessage( spNpcTalk );
        }
        break;
    case SNpcTalk::EM_QUEST_STEP:
        {
            LoadNode( spNpcTalk->m_dwACTION_NO );

            TranslateQuestStepMessage( spNpcTalk );
        }
        break;
    };
}

void CNPCDialoguePage::TranslateBasicMessage( std::tr1::shared_ptr<SNpcTalk> spNpcTalk )
{
    if ( !spNpcTalk )
        return;

    // Need Add New Item Con.
    DWORD dwMsg = spNpcTalk->m_dwACTION_NO;
    DWORD dwTalkID = spNpcTalk->m_dwGLOB_ID;

    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    if ( !pLand )
        return;
    std::tr1::shared_ptr<GLCrowClient> pCrow = pLand->GetCrow( CUIControlNpcActionable::GetNPCGlobalID() );
    if ( !pCrow )
        return;

    // ReqNpcTalkBasic() 대화문 유효성 체크 ( 서버 ) 
    switch ( dwMsg )
    {
    case SNpcTalk::EM_STORAGE:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;
    case SNpcTalk::EM_MARKET:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;
    case SNpcTalk::EM_CURE:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;
    case SNpcTalk::EM_STARTPOINT:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;

    case SNpcTalk::EM_CHAR_RESET:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;

        // Need Add New Item Con.
    case SNpcTalk::EM_ITEM_TRADE:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqItemTrade( CUIControlNpcActionable::GetNPCGlobalID(), dwTalkID, spNpcTalk );
        }
        break;

    case SNpcTalk::EM_BUSSTATION:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pInterface->SetBusWindowOpen(  CUIControlNpcActionable::GetNPCID(), CUIControlNpcActionable::GetNPCGlobalID(), spNpcTalk );
        }
        break;

    case SNpcTalk::EM_CLUB_NEW:
        {
            m_pInterface->UiHideGroup( GetWndID() );

            //	NOTE : 클럽 만들기 신청
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;

    case SNpcTalk::EM_CLUB_UP:
        {
            m_pInterface->UiHideGroup( GetWndID() );

            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;

    case SNpcTalk::EM_CD_CERTIFY:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;

    case SNpcTalk::EM_COMMISSION:
        {
            m_pInterface->UiHideGroup( GetWndID() );

            //	NOTE : 수수료 지정
            //
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;

    case SNpcTalk::EM_CLUB_STORAGE:
        {
            GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
            GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
            bool bVALID_CLUB = pMyClub->IsValid();
            bool bMASTER = pMyClub->MasterCharDbNum() == pCharacter->m_CharDbNum;

            if ( bVALID_CLUB && bMASTER )
            {
                m_pInterface->UiHideGroup( GetWndID() );
                m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
            }
            else
            {
                m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "ONLY_CLUB_MASTER_USE_CLUBSTORAGE" ) );
            }
        }
        break;

    case SNpcTalk::EM_ITEM_REBUILD:	// ITEMREBUILD_MARK
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;

    case SNpcTalk::EM_ODDEVEN:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;
    case SNpcTalk::EM_RECOVERY_EXP:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;
    case SNpcTalk::EM_RANDOM_PAGE:
        {
            int nCnt = MAX_RANDOM_PAGE;

            for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
            {
                if ( spNpcTalk->m_dwRandomPageID[i] == UINT_MAX )
                {
                    nCnt = i;
                    break;
                }
            }

            if ( nCnt <= 0 ) 
            {
                sc::writeLogError(
                    sc::string::format(
                    "Random Page Not Found [%1%]",
                    m_spDialogueSet->GetDlgSetName() ) );
                break;
            }

            int nChoice = int ( rand() % nCnt );

            LoadNode( spNpcTalk->m_dwRandomPageID[ nChoice ], true, spNpcTalk->m_dwRandomTime );
        }
        break;
    case SNpcTalk::EM_ITEMSEARCH_PAGE:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;

    case SNpcTalk::EM_ATTENDANCE_BOOK:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;
    case SNpcTalk::EM_ITEM_MIX:
        {
            m_pInterface->UiHideGroup( GetWndID() );
            m_pGaeaClient->GetCharacter()->ReqNpcTalkBasic( CUIControlNpcActionable::GetNPCGlobalID(), dwMsg, dwTalkID );
        }
        break;
	case SNpcTalk::EM_HIRE_SUMMON:
		{
			m_pInterface->UiHideGroup( GetWndID() );
			m_pGaeaClient->GetCharacter()->ReqHireSummon ( CUIControlNpcActionable::GetNPCGlobalID(), dwTalkID, spNpcTalk );
		}
		break;
    };
}

void CNPCDialoguePage::TranslateQuestStartMessage( std::tr1::shared_ptr<SNpcTalk> spNpcTalk )
{
    GASSERT( spNpcTalk && "CDialogueWindow::TranslateQuestStartMessage" );

    if ( !spNpcTalk )
        return;

    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    if ( !pLand )
        return;
    std::tr1::shared_ptr<GLCrowClient> pCrow = pLand->GetCrow( CUIControlNpcActionable::GetNPCGlobalID() );
    if ( !pCrow )
        return;

	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
    for ( int i = 0; i < MAX_QUEST_START; ++i )
    {
        if ( spNpcTalk->m_dwQuestStartID[i] != UINT_MAX && spNpcTalk->DoTEST( m_pCharLogic, i, cCurSvrTime.GetHour() ) )
        {
            m_pGaeaClient->GetCharacter()->ReqQuestStart(
                CUIControlNpcActionable::GetNPCGlobalID(), spNpcTalk->m_dwGLOB_ID, spNpcTalk->m_dwQuestStartID[i] );
        }
    }
}

void CNPCDialoguePage::TranslateQuestStepMessage( std::tr1::shared_ptr<SNpcTalk> spNpcTalk )
{
    GASSERT( spNpcTalk&&"CDialogueWindow::TranslateQuestStepMessage" );

    if ( !spNpcTalk )
        return;

    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    if ( !pLand )
        return;

    std::tr1::shared_ptr<GLCrowClient> pCrow = pLand->GetCrow( CUIControlNpcActionable::GetNPCGlobalID() );
    if ( !pCrow )
        return;

    m_pGaeaClient->GetCharacter()->ReqQuestStepNpcTalk(
        CUIControlNpcActionable::GetNPCGlobalID(),
        spNpcTalk->m_dwGLOB_ID,
        spNpcTalk->m_dwACTION_PARAM1,
        spNpcTalk->m_dwACTION_PARAM2 );
}


bool CNPCDialoguePage::SetDialogueData( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, GLCHARLOGIC* pCharLogic )
{
    GASSERT ( pCharLogic );

    m_pCharLogic = pCharLogic;

    std::tr1::shared_ptr<GLCrowClient> pCrow = m_pGaeaClient->GetActiveMap()->GetCrow( dwNPCGlobalID );
    if ( !pCrow )						
        return false;

    if ( pCrow->GETCROW() != CROW_NPC )
        return false;

    CUIControlNpcActionable::SetNpcActionableNPCID( sNPCID, dwNPCGlobalID );

    const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pCrow->m_pCrowData->sNativeID );
    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    if (spTalkMan)
    {
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = spTalkMan->Find( pCrowData->GetTalkFile() );
        if ( spDlgSet )
        {
            m_spDialogueSet = spDlgSet;
            m_nTalkButtonStartIndex = 0;
            LoadNode( nSTARTINDEX );
            return true;
        }
    }

    return false;
}

void CNPCDialoguePage::LoadNode( DWORD dwID, bool bRandom /*= false*/, DWORD dwRandomTime /*= 0 */ )
{
    m_spDialogue = m_spDialogueSet->GetDialogue( dwID );
    //GASSERT( m_spDialogue && "m_DialogueSet.GetDialogue()" );
    if ( !m_spDialogue )
    {
        std::string ErrorMsg = stringformat( "void CNPCDialoguePage::LoadNode - GetDialogue failed FileName : %1%, Index : %2%",
            m_spDialogueSet->m_TalkName, dwID );
        sc::writeLogError( ErrorMsg );

        return;
    }

	if ( !m_pGaeaClient )
		return;

	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
    DWORD dwIndex = m_spDialogue->FindConditionSatisfied( m_pCharLogic, m_pGaeaClient->GetMyPartyClient()->GetNMember(), cCurSvrTime.GetHour() );
    //GASSERT( dwIndex!=UINT_MAX&&"m_pDialogue->FindConditionSatisfied()" );
    if (dwIndex==UINT_MAX)
    {
        std::string ErrorMsg = stringformat( "void CNPCDialoguePage::LoadNode - Satisfied failed FileName : %1%, Index : %2%",
            m_spDialogueSet->m_TalkName, dwID );
        sc::writeLogError( ErrorMsg );

        return;
    }

    std::tr1::shared_ptr<CNpcDialogueCase> spCase = m_spDialogue->GetDlgCase( dwIndex );
    GASSERT( spCase && "m_pDialogueCase = NULL" );
    m_spDialogueCase = spCase;
	m_nCaseIdx = dwIndex;
    m_dwRandomTime = dwRandomTime;
    m_fRandomTime = 0.0f;

    //
    if ( m_dwRandomTime > 0 )
    {
        m_pRandomTimeButton->SetVisibleSingle( TRUE );
        m_pRandomTimeBack->SetVisibleSingle( TRUE );
        m_pRandomTimeText->SetVisibleSingle( TRUE );

        std::string strTime;
        strTime = sc::string::format( "%d", m_dwRandomTime );

        m_pRandomTimeText->SetText( strTime.c_str(), NS_UITEXTUTIL::WHITE );

    }
    else
    {
        m_pRandomTimeButton->SetVisibleSingle( FALSE );
        m_pRandomTimeBack->SetVisibleSingle( FALSE );
        m_pRandomTimeText->SetVisibleSingle( FALSE );
    }

    LoadBasicTalk(dwIndex);
    LoadButtonTalk( bRandom, m_nCaseIdx );

    // init talk button scroll bar.
    CBasicScrollThumbFrame* const pThumbFrame = m_pTalkButtonScrollBar->GetThumbFrame();
    pThumbFrame->SetState( m_nTalkButtonTotalLine, NUM_TALK_BUTTON );
    pThumbFrame->SetPercent( 0.0f );
}

void CNPCDialoguePage::LoadBasicTalk( const DWORD dwIndex )
{
    if ( !m_spDialogueCase )
        return;

	std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
	std::string strBasicTalk;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strTalkKey;
		const std::string strDlgSetName(m_spDialogueSet->GetDlgSetName());
		const DWORD dwDlgNID(m_spDialogue->GetDlgNID());
		strTalkKey.Format("%s_%d_%d", strDlgSetName.c_str(), dwDlgNID, dwIndex);
		strBasicTalk = CGameTextMan::GetInstance().GetText(strTalkKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();
	}
	else
		strBasicTalk = m_spDialogueCase->GetBasicTalk();

    m_pTalkText->SetText( strBasicTalk.c_str(), NS_UITEXTCOLOR::WHITE );

    int nTotal = m_pTalkText->GetTotalLine();
    int nViewPerPage = m_pTalkText->GetVisibleLine();
    CBasicScrollThumbFrame* const pThumbFrame = m_pTalkScrollBar->GetThumbFrame();
    pThumbFrame->SetState( nTotal, nViewPerPage );
    pThumbFrame->SetPercent( 0.0f );
}

void CNPCDialoguePage::LoadButtonTalk( bool bRandom /*= false */, const DWORD dwIndex/* = 0*/ )
{
    m_bButtonRandom = bRandom;

    for ( int i = 0; i < NUM_TALK_BUTTON; i++ )
        m_pTalkButtonText[ i ]->ClearText();

    static const int nKEYWORDSIZE = 10;

    static const std::string strKeyword[nKEYWORDSIZE] = 
    {
        "DIALOGUE_ICON_HEAL",
        "DIALOGUE_ICON_TRADE",
        "DIALOGUE_ICON_STARTPOINT",
        "DIALOGUE_ICON_STORAGE",
        "DIALOGUE_ICON_MARKET",
        "DIALOGUE_ICON_RESETPOINT",
        "DIALOGUE_ICON_BUS",
        "DIALOGUE_ICON_QUEST_START",
        "DIALOGUE_ICON_QUEST_ING",
        "DIALOGUE_ICON_TALK",
    };

    enum
    {
        ICON_HEAL,
        ICON_TRADE,
        ICON_STARTPOINT,
        ICON_STORAGE,
        ICON_MARKET,
        ICON_RESETPOINT,
        ICON_BUS,
        ICON_QUEST_START,
        ICON_QUEST_ING,
        ICON_QUEST_TALK,
    };

    //	버튼 텍스트 설정
    if ( !m_spDialogueCase )
        return;
    std::tr1::shared_ptr<CNpcTalkControl> spNpcTalkControl = m_spDialogueCase->GetTalkControl();
    if ( spNpcTalkControl )
    {
        CNpcTalkControl::NPCMAP* pMap = spNpcTalkControl->GetTalkMap();
        CNpcTalkControl::NPCMAP_IT iter = pMap->begin();
        CNpcTalkControl::NPCMAP_IT iter_end = pMap->end();

        std::vector<std::tr1::shared_ptr<SNpcTalk> > vecTalk;
        vecTalk.reserve( pMap->size() );

        for ( ; iter != iter_end; ++iter )
        {
            std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
            if ( spNpcTalk )
                vecTalk.push_back( spNpcTalk );
        }

        if ( bRandom )
        {
            random_shuffle( vecTalk.begin(), vecTalk.end() );			
        }

        std::vector<std::tr1::shared_ptr<SNpcTalk> >::iterator pos = vecTalk.begin();
        std::vector<std::tr1::shared_ptr<SNpcTalk> >::iterator pos_end = vecTalk.end();

        std::multimap<int, int> mapTalkIndex;

        int nTalkIndex = 0;

        // Note : 대화파일에 상점 조건에 대한 정보도 있기 때문에 여기서 설정해준다.
        m_pInterface->SetEnableDialogueWindowMarketPage(false);

		CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
        for ( ; pos != pos_end; ++pos, ++nTalkIndex )
        {
            std::tr1::shared_ptr<SNpcTalk> spNpcTalk = (*pos);

            //	Note : 조건에 부합하지 못할 경우 추가하지 않음.
            DWORD dwPartyMemberNum = m_pGaeaClient->GetMyPartyClient()->GetNMember();
            if ( !spNpcTalk->DoTEST( m_pCharLogic, dwPartyMemberNum, cCurSvrTime.GetHour() ) )
            {
                if ( CheckQuestProc( spNpcTalk, dwPartyMemberNum ) )
                    mapTalkIndex.insert( std::make_pair( TALK_QUEST_ING, nTalkIndex ) );

                continue;
            }

            if (spNpcTalk->m_nACTION == SNpcTalk::EM_BASIC &&
                spNpcTalk->m_dwACTION_NO == SNpcTalk::EM_MARKET )
                m_pInterface->SetEnableDialogueWindowMarketPage(true);

            switch ( spNpcTalk->m_nACTION )
            {
            case SNpcTalk::EM_QUEST_START:
                {
                    mapTalkIndex.insert( std::make_pair( TALK_QUEST_START, nTalkIndex ) );
                }
                break;

            case SNpcTalk::EM_QUEST_STEP:
                {
                    mapTalkIndex.insert( std::make_pair( TALK_QUEST_STEP, nTalkIndex ) );
                }
                break;

            case SNpcTalk::EM_BASIC:
                {
                    if ( spNpcTalk->m_dwACTION_NO != SNpcTalk::EM_MARKET )
                        mapTalkIndex.insert( std::make_pair( TALK_BASIC_TALK, nTalkIndex ) );
                }
                break;

            case SNpcTalk::EM_PAGE_MOVE:
                {
                    if ( spNpcTalk->m_dwACTION_NO == 1 )
                        mapTalkIndex.insert( std::make_pair( TALK_FIRST_TIME, nTalkIndex ) );
                    else
                        mapTalkIndex.insert( std::make_pair( TALK_BASIC_TALK, nTalkIndex ) );
                }
                break;

            default:
                {
                    mapTalkIndex.insert( std::make_pair( TALK_BASIC_TALK, nTalkIndex ) );
                }
                break;
            };
        }

        int talkIndexSize = static_cast<int>( mapTalkIndex.size() );
        m_nTalkButtonTotalLine = talkIndexSize + 1;

        int nButtonIndex = 0;
        int nCurIndex = 0;

        std::multimap<int, int>::iterator map_pos = mapTalkIndex.begin();
        std::multimap<int, int>::iterator map_pos_end = mapTalkIndex.end();

        for ( ; map_pos != map_pos_end; map_pos++ )
        {
            if ( nCurIndex < m_nTalkButtonStartIndex )
            {
                nCurIndex++;
                continue;
            }

            std::tr1::shared_ptr<SNpcTalk> spNpcTalk = vecTalk[ map_pos->second ];
            
			std::string strAnswerTalk;		
 			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
 			{
				CString strNpcAnswerKey;
				strNpcAnswerKey.Format("%s_%d_%d_%d"
					, m_spDialogueSet->m_TalkName.c_str()
					, m_spDialogue->GetDlgNID()
					, dwIndex
					, spNpcTalk->m_dwNID );
				strAnswerTalk = CGameTextMan::GetInstance().GetText(strNpcAnswerKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();
			}
			else
				strAnswerTalk = spNpcTalk->GetTalk();

            int nImageType = 0;	
            D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;

            switch ( map_pos->first )
            {
            case TALK_FIRST_TIME:
                {
                    nImageType = ICON_QUEST_TALK;
                }
                break;

            case TALK_QUEST_STEP:
                {
                    dwColor = NS_UITEXTCOLOR::YELLOW;
                    nImageType = ICON_QUEST_START;
                    strAnswerTalk = sc::string::format( "%s %s", ID2GAMEWORD( "DIALOGUE_TALK_BUTTON", 2 ), strAnswerTalk );
                }
                break;

            case TALK_QUEST_START:
                {
                    dwColor = NS_UITEXTCOLOR::FORESTGREEN;
                    nImageType = ICON_QUEST_START;
                    strAnswerTalk = sc::string::format( "%s %s", ID2GAMEWORD( "DIALOGUE_TALK_BUTTON", 3 ), strAnswerTalk );

                    //	NOTE : 비용이 있을 경우 추가
                    DWORD dwQuestID = spNpcTalk->m_dwQuestStartID[0];
                    GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
                    if ( !pQuest )
                    {
                        sc::writeLogError(
                            sc::string::format(
                            "quest id = %1% 의 정보를 가져오지 못하였습니다.", dwQuestID));
                        continue;
                    }

                    const DWORD dwBeginMoney = pQuest->m_dwBeginMoney;
                    if ( dwBeginMoney )
                    {
                        std::string strQuestCost;
                        strQuestCost = sc::string::format( " [%s:%d]", ID2GAMEWORD( "QUEST_BEGIN_MONEY" ), dwBeginMoney );
                        strAnswerTalk += strQuestCost;
                    }
                }
                break;

            case TALK_QUEST_ING:
                {
                    dwColor = NS_UITEXTCOLOR::DARKGRAY;
                    nImageType = ICON_QUEST_ING;
                    strAnswerTalk = sc::string::format( "%s %s", ID2GAMEWORD( "DIALOGUE_TALK_BUTTON", 4 ), strAnswerTalk );
                }
                break;

            case TALK_BASIC_TALK:
                {
                    switch ( spNpcTalk->m_dwACTION_NO )
                    {
                    case SNpcTalk::EM_CURE:
                        nImageType = ICON_HEAL;
                        break;

                    case SNpcTalk::EM_STARTPOINT:
                        nImageType = ICON_STARTPOINT;
                        break;

                    default:
                        nImageType = ICON_QUEST_TALK;
                        break;
                    }
                }
                break;
            };

            if ( nButtonIndex < NUM_TALK_BUTTON )
            {
                int nIndex = m_pTalkButtonText[ nButtonIndex ]->AddText( strAnswerTalk.c_str(), dwColor );
                m_pTalkButtonText[ nButtonIndex ]->SetTextData( nIndex, spNpcTalk->m_dwNID );
                m_pTalkButtonText[ nButtonIndex ]->SetTextImage( nIndex, strKeyword[nImageType].c_str() );
                nButtonIndex++;
            }
        }

        if ( nButtonIndex < NUM_TALK_BUTTON )
        {
            int nIndex = m_pTalkButtonText[ nButtonIndex ]->AddText( ID2GAMEWORD( "DIALOGUE_TALK_BUTTON", 1 ), NS_UITEXTCOLOR::WHITE );
            m_pTalkButtonText[ nButtonIndex ]->SetTextImage( nIndex, "DIALOGUE_ICON_TALK" );
        }
    }
}

bool CNPCDialoguePage::CheckQuestProc( std::tr1::shared_ptr<SNpcTalk> spNpcTalk, DWORD dwPartyMemberNum )
{
    if ( !m_pCharLogic )
        return false;

    if ( spNpcTalk->m_spCondition )
    {
		if ( !m_pGaeaClient )
			return false;

		CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
        BOOL bCONDITION = spNpcTalk->m_spCondition->DoTEST( m_pCharLogic, dwPartyMemberNum, cCurSvrTime.GetHour() );
        if ( !bCONDITION )
            return false;
    }

    // Note : 진행 중인 퀘스트는 보여주도록 한다.
    if ( spNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_START )
    {
        for ( int i = 0; i < MAX_QUEST_START; ++i )
        {
            DWORD dwQuestID = spNpcTalk->m_dwQuestStartID[i];
            if ( dwQuestID == UINT_MAX )
                continue;

            GLQUESTPROG* pQUEST_PROG = m_pCharLogic->m_cQuestPlay.FindProc( dwQuestID );

            if ( pQUEST_PROG )
            {
                GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
                if ( pQuest )
                {
                    if (pQUEST_PROG->m_dwSTEP == pQuest->m_vecProgStep.size() - 1)
                        return false;
                    else
                        return true;
                }
            }
        }
    }
    else if ( spNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_STEP )
    {
        DWORD dwQuestID = spNpcTalk->m_dwACTION_PARAM1;
        GLQUESTPROG* pQUEST_PROG = m_pCharLogic->m_cQuestPlay.FindProc( dwQuestID );
        if ( pQUEST_PROG )
        {
            DWORD dwTalkStep = spNpcTalk->m_dwACTION_PARAM2;
            if ( pQUEST_PROG->m_dwSTEP == dwTalkStep )
                return true;
        }
    }

    return false;
}