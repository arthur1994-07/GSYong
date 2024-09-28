#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"

#include "BallonText.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/TextureManager.h"
#include "../../../EngineLib/DxLand/DxLandMan.h"
#include "../../../EngineLib/DxTools/DxRenderStates.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/TipBox.h"

#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Item/InventoryWindow.h"
#include "../Item/InventoryInterface.h"
#include "../Skill/SkillWindowToTray.h"
#include "../Interface/MiniMap.h"
#include "../Interface/UILeftTopGroup.h"
#include "../Interface/BasicPotionTray.h"
#include "../Interface/SkillTrayTab.h"
#include "../Display/TargetInfoDisplay.h"

#include "MinimapTarget.h"
#include "ExplainInterface.h"
#include "TutorialDialogue.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBalloonText::CBalloonText(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pMain(NULL)
    , m_pEdgeLT(NULL)
    , m_pEdgeRT(NULL)
    , m_pEdgeLB(NULL)
    , m_pEdgeRB(NULL)
    , m_pLClick(NULL)
    , m_pRClick(NULL)
    , m_pMClick(NULL)
    , m_pScroll(NULL)
    , m_pNull(NULL)
    , m_pText(NULL)
    , m_pNormalText(NULL)
    , m_pSkillText(NULL)
    , m_fFlowTime(0.0f)
{
}

CBalloonText::~CBalloonText(void)
{
}

