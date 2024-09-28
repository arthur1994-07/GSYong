#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/DeviceMan.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/Common/DXInputString.h"
#include "../../EngineLib/Common/CommonWeb.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/Cursor.h"
#include "../../EngineLib/GUInterface/NSGUI.h"

#include "../../RanLogic/GLogicDataMan.h"

#include "../../RanUI/OldUI/Interface/SkillTrayTab.h"

#include "../GLGaeaClient.h"
#include "../Macro/MacroManager.h"
#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"
#include "../Tutorial/GLBaseTutorial.h"
#include "../StaticHoldItem.h"

#include "./GLInputManager_KeySetting.h"
#include "./GLControlBase.h"

#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#include "../GfxInterfaceBase.h"
#include "../Widget/GLWidgetScript.h"

struct SORTINPUTCAPS
{
	SORTINPUTCAPS( CKeySetting* pKeySetting ) 
		: m_pKeySetting(pKeySetting)
	{
	}

	const bool operator () ( const SKEYCAPS* lhs, const SKEYCAPS* rhs ) const
	{
		const SSHORTCUTS& sclhs = m_pKeySetting->GetShortcuts(lhs->emShortcuts);
		const SSHORTCUTS& scrhs = m_pKeySetting->GetShortcuts(rhs->emShortcuts);

		return (sclhs.GetMinimumKeyNum() > scrhs.GetMinimumKeyNum());
	}

	CKeySetting* m_pKeySetting;
};

// ----------------------------------------------------------------------------

GLControlBase::GLControlBase( GLGaeaClient* pGaeaClient )
: m_pGaeaClient(pGaeaClient)
, m_pKeySetting(NULL)
, m_bIsDragEndFromUI( false )
, m_fGfxUICheckTime( 0.0f )
, m_fMaxGfxUICheckTime( 0.2f )
, m_bFixedCusor( false )
{
	m_ptHoldCursor.x = 0;
	m_ptHoldCursor.y = 0;
}

GLControlBase::~GLControlBase()
{
	//Blank
}

