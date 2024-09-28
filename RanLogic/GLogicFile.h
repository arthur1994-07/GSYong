#ifndef _GLOGIC_FILE_H_
#define _GLOGIC_FILE_H_

#include <string>
#include "../EngineLib/ServiceProviderDefine.h"
//#include "./RanParamDefine.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

class GLogicFile
{
public:
    GLogicFile();
    ~GLogicFile();

protected:
    bool m_bLoad; //! 로딩이 제대로 되었는가?

    std::string m_CountryFileName; //! 국가별 파일설정
    
    std::string m_GlogicZipFile;
    std::string m_QuestZipFile;
    std::string m_LevelZipFile;
    std::string m_NpcTalkZipFile;
    std::string m_PetCommentFileName;
    std::string m_HideSetFileName;
	std::string m_AttendanceFileName;
    std::string m_BusStationFileName;
    std::string m_ColorTableFileName;
    std::string m_CommentFileName;
    std::string m_CrowFileName;
    std::string m_DefaultCharClassFileName;
    std::string m_ItemFileName;
    std::string m_ProductRecipeFileName;
    std::string m_PostFileName;
    std::string m_UseFeaturesFileName;
    std::string m_PointShopUiOptionFileName;
    std::string m_AttendanceUiOptionFileName;
    std::string m_QuestFileName;
    std::string m_SkillFileName;
    std::string m_SnsFileName;
    std::string m_TaxiStationFileName;
	std::string m_NewNpcTalkFileName; //! Npc Talk 통합 파일
    std::string m_TransformSkinSetFileName;
	std::string m_MiniGameOddEvenFileName;
    std::string m_SkillLearnPrereqFileName;  //! 스킬창 개선;
	std::string m_SkillSpecExperiencePointFileName;
	std::string m_InstanceTableFileName;
	std::string m_MiniGameTexasHoldemFileName;
	std::string m_CountryCodeFileName;
	
	//mjeon.attendance
	std::string m_AttendanceScriptName;

	//mjeon.CaptureTheField
	std::string m_CaptureTheFieldScriptName;

	//mjeon.instance.match
	std::string m_MatchSystemScriptName;

	//mjeon.groupchat
	std::string m_GroupChatScriptName;

    //bjju.buffmanager
    std::string m_BuffManagerScriptName;

	//bjju.controltype;
	std::string m_ControlTypeAFileName;
	std::string m_ControlTypeBFileName;
	std::string m_ControlTypeFlyCameraFileName;

	//mjeon.activity
	std::string m_RccPath_Activity;
	std::string m_ActivityFileName;	

    //! World battle 2012-02-08 jgkim
    std::string m_WorldBattleCountryFile;

    //! Club Define 2012-03-12 jgkim
    std::string m_ClubDefineFile;

    //! map list file 2012-08-14 jgkim
    std::string m_MapListFile;
	std::string m_MapListCompileFile;
    std::string m_MapListFileOld; //! 과거 파일

	std::string m_MapListWBFile;
	std::string m_MapListWBCompileFile;

	//! 개인상점 검색시스템  file 2012-11-13 bjkim
	std::string m_PrivateMarketFileName;

	//! 트리거시스템 - 트리거리스트파일
	std::string m_TriggerListFileName;	

	//! 출석부 리뉴얼  file 2013-7-8 bjkim
	std::string m_RnAttendanceFileName;

	//! Lotto System
	std::string m_lottoSystemFilename;

	//세트아이템리스트
	std::string m_SetItemListName;

	// FSM Container;
	std::string m_FSMContainerFileName;

	// 복제효과 복제물 위치
	std::string m_ClonSkillPosFileName;

	// 랜덤 옵션 파일 이름;
	std::string m_RandomOptionServerFileName; // server;
	std::string m_RandomOptionClientFileName; // client;
	std::string m_RandomOptionFileName;

	std::string m_strRandomAddonOpFileName; // 랜던 가산 옵션 파일;

	//! Country System;
	std::string m_countrySystemFileName;

	std::string m_strCostumeStatFileName;
	std::string m_strSkillChangeFileName;
	std::string m_strItemPeriodExtensionFileName;

    //! 확률제어 : sckim, Redmine #4544
    std::string m_strRandomChanceFileName;

