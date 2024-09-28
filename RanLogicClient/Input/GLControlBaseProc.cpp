#include "../pch.h"

#include "../../EngineLib/DeviceMan.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/Common/DXInputString.h"
#include "../../EngineLib/Common/CommonWeb.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanUI/OldUI/Interface/SkillTrayTab.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Tutorial/GLBaseTutorial.h"

#include "./GLInputManager_KeySetting.h"
#include "./GLControlBase.h"

#include "../Widget/GLWidgetScript.h"
#include "../Macro/MacroManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! 키 예약은 이미 키가 사용 했는지 안했는지 체크하는 것을 의미
void GLControlBase::Proc_QuickSlot( const SKEYCAPS& cKeyCaps, const UINT nIndex )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->ReqActionQ(static_cast<WORD>(nIndex));

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_IQS_UpdateTraySlot, "-n", nIndex );
}

void GLControlBase::Proc_SkillSlot( const SKEYCAPS& cKeyCaps, const UINT nIndex )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	WORD wTab = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_CurrentTab );
	WORD wSlotCount = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_TabSlotCount );

	WORD wSkillIndex = wTab * wSlotCount + static_cast< WORD >( nIndex );

	if ( !pCharacter->IsReqSwapArms() && (S_OK == pCharacter->ReqSkillRunSet( wSkillIndex )) )
	{	// 스킬을 변경시에 서브 무기에 해당 스킬을 사용할 수 있는 무기가 
		// 장착중이라면 변경한다. 단, 이미 스왑 기능 예약시에는 하지 않는다.
		pCharacter->AutoArmSwap();
	}
}

void GLControlBase::Proc_SkillTab( const SKEYCAPS& cKeyCaps, const UINT nIndex )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_SQSGroup, "-n", nIndex );
}

void GLControlBase::Proc_SkillTrayToggle( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_SQSTray );
}

void GLControlBase::Proc_ArmsSlotSwap( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	EMCHARCLASS emClass = pCharacter->m_emClass;
	if( emClass&GLCC_EXTREME_M || emClass&GLCC_EXTREME_W )
	{
		bool bMain = (0!=pCharacter->IsUseArmSub());

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_SQS_ArmSlotSwap,
			"-b", bMain );

		m_pGaeaClient->GetCharacter()->ReqSlotChange();
	}
}

void GLControlBase::Proc_UseChatMacro(const SKEYCAPS& cKeyCaps, const UINT nIndex)
{
	// 키 예약
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	if( CHAT_MACRO_NUM <= nIndex )
		return;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_AddChatMacro,
		"-s", RANPARAM::ChatMacro[ nIndex ].GetBuffer() );
}

void GLControlBase::Proc_Inventory( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Inventory );
}

void GLControlBase::Proc_Character( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Character );
}

void GLControlBase::Proc_Skill( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Skill );
}

void GLControlBase::Proc_Party( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Party );
}

void GLControlBase::Proc_Quest( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_Quest );
}

void GLControlBase::Proc_Club( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Club);	
}

void GLControlBase::Proc_Friend( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Community);	
}

void GLControlBase::Proc_ItemBank( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Bank );
}

void GLControlBase::Proc_MiniMap( const SKEYCAPS& cKeyCaps )
{
	// 키 예약
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_LargeMap );
}

void GLControlBase::Proc_PointShop( const SKEYCAPS& cKeyCaps )
{
	// 키 예약
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) || GLUseFeatures::GetInstance().IsUsingPointShop() == false )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	EMSERVICE_PROVIDER sp =	m_pGaeaClient->GetServiceProvider();

	switch ( sp )
	{
	case SP_CHINA:
	case SP_THAILAND:
		{
			//!! SF_TODO
			//m_pInterface->ToggleWindowWebItemShop();
		}
		break;

	default:
		{		
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_HotKey_PointShop );
		}
		break;
	};
}

void GLControlBase::Proc_ChatMacro( const SKEYCAPS& cKeyCaps )
{
	// 키 예약
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_HotKey_ChatMacro );
}

void GLControlBase::Proc_SNS( const SKEYCAPS& cKeyCaps )
{
	// SNS 사용안함.

	/*
	// 키 예약
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//m_pInterface->ToggleWindowSNS();
	*/
}

void GLControlBase::Proc_Help( const SKEYCAPS& cKeyCaps )
{
	// 키 예약
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_Help );
}

void GLControlBase::Proc_QBox( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter || (pCharacter->IsPartyMem() && FALSE == pCharacter->IsPartyMaster()) )
		return;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_QBox );
}

void GLControlBase::Proc_Pet( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Pet );
}

void GLControlBase::Proc_Vehicle( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_Vehicle );
}

void GLControlBase::Proc_Product( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) || GLUseFeatures::GetInstance().IsUsingRenewProduct() == false )
	{
		return;
	}

	GASSERT(m_pGaeaClient);
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Product );
}

void GLControlBase::Proc_SchoolRecord( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) || (GLUseFeatures::GetInstance().IsUsingActivity() == false && GLUseFeatures::GetInstance().IsUsingAttendance() == false) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_StudentRecord );
}

void GLControlBase::Proc_BattleGround( const SKEYCAPS& cKeyCaps )
{
	// 키 예약

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Competition );
}

