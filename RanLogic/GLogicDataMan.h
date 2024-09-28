#ifndef _GLOGIC_DATA_MAN_H_
#define _GLOGIC_DATA_MAN_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/function.hpp>
#include <string>
#include <vector>
#include "../EngineLib/ServiceProviderDefine.h"
#include "../EngineLib/G-Logic/TypeDefine.h"

namespace sc
{
    class Excel;
}

namespace RENDERPARAM
{
    enum EMSERVICE_TYPE;
}

//! World battle
namespace wb
{
    class Country;
    struct COUNTRY_INFO;
    struct TEXTURE_POSITION;
    enum EM_COUNTRY;
    struct FLAG_TEXTURE_POSITION;
}

namespace club
{
    class ClubDefineData;
    struct AUTH_GRADE;
}

class NpcTalkMan;
class GLRnSkillLearnSetting;
struct SLEARN_SETTING;
struct SITEM;
class SUBPATH;
class GLogicFile;
struct GLCLUBRANK;
struct GLSKILL;

class GLogicData
{
protected:
    GLogicData();

public:
    ~GLogicData();
    static GLogicData& GetInstance();

private:
    SUBPATH* m_pPath;
    std::tr1::shared_ptr<NpcTalkMan> m_spNpcTalkMan;
    std::tr1::shared_ptr<GLRnSkillLearnSetting> m_spRnSkillLearnSetting;
    GLogicFile* m_pLogicFile;
    wb::Country* m_pWbCountry; //! World battle country info
    club::ClubDefineData* m_pClubBaseData;

    static const unsigned int LIMIT_POINT_DIGIT = 100000;

public:
    HRESULT LoadData(
        BOOL bServer,
        SUBPATH* pPath,
        bool bToolMode,
        boost::function<void (std::string)> PercentCallbackFunc=NULL,		
		boost::function<void (unsigned int)> PercentCallbackFunc2=NULL);
    HRESULT ClearData();
	void	ReleaseInstance();
    HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
    HRESULT DeleteDeviceObjects();
    SUBPATH* GetPath() { return m_pPath; }

    //! -----------------------------------------------------------------------
    //! NpcTalk
    std::tr1::shared_ptr<NpcTalkMan> GetNpcTalkMan() { return m_spNpcTalkMan; }
    bool SaveNpcTalkExcel(std::string& SaveFileName OUT, boost::function<void (int)> PercentCallbackFunc=NULL);
    std::string GetNewNpcTalkFileName() const;

    //! GLRnSkillLearnSetting
    const DWORD GetRnSkillUseMoney( const SNATIVEID& sNativeID );
    const bool GetRnSkillUseScrool( const SNATIVEID& sNativeID );

