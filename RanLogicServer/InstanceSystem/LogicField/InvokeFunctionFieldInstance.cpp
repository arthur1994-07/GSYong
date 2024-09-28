#include "pch.h"

#include "../../../RanLogic/Msg/PVEMsg.h"
#include "../../Party/GLPartyManagerField.h"
#include "../../Club/GLClubManField.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "InstanceSystemField.h"
#include "InvokeFunctionField.h"

// added Contents
#include "../Contents/ClubDeathMatch/RnClubDeathMatch.h"
#include "../Contents/CaptureTheFlag/CaptureTheFlag.h"
#include "../Contents/GuidanceClubBattle/GuidanceClubBattle.h"
#include "../Contents/CountryBattle/CountryBattle.h"

#include "../InvokeUI/InvokeFunctionUI.h"
#include "../InvokeDBLog/InvokeFunctionDBLog.h"

namespace InstanceSystem
{
	/*void registFunctionFieldInstance(InstanceSystem::ScriptManager* const _pScriptManager)
	{
		// regist fucnction for added contents;
		{
            registFunction_CDM_Instance(_pScriptManager);
            registFunction_CaptureTheFlag_Instance(_pScriptManager);
            registFunction_Guidance_Instance(_pScriptManager);
			registFunction_CTI_Instance( _pScriptManager );

			RegistInvokeFunction_UI_Instance( _pScriptManager );
		}

		{ // isXXX;			
		}
		{ // getXXX;
			_pScriptManager->registFunction("GetHP", InstanceSystem::FieldScriptFunction::GetHP);
			_pScriptManager->registFunction("GetMP", InstanceSystem::FieldScriptFunction::GetMP);
			_pScriptManager->registFunction("GetSP", InstanceSystem::FieldScriptFunction::GetSP);
			_pScriptManager->registFunction("GetMaxHP", InstanceSystem::FieldScriptFunction::GetMaxHP);
			_pScriptManager->registFunction("GetMaxMP", InstanceSystem::FieldScriptFunction::GetMaxMP);
			_pScriptManager->registFunction("GetMaxSP", InstanceSystem::FieldScriptFunction::GetMaxSP);

			_pScriptManager->registFunction("GetPosition", InstanceSystem::FieldScriptFunction::GetPosition);

			_pScriptManager->registFunction("GetFaction", InstanceSystem::FieldScriptFunction::GetFaction);

			_pScriptManager->registFunction("GetLeftTime", InstanceSystem::FieldScriptFunction::GetLeftTime);
			_pScriptManager->registFunction("GetFactionList", InstanceSystem::FieldScriptFunction::GetFactionList);

			_pScriptManager->registFunction("GetOffensivePower", InstanceSystem::FieldScriptFunction::GetOffensivePower);
			_pScriptManager->registFunction("GetDefensivePower", InstanceSystem::FieldScriptFunction::GetDefensivePower);
			_pScriptManager->registFunction("GetFireRegistance", InstanceSystem::FieldScriptFunction::GetFireRegistance);
			_pScriptManager->registFunction("GetIceRegistance", InstanceSystem::FieldScriptFunction::GetIceRegistance);
			_pScriptManager->registFunction("GetElectricRegistance", InstanceSystem::FieldScriptFunction::GetElectricRegistance);
			_pScriptManager->registFunction("GetPoisonRegistance", InstanceSystem::FieldScriptFunction::GetPoisonRegistance);
			_pScriptManager->registFunction("GetSpiritRegistance", InstanceSystem::FieldScriptFunction::GetSpiritRegistance);
			_pScriptManager->registFunction("GetAccuracyRate", InstanceSystem::FieldScriptFunction::GetAccuracyRate);
			_pScriptManager->registFunction("GetAvoidanceRate", InstanceSystem::FieldScriptFunction::GetAvoidanceRate);
			_pScriptManager->registFunction("GetHPRestorationRate", InstanceSystem::FieldScriptFunction::GetHPRestorationRate);
			_pScriptManager->registFunction("GetMPRestirationRate", InstanceSystem::FieldScriptFunction::GetMPRestirationRate);
			_pScriptManager->registFunction("GetSPRestirationRate", InstanceSystem::FieldScriptFunction::GetSPRestirationRate);

			_pScriptManager->registFunction("GetMultipleHP", InstanceSystem::FieldScriptFunction::GetMultipleHP);
			_pScriptManager->registFunction("GetMultipleAttack", InstanceSystem::FieldScriptFunction::GetMultipleAttack);
			_pScriptManager->registFunction("GetMultipleDefense", InstanceSystem::FieldScriptFunction::GetMultipleDefense);
			_pScriptManager->registFunction("GetMultipleExp", InstanceSystem::FieldScriptFunction::GetMultipleExp);
			_pScriptManager->registFunction("GetMultipleDrop", InstanceSystem::FieldScriptFunction::GetMultipleDrop);
		}
		{ // setXXX;
			_pScriptManager->registFunction("SetHP", InstanceSystem::FieldScriptFunction::SetHP);
			_pScriptManager->registFunction("SetMP", InstanceSystem::FieldScriptFunction::SetMP);
			_pScriptManager->registFunction("SetSP", InstanceSystem::FieldScriptFunction::SetSP);

			_pScriptManager->registFunction("SetOffensivePower", InstanceSystem::FieldScriptFunction::SetOffensivePower);
			_pScriptManager->registFunction("SetDefensivePower", InstanceSystem::FieldScriptFunction::SetDefensivePower);
			_pScriptManager->registFunction("SetFireRegistance", InstanceSystem::FieldScriptFunction::SetFireRegistance);
			_pScriptManager->registFunction("SetIceRegistance", InstanceSystem::FieldScriptFunction::SetIceRegistance);
			_pScriptManager->registFunction("SetElectricRegistance", InstanceSystem::FieldScriptFunction::SetElectricRegistance);
			_pScriptManager->registFunction("SetPoisonRegistance", InstanceSystem::FieldScriptFunction::SetPoisonRegistance);
			_pScriptManager->registFunction("SetSpiritRegistance", InstanceSystem::FieldScriptFunction::SetSpiritRegistance);
			_pScriptManager->registFunction("SetAccuracyRate", InstanceSystem::FieldScriptFunction::SetAccuracyRate);
			_pScriptManager->registFunction("SetAvoidanceRate", InstanceSystem::FieldScriptFunction::SetAvoidanceRate);
			_pScriptManager->registFunction("SetHPRestorationRate", InstanceSystem::FieldScriptFunction::SetHPRestorationRate);
			_pScriptManager->registFunction("SetMPRestirationRate", InstanceSystem::FieldScriptFunction::SetMPRestirationRate);
			_pScriptManager->registFunction("SetSPRestirationRate", InstanceSystem::FieldScriptFunction::SetSPRestirationRate);

			_pScriptManager->registFunction("SetMultipleHP", InstanceSystem::FieldScriptFunction::SetMultipleHP);
			_pScriptManager->registFunction("SetMultipleAttack", InstanceSystem::FieldScriptFunction::SetMultipleAttack);
			_pScriptManager->registFunction("SetMultipleDefense", InstanceSystem::FieldScriptFunction::SetMultipleDefense);
			_pScriptManager->registFunction("SetMultipleExp", InstanceSystem::FieldScriptFunction::SetMultipleExp);
			_pScriptManager->registFunction("SetMultipleDrop", InstanceSystem::FieldScriptFunction::SetMultipleDrop);

			_pScriptManager->registFunction("ResetOffensivePower", InstanceSystem::FieldScriptFunction::ResetOffensivePower);
			_pScriptManager->registFunction("ResetDefensivePower", InstanceSystem::FieldScriptFunction::ResetDefensivePower);
			_pScriptManager->registFunction("ResetFireRegistance", InstanceSystem::FieldScriptFunction::ResetFireRegistance);
			_pScriptManager->registFunction("ResetIceRegistance", InstanceSystem::FieldScriptFunction::ResetIceRegistance);
			_pScriptManager->registFunction("ResetElectricRegistance", InstanceSystem::FieldScriptFunction::ResetElectricRegistance);
			_pScriptManager->registFunction("ResetPoisonRegistance", InstanceSystem::FieldScriptFunction::ResetPoisonRegistance);
			_pScriptManager->registFunction("ResetSpiritRegistance", InstanceSystem::FieldScriptFunction::ResetSpiritRegistance);
			_pScriptManager->registFunction("ResetAccuracyRate", InstanceSystem::FieldScriptFunction::ResetAccuracyRate);
			_pScriptManager->registFunction("ResetAvoidanceRate", InstanceSystem::FieldScriptFunction::ResetAvoidanceRate);
			_pScriptManager->registFunction("ResetHPRestorationRate", InstanceSystem::FieldScriptFunction::ResetHPRestorationRate);
			_pScriptManager->registFunction("ResetMPRestirationRate", InstanceSystem::FieldScriptFunction::ResetMPRestirationRate);
			_pScriptManager->registFunction("ResetSPRestirationRate", InstanceSystem::FieldScriptFunction::ResetSPRestirationRate);

			_pScriptManager->registFunction("SetVisible", InstanceSystem::FieldScriptFunction::SetVisible);
			_pScriptManager->registFunction("SetMoveLock", InstanceSystem::FieldScriptFunction::SetMoveLock);
			_pScriptManager->registFunction("SetHpVisible", InstanceSystem::FieldScriptFunction::SetHpVisible);
            _pScriptManager->registFunction("CreateFaction", InstanceSystem::FieldScriptFunction::CreateFaction);
			_pScriptManager->registFunction("SetFaction", InstanceSystem::FieldScriptFunction::SetFaction);
			_pScriptManager->registFunction("SetRelation", InstanceSystem::FieldScriptFunction::SetRelation);
			_pScriptManager->registFunction("SetPositionInstance", InstanceSystem::FieldScriptFunction::SetPositionInstance);
			_pScriptManager->registFunction("SetObjectPositionInstance", InstanceSystem::FieldScriptFunction::SetObjectPositionInstance);
			_pScriptManager->registFunction("SetPositionField", InstanceSystem::FieldScriptFunction::SetPositionField);
			_pScriptManager->registFunction("SetGatePosition", InstanceSystem::FieldScriptFunction::SetGatePositionField);
			_pScriptManager->registFunction("SetReturnPosition", InstanceSystem::FieldScriptFunction::SetReturnPosition);
			_pScriptManager->registFunction("SetResurrectionForced", InstanceSystem::FieldScriptFunction::SetResurrectionForced);
			_pScriptManager->registFunction("SetResurrectionForcedPosition", InstanceSystem::FieldScriptFunction::SetResurrectionForcedPosition);
			_pScriptManager->registFunction("SetResurrectionForcedInstance", InstanceSystem::FieldScriptFunction::SetResurrectionForcedInstance);
			_pScriptManager->registFunction("SetResurrectionForcedInstancePosition", InstanceSystem::FieldScriptFunction::SetResurrectionForcedInstancePosition);
			_pScriptManager->registFunction("SetDeathPenalty", InstanceSystem::FieldScriptFunction::SetDeathPenalty);
			_pScriptManager->registFunction("SetJoinSuspendInstance", InstanceSystem::FieldScriptFunction::SetJoinSuspendInstance);
			_pScriptManager->registFunction("SetJoinLockInstance", InstanceSystem::FieldScriptFunction::SetJoinLockInstance);
			_pScriptManager->registFunction("SetJoinLockType", InstanceSystem::FieldScriptFunction::SetJoinLockType);
			_pScriptManager->registFunction("RetryFaction", InstanceSystem::FieldScriptFunction::RetryFaction);
			//_pScriptManager->registFunction("ShowTimeUI", InstanceSystem::FieldScriptFunction::ShowTimeUI);
			//_pScriptManager->registFunction("ShowEntranceStateUI", InstanceSystem::FieldScriptFunction::ShowEntranceStateUI);
			_pScriptManager->registFunction("SetStageState", InstanceSystem::FieldScriptFunction::SetStageState);
			_pScriptManager->registFunction("SetResurrectionDisable", InstanceSystem::FieldScriptFunction::SetResurrectionDisable);
            _pScriptManager->registFunction("SetResurrectionDisableSkill", InstanceSystem::FieldScriptFunction::SetResurrectionDisableSkill);
			_pScriptManager->registFunction("SetViewRange", InstanceSystem::FieldScriptFunction::SetViewRange);
		}
		{  // sendXXX;
            _pScriptManager->registFunction("SendMessage", InstanceSystem::FieldScriptFunction::SendMessage);
		}
		{  // Layer;
			_pScriptManager->registFunction("AddMobLayer", InstanceSystem::FieldScriptFunction::AddMobLayer); 
			_pScriptManager->registFunction("DelMobLayer", InstanceSystem::FieldScriptFunction::DelMobLayer); 
			_pScriptManager->registFunction("DelAllMobLayer", InstanceSystem::FieldScriptFunction::DelAllMobLayer); 
		}
        {  // motion;
            _pScriptManager->registFunction("SetMotion", InstanceSystem::FieldScriptFunction::SetMotion);
            _pScriptManager->registFunction("IsMotion", InstanceSystem::FieldScriptFunction::IsMotion);
            _pScriptManager->registFunction("ResetMotion", InstanceSystem::FieldScriptFunction::ResetMotion);
        }
		{ // registXXX;
			_pScriptManager->registFunction("RegistTimer", InstanceSystem::FieldScriptFunction::RegistTimer);
			_pScriptManager->registFunction("RemoveTimer", InstanceSystem::FieldScriptFunction::RemoveTimer);
			_pScriptManager->registFunction("IsRegistTimer", InstanceSystem::FieldScriptFunction::IsRegistTimer);
			_pScriptManager->registFunction("SuspendTimer", InstanceSystem::FieldScriptFunction::SuspendTimer);
			_pScriptManager->registFunction("ResumeTimer", InstanceSystem::FieldScriptFunction::ResumeTimer);
			_pScriptManager->registFunction("RegistActorTrigger",InstanceSystem::FieldScriptFunction::RegistActorTrigger);
			_pScriptManager->registFunction("RegistActorZoneTrigger",InstanceSystem::FieldScriptFunction::RegistActorZoneTrigger);
			_pScriptManager->registFunction("RemoveActorTrigger",InstanceSystem::FieldScriptFunction::RemoveActorTrigger);
		}
		{ // Skill;
			_pScriptManager->registFunction("ApplySkill", InstanceSystem::FieldScriptFunction::ApplySkill);
			_pScriptManager->registFunction("IsHaveSkillFact", InstanceSystem::FieldScriptFunction::IsHaveSkillFact);
			_pScriptManager->registFunction("RemoveSkillFact", InstanceSystem::FieldScriptFunction::RemoveSkillFact);
		}
		{ // doXXX;			
			_pScriptManager->registFunction("DoDestroy", InstanceSystem::FieldScriptFunction::DoDestroy);
			_pScriptManager->registFunction("DoOut", InstanceSystem::FieldScriptFunction::DoOut);
			_pScriptManager->registFunction("DoJoin", InstanceSystem::FieldScriptFunction::DoJoin);
			_pScriptManager->registFunction("DoMoveToPosition", InstanceSystem::FieldScriptFunction::DoMoveToPosition);
			_pScriptManager->registFunction("DoMoveToObjectPosition", InstanceSystem::FieldScriptFunction::DoMoveToObjectPosition);
            _pScriptManager->registFunction("DoEffectPositionAll", InstanceSystem::FieldScriptFunction::DoEffectPositionAll);
            _pScriptManager->registFunction("DoEffectActor", InstanceSystem::FieldScriptFunction::DoEffectActor);
            _pScriptManager->registFunction("DoEffectActorAll", InstanceSystem::FieldScriptFunction::DoEffectActorAll);
		}
		{// dropXXX;
			_pScriptManager->registFunction("DropCrow", InstanceSystem::FieldScriptFunction::DropCrow);
		}
		{// UI;
			_pScriptManager->registFunction("PrintChatMsgFaction", InstanceSystem::FieldScriptFunction::PrintChatMsgFaction);
			_pScriptManager->registFunction("PrintChatXMLMsgFaction", InstanceSystem::FieldScriptFunction::PrintChatXMLMsgFaction);
			_pScriptManager->registFunction("PrintChatMsgAll", InstanceSystem::FieldScriptFunction::PrintChatMsgAll);
			_pScriptManager->registFunction("PrintChatXMLMsgAll", InstanceSystem::FieldScriptFunction::PrintChatXMLMsgAll);

			_pScriptManager->registFunction("PrintSimpleMsgFaction", InstanceSystem::FieldScriptFunction::PrintSimpleMsgFaction);
			_pScriptManager->registFunction("PrintSimpleXMLMsgFaction", InstanceSystem::FieldScriptFunction::PrintSimpleXMLMsgFaction);
			_pScriptManager->registFunction("PrintSimpleMsgAll", InstanceSystem::FieldScriptFunction::PrintSimpleMsgAll);
			_pScriptManager->registFunction("PrintSimpleXMLMsgAll", InstanceSystem::FieldScriptFunction::PrintSimpleXMLMsgAll);

			_pScriptManager->registFunction("UI_TimerMessageBox", InstanceSystem::FieldScriptFunction::UI_TimerMessageBox);
			_pScriptManager->registFunction("UI_TimerMessageBoxAll", InstanceSystem::FieldScriptFunction::UI_TimerMessageBoxAll);
			_pScriptManager->registFunction("UI_XmlTimerMessageBox", InstanceSystem::FieldScriptFunction::UI_XmlTimerMessageBox);
			_pScriptManager->registFunction("UI_XmlTimerMessageBoxAll", InstanceSystem::FieldScriptFunction::UI_XmlTimerMessageBoxAll);
		}
	}*/

