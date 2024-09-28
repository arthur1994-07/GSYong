#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/TextureManager.h"
#include "../../../EngineLib/DxLand/DxLandMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DxRenderStates.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/TipBox.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../../../RanLogicClient/StaticHoldItem.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../InnerInterface.h"
#include "../Interface/MiniMap.h"
#include "../Interface/BasicChat.h"
#include "../Interface/BasicGameMenu.h"
#include "../Interface/BasicInfoView.h"
#include "../Interface/BasicChat.h"
#include "../Interface/UILeftTopGroup.h"
#include "../Interface/BasicPotionTray.h"
#include "../Interface/SkillTrayTab.h"
#include "../Interface/BasicQuickSkillSlot.h"
#include "../Skill/SkillImage.h"
#include "../Skill/SkillWindow.h"
#include "../Skill/SkillWindowToTray.h"
#include "../Item/InventoryWindow.h"
#include "../Item/InventoryInterface.h"
#include "../Quest/QuestWindow.h"
#include "../Quest/QuestList.h"
#include "../Quest/QuestMain.h"
#include "../Quest/QuestStep.h"
#include "ExplainInterface.h"
#include "../Quest/WindowRenewal/QuestWindowRenewal.h"
#include "../Quest/WindowRenewal/List/QuestListRenewal.h"
#include "../Quest/WindowRenewal/Main/QuestMainRenewal.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CExplainInterface::CExplainInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pSkillTrayTab(NULL)
    , m_pBasicInfo(NULL)
    , m_pMiniMap(NULL)
    , m_pChatBox(NULL)
    , m_pQuickPostion(NULL)
    , m_pGameMenu(NULL)
    , m_pInventoryWindow(NULL)
    , m_pSkillWindow(NULL)
    , m_dwCount(0)
    , m_pSlotBox(NULL)
{
    for (int i=0; i<SLOTBOX_SIZE; i++)
    {
        m_bSlotBoxVisible[i] = FALSE;
    }

    m_bSlotBoxVisibleAtInven = FALSE;
}

CExplainInterface::~CExplainInterface(void)
{
}

