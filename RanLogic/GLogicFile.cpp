#include "pch.h"
#include "../SigmaCore/Compress/Unzipper.h"
#include "../SigmaCore/Compress/ZipMan.h"
#include "../SigmaCore/Lua/MinLua.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/Util/SystemInfo.h"

#include "../EngineLib/G-Logic/GLogic.h"

#include "./GLogicFile.h"

#include <boost/ref.hpp>

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLogicFile::GLogicFile()
    : m_bLoad(false)
{
    m_CountryFileName = "country.luc";

//     strGLOGIC_ZIPFILE;
//     strQUEST_ZIPFILE;
//     strLEVEL_ZIPFILE;
//     strNPCTALK_ZIPFILE;


    m_PetCommentFileName        = "petcomment.lua";
    m_HideSetFileName			= "HideSet.lua";
    m_AttendanceFileName		= "attendance.ini"; // (구) 출석부입니다
    m_BusStationFileName		= "busstation.ini";
    m_ColorTableFileName		= "colortable.ini";
    m_CommentFileName			= "comment.lua";
    m_CrowFileName				= "crow.mnsf";
    m_DefaultCharClassFileName	= "default.charclass";
    m_ItemFileName				= "item.isf";
    m_ProductRecipeFileName		= "ItemMix.ims";
    m_PostFileName				= "Post.ini";
    m_UseFeaturesFileName		= "UseFeatures.lua";
    m_PointShopUiOptionFileName	= "PointShop.lua";
    m_AttendanceUiOptionFileName= "AttendanceUi.xml";
    m_QuestFileName				= "quest.lst";
    m_SkillFileName				= "skill.ssf";	
    m_SnsFileName				= "sns.ini";    
    m_TaxiStationFileName		= "taxistation.tsf";

    m_TransformSkinSetFileName	= "TransformSkinSet.lua";

	m_MiniGameOddEvenFileName = "MiniGame_OddEven.lua";

    m_SkillLearnPrereqFileName	= "SkillLearnPrereq.lua";

    m_CaptureTheFieldScriptName	= "CaptureTheField.nut";
    m_BuffManagerScriptName		= "BuffManager.nut";

    m_MatchSystemScriptName	= "MatchSystem.nut";

	m_GroupChatScriptName = "GroupChat.nut";

    m_AttendanceScriptName = "Attendance.nut";

    m_ControlTypeAFileName = "ControlTypeA.nut";
    m_ControlTypeBFileName = "ControlTypeB.nut";
	m_ControlTypeFlyCameraFileName = "ControlTypeFlyCamera.nut";

	m_SkillSpecExperiencePointFileName = "SkillSpecial_ExperiencePoint.lua";

	m_CountryCodeFileName = "GeoIP.dat";


	//mjeon.attendance
	//strAttendanceScriptName;
    
	//mjeon.activity
	//strRccPath_Activity;
	m_ActivityFileName = "activity.bin";

    // Npc Talk 통합 파일
    m_NewNpcTalkFileName = "gametalk.dat";

    // World battle 국가 정보 파일
    m_WorldBattleCountryFile = "wbCountry.lua";

    m_ClubDefineFile = "ClubDefine.lua";

    // map list file
    m_MapListFile = "mapslist.lua";
	m_MapListCompileFile = "mapslist.luc";
    m_MapListFileOld = "mapslist.mst";

	m_MapListWBFile = "mapslist_wb.lua";
	m_MapListWBCompileFile = "mapslist_wb.luc";

	// 개인상점 검색 시스템
	m_PrivateMarketFileName = "PrivateMarket.lua";

	// 트리거시스템 - 트리거리스트파일
	m_TriggerListFileName = "triggerlist.nut";

	//출석부 리뉴얼
	m_RnAttendanceFileName = "RnAttendance.lua";

	//! 인스턴스 스크립트;
	m_InstanceTableFileName = "instanceTable.lua";

	//! 미니게임 텍사스홀덤 설정파일
	m_MiniGameTexasHoldemFileName = "MiniGame_TexasHoldem.luc";

	//! Lotto System
	m_lottoSystemFilename = "LottoSystem.lua";

	m_SetItemListName = "SetItemList.luc";

	m_FSMContainerFileName = "FSMFileList.lua";

	m_ClonSkillPosFileName = "Clonskillpos.lua";

	m_RandomOptionServerFileName = "RandomOptionServer.rsf";
	m_RandomOptionClientFileName = "RandomOptionClient.rsf";
	m_RandomOptionFileName = "RandomOption.rsf";

	m_strRandomAddonOpFileName = "ItemRandomAddonList.lua";

	m_countrySystemFileName = "CountrySystem.lua";

	m_strCostumeStatFileName	= "Item_cos_option.luc";
	m_strSkillChangeFileName	= "Skill_ChangeSkillValue.luc";
	m_strItemPeriodExtensionFileName = "ItemPeriodExtension.luc";

    //! 확률제어 : sckim, Redmine #4544
    m_strRandomChanceFileName = "RandomChanceCtrlSys.lua";

	// 아이템 등급;
	m_strItemLevelFileName = "ItemLevel.lua";

	// 아이템 기본 랜덤 능력치;
	m_strItemBasicStatFileName = "ItemBasicStat.lua";

	// 아이템 링크 스킬;
	m_strItemLinkSkillFileName = "ItemLinkSkill.lua";

	// 캐릭터 슬롯;
	m_strCharacterSlotFileName = "CharacterSlot.lua";

	// 란 모바일
	m_strRanMobileFileName = "RanMobile.luc";
}