	void registFunctionFieldInstance(sc::LuaInstance& _luaInstance)
	{
		// regist fucnction for added contents;
		{
            registFunction_CDM_Instance(_luaInstance);
            registFunction_CaptureTheFlag_Instance(_luaInstance);
            registFunction_Guidance_Instance(_luaInstance);
			registFunction_CTI_Instance( _luaInstance );

			RegistInvokeFunction_UI_Instance( _luaInstance );
			RegistInvokeFunction_DBLog_Instance( _luaInstance );
		}

		{ // isXXX;			
		}
		{ // getXXX;
			_luaInstance.RegistFunction("GetHP", InstanceSystem::FieldScriptFunction::GetHP);
			_luaInstance.RegistFunction("GetMP", InstanceSystem::FieldScriptFunction::GetMP);
			_luaInstance.RegistFunction("GetSP", InstanceSystem::FieldScriptFunction::GetSP);
			_luaInstance.RegistFunction("GetMaxHP", InstanceSystem::FieldScriptFunction::GetMaxHP);
			_luaInstance.RegistFunction("GetMaxMP", InstanceSystem::FieldScriptFunction::GetMaxMP);
			_luaInstance.RegistFunction("GetMaxSP", InstanceSystem::FieldScriptFunction::GetMaxSP);
			_luaInstance.RegistFunction("GetPosition", InstanceSystem::FieldScriptFunction::GetPosition);

			_luaInstance.RegistFunction("GetFaction", InstanceSystem::FieldScriptFunction::GetFaction);

			_luaInstance.RegistFunction("GetLeftTime", InstanceSystem::FieldScriptFunction::GetLeftTime);
			_luaInstance.RegistFunction("GetFactionList", InstanceSystem::FieldScriptFunction::GetFactionList);

			_luaInstance.RegistFunction("GetOffensivePower", InstanceSystem::FieldScriptFunction::GetOffensivePower);
			_luaInstance.RegistFunction("GetDefensivePower", InstanceSystem::FieldScriptFunction::GetDefensivePower);
			_luaInstance.RegistFunction("GetFireRegistance", InstanceSystem::FieldScriptFunction::GetFireRegistance);
			_luaInstance.RegistFunction("GetIceRegistance", InstanceSystem::FieldScriptFunction::GetIceRegistance);
			_luaInstance.RegistFunction("GetElectricRegistance", InstanceSystem::FieldScriptFunction::GetElectricRegistance);
			_luaInstance.RegistFunction("GetPoisonRegistance", InstanceSystem::FieldScriptFunction::GetPoisonRegistance);
			_luaInstance.RegistFunction("GetSpiritRegistance", InstanceSystem::FieldScriptFunction::GetSpiritRegistance);
			_luaInstance.RegistFunction("GetAccuracyRate", InstanceSystem::FieldScriptFunction::GetAccuracyRate);
			_luaInstance.RegistFunction("GetAvoidanceRate", InstanceSystem::FieldScriptFunction::GetAvoidanceRate);
			_luaInstance.RegistFunction("GetHPRestorationRate", InstanceSystem::FieldScriptFunction::GetHPRestorationRate);
			_luaInstance.RegistFunction("GetMPRestirationRate", InstanceSystem::FieldScriptFunction::GetMPRestirationRate);
			_luaInstance.RegistFunction("GetSPRestirationRate", InstanceSystem::FieldScriptFunction::GetSPRestirationRate);

			_luaInstance.RegistFunction("GetMultipleHP", InstanceSystem::FieldScriptFunction::GetMultipleHP);
			_luaInstance.RegistFunction("GetMultipleAttack", InstanceSystem::FieldScriptFunction::GetMultipleAttack);
			_luaInstance.RegistFunction("GetMultipleDefense", InstanceSystem::FieldScriptFunction::GetMultipleDefense);
			_luaInstance.RegistFunction("GetMultipleExp", InstanceSystem::FieldScriptFunction::GetMultipleExp);
			_luaInstance.RegistFunction("GetMultipleDrop", InstanceSystem::FieldScriptFunction::GetMultipleDrop);
		}
		{ // setXXX;
			_luaInstance.RegistFunction("SetHP", InstanceSystem::FieldScriptFunction::SetHP);
			_luaInstance.RegistFunction("SetMP", InstanceSystem::FieldScriptFunction::SetMP);
			_luaInstance.RegistFunction("SetSP", InstanceSystem::FieldScriptFunction::SetSP);

			_luaInstance.RegistFunction("SetOffensivePower", InstanceSystem::FieldScriptFunction::SetOffensivePower);
			_luaInstance.RegistFunction("SetDefensivePower", InstanceSystem::FieldScriptFunction::SetDefensivePower);
			_luaInstance.RegistFunction("SetFireRegistance", InstanceSystem::FieldScriptFunction::SetFireRegistance);
			_luaInstance.RegistFunction("SetIceRegistance", InstanceSystem::FieldScriptFunction::SetIceRegistance);
			_luaInstance.RegistFunction("SetElectricRegistance", InstanceSystem::FieldScriptFunction::SetElectricRegistance);
			_luaInstance.RegistFunction("SetPoisonRegistance", InstanceSystem::FieldScriptFunction::SetPoisonRegistance);
			_luaInstance.RegistFunction("SetSpiritRegistance", InstanceSystem::FieldScriptFunction::SetSpiritRegistance);
			_luaInstance.RegistFunction("SetAccuracyRate", InstanceSystem::FieldScriptFunction::SetAccuracyRate);
			_luaInstance.RegistFunction("SetAvoidanceRate", InstanceSystem::FieldScriptFunction::SetAvoidanceRate);
			_luaInstance.RegistFunction("SetHPRestorationRate", InstanceSystem::FieldScriptFunction::SetHPRestorationRate);
			_luaInstance.RegistFunction("SetMPRestirationRate", InstanceSystem::FieldScriptFunction::SetMPRestirationRate);
			_luaInstance.RegistFunction("SetSPRestirationRate", InstanceSystem::FieldScriptFunction::SetSPRestirationRate);

			_luaInstance.RegistFunction("SetMultipleHP", InstanceSystem::FieldScriptFunction::SetMultipleHP);
			_luaInstance.RegistFunction("SetMultipleAttack", InstanceSystem::FieldScriptFunction::SetMultipleAttack);
			_luaInstance.RegistFunction("SetMultipleDefense", InstanceSystem::FieldScriptFunction::SetMultipleDefense);
			_luaInstance.RegistFunction("SetMultipleExp", InstanceSystem::FieldScriptFunction::SetMultipleExp);
			_luaInstance.RegistFunction("SetMultipleDrop", InstanceSystem::FieldScriptFunction::SetMultipleDrop);

			_luaInstance.RegistFunction("ResetOffensivePower", InstanceSystem::FieldScriptFunction::ResetOffensivePower);
			_luaInstance.RegistFunction("ResetDefensivePower", InstanceSystem::FieldScriptFunction::ResetDefensivePower);
			_luaInstance.RegistFunction("ResetFireRegistance", InstanceSystem::FieldScriptFunction::ResetFireRegistance);
			_luaInstance.RegistFunction("ResetIceRegistance", InstanceSystem::FieldScriptFunction::ResetIceRegistance);
			_luaInstance.RegistFunction("ResetElectricRegistance", InstanceSystem::FieldScriptFunction::ResetElectricRegistance);
			_luaInstance.RegistFunction("ResetPoisonRegistance", InstanceSystem::FieldScriptFunction::ResetPoisonRegistance);
			_luaInstance.RegistFunction("ResetSpiritRegistance", InstanceSystem::FieldScriptFunction::ResetSpiritRegistance);
			_luaInstance.RegistFunction("ResetAccuracyRate", InstanceSystem::FieldScriptFunction::ResetAccuracyRate);
			_luaInstance.RegistFunction("ResetAvoidanceRate", InstanceSystem::FieldScriptFunction::ResetAvoidanceRate);
			_luaInstance.RegistFunction("ResetHPRestorationRate", InstanceSystem::FieldScriptFunction::ResetHPRestorationRate);
			_luaInstance.RegistFunction("ResetMPRestirationRate", InstanceSystem::FieldScriptFunction::ResetMPRestirationRate);
			_luaInstance.RegistFunction("ResetSPRestirationRate", InstanceSystem::FieldScriptFunction::ResetSPRestirationRate);

			_luaInstance.RegistFunction("SetVisible", InstanceSystem::FieldScriptFunction::SetVisible);
			_luaInstance.RegistFunction("SetMoveLock", InstanceSystem::FieldScriptFunction::SetMoveLock);
			_luaInstance.RegistFunction("SetHpVisible", InstanceSystem::FieldScriptFunction::SetHpVisible);
            _luaInstance.RegistFunction("CreateFaction", InstanceSystem::FieldScriptFunction::CreateFaction);
			_luaInstance.RegistFunction("SetFaction", InstanceSystem::FieldScriptFunction::SetFaction);
			_luaInstance.RegistFunction("SetRelation", InstanceSystem::FieldScriptFunction::SetRelation);
			_luaInstance.RegistFunction("SetPositionInstance", InstanceSystem::FieldScriptFunction::SetPositionInstance);
			_luaInstance.RegistFunction("SetObjectPositionInstance", InstanceSystem::FieldScriptFunction::SetObjectPositionInstance);
			_luaInstance.RegistFunction("SetPositionField", InstanceSystem::FieldScriptFunction::SetPositionField);
			_luaInstance.RegistFunction("SetGatePosition", InstanceSystem::FieldScriptFunction::SetGatePositionField);
			_luaInstance.RegistFunction("SetReturnPosition", InstanceSystem::FieldScriptFunction::SetReturnPosition);
			_luaInstance.RegistFunction("SetResurrectionForced", InstanceSystem::FieldScriptFunction::SetResurrectionForced);
			_luaInstance.RegistFunction("SetResurrectionForcedPosition", InstanceSystem::FieldScriptFunction::SetResurrectionForcedPosition);
			_luaInstance.RegistFunction("SetResurrectionForcedInstance", InstanceSystem::FieldScriptFunction::SetResurrectionForcedInstance);
			_luaInstance.RegistFunction("SetResurrectionForcedInstancePosition", InstanceSystem::FieldScriptFunction::SetResurrectionForcedInstancePosition);
			_luaInstance.RegistFunction("SetDeathPenalty", InstanceSystem::FieldScriptFunction::SetDeathPenalty);
			_luaInstance.RegistFunction("SetJoinSuspendInstance", InstanceSystem::FieldScriptFunction::SetJoinSuspendInstance);
			_luaInstance.RegistFunction("SetJoinLockInstance", InstanceSystem::FieldScriptFunction::SetJoinLockInstance);
			_luaInstance.RegistFunction("SetJoinLockType", InstanceSystem::FieldScriptFunction::SetJoinLockType);
			_luaInstance.RegistFunction("RetryFaction", InstanceSystem::FieldScriptFunction::RetryFaction);
			//_luaInstance.RegistFunction("ShowTimeUI", InstanceSystem::FieldScriptFunction::ShowTimeUI);
			//_luaInstance.RegistFunction("ShowEntranceStateUI", InstanceSystem::FieldScriptFunction::ShowEntranceStateUI);
			_luaInstance.RegistFunction("SetStageState", InstanceSystem::FieldScriptFunction::SetStageState);
			_luaInstance.RegistFunction("SetResurrectionDisable", InstanceSystem::FieldScriptFunction::SetResurrectionDisable);
            _luaInstance.RegistFunction("SetResurrectionDisableSkill", InstanceSystem::FieldScriptFunction::SetResurrectionDisableSkill);
			_luaInstance.RegistFunction("SetViewRange", InstanceSystem::FieldScriptFunction::SetViewRange);
		}
		{  // sendXXX;
            _luaInstance.RegistFunction("SendMessage", InstanceSystem::FieldScriptFunction::SendMessage);
		}
        {  // motion;
            _luaInstance.RegistFunction("SetMotion", InstanceSystem::FieldScriptFunction::SetMotion);
            _luaInstance.RegistFunction("IsMotion", InstanceSystem::FieldScriptFunction::IsMotion);
            _luaInstance.RegistFunction("ResetMotion", InstanceSystem::FieldScriptFunction::ResetMotion);
        }
		{  // Layer;
			_luaInstance.RegistFunction("AddMobLayer", InstanceSystem::FieldScriptFunction::AddMobLayer); 
			_luaInstance.RegistFunction("DelMobLayer", InstanceSystem::FieldScriptFunction::DelMobLayer); 
			_luaInstance.RegistFunction("DelAllMobLayer", InstanceSystem::FieldScriptFunction::DelAllMobLayer); 
		}

		{ // registXXX;
			_luaInstance.RegistFunction("RegistTimer", InstanceSystem::FieldScriptFunction::RegistTimer);
			_luaInstance.RegistFunction("RemoveTimer", InstanceSystem::FieldScriptFunction::RemoveTimer);
			_luaInstance.RegistFunction("IsRegistTimer", InstanceSystem::FieldScriptFunction::IsRegistTimer);
			_luaInstance.RegistFunction("SuspendTimer", InstanceSystem::FieldScriptFunction::SuspendTimer);
			_luaInstance.RegistFunction("ResumeTimer", InstanceSystem::FieldScriptFunction::ResumeTimer);
			_luaInstance.RegistFunction("RegistActorTrigger",InstanceSystem::FieldScriptFunction::RegistActorTrigger);
			_luaInstance.RegistFunction("RegistActorZoneTrigger",InstanceSystem::FieldScriptFunction::RegistActorZoneTrigger);
			_luaInstance.RegistFunction("RemoveActorTrigger",InstanceSystem::FieldScriptFunction::RemoveActorTrigger);
		}
		{ // Skill;
			_luaInstance.RegistFunction("ApplySkill", InstanceSystem::FieldScriptFunction::ApplySkill);
			_luaInstance.RegistFunction("ApplySkillFaction", InstanceSystem::FieldScriptFunction::ApplySkill_Faction);
			_luaInstance.RegistFunction("IsHaveSkillFact", InstanceSystem::FieldScriptFunction::IsHaveSkillFact);
			_luaInstance.RegistFunction("RemoveSkillFact", InstanceSystem::FieldScriptFunction::RemoveSkillFact);
			_luaInstance.RegistFunction("RemoveSkillFact_SkillEffect", InstanceSystem::FieldScriptFunction::RemoveSkillFact_SkillEffect);
		}
		{ // doXXX;			
			_luaInstance.RegistFunction("DoDestroy", InstanceSystem::FieldScriptFunction::DoDestroy);
			_luaInstance.RegistFunction("DoOut", InstanceSystem::FieldScriptFunction::DoOut);
			_luaInstance.RegistFunction("DoJoin", InstanceSystem::FieldScriptFunction::DoJoin);
			_luaInstance.RegistFunction("DoMoveToPosition", InstanceSystem::FieldScriptFunction::DoMoveToPosition);
			_luaInstance.RegistFunction("DoMoveToObjectPosition", InstanceSystem::FieldScriptFunction::DoMoveToObjectPosition);
            _luaInstance.RegistFunction("DoEffectPositionAll", InstanceSystem::FieldScriptFunction::DoEffectPositionAll);
            _luaInstance.RegistFunction("DoEffectActor", InstanceSystem::FieldScriptFunction::DoEffectActor);
            _luaInstance.RegistFunction("DoEffectActorAll", InstanceSystem::FieldScriptFunction::DoEffectActorAll);
		}
		{// dropXXX;
			_luaInstance.RegistFunction("DropCrow", InstanceSystem::FieldScriptFunction::DropCrow);
		}
		{// UI;
			_luaInstance.RegistFunction("PrintChatMsgFaction", InstanceSystem::FieldScriptFunction::PrintChatMsgFaction);
			_luaInstance.RegistFunction("PrintChatXMLMsgFaction", InstanceSystem::FieldScriptFunction::PrintChatXMLMsgFaction);
			_luaInstance.RegistFunction("PrintChatMsgAll", InstanceSystem::FieldScriptFunction::PrintChatMsgAll);
			_luaInstance.RegistFunction("PrintChatXMLMsgAll", InstanceSystem::FieldScriptFunction::PrintChatXMLMsgAll);

			_luaInstance.RegistFunction("PrintSimpleMsgFaction", InstanceSystem::FieldScriptFunction::PrintSimpleMsgFaction);
			_luaInstance.RegistFunction("PrintSimpleXMLMsgFaction", InstanceSystem::FieldScriptFunction::PrintSimpleXMLMsgFaction);
			_luaInstance.RegistFunction("PrintSimpleMsgAll", InstanceSystem::FieldScriptFunction::PrintSimpleMsgAll);
			_luaInstance.RegistFunction("PrintSimpleXMLMsgAll", InstanceSystem::FieldScriptFunction::PrintSimpleXMLMsgAll);

			_luaInstance.RegistFunction("UI_TimerMessageBox", InstanceSystem::FieldScriptFunction::UI_TimerMessageBox);
			_luaInstance.RegistFunction("UI_TimerMessageBoxAll", InstanceSystem::FieldScriptFunction::UI_TimerMessageBoxAll);
			_luaInstance.RegistFunction("UI_XmlTimerMessageBox", InstanceSystem::FieldScriptFunction::UI_XmlTimerMessageBox);
			_luaInstance.RegistFunction("UI_XmlTimerMessageBoxAll", InstanceSystem::FieldScriptFunction::UI_XmlTimerMessageBoxAll);
		}
	}