void CBalloonText::CreateSubControl()
{
	{
		CUIControl* pMain = new CUIControl(m_pEngineDevice);
		pMain->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_MAIN" );
		RegisterControl( pMain );
		m_pMain = pMain;
		m_pMain->SetVisibleSingle( FALSE );
	}

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	{
		CBasicTextBox* pExplainText = new CBasicTextBox(m_pEngineDevice);
		pExplainText->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_BOX" );
		pExplainText->SetFont( pFont );
		pExplainText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pExplainText );
		m_pText = pExplainText;
		m_pText->SetVisibleSingle( FALSE );

		if ( m_pText )
		{
			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_ACTIVE_SKILL" );

			m_pText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}

	{
		CBasicTextBox* pNormalText = new CBasicTextBox(m_pEngineDevice);
		pNormalText->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_BOX" );
		pNormalText->SetFont( pFont );
		pNormalText->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_TOP );
		RegisterControl( pNormalText );
		m_pNormalText = pNormalText;
		m_pNormalText->SetVisibleSingle( FALSE );

		if ( m_pNormalText )
		{
			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_NORMAL_ATTACK" );

			m_pNormalText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}

	{
		CBasicTextBox* pSkillText = new CBasicTextBox(m_pEngineDevice);
		pSkillText->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_BOX" );
		pSkillText->SetFont( pFont );
		pSkillText->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
		RegisterControl( pSkillText );
		m_pSkillText = pSkillText;
		m_pSkillText->SetVisibleSingle( FALSE );

		if ( m_pSkillText )
		{
			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_SKILL_ATTACK" );

			m_pSkillText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}

	{
		CUIControl* pNull = new CUIControl(m_pEngineDevice);
		pNull->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_NULL" );
		RegisterControl( pNull );
		m_pNull = pNull;
		m_pNull->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pLClick = new CUIControl(m_pEngineDevice);
		pLClick->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_LEFT_CLICK" );
		RegisterControl( pLClick );
		m_pLClick = pLClick;
		m_pLClick->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pRClick = new CUIControl(m_pEngineDevice);
		pRClick->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_RIGHT_CLICK" );
		RegisterControl( pRClick );
		m_pRClick = pRClick;
		m_pRClick->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pMClick = new CUIControl(m_pEngineDevice);
		pMClick->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_MIDDLE_CLICK" );
		RegisterControl( pMClick );
		m_pMClick = pMClick;
		m_pMClick->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pScroll = new CUIControl(m_pEngineDevice);
		pScroll->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_SCROLL" );
		RegisterControl( pScroll );
		m_pScroll = pScroll;
		m_pScroll->SetVisibleSingle( FALSE );
	}

	// Note : 모퉁이를 제일 마지막에 그려야되기 때문에, 이 뒤에서 컨트롤 등록해야한다. by luxes.
	{
		CUIControl* pEdgeLT = new CUIControl(m_pEngineDevice);
		pEdgeLT->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_LT" );
		RegisterControl( pEdgeLT );
		m_pEdgeLT = pEdgeLT;
		m_pEdgeLT->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pEdgeRT = new CUIControl(m_pEngineDevice);
		pEdgeRT->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_RT" );
		RegisterControl( pEdgeRT );
		m_pEdgeRT = pEdgeRT;
		m_pEdgeRT->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pEdgeLB = new CUIControl(m_pEngineDevice);
		pEdgeLB->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_LB" );
		RegisterControl( pEdgeLB );
		m_pEdgeLB = pEdgeLB;
		m_pEdgeLB->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pEdgeRB = new CUIControl(m_pEngineDevice);
		pEdgeRB->CreateSub( this, "TUTORIAL_BALLOON_TEXT_MOUSE_RB" );
		RegisterControl( pEdgeRB );
		m_pEdgeRB = pEdgeRB;
		m_pEdgeRB->SetVisibleSingle( FALSE );
	}
	//
}

void CBalloonText::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fFlowTime += fElapsedTime;

	m_pText->SetVisibleSingle( FALSE );
	m_pNormalText->SetVisibleSingle( FALSE );
	m_pSkillText->SetVisibleSingle( FALSE );

	if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubEightStep() )
	{
		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( FALSE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( TRUE );

		//m_pLClick->SetVisibleSingle( TRUE );
		m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pLClick->IsVisible() )
			{
				m_pLClick->SetVisibleSingle( FALSE );
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLClick->SetVisibleSingle( TRUE );
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
	else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubElevenStep() )
	{
		D3DXVECTOR3 vPos = m_pGaeaClient->GetTutorial()->m_vClickMoveTargetPos[ 0 ];
		vPos.y += 10.0f;
		D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );	

		const UIRECT& rcOriginPos = GetGlobalPos();
		int nPosX = (int)( floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ) ) );
		int nPosY = (int)( vScreen.y - rcOriginPos.sizeY );

		SetGlobalPos( D3DXVECTOR2 ( float( nPosX ), float( nPosY ) ) );

		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( FALSE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		//m_pLClick->SetVisibleSingle( TRUE );
		m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pLClick->IsVisible() )
			{
				m_pLClick->SetVisibleSingle( FALSE );
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLClick->SetVisibleSingle( TRUE );
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
	else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubThirteenStep() )
	{
		D3DXVECTOR3 vPos = m_pGaeaClient->GetTutorial()->m_vClickMoveTargetPos[ 1 ];
		vPos.y += 10.0f;
		D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );	

		const UIRECT& rcOriginPos = GetGlobalPos();
		int nPosX = (int)( floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ) ) );
		int nPosY = (int)( vScreen.y - rcOriginPos.sizeY );

		SetGlobalPos ( D3DXVECTOR2( float( nPosX ), float( nPosY ) ) );

		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( FALSE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		//m_pLClick->SetVisibleSingle( TRUE );
		m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pLClick->IsVisible() )
			{
				m_pLClick->SetVisibleSingle( FALSE );
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLClick->SetVisibleSingle( TRUE );
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
	else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubFifteenStep() )
	{
		const UIRECT& rcTempTarget = m_pInterface->MinimapTargetGetGlobalPos();
		const UIRECT& rcWindow = GetGlobalPos();

		D3DXVECTOR2 vPos( rcWindow.left, rcWindow.top );
		vPos.x = rcTempTarget.right;
		vPos.y = rcTempTarget.top - rcWindow.sizeY;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );

		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( FALSE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( TRUE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		//m_pLClick->SetVisibleSingle( TRUE );
		m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pLClick->IsVisible() )
			{
				m_pLClick->SetVisibleSingle( FALSE );
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLClick->SetVisibleSingle( TRUE );
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
	else if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
		&& m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
	{
		if ( !m_pGaeaClient->GetCharacter()->HaveHoldItem() )
		{
			// 들고 있는 아이템이 없다. 인벤토리 윈도우 아이콘 주위에 UI 그리자.
			if( !m_pInterface->IsInventoryOpen() )
				return;

			const UIRECT& rcTempGlobalPos = m_pInterface->GetExplainInterface()->GetSlotBoxGlobalPos();
			const UIRECT& rcTemp = GetGlobalPos();

			float fSlotSize = rcTempGlobalPos.sizeY;
			float fOffset = fSlotSize * 0.15f;

			D3DXVECTOR2 vPos;
			vPos.x = rcTempGlobalPos.right - fOffset;
			vPos.y =  rcTempGlobalPos.bottom - fOffset;

			SetLocalPos( vPos );
			SetGlobalPos( vPos );

			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_PICK_UP" );
			m_pText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_BOTTOM );

			m_pText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

			D3DXVECTOR2 vPosTex;
			vPosTex.x = vPos.x;
			vPosTex.y = vPos.y - rcTemp.sizeY;

			m_pText->SetLocalPos( vPosTex );
			m_pText->SetGlobalPos( vPosTex );
			m_pText->SetVisibleSingle( TRUE );
		}
		else
		{	// 들고 있는 스킬이 있다. 스킬 퀵슬롯 주위에 UI 그리자.

			CUIControl TempControl(m_pEngineDevice);
			TempControl.Create( 1, "INVENTORY_ITEM_SLOT0" );

			const UIRECT& rcTempGlobalPos = m_pInterface->GetExplainInterface()->GetSlotBoxGlobalPos();
			const UIRECT& rcTemp = GetGlobalPos();

			float fSlotSize = rcTempGlobalPos.sizeY;
			float fOffset = fSlotSize * 0.25f;

			UIRECT rcTarget = m_pInterface->GetUILeftTopGroup()->GetBasicPotionTray()->GetGlobalPos();

			D3DXVECTOR2 vPos;
			vPos.x = rcTarget.right - fOffset;
			vPos.y =  rcTarget.bottom - fOffset;

			SetLocalPos( vPos );
			SetGlobalPos( vPos );

			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_PUT_DOWN" );
			m_pText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_BOTTOM );

			m_pText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

			D3DXVECTOR2 vPosTex;
			vPosTex.x = vPos.x;
			vPosTex.y = vPos.y - rcTemp.sizeY;

			m_pText->SetLocalPos( vPosTex );
			m_pText->SetGlobalPos( vPosTex );
			m_pText->SetVisibleSingle( TRUE );
		}

		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( TRUE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		//m_pLClick->SetVisibleSingle( TRUE );
		m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pLClick->IsVisible() )
			{
				m_pLClick->SetVisibleSingle( FALSE );
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLClick->SetVisibleSingle( TRUE );
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
	else if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
		&& m_pGaeaClient->GetTutorial()->IsSubSixStep()
		|| m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
		&& m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
	{
		if ( m_pGaeaClient->GetTutorial()->m_sHoldSkill == NATIVEID_NULL() )
		{	// 들고 있는 스킬이 없다. 스킬 윈도우에 스킬 아이콘 주위에 UI 그리자.
			if ( !m_pInterface->UiIsVisibleGroup( SKILL_WINDOW )	)	return; // 스킬창 안 열려 있으면 리턴.

			const UIRECT& rcTempGlobalPos = m_pInterface->GetExplainInterface()->GetSlotBoxGlobalPos();
			const UIRECT& rcTemp = GetGlobalPos();

			float fSlotSize = rcTempGlobalPos.sizeY;
			float fOffset = fSlotSize * 0.15f;

			D3DXVECTOR2 vPos;
			vPos.x = rcTempGlobalPos.right - fOffset;
			vPos.y =  rcTempGlobalPos.bottom - fOffset;

			SetLocalPos( vPos );
			SetGlobalPos( vPos );

			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_PICK_UP" );
			m_pText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_BOTTOM );

			m_pText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

			D3DXVECTOR2 vPosTex;
			vPosTex.x = vPos.x;
			vPosTex.y = vPos.y - rcTemp.sizeY;

			m_pText->SetLocalPos( vPosTex );
			m_pText->SetGlobalPos( vPosTex );
			m_pText->SetVisibleSingle( TRUE );
		}
		else
		{	// 들고 있는 스킬이 있다. 스킬 퀵슬롯 주위에 UI 그리자.

			CUIControl TempControl(m_pEngineDevice);
			TempControl.Create( 1, "INVENTORY_ITEM_SLOT0" );

			const UIRECT& rcTempGlobalPos = m_pInterface->GetExplainInterface()->GetSlotBoxGlobalPos();
			const UIRECT& rcTemp = GetGlobalPos();

			float fSlotSize = rcTempGlobalPos.sizeY;
			float fOffset = fSlotSize * 0.25f;

			UIRECT rcTarget = m_pInterface->GetSkillTrayTab()->GetUIWindowGlobalPos();

			D3DXVECTOR2 vPos;
			vPos.x = rcTarget.right - fOffset;
			vPos.y =  rcTarget.bottom - fOffset;

			SetLocalPos( vPos );
			SetGlobalPos( vPos );

			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_PUT_DOWN" );
			m_pText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_BOTTOM );

			m_pText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

			D3DXVECTOR2 vPosTex;
			vPosTex.x = vPos.x;
			vPosTex.y = vPos.y - rcTemp.sizeY;

			m_pText->SetLocalPos( vPosTex );
			m_pText->SetGlobalPos( vPosTex );
			m_pText->SetVisibleSingle( TRUE );
		}

		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( TRUE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		//m_pLClick->SetVisibleSingle( TRUE );
		m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pLClick->IsVisible() )
			{
				m_pLClick->SetVisibleSingle( FALSE );
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLClick->SetVisibleSingle( TRUE );
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
	else if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
		&& m_pGaeaClient->GetTutorial()->IsSubEightStep()
		&& m_pInterface->GetTutorialDialogue()->GetTutorialDialogueIndex() == 7 )
	{
		const UIRECT& rcTempGlobalPos = m_pInterface->GetExplainInterface()->GetSlotBoxGlobalPos();

		float fSlotSize = rcTempGlobalPos.sizeY;
		float fOffset = fSlotSize * 0.15f;

		D3DXVECTOR2 vPos;
		vPos.x = rcTempGlobalPos.right - fOffset;
		vPos.y =  rcTempGlobalPos.bottom - fOffset;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );

		CString strCombine;
		strCombine = ID2GAMEINTEXT( "TUTORIAL_ACTIVE_SKILL" );
		m_pText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );

		m_pText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );

		m_pText->SetLocalPos( vPos );
		m_pText->SetGlobalPos( vPos );
		m_pText->SetVisibleSingle( TRUE );

		m_pMain->SetVisibleSingle( FALSE );

		//m_pEdgeLT->SetVisibleSingle( TRUE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		m_pLClick->SetVisibleSingle( FALSE );
		m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pEdgeLT->IsVisible() )
			{
				m_pEdgeLT->SetVisibleSingle( FALSE );
			}
			else
			{
				m_pEdgeLT->SetVisibleSingle( TRUE );
			}
		}
	}
	else if ( ( m_pGaeaClient->GetTutorial()->IsAttackStep()
		&& m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
		|| ( m_pGaeaClient->GetTutorial()->IsAttackStep()
		&&  m_pGaeaClient->GetTutorial()->IsSubFourStep() ) )
	{
		const UIRECT& rcTarget = m_pInterface->GetTargetInfoDisplay()->GetMouseOverGlobalPos();
		const UIRECT& rcWindow = GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = rcTarget.left + rcTarget.sizeX * 0.5f - rcWindow.sizeX * 0.5f;
		vPos.y = rcTarget.top - rcWindow.sizeY;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );

		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( FALSE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		//m_pLClick->SetVisibleSingle( FALSE );
		m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pLClick->IsVisible() )
			{
				m_pLClick->SetVisibleSingle( FALSE );
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLClick->SetVisibleSingle( TRUE );
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
	else if ( ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
		&&  m_pGaeaClient->GetTutorial()->IsSubFourStep() ) )
	{
		const UIRECT& rcTarget = m_pInterface->GetTargetInfoDisplay()->GetMouseOverGlobalPos();
		const UIRECT& rcWindow = GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = rcTarget.left + rcTarget.sizeX * 0.5f - rcWindow.sizeX * 0.5f;
		vPos.y = rcTarget.top - rcWindow.sizeY;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );

		D3DXVECTOR2 vTextPos;
		vTextPos.x = vPos.x - rcWindow.sizeX;
		vTextPos.y = vPos.y;

		m_pNormalText->SetLocalPos( vTextPos );
		m_pNormalText->SetGlobalPos( vTextPos );
		m_pNormalText->SetVisibleSingle( TRUE );

		vTextPos.x = vPos.x + rcWindow.sizeX;
		vTextPos.y = vPos.y;

		m_pSkillText->SetLocalPos( vTextPos );
		m_pSkillText->SetGlobalPos( vTextPos );
		m_pSkillText->SetVisibleSingle( TRUE );

		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( FALSE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		//m_pLClick->SetVisibleSingle( FALSE );
		//m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.5f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pLClick->IsVisible() )
			{
				m_pLClick->SetVisibleSingle( FALSE );
				m_pRClick->SetVisibleSingle( TRUE );
				//m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLClick->SetVisibleSingle( TRUE );
				m_pRClick->SetVisibleSingle( FALSE );
				//m_pNull->SetVisibleSingle( FALSE );
			}

			if ( !m_pLClick->IsVisible() && !m_pRClick->IsVisible() )
			{
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
	else if ( ( m_pGaeaClient->GetTutorial()->IsAttackStep()
		&&  m_pGaeaClient->GetTutorial()->IsSubSixStep() )
		|| ( m_pGaeaClient->GetTutorial()->IsAttackStep()
		&& m_pGaeaClient->GetTutorial()->IsSubEightStep() ) )
	{
		const UIRECT& rcTarget = m_pInterface->GetTargetInfoDisplay()->GetMouseOverGlobalPos();
		const UIRECT& rcWindow = GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = rcTarget.left + rcTarget.sizeX * 0.5f - rcWindow.sizeX * 0.5f;
		vPos.y = rcTarget.top - rcWindow.sizeY;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );

		m_pMain->SetVisibleSingle( TRUE );

		m_pEdgeLT->SetVisibleSingle( FALSE );
		m_pEdgeRT->SetVisibleSingle( FALSE );
		m_pEdgeLB->SetVisibleSingle( FALSE );
		m_pEdgeRB->SetVisibleSingle( FALSE );

		m_pLClick->SetVisibleSingle( FALSE );
		//m_pRClick->SetVisibleSingle( FALSE );
		m_pMClick->SetVisibleSingle( FALSE );
		m_pScroll->SetVisibleSingle( FALSE );
		//m_pNull->SetVisibleSingle( FALSE );

		if ( m_fFlowTime >= 0.8f )
		{
			m_fFlowTime = 0.0f;

			if ( m_pRClick->IsVisible() )
			{
				m_pRClick->SetVisibleSingle( FALSE );
				m_pNull->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pRClick->SetVisibleSingle( TRUE );
				m_pNull->SetVisibleSingle( FALSE );
			}
		}
	}
}

HRESULT CBalloonText::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubEightStep() )
	{
		const UIRECT& rcWindow = GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = X_RES * 0.4f - rcWindow.sizeX * 0.5f;
		vPos.y = Y_RES * 0.45f - rcWindow.sizeY * 0.5f;
		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}
	else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubNineStep() )
	{
		const UIRECT& rcWindow = GetGlobalPos();
		const UIRECT& rcTargetWindow = m_pInterface->GetMiniMap()->GetUIWindowGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = rcTargetWindow.left + rcTargetWindow.sizeX * 0.5f - rcWindow.sizeX * 0.5f;
		vPos.y = rcTargetWindow.top + rcTargetWindow.sizeY * 0.5f - rcWindow.sizeY * 0.5f;
		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}
	else if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubFifteenStep() )
	{
		const UIRECT& rcWindow = GetGlobalPos();
		const UIRECT& rcTargetWindow = m_pInterface->MinimapTargetGetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = rcTargetWindow.right;
		vPos.y = rcTargetWindow.top - rcWindow.sizeY;
		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}
	else if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep()
		&& m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
	{
		UIRECT rcTempInven;
		if( m_pInterface->GetInventory() )
			rcTempInven = m_pInterface->GetInventory()->GetInventoryPageGlobalPos();
		else
			rcTempInven = m_pInterface->GetInventoryWindow()->GetInventoryPageGlobalPos();

		const UIRECT& rcWindow = GetGlobalPos();
		D3DXVECTOR2 vPos( rcWindow.left, rcWindow.top );
		vPos.x = vPos.x + ( rcTempInven.left - rcWindow.left );
		vPos.y = vPos.y + ( rcTempInven.top - rcWindow.top );

		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}

	/*{
		const UIRECT& rcMouseTipWindow = GetGlobalPos ();
	}*/

	return S_OK;
}

