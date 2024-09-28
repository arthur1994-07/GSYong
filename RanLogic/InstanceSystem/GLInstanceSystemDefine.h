#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/gassert.h"
#include "../FSM/TMiniFSM.h"
#include "../Land/FactionManager/FactionManagerDefine.h"
#include "../../RanLogic/Network/NetLogicDefine.h"
#include "../../RanLogic/MatchSystem/GLCallbackTrigger.h"
#include "../../RanLogic/MatchSystem/MatchModel.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"


#include <string>
#include <vector>
#include <map>

namespace InstanceSystem
{
	// Log ��������� �ּ� ����;
#ifndef NDEBUG
//#define _INSTANCE_SCRIPT_SYSTEM_LOG_
#endif

	/*
    ���� ����!
    �δ� �ý��ۿ� ���Ǵ� ��ũ��Ʈ ���ϵ��� ������ �����ϱ� ���� ����;
    ��� �߰��� ��������� �ִ��� ��쿡 ���� ��ũ��Ʈ ������ ���ΰ����Ͽ� ����ȣȯ ���� �� �� �ֵ��� ��;

    - ������ Main Ver / Sub Ver�� 2���� Ÿ������ ����;
    - Main Ver�� ���ڿ��� �ν��ϸ�, Sub Ver�� ���ڷ� ������;
    ScriptVersion =
    {
        "1.0.1"  <= MainVersion ( �ݵ�� ��ġ�ؾ��ϴ� ���� ���� );
        2.3      <= SubVersion ( ��ġ�ϰų� ���� ������ ��츦 ����ϴ� ���� ���� );
    }

    1) ���� ����;
    - ������ Main Ver / Sub Ver�� 2���� Ÿ������ ����;
    - Main Ver�� ���ڿ��� �ν��ϸ�, Sub Ver�� ���ڷ� ������;
    - Main Ver�� �ݵ�� ��ġ�ؾ� �ϰ� �귱ġ���� �����ϱ� �����ϱ� ���ؼ� ���ڿ� ���;
    - Sub Ver�� �ݵ�� ��ġ�ؾ��� �ʿ䰡 ���� ����ȣȯ�� �����ϱ⶧���� double���� ��� (lua���� double <=> float ��ȯ�� �����߻������� double ���);

    2) ��Ȳ �� ���� ����;
        2.1) �������� ���� ���� ��;
        - �������� ������ �����Կ� ���� �� ������ �´� ��ũ��Ʈ �����ϱ� ���� Main ������ ù° ���� ����;
        ex) 
        1.5R5 : 1.5R5.0
        1.5R6 : 1.5R6.0

        2.2) �Լ� ���� �� ����;
        - �Լ� ��ü ���� �Ǵ� ������ ��� �ش� �Լ��� ��� ���� ��� ��ũ��Ʈ�� �����ؾ� �ϹǷ� Main ������ ��° ���� ����;
        ex)
        1.5R5.0     ��      1.5R5.1
        1.5R6.1     ��      1.5R6.2

        2.3) �Լ� �߰�;
        - ���ο� �Լ��� �߰��� ��� ���� ��ũ��Ʈ ������ ������ �ʿ� ������ �߰� �Լ� ��� ���� ���θ� �� �� �ֵ��� Sub ������ ù° ���� ����;
        ex) 
        1.5R5.0     ��      1.5R5.0
        0.0                 1.0

        2.4) �Լ� ���� ����;
        - �Լ� ������ ���� �Ƚ��� ��� ��ũ��Ʈ ���� ������ �ʿ� �����Ƿ� Sub ������ ��° ���� ����;
        ex)
        1.5R5.1     ��      1.5R5.1
        1.0                 1.1
        
    �� �� ��Ȳ�� ������ ������ ��, �� ���� ������ ��� 0���� �ʱ�ȭ��.;
    ex)
    1,5R5.3     ��      1.5R6.0
    2.7                 0.0
    

    3. ���� ���� ���濡 ���� �߰� ���;
        3.1) ���� ��ũ��Ʈ �ε�;
        - �������ϰ� ���� ������ ���� ����� ���� �޽����� �Բ� ��ũ��Ʈ ���� �������� ����;

        3.2) ���� ��ũ��Ʈ �ε�;
        - ��ũ��Ʈ ������ �����ϰ� �Ǵ��� ��Ȳ�� ���� �ٸ��� ����;
        - Main Ver : ��ũ��Ʈ�� ��õ� ������ �ڵ忡 ��õ� ������ ������ ��ġ�ؾ� �ϸ� �˾� �޽����� �Բ� ��ũ��Ʈ �ε����� ���� (������ ����);
        - Sub Ver : ���� �������� ���� �� ��Ȳ�� ���� ��ũ��Ʈ �ε����� �ʰų� ������ �� ����;
        ���������� Sub Ver�� �� ���� ��� : �������Ͽ��� �����ϴ� �Լ��� ������ ���� ����̹Ƿ� ��ũ��Ʈ �ε�;
        ��ũ��Ʈ�� Sub Ver�� �� ���� ��� : ��ũ��Ʈ���� ��� ���� �Լ��� �������Ͽ� ���Ե��� ���� ��찡 �߻��� �� �����Ƿ� ��ũ��Ʈ �ε����� ����;

    �� Ran_16R1_14Y11M ������ ������ �ٸ� ���(������)�� ���������� ����ϱ� ������ ���������� ���� �� ����;
    */

	static const double SCRIPT_VERSION = 1.4;

    static std::string SCRIPT_MAIN_VERSION = "AlphaServer";
    static const double SCRIPT_SUB_VERSION = 0.0;

