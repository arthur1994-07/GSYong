#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/GLTexFile.h"

#include "../GLogicData.h"
#include "../Quest/GLQuest.h"
#include "../Quest/GLQuestMan.h"
#include "./GLCharData.h"

#include "../Skill/GLSkillChangeList.h"
// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL SCHARDATA2::LOADFILE(const std::string& FileName)
{
	if (FileName.empty())
        return FALSE;

	std::string strPath(GLOGIC::GetPath());
	strPath.append(FileName);

	gltexfile cFILE;
	cFILE.reg_sep("\t ,{}[]()|");

	if (GLOGIC::UseLogicZipFile()) // by 경대
        cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if (!cFILE.open(strPath, true, GLOGIC::UseLogicPackFile()))
	{
        std::string ErrMsg(sc::string::format("Check! SCHARDATA2::LOADFILE %1%", strPath));
        if (GLOGIC::UseLogicZipFile())
        {
            ErrMsg.append(" ");
            ErrMsg.append(GLogicData::GetInstance().GetGlogicZipFile());
        }
        sc::writeLogError(ErrMsg);
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag( "VERSION", 1, 1, dwVERSION, true );

	cFILE.getflag( "emTribe", 1, 1, m_emTribe, true );
	cFILE.getflag( "emClass", 1, 1, m_emClass, true );
	cFILE.getflag( "wSchool", 1, 1, m_wSchool, true );
	cFILE.getflag( "wSex", 1, 1, m_wSex, true );
	cFILE.getflag( "wHair", 1, 1, m_wHair, true );
	cFILE.getflag( "wHairColor", 1, 1, m_wHairColor, true );
	cFILE.getflag( "wFace", 1, 1, m_wFace, true );
	
	cFILE.getflag( "nBright", 1, 1, m_nBright, true );
	cFILE.getflag( "wLevel" , 1, 1, m_wLevel, true );
	cFILE.getflag( "wGuild" , 1, 1, m_ClubDbNum, true );
	cFILE.getflag( "lnMoney", 1, 1, m_lnMoney, true );

	cFILE.getflag( "sStats", 1, 6, m_sStats.wPow, true );
	cFILE.getflag( "sStats", 2, 6, m_sStats.wStr, true );
	cFILE.getflag( "sStats", 3, 6, m_sStats.wSpi, true );
	cFILE.getflag( "sStats", 4, 6, m_sStats.wDex, true );
	cFILE.getflag( "sStats", 5, 6, m_sStats.wInt, true );
	cFILE.getflag( "sStats", 6, 6, m_sStats.wSta, true );
	
	cFILE.getflag( "wStatsPoint", 1, 1, m_wStatsPoint, true );
	cFILE.getflag( "wAP", 1, 1, m_powerAttack, true );
	cFILE.getflag( "wDP", 1, 1, m_powerDefence, true );
	cFILE.getflag( "wPA", 1, 1, m_powerAttribute[SKILL::EMAPPLY_MELEE], true );
	cFILE.getflag( "wSA", 1, 1, m_powerAttribute[SKILL::EMAPPLY_RANGE], true );

	cFILE.getflag( "sExperience", 1, 2, m_sExperience.lnNow, true );
	cFILE.getflag( "sExperience", 2, 2, m_sExperience.lnMax, true );

	cFILE.getflag( "dwSkillPoint", 1, 1, m_dwSkillPoint, true );
	
	cFILE.getflag( "sHP", 1, 2, m_sHP.nNow, true );
	cFILE.getflag( "sHP", 2, 2, m_sHP.nMax, true );

	cFILE.getflag( "sMP", 1, 2, m_sMP.nNow, true );
	cFILE.getflag( "sMP", 2, 2, m_sMP.nMax, true );

	cFILE.getflag( "sSP", 1, 2, m_sSP.nNow, true );
	cFILE.getflag( "sSP", 2, 2, m_sSP.nMax, true );

	cFILE.getflag( "wPK", 1, 1, m_wPK, true );

	cFILE.getflag( "sStartMapID", 1, 2, m_sStartMapID.wMainID, true );
	cFILE.getflag( "sStartMapID", 2, 2, m_sStartMapID.wSubID, true );

	cFILE.getflag( "dwStartGate", 1, 1, m_dwStartGate, true );

	cFILE.getflag( "vStartPos", 1, 3, m_vStartPos.x, true );
	cFILE.getflag( "vStartPos", 2, 3, m_vStartPos.y, true );
	cFILE.getflag( "vStartPos", 3, 3, m_vStartPos.z, true );

	m_ExpSkills.clear();
	DWORD dwNUM = cFILE.getflagnum ( "ExpSkills" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SCHARSKILL CharSkill;
		cFILE.getflag( i, "ExpSkills", 1, 3, CharSkill.sNativeID.wMainID, true );
		cFILE.getflag( i, "ExpSkills", 2, 3, CharSkill.sNativeID.wSubID, true );
		cFILE.getflag( i, "ExpSkills", 3, 3, CharSkill.wLevel, true );

		m_ExpSkills[CharSkill.sNativeID.dwID] = CharSkill;
	}

	for ( int i=0; i<EMSKILLQUICK_SIZE; ++i)	m_sSKILLQUICK[i] = NATIVEID_NULL();

	dwNUM = cFILE.getflagnum ( "QuickSkill" );
	if ( dwNUM > EMSKILLQUICK_SIZE ) dwNUM = EMSKILLQUICK_SIZE;

	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SNATIVEID skillID;
		cFILE.getflag( i, "QuickSkill", 1, 2, skillID.wMainID, true );
		cFILE.getflag( i, "QuickSkill", 2, 2, skillID.wSubID, true );

		m_sSKILLQUICK[i] = skillID;
	}

	m_cQuestPlay.DeleteAll();
	dwNUM = cFILE.getflagnum ( "Quest" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		DWORD dwNID;
		cFILE.getflag( i, "Quest", 1, 1, dwNID, true );

		//	Note : 퀘스트 시작을 위한 검사.
		//
		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwNID );
		if (!pQUEST)
        {
            SNATIVEID QuestId(dwNID);
            sc::writeLogError(
                sc::string::format(
                    "%1% Quest (%2%/%3%) not exist",
                    FileName,
                    QuestId.Mid(),
                    QuestId.Sid()));
            continue;
        }

		//	Note : 새로이 시작되는 퀘스트 설정.
		//
		GLQUESTPROG sQUEST_PROG_NEW;
		sQUEST_PROG_NEW.START(pQUEST, NULL);

		//	리스트에 등록.
		m_cQuestPlay.InsertProc(sQUEST_PROG_NEW, false);
	}

	char* strBodyPart[SLOT_NSIZE_S_2] =
	{
		"HEAD",	
		"UPPER", 
		"LOWER", 
		"HAND", 
		"FOOT", 

		"RHAND", 
		"LHAND", 
		"NECK", 
		"WRIST", 
		"RFINGER", 

		"LFINGER",
		"RHAND_S", 
		"LHAND_S",
		"VEHICLE",
        "",

        "",
        "",
        "",
        "",
	};
	
	for ( int i=0; i<SLOT_TSIZE; ++i )
		m_PutOnItems[i] = SITEMCUSTOM( NATIVEID_NULL() );
	
	dwNUM = cFILE.getflagnum ( "PutOnItems" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		std::string strArg;
		cFILE.getflag( i, "PutOnItems", 1, 11, strArg, true );
		int BodyPartNum = -1;

		for ( int j = 0; j < SLOT_NSIZE_S_2; j++ )
		{
			if ( strArg == strBodyPart[j] )
			{		
				BodyPartNum = j;
				break;
			}
		}

		if ( BodyPartNum != -1 )
		{
            WORD wMID = 0, wSID = 0;
			cFILE.getflag( i, "PutOnItems", 2, 11, wMID, true );
			cFILE.getflag( i, "PutOnItems", 3, 11, wSID, true );
            
            SITEMCUSTOM ItemCustom( SNATIVEID(wMID, wSID) );
			cFILE.getflag( i, "PutOnItems", 4, 11, ItemCustom.wTurnNum, true );
			cFILE.getflag( i, "PutOnItems", 5, 11, ItemCustom.cDAMAGE, true );
			cFILE.getflag( i, "PutOnItems", 6, 11, ItemCustom.cDEFENSE, true );
			cFILE.getflag( i, "PutOnItems", 7, 11, ItemCustom.cRESIST_FIRE, true );
			cFILE.getflag( i, "PutOnItems", 8, 11, ItemCustom.cRESIST_ICE, true );	
			cFILE.getflag( i, "PutOnItems", 9, 11, ItemCustom.cRESIST_ELEC, true );
			cFILE.getflag( i, "PutOnItems", 10, 11, ItemCustom.cRESIST_POISON, true );
			cFILE.getflag( i, "PutOnItems", 11, 11, ItemCustom.cRESIST_SPIRIT, true );

			//	Note : 아이템 생성 TYPE을 초기 부여로 지정.
			//
			ItemCustom.cGenType = EMGEN_INIT;

			m_PutOnItems[BodyPartNum] = ItemCustom;
		}
	}

	m_cInventory.DeleteItemAll ();
	dwNUM = cFILE.getflagnum ( "cInventory" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
        WORD wMID = 0, wSID = 0;
		cFILE.getflag( i, "cInventory", 1, 10, wMID, true );
		cFILE.getflag( i, "cInventory", 2, 10, wSID, true );

        SITEMCUSTOM ItemCustom( SNATIVEID(wMID, wSID) );
		cFILE.getflag( i, "cInventory", 3, 10, ItemCustom.wTurnNum, true );
		cFILE.getflag( i, "cInventory", 4, 10, ItemCustom.cDAMAGE, true );
		cFILE.getflag( i, "cInventory", 5, 10, ItemCustom.cDEFENSE, true );
		cFILE.getflag( i, "cInventory", 6, 10, ItemCustom.cRESIST_FIRE, true );
		cFILE.getflag( i, "cInventory", 7, 10, ItemCustom.cRESIST_ICE, true );
		cFILE.getflag( i, "cInventory", 8, 10, ItemCustom.cRESIST_ELEC, true );
		cFILE.getflag( i, "cInventory", 9, 10, ItemCustom.cRESIST_POISON, true );
		cFILE.getflag( i, "cInventory", 10, 10, ItemCustom.cRESIST_SPIRIT, true );
											   
		//	Note : 아이템 생성 TYPE을 초기 부여로 지정.
		//									   
		ItemCustom.cGenType = EMGEN_INIT;

		m_cInventory.InsertItem ( ItemCustom );
	}

	for ( int i=0; i<EMACTIONQUICK_SIZE; ++i)	m_sACTIONQUICK[i].sNID = NATIVEID_NULL();

	dwNUM = cFILE.getflagnum ( "QuickAction" );
	if ( dwNUM > EMACTIONQUICK_SIZE ) dwNUM = EMACTIONQUICK_SIZE;

	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SACTION_SLOT sItemSlot;
		sItemSlot.wACT = EMACT_SLOT_DRUG;
		cFILE.getflag( i, "QuickAction", 1, 2, sItemSlot.sNID.wMainID, true );
		cFILE.getflag( i, "QuickAction", 2, 2, sItemSlot.sNID.wSubID, true );

		m_sACTIONQUICK[i] = sItemSlot;
	}

	return TRUE;
}