void CBalloonText::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CBalloonText::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	m_pMain->SetVisibleSingle( bVisible );

	m_pEdgeLT->SetVisibleSingle( bVisible );
	m_pEdgeRT->SetVisibleSingle( bVisible );
	m_pEdgeLB->SetVisibleSingle( bVisible );
	m_pEdgeRB->SetVisibleSingle( bVisible );

	m_pLClick->SetVisibleSingle( bVisible );
	m_pRClick->SetVisibleSingle( bVisible );
	m_pMClick->SetVisibleSingle( bVisible );
	m_pScroll->SetVisibleSingle( bVisible );
	m_pNull->SetVisibleSingle( bVisible );
}

CBalloonSpaceBar::CBalloonSpaceBar(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pMain(NULL)
    , m_pText(NULL)
    , m_fFlowTime(0.0f)
{
}

CBalloonSpaceBar::~CBalloonSpaceBar(void)
{
}

void CBalloonSpaceBar::CreateSubControl()
{
	{
		CUIControl* pMain = new CUIControl(m_pEngineDevice);
		pMain->CreateSub( this, "TUTORIAL_BALLOON_SPACE_BAR" );
		RegisterControl( pMain );
		m_pMain = pMain;
		m_pMain->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 13, _DEFAULT_FONT_SHADOW_FLAG );

		CBasicTextBox* pExplainText = new CBasicTextBox(m_pEngineDevice);
		pExplainText->CreateSub( this, "TUTORIAL_BALLOON_SPACE_BAR_TEXT" );
		pExplainText->SetFont( pFont );
		pExplainText->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pExplainText );
		m_pText = pExplainText;
		m_pText->SetVisibleSingle( TRUE );

		if ( m_pText )
		{
			CString strCombine;
			strCombine = ID2GAMEINTEXT( "TUTORIAL_SPACE_DOWN" );

			m_pText->SetOneLineText( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}
}

void CBalloonSpaceBar::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fFlowTime += fElapsedTime;

	if( ( m_pGaeaClient->GetTutorial()->IsQuestNHuntStep()
		&& m_pGaeaClient->GetTutorial()->GetCurrentSubStep() >= m_pGaeaClient->GetTutorial()->FOUR_ ) )
	{
		m_pGaeaClient->GetTutorial()->m_bDrop = false;

		bool bFind(false);

		float fMinDist = FLT_MAX;
		D3DXVECTOR3 vMinPos;

		float fMinDistNotCan = FLT_MAX;
		D3DXVECTOR3 vMinPosNotCan; // 주울 수 있는 아이템이 없을 경우, 그냥 제일 가까운거 띄운다.

		D3DXVECTOR3 vCharPos = m_pGaeaClient->GetCharacter()->GetPosition();

		// Item
        m_pGaeaClient->GetLandManClient()->GetNearDropItem(
            fMinDist,
            vMinPos,
            fMinDistNotCan,
            vMinPosNotCan,
            bFind,
            vCharPos);

		// Money
        m_pGaeaClient->GetLandManClient()->GetNearDropMoney(
            fMinDist,
            vMinPos,
            fMinDistNotCan,
            vMinPosNotCan,
            bFind,
            vCharPos);

		if (bFind)
		{
			m_pGaeaClient->GetTutorial()->m_bDrop = true;
			m_pGaeaClient->GetTutorial()->m_vDropPos = vMinPos;

			D3DXVECTOR3 vPos = vMinPos;
			vPos.y += 6.0f;
			D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );	

			const UIRECT& rcOriginPos = GetGlobalPos();
			int nPosX = (int)( floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ) ) );
			int nPosY = (int)( vScreen.y - rcOriginPos.sizeY );

			SetGlobalPos ( D3DXVECTOR2( float( nPosX ), float( nPosY ) ) );

			//m_pMain->SetVisibleSingle( TRUE );

			if ( m_fFlowTime >= 0.8f )
			{
				m_fFlowTime = 0.0f;

				if ( m_pMain->IsVisible() )
				{
					m_pMain->SetVisibleSingle( FALSE );
				}
				else
				{
					m_pMain->SetVisibleSingle( TRUE );
				}
			}
		}
		else
		{	// 주울 수 있는건 없을땐, 그냥 제일 가까운거.
			m_pGaeaClient->GetTutorial()->m_bDrop = true;
			m_pGaeaClient->GetTutorial()->m_vDropPos = vMinPosNotCan;

			D3DXVECTOR3 vPos = vMinPosNotCan;
			vPos.y += 6.0f;
			D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );	

			const UIRECT& rcOriginPos = GetGlobalPos();
			int nPosX = (int)( floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ) ) );
			int nPosY = (int)( vScreen.y - rcOriginPos.sizeY );

			SetGlobalPos( D3DXVECTOR2 ( float( nPosX ), float( nPosY ) ) );

			//m_pMain->SetVisibleSingle( TRUE );

			if ( m_fFlowTime >= 0.8f )
			{
				m_fFlowTime = 0.0f;

				if ( m_pMain->IsVisible() )
				{
					m_pMain->SetVisibleSingle( FALSE );
				}
				else
				{
					m_pMain->SetVisibleSingle( TRUE );
				}
			}
		}
	}
}

