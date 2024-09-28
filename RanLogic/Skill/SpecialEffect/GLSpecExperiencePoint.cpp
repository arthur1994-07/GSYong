#include "pch.h"

#include "../../../SigmaCore/Lua/MinLua.h"

#include "../../../EngineLib/G-Logic/GLOGIC.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/File/BaseStream.h"
#include "../../../SigmaCore/Util/MinMessageBox.h"

#include "GLSpecExperiencePoint.h"

namespace SKILL
{
	namespace ExperiencePoint
	{
		HRESULT LoadFile(const std::string& fileName, const bool bPastLoad)
		{			
			ExperiencePointSettings* const pExpSettings(ExperiencePointSettings::getInstance());
			if ( bPastLoad == false )
			{
				pExpSettings->setValid();
				return S_OK;
			}

			sc::lua_init();

			std::string fullPath(GLOGIC::GetServerPath());
			fullPath.append("scripts\\");
			fullPath.append(fileName);

			if ( sc::lua_doFile(fullPath) == false )
				return E_FAIL;

			DWORD nTableIndex = 0;
			LuaPlus::LuaObject experienceTableGroup = sc::lua_getGlobalFromName("ExperienceTable");

			for ( LuaPlus::LuaTableIterator iterReward(experienceTableGroup); iterReward; iterReward.Next() )
			{
				LuaPlus::LuaTableIterator iterNode(iterReward.GetValue());

				if ( pExpSettings->push(nTableIndex) == false )
				{
					std::string ErrorMsg(sc::string::format("%1% file open failed.", fullPath));
					sc::ErrorMessageBox(ErrorMsg, false);
					return false;
				}

				for (; iterNode; )
				{
					const float fSelectRate(iterNode.GetValue().GetFloat());	iterNode.Next();
					const float fMinRate(iterNode.GetValue().GetFloat());		iterNode.Next();
					const float fMaxRate(iterNode.GetValue().GetFloat());	iterNode.Next();
					if ( pExpSettings->push(nTableIndex, fSelectRate, fMinRate, fMaxRate) == false )
					{
						std::string ErrorMsg(sc::string::format("%1% file open failed.", fullPath));
						sc::ErrorMessageBox(ErrorMsg, false);
						return false;
					}
				}
				++nTableIndex;
			}

			pExpSettings->setValid();

			return S_OK;
		}
	}

	ExperiencePointSettings* const ExperiencePointSettings::getInstance(void)
	{
		static ExperiencePointSettings instance;
		return &instance;
	}
	ExperiencePointSettings::~ExperiencePointSettings(void)
	{
	}
	ExperiencePointSettings::ExperiencePointSettings(void)
	{
	}

	const bool ExperiencePointSettings::push(const unsigned int nTableIndex)
	{
		if ( nTableIndex != m_TableVector.size() )
			return false;

		m_TableVector.push_back(Table());

		return true;
	}

	const bool ExperiencePointSettings::push(const unsigned int nTableIndex, const float fSelectRate, const float fMinRate, const float fMaxRate)
	{
		if ( nTableIndex >= m_TableVector.size() )
			return false;

		m_TableVector[nTableIndex].push(fSelectRate, fMinRate, fMaxRate);
		return true;
	}	


	ExperiencePointSettings::Table::Table(void)
	{
	}
	ExperiencePointSettings::Table::~Table(void)
	{
	}
	void ExperiencePointSettings::Table::push(const float fSelectRate, const float fMinRate, const float fMaxRate)
	{
		m_ItemVector.push_back(Item(fSelectRate, fMinRate, fMaxRate));
	}




	ExperiencePointSettings::Table::Item::Item(const float fSelectRate, const float fMinRate, const float fMaxRate)
		: m_fSelectRate(fSelectRate)
		, m_fMinRate(fMinRate)
		, m_fMaxRate(fMaxRate)
	{
	}
	ExperiencePointSettings::Table::Item::~Item(void)
	{
	}
}