void GLControlBase::Proc_PickupItem( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->ReqPickupNearItem();
}

void GLControlBase::Proc_ForcedPKMode( const SKEYCAPS& cKeyCaps )
{
	GASSERT(m_pGaeaClient);

	GLCharacter*	pCharacter = m_pGaeaClient->GetCharacter();
	const bool bDominateControl = m_pKeySetting->GetKeyState( EMSHORTCUTS_DOMINATE_CONTROL ) & DXKEY_DOWNED;	
	const bool bForcePKMode = (cKeyCaps.dwKeyState & DXKEY_DOWNED ? true : false);	
	if( (bForcePKMode == true) && (bDominateControl == false) )
	{
		pCharacter->m_bForcedPKMode = true;
		STARGETID&		selectId = pCharacter->GetSelectID();
		if( selectId.ISNULL() )
			return;

		pCharacter->SelectForcedTarget();			 //강제타겟팅 설정이 된 경우만 해당 아님 변경없음.
	} //if
	else
		pCharacter->m_bForcedPKMode = false;
}

void GLControlBase::NonEventProc_ForcedPKMode( const SKEYCAPS& cKeyCaps )
{
	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->m_bForcedPKMode = false;
}

void GLControlBase::Proc_RunMode( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_UP) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->ReqToggleRun ();
}

void GLControlBase::Proc_PeaceMode( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_UP) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter->IsProducting() )
		pCharacter->ReqTogglePeaceMode ();
}

void GLControlBase::Proc_VehicleActive( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	if (m_pGaeaClient->GetTradeClient()->Valid())
		return;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if (pCharacter->VehicleActiveValue() && cKeyCaps.fElapsedTime < 1.0f)
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_DELAY"));
		return;
	}

	pCharacter->ReqSetVehicle(!pCharacter->VehicleState());
}

void GLControlBase::Proc_VehicleBoost( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter->isAction(GLAT_MOVE) )
	{
		pCharacter->UseBikeBoost();
	}
	if( !pCharacter->isAction(GLAT_MOVE) && pCharacter->VehicleState() == EMVEHICLE_PASSENGER )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_USING_ACTION_MOVE") );
	}
	else if( !pCharacter->isAction(GLAT_MOVE) &&  pCharacter->VehicleState() == EMVEHICLE_DRIVER )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_USING_ACTION_MOVE") );
	}
}

void GLControlBase::Proc_ActionCancel( const SKEYCAPS& cKeyCaps )
{	
	// 키 예약
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( NULL == pCharacter )
		return;

	// 메시지 박스 끄기
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_CancelMsgBox ) )
		return;

	// 현재 열려있는 인터페이스 창 닫기
	if( true == GfxInterfaceBase::GetInstance()->EscWidgetClose() )
		return;

	// ESC 메뉴 열기
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_System );
}

void GLControlBase::Proc_FlyCameraControl( const SKEYCAPS& cKeyCaps )
{
	// 키 예약;
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
		return;

	GASSERT(m_pGaeaClient);
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_FlyCamCtrl );
}

void GLControlBase::Proc_CameraMove(const SKEYCAPS& cKeyCaps, const UINT nMoveDir)
{
	DoProc_CameraMove(cKeyCaps, nMoveDir);
}

void GLControlBase::Proc_CameraRotate(const SKEYCAPS& cKeyCaps, const UINT nMoveDir)
{
	DoProc_CameraRotate(cKeyCaps, nMoveDir);
}

void GLControlBase::Proc_CameraMoveSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraMoveSpeed_Variation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_CameraRotateSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraRotateSpeed_Variation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_SelectTarget(const SKEYCAPS& cKeyCaps)
{
	DoProc_SelectTarget(cKeyCaps);
}

void GLControlBase::Proc_CameraInterpMoveTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraInterpMoveTVariation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_CameraInterpRotateTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraInterpRotateTVariation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_CameraInterpClear(const SKEYCAPS& cKeyCaps)
{
	DoProc_CameraInterpClear(cKeyCaps);
}

void GLControlBase::Proc_CameraInterpTargetSelectTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraInterpTargetSelectTVariation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_CameraPlay(const SKEYCAPS& cKeyCaps)
{
	DoProc_CameraPlay(cKeyCaps);
}

void GLControlBase::Proc_CameraStop(const SKEYCAPS& cKeyCaps)
{
	DoProc_CameraStop(cKeyCaps);
}

void GLControlBase::Proc_CameraRewind(const SKEYCAPS& cKeyCaps)
{
	DoProc_CameraRewind(cKeyCaps);
}

void GLControlBase::Proc_Confirm( const SKEYCAPS& cKeyCaps )
{
	if( !( cKeyCaps.dwKeyState & DXKEY_UP ) )
		return;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Confirm );
}

void GLControlBase::Proc_InteractDropItem( const SKEYCAPS& cKeyCaps )
{
	if( cKeyCaps.dwKeyState & DXKEY_DOWN )
		GfxInterfaceBase::GetInstance()->SetInteractDropItem( true );
	else if( cKeyCaps.dwKeyState & DXKEY_UP )
		GfxInterfaceBase::GetInstance()->SetInteractDropItem( false );
}