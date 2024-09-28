#include "pch.h"
#include "stdafx.h"
#include "../../../SigmaCore/File/FileUtil.h"
#include "../../../SigmaCore/File/TxtFile.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../../SigmaCore/Util/SystemInfo.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../EngineLib/Common/SUBPATH.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#include "MarketSearchLuaLoad.h"

LoadMarketLuaFile::LoadMarketLuaFile( GLGaeaClient* pGaeaClient )
: m_pGaeaClient( pGaeaClient )
{
}

bool LoadMarketLuaFile::LoadUiOptionFile()
{
	try
	{
		std::string OptionFile(sc::getAppPath());
		const SUBPATH* pPath = m_pGaeaClient->GetSubPath();
		OptionFile.append(pPath->GLogicPath()); //SUBPATH::GLogicPath());
		OptionFile.append(GLogicData::GetInstance().GetPrivateMarketFileName());
		m_OptionFielName = OptionFile;

		sc::lua_init();
		if (!sc::lua_doFile(m_OptionFielName.c_str()))
		{        
			::MessageBoxA(
				NULL,
				sc::lua_getError().c_str(),
				OptionFile.c_str(),
				MB_OK);
			return false;
		}

		LoadItemCategory();
		LoadWearTypeCategory();
		LoadLevelCategory();
		LoadAddRandom();
		LoadClassCategory();
		LoadSexCategory();
		LoadBasicStats();
		LoadBasicStatsCategory();

		return true;

	}
	catch (const LuaPlus::LuaException& e)
	{        
		::MessageBoxA(
			NULL,
			e.GetErrorMessage(),
			m_OptionFielName.c_str(),
			MB_OK);
		return false;
	}
}


void LoadMarketLuaFile::LoadItemCategory()
{
	LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("ItemCategory");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;
		for (LuaPlus::LuaTableIterator Typeiter(OptionIter.GetValue()); Typeiter; Typeiter.Next())
		{
			std::wstring OptionNameW((const wchar_t*) Typeiter.GetValue().GetWString());
			UiOption.m_OptionName = sc::string::unicodeToAnsi(OptionNameW);

			Typeiter.Next();

			for (LuaPlus::LuaTableIterator IntIter(Typeiter.GetValue()); IntIter; IntIter.Next())
			{
				int IntType = IntIter.GetValue().GetInteger();
				if (IntType < 0 || IntType >= ITEM_NSIZE)
				{
					std::string ErrorMsg(
						sc::string::format(
						"ItemCategory unknown option type %1%", IntType));
					AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
				}
				else
				{
					UiOption.m_TypeVec.push_back(EMITEM_TYPE(IntType));
				}
			}
			m_UiOptionItemCategory.push_back(UiOption);
		}            
	}
}

void LoadMarketLuaFile::LoadWearTypeCategory()
{
	LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("WearTypeCategory_1");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;

		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			if (IntType < 0 || IntType >= ITEMATT_NSIZE)
			{
				std::string ErrorMsg(
					sc::string::format(
					"WearTypeCategory_1 unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					UiOption.m_OptionName = CGameTextMan::GetInstance().GetCommentText("ITEMATTACK", IntType).GetString();
				else
					UiOption.m_OptionName = COMMENT::ITEMATTACK[GLITEM_ATT(IntType)];
				UiOption.m_TypeVec.push_back(GLITEM_ATT(IntType));
			}

			m_UiOptionWearTypeCategory_1.push_back(UiOption);
		}
	}

	OptionList = sc::lua_getGlobalFromName("WearTypeCategory_2");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;

		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			if (IntType < 0 || IntType >= SUIT_NSIZE)
			{
				std::string ErrorMsg(
					sc::string::format(
					"WearTypeCategory_2 unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					UiOption.m_OptionName = CGameTextMan::GetInstance().GetCommentText("ITEMTYPE", IntType).GetString();
				else
					UiOption.m_OptionName = COMMENT::ITEMTYPE[EMSUIT(IntType)];
				UiOption.m_TypeVec.push_back(EMSUIT(IntType));
			}

			m_UiOptionWearTypeCategory_2.push_back(UiOption);
		}
	}
}