void GLControlBase::Init( CKeySetting& cKeySetting )
{
	m_pKeySetting = &cKeySetting;

	m_mInputCaps.clear();
	m_SortedInputCaps.clear();

	//! 프로시져 세팅
	{

		m_mInputCaps[EMSHORTCUTS_QUICKSLOT0] = boost::bind(&GLControlBase::Proc_QuickSlot, _1, _2, 0);
		m_mInputCaps[EMSHORTCUTS_QUICKSLOT1] = boost::bind(&GLControlBase::Proc_QuickSlot, _1, _2, 1);
		m_mInputCaps[EMSHORTCUTS_QUICKSLOT2] = boost::bind(&GLControlBase::Proc_QuickSlot, _1, _2, 2);
		m_mInputCaps[EMSHORTCUTS_QUICKSLOT3] = boost::bind(&GLControlBase::Proc_QuickSlot, _1, _2, 3);
		m_mInputCaps[EMSHORTCUTS_QUICKSLOT4] = boost::bind(&GLControlBase::Proc_QuickSlot, _1, _2, 4);
		m_mInputCaps[EMSHORTCUTS_QUICKSLOT5] = boost::bind(&GLControlBase::Proc_QuickSlot, _1, _2, 5);

		m_mInputCaps[EMSHORTCUTS_SKILLSLOT0] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 0);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT1] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 1);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT2] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 2);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT3] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 3);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT4] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 4);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT5] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 5);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT6] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 6);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT7] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 7);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT8] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 8);
		m_mInputCaps[EMSHORTCUTS_SKILLSLOT9] = boost::bind(&GLControlBase::Proc_SkillSlot, _1, _2, 9);

		m_mInputCaps[EMSHORTCUTS_SKILLTAB0] = boost::bind(&GLControlBase::Proc_SkillTab, _1, _2, 0);
		m_mInputCaps[EMSHORTCUTS_SKILLTAB1] = boost::bind(&GLControlBase::Proc_SkillTab, _1, _2, 1);
		m_mInputCaps[EMSHORTCUTS_SKILLTAB2] = boost::bind(&GLControlBase::Proc_SkillTab, _1, _2, 2);
		m_mInputCaps[EMSHORTCUTS_SKILLTAB3] = boost::bind(&GLControlBase::Proc_SkillTab, _1, _2, 3);

		m_mInputCaps[EMSHORTCUTS_SKILLTRAYTOGGLE] = &GLControlBase::Proc_SkillTrayToggle;

		m_mInputCaps[EMSHORTCUTS_ARMSSLOTSWAP] = &GLControlBase::Proc_ArmsSlotSwap;

		m_mInputCaps[EMSHORTCUTS_CHATMACRO0] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 0);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO1] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 1);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO2] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 2);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO3] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 3);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO4] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 4);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO5] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 5);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO6] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 6);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO7] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 7);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO8] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 8);
		m_mInputCaps[EMSHORTCUTS_CHATMACRO9] = boost::bind(&GLControlBase::Proc_UseChatMacro, _1, _2, 9);

		m_mInputCaps[EMSHORTCUTS_INVENTORY] = &GLControlBase::Proc_Inventory;
		m_mInputCaps[EMSHORTCUTS_CHARACTER] = &GLControlBase::Proc_Character;
		m_mInputCaps[EMSHORTCUTS_SKILL]		= &GLControlBase::Proc_Skill;
		m_mInputCaps[EMSHORTCUTS_PARTY]		= &GLControlBase::Proc_Party;
		m_mInputCaps[EMSHORTCUTS_QUEST]		= &GLControlBase::Proc_Quest;
		m_mInputCaps[EMSHORTCUTS_CLUB]		= &GLControlBase::Proc_Club;
		m_mInputCaps[EMSHORTCUTS_FRIEND]	= &GLControlBase::Proc_Friend;
		m_mInputCaps[EMSHORTCUTS_ITEMBANK]	= &GLControlBase::Proc_ItemBank;
		m_mInputCaps[EMSHORTCUTS_MINIMAP]	= &GLControlBase::Proc_MiniMap;
		m_mInputCaps[EMSHORTCUTS_POINTSHOP] = &GLControlBase::Proc_PointShop;
		m_mInputCaps[EMSHORTCUTS_CHATMACRO] = &GLControlBase::Proc_ChatMacro;
		//m_mInputCaps[EMSHORTCUTS_SNS]		= &GLControlBase::Proc_SNS;	 SNS 사용안함.
		m_mInputCaps[EMSHORTCUTS_HELP]		= &GLControlBase::Proc_Help;
		m_mInputCaps[EMSHORTCUTS_QBOX]		= &GLControlBase::Proc_QBox;
		m_mInputCaps[EMSHORTCUTS_PET]		= &GLControlBase::Proc_Pet;
		m_mInputCaps[EMSHORTCUTS_VEHICLE]	= &GLControlBase::Proc_Vehicle;
		m_mInputCaps[EMSHORTCUTS_PRODUCT]	= &GLControlBase::Proc_Product;

		m_mInputCaps[EMSHORTCUTS_SCHOOLRECORD] = &GLControlBase::Proc_SchoolRecord;
		m_mInputCaps[EMSHORTCUTS_BATTLEGROUND] = &GLControlBase::Proc_BattleGround;

		m_mInputCaps[EMSHORTCUTS_PICKUPITEM]	= &GLControlBase::Proc_PickupItem;
		m_mInputCaps[EMSHORTCUTS_RUNMODE]		= &GLControlBase::Proc_RunMode;
		m_mInputCaps[EMSHORTCUTS_PEACEMODE]		= &GLControlBase::Proc_PeaceMode;
		m_mInputCaps[EMSHORTCUTS_VEHICLEACTIVE] = &GLControlBase::Proc_VehicleActive;
		m_mInputCaps[EMSHORTCUTS_VEHICLEBOOST]  = &GLControlBase::Proc_VehicleBoost;

		m_mInputCaps[EMSHORTCUTS_FOURCEDPKMODE]	= SKEYCAPS(&GLControlBase::Proc_ForcedPKMode, &GLControlBase::NonEventProc_ForcedPKMode);

		m_mInputCaps[EMSHORTCUTS_ACTIONCANCEL]	= &GLControlBase::Proc_ActionCancel;

		m_mInputCaps[ EMSHORTCUTS_CONFIRM ] = &GLControlBase::Proc_Confirm;
		m_mInputCaps[ EMSHORTCUTS_INTERACT_DROPITEM ] = &GLControlBase::Proc_InteractDropItem;

		// 현재 캐릭터 슬롯 변경은 Scaleform Lua 에 구현되어 있기 때문에 아무것도 하지 않는다;
		// 이렇게 등록해둬야지 중복키를 막을 수 있다;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT0 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT1 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT2 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT3 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT4 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT5 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT6 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT7 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT8 ] = &GLControlBase::Proc_None;
		m_mInputCaps[ EMSHORTCUTS_CHANGE_CHARSLOT9 ] = &GLControlBase::Proc_None;
	}

	//! 타입별 세팅
	{
		DoInit();
	}

	//! 우선순위 세팅
	//	키중복을 막기위하여 사용된다. (ex. 1번 스킬사용키와 alt+1 매크로 사용키에서 1키가 중복)
	//  이 경우 사용 키갯수가 많은쪽을 앞에 둔다 ( 즉 alt+1을 먼저 체크 )
	{
		MAP_INPUTCAPS_ITER iter		= m_mInputCaps.begin();
		MAP_INPUTCAPS_ITER iterEnd  = m_mInputCaps.end();

		for ( ; iter!=iterEnd; ++iter )
		{
			EMSHORTCUTS emShortcuts = iter->first;
			SKEYCAPS&	sKeyCaps	= iter->second;

			sKeyCaps.emShortcuts = emShortcuts;
			m_SortedInputCaps.push_back( &sKeyCaps );

			if ( iter != m_mInputCaps.begin() )
			{
				LIST_PINPUTCAPS_ITER preEnd = m_SortedInputCaps.end();
				std::inplace_merge( m_SortedInputCaps.begin(), --preEnd, m_SortedInputCaps.end(), SORTINPUTCAPS(m_pKeySetting) );
			}
		}
	}
}

