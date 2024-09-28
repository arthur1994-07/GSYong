#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "GLItemSkill.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{
	VOID SISKILL::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "Target" << ",";
		SFile << "fOccuRate" << ",";
		SFile << "Skill_Mid" << ",";
		SFile << "Skill_Sid" << ",";
		SFile << "Skill_Lv" << ",";
		SFile << "LinkSkillIndex" << ",";
	}

	VOID SISKILL::SaveCsv ( std::fstream &SFile )
	{
		SFile << emSkillTarget		<< ",";
		SFile << fOccuRate			<< ",";
		SFile << sSkillID.wMainID	<< ",";
		SFile << sSkillID.wSubID	<< ",";
		SFile << wSkillLv			<< ",";
		SFile << nLinkSkillIndex	<< ",";
	}

	VOID SISKILL::LoadCsv ( CStringArray &StrArray, int& iCsvCur )
	{
		emSkillTarget		= (EMITEM_SKILL_LINK)atoi( StrArray[ iCsvCur++ ] );
		fOccuRate			= (float)atof( StrArray[ iCsvCur++ ] );
		sSkillID.wMainID	= (WORD)atoi( StrArray[ iCsvCur++ ] );
		sSkillID.wSubID		= (WORD)atoi( StrArray[ iCsvCur++ ] );
		wSkillLv			= (WORD)atoi( StrArray[ iCsvCur++ ] );
		nLinkSkillIndex		= atoi( StrArray[ iCsvCur++ ] );
	}

	BOOL SISKILL::LOAD(sc::BaseStream& SFile)
	{
		DWORD dwVer(0), dwSize(0);

		SFile >> dwVer;
		SFile >> dwSize;

		switch ( dwVer )
		{
		case 0x0100:
			Load_100(SFile);
			break;

		case SISKILL::VERSION:
			Load_101(SFile);
			break;
		}

		return TRUE;
	}

	BOOL SISKILL::SAVE(sc::BaseStream& SFile)
	{
		SFile << DWORD(SISKILL::VERSION);
		SFile << (DWORD)sizeof(SISKILL);

		SFile << (DWORD)emSkillTarget;
		SFile << fOccuRate;
		SFile << sSkillID.dwID;
		SFile << wSkillLv;

		SFile << nLinkSkillIndex;
		
		return TRUE;
	}

	BOOL SISKILL::Load_100(sc::BaseStream& SFile)
	{
		DWORD dwTemp = 0;

		SFile >> dwTemp;

		emSkillTarget = (EMITEM_SKILL_LINK)dwTemp;

		SFile >> fOccuRate;
		SFile >> sSkillID.dwID;
		SFile >> wSkillLv;

		return TRUE;
	}

	BOOL SISKILL::Load_101(sc::BaseStream& SFile)
	{
		DWORD dwTemp = 0;

		SFile >> dwTemp;

		emSkillTarget = (EMITEM_SKILL_LINK)dwTemp;

		SFile >> fOccuRate;
		SFile >> sSkillID.dwID;
		SFile >> wSkillLv;

		SFile >> nLinkSkillIndex;

		return TRUE;
	}
}