HRESULT CBalloonSpaceBar::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

void CBalloonSpaceBar::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CBalloonSpaceBar::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	m_pMain->SetVisibleSingle( bVisible );
}

MyBalloonText::MyBalloonText( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice )
    : CBalloonText( pGaeaClient, pInerface, pEngineDevice )
{
}

void MyBalloonText::CreateUIWindowAndRegisterOwnership()
{
    CBalloonText::Create( BALLOON_TEXT_WINDOW, "TUTORIAL_BALLOON_TEXT_MOUSE_MAIN" );
    CBalloonText::CreateSubControl();
    CBalloonText::m_pInterface->UiRegisterControl( this, true );
    CBalloonText::m_pInterface->UiShowGroupTop( BALLOON_TEXT_WINDOW );
    CBalloonText::m_pInterface->UiHideGroup( BALLOON_TEXT_WINDOW );
}

void MyBalloonText::SetUIWindowGlobalPos( const UIRECT& rcPos )
{
    CBalloonText::SetGlobalPos( rcPos );
}

void MyBalloonText::SetUIWindowLocalPos( const UIRECT& rcPos )
{
    CBalloonText::SetLocalPos( rcPos );
}

MyBalloonSpaceBar::MyBalloonSpaceBar( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CBalloonSpaceBar( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyBalloonSpaceBar::CreateUIWindowAndRegisterOwnership()
{
    CBalloonSpaceBar::Create( BALLOON_SPACE_BAR, "TUTORIAL_BALLOON_SPACE_BAR" );
    CBalloonSpaceBar::CreateSubControl();
    CBalloonSpaceBar::m_pInterface->UiRegisterControl( this, true );
    CBalloonSpaceBar::m_pInterface->UiShowGroupTop( BALLOON_SPACE_BAR );
    CBalloonSpaceBar::m_pInterface->UiHideGroup( BALLOON_SPACE_BAR );
}
