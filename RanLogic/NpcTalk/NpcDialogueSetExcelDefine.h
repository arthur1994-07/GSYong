#ifndef _NPC_DIALOGUE_SET_EXCEL_DEFINE_H_
#define _NPC_DIALOGUE_SET_EXCEL_DEFINE_H_

#pragma once

namespace NPC_TALK_EXCEL
{
    //! Sheet 순서를 바꾸지 마시오!
    enum EMSHEET
    {
        BASE_SHEET_NUM                       =  0,
        NPC_DIALOGUE_SHEET                   =  1,
        NPC_DIALOGUE_CASE                    =  2,

        NPC_TALK_CONDITION                   =  3,
        NPC_TALK_CONDITION_QUEST_COMPALTE    =  4,
        NPC_TALK_CONDITION_HAVE_ITEM         =  5,
        NPC_TALK_CONDITION_LEARN_SKILL       =  6,
        NPC_TALK_CONDITION_DIS_QUEST         =  7,
        NPC_TALK_CONDITION_DIS_SKILL         =  8,
        NPC_TALK_CONDITION_SKILL_FACT        =  9,
    
        NPC_TALK_CONTROL                     = 10,
        NPC_TALK_CONTROL_TALK                = 11,
        NPC_TALK_CONTROL_TALK_NEED_ITEM      = 12,
        NPC_TALK_CONTROL_TALK_TRADE_ITEM     = 13,

        NPC_TALK_CONTROL_TALK_CONDITION      = 14,
        NPC_TALK_CONTROL_TALK_QUEST_COMPALTE = 15,
        NPC_TALK_CONTROL_TALK_HAVE_ITEM      = 16,
        NPC_TALK_CONTROL_TALK_LEARN_SKILL    = 17,
        NPC_TALK_CONTROL_TALK_DIS_QUEST      = 18,
        NPC_TALK_CONTROL_TALK_DIS_SKILL      = 19,
        NPC_TALK_CONTROL_TALK_SKILL_FACT     = 20,

        EM_CONDITIONSIGN_SHEET               = 21,
        EMCHARCLASS_SHEET                    = 22,
        EMSCHOOLFLAG_SHEET                   = 23,

        ITEM_DATA_SHEET                      = 24,
        QUEST_DATA_SHEET                     = 25,
        SKILL_DATA_SHEET                     = 26,
        NPC_DATA_SHEET                       = 27,

        SHEET_TOTAL_NUM                      = 28,
    };
   
    //! Excel Sheet Name 은 31자까지 가능
    //! Item Excel file/worksheet name
    //!                                                      = "1234567890123456789012345678901"
    const std::string BaseSheetName                          = "CNpcDialogueSet|Talk Base";
    const std::string DialogueSheetName                      = "CNpcDialogue|Dialogue";
    const std::string DialogueCaseSheetName                  = "CNpcDialogueCase|Dialogue Case";
    
    const std::string NpcTalkCondtionSheetName               = "SNpcTalkCondition|Talk Condition";
    const std::string NpcTalkConditionQuestComplateSheetName = "Condition Quest Complete";
    const std::string NpcTalkConditionHaveItemSheetName      = "Condition Have Item";
    const std::string NpcTalkConditionLearnSkillSheetName    = "Condition Learn Skill";
    const std::string NpcTalkConditionDisQuestSheetName      = "Condition Dis Quest";
    const std::string NpcTalkConditionDisSkillSheetName      = "Condition Dis Skill";
    const std::string NpcTalkConditionSkillFactSheetName     = "Condition Skill Fact";
    
    const std::string NpcTalkControlSheetName                = "CNpcTalkControl|Talk Control";
    const std::string NpcTalkControlTalkSheetName            = "SNpcTalk|Talk Control";
    const std::string NpcTalkControlTalkNeedItem             = "Talk Control|Need Item";
    const std::string NpcTalkControlTalkTradeItem            = "Talk Control|Trade Item";

    const std::string NpcTalkControlTalkCondition            = "Talk Control|Condition";
    const std::string NpcTalkControlTalkQuestComplate        = "Talk Control|Quest Complete";
    const std::string NpcTalkControlTalkHaveItem             = "Talk Control|Have Item";
    const std::string NpcTalkControlTalkLearnSkill           = "Talk Control|Learn Skill";
    const std::string NpcTalkControlTalkDisQuest             = "Talk Control|Dis Quest";
    const std::string NpcTalkControlTalkDisSkill             = "Talk Control|Dis Skill";
    const std::string NpcTalkControlTalkSkillFact            = "Talk Control|Skill Fact";

    const std::string EmConditionSignSheetName               = "EM_CONDITIONSIGN";
    const std::string EmCharClassSheetName                   = "EMCHARCLASS";
    const std::string EmSchoolFlagSheetName                  = "EMSCHOOLFLAG";

    const std::string ItemDataSheetName                      = "Item Data";
    const std::string QuestDataSheetName                     = "Quest Data";
    const std::string SkillDataSheetName                     = "Skill Data";
    const std::string NpcDataSheetName                       = "NPC Data";
    
} // namespace NPC_TALK_EXCEL

#endif // _NPC_DIALOGUE_SET_EXCEL_DEFINE_H_