GLogicFile::~GLogicFile()
{
}

//! 국가별 파일설정을 읽는다
bool GLogicFile::LoadCountryFile()
{
    typedef std::map<std::string, boost::reference_wrapper<std::string> > MAPFILENAMES;
    MAPFILENAMES mapFilenames;
    {
//		mapFilenames.insert(std::make_pair("Comment", boost::ref(GLogicFile::m_CommentFileName)));
        mapFilenames.insert(std::make_pair("PetComment", boost::ref(GLogicFile::m_PetCommentFileName)));            
        mapFilenames.insert(std::make_pair("UseFeatures", boost::ref(GLogicFile::m_UseFeaturesFileName)));
		mapFilenames.insert(std::make_pair("PointShopUiOption", boost::ref(GLogicFile::m_PointShopUiOptionFileName)));
		mapFilenames.insert(std::make_pair("AttendanceUiOption", boost::ref(GLogicFile::m_AttendanceUiOptionFileName)));
		mapFilenames.insert(std::make_pair("ControlTypeA", boost::ref(GLogicFile::m_ControlTypeAFileName)));
		mapFilenames.insert(std::make_pair("ControlTypeB", boost::ref(GLogicFile::m_ControlTypeBFileName)));
		mapFilenames.insert(std::make_pair("ControlTypeFlyCamera", boost::ref(GLogicFile::m_ControlTypeFlyCameraFileName)));
        mapFilenames.insert(std::make_pair("WorldBattleCountry", boost::ref(GLogicFile::m_WorldBattleCountryFile)));
        mapFilenames.insert(std::make_pair("ClubDefine", boost::ref(GLogicFile::m_ClubDefineFile)));
		if ( GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
			mapFilenames.insert(std::make_pair("PrivateMarket", boost::ref(GLogicFile::m_PrivateMarketFileName)));
		if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
			mapFilenames.insert(std::make_pair("Rnattendance", boost::ref(GLogicFile::m_RnAttendanceFileName)));
    }

	typedef std::map<std::string, boost::reference_wrapper<std::string> > MAPSERVERFILES;
    MAPSERVERFILES mapServerFiles;
    {
        mapServerFiles.insert( std::make_pair("AttendanceScript", boost::ref(GLogicFile::m_AttendanceScriptName	) ) );
        mapServerFiles.insert( std::make_pair("CaptureTheFieldScript", boost::ref(GLogicFile::m_CaptureTheFieldScriptName	) ) );
        mapServerFiles.insert( std::make_pair("BuffManagerScript", boost::ref(GLogicFile::m_BuffManagerScriptName	) ) );
        mapServerFiles.insert( std::make_pair("MatchSystemScript", boost::ref(GLogicFile::m_MatchSystemScriptName )));
		mapServerFiles.insert( std::make_pair("GroupChatScript", boost::ref(GLogicFile::m_GroupChatScriptName )));
    }

    std::string CountryFile(sc::getAppPath());
    CountryFile.append("\\");
    CountryFile.append(m_CountryFileName);

    try
    {
        sc::lua_init();
        if (!sc::lua_doFile(CountryFile))
        {
            std::string ErrMsg(
                sc::string::format(
                    "%1% file missing.(error 3257)",
                    CountryFile));
            AfxMessageBox(ErrMsg.c_str(), MB_OK);
            return false;
        }

        // Service Provider
        LuaPlus::LuaObject ProviderIter = sc::lua_getGlobalFromName("ServiceProvider");
        for (LuaPlus::LuaTableIterator it(ProviderIter); it; it.Next())
        {
            std::string strKey((const char*) it.GetKey().GetString());
            if (strKey == "Country")
            {
                int ServiceProvider = it.GetValue().GetInteger();
                if (ServiceProvider < 0 || ServiceProvider >= SP_TOTAL_NUM)
                {
                    std::string Message(
                        sc::string::format(
                            "ERROR %1% ServiceProvider/Country unknown value %2%",
                            CountryFile,
                            ServiceProvider));
                    AfxMessageBox(Message.c_str(), MB_OK);
                }
                else
                {
                    ENGINE::emServiceProvider = EMSERVICE_PROVIDER(ServiceProvider);
                }
            }
        }

		//
		//GameLogic table
		//
        LuaPlus::LuaObject FileList = sc::lua_getGlobalFromName("GameLogicFile");
        for (LuaPlus::LuaTableIterator it(FileList); it; it.Next())
        {
            std::string  strKey   ((const char*) it.GetKey().GetString());
            std::wstring wstrFile ((wchar_t*) it.GetValue().GetWString());

            MAPFILENAMES::iterator findIter = mapFilenames.find(strKey); 
            if (findIter != mapFilenames.end())
            {
                std::string& ref_strFileName = findIter->second;
                ref_strFileName = sc::string::unicodeToAnsi(wstrFile);
            }
        }


		//
		//GameLogicServer table
		//
		FileList = sc::lua_getGlobalFromName("GameLogicServer");
        for (LuaPlus::LuaTableIterator it(FileList); it; it.Next())
        {
            std::string  strKey   ((const char*) it.GetKey().GetString());
            std::wstring wstrFile ((wchar_t*) it.GetValue().GetWString());

            MAPFILENAMES::iterator findIter = mapServerFiles.find( strKey ); 
            if ( findIter != mapServerFiles.end() )
            {
                std::string& ref_strFileName = findIter->second;
                ref_strFileName = sc::string::unicodeToAnsi(wstrFile);
            }
        }

        m_bLoad = true;
        return true;
	}
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
                "%1%, %2%",
                CountryFile,
                e.GetErrorMessage()));
        AfxMessageBox(ErrMsg.c_str(), MB_OK);
        return false;
    }
}