	// 아이템 등급;
	std::string m_strItemLevelFileName;

	// 아이템 기본 랜덤 능력치;
	std::string m_strItemBasicStatFileName;

	// 아이템 링크 스킬;
	std::string m_strItemLinkSkillFileName;

	// 캐릭터 슬롯;
	std::string m_strCharacterSlotFileName;

	// 란 모바일
	std::string m_strRanMobileFileName;

public:
    //! 로딩이 제대로 되었는가?
    bool IsLoad() const { return m_bLoad; }

    void SetFullPath(BOOL bPack);
    //mjeon.activity
//     std::string GetActivityFileName();
// 
//     std::string GetCommentFileName();
//     std::string GetItemFileName();
//     std::string GetCrowFileName();
//     std::string GetSkillFileName();
//     std::string GetQuestFileName();
//     std::string GetDefaultCharClassFileName();
//     std::string GetAttendanceFileName();
//     std::string GetPostFileName();
//     std::string GetSNSFileName();
//     std::string GetColorTableFileName();
//     std::string GetBusStationFileName();
//     std::string GetTaxiStationFileName();
//     std::string GetItemMixFileName();
// 
//     std::string GetUseFeaturesFileName();
//     std::string GetPointShopUiOptionFileName();
//     std::string GetAttendanceUiOptionFileName();
// 
//     //mjeon.attendance
//     std::string GetAttendanceScriptName();

    EMSERVICE_PROVIDER GetServiceProvider() const;

	RENDERPARAM::EMSERVICE_TYPE GetServiceType(EMSERVICE_PROVIDER emType) const;

    //! 국가별 파일설정을 읽는다
    bool LoadCountryFile();

    std::string GetHideSetFileName() const { return m_HideSetFileName; }	
	std::string GetGlogicZipFile() const { return m_GlogicZipFile; }
    std::string GetQuestZipFile() const { return m_QuestZipFile; }
    std::string GetLevelZipFile() const { return m_LevelZipFile; }
    std::string GetNpcTalkZipFile() const { return m_NpcTalkZipFile; }

    //! Npc Talk 통합 파일
    std::string GetNewNpcTalkFileName() const { return m_NewNpcTalkFileName; }

    //mjeon.activity
    std::string GetActivityFileName() const { return m_ActivityFileName; }
    std::string GetPetCommentFileName() const { return m_PetCommentFileName; }
    std::string GetCommentFileName() const { return m_CommentFileName; }
    std::string GetItemFileName() const { return m_ItemFileName; }
    std::string GetCrowFileName() const { return m_CrowFileName; }
    std::string GetSkillFileName() const { return m_SkillFileName; }
    std::string GetQuestFileName() const { return m_QuestFileName; }
    std::string GetDefaultCharClassFileName() const { return m_DefaultCharClassFileName; }
    std::string GetAttendanceFileName() const { return m_AttendanceFileName; } // (구) 출석부
    std::string GetPostFileName() const { return m_PostFileName; }
    std::string GetSNSFileName() const { return m_SnsFileName; }
    std::string GetColorTableFileName() const { return m_ColorTableFileName; }
    std::string GetBusStationFileName() const { return m_BusStationFileName; }
    std::string GetTaxiStationFileName() const { return m_TaxiStationFileName; }
    std::string GetProductRecipeFileName() const { return m_ProductRecipeFileName; }
    std::string GetUseFeaturesFileName() const { return m_UseFeaturesFileName; }
    std::string GetPointShopUiOptionFileName() const { return m_PointShopUiOptionFileName; }
    std::string GetAttendanceUiOptionFileName() const { return m_AttendanceUiOptionFileName; }
	std::string GetControlTypeAFileName() const { return m_ControlTypeAFileName; }
	std::string GetControlTypeBFileName() const { return m_ControlTypeBFileName; }    
	std::string GetControlTypeFlyCameraFileName() const { return m_ControlTypeFlyCameraFileName; }    
    std::string GetTransformSkinSetFileName() const { return m_TransformSkinSetFileName; }
	std::string GetMiniGameOddEvenFileName() const { return m_MiniGameOddEvenFileName; }
	std::string GetSkillSpecExperiencePointFileName() const { return m_SkillSpecExperiencePointFileName; }
	std::string GetCountryCodeFileName() const { return m_CountryCodeFileName; }
	std::string GetRandomOptionServerFileName() const { return m_RandomOptionServerFileName; }
	std::string GetRandomOptionClientFileName() const { return m_RandomOptionClientFileName; }
	std::string GetRandomOptionFileName() const { return m_RandomOptionFileName; }
	std::string GetRandomAddonOpFileName() const { return m_strRandomAddonOpFileName; }