void CExplainInterface::CreateSubControl ()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    {
        m_pSlotBox = new CTipBox(m_pEngineDevice);;
        m_pSlotBox->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pSlotBox->CreateTipBoxNoBody( "QUEST_WINDOW_LIST_PAGE_BACK_LUXES" );
        RegisterControl ( m_pSlotBox );
        m_pSlotBox->SetVisibleSingle( FALSE );
    }

    {
        m_pSkillTrayTab = new CTipBox(m_pEngineDevice);;
        m_pSkillTrayTab->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE, TUTORIAL_TIP_BOX );
        m_pSkillTrayTab->CreateTipBoxNoBody( "QUICK_SKILL_TRAY_TAB_WINDOW" );		
        RegisterControl ( m_pSkillTrayTab );
        m_pSkillTrayTab->SetVisibleSingle( FALSE );
    }

    {
        m_pBasicInfo = new CTipBox(m_pEngineDevice);;
        m_pBasicInfo->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pBasicInfo->CreateTipBoxNoBody( "BASIC_INFO_VIEW" );
        RegisterControl ( m_pBasicInfo );
        m_pBasicInfo->SetVisibleSingle( FALSE );
    }

    {
        m_pMiniMap = new CTipBox(m_pEngineDevice);;
        m_pMiniMap->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pMiniMap->CreateTipBoxNoBody( "MINIMAP_BACK" );
        RegisterControl( m_pMiniMap );
        m_pMiniMap->SetVisibleSingle( FALSE );

        {
            m_pMiniMap->SetGlobalPos ( m_pInterface->GetMiniMap()->GetMinimapBackGlobalPos() );
            m_pMiniMap->SetLocalPos ( m_pInterface->GetMiniMap()->GetMinimapBackGlobalPos() );
        }
    }

    {
        m_pChatBox = new CTipBox(m_pEngineDevice);;
        m_pChatBox->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pChatBox->CreateTipBoxNoBody( "BASIC_CHAT_BOX" );
        RegisterControl ( m_pChatBox );
        m_pChatBox->SetVisibleSingle( FALSE );

        const long lResolution = m_pInterface->UiGetResolution();
        WORD X_RES = HIWORD ( lResolution );
        WORD Y_RES = LOWORD ( lResolution );

        {
            D3DXVECTOR2 vPos;
            vPos.x = X_RES * 0.902f;
            vPos.y = Y_RES * 0.02f;
            m_pChatBox->SetGlobalPos ( m_pInterface->GetBasicChatWindow()->GetUIWindowGlobalPos() );
            m_pChatBox->SetLocalPos ( m_pInterface->GetBasicChatWindow()->GetUIWindowGlobalPos() );
        }
    }

    {
        m_pQuickPostion = new CTipBox(m_pEngineDevice);;
        m_pQuickPostion->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pQuickPostion->CreateTipBoxNoBody( "BASIC_QUICK_POTION_TRAY_BOX" );
        RegisterControl ( m_pQuickPostion );
        m_pQuickPostion->SetVisibleSingle( FALSE );
    }

    {
        m_pGameMenu = new CTipBox(m_pEngineDevice);;
        m_pGameMenu->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pGameMenu->CreateTipBoxNoBody( "GAME_MENU" );
        RegisterControl ( m_pGameMenu );

        m_pGameMenu->SetGlobalPos ( m_pInterface->GameMenuGetGlobalPos() );
        m_pGameMenu->SetLocalPos ( m_pInterface->GameMenuGetGlobalPos() );
        m_pGameMenu->SetVisibleSingle( FALSE );

    }

    {
        m_pInventoryWindow = new CTipBox(m_pEngineDevice);;
        m_pInventoryWindow->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pInventoryWindow->CreateTipBoxNoBody( "INVENTORY_WINDOW" );
        RegisterControl ( m_pInventoryWindow );

		UIRECT rcInvenPos;
		if( m_pInterface->GetInventory() )
			rcInvenPos = m_pInterface->GetInventory()->GetUIWindowGlobalPos();
		else
			rcInvenPos = m_pInterface->GetInventoryWindow()->GetUIWindowGlobalPos();

        m_pInventoryWindow->SetGlobalPos ( rcInvenPos );
        m_pInventoryWindow->SetLocalPos ( rcInvenPos );
        m_pInventoryWindow->SetVisibleSingle( FALSE );

    }

    {
        m_pSkillWindow = new CTipBox(m_pEngineDevice);;
        m_pSkillWindow->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pSkillWindow->CreateTipBoxNoBody( "SKILL_WINDOW" );
        RegisterControl ( m_pSkillWindow );

        m_pSkillWindow->SetGlobalPos ( m_pInterface->GetSkillWindow()->GetUIWindowGlobalPos() );
        m_pSkillWindow->SetLocalPos ( m_pInterface->GetSkillWindow()->GetUIWindowGlobalPos() );
        m_pSkillWindow->SetVisibleSingle( FALSE );

    }

    {
        m_pQuestList = new CTipBox(m_pEngineDevice);;
        m_pQuestList->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pQuestList->CreateTipBoxNoBody( "QUEST_LIST_BACK" );
        RegisterControl ( m_pQuestList );
        m_pQuestList->SetVisibleSingle( FALSE );

    }

    {
        m_pQuestMain = new CTipBox(m_pEngineDevice);;
        m_pQuestMain->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pQuestMain->CreateTipBoxNoBody( "TUTORIAL_QUEST_MAIN_DESC_BACK" );
        RegisterControl ( m_pQuestMain );
        m_pQuestMain->SetVisibleSingle( FALSE );

    }

    {
        m_pQuestMoreButton = new CTipBox(m_pEngineDevice);;
        m_pQuestMoreButton->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pQuestMoreButton->CreateTipBoxNoBody( "TUTORIAL_QUEST_MAIN_BUTTON_M" );
        RegisterControl ( m_pQuestMoreButton );

        m_pQuestMoreButton->SetVisibleSingle( FALSE );

    }

    {
        m_pQuestStep = new CTipBox(m_pEngineDevice);;
        m_pQuestStep->CreateSub( this, "TUTORIAL_EXPLAIN_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pQuestStep->CreateTipBoxNoBody( "QUEST_WINDOW_STEP_BACK_LUXES" );
        RegisterControl ( m_pQuestStep );

        m_pQuestStep->SetVisibleSingle( FALSE );

    }

    {
        CUIControl* pLeftDir = new CUIControl(m_pEngineDevice);
        pLeftDir->CreateSub ( this, "TUTORIAL_INTERFACE_LEFT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        RegisterControl ( pLeftDir );
        m_pLeftDir = pLeftDir;
        m_pLeftDir->SetVisibleSingle( FALSE );
    }

    {
        CUIControl* pRightDir = new CUIControl(m_pEngineDevice);
        pRightDir->CreateSub ( this, "TUTORIAL_INTERFACE_RIGHT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        RegisterControl ( pRightDir );
        m_pRightDir = pRightDir;
        m_pRightDir->SetVisibleSingle( FALSE );
    }

    {
        CUIControl* pUpDir = new CUIControl(m_pEngineDevice);
        pUpDir->CreateSub ( this, "TUTORIAL_INTERFACE_UP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        RegisterControl ( pUpDir );
        m_pUpDir = pUpDir;
        m_pUpDir->SetVisibleSingle( FALSE );
    }

    {
        CUIControl* pDownDir = new CUIControl(m_pEngineDevice);
        pDownDir->CreateSub ( this, "TUTORIAL_INTERFACE_DOWN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        RegisterControl ( pDownDir );
        m_pDownDir = pDownDir;
        m_pDownDir->SetVisibleSingle( FALSE );
    }
}

void CExplainInterface::SetSlotBoxOriginPos( UIRECT rcWindow, int nIndex )
{
    m_SlotBoxOrigin_rc[ nIndex ] = rcWindow;
}

void CExplainInterface::SetSlotBoxOriginPos( D3DXVECTOR2 vPos, int nIndex )
{
    m_vSlotBoxOrigin[ nIndex ] = vPos;
}

void CExplainInterface::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    m_fFlowTime += fElapsedTime;

    if ( m_fFlowTime >= SPEED_OF_UI_BLINKING )
    {
        m_fFlowTime = SPEED_OF_UI_BLINKING;
    }

    // move arrow mark.
    if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubThreeStep()
        || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
    {
        m_pBasicInfo->SetLocalPos( m_pInterface->GetBasicInfoView()->GetUIWindowGlobalPos() );
        m_pBasicInfo->SetGlobalPos( m_pInterface->GetBasicInfoView()->GetUIWindowGlobalPos() );

        const UIRECT& rcWindow = m_pUpDir->GetGlobalPos();
        const UIRECT& rcTarget = m_pBasicInfo->GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = rcTarget.left + rcTarget.sizeX * 0.5f - rcWindow.sizeX * 0.5f;
        vPos.y = rcTarget.bottom;

        D3DXVECTOR2 vTar( vPos.x, vPos.y + rcWindow.sizeY );
        D3DXVECTOR2 vDir = vPos - vTar;
        vPos = vTar + vDir * m_fFlowTime / SPEED_OF_UI_BLINKING;

        m_pUpDir->SetLocalPos( vPos );
        m_pUpDir->SetGlobalPos( vPos );

        m_pUpDir->SetVisibleSingle( TRUE );

        m_pRightDir->SetVisibleSingle( FALSE );
        m_pLeftDir->SetVisibleSingle( FALSE );
        m_pDownDir->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubFourStep()
        || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
    {
        m_pQuickPostion->SetLocalPos( m_pInterface->GetUILeftTopGroup()->GetBasicPotionTray()->GetGlobalPos() );
        m_pQuickPostion->SetGlobalPos( m_pInterface->GetUILeftTopGroup()->GetBasicPotionTray()->GetGlobalPos() );

        const UIRECT& rcWindow = m_pLeftDir->GetGlobalPos();
        const UIRECT& rcTarget = m_pQuickPostion->GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = rcTarget.right;
        vPos.y = rcTarget.top + rcTarget.sizeY * 0.5f - rcWindow.sizeY * 0.5f;

        D3DXVECTOR2 vTar( vPos.x + rcWindow.sizeX, vPos.y );
        D3DXVECTOR2 vDir = vPos - vTar;
        vPos = vTar + vDir * m_fFlowTime / SPEED_OF_UI_BLINKING;

        m_pLeftDir->SetLocalPos( vPos );
        m_pLeftDir->SetGlobalPos( vPos );

        m_pLeftDir->SetVisibleSingle( TRUE );

        m_pRightDir->SetVisibleSingle( FALSE );
        m_pUpDir->SetVisibleSingle( FALSE );
        m_pDownDir->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubFiveStep()
        || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
    {
        m_pSkillTrayTab->ResizeWindow( m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos() );

        m_pSkillTrayTab->SetLocalPos( m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos() );
        m_pSkillTrayTab->SetGlobalPos( m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos() );

        const UIRECT& rcWindow = m_pLeftDir->GetGlobalPos();
        const UIRECT& rcTarget = m_pSkillTrayTab->GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = rcTarget.right;
        vPos.y = rcTarget.top + rcTarget.sizeY * 0.5f - rcWindow.sizeY * 0.5f;

        D3DXVECTOR2 vTar( vPos.x + rcWindow.sizeX, vPos.y );
        D3DXVECTOR2 vDir = vPos - vTar;
        vPos = vTar + vDir * m_fFlowTime / SPEED_OF_UI_BLINKING;

        m_pLeftDir->SetLocalPos( vPos );
        m_pLeftDir->SetGlobalPos( vPos );

        m_pLeftDir->SetVisibleSingle( TRUE );

        m_pRightDir->SetVisibleSingle( FALSE );
        m_pUpDir->SetVisibleSingle( FALSE );
        m_pDownDir->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubSixStep()
        || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
    {
        m_pChatBox->ResizeWindow( m_pInterface->GetBasicChatWindow()->GetUIWindowGlobalPos() );

        m_pChatBox->SetLocalPos( m_pInterface->GetBasicChatWindow()->GetUIWindowGlobalPos() );
        m_pChatBox->SetGlobalPos( m_pInterface->GetBasicChatWindow()->GetUIWindowGlobalPos() );

        const UIRECT& rcWindow = m_pDownDir->GetGlobalPos();
        const UIRECT& rcTarget = m_pChatBox->GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = rcTarget.left + rcTarget.sizeX * 0.5f - rcWindow.sizeX * 0.5f;
        vPos.y = rcTarget.top - rcWindow.sizeY;

        D3DXVECTOR2 vTar( vPos.x, vPos.y - rcWindow.sizeY );
        D3DXVECTOR2 vDir = vPos - vTar;
        vPos = vTar + vDir * m_fFlowTime / SPEED_OF_UI_BLINKING;

        m_pDownDir->SetLocalPos( vPos );
        m_pDownDir->SetGlobalPos( vPos );

        m_pDownDir->SetVisibleSingle( TRUE );

        m_pRightDir->SetVisibleSingle( FALSE );
        m_pUpDir->SetVisibleSingle( FALSE );
        m_pLeftDir->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubSevenStep()
        || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
    {
        const UIRECT& rcWindow = m_pDownDir->GetGlobalPos();
        const UIRECT& rcTarget = m_pGameMenu->GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = rcTarget.left + rcTarget.sizeX * 0.5f - rcWindow.sizeX * 0.5f;
        vPos.y = rcTarget.top - rcWindow.sizeY;

        D3DXVECTOR2 vTar( vPos.x, vPos.y - rcWindow.sizeY );
        D3DXVECTOR2 vDir = vPos - vTar;
        vPos = vTar + vDir * m_fFlowTime / SPEED_OF_UI_BLINKING;

        m_pDownDir->SetLocalPos( vPos );
        m_pDownDir->SetGlobalPos( vPos );

        m_pDownDir->SetVisibleSingle( TRUE );

        m_pRightDir->SetVisibleSingle( FALSE );
        m_pUpDir->SetVisibleSingle( FALSE );
        m_pLeftDir->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubEightStep()
        || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
    {
        m_pMiniMap->SetGlobalPos ( m_pInterface->GetMiniMap()->GetMinimapBackGlobalPos() );
        m_pMiniMap->SetLocalPos ( m_pInterface->GetMiniMap()->GetMinimapBackGlobalPos() );

        const UIRECT& rcWindow = m_pRightDir->GetGlobalPos();
        const UIRECT& rcTarget = m_pMiniMap->GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = rcTarget.left - rcWindow.sizeX;
        vPos.y = rcTarget.top + rcTarget.sizeY * 0.5f - rcWindow.sizeY * 0.5f;

        D3DXVECTOR2 vTar( vPos.x - rcWindow.sizeX, vPos.y);
        D3DXVECTOR2 vDir = vPos - vTar;
        vPos = vTar + vDir * m_fFlowTime / SPEED_OF_UI_BLINKING;

        m_pRightDir->SetLocalPos( vPos );
        m_pRightDir->SetGlobalPos( vPos );

        m_pRightDir->SetVisibleSingle( TRUE );

        m_pLeftDir->SetVisibleSingle( FALSE );
        m_pUpDir->SetVisibleSingle( FALSE );
        m_pDownDir->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
        && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
    {
        m_pLeftDir->SetVisibleSingle( FALSE );
        m_pRightDir->SetVisibleSingle( FALSE );
        m_pUpDir->SetVisibleSingle( FALSE );
        m_pDownDir->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
        && m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
    {
		UIRECT rcTarWin;
		if( m_pInterface->GetInventory() )
			rcTarWin = m_pInterface->GetInventory()->GetUIWindowGlobalPos();
		else
			rcTarWin = m_pInterface->GetInventoryWindow()->GetUIWindowGlobalPos();

        D3DXVECTOR2& vPos = m_vSlotBoxOrigin[ 0 ];

        GLCharacter* pGLChar = m_pGaeaClient->GetCharacter();

        if ( !holditem::HaveHoldItem() )
        {
			// 들고 있는 아이템이 없다. 인벤토리 윈도우에 스킬 아이콘 주위에 UI 그리자.
            D3DXVECTOR2& vPos = m_vSlotBoxOrigin[ 0 ];
            D3DXVECTOR2 vGap;
            vGap.x = rcTarWin.left - m_SlotBoxOrigin_rc[ 0 ].left;
            vGap.y = rcTarWin.top - m_SlotBoxOrigin_rc[ 0 ].top;

            if ( vGap.x != 0.0f || vGap.y != 0.0f )
                m_SlotBoxOrigin_rc[ 0 ] = rcTarWin;

            //vPos.x += vGap.x;
            //vPos.y += vGap.y;

            m_pSlotBox->SetLocalPos( vPos );
            m_pSlotBox->SetGlobalPos( vPos );

            //m_pSlotBox->SetVisibleSingle( m_bSlotBoxVisibleAtInven );
            m_pQuickPostion->SetVisibleSingle( FALSE );
        }
        else
        {	// 들고 있는 스킬이 있다. 스킬 퀵슬롯 주위에 UI 그리자.
            m_pQuickPostion->SetLocalPos( m_pInterface->GetUILeftTopGroup()->GetBasicPotionTray()->GetGlobalPos() );
            m_pQuickPostion->SetGlobalPos( m_pInterface->GetUILeftTopGroup()->GetBasicPotionTray()->GetGlobalPos() );

            m_pSlotBox->SetVisibleSingle( FALSE );
            //m_pSkillTrayTab->SetVisibleSingle( TRUE );
        }
    }

    if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
        && m_pGaeaClient->GetTutorial()->IsSubSixStep() )
    {
        if ( m_pGaeaClient->GetTutorial()->m_sHoldSkill == NATIVEID_NULL() )
        {	// 들고 있는 스킬이 없다. 스킬 윈도우에 스킬 아이콘 주위에 UI 그리자.
            UIRECT rcTarWin = m_pInterface->GetSkillWindow()->GetUIWindowGlobalPos();
            D3DXVECTOR2& vPos = m_vSlotBoxOrigin[ 0 ];

            D3DXVECTOR2 vGap;
            vGap.x = rcTarWin.left - m_SlotBoxOrigin_rc[ 0 ].left;
            vGap.y = rcTarWin.top - m_SlotBoxOrigin_rc[ 0 ].top;

            if ( vGap.x != 0.0f || vGap.y != 0.0f )
                m_SlotBoxOrigin_rc[ 0 ] = rcTarWin;

            vPos.x += vGap.x;
            vPos.y += vGap.y;

            m_pSlotBox->SetLocalPos( vPos );
            m_pSlotBox->SetGlobalPos( vPos );

            //m_pSlotBox->SetVisibleSingle( m_bSlotBoxVisible[ 0 ] );
            m_pSkillTrayTab->SetVisibleSingle( FALSE );
        }
        else
        {	// 들고 있는 스킬이 있다. 스킬 퀵슬롯 주위에 UI 그리자.
            m_pSkillTrayTab->SetLocalPos( m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos() );
            m_pSkillTrayTab->SetGlobalPos( m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos() );

            m_pSlotBox->SetVisibleSingle( FALSE );
            //m_pSkillTrayTab->SetVisibleSingle( TRUE );
        }
    }

    if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
        && m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
    {
        if ( m_pGaeaClient->GetTutorial()->m_sHoldSkill == NATIVEID_NULL() )
        {	// 들고 있는 스킬이 없다. 스킬 윈도우에 스킬 아이콘 주위에 UI 그리자.
            UIRECT rcTarWin = m_pInterface->GetSkillWindow()->GetUIWindowGlobalPos();
            D3DXVECTOR2& vPos = m_vSlotBoxOrigin[ 1 ];

            D3DXVECTOR2 vGap;
            vGap.x = rcTarWin.left - m_SlotBoxOrigin_rc[ 1 ].left;
            vGap.y = rcTarWin.top - m_SlotBoxOrigin_rc[ 1 ].top;

            if ( vGap.x != 0.0f || vGap.y != 0.0f )
                m_SlotBoxOrigin_rc[ 1 ] = rcTarWin;

            vPos.x += vGap.x;
            vPos.y += vGap.y;

            m_pSlotBox->SetLocalPos( vPos );
            m_pSlotBox->SetGlobalPos( vPos );

            //m_pSlotBox->SetVisibleSingle( m_bSlotBoxVisible[ 1 ] );
            m_pSkillTrayTab->SetVisibleSingle( FALSE );
        }
        else
        {	// 들고 있는 스킬이 있다. 스킬 퀵슬롯 주위에 UI 그리자.
            m_pSkillTrayTab->SetLocalPos( m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos() );
            m_pSkillTrayTab->SetGlobalPos( m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos() );

            m_pSlotBox->SetVisibleSingle( FALSE );
            //m_pSkillTrayTab->SetVisibleSingle( TRUE );
        }
    }

    if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
        && m_pGaeaClient->GetTutorial()->IsSubEightStep() )
    {
        UIRECT rcTarWin = m_pInterface->GetSkillTrayTab()->GetCurrentSkillSlotGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = rcTarWin.left;
        vPos.y = rcTarWin.top;

        m_pSlotBox->SetLocalPos( vPos );
        m_pSlotBox->SetGlobalPos( vPos );

        //m_pSlotBox->SetVisibleSingle( TRUE );
    }

    if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
        && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
    {
        m_pQuestMain->SetVisibleSingle( FALSE );
        m_pQuestMoreButton->SetVisibleSingle( FALSE );
        m_pQuestStep->SetVisibleSingle( FALSE );

        if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
        {
            const UIRECT& rcTarGP = m_pInterface->GetQuestWindowRenewal()->GetListPageGlobalPos();
            m_pQuestList->SetGlobalPos ( rcTarGP );
        }
        else
            m_pQuestList->SetVisibleSingle( FALSE );

    }

    if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
        && m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->THREE_
        && m_pGaeaClient->GetTutorial()->GetCurrentSubStep() <= m_pGaeaClient->GetTutorial()->FOUR_ )
    {
        m_pQuestList->SetVisibleSingle( FALSE );
        m_pQuestMoreButton->SetVisibleSingle( FALSE );
        m_pQuestStep->SetVisibleSingle( FALSE );
        m_pQuestMain->SetVisibleSingle( FALSE );

    }

    if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
        && m_pGaeaClient->GetTutorial()->IsSubSixStep() )
    {
        m_pQuestList->SetVisibleSingle( FALSE );
        m_pQuestMain->SetVisibleSingle( FALSE );
        m_pQuestStep->SetVisibleSingle( FALSE );
        m_pQuestMoreButton->SetVisibleSingle( FALSE );

    }

    if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
        && m_pGaeaClient->GetTutorial()->IsSubEightStep() )
    {
        m_pQuestList->SetVisibleSingle( FALSE );
        m_pQuestMain->SetVisibleSingle( FALSE );
        m_pQuestMoreButton->SetVisibleSingle( FALSE);

        if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
        {
            {
                const UIRECT& rcWindow = m_pQuestStep->GetGlobalPos();
                const UIRECT& rcTarWindow = m_pInterface->GetQuestWindowRenewal()->GetMainStepBackPos();

                D3DXVECTOR2 vPos;
                vPos.x = rcTarWindow.left + rcTarWindow.sizeX * 0.5f - rcWindow.sizeX * 0.5f;
                vPos.y = rcTarWindow.top + rcTarWindow.sizeY * 0.5f - rcWindow.sizeY * 0.5f;

                m_pQuestStep->SetGlobalPos (vPos );
            }
        }
        else
            m_pQuestStep->SetVisibleSingle( FALSE );

    }
    //

    if ( m_fFlowTime >= SPEED_OF_UI_BLINKING )
    {
        if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubThreeStep()
            || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
        {
            if ( m_pBasicInfo->IsVisible() )
            {
                m_pBasicInfo->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pBasicInfo->SetVisibleSingle( TRUE );
            }
        }
        else
        {
            m_pBasicInfo->SetVisibleSingle( FALSE );
        }

        if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubFourStep()
            || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
        {
            if ( m_pQuickPostion->IsVisible() )
            {
                m_pQuickPostion->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pQuickPostion->SetVisibleSingle( TRUE );
            }
        }
        else
        {
            if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
                && m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
                m_pQuickPostion->SetVisibleSingle( m_pQuickPostion->IsVisible() );
            else
                m_pQuickPostion->SetVisibleSingle( FALSE );
        }

        if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubFiveStep()
            || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
        {
            if ( m_pSkillTrayTab->IsVisible() )
            {
                m_pSkillTrayTab->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pSkillTrayTab->SetVisibleSingle( TRUE );
            }
        }
        else
        {
            if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
                && m_pGaeaClient->GetTutorial()->IsSubSixStep()
                || m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
                && m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
                m_pSkillTrayTab->SetVisibleSingle( m_pSkillTrayTab->IsVisible() );
            else
                m_pSkillTrayTab->SetVisibleSingle( FALSE );
        }
        //

        if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubSixStep()
            || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
        {
            if ( m_pChatBox->IsVisible() )
            {
                m_pChatBox->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pChatBox->SetVisibleSingle( TRUE );
            }
        }
        else
        {
            m_pChatBox->SetVisibleSingle( FALSE );
        }

        if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubSevenStep()
            || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
        {
            if ( m_pGameMenu->IsVisible() )
            {
                m_pGameMenu->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pGameMenu->SetVisibleSingle( TRUE );
            }
        }
        else
        {
            m_pGameMenu->SetVisibleSingle( FALSE );
        }

        if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubEightStep()
            || m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
        {
            if ( m_pMiniMap->IsVisible() )
            {
                m_pMiniMap->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pMiniMap->SetVisibleSingle( TRUE );
            }
        }
        else
        {
            m_pMiniMap->SetVisibleSingle( FALSE );
        }

        if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
            && m_pGaeaClient->GetTutorial()->IsSubThreeStep())
        {
            GLCharacter* pGLChar = m_pGaeaClient->GetCharacter();

            if ( !holditem::HaveHoldItem() )
            {
                if ( m_bSlotBoxVisibleAtInven )
                {
                    if ( m_pSlotBox->IsVisible() )
                    {
                        m_pSlotBox->SetVisibleSingle( FALSE );
                    }
                    else
                    {
                        m_pSlotBox->SetVisibleSingle( TRUE );
                    }
                }
            }
            else
            {
                if ( m_pQuickPostion->IsVisible() )
                {
                    m_pQuickPostion->SetVisibleSingle( FALSE );
                }
                else
                {
                    m_pQuickPostion->SetVisibleSingle( TRUE );
                }
            }
        }

        if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
            && m_pGaeaClient->GetTutorial()->IsSubSixStep() )
        {
            if ( m_pGaeaClient->GetTutorial()->m_sHoldSkill == NATIVEID_NULL() )
            {
                if ( m_bSlotBoxVisible[0] )
                {
                    if ( m_pSlotBox->IsVisible() )
                    {
                        m_pSlotBox->SetVisibleSingle( FALSE );
                    }
                    else
                    {
                        m_pSlotBox->SetVisibleSingle( TRUE );
                    }
                }
            }
            else
            {
                if ( m_pSkillTrayTab->IsVisible() )
                {
                    m_pSkillTrayTab->SetVisibleSingle( FALSE );
                }
                else
                {
                    m_pSkillTrayTab->SetVisibleSingle( TRUE );
                }
            }
        }

        if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
            && m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
        {
            if ( m_pGaeaClient->GetTutorial()->m_sHoldSkill == NATIVEID_NULL() )
            {
                if ( m_bSlotBoxVisible[1] )
                {
                    if ( m_pSlotBox->IsVisible() )
                    {
                        m_pSlotBox->SetVisibleSingle( FALSE );
                    }
                    else
                    {
                        m_pSlotBox->SetVisibleSingle( TRUE );
                    }
                }
            }
            else
            {
                if ( m_pSkillTrayTab->IsVisible() )
                {
                    m_pSkillTrayTab->SetVisibleSingle( FALSE );
                }
                else
                {
                    m_pSkillTrayTab->SetVisibleSingle( TRUE );
                }
            }
        }

        if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
            && m_pGaeaClient->GetTutorial()->IsSubEightStep() )
        {
            if ( m_pSlotBox->IsVisible() )
            {
                m_pSlotBox->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pSlotBox->SetVisibleSingle( TRUE );
            }
        }

        if ( m_pGaeaClient->GetTutorial()->IsExplainInterfaceStep()
            && m_pGaeaClient->GetTutorial()->IsSubTwoStep()  )
        {
            m_dwCount++;

            if ( m_dwCount >= 4 )
            {
                m_dwCount = 0;

                m_pRightDir->SetVisibleSingle( FALSE );
                m_pLeftDir->SetVisibleSingle( FALSE );
                m_pUpDir->SetVisibleSingle( FALSE );
                m_pDownDir->SetVisibleSingle( FALSE );

                m_pSkillTrayTab->SetVisibleSingle( FALSE );
                m_pBasicInfo->SetVisibleSingle( FALSE );
                m_pMiniMap->SetVisibleSingle( FALSE );
                m_pChatBox->SetVisibleSingle( FALSE );
                m_pQuickPostion->SetVisibleSingle( FALSE );
                m_pGameMenu->SetVisibleSingle( FALSE );

                m_pGaeaClient->GetTutorial()->NextSubStep();

                CString strName;
                strName.Format( "TUTORIAL_DIALOGUE_STEP_TWO_1" );
                m_pInterface->GetTutorialDialogue()->SetDialogueText( ID2GAMEINTEXT(strName) );
            }
        }

        else if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
            && m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
        {
            if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
            {
                m_pQuestList->SetVisibleSingle( !m_pQuestList->IsVisible() );
            }
            else
            {
                m_pQuestList->SetVisibleSingle( FALSE );
            }
        }
        else if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
            && m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->THREE_
            && m_pGaeaClient->GetTutorial()->GetCurrentSubStep() <= m_pGaeaClient->GetTutorial()->FOUR_ )
        {
            m_pQuestMain->SetVisibleSingle( FALSE );
        }
        else if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
            && m_pGaeaClient->GetTutorial()->IsSubSixStep() )
        {
            m_pQuestMoreButton->SetVisibleSingle( FALSE );
        }
        else if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
            && m_pGaeaClient->GetTutorial()->IsSubEightStep() )
        {
            if ( m_pInterface->UiIsVisibleGroup( QUEST_WINDOW ) )
            {
                m_pQuestStep->SetVisibleSingle( !m_pQuestStep->IsVisible() );
            }
            else
            {
                m_pQuestStep->SetVisibleSingle( FALSE );
            }
        }

        m_fFlowTime = 0.0f;
    }
}

