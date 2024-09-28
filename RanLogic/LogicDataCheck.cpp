#include "pch.h"
#include <boost/tr1/memory.hpp>
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String\StringFormat.h"

#include "./Item/GLItemMan.h"
#include "./NpcTalk/NpcDialogueSet.h"
#include "./NpcTalk/NpcDialogue.h"
#include "./NpcTalk/NpcTalkMan.h"
#include "./Quest/GLQuest.h"
#include "./Quest/GLQuestMan.h"
#include "./GLogicDataMan.h"
#include "./LogicDataCheck.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace LOGIC_DATA_CHECK
{   
    void ErrorLog(std::string& ErrMsg, boost::function<void (std::string&)> ErrorLogFunc)
    {
        if (ErrorLogFunc)
            ErrorLogFunc(ErrMsg);
        else
            sc::writeLogError(ErrMsg);
    }

    void NtkCheck(const std::string& NtkFile, int& ErrorCount, boost::function<void (std::string&)> ErrorLogFunc)
    {
        std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
        if (!spTalkMan)
            return;

        spNpcDialogSet spDialogueSet = spTalkMan->Find(NtkFile);
        if (!spDialogueSet)
            return;

#pragma region "CNpcDialogue"
        const CNpcDialogueSet::DIALOGMAP& DialogSet = spDialogueSet->GetDialogueSet();
        for (CNpcDialogueSet::DIALOGMAP_CITER iter = DialogSet.begin(); iter!=DialogSet.end(); ++iter)
        {
            std::tr1::shared_ptr<CNpcDialogue> spDialog = iter->second;
            const CNpcDialogue::DLGCASE& DialogCase = spDialog->GetDlgCaseVec();
            // ----------------------------------------------------------------
#pragma region "Dialog Case"
            for (size_t i=0; i<DialogCase.size(); ++i)
            {
                std::tr1::shared_ptr<CNpcDialogueCase> spCase = DialogCase[i];
                if (!spCase)
                    continue;

                std::tr1::shared_ptr<SNpcTalkCondition> spCondition = spCase->GetCondition();
                if (!spCondition)
                    continue;
                
                // ----------------------------------------------------------------
#pragma region "Start Quest"
                if (UINT_MAX != spCondition->m_dwQUEST_NID)
                {
                    GLQUEST* pQuest = GLQuestMan::GetInstance().Find(spCondition->m_dwQUEST_NID);
                    if (!pQuest)
                    {
                        ++ErrorCount;				
                        ErrorLog(
                            sc::string::format(
                                "%1% DialogID %2% Quest %3% not exist.",
                                NtkFile,
                                spDialog->GetDlgNID(),
                                spCondition->m_dwQUEST_NID),
                            ErrorLogFunc);
                    }
                    
                    if (pQuest)
                    {
                        GLQUEST_STEP* pQuestStep = pQuest->GetSTEP(spCondition->m_dwQUEST_STEP);
                        if (!pQuestStep)
                        {
                            ++ErrorCount;			
							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							{
								CString strKey;
								strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
								ErrorLog(
									sc::string::format(
									"%1% DialogID %2% Quest %3% %4% Step %5% not exist.",
									NtkFile,
									spDialog->GetDlgNID(),
									spCondition->m_dwQUEST_NID,
									CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(),
									spCondition->m_dwQUEST_STEP),
									ErrorLogFunc);
							}
							else
							{
								ErrorLog(
									sc::string::format(
									"%1% DialogID %2% Quest %3% %4% Step %5% not exist.",
									NtkFile,
									spDialog->GetDlgNID(),
									spCondition->m_dwQUEST_NID,
									pQuest->GetTITLE(),
									spCondition->m_dwQUEST_STEP),
									ErrorLogFunc);
							}
                        }
                    }
                }
#pragma endregion
                // ----------------------------------------------------------------
#pragma region "Complete Quest"
                const SNpcTalkCondition::PROCQUEST& CompleteQuest = spCondition->GetCompleteQuest();
                for (size_t j=0; j<CompleteQuest.size(); ++j)
                {
                    // Quest 번호
                    if (UINT_MAX != CompleteQuest[j])
                    {
                        const GLQUEST* pQuest = GLQuestMan::GetInstance().Find(CompleteQuest[j]);
                        if (!pQuest)
                        {
                            ++ErrorCount;
                            ErrorLog(
                                sc::string::format(
                                    "%1% DialogID %2% Complete Quest %3% not exist.",
                                    NtkFile,
                                    spDialog->GetDlgNID(),
                                    CompleteQuest[j]),
                                ErrorLogFunc);
                        }
                    }
                }
#pragma endregion
                // ----------------------------------------------------------------
#pragma region "Dis Quest"
                const SNpcTalkCondition::PROCQUEST& DisQuest = spCondition->GetDisQuest();
                for (size_t k=0; k<DisQuest.size(); ++k)
                {
                    // Quest 번호
                    if (UINT_MAX != DisQuest[k])
                    {
                        const GLQUEST* pQuest = GLQuestMan::GetInstance().Find(DisQuest[k]);
                        if (!pQuest)
                        {
                            ++ErrorCount;
                            ErrorLog(
                                sc::string::format(
                                    "%1% DialogID %2% Dis Quest %3% not exist.",
                                    NtkFile,
                                    spDialog->GetDlgNID(),
                                    DisQuest[k]),
                                ErrorLogFunc);
                        }
                    }
                }
#pragma endregion
                // ----------------------------------------------------------------
#pragma region "Have Item"
                const SNpcTalkCondition::HAVEITEM& HaveItem = spCondition->GetHaveItem();
                for (size_t i=0; i<HaveItem.size(); ++i)
                {
                    const SITEM_NUM& ItemNum = HaveItem[i];                    
                    const SITEM* pItemData = GLogicData::GetInstance().GetItem(ItemNum.GetNativeId());
                    if (!pItemData)
                    {
                        ++ErrorCount;
                        ErrorLog(
                            sc::string::format(
                                "%1% DialogID %2% Have Item %3%/%4% not exist.",
                                NtkFile,
                                spDialog->GetDlgNID(),
                                ItemNum.GetNativeId().wMainID,
                                ItemNum.GetNativeId().wSubID),
                            ErrorLogFunc);
                    }
                }
#pragma endregion
                // ----------------------------------------------------------------
#pragma region "Learn Skill"
                const SNpcTalkCondition::LEARNSKILL& LearnSkill = spCondition->GetLearnSkill();
                for (size_t i=0; i<LearnSkill.size(); ++i)
                {
                    const SSkillCondition& SkillCondition = LearnSkill[i];
                    if (SkillCondition.SkillId() == UINT_MAX)
                        continue;

                    SNATIVEID SkillId(SkillCondition.SkillId());
                    const GLSKILL* pSkillData = GLSkillMan::GetInstance().GetData(SkillId);
                    if (!pSkillData)
                    {
                        ++ErrorCount;
                        ErrorLog(
                            sc::string::format(
                                "%1% DialogID %2% Learn Skill %3%/%4% not exist.",
                                NtkFile,
                                spDialog->GetDlgNID(),
                                SkillId.wMainID,
                                SkillId.wSubID),
                            ErrorLogFunc);
                    }
                    else
                    {
                        if (SkillCondition.wLevel != 0xFFFF && SkillCondition.wLevel >= SKILL::MAX_LEVEL)
                        {
                            //const SKILL::SLEARN& SkillLearn = pSkillData->GetLearnData();
                            ++ErrorCount;
                            ErrorLog(
                                sc::string::format(
                                    "%1% DialogID %2% Learn Skill %3%/%4% Level %5% not exist.",
                                    NtkFile,
                                    spDialog->GetDlgNID(),
                                    SkillId.wMainID,
                                    SkillId.wSubID,
                                    SkillCondition.wLevel),
                                ErrorLogFunc);                            
                        }
                    }
                }
#pragma endregion
                // ----------------------------------------------------------------
#pragma region "Skill Fact"
                const SNpcTalkCondition::SKILLFACT& SkillFact = spCondition->GetSkillFact();
                for (size_t i=0; i<SkillFact.size(); ++i)
                {                    
                    const SSkillCondition& SkillCondition = SkillFact[i];
                    if (SkillCondition.SkillId() == UINT_MAX)
                        continue;

                    SNATIVEID SkillId(SkillCondition.SkillId());
                    const GLSKILL* pSkillData = GLSkillMan::GetInstance().GetData(SkillId);
                    if (!pSkillData)
                    {
                        ++ErrorCount;
                        ErrorLog(
                            sc::string::format(
                                "%1% DialogID %2% Skill Fact %3%/%4% not exist.",
                                NtkFile,
                                spDialog->GetDlgNID(),
                                SkillId.wMainID,
                                SkillId.wSubID),
                            ErrorLogFunc);
                    }
                    else
                    {
                        if (SkillCondition.wLevel != 0xFFFF && SkillCondition.wLevel >= SKILL::MAX_LEVEL)
                        {
                            //const SKILL::SLEARN& SkillLearn = pSkillData->GetLearnData();
                            ++ErrorCount;
                            ErrorLog(
                                sc::string::format(
                                "%1% DialogID %2% Skill Fact %3%/%4% Level %5% not exist.",
                                    NtkFile,
                                    spDialog->GetDlgNID(),
                                    SkillId.wMainID,
                                    SkillId.wSubID,
                                    SkillCondition.wLevel),
                                ErrorLogFunc);                            
                        }
                    }
                }
#pragma endregion
                // ----------------------------------------------------------------
#pragma region "Dis Skill"
                const SNpcTalkCondition::PROCSKILL& DisSkill = spCondition->GetDisSkill();
                for (size_t i=0; i<DisSkill.size(); ++i)
                {
                    SNATIVEID SkillId(DisSkill[i]);
                    const GLSKILL* pSkillData = GLSkillMan::GetInstance().GetData(SkillId);
                    if (!pSkillData)
                    {
                        ++ErrorCount;
                        ErrorLog(
                            sc::string::format(
                                "%1% DialogID %2% Dis Skill %3%/%4% not exist.",
                                NtkFile,
                                spDialog->GetDlgNID(),
                                SkillId.wMainID,
                                SkillId.wSubID),
                            ErrorLogFunc);
                    }

                }
#pragma endregion
            }
#pragma endregion
        }
#pragma endregion
        // ---------------------------------------------------------------------------
        const CNpcDialogueSet::VECTALK& Talk = spDialogueSet->GetTalkVec();
#pragma region "Npc Talk"
        for ( size_t i=0; i<Talk.size(); ++i )
        {
            std::tr1::shared_ptr<SNpcTalk> spTalk = Talk[i];
            if (!spTalk)
                continue;		
            // ----------------------------------------------------------------
#pragma region "Talk Quest Start, Step"
            if ((spTalk->GetAction() == SNpcTalk::EM_QUEST_START || spTalk->GetAction() == SNpcTalk::EM_QUEST_STEP) && UINT_MAX != spTalk->QuestProgressID())
            {
                GLQUEST* pActionQuest = GLQuestMan::GetInstance().Find(spTalk->QuestProgressID());
                if (!pActionQuest)
                {
                    ++ErrorCount;
                    ErrorLog(
                        sc::string::format(
                            "%1% Talk (%2%/%3%) Action Quest %4% not exist.",
                            NtkFile,
                            spTalk->m_dwNID,
                            spTalk->m_dwGLOB_ID,//i,
                            spTalk->QuestProgressID()),
                        ErrorLogFunc);
                }
                
                DWORD QuestStep = spTalk->QuestProgressStepID();
                if (spTalk->GetAction() == SNpcTalk::EM_QUEST_STEP && pActionQuest && UINT_MAX != QuestStep)
                {                    
                    const GLQUEST_STEP* pStep = pActionQuest->GetSTEP(QuestStep);
                    if (!pStep)
                    {
                        ++ErrorCount;
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							CString strKey;
							strKey.Format("QN_%03d_%03d", pActionQuest->m_sNID.wMainID, pActionQuest->m_sNID.wSubID);
							ErrorLog(
								sc::string::format(
								"%1% Talk (%2%/%3%) Action %4% Quest %5% %6% Step %7% not exist.",
								NtkFile,
								spTalk->m_dwNID,
								spTalk->m_dwGLOB_ID,//i,
								spTalk->GetTalk(),
								spTalk->QuestProgressID(),
								CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(),
								QuestStep),
								ErrorLogFunc);
						}
						else
						{
							ErrorLog(
								sc::string::format(
								"%1% Talk (%2%/%3%) Action %4% Quest %5% %6% Step %7% not exist.",
								NtkFile,
								spTalk->m_dwNID,
								spTalk->m_dwGLOB_ID,//i,
								spTalk->GetTalk(),
								spTalk->QuestProgressID(),
								pActionQuest->GetTITLE(),
								QuestStep),
								ErrorLogFunc);
						}
                    }
                }
            }
#pragma endregion
            // ----------------------------------------------------------------
            std::tr1::shared_ptr<SNpcTalkCondition> spCondition = spTalk->GetCondition();
            if (!spCondition)
                continue;

            // ----------------------------------------------------------------
#pragma region "Talk Condition Completed Quest"
            DWORD TalkConditionProgressQuestID = spCondition->ProgressQuestId();
            if (TalkConditionProgressQuestID != UINT_MAX)
            {
                GLQUEST* pQuestData = GLQuestMan::GetInstance().Find(TalkConditionProgressQuestID);
                if (!pQuestData)
                {
                    ++ErrorCount;
                    ErrorLog(
                        sc::string::format(
                            "%1% Talk (%2%/%3%) Action %4% Condition Quest %5% not exist.",
                            NtkFile,
                            spTalk->m_dwNID,
                            spTalk->m_dwGLOB_ID,//i,
                            spTalk->GetTalk(),
                            TalkConditionProgressQuestID),
                        ErrorLogFunc);
                }
                else
                {
                    GLQUEST_STEP* pQuestStep = pQuestData->GetSTEP(spCondition->ProgressQuestStep());
                    if (!pQuestStep)
                    {
                        ++ErrorCount;
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							CString strKey;
							strKey.Format("QN_%03d_%03d", pQuestData->m_sNID.wMainID, pQuestData->m_sNID.wSubID);
							ErrorLog(
								sc::string::format(
								"%1% Talk (%2%/%3%) Action %4% Condition Quest %5% %6% Step %7% not exist.",
								NtkFile,
								spTalk->m_dwNID,
								spTalk->m_dwGLOB_ID,//i,
								spTalk->GetTalk(),
								TalkConditionProgressQuestID,
								CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(),
								spCondition->ProgressQuestStep()),
								ErrorLogFunc);
						}
						else
						{
							ErrorLog(
								sc::string::format(
								"%1% Talk (%2%/%3%) Action %4% Condition Quest %5% %6% Step %7% not exist.",
								NtkFile,
								spTalk->m_dwNID,
								spTalk->m_dwGLOB_ID,//i,
								spTalk->GetTalk(),
								TalkConditionProgressQuestID,
								pQuestData->GetTITLE(),
								spCondition->ProgressQuestStep()),
								ErrorLogFunc);
						}
                    }
                }                
            }            
#pragma endregion

            // ----------------------------------------------------------------
#pragma region "Talk Condition Completed Quest"
            const SNpcTalkCondition::PROCQUEST& TalkConditionCompleteQuest = spCondition->GetCompleteQuest();
            for (size_t j=0; j<TalkConditionCompleteQuest.size(); ++j)
            {
                // Quest 번호
                if (UINT_MAX != TalkConditionCompleteQuest[j])
                {
                    const GLQUEST* pQuest = GLQuestMan::GetInstance().Find(TalkConditionCompleteQuest[j]);
                    if (!pQuest)
                    {
                        ++ErrorCount;
                        ErrorLog(
                            sc::string::format(
                            "%1% Talk (%2%/%3%/%4%) (Talk Condition) Complete Quest %5% not exist.",
                            NtkFile,
                            spTalk->m_dwNID,
                            i,
                            j,
                            TalkConditionCompleteQuest[j]),
                        ErrorLogFunc);
                    }
                }
            }
#pragma endregion
            // ----------------------------------------------------------------
#pragma region "Talk Condition Dis Quest"
            const SNpcTalkCondition::PROCQUEST& TalkConditionDisQuest = spCondition->GetDisQuest();
            for (size_t k=0; k<TalkConditionDisQuest.size(); ++k)
            {
                // Quest 번호
                if (UINT_MAX != TalkConditionDisQuest[k])
                {
                    const GLQUEST* pQuest = GLQuestMan::GetInstance().Find( TalkConditionDisQuest[k] );
                    if (!pQuest)
                    {
                        ++ErrorCount;
                        ErrorLog(
                            sc::string::format(
                                "%1% Talk (%2%/%3%/%4%) (Talk Condition) Dis Quest %5% not exist.",
                                NtkFile,
                                spTalk->m_dwNID,
                                i,
                                k,
                                TalkConditionDisQuest[k]),
                            ErrorLogFunc);
                    }
                }
            }
#pragma endregion

            // ----------------------------------------------------------------
#pragma region "Talk Condition Have Item"
            const SNpcTalkCondition::HAVEITEM& TalkConditionHaveItem = spCondition->GetHaveItem();
            for (size_t i=0; i<TalkConditionHaveItem.size(); ++i)
            {
                const SITEM_NUM& ItemNum = TalkConditionHaveItem[i];
                const SNATIVEID& ItemId = ItemNum.GetNativeId();
                const SITEM* pItemData = GLogicData::GetInstance().GetItem(ItemId);
                if (!pItemData)
                {
                    ++ErrorCount;
                    ErrorLog(
                        sc::string::format(
                            "%1% Talk (%2%/%3%) Action %4% Condition Have Item %5%/%6% not exist.",
                            NtkFile,
                            spTalk->m_dwNID,
                            spTalk->m_dwGLOB_ID,//i,
                            spTalk->GetTalk(),
                            ItemId.wMainID,
                            ItemId.wSubID),
                        ErrorLogFunc);
                }
            }
#pragma endregion

            // ----------------------------------------------------------------
#pragma region "Talk Condition Learn Skill"
            const SNpcTalkCondition::LEARNSKILL& TalkConditionLearnSkill = spCondition->GetLearnSkill();
            for (size_t i=0; i<TalkConditionLearnSkill.size(); ++i)
            {
                const SSkillCondition& SkillCondition = TalkConditionLearnSkill[i];
                SNATIVEID SkillId(SkillCondition.dwNID);
                GLSKILL* pSkillData = GLSkillMan::GetInstance().GetData(SkillId);
                if (!pSkillData)
                {
                    ++ErrorCount;
                    ErrorLog(
                        sc::string::format(
                        "%1% Talk (%2%/%3%) Action %4% Condition Learn Skill %5%/%6% not exist.",
                            NtkFile,
                            spTalk->m_dwNID,
                            spTalk->m_dwGLOB_ID,//i,
                            spTalk->GetTalk(),
                            SkillId.wMainID,
                            SkillId.wSubID),
                        ErrorLogFunc);
                }
                else
                {
                    if (SkillCondition.wLevel != 0xFFFF && SkillCondition.wLevel >= SKILL::MAX_LEVEL)
                    {
                        ++ErrorCount;
                        ErrorLog(
                            sc::string::format(
                                "%1% Talk (%2%/%3%) Action %4% Condition Learn Skill %5%/%6% %7% Level %8% not exist.",
                                NtkFile,
                                spTalk->m_dwNID,
                                spTalk->m_dwGLOB_ID,
                                spTalk->GetTalk(),
                                SkillId.wMainID,
                                SkillId.wSubID,
                                pSkillData->GetName(),
                                SkillCondition.wLevel),
                            ErrorLogFunc);
                    }
                }                
            }
#pragma endregion

            // ----------------------------------------------------------------
#pragma region "Talk Condition Skill Fact"            
            const SNpcTalkCondition::SKILLFACT& TalkConditionSkillFact = spCondition->GetSkillFact();
            for (size_t i=0; i<TalkConditionSkillFact.size(); ++i)
            {                    
                const SSkillCondition& SkillCondition = TalkConditionSkillFact[i];
                if (SkillCondition.SkillId() == UINT_MAX)
                    continue;

                SNATIVEID SkillId(SkillCondition.SkillId());
                const GLSKILL* pSkillData = GLSkillMan::GetInstance().GetData(SkillId);
                if (!pSkillData)
                {
                    ++ErrorCount;
                    ErrorLog(
                        sc::string::format(
                            "%1% Talk (%2%/%3%) Action %4% Condition Skill Fact %5%/%6% not exist.",
                            NtkFile,
                            spTalk->m_dwNID,
                            spTalk->m_dwGLOB_ID,
                            spTalk->GetTalk(),
                            SkillId.wMainID,
                            SkillId.wSubID),
                        ErrorLogFunc);
                }
                else
                {
                    if (SkillCondition.wLevel != 0xFFFF && SkillCondition.wLevel >= SKILL::MAX_LEVEL)
                    {
                        ++ErrorCount;
                        ErrorLog(
                            sc::string::format(
                                "%1% Talk (%2%/%3%) Action %4% Condition Skill Fact %5%/%6% Level %7%not exist.",
                                NtkFile,
                                spTalk->m_dwNID,
                                spTalk->m_dwGLOB_ID,
                                spTalk->GetTalk(),
                                SkillId.wMainID,
                                SkillId.wSubID,
                                SkillCondition.wLevel),
                            ErrorLogFunc);
                    }
                }
            }
#pragma endregion

            // ----------------------------------------------------------------
#pragma region "Talk Condition Dis Skill"
            const SNpcTalkCondition::PROCSKILL& TalkConditionDisSkill = spCondition->GetDisSkill();
            for (size_t i=0; i<TalkConditionDisSkill.size(); ++i)
            {
                SNATIVEID SkillId(TalkConditionDisSkill[i]);
                const GLSKILL* pSkillData = GLSkillMan::GetInstance().GetData(SkillId);
                if (!pSkillData)
                {
                    ++ErrorCount;
                    ErrorLog(
                        sc::string::format(
                        "%1% Talk (%2%/%3%) Action %4% Condition Skill Dis Skill %5%/%6% not exist.",
                        NtkFile,
                        spTalk->m_dwNID,
                        spTalk->m_dwGLOB_ID,
                        spTalk->GetTalk(),
                        SkillId.wMainID,
                        SkillId.wSubID),
                        ErrorLogFunc);
                }
            }
#pragma endregion
            // ----------------------------------------------------------------

        }
#pragma endregion
    }
} // namespace LOGIC_DATA_CHECK