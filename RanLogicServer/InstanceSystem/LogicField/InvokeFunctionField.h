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

    void registFunctionField(InstanceSystem::ScriptManager* const _pScriptManager); //public funtion : Field ���� ���� Invoke�Լ�;
    void registFunctionField(sc::LuaInstance& _luaInstance);

    //void registFunctionFieldInstance(InstanceSystem::ScriptManager* const _pScriptManager); //Instance funtion : Agent ���� �δ� Instance Script�� Invoke�Լ� (�δ��������� ��밡��);
    void registFunctionFieldInstance(sc::LuaInstance& _luaInstance);

    namespace FieldScriptFunction
    {
        //////////////////////////////////////////////////////////////////////////////////////////////
        //public funtion : Field ���� ���� Invoke�Լ�;
        //////////////////////////////////////////////////////////////////////////////////////////////
        
        int GetLevel(lua_State* pState); // ���� Level Ȯ�� / param1 : DWORD �ش� ����ID (DBNum or GaeaID), return : int Level;
        int GetName(lua_State* pState);  // ���� Name Ȯ�� / param1 : DWROD �ش� ĳ���� DBNum, return : char* Name;
		int GetClass(lua_State* pState);  // ������ Ŭ���� ���� / param : DWORD �ش� ĳ���� DBNum, return : DWORD SchoolNum;
        int GetSchool(lua_State* pState);  // ������ �Ҽ��п� Ȯ�� / param : DWORD �ش� ĳ���� DBNum, return : DWORD SchoolNum;
        int GetClubID(lua_State* pState);  // ������ Ŭ�� DBNum Ȯ�� / param : DWORD �ش� ĳ���� DBNum, return : DWORD ClubDBNum;
        int GetClubName(lua_State* pState);  // Ŭ�� �̸� Ȯ�� / param : DWORD �ش� ClubDBNum, return : char* ClubName;
        int GetClubMasterName(lua_State* pState); // Ŭ�� �������� �̸� Ȯ�� / param : DWORD �ش� ClubDBNum, return : char* ClubMasterName;
        int GetClubRank(lua_State* pState); // Ŭ�� ��ũ;

		int IsAllianceClub ( lua_State* _pState );	// �� Ŭ���� ����Ŭ������ Ȯ���Ѵ�;

        int GetParty(lua_State* pState);
        int GetPartyID(lua_State* pState);
        int GetPartyMember(lua_State* pState); // return : ��Ƽ�� DbNum;
        int GetPartyMemberNum(lua_State* pState);
        int GetPartyMaster(lua_State* pState);  // return : ��Ƽ�� DbNum;
		int GetFieldSvrID(lua_State* pState);	// Field Server ID ��ȣ
		int GetServerChannel(lua_State* pState);	// Field Server ID ��ȣ

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
        int DoEffectPosition(lua_State* pState);	// ����Ʈ�� �����Ѵ�. Ư�� Ŭ���̾�Ʈ���Ը�;

        // party;
        int JoinParty(lua_State* pState);  // ��Ƽ ���� or ��Ƽ�� �߰�;
        int OutParty(lua_State* pState);  // ��Ƽ Ż��;
        int ClearParty(lua_State* pState);  // ��Ƽ �ػ�;

		// Item;
		int GetItemCount(lua_State* pState);
		int AddItem(lua_State* pState);
		int RemoveItem(lua_State* pState);

        // UI;
        int PrintChatMsg(lua_State* pState);  // ä��â�� System�޽����� ��� ( Ư�� �ο����� );
        int PrintChatXMLMsg(lua_State* pState);

        int PrintSimpleMsg(lua_State* pState);  // ȭ�� �߻�ܿ� �ؽ�Ʈ ���;
        int PrintSimpleXMLMsg(lua_State* pState);

        int PrintTextDlgMsg(lua_State* pState);  // ȭ�� ��� �ؽ�Ʈ ���̾�α� �ڽ� ���;
        int PrintTextDlgXMLMsg(lua_State* pState); 


        //////////////////////////////////////////////////////////////////////////////////////////////
        //Instance funtion : Field ���� �δ� Instance Script�� Invoke�Լ� (�δ��������� ��밡��);
        ///////////////////////////////////////////////////////////////////////////////////////////////
        
        // getXXX;
        int GetHP(lua_State* pState);  // ���� HP Ȯ�� / param1 : DWORD �ش� ����ID (DBNum or GaeaID), return : int HP;
        int GetMP(lua_State* pState);
        int GetSP(lua_State* pState);
		int GetMaxHP(lua_State* pState);  // ���� HP ���� / param1 : DWORD �ش� ����ID (DBNum or GaeaID), param2 : int HP, return : None;
		int GetMaxMP(lua_State* pState);
		int GetMaxSP(lua_State* pState);

        int GetPosition(lua_State* pState);

        int GetFaction(lua_State* pState);  // �ش� �������� ���� Ȯ�� /  param1 : DWORD ������ Type,  param2 : DWORD ������ ID,  return : DWORD ���� ID; 
        int GetFactionList(lua_State* pState);  // �ش� �������� ���� Ȯ�� /  param1 : DWORD ������ Type,  param2 : DWORD ������ ID,  return : Table < DWORD > ID ����Ʈ; 

		/////////////////////////////////////////////////////////////////////////////////////
		// ����� Crow ���� ���� Function;
		int GetOffensivePower ( lua_State* pState );			// ���ݷ�;
		int GetDefensivePower ( lua_State* pState );			// ����;
		int GetFireRegistance ( lua_State* pState );			// ȭ�� ���׷�;
		int GetIceRegistance ( lua_State* pState );				// �ñ� ���׷�;
		int GetElectricRegistance ( lua_State* pState );		// ���� ���׷�;
		int GetPoisonRegistance ( lua_State* pState );			// �͵� ���׷�;
		int GetSpiritRegistance ( lua_State* pState );			// ���� ���׷�;
		int GetAccuracyRate ( lua_State* pState );				// ���߷�;
		int GetAvoidanceRate ( lua_State* pState );				// ȸ����;
		int GetHPRestorationRate ( lua_State* pState );			// HP ȸ����;
		int GetMPRestirationRate ( lua_State* pState );			// MP ȸ����;
		int GetSPRestirationRate ( lua_State* pState );			// SP ȸ����;

		int GetMultipleHP ( lua_State* pState );
		int GetMultipleAttack ( lua_State* pState );
		int GetMultipleDefense ( lua_State* pState );
		int GetMultipleExp ( lua_State* pState );
		int GetMultipleDrop ( lua_State* pState );
		/////////////////////////////////////////////////////////////////////////////////////

        // setXXX;
        int SetHP(lua_State* pState);  // ���� HP ���� / param1 : DWORD �ش� ����ID (DBNum or GaeaID), param2 : int HP, return : None;
        int SetMP(lua_State* pState);
        int SetSP(lua_State* pState);	


		/////////////////////////////////////////////////////////////////////////////////////
		// ����� Crow ���� ���� Function;
		int SetOffensivePower ( lua_State* pState );			// ���ݷ�;
		int SetDefensivePower ( lua_State* pState );			// ����;
		int SetFireRegistance ( lua_State* pState );			// ȭ�� ���׷�;
		int SetIceRegistance ( lua_State* pState );				// �ñ� ���׷�;
		int SetElectricRegistance ( lua_State* pState );		// ���� ���׷�;
		int SetPoisonRegistance ( lua_State* pState );			// �͵� ���׷�;
		int SetSpiritRegistance ( lua_State* pState );			// ���� ���׷�;
		int SetAccuracyRate ( lua_State* pState );				// ���߷�;
		int SetAvoidanceRate ( lua_State* pState );				// ȸ����;
		int SetHPRestorationRate ( lua_State* pState );			// HP ȸ����;
		int SetMPRestirationRate ( lua_State* pState );			// MP ȸ����;
		int SetSPRestirationRate ( lua_State* pState );			// SP ȸ����;

		int SetMultipleHP ( lua_State* pState );
		int SetMultipleAttack ( lua_State* pState );
		int SetMultipleDefense ( lua_State* pState );
		int SetMultipleExp ( lua_State* pState );
		int SetMultipleDrop ( lua_State* pState );

		int ResetOffensivePower ( lua_State* pState );			// ���ݷ�;
		int ResetDefensivePower ( lua_State* pState );			// ����;
		int ResetFireRegistance ( lua_State* pState );			// ȭ�� ���׷�;
		int ResetIceRegistance ( lua_State* pState );			// �ñ� ���׷�;
		int ResetElectricRegistance ( lua_State* pState );		// ���� ���׷�;
		int ResetPoisonRegistance ( lua_State* pState );		// �͵� ���׷�;
		int ResetSpiritRegistance ( lua_State* pState );		// ���� ���׷�;
		int ResetAccuracyRate ( lua_State* pState );			// ���߷�;
		int ResetAvoidanceRate ( lua_State* pState );			// ȸ����;
		int ResetHPRestorationRate ( lua_State* pState );		// HP ȸ����;
		int ResetMPRestirationRate ( lua_State* pState );		// MP ȸ����;
		int ResetSPRestirationRate ( lua_State* pState );		// SP ȸ����;
		/////////////////////////////////////////////////////////////////////////////////////

        int SetVisible(lua_State* pState);
        int SetMoveLock(lua_State* pState);
        int SetHpVisible(lua_State* pState);

        int CreateFaction(lua_State* pState);  // ������ �����Ѵ�. ( SetFaction���� ������ Faction���� �ٸ��� �ο��� 0���� ��쿡�� �ڵ� �ı��� �����ʴ´�.):
        int SetFaction(lua_State* pState);  // �ش� �������� ���� ���� / param1 : DWORD ������ Type,  param2 : DWORD ������ ID,  param3 : DWORD ���� ����ID,  return : None;

        int SetRelation(lua_State* pState);  // �ش� ������ Ÿ�������� ���� ���� / param1 : DWORD ���� Type,  param2 : DWORD ���� ID,  param3 : DWORD ���� Ÿ��,  return : None;

        int SetPositionInstance(lua_State* pState); // �δ� �ȿ����� Ư�� ��ġ�� �̵�;
        int SetObjectPositionInstance(lua_State* pState); // �δ� �ȿ����� Ư�� Object ��ġ�� �̵�;
        int SetPositionField(lua_State* pState); // �δ� �ȿ��� �ٱ� �ʵ��� Ư�� ��ġ���� �̵�;
        int SetGatePositionField(lua_State* pState); // �δ� �ȿ��� �ٱ� �ʵ��� ����Ʈ ��ġ���� �̵�;	

		int SetReturnPosition ( lua_State* pState );	// Out �� ���� ��ȯ ��ġ ����;

        int SetResurrectionForced(lua_State* pState); // ���� ��Ȱ ���� (�ʵ� ������ ��Ȱ);
		int SetResurrectionForcedPosition(lua_State* pState); // ���� ��Ȱ ���� (�ʵ� ������ ��Ȱ);
        int SetResurrectionForcedInstance(lua_State* pState); // ���� ��Ȱ ���� (�δ� ������ ��Ȱ);
		int SetResurrectionForcedInstancePosition(lua_State* pState); // ���� ��Ȱ ���� (�δ� ������ ��Ȱ);
		int SetResurrectionDisable(lua_State* pState);  // �÷��̾ ��Ȱ ���� ��Ų�� (��ų��Ȱ ������ �Բ� ��������);
        int SetResurrectionDisableSkill(lua_State* pState);  // �÷��̾ ��ų��Ȱ�� ���� ��Ų��;
        int SetDeathPenalty(lua_State* pState);  // ��� �г�Ƽ ����;

        int SetJoinSuspendInstance(lua_State* pState); // Ư�� �δ��� ���� ������ ���� ��Ŵ;
        int SetJoinLockInstance(lua_State* pState); // Ư�� �δ��� ���� ������ ���� ��;
        int SetJoinLockType(lua_State* pState); // Ư�� ������ ���ϴ� ��� �δ��� ���� ������ ���� ��;

		int RetryFaction ( lua_State* pState );	// Faction �絵��;
		//int ShowTimeUI ( lua_State* pState );	// Show Time UI;
		//int ShowEntranceStateUI ( lua_State* pState );	// Show Entrance State UI;

		int SetStageState(lua_State* pState); // ������ ��ܿ��� ����ϴ� UI (������ ���.. �������);

        int SetViewRange(lua_State* pState); // �δ������� �ִ� �þ߰Ÿ��� ������ (�ʹ� ���� �þߴ� ���� ��ȭ�� ������ ����� ����. ���̵��� ������);

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
        int AddMobLayer(lua_State* pState);			// Ư�� �� ���̾ �ʿ� ���� ( �߰� ��, ������ �ȵ� );
		int DelMobLayer(lua_State* pState);			// Ư�� �� ���̾� ����;
        int DelAllMobLayer(lua_State* pState);		// ��� �� ���̾� ����;

        // doXXX;	
        int DoDestroy(lua_State* pState);  // �ش� �δ��� �ı� ��û(����� ���) / param null, return : None;

        int DoOut(lua_State* pState); // Ư�� �÷��̾ �ѾƳ�;
        int DoJoin(lua_State* pState); // Ư�� �÷��̾ ���� ��û ��Ŵ;
        int DoMoveToPosition(lua_State* pState); // Ư�� ��ġ�� �̵� ��Ŵ (���� �̱���);
        int DoMoveToObjectPosition(lua_State* pState); // Ư�� ��ü ��ġ�� �̵� ��Ŵ (���� �̱���);

        //int DoResurrect(lua_State* pState);  // �÷��̾ ��Ȱ ��Ų��;
        //int DoKillActor(lua_State* pState);  // �ش� ���͸� ���δ�.;

        int DoEffectPositionAll(lua_State* pState);	// ����Ʈ�� �����Ѵ�.;
        int DoEffectActor(lua_State* pState);	    // (���Ϳ��� �����Ǵ�)����Ʈ�� �����Ѵ�. Ư�� Ŭ���̾�Ʈ���Ը�;
        int DoEffectActorAll(lua_State* pState);	// (���Ϳ��� �����Ǵ�)����Ʈ�� �����Ѵ�.;

        // RegistXXX;
        int RegistActorTrigger(lua_State* pState);  // ���� Ʈ���Ÿ� ���;
        int RegistActorZoneTrigger(lua_State* pState);  // ���� �� Ʈ���Ÿ� ��� (�̺�Ʈ ������ �簢���ΰ� �ܿ��� ����);
        int RemoveActorTrigger(lua_State* pState);  // ���� Ʈ���Ÿ� ���;

        // dropXXX;
        int DropCrow(lua_State* pState);

        // Skill;
        int ApplySkill(lua_State* pState);  // ��ųȿ���� ����;
		int ApplySkill_Faction ( lua_State* _pState );	// Faction �� ��ųȿ���� ����;
        int IsHaveSkillFact(lua_State* pState);  // �ش� ��ų ȿ���� �ִ��� �˻�;
        int RemoveSkillFact(lua_State* pState);  // ��ųȿ���� ����;
		int RemoveSkillFact_SkillEffect(lua_State* pState);  // ��ųȿ���� ����;

		// Timer;	
        int RegistTimer(lua_State* pState); // Ÿ�̸� ���;
        int RemoveTimer(lua_State* pState); // Ÿ�̸� ����;
        int IsRegistTimer(lua_State* pState); // Ÿ�̸� ��� ���� Ȯ��;
        int SuspendTimer(lua_State* pState); // Ÿ�̸Ӹ� ���߾��;
        int ResumeTimer(lua_State* pState); // ���߾�� Ÿ�̸Ӹ� �簳��;	
        int GetLeftTime(lua_State* pState); /// ���� �ð� ���� ��� �´�;

		// SendMessage;
        int SendMessage(lua_State* pState);

        // UI;
        int PrintChatMsgFaction(lua_State* pState);  // ä��â�� System�޽����� ��� ( �ش� �������� );
        int PrintChatXMLMsgFaction(lua_State* pState);

        int PrintChatMsgAll(lua_State* pState);  // ä��â�� System�޽����� ��� ( �ش� �δ���ü�� );
        int PrintChatXMLMsgAll(lua_State* pState);

        int PrintSimpleMsgFaction(lua_State* pState);  // ȭ�� �߾ӿ� System�޽����� ��� ( �ش� �������� );
        int PrintSimpleXMLMsgFaction(lua_State* pState);

        int PrintSimpleMsgAll(lua_State* pState);  // ȭ�� �߾ӿ� System�޽����� ��� ( �ش� �δ���ü�� );
        int PrintSimpleXMLMsgAll(lua_State* pState);

        int PrintTextDlgMsgFaction(lua_State* pState);  // ȭ�� ��� System�޽��� ���̾�α� �ڽ� ��� ( �ش� �������� );
        int PrintTextDlgXMLMsgFaction(lua_State* pState);

        int PrintTextDlgMsgAll(lua_State* pState);  // ȭ�� ��� System�޽��� ���̾�α� �ڽ� ��� ( �ش� �δ� ��ü�� );
        int PrintTextDlgXMLMsgAll(lua_State* pState);

        int UI_TimerMessageBox(lua_State* pState);
        int UI_TimerMessageBoxAll(lua_State* pState);
        int UI_XmlTimerMessageBox(lua_State* pState);
        int UI_XmlTimerMessageBoxAll(lua_State* pState);



        //////////////////////////////////////////////////////////////////////////////////////////////
        //priavete Instance funtion : Field ������ ��ũ��Ʈ���� ���� ���Ǵ� �ڵ��� (�ߺ��ڵ� ����)�Լ�����;
        ///////////////////////////////////////////////////////////////////////////////////////////////
        ServerActor* const _getActor(lua_State* pState, int& currentStack);
        std::tr1::shared_ptr<GLClubField> _getClub(lua_State* pState, int& currentStack);
        GLPartyField* _getParty(lua_State* pState, int& currentStack);
    }
}