HRESULT CExplainInterface::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    HRESULT hr = S_OK;
    hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
    if ( FAILED ( hr ) ) return hr;

    const long lResolution = m_pInterface->UiGetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        m_pMiniMap->SetGlobalPos ( m_pInterface->GetMiniMap()->GetMinimapBackGlobalPos() );
        m_pMiniMap->SetLocalPos ( m_pInterface->GetMiniMap()->GetMinimapBackGlobalPos() );
    }

    {

        D3DXVECTOR2 vPos;
        vPos.x = X_RES * 0.902f;
        vPos.y = Y_RES * 0.02f;
        m_pChatBox->SetGlobalPos( m_pInterface->GetBasicChatWindow()->GetUIWindowGlobalPos() );
        m_pChatBox->SetLocalPos( m_pInterface->GetBasicChatWindow()->GetUIWindowGlobalPos() );
    }

    {
        m_pGameMenu->SetGlobalPos ( m_pInterface->GameMenuGetGlobalPos() );
        m_pGameMenu->SetLocalPos ( m_pInterface->GameMenuGetGlobalPos() );
    }

    return S_OK;
}

void CExplainInterface::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    if ( (dwMsg & UIMSG_LB_UP) )
    {
        m_fFlowTime = 1.0f;
    }

    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CExplainInterface::SetVisibleSingle( BOOL bVisible )
{
    CUIGroup::SetVisibleSingle( bVisible );

    m_pRightDir->SetVisibleSingle( FALSE );
    m_pLeftDir->SetVisibleSingle( FALSE );
    m_pUpDir->SetVisibleSingle( FALSE );
    m_pDownDir->SetVisibleSingle( FALSE );

    m_pSkillTrayTab->SetVisibleSingle( FALSE );
    m_pBasicInfo->SetVisibleSingle( FALSE );
    m_pMiniMap->SetVisibleSingle( FALSE );
    m_pChatBox->SetVisibleSingle( FALSE );
    m_pQuickPostion->SetVisibleSingle( FALSE );
    m_pGameMenu->SetVisibleSingle( FALSE );
}