void GLogicFile::SetFullPath(BOOL bPack)
{
    if (bPack)
    {
        m_GlogicZipFile	 = "\\data\\glogic\\";
        m_QuestZipFile	 = "\\data\\glogic\\quest\\";
        m_LevelZipFile	 = "\\data\\glogic\\level\\";
        m_NpcTalkZipFile = "\\data\\glogic\\npctalk\\";

		m_RccPath_Activity = "\\data\\glogic\\activity\\";
    }
    else
    {
        m_GlogicZipFile = GLOGIC::GetPath();
        m_GlogicZipFile += "GLogic.rcc";

        m_QuestZipFile = GLOGIC::GetPath();
        m_QuestZipFile += "Quest\\Quest.rcc";

        m_LevelZipFile = GLOGIC::GetPath();
        m_LevelZipFile += "Level.rcc";

        m_NpcTalkZipFile = GLOGIC::GetPath();
        m_NpcTalkZipFile += "NpcTalk\\NpcTalk.rcc";

		m_RccPath_Activity = GLOGIC::GetPath();
		m_RccPath_Activity += "Activity\\Activity.rcc";

        /*
        CUnzipper::LOADFILE_RCC(strGLOGIC_ZIPFILE);
        CUnzipper::LOADFILE_RCC(strQUEST_ZIPFILE);
        CUnzipper::LOADFILE_RCC(strLEVEL_ZIPFILE);
        CUnzipper::LOADFILE_RCC(strNPCTALK_ZIPFILE);
		CUnzipper::LOADFILE_RCC(strRccPath_Activity);
        */
        ZipMan::GetInstance().LOADFILE_RCC(m_GlogicZipFile);
        ZipMan::GetInstance().LOADFILE_RCC(m_QuestZipFile);
        ZipMan::GetInstance().LOADFILE_RCC(m_LevelZipFile);
        ZipMan::GetInstance().LOADFILE_RCC(m_NpcTalkZipFile);
        ZipMan::GetInstance().LOADFILE_RCC(m_RccPath_Activity);
    }
}