    //! -----------------------------------------------------------------------
    //! Quest
    const std::string& GetQuestName(const SNATIVEID& QuestId);
    const std::string& GetQuestName(WORD MainId, WORD SubId);
    const std::string& GetQuestName(DWORD QuestId);
    void SaveQuestDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName);
    bool IsExistQuest(const SNATIVEID& QuestId);
    bool IsExistQuest(DWORD QuestId);

    //! -----------------------------------------------------------------------
    //! Item
    const std::string& GetItemName(const SNATIVEID& sNativeID);
    const std::string& GetItemName(WORD MainId, WORD SubId);
    const std::string& GetItemName(DWORD ItemId);

    SITEM* GetItem(WORD wMID, WORD wSID);
    SITEM* GetItem(const SNATIVEID& sNativeID);
    SITEM* GetItem(DWORD ItemId);

    void SaveItemDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName);

    //! -----------------------------------------------------------------------
    size_t GetItemMaxMid() const;
    size_t GetItemMaxSid() const;

    //! Skill
    const std::string GetSkillName(const SNATIVEID& SkillId) const;
    const std::string GetSkillName(WORD MainId, WORD SubId) const;
    const std::string GetSkillName(DWORD SkillId) const;
    GLSKILL* GetSkillData(const SNATIVEID& SkillId) const;
    void SaveSkillDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName);

    //! -----------------------------------------------------------------------
    //! NPC/Crow
    const std::string GetCrowName(WORD wMID, WORD wSID);
    const std::string GetCrowName(SNATIVEID sNativeID);
    const std::string GetCrowName(DWORD NpcId);
    void SaveCrowDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName);

    //! -----------------------------------------------------------------------
    //! GLogicFile
    bool LoadCountryFile();
    EMSERVICE_PROVIDER GetServiceProvider() const;
    RENDERPARAM::EMSERVICE_TYPE GetServiceType(EMSERVICE_PROVIDER emType) const;

    void SetLogicFileFullPath(BOOL bPack);

    std::string GetGlogicZipFile() const;
    std::string GetQuestZipFile() const;
    std::string GetLevelZipFile() const;
    std::string GetNpcTalkZipFile() const;

    std::string GetCommentFileName() const;
    std::string GetItemFileName() const;
    std::string GetCrowFileName() const;
    std::string GetSkillFileName() const;
    std::string GetQuestFileName() const;
    std::string GetDefaultCharClassFileName() const;
    std::string GetAttendanceFileName() const;
    std::string GetPostFileName() const;
    std::string GetSNSFileName() const;
    std::string GetColorTableFileName() const;
    std::string GetBusStationFileName() const;
    std::string GetTaxiStationFileName() const;
    std::string GetItemMixFileName() const;
    std::string GetUseFeaturesFileName() const;
    std::string GetPointShopUiOptionFileName() const;
    std::string GetAttendanceUiOptionFileName() const;
    std::string GetControlTypeAFileName() const;
    std::string GetControlTypeBFileName() const;
	std::string GetControlTypeFlyCameraFileName() const;

    std::string GetAttendanceScriptName() const;
    std::string GetCaptureTheFieldScriptName() const;
    std::string GetBuffManagerScriptName() const;
    std::string GetActivityRccPath() const;
    std::string GetMatchSystemScriptName() const;
	std::string GetGroupChatScriptName() const;
    std::string GetMapListFileName() const;
	std::string GetMapListCompileFileName() const;
    std::string GetMapListFileNameOld() const;
	std::string GetMapListWBFileName() const;
	std::string GetMapListWBCompileFileName() const;

	std::string GetPrivateMarketFileName() const;

	std::string GetTriggerListFileName() const;

	std::string GetRnAttendanceFileName() const;

	std::string GetLottoSystemFileName() const;

	std::string GetSetItemListFileName() const;

	std::string GetFSMContainerFileName() const;

	std::string GetClonSkillPosFileName() const;

	std::string GetCountrySystemFileName() const;

	std::string GetCostumeStatFileName() const;

	std::string GetSkillChangeFileName() const;

    //! 확률제어 : sckim, Redmine #4544
    std::string GetRandomChanceFileName() const;

	// 아이템 등급;
	std::string GetItemLevelFileName() const;

	// 아이템 기본 랜덤 능력치;
	std::string GetItemBasicStatFileName() const;

	// 아이템 링크 스킬;
	std::string GetItemLinkSkillFileName() const;

	// 캐릭터 슬롯;
	std::string GetCharacterSlotFileName() const;

	// 란 모바일
	std::string GetRanMobileFileName() const;

    //! -----------------------------------------------------------------------
    //! World Battle
    //! 국가명으로 검색한다
    void wbSearch(const std::string& SearchWord, std::vector<wb::COUNTRY_INFO>& Result);
    //! 설정된 전체국가를 가져온다
    void wbSearch(std::vector<wb::COUNTRY_INFO>& Result);
    bool wbIsExistCountry(wb::EM_COUNTRY CountryCode) const;

    //! 국가 정보를 가져온다
    const wb::COUNTRY_INFO& wbGetData(wb::EM_COUNTRY CountryCode) const;

    //! 국기 전체 텍스쳐 폭/높이
    WORD wbGetTextureWidth() const;
    WORD wbGetTextureHeight() const;

    //! 국기 이미지
    const std::string& wbGetFlagTexture(wb::EM_COUNTRY CountryCode) const;

    //! 국기 폭/높이
    WORD wbGetFlagWidth() const;
    WORD wbGetFlagHeight() const;
    
    //! 국기 이미지. 국기 위치
    const wb::FLAG_TEXTURE_POSITION& wbGetFlagTexturePos(wb::EM_COUNTRY CountryCode) const;
    
    //! 국가/국가별 클럽숫자
    void wbSetCountryInfo(wb::EM_COUNTRY CountryCode, WORD ClubCount);

	//! Effect 가 어느 선도 구역을 대표하는가~?
    BOOL wbGetGuidanceID( const std::string& strEffName, DWORD& dwGuidanceID_OUT ) const;

    //! -----------------------------------------------------------------------
    //! Club
    size_t GetClubAuthDescSize() const;
    std::string GetClubAuthDesc(size_t Index) const;
    size_t GetClubGradeSize() const;
    //! 초기 기본값이다. 클럽마다 이름/권한을 조정가능하다.
    club::AUTH_GRADE GetClubBaseGrade(size_t Index) const;

    const GLCLUBRANK& GetClubRankData(size_t Rank) const;
	club::ClubDefineData* GetClubBase()
	{	return m_pClubBaseData;	}

    //! 클럽원이 특정 몬스터에게 사망시 로그를 남기는가?
    bool IsClubLogDeadByMonster(const SNATIVEID& MonsterID) const;

    //! 클럽원이 특정 몬스터를 쓰러뜨릴시 로그를 남기는가?
    bool IsClubLogKillMonster(const SNATIVEID& MonsterID) const;

    //! 클럽원이 해당 레벨에 도달하면 로그를 남기는가?
    bool IsClubLogLevelUp(int ChaLevel) const;

    LONGLONG MultiplyBigValueByPoint( LONGLONG Value, double Point );
    LONGLONG MultiplyBigValueByPoint( LONGLONG Value, float Point );
};

#endif // _GLOGIC_DATA_MAN_H_
