#include "../pch.h"
#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringFormatW.h"

#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/Collision.h"
#include "../../EngineLib/GlobalParam.h"

#include "../GLogicData.h"
#include "../GLStringTable.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../GLUseFeatures.h"
#include "./GLItem.h"
#include "./GLItemMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void SITEM::LoadAfter()
{

	//////////////////////////////////////////////////////////////////////////
	// 실제 텍스트 할당
	// 변경사유 : 매 프레임마다 str를 로드하는 방식을 피하기 위함
	char* szName = NULL;
	if (GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		szName = const_cast< char* >( CGameTextMan::GetInstance().GetText( sBasicOp.strName, 0, CGameTextMan::EM_ITEM_TEXT ).GetString() );
	else 
		szName = const_cast< char* >( GLStringTable::GetInstance().GetString( sBasicOp.strName, GLStringTable::ITEM ) );

	if (szName == NULL || strcmp( szName, "Skip" ) == 0 )
		strRealName_NO_SAVE = sBasicOp.strName.c_str();
	else
		strRealName_NO_SAVE = szName;


	//////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////////
//	$1.	2003-01-17 Txx:xx	[최초 릴리즈]
////////////////////////////////////////////////////////////////////////////////////

BOOL SITEM::SaveFile ( sc::SerialFile &SFile )
{
	SFile << DWORD(VERSION);

	SFile << (DWORD)FILE_SBASIC;
	{
		SFile << DWORD(ITEM::SBASIC::VERSION);

		SFile.BeginBlock();
		{
			sBasicOp.SAVE (SFile);
		}
		SFile.EndBlock();
	}

	SFile << (DWORD)FILE_SSUIT;
	{
		SFile << DWORD(ITEM::SSUIT::VERSION);
		SFile << (DWORD)sizeof(ITEM::SSUIT);
		SFile.WriteBuffer ( &sSuitOp, sizeof(ITEM::SSUIT) );
	}

	SFile << (DWORD)FILE_SDRUG;
	{
		SFile << DWORD(ITEM::SDRUG::VERSION);
		SFile << (DWORD)sizeof(ITEM::SDRUG);
		SFile.WriteBuffer ( &sDrugOp, sizeof(ITEM::SDRUG) );
	}

	SFile << (DWORD)FILE_SSKILLBOOK;
	{
		SFile << DWORD(ITEM::SSKILLBOOK::VERSION);
		SFile << (DWORD)sizeof(ITEM::SSKILLBOOK);
		SFile.WriteBuffer ( &sSkillBookOp, sizeof(ITEM::SSKILLBOOK) );
	}

	SFile << SITEM::FILE_SGENERATE;
	{
		SFile << DWORD(ITEM::SGENERATE::VERSION);
		SFile << (DWORD)sizeof(ITEM::SGENERATE);
		SFile.WriteBuffer ( &sGenerateOp, sizeof(ITEM::SGENERATE) );
	}

	SFile << SITEM::FILE_SGRINDING;
	{
		sGrindingOp.SAVE(SFile);
	}

	SFile << SITEM::FILE_BOX;
	{
		sBox.SAVE (SFile);
	}

	SFile << SITEM::FILE_RANDOMBOX;
	{
		sRandomBox.SAVE (SFile);
	}

	SFile << SITEM::FILE_QUESTIONITEM;
	{
		sQuestionItem.SAVE (SFile);
	}

	SFile << SITEM::FILE_RANDOMITEM;
	{
		sRandomOpt.SAVE (SFile);
	}

	SFile << SITEM::FILE_PET;	// PetData
	{
		sPet.SAVE(SFile);
	}

	SFile << SITEM::FILE_VEHICLE;
	{
		sVehicle.SAVE(SFile);
	}

	SFile << SITEM::FILE_PETSKINPACK;
	{
		sPetSkinPack.SAVE(SFile);
	}

	SFile << SITEM::FILE_SPLIT;
	{
		sSplit.SAVE(SFile);
	}

	SFile << SITEM::FILE_SKILL;
	{
		sItemSkill.SAVE(SFile);
	}

	SFile << (DWORD)FILE_END_DATA;

	return TRUE;
}

BOOL SITEM::SaveFileLua(sc::TxtFile& LuaFile)
{
    LuaFile.write(sc::string::wformat(L"      {%1% --[[ SITEM Version ]],", VERSION), false);    
    LuaFile.write(sc::string::wformat(L" %1% --[[ Basic Enum ]],", FILE_SBASIC), false);
    LuaFile.write(sc::string::wformat(L" %1% --[[ Basic Version ]],", ITEM::SBASIC::VERSION), false);    
    
    // Basic
    sBasicOp.SaveLua(LuaFile);
/*
    LuaFile.write(L" --[[ Suit ]]");
    SFile << (DWORD)FILE_SSUIT;
    {
        SFile << DWORD(ITEM::SSUIT::VERSION);
        SFile << (DWORD)sizeof(ITEM::SSUIT);
        SFile.WriteBuffer ( &sSuitOp, sizeof(ITEM::SSUIT) );
    }

    LuaFile.write(                    L"      -- Drug");
    SFile << (DWORD)FILE_SDRUG;
    {
        SFile << DWORD(ITEM::SDRUG::VERSION);
        SFile << (DWORD)sizeof(ITEM::SDRUG);
        SFile.WriteBuffer ( &sDrugOp, sizeof(ITEM::SDRUG) );
    }

    LuaFile.write(                    L"      -- Skill Book");
    SFile << (DWORD)FILE_SSKILLBOOK;
    {
        SFile << DWORD(ITEM::SSKILLBOOK::VERSION);
        SFile << (DWORD)sizeof(ITEM::SSKILLBOOK);
        SFile.WriteBuffer ( &sSkillBookOp, sizeof(ITEM::SSKILLBOOK) );
    }

    LuaFile.write(                    L"      -- Generate");
    SFile << SITEM::FILE_SGENERATE;
    {
        SFile << DWORD(ITEM::SGENERATE::VERSION);
        SFile << (DWORD)sizeof(ITEM::SGENERATE);
        SFile.WriteBuffer ( &sGenerateOp, sizeof(ITEM::SGENERATE) );
    }

    LuaFile.write(                    L"      -- Grinding");
    SFile << SITEM::FILE_SGRINDING;
    {
        SFile << DWORD(ITEM::SGRINDING::VERSION);
        SFile << (DWORD)sizeof(ITEM::SGRINDING);
        SFile.WriteBuffer ( &sGrindingOp, sizeof(ITEM::SGRINDING) );
    }

    LuaFile.write(                    L"      -- Box");
    SFile << SITEM::FILE_BOX;
    {
        sBox.SAVE (SFile);
    }

    LuaFile.write(                    L"      -- Random Box");
    SFile << SITEM::FILE_RANDOMBOX;
    {
        sRandomBox.SAVE (SFile);
    }

    LuaFile.write(                    L"      -- Question Item");
    SFile << SITEM::FILE_QUESTIONITEM;
    {
        sQuestionItem.SAVE (SFile);
    }

    LuaFile.write(                    L"      -- Random Item");
    SFile << SITEM::FILE_RANDOMITEM;
    {
        sRandomOpt.SAVE (SFile);
    }

    LuaFile.write(                    L"      -- Pet");
    SFile << SITEM::FILE_PET;	// PetData
    {
        sPet.SAVE(SFile);
    }

    LuaFile.write(                    L"      -- Vehicle");
    SFile << SITEM::FILE_VEHICLE;
    {
        sVehicle.SAVE(SFile);
    }

    LuaFile.write(                    L"      -- Pet Skin Pack");
    SFile << SITEM::FILE_PETSKINPACK;
    {
        sPetSkinPack.SAVE(SFile);
    }
*/
    LuaFile.write(L" },", true);
    return TRUE;
}

BOOL SITEM::LoadFile(sc::BaseStream& SFile)
{
	DWORD dwVersion;
	SFile >> dwVersion;

	if (dwVersion > VERSION)
	{
        sc::writeLogError(
            sc::string::format(
                "SITEM::LoadFile Version mismatch %1% %2% != %3%",
                SFile.GetFileName(),
                dwVersion,
                VERSION));
		MessageBox(NULL, "Need New version Exe File!", "ERROR", MB_OK);
		return FALSE;
	}

	switch( dwVersion )
	{
	case 0x104:
		Load104(SFile);
		break;
	case 0x105:
	case 0x106:
	case VERSION:
		LoadCurrent(SFile);
		break;
	}

	// 로딩후 처리
	LoadAfter();

	return TRUE;
}

BOOL SITEM::Load104( sc::BaseStream& SFile )
{
	// sRandomOpt.NO_VERSION_LOAD(SFile); << 이 함수가 하나 바뀜으로써 문제가 됨..

	DWORD dwDataType;
	SFile >> dwDataType;
	while (dwDataType != SITEM::FILE_END_DATA)
	{
		switch (dwDataType)
		{
		case SITEM::FILE_SBASIC:
			LoadFileBasic(SFile);
			//if (!LoadFileBasic(SFile))
			//	return FALSE;
			break;

		case SITEM::FILE_SSUIT:
			LoadFileSuit(SFile);
			break;

		case SITEM::FILE_SDRUG:
			LoadFileDrug(SFile);
			break;

		case SITEM::FILE_SSKILLBOOK:
			LoadFileSkillBook(SFile);
			break;

		case SITEM::FILE_SGENERATE:
			LoadFileGenerate(SFile);
			break;

		case SITEM::FILE_SGRINDING:
			sGrindingOp.LOAD(SFile);
			break;

		case SITEM::FILE_BOX:
			sBox.LOAD(SFile);
			break;

		case SITEM::FILE_RANDOMBOX:
			sRandomBox.LOAD(SFile);
			break;

		case SITEM::FILE_QUESTIONITEM:
			sQuestionItem.LOAD(SFile);
			break;

		case SITEM::FILE_RANDOMITEM:
			sRandomOpt.NO_VERSION_LOAD(SFile);
			break;

		case SITEM::FILE_PET:	// PetData
			sPet.LOAD(SFile);
			break;

		case SITEM::FILE_VEHICLE:
			sVehicle.LOAD(SFile);
			break;

		case SITEM::FILE_PETSKINPACK:
			sPetSkinPack.LOAD(SFile);
			break;

		default:
			{
				MessageBox ( NULL, "SITEM::LoadFile(), unknown data version.", "ERROR", MB_OK );
				DWORD dwVer, dwSize;
				SFile >> dwVer;
				SFile >> dwSize;
				SFile.SetOffSet ( SFile.GetfTell()+dwSize );
			}
			break;
		};

		SFile >> dwDataType;
	};

	return TRUE;
}
/*
BOOL SITEM::Load105( sc::BaseStream& SFile )
{
	DWORD dwDataType;
	SFile >> dwDataType;
	while (dwDataType != SITEM::FILE_END_DATA)
	{
		switch (dwDataType)
		{
		case SITEM::FILE_SBASIC:
			LoadFileBasic(SFile);
			//if (!LoadFileBasic(SFile))
			//	return FALSE;
			break;

		case SITEM::FILE_SSUIT:
			LoadFileSuit(SFile);
			break;

		case SITEM::FILE_SDRUG:
			LoadFileDrug(SFile);
			break;

		case SITEM::FILE_SSKILLBOOK:
			LoadFileSkillBook(SFile);
			break;

		case SITEM::FILE_SGENERATE:
			LoadFileGenerate(SFile);
			break;

		case SITEM::FILE_SGRINDING:
			sGrindingOp.LOAD(SFile);
			break;

		case SITEM::FILE_BOX:
			sBox.LOAD(SFile);
			break;

		case SITEM::FILE_RANDOMBOX:
			sRandomBox.LOAD(SFile);
			break;

		case SITEM::FILE_QUESTIONITEM:
			sQuestionItem.LOAD(SFile);
			break;

		case SITEM::FILE_RANDOMITEM:
			sRandomOpt.LOAD(SFile);		// 이 함수에 버전을 읽는 부분이 추가됨
			break;

		case SITEM::FILE_PET:	// PetData
			sPet.LOAD(SFile);
			break;

		case SITEM::FILE_VEHICLE:
			sVehicle.LOAD(SFile);
			break;

		case SITEM::FILE_PETSKINPACK:
			sPetSkinPack.LOAD(SFile);
			break;

		default:
			{
				MessageBox ( NULL, "SITEM::LoadFile(), unknown data version.", "ERROR", MB_OK );
				DWORD dwVer, dwSize;
				SFile >> dwVer;
				SFile >> dwSize;
				SFile.SetOffSet ( SFile.GetfTell()+dwSize );
			}
			break;
		};

		SFile >> dwDataType;
	};

	return TRUE;
}
*/

BOOL SITEM::LoadCurrent( sc::BaseStream& SFile )
{
	DWORD dwDataType;
	SFile >> dwDataType;
	while (dwDataType != SITEM::FILE_END_DATA)
	{
		switch (dwDataType)
		{
		case SITEM::FILE_SBASIC:
			LoadFileBasic(SFile);
			//if (!LoadFileBasic(SFile))
			//	return FALSE;
			break;

		case SITEM::FILE_SSUIT:
			LoadFileSuit(SFile);
			break;

		case SITEM::FILE_SDRUG:
			LoadFileDrug(SFile);
			break;

		case SITEM::FILE_SSKILLBOOK:
			LoadFileSkillBook(SFile);
			break;

		case SITEM::FILE_SGENERATE:
			LoadFileGenerate(SFile);
			break;

		case SITEM::FILE_SGRINDING:
			sGrindingOp.LOAD(SFile);
			break;

		case SITEM::FILE_BOX:
			sBox.LOAD(SFile);
			break;

		case SITEM::FILE_RANDOMBOX:
			sRandomBox.LOAD(SFile);
			break;

		case SITEM::FILE_QUESTIONITEM:
			sQuestionItem.LOAD(SFile);
			break;

		case SITEM::FILE_RANDOMITEM:
			sRandomOpt.LOAD(SFile);
			break;

		case SITEM::FILE_PET:	// PetData
			sPet.LOAD(SFile);
			break;

		case SITEM::FILE_VEHICLE:
			sVehicle.LOAD(SFile);
			break;

		case SITEM::FILE_PETSKINPACK:
			sPetSkinPack.LOAD(SFile);
			break;

		case SITEM::FILE_SPLIT:
			sSplit.LOAD(SFile);
			break;
		case SITEM::FILE_SKILL :
			sItemSkill.LOAD(SFile);
			break;

		default:
			{
				MessageBox ( NULL, "SITEM::LoadFile(), unknown data version.", "ERROR", MB_OK );
				DWORD dwVer, dwSize;
				SFile >> dwVer;
				SFile >> dwSize;
				SFile.SetOffSet ( SFile.GetfTell()+dwSize );
			}
			break;
		};

		SFile >> dwDataType;
	};
	return TRUE;
}

VOID SITEM::SaveCsvHead(std::fstream& SFile)
{
	ITEM::SBASIC::SaveCsvHead(SFile);
	ITEM::SSUIT::SaveCsvHead(SFile);
	ITEM::SDRUG::SaveCsvHead(SFile);
	ITEM::SSKILLBOOK::SaveCsvHead(SFile);
	ITEM::SGRINDING::SaveCsvHead(SFile);
	ITEM::SGENERATE::SaveCsvHead(SFile);
	ITEM::SBOX::SaveCsvHead(SFile);
	ITEM::SRANDOMBOX::SaveCsvHead(SFile);
	ITEM::SQUESTIONITEM::SaveCsvHead(SFile);
	ITEM::SRANDOM_OPT::SaveCsvHead(SFile);
	ITEM::SPET::SaveCsvHead(SFile);	// PetData
	ITEM::SVEHICLE::SaveCsvHead(SFile);
	ITEM::SPETSKINPACKITEM::SaveCsvHead(SFile);
	ITEM::SSPLIT::SaveCsvHead(SFile);
	ITEM::SISKILL::SaveCsvHead(SFile);

	SFile << std::endl;
}

VOID SITEM::SaveCsv(std::fstream& SFile)
{
	sBasicOp.SaveCsv(SFile);
	sSuitOp.SaveCsv(SFile);
	sDrugOp.SaveCsv(SFile);
	sSkillBookOp.SaveCsv(SFile);
	sGrindingOp.SaveCsv(SFile);
	sGenerateOp.SaveCsv(SFile);
	sBox.SaveCsv(SFile);
	sRandomBox.SaveCsv(SFile);
	sQuestionItem.SaveCsv(SFile);
	sRandomOpt.SaveCsv(SFile);
	sPet.SaveCsv(SFile);				// PetData
	sVehicle.SaveCsv(SFile);
	sPetSkinPack.SaveCsv(SFile);
	sSplit.SaveCsv(SFile);
	sItemSkill.SaveCsv(SFile);

	SFile << std::endl;
}

namespace ITEM
{
    // Csv 로딩할 때 사용
    int iCsvCur = 0;
}

VOID SITEM::LoadCsv(CStringArray& StrArray)
{
    sBasicOp.LoadCsv(StrArray, ITEM::iCsvCur);
	sSuitOp.LoadCsv(StrArray, ITEM::iCsvCur);
	sDrugOp.LoadCsv(StrArray, ITEM::iCsvCur);
	sSkillBookOp.LoadCsv(StrArray, ITEM::iCsvCur);
	sGrindingOp.LoadCsv(StrArray, ITEM::iCsvCur);
	sGenerateOp.LoadCsv(StrArray, ITEM::iCsvCur);
    sBox.LoadCsv(StrArray, ITEM::iCsvCur);
	sRandomBox.LoadCsv(StrArray, ITEM::iCsvCur);
	sQuestionItem.LoadCsv(StrArray, ITEM::iCsvCur);
	sRandomOpt.LoadCsv(StrArray, ITEM::iCsvCur);
	sPet.LoadCsv(StrArray, ITEM::iCsvCur);	// PetData
	sVehicle.LoadCsv(StrArray, ITEM::iCsvCur);
	sPetSkinPack.LoadCsv( StrArray );
	sSplit.LoadCsv( StrArray, ITEM::iCsvCur );
	sItemSkill.LoadCsv(StrArray, ITEM::iCsvCur);

	// 로딩후 처리
	LoadAfter();
}

WORD SITEM::GETAPPLYNUM () const
{
	switch ( sBasicOp.emItemType )
	{
	case ITEM_ARROW:
	case ITEM_CHARM:
	case ITEM_BULLET:

	case ITEM_FIRECRACKER:
	case ITEM_LOUDSPEAKER:
	case ITEM_TICKET:
	case ITEM_RECALL:
	case ITEM_STORAGE_CONNECT:
	case ITEM_DISJUNCTION:
	case ITEM_2FRIEND:
	case ITEM_REVIVE: // 귀혼주일 경우
	case ITEM_ANTI_DISAPPEAR:
	case ITEM_INC_GRINDINGRATE:
	case ITEM_ANTI_RESET:
	case ITEM_REMODEL:
	case ITEM_GARBAGE_CARD:
	case ITEM_TELEPORT_CARD:
	case ITEM_PMARKET_SEARCH_CARD:
	case ITEM_REPAIR_CARD:
	case ITEM_STAMP_CARD:
	//case ITEM_STAGE_PASS:
	//case ITEM_ENTER_NUM_CHARGE:
		return sDrugOp.dwCureVolume;
	};

	//	귀혼주일 경우.
	if ( sDrugOp.emDrug == ITEM_DRUG_CALL_REVIVE && sBasicOp.emItemType==ITEM_SUIT )
	{
		return sDrugOp.dwCureVolume;
	}

	return 1;
}

SITEM& SITEM::operator = ( const SITEM& rvalue )
{
	if( this == &rvalue )
		return *this;

	strRealName_NO_SAVE = rvalue.strRealName_NO_SAVE.c_str();
	sBasicOp = rvalue.sBasicOp;
	sSuitOp = rvalue.sSuitOp;
	sDrugOp = rvalue.sDrugOp;
	sSkillBookOp = rvalue.sSkillBookOp;
	sGrindingOp = rvalue.sGrindingOp;
	sGenerateOp = rvalue.sGenerateOp;
	sBox = rvalue.sBox;
	sRandomBox = rvalue.sRandomBox;
	sQuestionItem = rvalue.sQuestionItem;
	sRandomOpt = rvalue.sRandomOpt;
	sPet = rvalue.sPet;
	sVehicle = rvalue.sVehicle;
	sPetSkinPack = rvalue.sPetSkinPack;
	sSplit = rvalue.sSplit;
	sItemSkill = rvalue.sItemSkill;

	return *this;
}

const char* SITEM::GetName() const
{
	if (GLOBAL_PARAM::bShowContentID)
		return sBasicOp.strName.c_str();

	return strRealName_NO_SAVE.c_str();
}

const std::string SITEM::GetNameStr() const
{
	if (GLOBAL_PARAM::bShowContentID)
		return sBasicOp.strName;

	return strRealName_NO_SAVE;
}

const char* SITEM::GetComment()
{
    if (GLOBAL_PARAM::bShowContentID)
        return sBasicOp.strComment.c_str();

	char* szDesc = NULL;
	if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
		szDesc = const_cast< char* >( CGameTextMan::GetInstance().GetText( sBasicOp.strComment, 0, CGameTextMan::EM_ITEM_TEXT ).GetString() );
	else
		szDesc = const_cast< char* >( GLStringTable::GetInstance().GetString( sBasicOp.strComment, GLStringTable::ITEM ) );

	if (szDesc == NULL || strcmp( szDesc, "Skip" ) == 0 || strcmp( szDesc, "skip" ) == 0 )
		return NULL;
	else
		return szDesc;
}

const bool SITEM::IsColorChange() const
{
	return sSuitOp.nItemSuitState[EMITEM_SUIT_CHANGE_COLOR];
}

const bool SITEM::IsUnique() const
{
	return sSuitOp.nItemSuitState[EMITEM_SUIT_UNIQUE];
}

const bool SITEM::IsBasicStatExist() const
{
	// 능력치가 한개라도 붙어 있으면 있는거다..
	if( sBasicOp.fExpMultiple > 1.f ) return true;
	if( sSuitOp.IsStatEnable()      ) return true;

	return false;
}

const bool SITEM::IsSuitable() const
{
    switch ( sBasicOp.emItemType )
    {
    case ITEM_SUIT:
    case ITEM_ARROW:
    case ITEM_CHARM:
    case ITEM_BULLET:
    case ITEM_VEHICLE:
    case ITEM_ANTI_DISAPPEAR:
    case ITEM_ANTI_RESET:
    case ITEM_INC_GRINDINGRATE:
    case ITEM_REVIVE:
	case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
        return true;
    };

    return false;
}

const bool SITEM::IsPreview() const
{
	if( ITEM_SUIT != sBasicOp.emItemType )
		return false;

	switch( sSuitOp.Suit() )
	{
	case SUIT_HEADGEAR :
	case SUIT_UPPER :
	case SUIT_LOWER :
	case SUIT_HAND :
	case SUIT_FOOT :
	case SUIT_HANDHELD :
		return true;
	}

	return false;
}

SITEM_LOBY_102::SITEM_LOBY_102()
	: sNativeID(false)
	, nidDISGUISE(false)

	, lnGenNum(0)
	, wTurnNum(1)
	, cGenType(EMGEN_DEFAULT)
	, cChnID(0)
	
	, cFieldID(0)
	
	, cDAMAGE(0)
	, cDEFENSE(0)
	, cRESIST_FIRE(0)
	
	, cRESIST_ICE(0)
	, cRESIST_ELEC(0)
	, cRESIST_POISON(0)
	, cRESIST_SPIRIT(0)

	, cOptTYPE1(0) // 공격력
	, cOptTYPE2(0) // 기저항
	, cOptTYPE3(0)
	, cOptTYPE4(0)

	, nOptVALUE1(0) // 공격력 %
	, nOptVALUE2(0) // 기저항 %
	, nOptVALUE3(0)
	, nOptVALUE4(0)

	, dwMainColor(0)
	, dwSubColor(0)
{
}

SITEM_LOBY_102::SITEM_LOBY_102(const SNATIVEID& sNID)
	: sNativeID(sNID)
	, nidDISGUISE(false)
	, lnGenNum(0)
	, wTurnNum(1)
	
	, cGenType(EMGEN_DEFAULT)
	, cChnID(0)
	
	, cFieldID(0)
	
	, cDAMAGE(0)
	, cDEFENSE(0)
	, cRESIST_FIRE(0)	
	, cRESIST_ICE(0)
	, cRESIST_ELEC(0)
	, cRESIST_POISON(0)
	, cRESIST_SPIRIT(0)

	, cOptTYPE1(0) // 공격력
	, cOptTYPE2(0) // 기저항
	, cOptTYPE3(0)
	, cOptTYPE4(0)

	, nOptVALUE1(0) // 공격력 %
	, nOptVALUE2(0) // 기저항 %
	, nOptVALUE3(0)
	, nOptVALUE4(0)

	, dwMainColor(0)
	, dwSubColor(0)
{
}

bool SITEM_LOBY_102::operator == (const SITEM_LOBY_102& value)
{
	return !memcmp(this,&value,sizeof(SITEM_LOBY_102));
}

bool SITEM_LOBY_102::operator != (const SITEM_LOBY_102& value)
{
	return 0!=memcmp(this,&value,sizeof(SITEM_LOBY_102));
}

void SITEM_LOBY_102::Assign(const SITEMCUSTOM& sItemCustom)
{
	sNativeID			= sItemCustom.GetNativeID();	
	nidDISGUISE			= sItemCustom.nidDISGUISE;

	//lnGenNum			= sItemCustom.lnGenNum;
	wTurnNum			= sItemCustom.wTurnNum;
	cGenType			= sItemCustom.cGenType;	
	cChnID				= 0; // sItemCustom.cChnID;
	cFieldID			= 0;
	//cFieldID			= sItemCustom.cFieldID;

	cDAMAGE				= sItemCustom.cDAMAGE;	
	cDEFENSE			= sItemCustom.cDEFENSE;	
	cRESIST_FIRE		= sItemCustom.cRESIST_FIRE;
	cRESIST_ICE			= sItemCustom.cRESIST_ICE;
	cRESIST_ELEC		= sItemCustom.cRESIST_ELEC;
	cRESIST_POISON		= sItemCustom.cRESIST_POISON;
	cRESIST_SPIRIT		= sItemCustom.cRESIST_SPIRIT;

	cOptTYPE1			= sItemCustom.randomOption.getType(0);
	cOptTYPE2			= sItemCustom.randomOption.getType(1);
	cOptTYPE3			= sItemCustom.randomOption.getType(2);
	cOptTYPE4			= sItemCustom.randomOption.getType(3);
	
	nOptVALUE1			= sItemCustom.randomOption.getValue(0);
	nOptVALUE2			= sItemCustom.randomOption.getValue(1);
	nOptVALUE3			= sItemCustom.randomOption.getValue(2);
	nOptVALUE4			= sItemCustom.randomOption.getValue(3);

	dwMainColor			= sItemCustom.dwMainColor;
	dwSubColor			= sItemCustom.dwSubColor;
};

SITEM_LOBY::SITEM_LOBY()
    : sNativeID(false)
    , nidDISGUISE(false)    
    , cDAMAGE(0)
    , cDEFENSE(0)
    , dwMainColor(0)
    , dwSubColor(0)
{
    MIN_STATIC_ASSERT(SITEM_LOBY::VERSION == 0x0103);
}

SITEM_LOBY::SITEM_LOBY(const SNATIVEID& sNID)
    : sNativeID(sNID)
    , nidDISGUISE(false)
    , cDAMAGE(0)
    , cDEFENSE(0)    
    , dwMainColor(0)
    , dwSubColor(0)
{
    MIN_STATIC_ASSERT(SITEM_LOBY::VERSION == 0x0103);
}

bool SITEM_LOBY::operator == (const SITEM_LOBY& value)
{
    MIN_STATIC_ASSERT(SITEM_LOBY::VERSION == 0x0103);
    return !memcmp(this,&value,sizeof(SITEM_LOBY));
}

bool SITEM_LOBY::operator != (const SITEM_LOBY& value)
{
    MIN_STATIC_ASSERT(SITEM_LOBY::VERSION == 0x0103);
    return 0!=memcmp(this,&value,sizeof(SITEM_LOBY));
}

void SITEM_LOBY::Assign(const SITEMCUSTOM& sItemCustom)
{
    MIN_STATIC_ASSERT(SITEM_LOBY::VERSION == 0x0103);
    sNativeID			= sItemCustom.GetNativeID();	
    nidDISGUISE			= sItemCustom.nidDISGUISE;
	//  [1/18/2013 gbgim]
	// - 인첸트 +계수에 따라 방어구 이펙트가 바뀌는데 기존에는 단순 +계수만 읽어와서 버그같이;
	//   보이는 문제가 생겨서 수정함
	// - 아이템 옵션중에 방어 연마, 공격 연마가 있는데 이 옵션같은 경우;
	//   인첸트 +1해준다 예를들어 +9 ITEM에 공격력 연마 1이 있으면 +10 ITEM이 된다;
	//! 인게임에서는 이렇게 사용하면안됨;
    cDAMAGE				= sItemCustom.cDAMAGE + static_cast<BYTE>( sItemCustom.GETOptVALUE_SHORT(RandomOption::RANDOM_OPTION_GRIND_DAMAGE) );	//sItemCustom.cDAMAGE
    cDEFENSE			= sItemCustom.cDEFENSE + static_cast<BYTE>( sItemCustom.GETOptVALUE_SHORT(RandomOption::RANDOM_OPTION_GRIND_DEFENCE) );//sItemCustom.cDEFENSE;	
    dwMainColor			= sItemCustom.dwMainColor;
    dwSubColor			= sItemCustom.dwSubColor;
}

BYTE SITEMCLIENT::GETGRADE_EFFECT () const
{
	BYTE cGRADE = GETGRADE();

	if ( cGRADE==0 )	return 0;

	// +9 등급 이하일 경우 
	if( cGRADE <= 9 )
	{
		return cGRADE = 1 + (cGRADE-1)/2;
	}
	
	return cGRADE = cGRADE - 4;

}


BOOL SETPUTONITEMS_BYBUF(SITEM_LOBY* pPutOnItems, se::ByteStream& ByteStream)
{
	if (ByteStream.IsEmpty())
        return TRUE;

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + PUTONITEMS_ARRAY[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	SITEMCUSTOM sItemCustom;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		if ( i>=SLOT_TSIZE )
            break;

		//	Note : 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
		//
		if ( dwVersion==0x0100 )
		{
			SITEMCUSTOM_100 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0101 )
		{
			SITEMCUSTOM_101 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0102 )
		{
			SITEMCUSTOM_102 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0103 )
		{
			SITEMCUSTOM_103 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0104 )
		{
			SITEMCUSTOM_104 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0105 )
		{
			SITEMCUSTOM_105 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0106 )
		{
			SITEMCUSTOM_106 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0107 )
		{
			SITEMCUSTOM_107 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0108 )
		{
			SITEMCUSTOM_108 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0109 )
		{
			SITEMCUSTOM_109 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0110 )
		{
			SITEMCUSTOM_110 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0111 )
		{
			SITEMCUSTOM_111 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0112 )
		{
			SITEMCUSTOM_112 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
        else if ( dwVersion==0x0113 )
        {
            SITEMCUSTOM_113 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            pPutOnItems[i].Assign ( sItemCustom );
        }
		else if ( dwVersion==0x0114 )
		{
			SITEMCUSTOM_114 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0115 )
		{
			SITEMCUSTOM_115 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
        else if ( dwVersion==0x0116 )
        {
            SITEMCUSTOM_116 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            pPutOnItems[i].Assign ( sItemCustom );
        }
		else if ( dwVersion==0x0117 )
		{
			SITEMCUSTOM_117 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0118 )
		{
			SITEMCUSTOM_118 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0119 )
		{
			SITEMCUSTOM_119 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0120 )
		{
			SITEMCUSTOM_120 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0121 )
		{
			SITEMCUSTOM_121 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0122 )
		{
			SITEMCUSTOM_122 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==SITEMCUSTOM::VERSION )
		{
			GASSERT(dwSize==sizeof(SITEMCUSTOM));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustom, sizeof(SITEMCUSTOM) );
			pPutOnItems[i].Assign ( sItemCustom );
		}
		else
		{
		}
	}

	return TRUE;
}

BOOL SETPUTONITEMS_BYBUF(std::vector<SITEM_LOBY>& vPutOnItems, se::ByteStream& ByteStream)
{
    if (ByteStream.IsEmpty())
        return TRUE;

    //	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + PUTONITEMS_ARRAY[SIZE]
    //
    DWORD dwVersion, dwSize, dwNum;

    ByteStream >> dwVersion;
    ByteStream >> dwSize;
    ByteStream >> dwNum;

    SITEMCUSTOM sItemCustom;
    for ( DWORD i=0; i<dwNum; i++ )
    {
        if ( i>=SLOT_TSIZE )
            break;

        //	Note : 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
        //
        if ( dwVersion==0x0100 )
        {
            SITEMCUSTOM_100 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0101 )
        {
            SITEMCUSTOM_101 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0102 )
        {
            SITEMCUSTOM_102 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0103 )
        {
            SITEMCUSTOM_103 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0104 )
        {
            SITEMCUSTOM_104 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0105 )
        {
            SITEMCUSTOM_105 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0106 )
        {
            SITEMCUSTOM_106 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0107 )
        {
            SITEMCUSTOM_107 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0108 )
        {
            SITEMCUSTOM_108 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0109 )
        {
            SITEMCUSTOM_109 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0110 )
        {
            SITEMCUSTOM_110 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0111 )
        {
            SITEMCUSTOM_111 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else if ( dwVersion==0x0112 )
        {
            SITEMCUSTOM_112 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
		else if ( dwVersion==0x0113 )
		{
			SITEMCUSTOM_113 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0114 )
		{
			SITEMCUSTOM_114 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0115 )
		{
			SITEMCUSTOM_115 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
        else if ( dwVersion==0x0116 )
        {
            SITEMCUSTOM_116 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            sItemCustom.Assign ( sItemCustomOld );
            vPutOnItems[i].Assign ( sItemCustom );
        }
		else if ( dwVersion==0x0117 )
		{
			SITEMCUSTOM_117 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0118 )
		{
			SITEMCUSTOM_118 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0119 )
		{
			SITEMCUSTOM_119 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0120 )
		{
			SITEMCUSTOM_120 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0121 )
		{
			SITEMCUSTOM_121 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0122 )
		{
			SITEMCUSTOM_122 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
        else if ( dwVersion==SITEMCUSTOM::VERSION )
        {
            GASSERT(dwSize==sizeof(SITEMCUSTOM));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustom, sizeof(SITEMCUSTOM) );
            vPutOnItems[i].Assign ( sItemCustom );
        }
        else
        {
        }
    }

    return TRUE;
}

BOOL SETPUTONITEMS_BYBUF_FOR_MIGRATION( std::vector< SITEM_LOBY >& vPutOnItems, se::ByteStream& ByteStream )
{
	if (ByteStream.IsEmpty())
		return TRUE;

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + PUTONITEMS_ARRAY[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	SITEMCUSTOM sItemCustom;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		if ( i>=SLOT_TSIZE )
			break;

		//	Note : 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
		//
		if ( dwVersion==0x0100 )
		{
			SITEMCUSTOM_100 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0101 )
		{
			SITEMCUSTOM_101 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0102 )
		{
			SITEMCUSTOM_102 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0103 )
		{
			SITEMCUSTOM_103 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0104 )
		{
			SITEMCUSTOM_104 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0105 )
		{
			SITEMCUSTOM_105 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0106 )
		{
			SITEMCUSTOM_106 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0107 )
		{
			SITEMCUSTOM_107 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0108 )
		{
			SITEMCUSTOM_108 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0109 )
		{
			SITEMCUSTOM_109 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0110 )
		{
			SITEMCUSTOM_110 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0111 )
		{
			SITEMCUSTOM_111 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0112 )
		{
			SITEMCUSTOM_112 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0113 )
		{
			SITEMCUSTOM_113 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0114 )
		{
			SITEMCUSTOM_114 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0115 )
		{
			SITEMCUSTOM_115 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0116 )
		{
			SITEMCUSTOM_116 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0117 )
		{
			SITEMCUSTOM_117 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0118 )
		{
			SITEMCUSTOM_118 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0119 )
		{
			SITEMCUSTOM_119 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0120 )
		{
			SITEMCUSTOM_120 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0121 )
		{
			SITEMCUSTOM_121 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==0x0122 )
		{
			SITEMCUSTOM_122 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			sItemCustom.Assign ( sItemCustomOld );
			vPutOnItems[i].Assign ( sItemCustom );
		}
		else if ( dwVersion==SITEMCUSTOM::VERSION )
		{
			// 아이템 버전이 올라가도 xxx_migration(MIGRATION) 함수들은 수정할 필요 없음
			GASSERT(dwSize==sizeof(SITEMCUSTOM));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustom, sizeof(SITEMCUSTOM) );

			SNATIVEID ItemID = sItemCustom.GetRealNativeID();
			ItemID.wMainID += 3000;
			sItemCustom.SetNativeID( ItemID );
			if ( NATIVEID_NULL() != sItemCustom.nidDISGUISE )
			{
				sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			vPutOnItems[i].Assign ( sItemCustom );
		}
		else
		{
		}
	}

	return TRUE;
}

BOOL SETPUTONITEMS_VECTOR_NEW( std::vector< SITEM_LOBY >& vPutOnItems, std::vector< SINVENITEM_SAVE >& vItems )
{
	size_t DbItemSize = vItems.size();
	for ( size_t loop = 0; loop < DbItemSize; loop++ )
	{
		SINVENITEM_SAVE& sItem = vItems[loop];
		if ( sItem.wPosX >= SLOT_TSIZE )
		{
			continue;
		}

		vPutOnItems[sItem.wPosX].Assign( sItem.sItemCustom );
	}

	return TRUE;
}

bool CItemDrop::IsTakeItem(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	bool bhold(false);

	switch ( emGroup )
	{
	case EMGROUP_ONE:
		if ( dwHoldGID==GAEAID_NULL )
			bhold = true;
		else if ( dwHoldGID == dwGaeaID )
			bhold = true;
		break;

	case EMGROUP_PARTY:
		if ( ((dwHoldGID == PARTY_NULL) || (PartyID == dwHoldGID)) && !bLock )
			bhold = true;
		break;

	case EMGROUP_GUILD:
		break;
	};

	// bLock == true 면 무조건 가져갈수 없도록...
 	//bool bResult = bLock ? bhold = false : bhold;

	//return bResult;
	return bhold;
}

bool CItemDrop::IsDropOut()
{
	return ( GLCONST_CHAR::wMAXITEM_AGE < fAge );
}

void CItemDrop::Update ( float fElapsedTime )
{
	fAge += fElapsedTime;

	//	Note : 임시 소유자 유효성 검사.
	//	
	if ( dwHoldGID != GAEAID_NULL )
	{
		if ( GLCONST_CHAR::wMAXITEM_HOLD < fAge )
		{
			switch ( emGroup )
			{
			case EMGROUP_ONE:				
				dwHoldGID = GAEAID_NULL;
				break;
			case EMGROUP_PARTY:		
				{
					if(!bLock) dwHoldGID = PARTY_NULL;
				}
				break;
			case EMGROUP_GUILD:
				break;
			};
		}
	}	
}


//////////////////////////////////////////////////////////////////////////
//						CItemClientDrop
CItemClientDrop::CItemClientDrop()
	: fAge(0.0f)
	, fNextMsgDelay(0.0f)
	, vMax(0,0,0)
	, vMin(0,0,0)
	, pSimMesh(NULL)
	, bCanGet(FALSE)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
{	
}

bool CItemClientDrop::IsCollision ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt ) const
{
	BOOL bCol = COLLISION::IsCollisionLineToAABB ( vFromPt, vTargetPt, vMax, vMin );

	return FALSE != bCol;
}

bool CItemClientDrop::IsCollision ( const CLIPVOLUME &cv ) const
{
	BOOL bCol = COLLISION::IsCollisionVolume ( cv, vMax, vMin );

	return FALSE != bCol;
}

void CItemClientDrop::FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fElapsedTime )
{
	if (fNextMsgDelay > 0.0f)
		fNextMsgDelay -= fElapsedTime;

	//////////////////////////////////////////////////////////////////////////
	//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spNameCollision->InitData( spLandMan );
	NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), vPos + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );

}

HRESULT CItemClientDrop::RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, D3DMATERIALQ* pMaterials )
{
    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( pSimMesh )
	{
		pSimMesh->RenderItem ( pd3dDevice, matWld, pMaterials );
	}

	return S_OK;
}

BOOL CItemClientDrop::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}


namespace COMMENT
{
	std::string ADDON_NO[ITEM::SSUIT::ADDON_SIZE] =
	{
		"0",
		"1",
		"2",
		"3",
	};
};
