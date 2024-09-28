
#include "pch.h"

#include "../../enginelib/G-Logic/GLogic.h"
#include "../../SigmaCore/Hash/CHash.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Memory/SmartPtr.h"
#include "../GLogicFile.h"
#include "../GLogicDataMan.h"

#include "./RandomAddon.h"

namespace RandomAddon
{
	Manager& Manager::GetInstance(void)
	{
		static Manager s_Manager;
		return s_Manager;
	}

	extern BOOL Lua_LoadDoBuffer(std::string& strPath, std::string& strFile)
	{
		const std::string strPathLogic(GLOGIC::GetPath())
			, strFullPath(strPath + strFile);

		std::auto_ptr<sc::BaseStream> pStreamBase(
			GLOGIC::openfile_basestream(
			GLOGIC::UseLogicZipFile(),
			GLogicData::GetInstance().GetGlogicZipFile(),
			strFullPath.c_str(),
			strFile,
			false,
			GLOGIC::UseLogicPackFile()));

		if ( pStreamBase.get() == 0 )
		{
			const std::string strError(sc::string::format("RandomAddon::Lua_LoadDoBuffer faild: %1%", strFullPath));
			::MessageBox(0, strError.c_str(), "error", MB_OK);
			return E_FAIL;
		}

		const size_t nSize = static_cast< size_t >( pStreamBase->GetFileSize() );
		void* pBuffer = ::malloc(nSize);
		pStreamBase->ReadBuffer(pBuffer, 0);

		sc::lua_init();
		if ( !sc::lua_doBuffer(static_cast<char*>(pBuffer), nSize, NULL) )
		{
			const std::string strError(sc::string::format("RandomAddon::Lua_LoadDoBuffer lua_doBuffer faild: %1%", strFullPath));
			::MessageBox(0, strError.c_str(), "error", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	Value::Value(void) : fRateChoose(0.0f), nValueHigh(0) , nValueLow(0){}
	Value::Value(float rateChoose, INT	valueHigh, INT valueLow) : fRateChoose(rateChoose), nValueHigh(valueHigh) , nValueLow(valueLow)	{}
	Value::Value(float rateChoose, float valueHigh, float valueLow)	: fRateChoose(rateChoose), fValueHigh(valueHigh) , fValueLow(valueLow) {}

	Bunch::Bunch(void) : fRateChoose(0.0f), emAddon(EMADD_NONE), nValueHigh(0) , nValueLow(0) { }
	Bunch::Bunch(float rateChoose, EMITEM_ADDON addon) : fRateChoose(0.0f), emAddon(addon), nValueHigh(0) , nValueLow(0) { }
	Entity::Entity(void) : fRateGenerate(0.0f) {}
	Manager::Manager(void) {}
	Manager::~Manager(void){}

	const float Value::GetValue(const bool bInteger) const
	{
		float fReturnValue = -1.f;

		float fGenValueLow = 0.f;
		float fGenValueHigh = 0.f;
		float fGenValueGap = 0.f;

		const float fRate = sc::Random::getInstance().GetFloatRate();
		if ( bInteger == true )
		{
			fGenValueLow = static_cast< float >(this->nValueLow);
			fGenValueHigh = static_cast< float >(this->nValueHigh);
			fGenValueGap = fGenValueHigh - fGenValueLow;

			fReturnValue = fGenValueLow + fGenValueGap * fRate;
		}
		else
		{
			fGenValueLow = fValueLow;
			fGenValueHigh = fValueHigh;
			fGenValueGap = fGenValueHigh - fGenValueLow;

			fReturnValue = fGenValueLow + fGenValueGap * fRate;
		}

		if ( fReturnValue < fGenValueLow )
		{
			sc::writeLogError(sc::string::format(
				"Invalid Add Option Value ( fValue(%1%) < fLowValue(%2%) )",
				fReturnValue,
				fGenValueLow));

			fReturnValue = fGenValueLow;
		}
		else if ( fGenValueHigh < fReturnValue )
		{
			sc::writeLogError(sc::string::format(
				"Invalid Add Option Value ( fHighValue(%1%) < fValue(%2%) )",
				fGenValueHigh,
				fReturnValue));

			fReturnValue = fGenValueHigh;
		}

		return fReturnValue;
	}

	const Value& Bunch::GetValue(void) const
	{
		const float fPercent(sc::Random::getInstance().GetFloatRate());
		float fRange(0.0f);
		const INT nSize(vecValue.size());
		for ( INT i = 0; i < nSize; ++i )
		{
			fRange += vecValue[i].fRateChoose;
			if ( fPercent <= fRange )
				return vecValue[i];
		}

		// 아무것에도 걸리지않으면;
		return vecValue[0];
	}

	BOOL Entity::LoadFile(std::string& strFileName)
	{
		if ( Lua_LoadDoBuffer(std::string(GLOGIC::GetPath()), strFileName) != S_OK )
			return E_FAIL;

		m_strName = strFileName;

		LuaPlus::LuaObject loRate = sc::lua_getGlobalFromName("fRATE");
		fRateGenerate = loRate.GetFloat();

		Load(Bunch::BUNCH_TYPE_DROP, vecBunchDrop);
		Load(Bunch::BUNCH_TYPE_GENERATE, vecBunchRemodel);

		return S_OK;
	}

	BOOL Entity::Load(Bunch::EMBUNCHTYPE emType, VEC_BUNCH& vecStorage)
	{
		LuaPlus::LuaObject loList =
			( emType == Bunch::BUNCH_TYPE_DROP )
			? sc::lua_getGlobalFromName("DROP")
			: sc::lua_getGlobalFromName("GENERATE");

		for ( LuaPlus::LuaTableIterator itrFile(loList); itrFile.IsValid(); itrFile.Next() )
		{
			LuaPlus::LuaObject luaObj = itrFile.GetValue();

			Bunch bunchAddon;
			// 조건;
			{
				LuaPlus::LuaObject luaObj_Condition = luaObj.GetByName("CONDITION");

				LuaPlus::LuaTableIterator luaItr_Con(luaObj_Condition);
				// 발생확률, 옵션인덱스;
				const float fBunchRate = luaObj_Condition.GetByIndex(1).GetFloat();
				const int nBunchEnum = luaObj_Condition.GetByIndex(2).GetInteger();

				bunchAddon.fRateChoose = fBunchRate;
				bunchAddon.emAddon = (EMITEM_ADDON)nBunchEnum;
			}

			// 설정값;
			{
				LuaPlus::LuaObject luaObj_Set = luaObj.GetByName("SETTING");
				for ( LuaPlus::LuaTableIterator itrSet(luaObj_Set); itrSet.IsValid(); itrSet.Next() )
				{
					LuaPlus::LuaObject luaObj_Value = itrSet.GetValue();
					// 당첨확률, 최대값, 최소값;
					Value valueAddon( luaObj_Value.GetByIndex(1).GetFloat()
						, luaObj_Value.GetByIndex(2).GetInteger()
						, luaObj_Value.GetByIndex(3).GetInteger() );

					if ( ITEM::bAddonValueInteger[bunchAddon.emAddon] )
					{
						// 당첨확률, 최대값, 최소값;
						Value valueAddon( luaObj_Value.GetByIndex(1).GetFloat()
							, luaObj_Value.GetByIndex(2).GetInteger()
							, luaObj_Value.GetByIndex(3).GetInteger() );
						bunchAddon.vecValue.push_back(valueAddon);

						bunchAddon.nValueHigh = max(bunchAddon.nValueHigh, valueAddon.nValueHigh);
						bunchAddon.nValueLow = min(bunchAddon.nValueLow, valueAddon.nValueLow);
					}
					else
					{
						// 당첨확률, 최대값, 최소값;
						Value valueAddon( luaObj_Value.GetByIndex(1).GetFloat()
							, luaObj_Value.GetByIndex(2).GetFloat()
							, luaObj_Value.GetByIndex(3).GetFloat() );
						bunchAddon.vecValue.push_back(valueAddon);

						bunchAddon.nValueHigh = max(bunchAddon.fValueHigh, valueAddon.fValueHigh);
						bunchAddon.nValueLow = min(bunchAddon.fValueLow, valueAddon.fValueLow);
					}
				}
				vecStorage.push_back(bunchAddon);
			}
		}
		return S_OK;
	}

	BOOL Manager::LoadFile(GLogicFile* const pFileLogic)
	{
		if ( Lua_LoadDoBuffer(std::string(GLOGIC::GetPath()), pFileLogic->GetRandomAddonOpFileName()) != S_OK )
			return E_FAIL;

		try
		{
			LuaPlus::LuaObject loItemList = sc::lua_getGlobalFromName("FileList");

			std::vector<std::string> vecFileList;
			vecFileList.reserve(loItemList.GetTableCount());
			for ( LuaPlus::LuaTableIterator itrFile(loItemList); itrFile.IsValid(); itrFile.Next() )
			{
				const std::string strFile(sc::string::unicodeToAnsi((const wchar_t*)itrFile.GetValue().GetWString()));
				vecFileList.push_back(strFile);
			}

			const INT nSize(static_cast<INT>(vecFileList.size()));
			for ( INT i = 0; i < nSize; ++i )
			{
				Entity entity;
				entity.LoadFile(vecFileList[i]);

				unsigned int _hashKey(HASH::generateHashKey(vecFileList[i].c_str(), vecFileList[i].length()));
				m_mapEntity.insert( std::make_pair(_hashKey, entity) );
			}

			return S_OK;
		}
		catch( const LuaPlus::LuaException& e )
		{        
			return E_FAIL;
		}

		return E_FAIL;
	}

	BOOL Manager::Find(IN const std::string& strName, IN OUT Entity& sEntity)
	{
		unsigned int nHashKey(HASH::generateHashKey(strName.c_str(), strName.length()));
		MAP_ENTITY_ITR itrEntity = this->m_mapEntity.find(nHashKey);
		if ( itrEntity == m_mapEntity.end() )
			return E_FAIL;

		sEntity = (itrEntity->second);
		return TRUE;
	}
};