void GLControlBase::FrameMove( float fTime, float fElapsedTime )
{
	GASSERT(m_pGaeaClient);
	GASSERT(m_pKeySetting);

	Input( fTime, fElapsedTime );
	Update( fTime, fElapsedTime );
}

void GLControlBase::Input( float fTime, float fElapsedTime )
{
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	//if( pCharacter && RF_PROVOKE( pCharacter->m_EffSkillVarSet ).IsOn() ) return;

// 	if ( m_pGaeaClient->GetUseChageCam() == false )
// 	{
// 		D3DXVECTOR3 vCameraPos = pCharacter->GetPosition();
// 		vCameraPos.y += 10.0f;
// 		DxViewPort::GetInstance().CameraJump(vCameraPos);
// 	}
// 	else
// 	{
// 		// const DWORD dwChaseTarget = m_pGaeaClient->GetChaseCameraTarget();
// 		const STARGETID sChaseTarget = m_pGaeaClient->GetChaseCameraTarget();
// 		const DWORD dwChaseTarget = sChaseTarget.GaeaId;
// 		D3DXVECTOR3 vCameraPos(0.f, 0.f, 0.f);
// 		if ( pCharacter && pCharacter->GetGaeaID() == dwChaseTarget )
// 		{
// 			vCameraPos = pCharacter->GetPosition();
// 			vCameraPos.y += 10.0f;
// 			DxViewPort::GetInstance().CameraJump(vCameraPos);
// 		}
// 		else
// 		{
// 			PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
// 			if ( pLand && dwChaseTarget != EMTARGET_NULL && sChaseTarget.emCrow != CROW_NULL )
// 			{
// 				switch (sChaseTarget.emCrow)
// 				{
// 				case CROW_PC :		
// 					{
// 						std::tr1::shared_ptr<GLCharClient> spChar = pLand->GetChar(dwChaseTarget);
// 						if ( spChar )
// 							vCameraPos = spChar->GetPosition(); 
// 					}
// 					break;
// 				case CROW_NPC :		
// 				case CROW_MOB :		
// 					{
// 						std::tr1::shared_ptr<GLCrowClient> spCrow = pLand->GetCrow(dwChaseTarget);
// 						if ( spCrow )
// 							vCameraPos = spCrow->GetPosition(); 
// 					}
// 					break;
// 				case CROW_ITEM :	
// 					{
// 						std::tr1::shared_ptr<CItemClientDrop> spItemDrop = pLand->GetItem(dwChaseTarget);
// 						if ( spItemDrop )
// 							vCameraPos = spItemDrop->vPos; 
// 					}
// 					break;
// 				case CROW_MONEY :	
// 					{
// 						std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = pLand->GetMoney(dwChaseTarget);
// 						if ( spMoneyDrop )
// 							vCameraPos = spMoneyDrop->vPos; 
// 					}
// 					break;
// 				case CROW_PET :		
// 					{
// 						std::tr1::shared_ptr<GLAnyPet> spPet = pLand->GetPet(dwChaseTarget);
// 						if ( spPet )
// 							vCameraPos = spPet->GetPosition();
// 					}
// 					break;
// 				case CROW_SUMMON :	
// 					{
// 						std::tr1::shared_ptr<GLSummonClient> spSummon = pLand->GetSummon(dwChaseTarget);
// 						if ( spSummon )
// 							vCameraPos = spSummon->GetPosition();
// 					}
// 					break;
// 				}
// 				vCameraPos.y += 10.f;
// 				DxViewPort::GetInstance().CameraJump(vCameraPos);
// 			}
// 		}
// 	}

	// 카메라 컨트롤 ( 카메라 컨트롤을 막는 상태는 없음 )
	DoInputCamera(fTime, fElapsedTime);

	//! 사용된 키 초기화
	m_mUsedKeys.clear();	

	if ( !m_pKeySetting )
	{
		GASSERT(!"GLControlBase::FrameMove() - m_pKeySetting is null");
		return;
	}

	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

	//! 단축키 초기화
	{
		LIST_PINPUTCAPS_ITER iter	 = m_SortedInputCaps.begin();
		LIST_PINPUTCAPS_ITER iterEnd = m_SortedInputCaps.end();

		for ( ; iter!=iterEnd; ++iter )
		{
			SKEYCAPS* pKeyCaps = (*iter);
			
			if ( !pKeyCaps )
			{
				continue;
			}

			pKeyCaps->fElapsedTime  += fElapsedTime;
			pKeyCaps->dwKeyState	=  0;
		}
	}

	//! 단축키 체크
	if ( !IsShortcutsBlock( fTime, fElapsedTime ) )
	{
		LIST_PINPUTCAPS_ITER iter	 = m_SortedInputCaps.begin();
		LIST_PINPUTCAPS_ITER iterEnd = m_SortedInputCaps.end();

		for ( ; iter!=iterEnd; ++iter )
		{
			SKEYCAPS* pKeyCaps = (*iter);

			if ( !pKeyCaps )
			{
				continue;
			}

			if ( pKeyCaps->fnProc.empty() )
			{
				continue;
			}

			EMSHORTCUTS emShortcuts = pKeyCaps->emShortcuts;
			const SSHORTCUTS& sKey	= m_pKeySetting->GetShortcuts( emShortcuts );

			if ( sKey.IsNull() )
			{
				return;
			}

			if( sKey.GetString() != "Esc" )
			{
				// 완전 블록 UI 체크
				if( !pCharacter->m_bSTATE_CONTROL
					|| NSGUI::IsCharMoveAllBlock() )
				{
					CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);
					DxViewPort::GetInstance().SetHandling ( false );
					continue;
				}
			}

			bool bNonEvent = true;

			//! 키 중복 체크
			if ( !CheckDuplicationShockcut(sKey) )
			{
				DWORD dwKeyState = sKey.GetKeyState();
				pKeyCaps->dwKeyState = dwKeyState;

				if ( (dwKeyState != 0) && (dwKeyState != DXKEY_IDLE) )
				{
					if( false == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
						NSWIDGET_SCRIPT::g_strFunc_HotKeyConfig_IsOpen ) )
                    {
					    pKeyCaps->fnProc( this, (*pKeyCaps) );
					    pKeyCaps->fElapsedTime = 0.0f;
					    bNonEvent = false;

					    RegisterUsedKey(sKey);
                    }
				}
			}

			if ( bNonEvent && !pKeyCaps->fnNonEventProc.empty() )
			{
				pKeyCaps->fnNonEventProc( this, (*pKeyCaps) );
			}
		}
	}
	//! 단축키 블록 상태 ( 없는 기능 )
	else
	{
		LIST_PINPUTCAPS_ITER iter	 = m_SortedInputCaps.begin();
		LIST_PINPUTCAPS_ITER iterEnd = m_SortedInputCaps.end();

		for ( ; iter!=iterEnd; ++iter )
		{
			SKEYCAPS* pKeyCaps = (*iter);

			if ( !pKeyCaps )
			{
				continue;
			}

			if ( pKeyCaps->fnNonEventProc.empty() )
			{
				continue;
			}

			pKeyCaps->fnNonEventProc( this, (*pKeyCaps)  );
		}
	}	
	
	//! 캐릭터 블록 체크
	if ( !pCharacter->m_bSTATE_CONTROL
		|| NSGUI::IsCharMoveAllBlock()
		|| IsCharacterBlock( fTime, fElapsedTime ) )
	{
		//DxCursor::GetInstance().SetCursorType(EMCS_NORMAL);
		//CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);
		CharBlockCursorUpdate();
		DxViewPort::GetInstance().SetHandling ( false );

		//!! SF_TODO
		//m_pInterface->SetTutorialDialogueEnable( false );
		return;
	}

    //! 채팅 중인지 체크
    if( true == GfxInterfaceBase::GetInstance()->GetFocusInTextInput() )
        return;

	//! 커서가 윈도우 안에 있을경우만
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
	{
		return;
	}

	if( true == GfxInterfaceBase::GetInstance()->IsMouseHitUI() || 
		true == GfxInterfaceBase::GetInstance()->IsDragging() )
		return;

	DWORD dwDropKeyState      = m_pKeySetting->GetKeyState( EMSHORTCUTS_ITEMDROP );
	DWORD dwDropUseKeyState   = m_pKeySetting->GetKeyState( EMSHORTCUTS_ITEMDROP_USE );
	DWORD dwDropKeyStateTotal = dwDropKeyState | dwDropUseKeyState;

	// RM# 2174 - gbgim;
	// 클라, 플래쉬가 각각따로 윈도우 메세지를 받아서 처리함;
	// 처리하는 시점에 따라 저장해야될 필요가 생김;
	if ( holditem::HaveHoldItem() && (dwDropKeyStateTotal&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP)) )
	{
		if ( dwDropKeyStateTotal&DXKEY_UP )
		{
			if ( holditem::IsReserveDelete() )
				holditem::ReleaseHoldItem();
		}
		return ;
	}

	DoInput( fTime, fElapsedTime );
}

