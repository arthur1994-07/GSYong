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
    bool m_bLoad; //! �ε��� ����� �Ǿ��°�?

    std::string m_CountryFileName; //! ������ ���ϼ���
    
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
	std::string m_NewNpcTalkFileName; //! Npc Talk ���� ����
    std::string m_TransformSkinSetFileName;
	std::string m_MiniGameOddEvenFileName;
    std::string m_SkillLearnPrereqFileName;  //! ��ųâ ����;
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
    std::string m_MapListFileOld; //! ���� ����

	std::string m_MapListWBFile;
	std::string m_MapListWBCompileFile;

	//! ���λ��� �˻��ý���  file 2012-11-13 bjkim
	std::string m_PrivateMarketFileName;

	//! Ʈ���Žý��� - Ʈ���Ÿ���Ʈ����
	std::string m_TriggerListFileName;	

	//! �⼮�� ������  file 2013-7-8 bjkim
	std::string m_RnAttendanceFileName;

	//! Lotto System
	std::string m_lottoSystemFilename;

	//��Ʈ�����۸���Ʈ
	std::string m_SetItemListName;

	// FSM Container;
	std::string m_FSMContainerFileName;

	// ����ȿ�� ������ ��ġ
	std::string m_ClonSkillPosFileName;

	// ���� �ɼ� ���� �̸�;
	std::string m_RandomOptionServerFileName; // server;
	std::string m_RandomOptionClientFileName; // client;
	std::string m_RandomOptionFileName;

	std::string m_strRandomAddonOpFileName; // ���� ���� �ɼ� ����;

	//! Country System;
	std::string m_countrySystemFileName;

	std::string m_strCostumeStatFileName;
	std::string m_strSkillChangeFileName;
	std::string m_strItemPeriodExtensionFileName;

    //! Ȯ������ : sckim, Redmine #4544
    std::string m_strRandomChanceFileName;

	// ������ ���;
	std::string m_strItemLevelFileName;

	// ������ �⺻ ���� �ɷ�ġ;
	std::string m_strItemBasicStatFileName;

	// ������ ��ũ ��ų;
	std::string m_strItemLinkSkillFileName;

	// ĳ���� ����;
	std::string m_strCharacterSlotFileName;

	// �� �����
	std::string m_strRanMobileFileName;

public:
    //! �ε��� ����� �Ǿ��°�?
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

    //! ������ ���ϼ����� �д´�
    bool LoadCountryFile();

    std::string GetHideSetFileName() const { return m_HideSetFileName; }	
	std::string GetGlogicZipFile() const { return m_GlogicZipFile; }
    std::string GetQuestZipFile() const { return m_QuestZipFile; }
    std::string GetLevelZipFile() const { return m_LevelZipFile; }
    std::string GetNpcTalkZipFile() const { return m_NpcTalkZipFile; }

    //! Npc Talk ���� ����
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
    std::string GetAttendanceFileName() const { return m_AttendanceFileName; } // (��) �⼮��
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

	//! ���λ����˻��ý���
	std::string GetPrivateMarketFileName() const { return m_PrivateMarketFileName; }	

	//! Ʈ���Žý��� - Ʈ���Ÿ���Ʈ����
	std::string GetTriggerListFileName() const { return m_TriggerListFileName; }

	//! �⼮�� ������
	std::string GetRnAttendanceFileName() const { return m_RnAttendanceFileName; }
	
	//! �ν��Ͻ� ��ũ��Ʈ;
	std::string GetInstanceTableFileName() const { return m_InstanceTableFileName; }

	//! �̴ϰ��� �ػ罺Ȧ��
	std::string GetMiniGameTexasHoldemFileName() const { return m_MiniGameTexasHoldemFileName; }

	//! Lotto System;
	std::string GetLottoSystemFilename() const { return m_lottoSystemFilename; }
	
	//��Ʈ������ ����Ʈ
	std::string GetSetListFileName() const {return m_SetItemListName;}

	// FSM Container;
	std::string GetFSMContainerFileName() const { return m_FSMContainerFileName; }

	std::string GetClonSkillPosFilename() const { return m_ClonSkillPosFileName; }

	//! Country System;
	std::string GetCountrySystemFileName() const { return m_countrySystemFileName; }

	std::string GetCostumeStatFileName() const { return m_strCostumeStatFileName; }
	std::string GetSkillChangeFileName() const { return m_strSkillChangeFileName; }
	std::string GetItemPeriodExtensionFileName() const { return m_strItemPeriodExtensionFileName; }

    //! Ȯ������ : sckim, Redmine #4544
    std::string GetRandomChanceFileName() const { return m_strRandomChanceFileName; }

	// ������ ���;
	std::string GetItemLevelFileName() const { return m_strItemLevelFileName; }

	// ������ �⺻ ���� �ɷ�ġ;
	std::string GetItemBasicStatFileName() const { return m_strItemBasicStatFileName; }

	// ������ ��ũ ��ų;
	std::string GetItemLinkSkillFileName() const { return m_strItemLinkSkillFileName; }

	// ĳ���� ����;
	std::string GetCharacterSlotFileName() const { return m_strCharacterSlotFileName; }

	// �� �����
	std::string GetRanMobileFileName() const { return m_strRanMobileFileName; }
};

#endif // _GLOGIC_FILE_H_