CTutorialDamageAlarm::CTutorialDamageAlarm(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pText(NULL)
    , m_fFlowTime(0.0f)
{
}

CTutorialDamageAlarm::~CTutorialDamageAlarm(void)
{
}


void CTutorialDamageAlarm::CreateSubControl ()
{
    {
        CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 13, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
        pText->CreateSub ( this, "TUTORIAL_DAMAGE_ALARM" );
        pText->SetFont ( pFont );
        pText->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl ( pText );
        m_pText = pText;

        if ( m_pText )
        {
            CString strCombine;
            strCombine = ID2GAMEINTEXT("TUTORIAL_DAMAGE_ALARM");
            m_pText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
            m_pText->SetVisibleSingle( TRUE );
        }
    }
}

void CTutorialDamageAlarm::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible () ) return ;

    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    const long lResolution = m_pInterface->UiGetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcWindow = GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = X_RES * 0.5f - rcWindow.sizeX * 0.5f;
        vPos.y = Y_RES * 0.7f;

        SetLocalPos( vPos );
        SetGlobalPos( vPos );
    }

    m_fFlowTime += fElapsedTime;

    if ( m_fFlowTime >= 3.0f )
    {
        m_fFlowTime = 0.0f;
        SetVisibleSingle( FALSE );
    }
}


