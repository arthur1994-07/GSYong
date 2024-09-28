#include "StdAfx.h"

#include <map>

#include "RnMapInfoTap.h"

#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"


#include "../../../RanLogic/Crow/GLMobScheduleMan.h"
#include "../../../RanLogic/Crow/GLMobSchedule.h"

#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/Crow/GLCrowData.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/Quest/GLQuest.h"


#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"


#include "../Util/CheckBox.h"
#include "../Item/ItemImage.h"

#include "../../InnerInterface.h"


#include "../../../SigmaCore/Container/GLList.h"

#include "./RnOldLargeMapWindow.h"

#include "./RnMapHelper.h"

#include "../../../RanLogic/Item/GLItemMan.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"


namespace
{
	const char* ID2GAMEWORD_MONSTER_INFO = "몬스터 정보";
	const char* ID2GAMEWORD_MONSTER_REWARD_ITEM = "몬스터 보상 아이템";




	
	
}


RnMapInfoTap::RnMapInfoTap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, 
						   EngineDeviceMan* pEngineDevice,RnOldLargeMapWindow*& pOldLargeMapWindow) :
	CUIGroupHelper(pEngineDevice),
	m_pOldLargeMapWindow(pOldLargeMapWindow)
{
	m_LoadedMapId = NATIVEID_NULL();

	m_pInterface  = pInterface;
	m_pGaeaClient = pGaeaClient;
}

void RnMapInfoTap::CreateSubControl()
{
	CreateLineBox("MAP_WINDOW_INFO_TAP_REGION","MAP_WINDOW_LINEBOX_TEXTUREINFO");

	CUIControl* pBox = NULL;

	pBox = CreateLineBox("MAP_WINDOW_INFO_TAP_SIGN_MONSTER_INFO_REGION","MAP_WINDOW_LINEBOX2_TEXTUREINFO");
	pBox->SetDiffuse(D3DCOLOR_XRGB( BYTE(111 * 1.7),BYTE(127*1.7),255) );

	pBox = CreateLineBox("MAP_WINDOW_INFO_TAP_SIGN_MONSTER_REWARD_REGION","MAP_WINDOW_LINEBOX2_TEXTUREINFO");
	pBox->SetDiffuse(D3DCOLOR_XRGB( BYTE(111 * 1.7),BYTE(127*1.7),255) );

	CUIControl* pMonsterScrollBarSpace = new CUIControl(m_pEngineDevice);
	pMonsterScrollBarSpace->CreateSub(this, "MAP_WINDOW_INFO_TAP_MONSTER_LIST_SCROLL_SPACE");
	RegisterControl ( pMonsterScrollBarSpace );
	m_pMonsterScrollBarSpace = pMonsterScrollBarSpace;

	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, MONSTER_LIST_SCROLL_BAR );
	pScrollBar->CreateBaseScrollBar( "MAP_WINDOW_INFO_TAP_MONSTER_LIST_SCROLL" );
	RegisterControl ( pScrollBar );
	m_pMonsterScrollBar = pScrollBar;
	m_pMonsterScrollBar->GetThumbFrame ()->SetScrollParent(pMonsterScrollBarSpace);


	m_pMonsterAllSignCheckBox = CreateCheckBox("MAP_WINDOW_INFO_TAP_SIGN_CHECKBOX",CHECKBOX_ALL_SIGN,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[0] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER1_CHECKBOX",CHECKBOX_MONSTER1,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[1] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER2_CHECKBOX",CHECKBOX_MONSTER2,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[2] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER3_CHECKBOX",CHECKBOX_MONSTER3,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[3] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER4_CHECKBOX",CHECKBOX_MONSTER4,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[4] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER5_CHECKBOX",CHECKBOX_MONSTER5,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[5] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER6_CHECKBOX",CHECKBOX_MONSTER6,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[6] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER7_CHECKBOX",CHECKBOX_MONSTER7,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);	
	m_pMonsterCheckBox[7] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER8_CHECKBOX",CHECKBOX_MONSTER8,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[8] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER9_CHECKBOX",CHECKBOX_MONSTER9,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);
	m_pMonsterCheckBox[9] = CreateCheckBox("MAP_WINDOW_INFO_TAP_MONSTER10_CHECKBOX",CHECKBOX_MONSTER10,"MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF",false);


	m_pMonsterIcon[0] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER1_ICON",ICON_MONSTER1);
	m_pMonsterIcon[1] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER2_ICON",ICON_MONSTER2);
	m_pMonsterIcon[2] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER3_ICON",ICON_MONSTER3);
	m_pMonsterIcon[3] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER4_ICON",ICON_MONSTER4);
	m_pMonsterIcon[4] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER5_ICON",ICON_MONSTER5);
	m_pMonsterIcon[5] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER6_ICON",ICON_MONSTER6);
	m_pMonsterIcon[6] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER7_ICON",ICON_MONSTER7);
	m_pMonsterIcon[7] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER8_ICON",ICON_MONSTER8);
	m_pMonsterIcon[8] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER9_ICON",ICON_MONSTER9);
	m_pMonsterIcon[9] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER10_ICON",ICON_MONSTER10);


// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER1_DETAIL",DETAIL_MONSTER1);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER2_DETAIL",DETAIL_MONSTER2);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER3_DETAIL",DETAIL_MONSTER3);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER4_DETAIL",DETAIL_MONSTER4);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER5_DETAIL",DETAIL_MONSTER5);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER6_DETAIL",DETAIL_MONSTER6);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER7_DETAIL",DETAIL_MONSTER7);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER8_DETAIL",DETAIL_MONSTER8);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER9_DETAIL",DETAIL_MONSTER9);
// 	CreateControl("MAP_WINDOW_INFO_TAP_MONSTER10_DETAIL",DETAIL_MONSTER10);

	

	CreateText(ID2GAMEWORD("MAP_MONSTER_INFO"), "MAP_WINDOW_INFO_TAP_SIGN_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[0] = CreateText("MONSTER_NAME1","MAP_WINDOW_INFO_TAP_MONSTER1_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID); 
	m_pMonsterTextBox[1] = CreateText("MONSTER_NAME2","MAP_WINDOW_INFO_TAP_MONSTER2_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[2] = CreateText("MONSTER_NAME3","MAP_WINDOW_INFO_TAP_MONSTER3_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[3] = CreateText("MONSTER_NAME4","MAP_WINDOW_INFO_TAP_MONSTER4_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[4] = CreateText("MONSTER_NAME5","MAP_WINDOW_INFO_TAP_MONSTER5_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[5] = CreateText("MONSTER_NAME6","MAP_WINDOW_INFO_TAP_MONSTER6_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[6] = CreateText("MONSTER_NAME7","MAP_WINDOW_INFO_TAP_MONSTER7_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[7] = CreateText("MONSTER_NAME8","MAP_WINDOW_INFO_TAP_MONSTER8_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[8] = CreateText("MONSTER_NAME9","MAP_WINDOW_INFO_TAP_MONSTER9_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);
	m_pMonsterTextBox[9] = CreateText("MONSTER_NAME10","MAP_WINDOW_INFO_TAP_MONSTER10_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);

	CreateText(ID2GAMEWORD("MAP_MONSTER_REWARD_ITEM"), "MAP_WINDOW_INFO_TAP_REWARD_TEXT",TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,NO_ID);

	
	//////////////////////////////////////////////////////////////////////////

// 	m_pMonsterMouseOverRect[0] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER1_MOUSE_OVER_RECT",ICON_MONSTER1);
// 	m_pMonsterMouseOverRect[1] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER2_MOUSE_OVER_RECT",ICON_MONSTER2);
// 	m_pMonsterMouseOverRect[2] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER3_MOUSE_OVER_RECT",ICON_MONSTER3);
// 	m_pMonsterMouseOverRect[3] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER4_MOUSE_OVER_RECT",ICON_MONSTER4);
// 	m_pMonsterMouseOverRect[4] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER5_MOUSE_OVER_RECT",ICON_MONSTER5);
// 	m_pMonsterMouseOverRect[5] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER6_MOUSE_OVER_RECT",ICON_MONSTER6);
// 	m_pMonsterMouseOverRect[6] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER7_MOUSE_OVER_RECT",ICON_MONSTER7);
// 	m_pMonsterMouseOverRect[7] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER8_MOUSE_OVER_RECT",ICON_MONSTER8);
// 	m_pMonsterMouseOverRect[8] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER9_MOUSE_OVER_RECT",ICON_MONSTER9);
// 	m_pMonsterMouseOverRect[9] = CreateControl("MAP_WINDOW_INFO_TAP_MONSTER10_MOUSE_OVER_RECT",ICON_MONSTER10);


    //////////////////////////////////////////////////////////////////////////

	
	for (int v=0; v < REWARD_V_LINE_COUNT; ++v)
	{
		for (int h=0; h < REWARD_H_LINE_COUNT; ++h)
		{
			int index = v*REWARD_H_LINE_COUNT + h;  
			std::string imageKey = sc::string::format("MAP_WINDOW_INFO_TAP_MONSTER_REWARD%1%", index + 1);
			m_pRewardItems[v][h] = CreateItemImage(imageKey.c_str(),REWARD_ITEM1 + index );
		}
	}

	CUIControl* pRewardScrollBarSpace = new CUIControl(m_pEngineDevice);
	pRewardScrollBarSpace->CreateSub(this, "MAP_WINDOW_INFO_TAP_MONSTER_REWARD_SCROLL_SPACE");
	RegisterControl ( pRewardScrollBarSpace );
	m_pRewardScrollBarSpace = pRewardScrollBarSpace;

	CBasicScrollBarEx* pRewardScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pRewardScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, REWARD_SCROLL_BAR);
	pRewardScrollBar->CreateBaseScrollBar( "MAP_WINDOW_INFO_TAP_MONSTER_REWARD_SCROLL" );
	RegisterControl ( pRewardScrollBar );
	m_pRewardScrollBar = pRewardScrollBar;
	m_pRewardScrollBar->GetThumbFrame ()->SetScrollParent(m_pRewardScrollBarSpace);
}

void RnMapInfoTap::SetVisibleSingle( BOOL isVisible )
{
	CUIGroupHelper::SetVisibleSingle(isVisible);

	if( isVisible )
	{
		MakeMonsterInfo();
	}
}

void RnMapInfoTap::MakeMonsterInfo()
{	

	if( m_LoadedMapId == m_pOldLargeMapWindow->GetMapLevelFile()->GetBaseMapID() )
		return;

	m_LoadedMapId = m_pOldLargeMapWindow->GetMapLevelFile()->GetBaseMapID();

	std::vector<DWORD> crowIdVec;
	m_pOldLargeMapWindow->GetMonster(&crowIdVec);

	m_pMonsterScrollBar->GetThumbFrame()->SetState( (int)crowIdVec.size(), MONSTER_VIEW_PAGE_COUNT );

	m_MonsterData.clear();

	m_RewardItemMap.clear();
	m_UniqueRewardItemSet.clear();

	const int MAX_VIEW_REWARD_ITEM_COUNT = 500;

	for(size_t i=0; i < crowIdVec.size(); ++i)
	{
		Data data;
		data.isCheck = false;
		data.MonsterID = crowIdVec[i];
		m_MonsterData.push_back(data);


		//////////////////////////////////////////////////////////////////////////
		std::vector<DWORD>& monsterGenItemVec = m_RewardItemMap[data.MonsterID];
		monsterGenItemVec.reserve(30);

		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(data.MonsterID);
		CGENITEM::VEC_SGENINFO &vecGenInfo = pCrowData->m_cGenItem.m_vecGenInfo;

		CGENITEM::VEC_SGENINFO_ITER InfoIter = vecGenInfo.begin();
		for ( ; InfoIter != vecGenInfo.end(); ++InfoIter )
		{
			SGENINFO& sInfo = (*InfoIter);
			SGENINFO::VEC_SPGENITEM& genItemVec = sInfo.vecGenItem;

			for (size_t itemIdx = 0; itemIdx < genItemVec.size(); ++itemIdx)
			{
				if( EMGENITEMTYPE_ITEM == genItemVec[itemIdx]->emType )
				{
					SGENITEMSPEC* pSpec = dynamic_cast<SGENITEMSPEC*>(genItemVec[itemIdx].get());
					if( pSpec )
					{
						const GLItemMan::SGENITEMS& genItems = GLItemMan::GetInstance().GetGenSpecItems(pSpec->dwSpecID);
						const std::vector<SNATIVEID>& genItemVec = genItems.m_vecItems;
						for (size_t genIdx = 0; genIdx < genItemVec.size(); ++genIdx)
						{
							if(genItemVec[genIdx] != NATIVEID_NULL())
								monsterGenItemVec.push_back( genItemVec[genIdx].Id() );
						}
					}
					else 
					{
						SGENITEMID* pItem = dynamic_cast<SGENITEMID*>(genItemVec[itemIdx].get());
						GASSERT(pItem);
						if(pItem->sItemID != NATIVEID_NULL())
							monsterGenItemVec.push_back( pItem->sItemID.Id() );
					}
				}

			}
		}
	}
	

	//////////////////////////////////////////////////////////////////////////

	UpdateMonsterInfo();
}

void RnMapInfoTap::UpdateMonsterInfo()
{
	int monsterListPos = m_pMonsterScrollBar->GetThumbFrame()->GetPosition();
	for (int i = 0; i < MONSTER_VIEW_PAGE_COUNT; ++i)
	{
		m_pMonsterCheckBox[i]->SetVisibleSingle(FALSE);
		m_pMonsterIcon[i]->SetVisibleSingle(FALSE);
		m_pMonsterTextBox[i]->SetVisibleSingle(FALSE);
	}

	size_t size = m_MonsterData.size() < MONSTER_VIEW_PAGE_COUNT ? m_MonsterData.size() : MONSTER_VIEW_PAGE_COUNT;
	for (size_t i=0; i < size; ++i)
	{
		m_pMonsterCheckBox[i]->SetVisibleSingle(TRUE);
		m_pMonsterIcon[i]->SetVisibleSingle(TRUE);
		m_pMonsterTextBox[i]->SetVisibleSingle(TRUE);

		if ( (i+monsterListPos) < m_MonsterData.size() )
		{
			const Data& data = m_MonsterData[i+monsterListPos];
			data.isCheck ? m_pMonsterCheckBox[i]->SetCheckOn() : m_pMonsterCheckBox[i]->SetCheckOff();

			SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(data.MonsterID);

			//////////////////////////////////////////////////////////////////////////

			RnMapPrivate::ChangeTexture(m_pMonsterIcon[i],RnMapPrivate::GetMonsterIconType(m_pGaeaClient,pCrowData));

			//////////////////////////////////////////////////////////////////////////


			m_pMonsterTextBox[i]->SetTextToLimit(pCrowData->GetName(), TRUE );
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	for (int v=0; v < REWARD_V_LINE_COUNT; ++v)
	{
		for (int h=0; h < REWARD_H_LINE_COUNT; ++h)
		{
			m_pRewardItems[v][h]->SetVisibleSingle(FALSE);
		}
	}

	
	m_UniqueRewardItemSet.clear();
	//////////////////////////////////////////////////////////////////////////
	for (size_t i = 0; i < m_MonsterData.size(); ++i)
	{
		if( m_MonsterData[i].isCheck )
		{
			std::vector<DWORD>& itemVec = m_RewardItemMap[m_MonsterData[i].MonsterID];
			for (size_t genIdx = 0; genIdx < itemVec.size(); ++genIdx)
			{
				m_UniqueRewardItemSet.insert(itemVec[genIdx]);
			}

		}
	}

	int lineCount = (int)m_UniqueRewardItemSet.size()/REWARD_H_LINE_COUNT;

	if( m_UniqueRewardItemSet.size() % REWARD_H_LINE_COUNT != 0 )
		lineCount++;

	m_pRewardScrollBar->GetThumbFrame()->SetState( lineCount, REWARD_V_LINE_COUNT );


	
	//여기 하고 있었다  체크한것만 드랍아이템 표시하기

	int rewardPos = m_pRewardScrollBar->GetThumbFrame()->GetPosition();
	for (int v=0; v < REWARD_V_LINE_COUNT; ++v)
	{
		for (int h=0; h < REWARD_H_LINE_COUNT; ++h)
		{
			size_t index = (v+rewardPos)*REWARD_H_LINE_COUNT + h;  
			if( index >= m_UniqueRewardItemSet.size())
			{
				m_pRewardItems[v][h]->SetVisibleSingle(FALSE);
				continue;
			}

			m_pRewardItems[v][h]->SetVisibleSingle(TRUE);
			
			std::set<DWORD>::iterator it = m_UniqueRewardItemSet.begin();
			std::advance(it,index); 

			m_pRewardItems[v][h]->SetItem(*it);
			
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
}


void RnMapInfoTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	UpdateMonsterInfo();

}

void RnMapInfoTap::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	int scrollPos = m_pMonsterScrollBar->GetThumbFrame()->GetPosition();

	if( m_pMonsterAllSignCheckBox->GetWndID() == ControlID )
	{
		if( dwMsg & UIMSG_CHECKBOX_PUSH)
		{
			for (size_t i=0; i < m_MonsterData.size(); ++i)
			{
				m_MonsterData[i].isCheck = m_pMonsterAllSignCheckBox->IsChecked() ? true : false;
			}
		}
	}

	for (int i = 0; i < MONSTER_VIEW_PAGE_COUNT; ++i)
	{
		if( m_pMonsterCheckBox[i]->GetWndID() == ControlID )
		{
			if( dwMsg & UIMSG_CHECKBOX_PUSH)
			{
				if ( (i+scrollPos) < m_MonsterData.size() )
				{
					m_MonsterData[i+scrollPos].isCheck = m_pMonsterCheckBox[i]->IsChecked() ? true : false;
				}
			}
		}
	}


	for (int i = 0; i < MONSTER_VIEW_PAGE_COUNT; ++i)
	{
		if( m_pMonsterIcon[i]->GetWndID() == ControlID )
		{
			if( CHECK_MOUSE_IN(dwMsg) )
			{
				if ( (i+scrollPos) < m_MonsterData.size() )
				{
					const Data& data = m_MonsterData[i+scrollPos];
					m_pOldLargeMapWindow->ShowTooltip(data.MonsterID);
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////

	for (int v=0; v < REWARD_V_LINE_COUNT; ++v)
	{
		for (int h=0; h < REWARD_H_LINE_COUNT; ++h)
		{
			if( m_pRewardItems[v][h]->GetWndID() == ControlID)
			{
				if( CHECK_MOUSE_IN(dwMsg) )
				{
					if ( m_pRewardItems[v][h]->GetItemID() != NATIVEID_NULL() )
					{
						m_pInterface->SHOW_ITEM_INFO(
							SITEMCUSTOM(m_pRewardItems[v][h]->GetItemID()),
							FALSE, FALSE, FALSE, FALSE, FALSE,
							USHRT_MAX, USHRT_MAX );
					}
				}
			}
		}
	}
}

bool RnMapInfoTap::IsVisibleMonster(DWORD monsterID)
{
	for (size_t i=0; i < m_MonsterData.size(); ++i)
	{
		if( m_MonsterData[i].MonsterID == monsterID )
			return m_MonsterData[i].isCheck;
	}

	return false;
}