    //mjeon.attendance
    std::string GetAttendanceScriptName() const { return m_AttendanceScriptName; }

	//mjeon.CaptureTheField
	std::string GetCaptureTheFieldScriptName() const { return m_CaptureTheFieldScriptName; }

    //bjju.buffmanager    
    std::string GetBuffManagerScriptName() const { return m_BuffManagerScriptName; }    

    std::string GetActivityRccPath() const { return m_RccPath_Activity; }

	//mjeon.instance.match
	std::string GetMatchSystemScriptName() const { return m_MatchSystemScriptName; }

	std::string GetGroupChatScriptName() const { return m_GroupChatScriptName; }	

    //! World battle 2012-02-08 jgkim
    std::string GetWorldBattleCountryFile() const { return m_WorldBattleCountryFile; }
	
    //! Club Define 2012-03-12 jgkim
    std::string GetClubDefineFile() const { return m_ClubDefineFile; }

    std::string GetSkillLearnPrereqFileName() const { return m_SkillLearnPrereqFileName; }

    //! map list file 2012-08-14 jgkim
    std::string GetMapListFile() const { return m_MapListFile; }
	std::string GetMapListCompileFile() const { return m_MapListCompileFile; }
    std::string GetMapListFileOld() const { return m_MapListFileOld; }

	std::string GetMapListWBFile() const { return m_MapListWBFile; }
	std::string GetMapListWBCompileFile() const { return m_MapListWBCompileFile; }

	//! 개인상점검색시스템
	std::string GetPrivateMarketFileName() const { return m_PrivateMarketFileName; }	

	//! 트리거시스템 - 트리거리스트파일
	std::string GetTriggerListFileName() const { return m_TriggerListFileName; }

	//! 출석부 리뉴얼
	std::string GetRnAttendanceFileName() const { return m_RnAttendanceFileName; }
	
	//! 인스턴스 스크립트;
	std::string GetInstanceTableFileName() const { return m_InstanceTableFileName; }

	//! 미니게임 텍사스홀덤
	std::string GetMiniGameTexasHoldemFileName() const { return m_MiniGameTexasHoldemFileName; }

	//! Lotto System;
	std::string GetLottoSystemFilename() const { return m_lottoSystemFilename; }
	
	//세트아이템 리스트
	std::string GetSetListFileName() const {return m_SetItemListName;}

	// FSM Container;
	std::string GetFSMContainerFileName() const { return m_FSMContainerFileName; }

	std::string GetClonSkillPosFilename() const { return m_ClonSkillPosFileName; }

	//! Country System;
	std::string GetCountrySystemFileName() const { return m_countrySystemFileName; }

	std::string GetCostumeStatFileName() const { return m_strCostumeStatFileName; }
	std::string GetSkillChangeFileName() const { return m_strSkillChangeFileName; }
	std::string GetItemPeriodExtensionFileName() const { return m_strItemPeriodExtensionFileName; }

    //! 확률제어 : sckim, Redmine #4544
    std::string GetRandomChanceFileName() const { return m_strRandomChanceFileName; }

	// 아이템 등급;
	std::string GetItemLevelFileName() const { return m_strItemLevelFileName; }

	// 아이템 기본 랜덤 능력치;
	std::string GetItemBasicStatFileName() const { return m_strItemBasicStatFileName; }

	// 아이템 링크 스킬;
	std::string GetItemLinkSkillFileName() const { return m_strItemLinkSkillFileName; }

	// 캐릭터 슬롯;
	std::string GetCharacterSlotFileName() const { return m_strCharacterSlotFileName; }

	// 란 모바일
	std::string GetRanMobileFileName() const { return m_strRanMobileFileName; }
};

#endif // _GLOGIC_FILE_H_