void LoadMarketLuaFile::LoadLevelCategory()
{
	LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("LevelCategory");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;

		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			if (IntType < 0 || IntType >= EMITEM_LEVEL_NSIZE)
			{
				std::string ErrorMsg(
					sc::string::format(
					"LevelCategory unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					UiOption.m_OptionName = CGameTextMan::GetInstance().GetCommentText("ITEMLEVEL", IntType).GetString();
				else
					UiOption.m_OptionName = COMMENT::ITEMLEVEL[EMITEMLEVEL(IntType)];
				UiOption.m_TypeVec.push_back(EMITEMLEVEL(IntType));
			}

			m_UiOptionLevelCategory.push_back(UiOption);
		}
	}

}

void LoadMarketLuaFile::LoadAddRandom()
{
	LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("AddRandom_1");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;

		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			if (IntType < 0 || IntType >= EMADD_SIZE)
			{
				std::string ErrorMsg(
					sc::string::format(
					"LoadAddRandom_1 unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					UiOption.m_OptionName = CGameTextMan::GetInstance().GetCommentText("ITEMADDON", IntType).GetString();
				else
					UiOption.m_OptionName = COMMENT::ITEMADDON[EMITEM_ADDON(IntType)];
				UiOption.m_TypeVec.push_back(EMITEM_ADDON(IntType));
			}

			m_UiOptionAddRandom_1.push_back(UiOption);
		}
	}

	OptionList = sc::lua_getGlobalFromName("AddRandom_2");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;

		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			if (IntType < 0 || IntType >= EMVAR_SIZE)
			{
				std::string ErrorMsg(
					sc::string::format(
					"LoadAddRandom_2 unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					UiOption.m_OptionName = CGameTextMan::GetInstance().GetCommentText("ITEMVAR", IntType).GetString();
				else
					UiOption.m_OptionName = COMMENT::ITEMVAR[EMITEM_VAR(IntType)];
				UiOption.m_TypeVec.push_back(EMITEM_VAR(IntType));
			}

			m_UiOptionAddRandom_2.push_back(UiOption);
		}
	}

	OptionList = sc::lua_getGlobalFromName("AddRandom_3");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;

		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			if (IntType < 0 || IntType >= EMVAR_SIZE)
			{
				std::string ErrorMsg(
					sc::string::format(
					"LoadAddRandom_3 unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					UiOption.m_OptionName = CGameTextMan::GetInstance().GetCommentText("ITEMVOL", IntType).GetString();
				else
					UiOption.m_OptionName = COMMENT::ITEMVOL[EMITEM_VAR(IntType)];
				UiOption.m_TypeVec.push_back(EMITEM_VAR(IntType));
			}

			m_UiOptionAddRandom_3.push_back(UiOption);
		}
	}

}

void LoadMarketLuaFile::LoadClassCategory()
{
	LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("ClassCategory");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;
		std::string strTemp;
		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			strTemp.clear();
			strTemp = sc::string::format( "%s", ID2GAMEWORD("ITEM_SHOP_SEARCH_CLASS", IntType));
			if ( strTemp.length() == NULL )
			{
				std::string ErrorMsg(
					sc::string::format(
					"ClassCategory unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				UiOption.m_OptionName = strTemp.c_str();
				UiOption.m_TypeVec.push_back(IntType);
			}

			m_UiOptionClassCategory.push_back(UiOption);
		}
	}
}

void LoadMarketLuaFile::LoadSexCategory()
{
	LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("SexCategory");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;
		std::string strTemp;
		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			strTemp.clear();
			strTemp = sc::string::format( "%s", ID2GAMEWORD("BASIC_SEX", IntType));
			if ( strTemp.length() == NULL )
			{
				std::string ErrorMsg(
					sc::string::format(
					"SexCategory unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				UiOption.m_OptionName = strTemp.c_str();
				UiOption.m_TypeVec.push_back(IntType);
			}

			m_UiOptionSexCategory.push_back(UiOption);
		}
	}
}

