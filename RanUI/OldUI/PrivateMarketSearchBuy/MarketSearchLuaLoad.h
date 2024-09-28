#ifndef _MARKET_SEARCH_LUA_LOAD_
#define _MARKET_SEARCH_LUA_LOAD_

#pragma once

class GLGaeaClient;

struct UI_OPTION
{
	std::string m_OptionName;
	//std::vector<EMITEM_TYPE> m_TypeVec;
	std::vector<int> m_TypeVec;

	UI_OPTION()
	{
	}

	inline std::string OptionName() const { return m_OptionName; }
	//inline const std::vector<EMITEM_TYPE>& TypeVec() const { return m_TypeVec; }
	inline const std::vector<int>& TypeVec() const { return m_TypeVec; }
};

class LoadMarketLuaFile
{
public:
	LoadMarketLuaFile( GLGaeaClient* pGaeaClient );
	GLGaeaClient* m_pGaeaClient;
	std::string m_OptionFielName;
	std::vector<UI_OPTION> m_UiOptionItemCategory;
	std::vector<UI_OPTION> m_UiOptionWearTypeCategory_1;
	std::vector<UI_OPTION> m_UiOptionWearTypeCategory_2;
	std::vector<UI_OPTION> m_UiOptionLevelCategory;
	std::vector<UI_OPTION> m_UiOptionAddRandom_1;
	std::vector<UI_OPTION> m_UiOptionAddRandom_2;
	std::vector<UI_OPTION> m_UiOptionAddRandom_3;
	std::vector<UI_OPTION> m_UiOptionClassCategory;
	std::vector<UI_OPTION> m_UiOptionSexCategory;
	std::vector<UI_OPTION> m_UiOptionBasicStats_1;
	std::vector<UI_OPTION> m_UiOptionBasicStats_2;
	std::vector<UI_OPTION> m_UiOptionBasicStats_3;
	std::vector<UI_OPTION> m_UiOptionBasicStatsCategory;
	void LoadItemCategory();
	void LoadWearTypeCategory();
	void LoadLevelCategory();
	void LoadAddRandom();
	void LoadClassCategory();
	void LoadSexCategory();
	void LoadBasicStats();
	void LoadBasicStatsCategory();
	bool LoadUiOptionFile();
	std::vector<UI_OPTION> GetItemCategory();
	std::vector<UI_OPTION> GetWearTypeCategory_1();
	std::vector<UI_OPTION> GetWearTypeCategory_2();
	std::vector<UI_OPTION> GetLevelCategory();
	std::vector<UI_OPTION> GetAddRandom_1();
	std::vector<UI_OPTION> GetAddRandom_2();
	std::vector<UI_OPTION> GetAddRandom_3();
	std::vector<UI_OPTION> GetClassCategory();
	std::vector<UI_OPTION> GetSexCategory();
	std::vector<UI_OPTION> GetBasicStats_1();
	std::vector<UI_OPTION> GetBasicStats_2();
	std::vector<UI_OPTION> GetBasicStats_3();
	std::vector<UI_OPTION> GetBasicStatsCategory();
};


#endif // _MARKET_SEARCH_LUA_LOAD_