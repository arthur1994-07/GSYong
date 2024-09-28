#pragma once

#include "../../SigmaCore/Lua/MinLua.h"

namespace InstanceSystem
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Field Public Function;
    void InitializeFieldPublicInvokeFunction(void);   

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Field Instance Fuction;
    void InitializeFieldInstanceInvokeFunction(sc::LuaInstance& _luaInstance);

    void registFunctionField(InstanceSystem::ScriptManager* const _pScriptManager); //public funtion : Field 서버 공용 Invoke함수;
    void registFunctionField(sc::LuaInstance& _luaInstance);

    //void registFunctionFieldInstance(InstanceSystem::ScriptManager* const _pScriptManager); //Instance funtion : Agent 서버 인던 Instance Script용 Invoke함수 (인던내에서만 사용가능);
    void registFunctionFieldInstance(sc::LuaInstance& _luaInstance);

    namespace FieldScriptFunction
    {
        //////////////////////////////////////////////////////////////////////////////////////////////
        //public funtion : Field 서버 공용 Invoke함수;
        //////////////////////////////////////////////////////////////////////////////////////////////
        
        int GetLevel(lua_State* pState); // 액터 Level 확인 / param1 : DWORD 해당 액터ID (DBNum or GaeaID), return : int Level;
        int GetName(lua_State* pState);  // 유저 Name 확인 / param1 : DWROD 해당 캐릭터 DBNum, return : char* Name;
		int GetClass(lua_State* pState);  // 유저의 클레스 리턴 / param : DWORD 해당 캐릭터 DBNum, return : DWORD SchoolNum;
        int GetSchool(lua_State* pState);  // 유저의 소속학원 확인 / param : DWORD 해당 캐릭터 DBNum, return : DWORD SchoolNum;
        int GetClubID(lua_State* pState);  // 유저의 클럽 DBNum 확인 / param : DWORD 해당 캐릭터 DBNum, return : DWORD ClubDBNum;
        int GetClubName(lua_State* pState);  // 클럽 이름 확인 / param : DWORD 해당 ClubDBNum, return : char* ClubName;
        int GetClubMasterName(lua_State* pState); // 클럽 마스터의 이름 확인 / param : DWORD 해당 ClubDBNum, return : char* ClubMasterName;
        int GetClubRank(lua_State* pState); // 클럽 랭크;

		int IsAllianceClub ( lua_State* _pState );	// 두 클럽이 동맹클럽인지 확인한다;

        int GetParty(lua_State* pState);
        int GetPartyID(lua_State* pState);
        int GetPartyMember(lua_State* pState); // return : 파티원 DbNum;
        int GetPartyMemberNum(lua_State* pState);
        int GetPartyMaster(lua_State* pState);  // return : 파티장 DbNum;
		int GetFieldSvrID(lua_State* pState);	// Field Server ID 번호
		int GetServerChannel(lua_State* pState);	// Field Server ID 번호

        int GetPosFromMapPos(lua_State* pState);
        int GetMapPosFromPos(lua_State* pState);
		int GetCountry(lua_State* pState);
		int GetCountryName(lua_State* pState);

		// Set;
		int ActiveFactionChatting(lua_State* pState);
		int InactiveFactionChatting(lua_State* pState);
		int SetCountryMark(lua_State* pState);

		// Exp;
		int AddExp(lua_State* pState);

        // SendXXX;
        int SendSystemMail(lua_State* pState);

        // DoXXX;
        int DoEffectPosition(lua_State* pState);	// 이펙트를 생성한다. 특정 클라이언트에게만;

        // party;
        int JoinParty(lua_State* pState);  // 파티 생성 or 파티원 추가;
        int OutParty(lua_State* pState);  // 파티 탈퇴;
        int ClearParty(lua_State* pState);  // 파티 해산;

		// Item;
		int GetItemCount(lua_State* pState);
		int AddItem(lua_State* pState);
		int RemoveItem(lua_State* pState);

        // UI;
        int PrintChatMsg(lua_State* pState);  // 채팅창에 System메시지를 출력 ( 특정 인원에게 );
        int PrintChatXMLMsg(lua_State* pState);

        int PrintSimpleMsg(lua_State* pState);  // 화면 중상단에 텍스트 출력;
        int PrintSimpleXMLMsg(lua_State* pState);

        int PrintTextDlgMsg(lua_State* pState);  // 화면 상단 텍스트 다이얼로그 박스 출력;
        int PrintTextDlgXMLMsg(lua_State* pState); 


        //////////////////////////////////////////////////////////////////////////////////////////////
        //Instance funtion : Field 서버 인던 Instance Script용 Invoke함수 (인던내에서만 사용가능);
        ///////////////////////////////////////////////////////////////////////////////////////////////
        
        // getXXX;
        int GetHP(lua_State* pState);  // 액터 HP 확인 / param1 : DWORD 해당 액터ID (DBNum or GaeaID), return : int HP;
        int GetMP(lua_State* pState);
        int GetSP(lua_State* pState);
		int GetMaxHP(lua_State* pState);  // 액터 HP 설정 / param1 : DWORD 해당 액터ID (DBNum or GaeaID), param2 : int HP, return : None;
		int GetMaxMP(lua_State* pState);
		int GetMaxSP(lua_State* pState);

        int GetPosition(lua_State* pState);

        int GetFaction(lua_State* pState);  // 해당 설정자의 진영 확인 /  param1 : DWORD 설정자 Type,  param2 : DWORD 설정자 ID,  return : DWORD 진영 ID; 
        int GetFactionList(lua_State* pState);  // 해당 설정자의 진영 확인 /  param1 : DWORD 설정자 Type,  param2 : DWORD 설정자 ID,  return : Table < DWORD > ID 리스트; 

		/////////////////////////////////////////////////////////////////////////////////////
		// 현재는 Crow 만을 위한 Function;
		int GetOffensivePower ( lua_State* pState );			// 공격력;
		int GetDefensivePower ( lua_State* pState );			// 방어력;
		int GetFireRegistance ( lua_State* pState );			// 화염 저항력;
		int GetIceRegistance ( lua_State* pState );				// 냉기 저항력;
		int GetElectricRegistance ( lua_State* pState );		// 전기 저항력;
		int GetPoisonRegistance ( lua_State* pState );			// 맹독 저항력;
		int GetSpiritRegistance ( lua_State* pState );			// 정기 저항력;
		int GetAccuracyRate ( lua_State* pState );				// 명중률;
		int GetAvoidanceRate ( lua_State* pState );				// 회피율;
		int GetHPRestorationRate ( lua_State* pState );			// HP 회복률;
		int GetMPRestirationRate ( lua_State* pState );			// MP 회복률;
		int GetSPRestirationRate ( lua_State* pState );			// SP 회복률;

		int GetMultipleHP ( lua_State* pState );
		int GetMultipleAttack ( lua_State* pState );
		int GetMultipleDefense ( lua_State* pState );
		int GetMultipleExp ( lua_State* pState );
		int GetMultipleDrop ( lua_State* pState );
		/////////////////////////////////////////////////////////////////////////////////////

        // setXXX;
        int SetHP(lua_State* pState);  // 액터 HP 설정 / param1 : DWORD 해당 액터ID (DBNum or GaeaID), param2 : int HP, return : None;
        int SetMP(lua_State* pState);
        int SetSP(lua_State* pState);	


		/////////////////////////////////////////////////////////////////////////////////////
		// 현재는 Crow 만을 위한 Function;
		int SetOffensivePower ( lua_State* pState );			// 공격력;
		int SetDefensivePower ( lua_State* pState );			// 방어력;
		int SetFireRegistance ( lua_State* pState );			// 화염 저항력;
		int SetIceRegistance ( lua_State* pState );				// 냉기 저항력;
		int SetElectricRegistance ( lua_State* pState );		// 전기 저항력;
		int SetPoisonRegistance ( lua_State* pState );			// 맹독 저항력;
		int SetSpiritRegistance ( lua_State* pState );			// 정기 저항력;
		int SetAccuracyRate ( lua_State* pState );				// 명중률;
		int SetAvoidanceRate ( lua_State* pState );				// 회피율;
		int SetHPRestorationRate ( lua_State* pState );			// HP 회복률;
		int SetMPRestirationRate ( lua_State* pState );			// MP 회복률;
		int SetSPRestirationRate ( lua_State* pState );			// SP 회복률;

		int SetMultipleHP ( lua_State* pState );
		int SetMultipleAttack ( lua_State* pState );
		int SetMultipleDefense ( lua_State* pState );
		int SetMultipleExp ( lua_State* pState );
		int SetMultipleDrop ( lua_State* pState );

		int ResetOffensivePower ( lua_State* pState );			// 공격력;
		int ResetDefensivePower ( lua_State* pState );			// 방어력;
		int ResetFireRegistance ( lua_State* pState );			// 화염 저항력;
		int ResetIceRegistance ( lua_State* pState );			// 냉기 저항력;
		int ResetElectricRegistance ( lua_State* pState );		// 전기 저항력;
		int ResetPoisonRegistance ( lua_State* pState );		// 맹독 저항력;
		int ResetSpiritRegistance ( lua_State* pState );		// 정기 저항력;
		int ResetAccuracyRate ( lua_State* pState );			// 명중률;
		int ResetAvoidanceRate ( lua_State* pState );			// 회피율;
		int ResetHPRestorationRate ( lua_State* pState );		// HP 회복률;
		int ResetMPRestirationRate ( lua_State* pState );		// MP 회복률;
		int ResetSPRestirationRate ( lua_State* pState );		// SP 회복률;
		/////////////////////////////////////////////////////////////////////////////////////

        int SetVisible(lua_State* pState);
        int SetMoveLock(lua_State* pState);
        int SetHpVisible(lua_State* pState);

        int CreateFaction(lua_State* pState);  // 진영을 생성한다. ( SetFaction으로 생성한 Faction과는 다르게 인원이 0명일 경우에도 자동 파괴가 되지않는다.):
        int SetFaction(lua_State* pState);  // 해당 설정자의 진영 설정 / param1 : DWORD 설정자 Type,  param2 : DWORD 설정자 ID,  param3 : DWORD 설정 진영ID,  return : None;

        int SetRelation(lua_State* pState);  // 해당 진영과 타겟진영의 관계 설정 / param1 : DWORD 진영 Type,  param2 : DWORD 진영 ID,  param3 : DWORD 관계 타입,  return : None;

        int SetPositionInstance(lua_State* pState); // 인던 안에서의 특정 위치로 이동;
        int SetObjectPositionInstance(lua_State* pState); // 인던 안에서의 특정 Object 위치로 이동;
        int SetPositionField(lua_State* pState); // 인던 안에서 바깥 필드의 특정 위치로의 이동;
        int SetGatePositionField(lua_State* pState); // 인던 안에서 바깥 필드의 게이트 위치로의 이동;	

		int SetReturnPosition ( lua_State* pState );	// Out 될 시의 반환 위치 설정;

        int SetResurrectionForced(lua_State* pState); // 강제 부활 설정 (필드 맵으로 부활);
		int SetResurrectionForcedPosition(lua_State* pState); // 강제 부활 설정 (필드 맵으로 부활);
        int SetResurrectionForcedInstance(lua_State* pState); // 강제 부활 설정 (인던 맵으로 부활);
		int SetResurrectionForcedInstancePosition(lua_State* pState); // 강제 부활 설정 (인던 맵으로 부활);
		int SetResurrectionDisable(lua_State* pState);  // 플레이어를 부활 방지 시킨다 (스킬부활 방지도 함께 설정가능);
        int SetResurrectionDisableSkill(lua_State* pState);  // 플레이어를 스킬부활을 방지 시킨다;
        int SetDeathPenalty(lua_State* pState);  // 사망 패널티 설정;

        int SetJoinSuspendInstance(lua_State* pState); // 특정 인던에 대해 접속을 지연 시킴;
        int SetJoinLockInstance(lua_State* pState); // 특정 인던에 대해 접속을 제한 함;
        int SetJoinLockType(lua_State* pState); // 특정 종류에 속하는 모든 인던에 대해 접속을 제한 함;

		int RetryFaction ( lua_State* pState );	// Faction 재도전;
		//int ShowTimeUI ( lua_State* pState );	// Show Time UI;
		//int ShowEntranceStateUI ( lua_State* pState );	// Show Entrance State UI;

		int SetStageState(lua_State* pState); // 무한의 계단에서 사용하는 UI (사망대기 등등.. 을사용함);

        int SetViewRange(lua_State* pState); // 인던에서의 최대 시야거리를 조정함 (너무 넓은 시야는 서버 부화를 유발할 우려가 있음. 맵이동시 해제됨);

		int SetCrowMultipleHP(lua_State* pState);
		int SetCrowMultipleAttack(lua_State* pState);
		int SetCrowMultipleDefence(lua_State* pState);
		int SetCrowMultipleExp(lua_State* pState);
		int SetCrowMultipleDrop(lua_State* pState);

        // Motion
        int SetMotion(lua_State* pState);
        int IsMotion(lua_State* pState);
        int ResetMotion(lua_State* pState);

        // MobLayer
        int AddMobLayer(lua_State* pState);			// 특정 몹 레이어를 맵에 적용 ( 추가 됨, 리젠은 안됨 );
		int DelMobLayer(lua_State* pState);			// 특정 몹 레이어 제거;
        int DelAllMobLayer(lua_State* pState);		// 모든 몹 레이어 제거;

        // doXXX;	
        int DoDestroy(lua_State* pState);  // 해당 인던을 파괴 요청(종료시 사용) / param null, return : None;

        int DoOut(lua_State* pState); // 특정 플레이어를 쫓아냄;
        int DoJoin(lua_State* pState); // 특정 플레이어를 입장 요청 시킴;
        int DoMoveToPosition(lua_State* pState); // 특정 위치로 이동 시킴 (아직 미구현);
        int DoMoveToObjectPosition(lua_State* pState); // 특정 객체 위치로 이동 시킴 (아직 미구현);

        //int DoResurrect(lua_State* pState);  // 플레이어를 부활 시킨다;
        //int DoKillActor(lua_State* pState);  // 해당 액터를 죽인다.;

        int DoEffectPositionAll(lua_State* pState);	// 이펙트를 생성한다.;
        int DoEffectActor(lua_State* pState);	    // (액터에게 부착되는)이펙트를 생성한다. 특정 클라이언트에게만;
        int DoEffectActorAll(lua_State* pState);	// (액터에게 부착되는)이펙트를 생성한다.;

        // RegistXXX;
        int RegistActorTrigger(lua_State* pState);  // 액터 트리거를 등록;
        int RegistActorZoneTrigger(lua_State* pState);  // 액터 존 트리거를 등록 (이벤트 판정이 사각형인것 외에는 동일);
        int RemoveActorTrigger(lua_State* pState);  // 액터 트리거를 등록;

        // dropXXX;
        int DropCrow(lua_State* pState);

        // Skill;
        int ApplySkill(lua_State* pState);  // 스킬효과를 적용;
		int ApplySkill_Faction ( lua_State* _pState );	// Faction 에 스킬효과를 적용;
        int IsHaveSkillFact(lua_State* pState);  // 해당 스킬 효과가 있는지 검사;
        int RemoveSkillFact(lua_State* pState);  // 스킬효과를 해제;
		int RemoveSkillFact_SkillEffect(lua_State* pState);  // 스킬효과를 해제;

		// Timer;	
        int RegistTimer(lua_State* pState); // 타이머 등록;
        int RemoveTimer(lua_State* pState); // 타이머 삭제;
        int IsRegistTimer(lua_State* pState); // 타이머 등록 여부 확인;
        int SuspendTimer(lua_State* pState); // 타이머를 멈추어둠;
        int ResumeTimer(lua_State* pState); // 멈추어둔 타이머를 재개함;	
        int GetLeftTime(lua_State* pState); /// 남은 시간 값을 얻어 온다;

		// SendMessage;
        int SendMessage(lua_State* pState);

        // UI;
        int PrintChatMsgFaction(lua_State* pState);  // 채팅창에 System메시지를 출력 ( 해당 진영에게 );
        int PrintChatXMLMsgFaction(lua_State* pState);

        int PrintChatMsgAll(lua_State* pState);  // 채팅창에 System메시지를 출력 ( 해당 인던전체에 );
        int PrintChatXMLMsgAll(lua_State* pState);

        int PrintSimpleMsgFaction(lua_State* pState);  // 화면 중앙에 System메시지를 출력 ( 해당 진영에게 );
        int PrintSimpleXMLMsgFaction(lua_State* pState);

        int PrintSimpleMsgAll(lua_State* pState);  // 화면 중앙에 System메시지를 출력 ( 해당 인던전체에 );
        int PrintSimpleXMLMsgAll(lua_State* pState);

        int PrintTextDlgMsgFaction(lua_State* pState);  // 화면 상단 System메시지 다이얼로그 박스 출력 ( 해당 진영에게 );
        int PrintTextDlgXMLMsgFaction(lua_State* pState);

        int PrintTextDlgMsgAll(lua_State* pState);  // 화면 상단 System메시지 다이얼로그 박스 출력 ( 해당 인던 전체에 );
        int PrintTextDlgXMLMsgAll(lua_State* pState);

        int UI_TimerMessageBox(lua_State* pState);
        int UI_TimerMessageBoxAll(lua_State* pState);
        int UI_XmlTimerMessageBox(lua_State* pState);
        int UI_XmlTimerMessageBoxAll(lua_State* pState);



        //////////////////////////////////////////////////////////////////////////////////////////////
        //priavete Instance funtion : Field 서버용 스크립트에서 자주 사용되는 코드의 (중복코드 방지)함수모음;
        ///////////////////////////////////////////////////////////////////////////////////////////////
        ServerActor* const _getActor(lua_State* pState, int& currentStack);
        std::tr1::shared_ptr<GLClubField> _getClub(lua_State* pState, int& currentStack);
        GLPartyField* _getParty(lua_State* pState, int& currentStack);
    }
}