void GLControlBase::Update( float fTime, float fElapsedTime )
{
	DoUpdate( fTime, fElapsedTime );
}

const bool GLControlBase::CheckDuplicationShockcut( const SSHORTCUTS& sShortcuts )
{
	//! 중복키 여부를 체크한다.
	SSHORTCUTS::VEC_KEY_CITER iter    = sShortcuts.IterBegin();
	SSHORTCUTS::VEC_KEY_CITER iterEnd = sShortcuts.IterEnd();

	for ( ; iter!=iterEnd; ++iter )
	{
		const SSHORTCUTS::KEY& sKey = (*iter);

		MAP_USEDKEYS_ITER finditer = m_mUsedKeys.find( sKey );
		if ( finditer != m_mUsedKeys.end() )
		{
			return true;
		}
	}

	return false;
}

void GLControlBase::RegisterUsedKey( const SSHORTCUTS& sShortcuts )
{
	//! 다중키 (ex. alt+1)일 경우만 등록을해서 중복적용을 막는다.
	if ( 1 < static_cast<UINT>(sShortcuts.GetMinimumKeyNum()) )
	{
		SSHORTCUTS::VEC_KEY_CITER iter    = sShortcuts.IterBegin();
		SSHORTCUTS::VEC_KEY_CITER iterEnd = sShortcuts.IterEnd();

		for ( ; iter!=iterEnd; ++iter )
		{
			const SSHORTCUTS::KEY& sKey = (*iter);
			if ( sKey.IsKeyType() )
			{
				m_mUsedKeys[sKey] = true;
			}
		}
	}
}