void CTutorialDamageAlarm::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
}

void CTutorialDamageAlarm::SetVisibleSingle( BOOL bVisible )
{
    CUIGroup::SetVisibleSingle( bVisible );

    m_pText->SetVisibleSingle( bVisible );
}

HRESULT CTutorialDamageAlarm::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    HRESULT hr = S_OK;
    hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
    if ( FAILED ( hr ) ) return hr;

    return S_OK;
}

BOOL CExplainInterface::IsSlotBoxVisible(unsigned int Index) const
{
    if (Index >= SLOTBOX_SIZE)
        return FALSE;
    else
        return m_bSlotBoxVisible[Index];
}

void CExplainInterface::SetSlotBoxVisible(unsigned int Index, BOOL Visible)
{
    if (Index >= SLOTBOX_SIZE)
        return;
    else
        m_bSlotBoxVisible[Index] = Visible;
}

void CExplainInterface::SetSlotBoxVisibleSingle( BOOL bVisible )
{
    m_pSlotBox->SetVisibleSingle( bVisible );
}

const UIRECT& CExplainInterface::GetSlotBoxGlobalPos()
{
    return m_pSlotBox->GetGlobalPos();
}

MyExplainInterface::MyExplainInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CExplainInterface( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyExplainInterface::CreateUIWindowAndRegisterOwnership()
{
    CExplainInterface::Create( EXPLAIN_INTERFACE, "EXPLAIN_INTERFACE_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CExplainInterface::CreateSubControl();
    CExplainInterface::SetAlignFlag( UI_FLAG_LEFT | UI_FLAG_TOP );
    CExplainInterface::m_pInterface->UiRegisterControl( this, true );
    CExplainInterface::m_pInterface->UiShowGroupTop( EXPLAIN_INTERFACE );
    CExplainInterface::m_pInterface->UiHideGroup( EXPLAIN_INTERFACE );
}

BOOL MyExplainInterface::IsSlotBoxVisible( unsigned int Index ) const
{
    return CExplainInterface::IsSlotBoxVisible( Index );
}

BOOL MyExplainInterface::IsSlotBoxVisibleAtInven() const
{
    return CExplainInterface::IsSlotBoxVisibleAtInven();
}

void MyExplainInterface::SetSlotBoxVisible( unsigned int Index, BOOL Visible )
{
    CExplainInterface::SetSlotBoxVisible( Index, Visible );
}

void MyExplainInterface::SetSlotBoxVisibleAtInven( BOOL State )
{
    CExplainInterface::SetSlotBoxVisibleAtInven( State );
}

CTipBox* MyExplainInterface::GetSlotBox()
{
    return CExplainInterface::GetSlotBox();
}

void MyExplainInterface::SetSlotBoxOriginPos( D3DXVECTOR2 vPos, int nIndex )
{
    CExplainInterface::SetSlotBoxOriginPos( vPos, nIndex );
}

void MyExplainInterface::SetSlotBoxOriginPos( UIRECT rcWindow, int nIndex )
{
    CExplainInterface::SetSlotBoxOriginPos( rcWindow, nIndex );
}

void MyExplainInterface::SetSlotBoxVisibleSingle( BOOL bVisible )
{
    CExplainInterface::SetSlotBoxVisibleSingle( bVisible );
}

const UIRECT& MyExplainInterface::GetSlotBoxGlobalPos()
{
    return CExplainInterface::GetSlotBoxGlobalPos();
}