void LoadMarketLuaFile::LoadBasicStats()
{
	LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("BasicStats_1");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;
		std::string strTemp;
		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			strTemp.clear();
			strTemp = sc::string::format( "%s", ID2GAMEWORD("ITEM_ADVANCED_INFO", IntType));
			if ( strTemp.length() == NULL )
			{
				std::string ErrorMsg(
					sc::string::format(
					"BasicStats_1 unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				UiOption.m_OptionName = strTemp.c_str();
				UiOption.m_TypeVec.push_back(IntType);
			}

			m_UiOptionBasicStats_1.push_back(UiOption);
		}
	}

	OptionList = sc::lua_getGlobalFromName("BasicStats_2");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;
		for (LuaPlus::LuaTableIterator Typeiter(OptionIter.GetValue()); Typeiter; Typeiter.Next())
		{
			std::wstring OptionNameW((const wchar_t*) Typeiter.GetValue().GetWString());
			UiOption.m_OptionName = sc::string::unicodeToAnsi(OptionNameW);

			Typeiter.Next();

			for (LuaPlus::LuaTableIterator IntIter(Typeiter.GetValue()); IntIter; IntIter.Next())
			{
				int IntType = IntIter.GetValue().GetInteger();
				//if (IntType < 0 || IntType >= ITEM_NSIZE)
				//{
				//	std::string ErrorMsg(
				//		sc::string::format(
				//		"ItemCategory unknown option type %1%", IntType));
				//	AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
				//}
				//else
				//{
					UiOption.m_TypeVec.push_back(IntType);
				/*}*/
			}
			m_UiOptionItemCategory.push_back(UiOption);
		}            
	}

	OptionList = sc::lua_getGlobalFromName("BasicStats_3");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;
		std::string strTemp;
		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			strTemp.clear();
			strTemp = sc::string::format( "%s", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", IntType));
			if ( strTemp.length() == NULL )
			{
				std::string ErrorMsg(
					sc::string::format(
					"BasicStats_3 unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				UiOption.m_OptionName = strTemp.c_str();
				UiOption.m_TypeVec.push_back(IntType);
			}

			m_UiOptionBasicStats_1.push_back(UiOption);
		}
	}
}

void LoadMarketLuaFile::LoadBasicStatsCategory()
{
	LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("BasicStatsCategory");
	for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
	{
		UI_OPTION UiOption;
		std::string strTemp;
		for (LuaPlus::LuaTableIterator IntIter(OptionIter.GetValue()); IntIter; IntIter.Next())
		{
			int IntType = IntIter.GetValue().GetInteger();
			strTemp.clear();
			strTemp = sc::string::format( "%s", ID2GAMEWORD("ITEM_SHOP_SEARCH_STATUS", IntType));
			if ( strTemp.length() == NULL )
			{
				std::string ErrorMsg(
					sc::string::format(
					"BasicStatsCategory unknown option type %1%", IntType));
				AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
			}
			else
			{
				UiOption.m_OptionName = strTemp.c_str();
				UiOption.m_TypeVec.push_back(IntType);
			}

			m_UiOptionBasicStatsCategory.push_back(UiOption);
		}
	}
}


std::vector<UI_OPTION> LoadMarketLuaFile::GetItemCategory()
{
	return m_UiOptionItemCategory;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetWearTypeCategory_1()
{
	return m_UiOptionWearTypeCategory_1;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetWearTypeCategory_2()
{
	return m_UiOptionWearTypeCategory_2;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetLevelCategory()
{
	return m_UiOptionLevelCategory;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetAddRandom_1()
{
	return m_UiOptionAddRandom_1;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetAddRandom_2()
{
	return m_UiOptionAddRandom_2;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetAddRandom_3()
{
	return m_UiOptionAddRandom_3;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetClassCategory()
{
	return m_UiOptionClassCategory;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetSexCategory()
{
	return m_UiOptionSexCategory;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetBasicStats_1()
{
	return m_UiOptionBasicStats_1;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetBasicStats_2()
{
	return m_UiOptionBasicStats_2;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetBasicStats_3()
{
	return m_UiOptionBasicStats_3;
}

std::vector<UI_OPTION> LoadMarketLuaFile::GetBasicStatsCategory()
{
	return m_UiOptionBasicStatsCategory;
}