const bool GLControlBase::IsKeyboardBlock( float fTime, float fElapsedTime )
{
	EngineDeviceMan* pDevice = m_pGaeaClient->GetEngineDevice();

	DXInputString* pInputString = pDevice->GetInputString();
	if ( !pInputString || pInputString->IsOn() || COMMON_WEB::GetTopWindowID() >= 0 )
	{
		return true;
	}

	if ( GfxInterfaceBase::GetInstance()->GetFocusInTextInput() == true )
		return true;

	if ( DoIsKeyboardBlock( fTime, fElapsedTime ) )
	{
		return true;
	}

	return false;
}

const bool GLControlBase::IsMouseBlock( float fTime, float fElapsedTime )
{
	if ( DoIsMouseBlock( fTime, fElapsedTime ) )
	{
		return true;
	}

	return false;
}

const bool GLControlBase::IsShortcutsBlock( float fTime, float fElapsedTime )
{
	GASSERT(m_pGaeaClient);
	
	if( true == GfxInterfaceBase::GetInstance()->GetFocusInTextInput() )
		return true;

	if ( m_pGaeaClient->GetTutorial()->IsTutorial() && !m_pGaeaClient->GetTutorial()->IsCanMoveInTutorial() )
	{
		return true;
	}

	if ( DoIsShortcutsBlock( fTime, fElapsedTime ) )
	{
		return true;
	}

	return false;
}

const bool GLControlBase::IsCharacterBlock( float fTime, float fElapsedTime )
{
	//! 마켓, 창고, 거래창, 모달박스가 활성화시 케릭터 제어 잠김.
	//
	if ( NSGUI::IsCharMoveBlock() )
	{
		return true;
	}

	if ( m_pGaeaClient->GetTutorial()->IsTutorial() && !m_pGaeaClient->GetTutorial()->IsCanMoveInTutorial() )
	{
		return true;
	}

	if ( DoIsCharacterBlock( fTime, fElapsedTime ) )
	{
		return true;
	}

	return false;
}