	enum
	{
		EMINSTANCE_NAME_LENGTH = 64,
	};	

    enum EMMOVE_TYPE
    {
        EMMOVE_FIELD2INSTANCE,
		EMMOVE_INSTANCE2INSTANCE,
		EMMOVE_INSTANCE2FIELD,
		EMMOVE_NSIZE,
    };

    enum
    {
        EMINSTANCE_SCRIPT_NULL			= 0x00000000L,
        EMINSTANCE_SCRIPT_VISIBLE		= 0x00000001L, // �δ��� ������ �ʰ�;
        EMINSTANCE_SCRIPT_USE				= 0x00000002L, // �δ��� ������� ����;
        EMINSTANCE_SCRIPT_JOIN_LOCK	= 0x00000004L, // �δ��� �����ִ���?;
		EMINSTANCE_SCRIPT_RELOAD		= 0x00000008L, // reload ��;
    };

	enum EMINSTANCE_TYPE
	{
		// ���� �����ʿ��� �� ���� ������ ���� ���� ó������ �ʴ´�;
		// ��ũ���Ͱ� Ȱ���Ҽ� �ְ� �ϵ�, �����ʿ��� ���� �ϴ��� ���ݸ� �ϵ��� �Ѵ�;
		// ����� �ƹ��������� ������ �ʴ´�, �Ϲ��δ��� �����ϸ� ������ ���� ����;
		EMINSTANCE_NORMAL,		// �Ϲ� �δ�;
		EMINSTANCE_NSIZE,
	};
	enum EMAUTHORITY
	{	
        EMAUTHORITY_NORMAL, // �Ϲ� �÷��̾� �ڰ����� ����;
        EMAUTHORITY_NORMAL_FORCED, // ������� �÷��̾� �ڰ����� ����;
		EMAUTHORITY_GM, // GM �������� ����;		
		EMAUTHORITY_OBSERVER, // ������ �ڰ����� ����;
		EMAUTHORITY_NSIZE,
	};

    enum EM_SEND_SERVER_TYPE
    {
        EM_TO_MY_INSTANCE, // �ڽ��� ���� �δ���;
        EM_TO_FIELD,  // �ٸ� �ʵ弭���� �δ� ��ü���� (Field Intance);
        EM_TO_AGENT,  // �ٸ� ������Ʈ������ �δ� �������� (Agent Public);
        EM_TO_AGENT_FROM_FIELD, // �ʵ弭������ ������Ʈ ������ ������ ������ ���(Field Instance -> Agent Public);
        EM_TO_FIELD_FROM_AGENT, // ������Ʈ�������� �ʵ� ������ ������ ������ ���(Agent Public -> Field Instance);
    };

    enum EM_CUSTOM_MESSAGE_TYPE
    {
        EM_MESSAGE_USER,                // Ư�� ����;
        EM_MESSAGE_MY_INSTANCE,         // ���� �ڽ��� ���� �ʵ弭�� �δ� ��ü (LevelField);
        EM_MESSAGE_FIELD_BASE,          // �ʵ弭���� Ư�� �δ� ������ (PublicField);
        EM_MESSAGE_FIELD_INSTANCE,      // �ʵ弭���� Ư�� �δ� ��ü (LevelField);
        EM_MESSAGE_AGENT_BASE,          // ������Ʈ������ Ư�� �δ� ������ (PublicAgent);
        EM_MESSAGE_AGENT_INSTANCE,      // ������Ʈ������ Ư�� �δ� ��ü (LevelAgent);
    };

	class InstancePopUpTrigger : public CallbackUtility::CallBackTrigger<MatchingSystem::InstancePopUpReceipt>
	{
	public:
		InstancePopUpTrigger() : CallBackTrigger(NULL){}
	};


	
	typedef unsigned int HashKey;
	typedef unsigned int DBNUM;	
    typedef unsigned int InstanceMapID;
	typedef unsigned int TimerHandle;

	class LevelScript;
	typedef std::map<HashKey, LevelScript> LevelScriptMap;
	typedef LevelScriptMap::const_iterator LevelScriptMapCIter;
	typedef LevelScriptMap::iterator LevelScriptMapIter;
	
	class InstanceScript;
	typedef std::map<SNATIVEID, InstanceScript> InstanceScriptMap;
	typedef InstanceScriptMap::const_iterator InstanceScriptMapCIter;
	typedef InstanceScriptMap::iterator InstanceScriptMapIter;
	typedef InstanceScriptMap::value_type InstanceScriptValue;

	class InstanceInformationClient;
	typedef std::map<SNATIVEID, InstanceInformationClient> InstanceInformationMap;
	typedef InstanceInformationMap::const_iterator InstanceInformationMapCIter;
	typedef InstanceInformationMap::iterator InstanceInformationMapIter;
	typedef InstanceInformationMap::value_type InstanceInformationValue;

	typedef std::vector<SNATIVEID> MapIDVector;
	typedef MapIDVector::const_iterator MapIDVectorCIter;
	typedef MapIDVector::iterator MapIDVectorIter;

	typedef std::vector<DWORD> DWORDVector;
	typedef DWORDVector::const_iterator DWORDVectorCIter;
	typedef DWORDVector::iterator DWORDVectorIter;

    // ���� ������ �ߺ� �ؽð��� ���ü��ִ�;
    // �ݵ�� 1�� ���� �Ϸ� �� ����� �� �ؽø����� ���� ���־�� ��;
	typedef DWORDVector HashVector; 	

	extern __forceinline const unsigned int __fastcall generateHashKey(const void* const _pKey, const unsigned int _nSize);	
};