RENDERPARAM::EMSERVICE_TYPE GLogicFile::GetServiceType(EMSERVICE_PROVIDER emType) const
{
    RENDERPARAM::EMSERVICE_TYPE ServiceType = RENDERPARAM::EMSERVICE_DEFAULT;
    switch (emType)
    {
    case SP_CHINA:
        ServiceType = RENDERPARAM::EMSERVICE_CHINA;
        break;
    case SP_THAILAND:
        ServiceType = RENDERPARAM::EMSERVICE_THAILAND;
        break;
    case SP_MALAYSIA:
        ServiceType = RENDERPARAM::EMSERVICE_MALAYSIA_CN;
        break;
    case SP_MALAYSIA_EN:
        ServiceType = RENDERPARAM::EMSERVICE_MALAYSIA_EN;
        break;
    case SP_INDONESIA:
        ServiceType = RENDERPARAM::EMSERVICE_INDONESIA;
        break;
    case SP_PHILIPPINES:
        ServiceType = RENDERPARAM::EMSERVICE_PHILIPPINES;
        break;
    case SP_VIETNAM:
        ServiceType = RENDERPARAM::EMSERVICE_VIETNAM;
        break;
    case SP_JAPAN:
        ServiceType = RENDERPARAM::EMSERVICE_JAPAN;
        break;
    case SP_TAIWAN:
        ServiceType = RENDERPARAM::EMSERVICE_FEYA;
        break;
    case SP_HONGKONG:
        ServiceType = RENDERPARAM::EMSERVICE_FEYA;
        break;
    case SP_OFFICE_TEST:
        ServiceType = RENDERPARAM::EMSERVICE_KOREA;
        break;
    case SP_KOREA:
        ServiceType = RENDERPARAM::EMSERVICE_KOREA;
        break;
    case SP_KOREA_TEST:
        ServiceType = RENDERPARAM::EMSERVICE_KOREA;
        break;
    case SP_GS:
        ServiceType = RENDERPARAM::EMSERVICE_GLOBAL;
        break;
    case SP_GLOBAL:
        ServiceType = RENDERPARAM::EMSERVICE_GLOBAL;
        break;
    case SP_WORLD_BATTLE:
        ServiceType = RENDERPARAM::EMSERVICE_WORLD_BATTLE;
        break;
	case SP_EU:
		ServiceType = RENDERPARAM::EMSERVICE_EU;
		break;
	case SP_US:
		ServiceType = RENDERPARAM::EMSERVICE_US;
		break;
    default:
        ServiceType = RENDERPARAM::EMSERVICE_DEFAULT;
    }
    return ServiceType;
}

EMSERVICE_PROVIDER GLogicFile::GetServiceProvider() const
{
    return ENGINE::emServiceProvider;
}