void GLControlBase::DefaultCameraUpdate( float fTime, float fElapsedTime )
{
	GASSERT(m_pKeySetting);

	DxViewPort& cViewPort = DxViewPort::GetInstance();

	POINT ptCurPos;
	GetCursorPos ( &ptCurPos );
	BOOL bTARPOSWIN = cViewPort.GetMouseTargetPosWnd ( D3DXVECTOR3((float)ptCurPos.x,(float)ptCurPos.y,0) );

	BOOL bCURSOR_SHOW(TRUE);
	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

	DWORD dwKEY_DIRECTIONMODE(DXKEY_IDLE);
	DWORD dwKEY_ZOOMIN(DXKEY_IDLE), dwKEY_ZOOMOUT(DXKEY_IDLE);
	DWORD dwKEY_RIGHT(DXKEY_IDLE), dwKEY_LEFT(DXKEY_IDLE);
	DWORD dwKEY_UP(DXKEY_IDLE), dwKEY_DOWN(DXKEY_IDLE);

	int nKEY_ZOOM(0);
	int dx(0), dy(0), dz(0);

	bool bZoomBlock = GfxInterfaceBase::GetInstance()->IsMouseHitUI();

	if ( bTARPOSWIN )
	{
		m_pKeySetting->GetMouseMove ( dx, dy, dz );

		dwKEY_DIRECTIONMODE = m_pKeySetting->GetKeyState( EMSHORTCUTS_CAMERA_DIRECTIONMODE );

		// 방향키 카메라 회전.
		if ( !IsKeyboardBlock(fTime, fElapsedTime) )
		{
			dwKEY_RIGHT   = m_pKeySetting->GetKeyState( EMSHORTCUTS_CAMERA_RIGHT,	FLAG_GETKEYSTATE_ONLY_KEYBOARD );
			dwKEY_LEFT    = m_pKeySetting->GetKeyState( EMSHORTCUTS_CAMERA_LEFT,	FLAG_GETKEYSTATE_ONLY_KEYBOARD );
			dwKEY_UP	  = m_pKeySetting->GetKeyState( EMSHORTCUTS_CAMERA_UP,		FLAG_GETKEYSTATE_ONLY_KEYBOARD );
			dwKEY_DOWN	  = m_pKeySetting->GetKeyState( EMSHORTCUTS_CAMERA_DOWN,	FLAG_GETKEYSTATE_ONLY_KEYBOARD );

			if ( !bZoomBlock )
			{
				dwKEY_ZOOMIN  = m_pKeySetting->GetKeyState( EMSHORTCUTS_CAMERA_ZOOMIN,  FLAG_GETKEYSTATE_ONLY_KEYBOARD );
				dwKEY_ZOOMOUT = m_pKeySetting->GetKeyState( EMSHORTCUTS_CAMERA_ZOOMOUT, FLAG_GETKEYSTATE_ONLY_KEYBOARD );
			}
		}

		if ( !bZoomBlock )
		{
			nKEY_ZOOM = m_pKeySetting->GetKeyAmount( EMSHORTCUTS_CAMERA_ZOOM );
		}
	}

	float fZoom(0.0f);
	D3DXVECTOR3 vMove(0,0,0);
	D3DXVECTOR3 vRotate(0,0,0);

	if ( dwKEY_RIGHT & DXKEY_PRESSED )
	{
		vRotate.y += fElapsedTime*D3DX_PI/2;
	}
	if ( dwKEY_LEFT & DXKEY_PRESSED )
	{
		vRotate.y -= fElapsedTime*D3DX_PI/2;
	}

	if ( dwKEY_UP & DXKEY_PRESSED )
	{
		vRotate.x += fElapsedTime*D3DX_PI/2;
	}
	if ( dwKEY_DOWN & DXKEY_PRESSED )
	{
		vRotate.x -= fElapsedTime*D3DX_PI/2;
	}

	if ( dwKEY_ZOOMIN & DXKEY_PRESSED )	
	{
		fZoom -= fElapsedTime*cViewPort.GetVeloZoom();
	}
	if ( dwKEY_ZOOMOUT & DXKEY_PRESSED )
	{
		fZoom += fElapsedTime*cViewPort.GetVeloZoom();
	}


	if ( (dwKEY_RIGHT|dwKEY_LEFT|dwKEY_UP|dwKEY_DOWN)!=DXKEY_IDLE )
	{
		cViewPort.CalculateVeloDirDamper( vRotate, vRotate, fElapsedTime );
	}
	//#ifdef CH_PARAM
	//		else if ( dwMOUSEKEY_R & DXKEY_DRAG )
	//#else

	else if ( dwKEY_DIRECTIONMODE&DXKEY_DOWN )
	{
		m_ptHoldCursor = ptCurPos;
		SetCursorPos(m_ptHoldCursor.x, m_ptHoldCursor.y);
		//dxInputDev.HoldCursor();
	}
	else if ( (dwKEY_DIRECTIONMODE&DXKEY_DRAG) || (dwKEY_DIRECTIONMODE&DXKEY_PRESSED) )
		//#endif
	{
		if( false == m_bIsDragEndFromUI &&
			false == GfxInterfaceBase::GetInstance()->IsDragging() )
		{
			int nHoldToCurX = m_ptHoldCursor.x - ptCurPos.x;
			int nHoldToCurY = m_ptHoldCursor.y - ptCurPos.y;

			vRotate.x += cViewPort.GetVeloDir()*(-nHoldToCurY);
			vRotate.y += cViewPort.GetVeloDir()*(-nHoldToCurX);

			cViewPort.CalculateVeloDirDamper( vRotate, vRotate, fElapsedTime );
			SetCursorPos(m_ptHoldCursor.x, m_ptHoldCursor.y);
			//dxInputDev.HoldCursor();

			bCURSOR_SHOW = FALSE;
		}

		m_bIsDragEndFromUI = GfxInterfaceBase::GetInstance()->IsDragging();
	}
	else
	{
		cViewPort.CalculateVeloDirDamper( vRotate, D3DXVECTOR3(0,0,0), fElapsedTime );
	}

	fZoom += cViewPort.GetVeloZoom()*static_cast<float>(nKEY_ZOOM)/1000.0f;
	fZoom =  cViewPort.CalculateVeloZoomDamper( fZoom, fElapsedTime );

	cViewPort.CameraMove ( vMove );
	cViewPort.CameraRotation ( vRotate );
	cViewPort.CameraZoom ( fZoom );

	CCursor::GetInstance().SetShowCursor ( bCURSOR_SHOW );
}

BOOL GLControlBase::CharBlockCursorUpdate(void)
{
	//!! SF_TODO
	/*
	// ui가 커서 제어 발생;
	if ( NSGUI::IsCharMoveBlock() )  
	{
		if ( m_pInterface->IsItemDecompose() )
			CCursor::GetInstance().SetCursorType(CURSOR_DECOMPOSE);
		else
			CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);

		return (CCursor::GetInstance().GetCurCursorType()!=CURSOR_NORMAL);
	}*/
	return FALSE;
}

