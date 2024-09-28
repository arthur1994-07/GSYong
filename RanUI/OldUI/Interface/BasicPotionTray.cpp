#include "StdAfx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../../../RanLogicClient/StaticHoldItem.h"

#include "../../InnerInterface.h"
#include "../Item/ItemMove.h"
#include "BasicPotionTray.h"
#include "BasicQuickPotionSlotEx.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	int	CBasicPotionTray::nOUTOFRANGE = -1;

CBasicPotionTray::CBasicPotionTray(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pFont8 ( NULL )
{
}

CBasicPotionTray::~CBasicPotionTray ()
{
}

void CBasicPotionTray::CreateSubControl ()
{
	m_pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CUIControl* pSlotBegin = new CUIControl(m_pEngineDevice);
	pSlotBegin->CreateSub ( this, "BASIC_QUICK_POTION_TRAY_SLOT_BEGIN" );
	pSlotBegin->SetControlNameEx ( "퀵 포션 슬롯 스타트" );
    RegisterControl ( pSlotBegin );

	for ( int i = 0; i < EMACTIONQUICK_SIZE; i++ )
	{
		CString strKeyword;
		strKeyword.Format ( "BASIC_QUICK_POTION_TRAY_SLOT%d", i );

		CBasicQuickPotionSlotEx* pSlot = new CBasicQuickPotionSlotEx ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pSlot->CreateSub ( this, strKeyword.GetString(), UI_FLAG_DEFAULT, QUICK_POTION_SLOT0 + i );
		pSlot->CreateSubControl ( i );
		RegisterControl ( pSlot );
		m_pSlot[i] = pSlot;
	}

// #ifndef CH_PARAM // 중국 인터페이스 변경
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "QUICK_POTION_TRAY_CLOSE_BUTTON", UI_FLAG_DEFAULT, QUICK_POTION_TRAY_CLOSE_BUTTON );
	pButton->CreateFlip ( "QUICK_POTION_TRAY_CLOSE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	pButton->SetControlNameEx ( "퀵슬롯트레이 클로즈버튼" );
	RegisterControl ( pButton );
// #endif
}

void CBasicPotionTray::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case QUICK_POTION_SLOT0:
	case QUICK_POTION_SLOT1:
	case QUICK_POTION_SLOT2:
	case QUICK_POTION_SLOT3:
	case QUICK_POTION_SLOT4:
	case QUICK_POTION_SLOT5:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{	
				int nIndex = ControlID - QUICK_POTION_SLOT0;
				SetSlotIndex ( nIndex );

				if ( !m_pInterface->IsFirstItemSlot () ) return ;				

				//	스냅, 스킬 이미지 붙이기
				IItemMove* pItemMove = m_pInterface->GetItemMove();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPotionImagePos = m_pSlot[nIndex]->GetAbsPosPotionImage ();
					pItemMove->SetUIWindowGlobalPos ( rcSlotPotionImagePos );						

					AddMessageEx ( UIMSG_MOUSEIN_POTIONSLOT );
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
                    const SITEMCUSTOM& sItemCustom = holditem::GetHoldItem();
					if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )	//	설정
					{
						m_pGaeaClient->GetCharacter()->ReqItemQuickSet( (WORD) nIndex );
					}
					else	//	쓰기
					{
						m_pGaeaClient->GetCharacter()->ReqActionQ( (WORD) nIndex );
					}
				}

				//	퀵 아이템 해제
				if ( dwMsg & UIMSG_RB_UP )
				{
					m_pGaeaClient->GetCharacter()->ReqItemQuickReSet ( (WORD) nIndex );
				}

                //	설명 뿌리기
				SACTION_SLOT sSlot = m_pGaeaClient->GetCharacter()->m_sACTIONQUICK[nIndex];
				if ( sSlot.sNID != NATIVEID_NULL () )
				{
					const SITEM* pItem = GLogicData::GetInstance().GetItem ( sSlot.sNID );
					if ( pItem )
					{
						DWORD dwLevel = pItem->sBasicOp.emLevel;

                        if ( GLUseFeatures::GetInstance().IsUsingItemTooltip() )
                        {
                            std::string strText = "";
                            // Info : 약품류 사용 효과
							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							{
								if( pItem->sDrugOp.dwCureVolume == 0 )
									strText = sc::string::format( "%s", CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", pItem->sDrugOp.emDrug).GetString() );
								else
									strText = sc::string::format( "%s :%d",CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", pItem->sDrugOp.emDrug).GetString() );

								if ( pItem->sDrugOp.bRatio )
								{
									strText += "%";
								}
							}
							else
							{
								if( pItem->sDrugOp.dwCureVolume == 0 )
									strText = sc::string::format( "%s", COMMENT::ITEMDRUG[pItem->sDrugOp.emDrug].c_str() );
								else
									strText = sc::string::format( "%s :%d",COMMENT::ITEMDRUG[pItem->sDrugOp.emDrug].c_str(), pItem->sDrugOp.dwCureVolume );

								if ( pItem->sDrugOp.bRatio )
								{
									strText += "%";
								}
							}
							m_pInterface->BEGIN_COMMON_LINEINFO_MULTI();
							m_pInterface->ADD_COMMON_LINEINFO_MULTI( pItem->GetName(), COMMENT::ITEMCOLOR[dwLevel] );
							m_pInterface->ADD_COMMON_LINEINFO_MULTI( strText.c_str(), COMMENT::ITEMCOLOR[dwLevel]);
							m_pInterface->END_COMMON_LINEINFO_MULTI();
                        }
                        else
						{
							m_pInterface->SHOW_COMMON_LINEINFO( pItem->GetName(), COMMENT::ITEMCOLOR[dwLevel] );
						}
					}
				}
			}
		}
		break;

	case QUICK_POTION_TRAY_CLOSE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					if ( m_pGaeaClient->GetTutorial()->IsTutorial() ) // 튜토리얼 중에는 사용금지.
						m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE") );
					else
						AddMessageEx ( UIMSG_TRAY_CLOSE_LBUP );
				}
			}
		}
		break;
	}
}

void CBasicPotionTray::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	if ( !IsVisible () ) return ;
	SetSlotIndex ( nOUTOFRANGE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void 	CBasicPotionTray::SetShotcutText ( DWORD nID, CString& strTemp )
{
	m_pSlot[nID]->SetShotcutText(strTemp );	
}


void CBasicPotionTray::ResetSlotText()
{
	for ( int i = 0; i < EMACTIONQUICK_SIZE; i++ )
	{
		if ( m_pSlot[i] )
			m_pSlot[i]->ResetText();
	}
}