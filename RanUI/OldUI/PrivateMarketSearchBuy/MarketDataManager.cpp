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

#include "../../../EngineLib/Common/SUBPATH.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "MarketDataManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


MaketDataMng::MaketDataMng(GLGaeaClient* pGaeaClient)
{

}

MaketDataMng::~MaketDataMng()
{

}

bool MaketDataMng::LoadUiOptionFile()
{
	m_UiOption.clear();

	try
	{
		std::string OptionFile(sc::getAppPath());
		const SUBPATH* pPath = m_pGaeaClient->GetSubPath();
		OptionFile.append(pPath->GLogicPath()); //SUBPATH::GLogicPath());
		OptionFile.append(GLogicData::GetInstance().GetPointShopUiOptionFileName());
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

		LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("PointShopOption");
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
							"PointShopOption unknown option type %1%", IntType));
						AfxMessageBox(ErrorMsg.c_str(), MB_OK);                        
					}
					else
					{
						UiOption.m_TypeVec.push_back(EMITEM_TYPE(IntType));
					}
				}
				m_UiOption.push_back(UiOption);
			}            
		}
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
