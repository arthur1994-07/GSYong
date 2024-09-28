#pragma once

#include "./GLItemDef.h"

namespace sc
{
	class BaseStream;
	class SerialFile;
}

namespace ITEM
{
	struct SISKILL
	{
		enum { VERSION = 0x0101 };

		float fOccuRate;

		SNATIVEID			sSkillID;
		EMITEM_SKILL_LINK	emSkillTarget;

		WORD				wSkillLv;

		int					nLinkSkillIndex;

		SISKILL() : fOccuRate( 0.f ), sSkillID( NATIVEID_NULL() ), emSkillTarget( EMITEM_SKILL_ALL ), wSkillLv(1), nLinkSkillIndex(-1)
		{}

		VOID reset() { fOccuRate = 0.f; sSkillID = NATIVEID_NULL(); emSkillTarget = EMITEM_SKILL_ALL; wSkillLv = 1; nLinkSkillIndex = -1; }

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );

		BOOL LOAD(sc::BaseStream& SFile);
		BOOL SAVE(sc::BaseStream& SFile);

		inline const EMITEM_SKILL_LINK GetTarget ( void ) const { return emSkillTarget; }
		inline const SNATIVEID& GetSkillID ( void ) const { return sSkillID; }
		inline const WORD GetSkillLv ( void ) const { return ( wSkillLv - 1 );}
		inline const float GetOccurRate ( void ) const { return fOccuRate; }

		inline const int GetLinkSkillIndex ( void ) const { return nLinkSkillIndex; }

	private :
		BOOL Load_100(sc::BaseStream& SFile);
		BOOL Load_101(sc::BaseStream& SFile);
	};
}