void GLControlBase::DefaultCursorUpdate( float fTime, float fElapsedTime, SACTIONTAR& sActorTar )
{
	// 마우스 피킹시 오브젝트에 맞는 커서를 선택;
	if ( !CharBlockCursorUpdate() )
	{
		//! 마우스 모양 선택.
		switch ( sActorTar.emACTAR )
		{
		case EMACTAR_NULL:
			CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);
			break;
		case EMACTAR_SUMMON_ATTACK:
			CCursor::GetInstance().SetCursorType(CURSOR_ATTACK);
			break;
		case EMACTAR_PC_PVP:
			CCursor::GetInstance().SetCursorType(CURSOR_ATTACK);
			break;
		case EMACTAR_MOB:
			{
				bool bNONDAMAGED(false);
				PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
				if (pLAND)
				{
					std::tr1::shared_ptr<GLCrowClient> spCROW = pLAND->GetCrow(sActorTar.sTARID.GaeaId);
					if (spCROW && spCROW->IsNonDamaged())
						bNONDAMAGED = true;
				}

				if (bNONDAMAGED)
					CCursor::GetInstance().SetCursorType(CURSOR_NONATTACK);
				else
					CCursor::GetInstance().SetCursorType(CURSOR_ATTACK);
			}
			break;

		case EMACTAR_NPC:
			{
				bool bTALK(false);
				PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
				if (pLAND)
				{
					std::tr1::shared_ptr<GLCrowClient> spCROW = pLAND->GetCrow(sActorTar.sTARID.GaeaId);
					if (spCROW)
					{
						switch (spCROW->m_pCrowData->m_emNPCType)
						{
						default:
						case NPC_TYPE_NORMAL:
							{
								if ( _tcslen(spCROW->m_pCrowData->GetTalkFile()) )
								{
									CCursor::GetInstance().SetCursorType(CURSOR_TALK);
								}
								else
								{
									CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);
								}
							}
							break;

						case NPC_TYPE_POSTBOX:
							{
								CCursor::GetInstance().SetCursorType(CURSOR_POSTBOX);
							}
							break;
						};
					}
				}
			}
			break;

		case EMACTAR_PC_OUR:	CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
		case EMACTAR_PC_P2P:	CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
		case EMACTAR_PC_ANY:	CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
		case EMACTAR_ITEM:		CCursor::GetInstance().SetCursorType(CURSOR_HAND);			break;
		case EMACTAR_MATERIAL:	CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
		};

		if( CURSOR_NORMAL != CCursor::GetInstance().GetCurCursorType() )
		{
			if( m_bFixedCusor == true )
				CCursor::GetInstance().SetCursorType( CURSOR_NORMAL );
				
			m_fGfxUICheckTime += fElapsedTime;
			if( m_fGfxUICheckTime <= m_fMaxGfxUICheckTime )
				return ;

			if( true == GfxInterfaceBase::GetInstance()->IsMouseInUI() )
			{
				m_bFixedCusor = true;
				m_fGfxUICheckTime = 0.0f;
				CCursor::GetInstance().SetCursorType( CURSOR_NORMAL );
			}
			else
			{
				m_fGfxUICheckTime = 0.0f;
				m_bFixedCusor = false;
			}

			m_fMaxGfxUICheckTime = 1.5f;
		}
		else
		{
			if( m_bFixedCusor == false )
			{
				m_fMaxGfxUICheckTime = 0.0f;
				m_fGfxUICheckTime = 0.0f;
			}
		}
	}
}

void GLControlBase::DefaultMovingUpdate ( float fTime, float fElapsedTime )
{
	//! 커서가 윈도우 안에 있을경우만
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
	{
		return;
	}
	
	const bool bDominateControl = m_pKeySetting->GetKeyState( EMSHORTCUTS_DOMINATE_CONTROL ) & DXKEY_DOWNED;	

	//! 일반 이동 처리.
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwMoveKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_MOVING );
	
	if ( pCharacter->IsMoveAble() )
	{
		SDOMINATE& sDominate = pCharacter->GetDominate();
		switch ( sDominate.emController )
		{			
		case EMSPEC_DOMINATE_TYPE_RANDOM:
			DefaultMovingUpdate_Random();
			break;

		case EMSPEC_DOMINATE_TYPE_SELF:
			if ( sDominate.bDominate && bDominateControl )
				DefaultMovingUpdate_Domination(fTime, vFromPt, vTargetPt);					
			else
				DefaultMovingUpdate_Self(fTime, vFromPt, vTargetPt);
			break;
		}
	}

	if ( !(dwMoveKeyState&DXKEY_DOWNED) && pCharacter->IsActState(EM_ACT_CONTINUEMOVE) )		
	{
		pCharacter->ReSetActState(EM_ACT_CONTINUEMOVE);
	}
}

void GLControlBase::DefaultMovingUpdate_Self(const float fTime, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vTargetPt)
{
	// GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_LargeMap_HideMoveIcon );

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwMoveKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_MOVING );	
	BOOL bActionMoveTo(FALSE);
	if ( dwMoveKeyState&DXKEY_UP )
	{
		pCharacter->ResetReservationAction(true);
		pCharacter->ReSetActState(EM_ACT_CONTINUEMOVE);
	}
	else if ( dwMoveKeyState&DXKEY_DOWN )
	{
		pCharacter->ResetReservationAction(true);
		pCharacter->SetActState(EM_ACT_CONTINUEMOVE);
		bActionMoveTo = pCharacter->ActionMoveTo ( fTime, vFromPt, vTargetPt, TRUE );
	}
	else if ( (dwMoveKeyState&DXKEY_PRESSED || dwMoveKeyState&DXKEY_DRAG) )
	{
		pCharacter->ResetReservationAction();

		if ( pCharacter->IsActState(EM_ACT_CONTINUEMOVE) )
		{
			bActionMoveTo = pCharacter->ActionMoveTo ( fTime, vFromPt, vTargetPt, TRUE );
		}
	}

	if ( (bActionMoveTo == TRUE) && GLMacroManager::GetInstance().IsActive() )
		GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_LOCK_USERINPUT, MACRO::EMMACRO_MSGVALUE_LOCK_USERINPUT_MOVE);
}

