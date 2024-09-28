

#pragma once


#include "../Util/UIGroupHelper.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

class CBasicScrollBarEx; 

class CCheckBox;
class CBasicTextBox;

#include "RnMapHelper.h"
#include "../../../EngineLib/G-Logic/TypeDefine.h"


#include <map>
#include <vector>

namespace RnMapPrivate
{
	
	class IMapInfoInterface;
}

class RnOldLargeMapWindow;


class RnMapInfoTap : public CUIGroupHelper , public RnMapPrivate::IMapInfoInterface
{
public:
	enum
	{
		MONSTER_LIST_SCROLL_BAR = NO_ID +1,
		CHECKBOX_ALL_SIGN,
		CHECKBOX_MONSTER1,
		CHECKBOX_MONSTER2,
		CHECKBOX_MONSTER3,
		CHECKBOX_MONSTER4,
		CHECKBOX_MONSTER5,
		CHECKBOX_MONSTER6,
		CHECKBOX_MONSTER7,
		CHECKBOX_MONSTER8,
		CHECKBOX_MONSTER9,
		CHECKBOX_MONSTER10,

		ICON_MONSTER1,
		ICON_MONSTER2,
		ICON_MONSTER3,
		ICON_MONSTER4,
		ICON_MONSTER5,
		ICON_MONSTER6,
		ICON_MONSTER7,
		ICON_MONSTER8,
		ICON_MONSTER9,
		ICON_MONSTER10,

// 		DETAIL_MONSTER1,
// 		DETAIL_MONSTER2,
// 		DETAIL_MONSTER3,
// 		DETAIL_MONSTER4,
// 		DETAIL_MONSTER5,
// 		DETAIL_MONSTER6,
// 		DETAIL_MONSTER7,
// 		DETAIL_MONSTER8,
// 		DETAIL_MONSTER9,
// 		DETAIL_MONSTER10,

		REWARD_SCROLL_BAR,
		REWARD_ITEM1,
		REWARD_ITEM2,
		REWARD_ITEM3,
		REWARD_ITEM4,
		REWARD_ITEM5,
		REWARD_ITEM6,
		REWARD_ITEM7,
		REWARD_ITEM8,
		REWARD_ITEM9,
		REWARD_ITEM10,
		REWARD_ITEM11,
		REWARD_ITEM12,
		REWARD_ITEM13,
		REWARD_ITEM14,
		REWARD_ITEM15,
		REWARD_ITEM16,
		REWARD_ITEM17,
		REWARD_ITEM18,
		REWARD_ITEM19,
		REWARD_ITEM20,
		REWARD_ITEM21,
		REWARD_ITEM22,
		REWARD_ITEM23,
		REWARD_ITEM24,

	};

	static const int MONSTER_VIEW_PAGE_COUNT = 10;

	
	static const int REWARD_H_LINE_COUNT = 4;
	static const int REWARD_V_LINE_COUNT = 6;
	static const int REWARD_COUNT = REWARD_H_LINE_COUNT * REWARD_V_LINE_COUNT;

	RnMapInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice,
		RnOldLargeMapWindow*& pOldLargeMapWindow);

	void CreateSubControl();

	virtual void SetVisibleSingle(BOOL isVisible);
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );


	virtual GLGaeaClient* GetGaeaClient() { return m_pGaeaClient; }


	virtual bool IsVisibleMonster(DWORD monsterID);


	void MakeMonsterInfo();
	void UpdateMonsterInfo();


private:
	SNATIVEID m_LoadedMapId;

	CInnerInterface* m_pInterface;
	GLGaeaClient* m_pGaeaClient; 
	RnOldLargeMapWindow*& m_pOldLargeMapWindow;

	CUIControl*			m_pMonsterScrollBarSpace;
	CBasicScrollBarEx*  m_pMonsterScrollBar;
	CUIControl*			m_pRewardScrollBarSpace;
	CBasicScrollBarEx*  m_pRewardScrollBar;

	CCheckBox*	   m_pMonsterAllSignCheckBox;
	CCheckBox*	   m_pMonsterCheckBox[MONSTER_VIEW_PAGE_COUNT];
	CUIControl*    m_pMonsterIcon[MONSTER_VIEW_PAGE_COUNT];
	CBasicTextBox* m_pMonsterTextBox[MONSTER_VIEW_PAGE_COUNT];
//	CUIControl*    m_pMonsterMouseOverRect[MONSTER_VIEW_PAGE_COUNT];
	
	struct Data
	{
		Data() { isCheck = false; MonsterID = (DWORD)-1; }
		bool isCheck;
		DWORD MonsterID;
	};

	std::vector<Data> m_MonsterData;
	std::map<DWORD, std::vector<DWORD> > m_RewardItemMap;
	std::set<DWORD> m_UniqueRewardItemSet;
	//////////////////////////////////////////////////////////////////////////
	CItemImage*  m_pRewardItems[REWARD_V_LINE_COUNT][REWARD_H_LINE_COUNT];

};