    namespace FieldScriptFunction
    {
		void ServerLog_InstanceScriptSystem_InstanceFunction ( lua_State* pState, const std::string& strServerLog )
		{
#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			lua_getglobal(pState, "keyMapID");
			SNATIVEID _keyMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			sc::writeLogInfo( strServerLog );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Instance ID : %1% ]", _instanceMapID ) );
			sc::writeLogInfo( sc::string::format( "[ Instance Log ] -> [ Map ID : %1% / %2% ]", _keyMapID.Mid(), _keyMapID.Sid() ) );
#endif
		}

        int GetHP(lua_State* pState)
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetHP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            lua_pushinteger(pState, pActor->GetNowHP());
            return 1;
        }

        int GetMP(lua_State* pState)
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            lua_pushinteger(pState, pActor->GetNowMP());
            return 1;
        }

        int GetSP(lua_State* pState)
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetSP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            lua_pushinteger(pState, pActor->GetNowSP());
            return 1;
        }

        int GetMaxHP(lua_State* pState)  // 액터 HP 설정 / param1 : DWORD 해당 액터ID (DBNum or GaeaID), param2 : int HP, return : None;
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMaxHP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            lua_pushinteger(pState, pActor->GetMaxHP());
            return 1;
        }

        int GetMaxMP(lua_State* pState)
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMaxMP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            lua_pushinteger(pState, pActor->GetMaxSP());
            return 1;
        }

        int GetMaxSP(lua_State* pState)
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMaxSP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            lua_pushinteger(pState, pActor->GetMaxMP());
            return 1;
        }

        int GetPosition(lua_State* pState)
        {
            int currentStack(1);

            ServerActor* const pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetPosition ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            const D3DXVECTOR3& vPos = pActor->GetPosition();
            lua_pushnumber(pState, vPos.x);
            lua_pushnumber(pState, vPos.y);
            lua_pushnumber(pState, vPos.z);
            return 3;
        }

        int GetFaction(lua_State* pState)
        {
            int currentStack(1);

            const EMCROW _ActorType((EMCROW)lua_tointeger(pState, currentStack++ ));
            const DWORD _ActorID_Num((DWORD)lua_tonumber(pState, currentStack++ ));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLanManager(gpGaeaServer->GetLandManager(_instanceMapID));
            if ( _pLanManager == 0 )
            {
                return 0;
            }		
            Faction::ManagerField* const _pFactionManager(_pLanManager->GetFactionManager());
            if ( _pFactionManager == 0 )
            {
                return 0;
            }
            const Faction::GLActorID _actorID(_ActorType, _ActorID_Num);
            const Faction::FactionID* const _factionID(_pFactionManager->getFactionID(_actorID));
            if ( _factionID  == NULL )
            {
                return 0;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetFaction ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            lua_pushinteger(pState, _factionID->factionType);
            lua_pushinteger(pState, _factionID->factionID_Num);
            return 2;
        }

        int GetFactionList(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _FactionType((DWORD)lua_tonumber(pState, currentStack++ ));
            const DWORD _FactionID((DWORD)lua_tonumber(pState, currentStack++ ));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLanManager(gpGaeaServer->GetLandManager(_instanceMapID));
            if ( _pLanManager == 0 )
            {
                return 0;
            }	

            Faction::ManagerField* const _pFactionManager(_pLanManager->GetFactionManager());
            if ( _pFactionManager == 0 )
            {
                return 0;
            }

            Faction::FactionID _FactionIdInstance(_FactionType, _FactionID);
            const Faction::FactionMemberVector* factionMember =_pFactionManager->getFactionMembers(_FactionIdInstance);
            if ( factionMember == NULL )
                return 0;

            if ( factionMember->size() == 0 )
                return 0;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetFactionList ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            lua_newtable(pState);
            int top = lua_gettop(pState);
            Faction::FactionMemberVectorCIter iter = factionMember->begin();
            Faction::FactionMemberVectorCIter eiter = factionMember->end();
            for ( int i = 1; iter != eiter; iter++, i++ )
            {
                lua_newtable(pState);
                int faciontIDTop = lua_gettop(pState);
                lua_pushinteger(pState, iter->actorType);
                lua_rawseti(pState, faciontIDTop, 1);
                lua_pushinteger(pState, iter->actorID_Num);
                lua_rawseti(pState, faciontIDTop, 2);

                lua_rawseti(pState, top, i);
            }
            return 1;

        }


		int GetOffensivePower ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetOffensivePower ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushinteger( pState, pActor->GetOffenceLow() );
			lua_pushinteger( pState, pActor->GetOffenceHigh() );

			return 2;
		}

		int GetDefensivePower ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetDefensivePower ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushinteger( pState, pActor->GetDefense() );

			return 1;
		}

		int GetFireRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetFireRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const SRESIST& sRegist = pActor->GETRESIST();
			short nRegist = sRegist.GetElement( EMELEMENT_FIRE );

			lua_pushinteger( pState, nRegist );

			return 1;
		}

		int GetIceRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetIceRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const SRESIST& sRegist = pActor->GETRESIST();
			short nRegist = sRegist.GetElement( EMELEMENT_ICE );

			lua_pushinteger( pState, nRegist );

			return 1;
		}

		int GetElectricRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetElectricRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const SRESIST& sRegist = pActor->GETRESIST();
			short nRegist = sRegist.GetElement( EMELEMENT_ELECTRIC );

			lua_pushinteger( pState, nRegist );

			return 1;
		}

		int GetPoisonRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetPoisonRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const SRESIST& sRegist = pActor->GETRESIST();
			short nRegist = sRegist.GetElement( EMELEMENT_POISON );

			lua_pushinteger( pState, nRegist );

			return 1;
		}

		int GetSpiritRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetSpiritRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const SRESIST& sRegist = pActor->GETRESIST();
			short nRegist = sRegist.GetElement( EMELEMENT_SPIRIT );

			lua_pushinteger( pState, nRegist );

			return 1;
		}

		int GetAccuracyRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetAccuracyRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushinteger( pState, pActor->GetHitRate() );

			return 1;
		}

		int GetAvoidanceRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetAvoidanceRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushinteger( pState, pActor->GetAvoid() );

			return 1;
		}

		int GetHPRestorationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetHPRestorationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushnumber( pState, pActor->GetHPRestorationRate() );

			return 1;
		}

		int GetMPRestirationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMPRestirationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushnumber( pState, pActor->GetMPRestorationRate() );

			return 1;
		}

		int GetSPRestirationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetSPRestirationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushnumber( pState, pActor->GetSPRestorationRate() );

			return 1;
		}

		int GetMultipleHP ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMultipleHP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushnumber( pState, pActor->GetMultipleHP() );

			return 1;
		}

		int GetMultipleAttack ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMultipleAttack ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushnumber( pState, pActor->GetMultipleAttack() );

			return 1;
		}

		int GetMultipleDefense ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMultipleDefense ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushnumber( pState, pActor->GetMultipleDefense() );

			return 1;
		}

		int GetMultipleExp ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMultipleExp ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushnumber( pState, pActor->GetMultipleExp() );

			return 1;
		}

		int GetMultipleDrop ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetMultipleDrop ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			lua_pushnumber( pState, pActor->GetMultipleDrop() );

			return 1;
		}


        int SetHP(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            const int _nHP(lua_tointeger(pState, currentStack++));

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* const _pChar = (GLChar*)pActor;
                    _pChar->m_sHP.nNow = _nHP;

                    //	[자신에게] 회복상태 갱신.
                    GLMSG::SNETPC_UPDATE_STATE NetMsg;
                    NetMsg.sHP = _pChar->m_sHP;
                    NetMsg.sMP =_pChar->m_sMP;
                    NetMsg.sSP = _pChar->m_sSP;

                    //NetMsg.ENCODE ( m_dwGaeaID );
                    _pChar->SendToClient( &NetMsg );

                    //	주위에 체력 변화 알림.
                    GLMSG::SNETPC_UPDATE_STATE_BRD NetMsgBrd;
                    NetMsgBrd.dwGaeaID = _pChar->GetGaeaID();
                    NetMsgBrd.sHP = _pChar->m_sHP;
                    _pChar->SendMsgViewAround(&NetMsgBrd);

                }
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                {
                    GLCrow* const _pCrow = (GLCrow*)pActor;
                    _pCrow->m_dwNowHP = (DWORD)_nHP;

                    GLMSG::SNETCROW_UPDATE_STATE NetMsgBrd(_pCrow->GetGaeaID(), _pCrow->m_dwNowHP);
                    _pCrow->SendMsgViewAround(&NetMsgBrd);
                }
                break;

            default:
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetHP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetMP(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            const int _nMP(lua_tointeger(pState, currentStack++));

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* const _pChar = (GLChar*)pActor;
                    _pChar->m_sMP.nNow = _nMP;
                }
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                break;

            default:
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetSP(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            const int _nSP(lua_tointeger(pState, currentStack++));

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* const _pChar = (GLChar*)pActor;
                    _pChar->m_sMP.nNow = _nSP;
                }
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                break;

            default:
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetSP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }


		int SetOffensivePower ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetOffensivePower ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nOffence = lua_tointeger( pState, nCurrentStack++ );
			int nOffencePlus = nOffence - pActor->GetBasicOffenceHigh();

			pActor->SetOffence( nOffencePlus );

			return 0;
		}

		int SetDefensivePower ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetDefensivePower ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nDefence = lua_tointeger( pState, nCurrentStack++ );
			int nDefencePlus = nDefence - pActor->GetBasicDefence();

			pActor->SetDefence( nDefencePlus );

			return 0;
		}

		int SetFireRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetFireRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nTempRegist = lua_tointeger( pState, nCurrentStack++ );
			const SRESIST& sRegist = pActor->GetBasicRegist();
			short nRegist = sRegist.GetElement( EMELEMENT_FIRE );
			int nRegistPlus = nTempRegist - nRegist;

			pActor->SetFireRegist( nRegistPlus );

			return 0;
		}

		int SetIceRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetIceRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nTempRegist = lua_tointeger( pState, nCurrentStack++ );
			const SRESIST& sRegist = pActor->GetBasicRegist();
			short nRegist = sRegist.GetElement( EMELEMENT_ICE );
			int nRegistPlus = nTempRegist - nRegist;

			pActor->SetIceRegist( nRegistPlus );

			return 0;
		}

		int SetElectricRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetElectricRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nTempRegist = lua_tointeger( pState, nCurrentStack++ );
			const SRESIST& sRegist = pActor->GetBasicRegist();
			short nRegist = sRegist.GetElement( EMELEMENT_ELECTRIC );
			int nRegistPlus = nTempRegist - nRegist;

			pActor->SetElectricRegist( nRegistPlus );

			return 0;
		}

		int SetPoisonRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetPoisonRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nTempRegist = lua_tointeger( pState, nCurrentStack++ );
			const SRESIST& sRegist = pActor->GetBasicRegist();
			short nRegist = sRegist.GetElement( EMELEMENT_POISON );
			int nRegistPlus = nTempRegist - nRegist;

			pActor->SetPoisonRegist( nRegistPlus );

			return 0;

		}
		int SetSpiritRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetSpiritRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nTempRegist = lua_tointeger( pState, nCurrentStack++ );
			const SRESIST& sRegist = pActor->GetBasicRegist();
			short nRegist = sRegist.GetElement( EMELEMENT_SPIRIT );
			int nRegistPlus = nTempRegist - nRegist;

			pActor->SetSpiritRegist( nRegistPlus );

			return 0;

		}

		int SetAccuracyRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetAccuracyRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nAccuracyRate = lua_tointeger( pState, nCurrentStack++ );
			int nAccuracyRatePlus = nAccuracyRate - pActor->GetBasicHitRate();

			pActor->SetAccuracyRate( nAccuracyRatePlus );

			return 0;
		}

		int SetAvoidanceRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetAvoidanceRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const int nAvoidanceRate = lua_tointeger( pState, nCurrentStack++ );
			int nAvoidanceRatePlus = nAvoidanceRate - pActor->GetBasicAvoid();

			pActor->SetAvoidanceRate( nAvoidanceRatePlus );

			return 0;
		}

		int SetHPRestorationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetHPRestorationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const float fRate = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
			float fRatePlus = fRate - pActor->GetBasicHPRestorationRate();

			pActor->SetHPRestorationRate( fRatePlus );

			return 0;
		}

		int SetMPRestirationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMPRestirationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const float fRate = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
			float fRatePlus = fRate - pActor->GetBasicMPRestorationRate();

			pActor->SetMPRestorationRate( fRatePlus );

			return 0;
		}

		int SetSPRestirationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetSPRestirationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const float fRate = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
			float fRatePlus = fRate - pActor->GetBasicSPRestorationRate();

			pActor->SetSPRestorationRate( fRatePlus );

			return 0;
		}

		int SetMultipleHP ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMultipleHP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const float fMultipleValue = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
			pActor->SetMultipleHP( fMultipleValue );

			return 0;
		}

		int SetMultipleAttack ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMultipleAttack ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const float fMultipleValue = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
			pActor->SetMultipleAttack( fMultipleValue );

			return 0;
		}

		int SetMultipleDefense ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMultipleDefense ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const float fMultipleValue = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
			pActor->SetMultipleDefense( fMultipleValue );

			return 0;
		}

		int SetMultipleExp ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMultipleExp ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const float fMultipleValue = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
			pActor->SetMultipleExp( fMultipleValue );

			return 0;
		}

		int SetMultipleDrop ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMultipleDrop ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			const float fMultipleValue = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
			pActor->SetMultipleDrop( fMultipleValue );

			return 0;
		}


		int ResetOffensivePower ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetOffensivePower ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetOffence( 0 );

			return 0;
		}

		int ResetDefensivePower ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetDefensivePower ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetDefence( 0 );

			return 0;
		}

		int ResetFireRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResSetFireRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetFireRegist( 0 );

			return 0;
		}

		int ResetIceRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetIceRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetIceRegist( 0 );

			return 0;
		}

		int ResetElectricRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetElectricRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetElectricRegist( 0 );

			return 0;
		}

		int ResetPoisonRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResSetPoisonRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetPoisonRegist( 0 );

			return 0;
		}

		int ResetSpiritRegistance ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetSpiritRegistance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetSpiritRegist( 0 );

			return 0;
		}

		int ResetAccuracyRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetAccuracyRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetAccuracyRate( 0 );

			return 0;
		}

		int ResetAvoidanceRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetAvoidanceRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetAvoidanceRate( 0 );

			return 0;
		}

		int ResetHPRestorationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetHPRestorationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetHPRestorationRate( 0.f );

			return 0;
		}

		int ResetMPRestirationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetMPRestirationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetMPRestorationRate( 0.f );

			return 0;
		}

		int ResetSPRestirationRate ( lua_State* pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( pState, nCurrentStack );
			if ( NULL == pActor )
			{
				return 0;
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetSPRestirationRate ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			pActor->SetSPRestorationRate( 0.f );

			return 0;
		}

        int SetVisible(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            const DWORD _bVisible(lua_toboolean(pState, currentStack++));

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* const _pChar = (GLChar*)pActor;

                    if ( _bVisible )
                        _pChar->ReSetActState(EM_REQ_VISIBLEOFF);
                    else
                        _pChar->SetActState(EM_REQ_VISIBLEOFF);

                    GLMSG::NET_INSTANCE_SET_ACT_STATE_BRD NetMsgBrd(_pChar->GetGaeaID(), EM_REQ_VISIBLEOFF, (!_bVisible) );
                    _pChar->SendMsgViewAround(&NetMsgBrd);
                    gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(),&NetMsgBrd);
                }
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                break;

            default:
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetVisible ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetMoveLock(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            const bool _bLock(lua_toboolean(pState, currentStack++));

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* const _pChar = (GLChar*)pActor;

                    _pChar->SetMovement(CHAR_MOVEMENT_INSTANCE_SCRIPT, _bLock);
                }
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                break;

            default:
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMoveLock ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetHpVisible(lua_State* pState)
        {
            int currentStack(1);
            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);
            GLLandManager* const _pLanManager(gpGaeaServer->GetLandManager(_instanceMapID));
            if ( _pLanManager == 0 )
                return 0;

            const bool _bHpVisible(lua_toboolean(pState, currentStack++));

            _pLanManager->setHpVisible(_bHpVisible ? Faction::EM_RELATION_FLAG_ALL : Faction::EM_RELATION_FLAG_NONE);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - GetFactionList ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int CreateFaction(lua_State* pState)
        {
			// 구현할 필요가 없는 부분;
            /*int currentStack(1);
            const DWORD _FactionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _FactionID_Num((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);
            GLLandManager* const _pLanManager(gpGaeaServer->GetLandManager(_instanceMapID));
            if ( _pLanManager == 0 )
                return 0;

            const Faction::FactionID _factionID((Faction::EMFACTION_TYPE)_FactionType, _FactionID_Num);
            _pLanManager->createFaction(_factionID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - CreateFaction ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif
*/
            return 0;
        }

        int SetFaction(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _ActorType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _ActorID_Num((DWORD)lua_tonumber(pState, currentStack++));  // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID, 파티의 경우 PartyID, 클럽의 경우 ClubID;
            const DWORD _FactionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _FactionID_Num((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);
            GLLandManager* const _pLanManager(gpGaeaServer->GetLandManager(_instanceMapID));
            if ( _pLanManager == 0 )
                return 0;

            const Faction::GLActorID _actorID((EMCROW)_ActorType, _ActorID_Num);
            const Faction::FactionID _factionID((Faction::EMFACTION_TYPE)_FactionType, _FactionID_Num);
            _pLanManager->setFactionID(_actorID, _factionID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetFaction ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetRelation(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _factionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _factionID_Num((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _targetFactionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _targetFactionID_Num((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _relation((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandManager(gpGaeaServer->GetLandManager(_instanceMapID));
            if ( _pLandManager == 0 )
                return 0;

            Faction::FactionID _myFaction(_factionType, _factionID_Num);
            Faction::FactionID _targetFaction(_targetFactionType, _targetFactionID_Num);
            _pLandManager->setRelation(_myFaction, _targetFaction, (Faction::EMRELATION_TYPE)_relation);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetRelation ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetPositionInstance(lua_State* pState)
        { // 인던 안에서의 특정 위치로 이동;
            int currentStack(1);

            const DWORD _actorType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _actorID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nMainMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nSubMapID((DWORD)lua_tonumber(pState, currentStack++));
            const float _fPositionX(lua_tonumber(pState, currentStack++));
            const float _fPositionZ(lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            switch( _actorType )
            { // 아래 목록에 포함되지 않는 것들은 이동 시킬 수 없다 (InstanceAgent::_frameMoveSetPosition에도 예외 처리 되어 있음);
            case CROW_PC:
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                // 몹 스케쥴러등의 개편이 이루어지고 나면 적용 해야 한다. 현재로썬 힘들다;
            default:
                return 0;
            }

            // 최초 이동 요청인 경우 field단에서는 생성 여부에 상관 없이 사용 가능 하지만,;
            // targetMap은 아직 생성 되어 있지 않은 점에 유의 해야 한다;
            const InformationPosition _reserveSetPosition(
                Faction::GLActorID(_actorType, _actorID),
                SNATIVEID(_nMainMapID, _nSubMapID),                 
                Faction::GLActorID(CROW_NULL, GAEAID_NULL), 
                _fPositionX, _fPositionZ, InstanceSystem::EMMOVE_INSTANCE2INSTANCE);
            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            const bool _bSuccess(_pManager->requestMoveToPosition(_instanceMapID, _reserveSetPosition));		

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetPositionInstance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetObjectPositionInstance(lua_State* pState)
        {// 인던 안에서의 특정 Object(PC, MOB 포함) 위치로 이동;
            int currentStack(1);

            const DWORD _actorType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _actorID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nMainMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nSubMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _targetActorType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _targetActorID((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            switch( _actorType )
            { // 아래 목록에 포함되지 않는 것들은 이동 시킬 수 없다 (InstanceAgent::_frameMoveSetPosition에도 예외 처리 되어 있음);
            case CROW_PC:
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                // 몹 스케쥴러등의 개편이 이루어지고 나면 적용 해야 한다. 현재로썬 힘들다;
            default:
                return 0;
            }

            // 최초 이동 요청인 경우 field단에서는 생성 여부에 상관 없이 사용 가능 하지만,;
            // targetMap은 아직 생성 되어 있지 않은 점에 유의 해야 한다;
            const InformationPosition _reserveSetPosition(
                Faction::GLActorID((EMCROW)_actorType, _actorID), 
                SNATIVEID(_nMainMapID, _nSubMapID),                 
                Faction::GLActorID((EMCROW)_targetActorType, _targetActorID), 
                0.0f, 0.0f, InstanceSystem::EMMOVE_INSTANCE2INSTANCE);
            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            const bool _bSuccess(_pManager->requestMoveToPosition(_instanceMapID, _reserveSetPosition));

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetObjectPositionInstance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetPositionField(lua_State* pState)
        { // 인던 안에서 바깥 필드의 특정 위치로의 이동;
            int currentStack(1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nMainMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nSubMapID((DWORD)lua_tonumber(pState, currentStack++));
            const float _fPositionX(lua_tonumber(pState, currentStack++));
            const float _fPositionZ(lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            // SetObjectPositionInstance()/SetPositionInstance() 와는 달리,
            // player만 인던 바깥으로 이동 시킬 수 있다;
            const InformationPosition _reserveSetPosition(
                Faction::GLActorID(CROW_PC, _CharDbNum), 
                SNATIVEID(_nMainMapID, _nSubMapID),                 
                Faction::GLActorID(CROW_NULL, GAEAID_NULL), 
                _fPositionX, _fPositionZ, InstanceSystem::EMMOVE_INSTANCE2FIELD);
            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            const bool _bSuccess(_pManager->requestMoveToPosition(_instanceMapID, _reserveSetPosition));

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetPositionField ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetGatePositionField(lua_State* pState)
        { // 인던 안에서 바깥 필드의 게이트 위치로의 이동;
            int currentStack(1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nMainMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nSubMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nGateID((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            // SetObjectPositionInstance()/SetPositionInstance() 와는 달리,
            // player만 인던 바깥으로 이동 시킬 수 있다;
            const InformationPosition _reserveSetPosition(
                Faction::GLActorID(CROW_PC, _CharDbNum), 
                SNATIVEID(_nMainMapID, _nSubMapID),                 
                Faction::GLActorID(CROW_GATE, _nGateID), 
                0.0f, 0.0f, InstanceSystem::EMMOVE_INSTANCE2FIELD);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            const bool _bSuccess(_pManager->requestMoveToPosition(_instanceMapID, _reserveSetPosition));

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetGatePosition ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }


		int SetReturnPosition ( lua_State* pState )
		{
			int nCurrentStack = 1;

			// Parameter;
			const DWORD dwChaDBNum = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );
			const DWORD dwMainMapID = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );
			const DWORD dwSubMapID = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );
			const DWORD dwGateID = static_cast< DWORD >( lua_tonumber( pState, nCurrentStack++ ) );

			// Key Map ID;
			lua_getglobal( pState, "keyMapID" );
			const DWORD dwKeyMapID = static_cast< DWORD >( lua_tonumber( pState, -1 ) );
			lua_pop( pState, 1 );

			// Instance ID;
			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID nInstanceMapID = static_cast< DWORD >( lua_tonumber( pState, -1 ) );
			lua_pop( pState, 1 );

			// Return Map Information
			MapInformation sReturnMapInformation = MapInformation(
				SNATIVEID( dwMainMapID, dwSubMapID ),
				D3DXVECTOR3( 0.f, 0.f, 0.f ),
				Faction::GLActorID( CROW_GATE, dwGateID ) );

			// Agent 에 알려 갱신한다;
			GLMSG::NET_SET_RETURNMAP_INFORMATION_FA sNetMsgFA( nInstanceMapID, SNATIVEID( dwKeyMapID ) );
			sNetMsgFA.dwChaDBNum = dwChaDBNum;
			sNetMsgFA.sReturnMapInformation = sReturnMapInformation;
			gpGaeaServer->SENDTOAGENT( &sNetMsgFA );

			return 0;
		}

        int SetResurrectionForced(lua_State* pState)  // 플레이어를 부활 시킨다;
        {
            int currentStack(1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nMainMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nSubMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nGateID((DWORD)lua_tonumber(pState, currentStack++));
            const float _fTimer(lua_tonumber(pState, currentStack++));
            const float _fRestore(lua_tonumber(pState, currentStack++));

            GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
            if ( !_pChar )
                return 0;

            _pChar->SetResurrectionForced(false, SNATIVEID(_nMainMapID, _nSubMapID), _nGateID, D3DXVECTOR3(0.0f, 0.0f, 0.0f), _fTimer, _fRestore);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetResurrectionForced ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }
		int SetResurrectionForcedPosition(lua_State* pState) // 강제 부활 설정 (필드 맵으로 부활);
		{
			int currentStack(1);

			const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _nMainMapID((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _nSubMapID((DWORD)lua_tonumber(pState, currentStack++));
			const int _positionX(lua_tonumber(pState, currentStack++));			
			const int _positionZ(lua_tonumber(pState, currentStack++));
			const float _fTimer(lua_tonumber(pState, currentStack++));
			const float _fRestore(lua_tonumber(pState, currentStack++));

			GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
			if ( !_pChar )
				return 0;

			_pChar->SetResurrectionForced(false, SNATIVEID(_nMainMapID, _nSubMapID), GAEAID_NULL, D3DXVECTOR3(_positionX, 0.0f, _positionZ), _fTimer, _fRestore);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetResurrectionForcedPosition ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

        int SetResurrectionForcedInstance(lua_State* pState)  // 플레이어를 부활 시킨다;
        {
            int currentStack(1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nMainMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nSubMapID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nGateID((DWORD)lua_tonumber(pState, currentStack++));
            const float _fTimer(lua_tonumber(pState, currentStack++));
            const float _fRestore(lua_tonumber(pState, currentStack++));

            GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
            if ( !_pChar )
                return 0;

            _pChar->SetResurrectionForced(true, SNATIVEID(_nMainMapID, _nSubMapID), _nGateID, D3DXVECTOR3(0.0f, 0.0f, 0.0f), _fTimer, _fRestore);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetResurrectionForcedInstance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }
		int SetResurrectionForcedInstancePosition(lua_State* pState) // 강제 부활 설정 (인던 맵으로 부활);
		{
			int currentStack(1);

			const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _nMainMapID((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _nSubMapID((DWORD)lua_tonumber(pState, currentStack++));
			const int _positionX(lua_tonumber(pState, currentStack++));			
			const int _positionZ(lua_tonumber(pState, currentStack++));
			const float _fTimer(lua_tonumber(pState, currentStack++));
			const float _fRestore(lua_tonumber(pState, currentStack++));

			GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
			if ( !_pChar )
				return 0;

			_pChar->SetResurrectionForced(true, SNATIVEID(_nMainMapID, _nSubMapID), GAEAID_NULL, D3DXVECTOR3(_positionX, 0.0f, _positionZ), _fTimer, _fRestore);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetResurrectionForcedInstancePosition ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

        int SetResurrectionDisable(lua_State* pState)  // 플레이어를 부활 방지 시킨다;
        {
            int currentStack(1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            const bool _bDisableSkill(lua_toboolean(pState, currentStack++));

            GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
            if ( !_pChar )
                return 0;

            _pChar->SetResurrectionDisable(_bDisableSkill);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetResurrectionDisable ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetResurrectionDisableSkill(lua_State* pState)  // 플레이어를 부활 방지 시킨다;
        {
            int currentStack(1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            const bool _bDisableSkill(lua_toboolean(pState, currentStack++));

            GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
            if ( !_pChar )
                return 0;

            _pChar->SetResurrectionDisableSkill(_bDisableSkill);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetResurrectionDisableSkill ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetDeathPenalty(lua_State* pState)  // 사망 패널티 설정;
        {
            int currentStack(1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nExpPenalty((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nDropNum((DWORD)lua_tonumber(pState, currentStack++));
            const float _fDropRate(lua_tonumber(pState, currentStack++));

            GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_CharDbNum);
            if ( !_pChar )
                return 0;

            _pChar->DeathPenalty(_nExpPenalty, _nDropNum, _fDropRate);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetDeathPenalty ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetJoinSuspendInstance(lua_State* pState) // 특정 인던에 대해 접속을 지연 시킴;
        {
            int currentStack(1);

            const bool _bLock(lua_toboolean(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLMSG::NET_INSTANCE_JOIN_SUSPEND_FA _joinLockMessage(_instanceMapID, SNATIVEID(false), _bLock);
            gpGaeaServer->SENDTOMYSELF(&_joinLockMessage);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetJoinSuspendInstance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetJoinLockInstance(lua_State* pState) // 특정 인던에 대해 접속을 제한 함;
        {
            int currentStack(1);

            const bool _bLock(lua_toboolean(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLMSG::NET_INSTANCE_JOIN_LOCK_FA _joinLockMessage(_instanceMapID, SNATIVEID(false), _bLock);
            gpGaeaServer->SENDTOMYSELF(&_joinLockMessage);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetJoinLockInstance ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetJoinLockType(lua_State* pState) // 특정 종류에 속하는 모든 인던에 대해 접속을 제한 함;
        {
            int currentStack(1);

            const bool _bLock(lua_toboolean(pState, currentStack++));

            lua_getglobal(pState, "keyMapID");
            const DWORD _nKeyMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLMSG::NET_INSTANCE_JOIN_LOCK_FA _joinLockMessage(GAEAID_NULL, _nKeyMapID, _bLock);
            gpGaeaServer->SENDTOMYSELF(&_joinLockMessage);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetJoinLockType ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

		int RetryFaction ( lua_State* pState )
		{
			int currentStack = 1;

			// Faction ID;
			Faction::FactionID sFactionID;
			const DWORD emFactionType = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );
			const DWORD dwFactionID = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );

			// 시간과 액수;
			const DWORD dwTime = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );
			const DWORD dwMoney = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );

			sFactionID.factionType = static_cast< Faction::EMFACTION_TYPE >( emFactionType );
			sFactionID.factionID_Num = dwFactionID;

			// Instance ID;
			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tonumber( pState, -1 ) );
			lua_pop( pState, 1 );

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
			if ( NULL == pLandManager )
				return 0;

			InstanceSystem::InstanceField* pInstance =
				pLandManager->GetScriptInstance();
			if ( NULL == pInstance )
				return 0;

			SRETRY_INFO sRetryInfo;
			sRetryInfo.bRetry = true;
			sRetryInfo.dwMoney = dwMoney;
			sRetryInfo.fRetryTime = static_cast< float >( dwTime );
			sRetryInfo.sFactionID = sFactionID;

			pInstance->RetryFaction( sRetryInfo );

			return 0;
		}

		/*int ShowTimeUI ( lua_State* pState )
		{
			int currentStack = 1;

			const DWORD dwTime = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );
			const bool bShow = lua_toboolean( pState, currentStack++ );

			// Instance ID;
			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tonumber( pState, -1 ) );
			lua_pop( pState, 1 );

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
			if ( NULL == pLandManager )
				return 0;

			InstanceSystem::InstanceField* pInstance =
				pLandManager->GetScriptInstance();
			if ( NULL == pInstance )
				return 0;

			pInstance->ShowTimeUI( static_cast< const float >( dwTime ), bShow );

			return 0;
		}

		int ShowEntranceStateUI ( lua_State* pState )
		{
			int currentStack = 1;

			// Faction ID;
			Faction::FactionID sFactionID;
			const DWORD emFactionType = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );
			const DWORD dwFactionID = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );

			sFactionID.factionType = static_cast< Faction::EMFACTION_TYPE >( emFactionType );
			sFactionID.factionID_Num = dwFactionID;

			const DWORD dwCurCount = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );
			const DWORD dwMaxCount = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );
			const DWORD dwTime = static_cast< DWORD >(
				lua_tonumber( pState, currentStack++ ) );
			const bool bShow = lua_toboolean( pState, currentStack++ );

			// Instance ID;
			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tonumber( pState, -1 ) );
			lua_pop( pState, 1 );

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
			if ( NULL == pLandManager )
				return 0;

			InstanceSystem::InstanceField* pInstance =
				pLandManager->GetScriptInstance();
			if ( NULL == pInstance )
				return 0;

			pInstance->ShowEntranceStateUI( sFactionID, dwCurCount, dwMaxCount, static_cast< const float >( dwTime ), bShow );

			return 0;
		}*/

        int SetStageState(lua_State* pState) // 무한의 계단에서 사용하는 UI (사망대기 등등.. 을사용함);
        {
            // 			ERESULT_SUCCEED,        //@ 성공
            // 			ERESULT_FAILED,         //@ 실패
            // 			ERESULT_WAITAFTERDEATH, //@ 죽음 대기
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            GLChar* const _pChar = (GLChar*)pActor;


            // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID, 파티의 경우 PartyID, 클럽의 경우 ClubID;
            const DWORD _GameType((DWORD)lua_tonumber(pState, currentStack++));
            const bool _isShow(lua_toboolean(pState, currentStack++));  

            DWORD genType = GLMSG::STAGE_STATE_ONGOING;

            switch (_GameType)
            {
            case 0:
                genType = GLMSG::STAGE_STATE_GAMEROVER_COMPLETED;
                break;

            case 1:
                genType = GLMSG::STAGE_STATE_DIED;
                break;
            }

            GLMSG::NET_MATCHING_STAGE_STATE_FC NetMsgBRD(genType,_isShow);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), &NetMsgBRD);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetStageState ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int SetViewRange(lua_State* pState)
        {
            //* GLchar 에서 아래의 내용을 참고 할것! *//
            //1. View Around는 성능에 매우 큰 저하를 가져다 올수 있으므로 수치조정에 주의하여야 한다.;
            //   맵이동("맵의 위치가 바꼈을때 퀘스트 실패 여부"와 같음) 과 ;
            //   안전시간(m_fViewAroundSettingReleaseTimer)을 통해 풀린다. ( 혹시나 컨텐츠를 벗어나서 동작하는걸 방지하기 위함 );
            //2. MAX_VIEWRANGE 보다 작을경우 동작하지 않는다. (의도된 동작임);
            int currentStack(1);

            LuaPlus::LuaObject _DbNumObj(LuaPlus::LuaState::CastState(pState), currentStack++);
            if ( _DbNumObj.IsInteger() == false )
                return 0;
            const int _nDbNum( _DbNumObj.GetInteger() );

            LuaPlus::LuaObject _RangeObj(LuaPlus::LuaState::CastState(pState), currentStack++);
            if ( _RangeObj.IsInteger() == false )
                return 0;
            const int _nViewRange( _RangeObj.GetInteger() );

            float _nDuration(60.0f);
            LuaPlus::LuaObject _DurationObj(LuaPlus::LuaState::CastState(pState), currentStack++);
            if ( _DurationObj.IsNumber() )
            {
                _nDuration = _DurationObj.GetFloat();
            }

            if(_nViewRange <=0)
                return 0;

            if(_nDuration <=0)
                return 0;

            GLChar* const _pChar = gpGaeaServer->GetCharByDbNum(_nDbNum);
            if(_pChar == NULL)
                return 0;

            // 뷰어라운드 세팅;
            _pChar->SetViewAround((unsigned int)_nViewRange,_nDuration);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetViewRange ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

		int SetCrowMultipleHP(lua_State* pState)
		{
			int currentStack(1);
			const DWORD _mapMid((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _mapSid((DWORD)lua_tonumber(pState, currentStack++));
			const float _multipleHp(lua_tonumber(pState, currentStack++));

			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager(_instanceMapID);
			if ( pLandManager == NULL )
				return 0;

			GLLandMan* pLand = pLandManager->getLand(SNATIVEID(_mapMid, _mapSid));
			if ( pLand == NULL )
				return 0;

			pLand->SetCrowMultipleHP(_multipleHp);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetCrowMultipleHP ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

		int SetCrowMultipleAttack(lua_State* pState)
		{
			int currentStack(1);
			const DWORD _mapMid((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _mapSid((DWORD)lua_tonumber(pState, currentStack++));
			const float _multipleHp(lua_tonumber(pState, currentStack++));

			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager(_instanceMapID);
			if ( pLandManager == NULL )
				return 0;

			GLLandMan* pLand = pLandManager->getLand(SNATIVEID(_mapMid, _mapSid));
			if ( pLand == NULL )
				return 0;

			pLand->SetCrowMultipleAttack(_multipleHp);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetCrowMultipleAttack ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

		int SetCrowMultipleDefence(lua_State* pState)
		{
			int currentStack(1);
			const DWORD _mapMid((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _mapSid((DWORD)lua_tonumber(pState, currentStack++));
			const float _multipleHp(lua_tonumber(pState, currentStack++));

			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager(_instanceMapID);
			if ( pLandManager == NULL )
				return 0;

			GLLandMan* pLand = pLandManager->getLand(SNATIVEID(_mapMid, _mapSid));
			if ( pLand == NULL )
				return 0;

			pLand->SetCrowMultipleDefense(_multipleHp);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetCrowMultipleDefence ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

		int SetCrowMultipleExp(lua_State* pState)
		{
			int currentStack(1);
			const DWORD _mapMid((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _mapSid((DWORD)lua_tonumber(pState, currentStack++));
			const float _multipleHp(lua_tonumber(pState, currentStack++));

			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager(_instanceMapID);
			if ( pLandManager == NULL )
				return 0;

			GLLandMan* pLand = pLandManager->getLand(SNATIVEID(_mapMid, _mapSid));
			if ( pLand == NULL )
				return 0;

			pLand->SetCrowMultipleExp(_multipleHp);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetCrowMultipleExp ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

		int SetCrowMultipleDrop(lua_State* pState)
		{
			int currentStack(1);
			const DWORD _mapMid((DWORD)lua_tonumber(pState, currentStack++));
			const DWORD _mapSid((DWORD)lua_tonumber(pState, currentStack++));
			const float _multipleHp(lua_tonumber(pState, currentStack++));

			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager(_instanceMapID);
			if ( pLandManager == NULL )
				return 0;

			GLLandMan* pLand = pLandManager->getLand(SNATIVEID(_mapMid, _mapSid));
			if ( pLand == NULL )
				return 0;

			pLand->SetCrowMultipleDrop(_multipleHp);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetCrowMultipleDrop ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}


        // Motion;
        int SetMotion(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            const DWORD _motionMainID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _motionSubID((DWORD)lua_tonumber(pState, currentStack++));
            const float _motionSec(lua_tonumber(pState, currentStack++));

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* const _pChar = (GLChar*)pActor;

                    const float _motionTime((_motionSec == 0.0f) ? FLT_MAX : _motionSec);
                    _pChar->SetMotion((EMANI_MAINTYPE)_motionMainID, (EMANI_SUBTYPE)_motionSubID, _motionTime, boost::bind( boost::mem_fn( &GLChar::EventMotionFinish ), _pChar ) );
                    GLMSG::NET_INSTANCE_SET_MOTION_BRD NetMsgBrd(_pChar->GetGaeaID(), _motionMainID, _motionSubID, _motionTime);
                    _pChar->SendMsgViewAround(&NetMsgBrd);
                    gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(),&NetMsgBrd);
                }
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                break;

            default:
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SetMotion ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int IsMotion(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            const DWORD _motionMainID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _motionSubID((DWORD)lua_tonumber(pState, currentStack++));

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* const _pChar = (GLChar*)pActor;
                    const bool _bReturn = _pChar->IsMotion((EMANI_MAINTYPE)_motionMainID, (EMANI_SUBTYPE)_motionSubID);
                    lua_pushboolean(pState, _bReturn);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
					std::string tempLogString = "[ Instance Log ] [ Invoke - IsMotion ]";
					ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

                    return 1;
                }
                break;

            case CROW_MOB:
            case CROW_NPC:
            case CROW_MATERIAL:
                break;

            default:
                break;
            }

            return 0;
        }

        int ResetMotion(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor = _getActor(pState, currentStack);
            if ( pActor == NULL )
            {
                return 0;
            }

            if ( pActor->isAction(GLAT_MOTION) )
            {
                pActor->TurnAction(GLAT_IDLE);
                GLMSG::SNET_ACTION_BRD NetMsgBrd;
                NetMsgBrd.emCrow	= pActor->GetCrow();
                NetMsgBrd.dwID		= pActor->GetGaeaID();
                NetMsgBrd.emAction	= GLAT_IDLE;
                pActor->SendMsgViewAround(&NetMsgBrd);

                if( pActor->GetCrow() == CROW_PC )
                {
                    gpGaeaServer->SENDTOCLIENT(pActor->GETCLIENTID(),&NetMsgBrd);
                }
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResetMotion ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        

        // Mob;
        int AddMobLayer ( lua_State* pState )
        {
            int nCurrentStack = 1;

            DWORD dwMapMID = 0;
            DWORD dwMapSID = 0;
			DWORD dwLayerIndex = 0;			// Once Layer 의 번호이다;

			dwMapMID = static_cast< DWORD >( lua_tointeger( pState, nCurrentStack++ ) );
			dwMapSID = static_cast< DWORD >( lua_tointeger( pState, nCurrentStack++ ) );
			dwLayerIndex = static_cast< DWORD >( lua_tointeger( pState, nCurrentStack++ ) );

			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tointeger( pState, -1 ) );
			lua_pop( pState, 1 );  

            GLLandManager* pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
            if ( NULL == pLandManager )
                return 0;

            GLLandMan* pLand = pLandManager->getLand( SNATIVEID( dwMapMID, dwMapSID ) );
            if ( NULL == pLand )
                return 0;

            pLand->AddMobSchManLayer( dwLayerIndex, false );
			//pLand->CopyBaseLayer( dwLayerIndex, dwCopyLayerIndex );

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - AddMobLayer ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

		int DelMobLayer ( lua_State* pState )
		{
			int currentStack = 1 ;

			DWORD dwMapMID = 0;
			DWORD dwMapSID = 0;
			DWORD dwLayerIndex = 0;			// Once Layer 의 번호이다;
			bool bOnce = true;

			dwMapMID = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
			dwMapSID = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
			dwLayerIndex = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
			bOnce = static_cast< bool >( lua_toboolean( pState, currentStack++ ) );

			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tointeger( pState, -1 ) );
			lua_pop( pState, 1 );

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
			if ( NULL == pLandManager )
				return 0;

			GLLandMan* pLand = pLandManager->getLand( SNATIVEID( dwMapMID, dwMapSID ) );
			if ( NULL == pLand )
				return 0;

			if ( bOnce )
				pLand->DelMobSchManOnceLayer( dwLayerIndex );
			else
				pLand->DelMobSchManRegenLayer( dwLayerIndex );

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DelMobLayer ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

		int DelAllMobLayer ( lua_State* pState )
		{
			int currentStack = 1 ;

			DWORD dwMapMID = 0;
			DWORD dwMapSID = 0;
			bool bOnce = true;

			dwMapMID = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
			dwMapSID = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );
			bOnce = static_cast< bool >( lua_toboolean( pState, currentStack++ ) );

			lua_getglobal( pState, "instanceMapID" );
			const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tointeger( pState, -1 ) );
			lua_pop( pState, 1 );

			GLLandManager* pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
			if ( NULL == pLandManager )
				return 0;

			GLLandMan* pLand = pLandManager->getLand( SNATIVEID( dwMapMID, dwMapSID ) );
			if ( NULL == pLand )
				return 0;

			if ( bOnce )
				pLand->DelAllMobSchManOnceLayer();
			else
				pLand->DelAllMobSchManRegenLayer();

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DelAllMobLayer ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

			return 0;
		}

        int DoDestroy(lua_State* pState)
        {
            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            _pManager->requestDestroy(_instanceMapID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoDestroy ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int DoOut(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLMSG::NET_INSTANCE_REQUEST_OUT _outMessage(_instanceMapID, _CharDbNum);
            gpGaeaServer->SENDTOAGENT(&_outMessage);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoOut ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif
            return 0;
        }

        int DoJoin(lua_State* pState)
        {
            int currentStack(1);

			lua_getglobal(pState, "instanceMapID");
			const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
			lua_pop(pState, 1);

            const DWORD _CharDbNum((DWORD)lua_tonumber(pState, currentStack++));
			LuaPlus::LuaObject _firstReturnValue(LuaPlus::LuaState::CastState(pState), currentStack++);
			LuaPlus::LuaObject _secondReturnValue(LuaPlus::LuaState::CastState(pState), currentStack++);

			if ( _firstReturnValue.IsNumber() == false )
			{
				// 리턴값이 없다면 초기 위치 변경 안함;
				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _CharDbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(), 
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				GLMSG::NET_INSTANCE_REQUEST_JOIN _joinMessage(_informationPosition, InstanceSystem::EMAUTHORITY_NORMAL_FORCED);
				gpGaeaServer->SENDTOAGENT(&_joinMessage);
			}
			else if ( _secondReturnValue.IsNumber() == false )
			{
				// 리턴 값이 1개일 때는 게이트 번호를 뜻한다.;
				const int _nGateID( _firstReturnValue.GetInteger() );

				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _CharDbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(CROW_GATE, _nGateID), 
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				GLMSG::NET_INSTANCE_REQUEST_JOIN _joinMessage(_informationPosition, InstanceSystem::EMAUTHORITY_NORMAL_FORCED);
				gpGaeaServer->SENDTOAGENT(&_joinMessage);
			}
			else
			{
				// 리턴 값이 2개일 때는 월드 좌표를 뜻한다.;
				const float _fPositionX( _firstReturnValue.GetFloat() );
				const float _fPositionZ( _secondReturnValue.GetFloat() );

				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _CharDbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(), 
					_fPositionX, _fPositionZ, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				GLMSG::NET_INSTANCE_REQUEST_JOIN _joinMessage(_informationPosition, InstanceSystem::EMAUTHORITY_NORMAL_FORCED);
				gpGaeaServer->SENDTOAGENT(&_joinMessage);
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoJoin ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int DoMoveToPosition(lua_State* pState)
        { // 인던 안에서의 특정 위치로 이동;
            int currentStack(1);

            const DWORD _nActorType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nActorID((DWORD)lua_tonumber(pState, currentStack++)); // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID;
            const float _fPositionX(lua_tonumber(pState, currentStack++));
            const float _fPositionY(lua_tonumber(pState, currentStack++));
            const float _fPositionZ(lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            const Faction::GLActorID _actorID(_nActorType, _nActorID);
            const D3DXVECTOR3 _vPosition(_fPositionX, _fPositionY, _fPositionZ);
            GLMSG::NET_INSTANCE_DO_MOVE_TO _doMoveMessage(_instanceMapID, _actorID, Faction::GLActorID(), _vPosition);
            gpGaeaServer->SENDTOMYSELF(&_doMoveMessage);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoMoveToPosition ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int DoMoveToObjectPosition(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _nActorType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nActorID((DWORD)lua_tonumber(pState, currentStack++)); // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID;
            const DWORD _nTargetActorType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _nTargetActorID((DWORD)lua_tonumber(pState, currentStack++)); // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID;

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            const Faction::GLActorID _actorID(_nActorType, _nActorID);
            const Faction::GLActorID _targetActorID(_nTargetActorType, _nTargetActorID);
            GLMSG::NET_INSTANCE_DO_MOVE_TO _doMoveMessage(_instanceMapID, _actorID, _targetActorID);
            gpGaeaServer->SENDTOMYSELF(&_doMoveMessage);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoMoveToObjectPosition ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

		int DoEffectPositionAll(lua_State* pState)
		{
			int currentStack(1);
			
            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( pLandMan == NULL )
                return 0;

			LuaPlus::LuaObject _effectNameObj(LuaPlus::LuaState::CastState(pState), currentStack++);
			if ( _effectNameObj.IsString() == false )
				return 0;

			const char* _effectName = _effectNameObj.GetString();
            const WORD _wMapMID((WORD)lua_tonumber(pState, currentStack++));
            const WORD _wMapSID((WORD)lua_tonumber(pState, currentStack++));
            const float _fXPos((float)lua_tonumber(pState, currentStack++));
            const float _fYPos((float)lua_tonumber(pState, currentStack++));
            const float _fZPos((float)lua_tonumber(pState, currentStack++));

            GLMSG::NET_INSTANCE_EFFECT NetMsg;
            NetMsg._strEffectName = _effectName;
            NetMsg._fEffectZonePosX = _fXPos;
            NetMsg._fEffectZonePosY = _fYPos;
            NetMsg._fEffectZonePosZ = _fZPos;
			//DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            GLLandMan* _pLand = pLandMan->getLand(SNATIVEID(_wMapMID, _wMapSID));
            if ( _pLand == NULL )
                return 0;

            _pLand->SendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoEffectPositionAll ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
		}

        int DoEffectActor(lua_State* pState)	    // (액터에게 부착되는)이펙트를 생성한다. 특정 클라이언트에게만;
        {
            int currentStack(1);
            const DWORD _charDbNum((DWORD)lua_tonumber(pState, currentStack++ ));

            GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_charDbNum);
            if ( _pChar == NULL )
                return 0;

            LuaPlus::LuaObject _effectNameObj(LuaPlus::LuaState::CastState(pState), currentStack++);
            if ( _effectNameObj.IsString() == false )
                return 0;

            const char* _effectName = _effectNameObj.GetString();
            ServerActor* pActor(_getActor(pState,currentStack));
            if ( pActor == NULL )
                return 0;

            GLMSG::NET_INSTANCE_EFFECT NetMsg;
            NetMsg._strEffectName = _effectName;
            NetMsg._targetActorType = pActor->GetCrow();
            NetMsg._targetActorID= pActor->GetGaeaID();

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            gpGaeaServer->SENDTOCLIENT(_pChar->GETCLIENTID(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoEffectActor ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int DoEffectActorAll(lua_State* pState)	// (액터에게 부착되는)이펙트를 생성한다.;
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( pLandMan == NULL )
                return 0;

            LuaPlus::LuaObject _effectNameObj(LuaPlus::LuaState::CastState(pState), currentStack++);
            if ( _effectNameObj.IsString() == false )
                return 0;

            const char* _effectName = _effectNameObj.GetString();
            ServerActor* pActor(_getActor(pState,currentStack));
            if ( pActor == NULL )
                return 0;

            GLMSG::NET_INSTANCE_EFFECT NetMsg;
            NetMsg._strEffectName = _effectName;
            NetMsg._targetActorType = pActor->GetCrow();
            NetMsg._targetActorID= pActor->GetGaeaID();

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DoEffectActorAll ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int RegistActorTrigger(lua_State* pState)
        {
            int currentStack(1);

            const EMCROW _ActorType((EMCROW)lua_tointeger(pState, currentStack++));
            const DWORD _ActorID_Num((DWORD)lua_tonumber(pState, currentStack++));  // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID;
            const DWORD _triggerType((DWORD)lua_tonumber(pState, currentStack++));
            const float _triggerRange(lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            const bool _bReturn(_pManager->reserveRegistActorTrigger(_instanceMapID, Faction::GLActorID(_ActorType, _ActorID_Num), _triggerType, _triggerRange));
            lua_pushboolean(pState, _bReturn);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RegistActorTrigger ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 1;
        }

        int RegistActorZoneTrigger(lua_State* pState)
        {
            int currentStack(1);

            const EMCROW _ActorType((EMCROW)lua_tointeger(pState, currentStack++));
            const DWORD _ActorID_Num((DWORD)lua_tonumber(pState, currentStack++));  // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID;
            const DWORD _triggerType((DWORD)lua_tonumber(pState, currentStack++));
            const float _triggerZoneX(lua_tonumber(pState, currentStack++));
            const float _triggerZoneZ(lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            //const bool _bReturn(_pManager->registActorTrigger(_instanceMapID, Faction::GLActorID(_ActorType, _ActorID_Num), _triggerType, _triggerZoneX, _triggerZoneZ, _triggerZoneY));
            const bool _bReturn(_pManager->reserveRegistActorTrigger(_instanceMapID, Faction::GLActorID(_ActorType, _ActorID_Num), _triggerType, _triggerZoneX, _triggerZoneZ, 100.0f));
            lua_pushboolean(pState, _bReturn);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RegistActorZoneTrigger ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 1;
        }

        int RemoveActorTrigger(lua_State* pState)
        {
            int currentStack(1);

            const EMCROW _ActorType((EMCROW)lua_tointeger(pState, currentStack++));
            const DWORD _ActorID_Num((DWORD)lua_tonumber(pState, currentStack++));  // 주의! : PC의 경우 DbNum, 몹의 경우 GlobalID;

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            const bool _bReturn(_pManager->reserveRemoveActorTrigger(_instanceMapID, Faction::GLActorID(_ActorType, _ActorID_Num)));
            lua_pushboolean(pState, _bReturn);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RemoveActorTrigger ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 1;
        }

        int DropCrow(lua_State* pState)
        {
            int currentStack(1);

            const DWORD _mapMainID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _mapSubID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _crowMainID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _crowSubID((DWORD)lua_tonumber(pState, currentStack++));
            const float _posX(lua_tonumber(pState, currentStack++));
            const float _posZ(lua_tonumber(pState, currentStack++));
            const float _rotate(lua_tonumber(pState, currentStack++));
			const DWORD dwLayerIndex = static_cast< DWORD >( lua_tonumber( pState, currentStack++ ) );

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            const SNATIVEID _crowID(_crowMainID, _crowSubID);
            const SNATIVEID _mapID(_mapMainID, _mapSubID);
            DWORD dwGaeaID(0);
            GLLandManager* pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( pLandMan )
            {
                GLLandMan* pLand = pLandMan->getLand(_mapID);
                if ( pLand )
                {
                    dwGaeaID = pLand->DropCrow(_crowID, _posX, _posZ, _rotate, dwLayerIndex);
                    if ( dwGaeaID == UINT_MAX )
                        dwGaeaID = 0;
                }
            }

            lua_pushinteger(pState, dwGaeaID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - DropCrow ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 1;
        }

        int ApplySkill(lua_State* pState)
        {
            int nCurrentStack = 1;

            ServerActor* pActor = _getActor( pState, nCurrentStack );
            const WORD wSkillMID = static_cast< WORD >( lua_tointeger( pState, nCurrentStack++ ) );
            const WORD wSkillSID = static_cast< WORD >( lua_tointeger( pState, nCurrentStack++ ) );
            const WORD wSkillLevel = static_cast< WORD >( lua_tointeger( pState, nCurrentStack++ ) );
            const float fSkillTime = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );
            const DWORD dwSkillFactType = static_cast< DWORD >( lua_tointeger( pState, nCurrentStack++ ) );
			const float fSkillLifeTime = static_cast< float >( lua_tonumber( pState, nCurrentStack++ ) );

            if ( NULL == pActor )
                return 0;

            SNATIVEID sSkillID( wSkillMID, wSkillSID );
            STARGETID sTargetID( CROW_NULL, GAEAID_NULL );
            SSKILLFACT::STIME sSkillTime( fSkillTime );
			sSkillTime.fLIFE = ( fSkillLifeTime <= 0 ) ? fSkillTime : fSkillLifeTime;
            EMSKILLFACT_TYPE skillFactType( static_cast< EMSKILLFACT_TYPE >( dwSkillFactType ) );

            DWORD dwSlot = 0;
            if ( false == pActor->RECEIVE_SKILLFACT(
				sSkillID,
				wSkillLevel,
				dwSlot,
				sTargetID,
				sSkillTime,
				_SKILLFACT_DEFAULTCUSTOMCOUNT,
				skillFactType ) )
            {
                return 0;
            }

            if ( UINT_MAX == dwSlot )
            {
                return 0;
            }

            GLMSG::SNETPC_SKILLHOLD_BRD NetMsgBRD;
            NetMsgBRD.emCrow = pActor->GetCrow();
            NetMsgBRD.dwID = pActor->GetGaeaID();
            NetMsgBRD.skill_id = sSkillID;
            NetMsgBRD.wLEVEL = wSkillLevel;
            NetMsgBRD.wSELSLOT = static_cast< WORD >( dwSlot );
            NetMsgBRD.sTIME = sSkillTime;
            NetMsgBRD.sID = sTargetID;

            // 스킬대상과 주변 클라이언트들에게 메세지 전송;
            pActor->SendMsgViewAround( &NetMsgBRD );

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    // 유저의 경우 자신에게도 메세지 전송;
                    pActor->SendToClient( &NetMsgBRD );
                }
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ApplySkill ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }


		int ApplySkill_Faction ( lua_State* _pState )
		{
			int nCurrentStack = 1;

			const DWORD dwFactionType = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
			const DWORD dwFactionID = static_cast< DWORD >( lua_tonumber( _pState, nCurrentStack++ ) );
			const WORD wSkillMID = static_cast< WORD >( lua_tointeger( _pState, nCurrentStack++ ) );
			const WORD wSkillSID = static_cast< WORD >( lua_tointeger( _pState, nCurrentStack++ ) );
			const WORD wSkillLevel = static_cast< WORD >( lua_tointeger( _pState, nCurrentStack++ ) );
			const float fSkillTime = static_cast< float >( lua_tonumber( _pState, nCurrentStack++ ) );
			const DWORD dwSkillFactType = static_cast< DWORD >( lua_tointeger( _pState, nCurrentStack++ ) );

			lua_getglobal( _pState, "instanceMapID" );
			const InstanceMapID sInstanceMapID = static_cast< DWORD >( lua_tonumber( _pState, -1 ) );
			lua_pop( _pState, 1 );

			if ( NULL == gpGaeaServer )
				return 0;

			GLLandManager* const pLandManager = gpGaeaServer->GetLandManager( sInstanceMapID );
			if ( NULL == pLandManager )
				return 0;

			Faction::ManagerField* const pFactionManager = pLandManager->GetFactionManager();
			if ( NULL == pFactionManager )
				return 0;

			Faction::FactionID sFactionID( dwFactionType, dwFactionID );
			const Faction::FactionMemberVector* vecFactionMember = pFactionManager->getFactionMembers( sFactionID );
			if ( NULL == vecFactionMember )
				return 0;

			if ( vecFactionMember->empty() )
				return 0;

			Faction::FactionMemberVectorCIter bIter = vecFactionMember->begin();
			Faction::FactionMemberVectorCIter eIter = vecFactionMember->end();
			for ( ; bIter != eIter; bIter++ )
			{
				ServerActor* pActor = NULL;
				switch( bIter->actorType )
				{
				case CROW_PC:
					{
						GLChar* const pChar(gpGaeaServer->GetCharByDbNum( bIter->actorID_Num ));
						if ( NULL == pChar )
							break;

						pActor = pChar;
					}
					break;

				case CROW_MOB:
				case CROW_NPC:
				case CROW_MATERIAL:
					{
						GLCrow* const pCrow = pLandManager->getCrow( bIter->actorID_Num );
						if ( NULL == pCrow ) 
							break;

						pActor = pCrow;
					}
					break;
				}

				if ( NULL == pActor )
					return 0;

				SNATIVEID sSkillID( wSkillMID, wSkillSID );
				STARGETID sTargetID( CROW_NULL, GAEAID_NULL );
				SSKILLFACT::STIME sSkillTime( fSkillTime );
				EMSKILLFACT_TYPE skillFactType( static_cast< EMSKILLFACT_TYPE >( dwSkillFactType ) );

				DWORD dwSlot = 0;
				if ( false == pActor->RECEIVE_SKILLFACT(
					sSkillID,
					wSkillLevel,
					dwSlot,
					sTargetID,
					sSkillTime,
					_SKILLFACT_DEFAULTCUSTOMCOUNT,
					skillFactType ) )
				{
					return 0;
				}

				if ( UINT_MAX == dwSlot )
				{
					return 0;
				}

				GLMSG::SNETPC_SKILLHOLD_BRD NetMsgBRD;
				NetMsgBRD.emCrow = pActor->GetCrow();
				NetMsgBRD.dwID = pActor->GetGaeaID();
				NetMsgBRD.skill_id = sSkillID;
				NetMsgBRD.wLEVEL = wSkillLevel;
				NetMsgBRD.wSELSLOT = static_cast< WORD >( dwSlot );
				NetMsgBRD.sTIME = sSkillTime;
				NetMsgBRD.sID = sTargetID;

				// 스킬대상과 주변 클라이언트들에게 메세지 전송;
				pActor->SendMsgViewAround( &NetMsgBRD );

				switch( pActor->GetCrow() )
				{
				case CROW_PC:
					{
						// 유저의 경우 자신에게도 메세지 전송;
						pActor->SendToClient( &NetMsgBRD );
					}
					break;
				}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
				std::string tempLogString = "[ Instance Log ] [ Invoke - ApplySkill_Faction ]";
				ServerLog_InstanceScriptSystem_InstanceFunction( _pState, tempLogString );
#endif

				return 0;
			}
		}

        int IsHaveSkillFact(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor(_getActor(pState,currentStack));
            const WORD  _nSkillMID((WORD)lua_tointeger(pState, currentStack++));
            const WORD  _nSkillSID((WORD)lua_tointeger(pState, currentStack++));

            if ( pActor == NULL )
                return 0;

            switch( pActor->GetCrow() )
            {
            case CROW_PC:
                {
                    GLChar* pChar = (GLChar*)pActor;
                    SNATIVEID skillID(_nSkillMID, _nSkillSID);
                    WORD wOutSlot;
                    const bool bHave = pChar->IS_HAVE_SKILLFACT(skillID, wOutSlot);
                    lua_pushboolean(pState, bHave);
                    return 1;
                }
                break;
            case CROW_NPC:
            case CROW_MOB:
                {
                    GLCrow* pCrow = (GLCrow*)pActor;
                    SNATIVEID skillID(_nSkillMID, _nSkillSID);
                    WORD wOutSlot;
                    const bool bHave = pCrow->IS_HAVE_SKILLFACT(skillID, wOutSlot);
                    lua_pushboolean(pState, bHave);
                    return 1;                
                }
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - IsHaveSkillFact ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int RemoveSkillFact(lua_State* pState)
        {
            int currentStack(1);
            ServerActor* pActor(_getActor(pState,currentStack));
            const WORD  _nSkillMID((WORD)lua_tointeger(pState, currentStack++));
            const WORD  _nSkillSID((WORD)lua_tointeger(pState, currentStack++));

            if ( pActor == NULL )
                return 0;
            
            SNATIVEID skillID(_nSkillMID, _nSkillSID);
            pActor->REMOVE_SKILLFACT(skillID);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RemoveSkillFact ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

		int RemoveSkillFact_SkillEffect ( lua_State* _pState )
		{
			int nCurrentStack = 1;

			ServerActor* pActor = _getActor( _pState, nCurrentStack );

			const EMSPEC_ADDON _emSkillEffect = static_cast< EMSPEC_ADDON >( lua_tointeger( _pState, nCurrentStack++ ) );

			if ( NULL == pActor )
				return 0;

			pActor->REMOVE_SKILLFACT( _emSkillEffect );

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RemoveSkillFact_SkillEffect ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( _pState, tempLogString );
#endif

			return 0;
		}

        ///////////////////////// Timer //////////////////////////////
        int RegistTimer(lua_State* pState)
        {  
            int currentStack(1);

            const float _nTimeSec((float)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam0((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam1((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam2((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _dwParam3((DWORD)lua_tonumber(pState, currentStack++));
            const bool _bLoop(lua_toboolean(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());
            const TimerHandle _hTimer(_pManager->registTimer(_instanceMapID, _nTimeSec, _dwParam0, _dwParam1, _dwParam2, _dwParam3, _bLoop));
            lua_pushinteger(pState, _hTimer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RegistTimer ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 1;
        }

        int RemoveTimer(lua_State* pState)
        {
            int currentStack(1);

            if ( lua_isnumber(pState, currentStack) == false )
                return 0;

            const DWORD nTimerHandle((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());		
            const bool _bReturn(_pManager->removeTimer(_instanceMapID, nTimerHandle));		

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - RemoveTimer ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }	

        int SuspendTimer(lua_State* pState) // 타이머를 멈추어둠;
        {
            int currentStack(1);

            if ( lua_isnumber(pState, currentStack) == false )
                return 0;

            const unsigned int  nTimerHandle((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());		
            const bool _bReturn(_pManager->suspendTimer(_instanceMapID, nTimerHandle));		

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SuspendTimer ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int ResumeTimer(lua_State* pState) // 멈추어둔 타이머를 재개함;	
        {
            int currentStack(1);

            if ( lua_isnumber(pState, currentStack) == false )
                return 0;

            const unsigned int  nTimerHandle((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());		
            const bool _bReturn(_pManager->resumeTimer(_instanceMapID, nTimerHandle));		

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - ResumeTimer ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int IsRegistTimer(lua_State* pState) // 타이머 등록 여부 확인;
        {
            int currentStack(1);

            if ( lua_isnumber(pState, currentStack) == false )
                return 0;

            const unsigned int  nTimerHandle((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());		
            const bool _bReturn(_pManager->isRegistTimer(_instanceMapID, nTimerHandle));
            lua_pushboolean(pState, _bReturn);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - IsRegistTimer ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 1;
        }	

        int GetLeftTime(lua_State* pState) /// 남은 시간 값을 얻어 온다;
        {
            int currentStack(1);

            if ( lua_isnumber(pState, currentStack) == false )
                return 0;

            const unsigned int  nTimerHandle((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            InstanceSystem::ManagerField* const _pManager(gpGaeaServer->GetInstanceManagerField());		
            const int _nReturn(_pManager->getLeftTime(_instanceMapID, nTimerHandle));
            lua_pushinteger(pState, _nReturn);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			//std::string tempLogString = "[ Instance Log ] [ Invoke - GetLeftTime ]";
			//ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 1;
        }

		//////////////////////// SendMessage ////////////////////////////
        int SendMessage(lua_State* pState)
        {
            int currentStack(1);
            const DWORD _DestType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _DestID((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _Param1((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _Param2((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _Param3((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _Param4((DWORD)lua_tonumber(pState, currentStack++));

            lua_getglobal(pState, "instanceMapID");
            const DWORD _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);
            lua_getglobal(pState, "keyMapID");
            const DWORD _keyMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE netMsg(_Param1, _Param2, _Param3, _Param4);
            netMsg.emSrcType = EM_MESSAGE_FIELD_INSTANCE;
            netMsg.dwSrcID = _instanceMapID;
            netMsg.emDestType = _DestType;
            netMsg.dwDestID = _DestID;

            switch ( _DestType )
            {
            case EM_MESSAGE_USER:
                {
                    GLChar* _pChar = gpGaeaServer->GetCharByDbNum(_DestID);
                    if ( _pChar )
                    {
                        gpGaeaServer->SENDTOCLIENT(_pChar->ClientSlot(), &netMsg);
                    }
                    else
                    {
                        gpGaeaServer->SENDTOAGENT(&netMsg);
                    }
                }
                break;

            //case EM_MESSAGE_MY_INSTANCE:    // netMsg.dwDestID 의미없음;
            case EM_MESSAGE_FIELD_BASE:
                {
                    sc::LuaInstance* const _pLuaInstance(InstanceSystem::getScript(_keyMapID));

                    if ( _pLuaInstance->PushFunctionEx("luaFieldEventCustomMessage") == true )
                    {
                        _pLuaInstance->PushInteger(EM_MESSAGE_FIELD_INSTANCE);
                        _pLuaInstance->PushInteger(_instanceMapID);
                        _pLuaInstance->PushInteger(_Param1);
                        _pLuaInstance->PushInteger(_Param2);
                        _pLuaInstance->PushInteger(_Param3);
                        _pLuaInstance->PushInteger(_Param4);
                        _pLuaInstance->CallFunction(6, 0);
                    }
                }
                break;

            case EM_MESSAGE_FIELD_INSTANCE:
            case EM_MESSAGE_AGENT_BASE:
            case EM_MESSAGE_AGENT_INSTANCE:
                {
                    gpGaeaServer->SENDTOAGENT(&netMsg);
                }
                break;
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

		//////////////////////////  U I  /////////////////////////////////////
        int PrintChatMsgFaction(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            Faction::ManagerField* const pFactionMan = _pLandMan->GetFactionManager();
            if ( pFactionMan == NULL )
                return 0;

            const DWORD _factionID_Num((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _factionType((DWORD)lua_tonumber(pState, currentStack++));


            GLMSG::NET_INSTANCE_UI_CHATMSG NetMsg;
            if ( _makeChatMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

			const bool _bAlly = lua_toboolean( pState, currentStack++ );

            msgpack::sbuffer SendBuffer;
            msgpack::pack( SendBuffer, NetMsg );

			if ( _bAlly )
			{
				pFactionMan->sendMessageToAllyFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}
			else
			{
				pFactionMan->sendMessageToFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintChatXMLMsgFaction(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            Faction::ManagerField* const pFactionMan = _pLandMan->GetFactionManager();
            if ( pFactionMan == NULL )
                return 0;

            const DWORD _factionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _factionID_Num((DWORD)lua_tonumber(pState, currentStack++));

            GLMSG::NET_INSTANCE_UI_CHATXMLMSG NetMsg;
            if ( _makeChatXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

			const bool _bAlly = lua_toboolean( pState, currentStack++ );

			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, NetMsg );

			if ( _bAlly )
			{
				pFactionMan->sendMessageToAllyFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}
			else
			{
				pFactionMan->sendMessageToFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintChatMsgAll(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_CHATMSG NetMsg;
            if ( _makeChatMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintChatXMLMsgAll(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_CHATXMLMSG NetMsg;
            if ( _makeChatXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintSimpleMsgFaction(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            Faction::ManagerField* const pFactionMan = _pLandMan->GetFactionManager();
            if ( pFactionMan == NULL )
                return 0;

            const DWORD _factionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _factionID_Num((DWORD)lua_tonumber(pState, currentStack++));

            GLMSG::NET_INSTANCE_UI_SIMPLE_MSG NetMsg;
            if ( _makeSimpleMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

			const bool _bAlly = lua_toboolean( pState, currentStack++ );

			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, NetMsg );

			if ( _bAlly )
			{
				pFactionMan->sendMessageToAllyFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}
			else
			{
				pFactionMan->sendMessageToFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintSimpleXMLMsgFaction(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            Faction::ManagerField* const pFactionMan = _pLandMan->GetFactionManager();
            if ( pFactionMan == NULL )
                return 0;

            const DWORD _factionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _factionID_Num((DWORD)lua_tonumber(pState, currentStack++));

            GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG NetMsg;
            if ( _makeSimpleXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

			const bool _bAlly = lua_toboolean( pState, currentStack++ );

			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, NetMsg );

			if ( _bAlly )
			{
				pFactionMan->sendMessageToAllyFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}
			else
			{
				pFactionMan->sendMessageToFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintSimpleMsgAll(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_SIMPLE_MSG NetMsg;
            if ( _makeSimpleMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintSimpleXMLMsgAll(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG NetMsg;
            if ( _makeSimpleXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintTextDlgMsgFaction(lua_State* pState)  // 화면 상단 System메시지 다이얼로그 박스 출력 ( 해당 진영에게 );
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            Faction::ManagerField* const pFactionMan = _pLandMan->GetFactionManager();
            if ( pFactionMan == NULL )
                return 0;

            const DWORD _factionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _factionID_Num((DWORD)lua_tonumber(pState, currentStack++));

            GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG NetMsg;
            if ( _makeTextDlgMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

			const bool _bAlly = lua_toboolean( pState, currentStack++ );

			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, NetMsg );

			if ( _bAlly )
			{
				pFactionMan->sendMessageToAllyFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}
			else
			{
				pFactionMan->sendMessageToFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintTextDlgXMLMsgFaction(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            Faction::ManagerField* const pFactionMan = _pLandMan->GetFactionManager();
            if ( pFactionMan == NULL )
                return 0;

            const DWORD _factionType((DWORD)lua_tonumber(pState, currentStack++));
            const DWORD _factionID_Num((DWORD)lua_tonumber(pState, currentStack++));

            GLMSG::NET_INSTANCE_UI_TEXT_DLG_XMLMSG NetMsg;
            if ( _makeTextDlgXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

			const bool _bAlly = lua_toboolean( pState, currentStack++ );

			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, NetMsg );

			if ( _bAlly )
			{
				pFactionMan->sendMessageToAllyFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}
			else
			{
				pFactionMan->sendMessageToFaction(
					Faction::FactionID( _factionType, _factionID_Num ),
					NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
					SendBuffer );
			}

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintTextDlgMsgAll(lua_State* pState)  // 화면 상단 System메시지 다이얼로그 박스 출력 ( 해당 인던 전체에 );
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG NetMsg;
            if ( _makeTextDlgMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int PrintTextDlgXMLMsgAll(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_TEXT_DLG_XMLMSG NetMsg;
            if ( _makeTextDlgXmlMsgPacket(pState, currentStack, NetMsg) == false )
                return 0;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - SendMessage ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int UI_TimerMessageBox(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            bool bMyInstance(false);
            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetChar(_nDbNum);
            if ( _pChar )
            {
                GLLandMan* _pLand(_pChar->GetLandMan());
                if ( _pLand )
                {
                    InstanceSystem::InstanceField* _pScript(_pLand->GetScriptInstance());
                    if ( _pScript )
                    {
                        // 자신이 해당 인던에 속해있는지 검사;
                        if ( _instanceMapID == _pLand->GetGaeaMapID().wMainID )
                        {
                            bMyInstance = true;
                        }
                    }
                }
            }

            GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

            _makeTimerMsgBoxPacket(pState, currentStack, NetMsg);
            
            if ( bMyInstance )
            {
                // 현재 인던에 해당유저가 속해있는 경우;
                // 응답을 현재 인던으로 설정;
                NetMsg.emResponseType = InstanceSystem::EM_MESSAGE_MY_INSTANCE;
            }
            else
            {
                // 현재 인던에 해당 유저가 속해있지 않는 경우;
                // 응답받을 인던의 ID를 알려줘서 응답할 수 있도록 설정;
                NetMsg.emResponseType = InstanceSystem::EM_MESSAGE_FIELD_INSTANCE;
                NetMsg.dwInstanceID = _instanceMapID;
            }
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);

            if ( _pChar )
            {
                // 현재 서버에 유저가 접속해있는경우 바로 전송;
                gpGaeaServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);
            }
            else
            {   
                // 유저가 다른 서버에 접속해 있는 경우 Agent서버를 경유하여 전송;
                NetMsg.dwDbNum = _nDbNum;
                gpGaeaServer->SENDTOAGENT(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - UI_TimerMessageBox ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int UI_TimerMessageBoxAll(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

            _makeTimerMsgBoxPacket(pState, currentStack, NetMsg);

            NetMsg.emResponseType = InstanceSystem::EM_MESSAGE_MY_INSTANCE;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - UI_TimerMessageBoxAll ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int UI_XmlTimerMessageBox(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            bool bMyInstance(false);
            const DWORD _nDbNum((DWORD)lua_tonumber(pState, currentStack++));
            GLChar* const _pChar = gpGaeaServer->GetChar(_nDbNum);
            if ( _pChar )
            {
                GLLandMan* _pLand(_pChar->GetLandMan());
                if ( _pLand )
                {
                    InstanceSystem::InstanceField* _pScript(_pLand->GetScriptInstance());
                    if ( _pScript )
                    {
                        // 자신이 해당 인던에 속해있는지 검사;
                        if ( _instanceMapID == _pLand->GetGaeaMapID().wMainID )
                        {
                            bMyInstance = true;
                        }
                    }
                }
            }

            GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

            _makeXmlTimerMsgBoxPacket(pState, currentStack, NetMsg);

            if ( bMyInstance )
            {
                // 현재 인던에 해당유저가 속해있는 경우;
                // 응답을 현재 인던으로 설정;
                NetMsg.emResponseType = InstanceSystem::EM_MESSAGE_MY_INSTANCE;
            }
            else
            {
                // 현재 인던에 해당 유저가 속해있지 않는 경우;
                // 응답받을 인던의 ID를 알려줘서 응답할 수 있도록 설정;
                NetMsg.emResponseType = InstanceSystem::EM_MESSAGE_FIELD_INSTANCE;
                NetMsg.dwInstanceID = _instanceMapID;
            }
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);

            if ( _pChar )
            {
                // 현재 서버에 유저가 접속해있는경우 바로 전송;
                gpGaeaServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);
            }
            else
            {   
                // 유저가 다른 서버에 접속해 있는 경우 Agent서버를 경유하여 전송;
                NetMsg.dwDbNum = _nDbNum;
                gpGaeaServer->SENDTOAGENT(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);
            }

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - UI_XmlTimerMessageBox ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }

        int UI_XmlTimerMessageBoxAll(lua_State* pState)
        {
            int currentStack(1);

            lua_getglobal(pState, "instanceMapID");
            const InstanceMapID _instanceMapID((DWORD)lua_tonumber(pState, -1));
            lua_pop(pState, 1);

            GLLandManager* const _pLandMan = gpGaeaServer->GetLandManager(_instanceMapID);
            if ( _pLandMan == NULL )
                return 0;

            GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

            _makeXmlTimerMsgBoxPacket(pState, currentStack, NetMsg);

            NetMsg.emResponseType = InstanceSystem::EM_MESSAGE_MY_INSTANCE;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsg);
            _pLandMan->sendMsgPC(NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
			std::string tempLogString = "[ Instance Log ] [ Invoke - UI_XmlTimerMessageBoxAll ]";
			ServerLog_InstanceScriptSystem_InstanceFunction( pState, tempLogString );
#endif

            return 0;
        }
    }
}