void GLControlBase::DefaultMovingUpdate_Random(void)
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	SDOMINATE& sDomination = pCharacter->GetDominate();
	
	const DWORD dwCurTimer = ::clock();
	if ( dwCurTimer - sDomination.dwTimer > 1000 )
	{
		const D3DXVECTOR3& vPosition =  pCharacter->GetPosition();
		const float fDirectAngle = sc::Random::getInstance().RANDOM_NUM() * 6.28f;
		const float fDistance = sc::Random::getInstance().RANDOM_POS() * sDomination.fRadiusMoveable;
		D3DXVECTOR3 vRandomPosition(vPosition.x + cos(fDirectAngle) * fDistance, vPosition.y, vPosition.z + sin(fDirectAngle) * fDistance);	

//		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_LargeMap_HideMoveIcon );

		pCharacter->ResetReservationAction();
		pCharacter->ReSetActState(EM_ACT_CONTINUEMOVE);
		pCharacter->LargeMapMoveTo(&vRandomPosition, FALSE);	
		sDomination.dwTimer = dwCurTimer;
	}
}

void GLControlBase::DefaultMovingUpdate_Domination(const float fTime, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vTargetPt)
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	SDOMINATE& sDomination = pCharacter->GetDominate();
	const DWORD dwMoveKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_MOVING );

	if ( dwMoveKeyState&DXKEY_UP )
		pCharacter->DominationMoveTo(fTime, vFromPt, vTargetPt);
	else if ( dwMoveKeyState&DXKEY_DOWNED )			
		pCharacter->DominationMoveTo(fTime, vFromPt, vTargetPt, TRUE);
}

void GLControlBase::DropHoldItem()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( NULL == pCharacter )
		return;

	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if( FALSE == DxViewPort::GetInstance().GetMouseTargetPosWnd( vTargetPt ) )
		return;

	// 손에 든 아이템을 땅에다 버리기/사용	
	DWORD dwDropKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_ITEMDROP );
	DWORD dwDropUseKeyState = m_pKeySetting->GetKeyState( EMSHORTCUTS_ITEMDROP_USE );
	DWORD dwDropKeyStateTotal = dwDropKeyState | dwDropUseKeyState;

	if( true == holditem::HaveHoldItem() && ( dwDropKeyStateTotal & ( DXKEY_DOWNED | DXKEY_UP | DXKEY_DUP ) ) )
	{
		D3DXVECTOR3 vCollisionPos;
		DWORD dwCollisionID;
		BOOL bCollision;
		m_pGaeaClient->GetActiveMap()->GetNaviMesh()->IsCollision(
			vFromPt, vTargetPt, vCollisionPos, dwCollisionID, bCollision );

		if( FALSE == bCollision )
			return;

		D3DXVECTOR3 vDir = vCollisionPos - pCharacter->GetPosition();
		vDir.y = 0.0f;

		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vDropPos = pCharacter->GetPosition() + vDir * float( GLCONST_CHAR::wBODYRADIUS + 12 );

		m_pGaeaClient->GetActiveMap()->GetNaviMesh()->IsCollision(
			vDropPos + D3DXVECTOR3( 0.0f, 5.0f, 0.0f ),	vDropPos + D3DXVECTOR3( 0.0f, -5.0f, 0.0f ),
			vCollisionPos, dwCollisionID, bCollision );

		vDropPos = pCharacter->GetPosition();
		if( TRUE == bCollision )
			vDropPos = vCollisionPos;

		if( pCharacter->CheckHoldToField( vDropPos ) == S_OK )
		{
			SITEM* pItem = NULL;
			if( holditem::IsHoldPlace( holditem::INVENTORY ) )
				pItem = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );

			if( NULL == pItem )
			{
				holditem::ReleaseHoldItem();
				return;
			}

			if( pItem->BasicType() == ITEM_FIRECRACKER )
			{
				pCharacter->ReqHoldToFieldFireCracker( vDropPos );
			}
			else
			{
				holditem::SetReserveDelete(true);
				GLWidgetScript::GetInstance().LuaCallFunc< void >( 
					NSWIDGET_SCRIPT::g_strFunc_HoldItemToField,
					"-n, -n, -n, -f, -f, -f",
					(int) holditem::WhenHoldItem(), holditem::GetHoldPosX(), holditem::GetHoldPosY(),
					vDropPos.x, vDropPos.y, vDropPos.z );
			}
		}

		holditem::ReleaseHoldItem();
	}
}

const bool GLControlBase::IsCameraBlock()
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	if( pChar)
	{
		if( RF_CAMERA_HIT( pChar->m_EffSkillVarSet ).IsOn() )
			return true;
	}

	return false;
}