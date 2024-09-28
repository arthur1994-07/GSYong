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
	// Log 보고싶으면 주석 제거;
#ifndef NDEBUG
//#define _INSTANCE_SCRIPT_SYSTEM_LOG_
#endif

	/*
    주의 사항!
    인던 시스템에 사용되는 스크립트 파일들의 버전을 관리하기 위해 만듦;
    기능 추가나 변경사항이 있더라도 경우에 따라 스크립트 버전을 세부관리하여 하위호환 가능 할 수 있도록 함;

    - 버전은 Main Ver / Sub Ver의 2가지 타입으로 나뉨;
    - Main Ver은 문자열로 인식하며, Sub Ver은 숫자로 구성됨;
    ScriptVersion =
    {
        "1.0.1"  <= MainVersion ( 반드시 일치해야하는 메인 버전 );
        2.3      <= SubVersion ( 일치하거나 상위 버전인 경우를 허용하는 서브 버전 );
    }

    1) 전제 조건;
    - 버전은 Main Ver / Sub Ver의 2가지 타입으로 나뉨;
    - Main Ver은 문자열로 인식하며, Sub Ver은 숫자로 구성됨;
    - Main Ver은 반드시 일치해야 하고 브런치별로 관리하기 용이하기 위해서 문자열 사용;
    - Sub Ver은 반드시 일치해야할 필요가 없고 하위호환이 가능하기때문에 double값을 사용 (lua에서 double <=> float 변환시 오차발생때문에 double 사용);

    2) 상황 별 버전 증가;
        2.1) 실행파일 버전 증가 시;
        - 실행파일 버전이 증가함에 따라 각 버전에 맞는 스크립트 인지하기 쉽게 Main 버전의 첫째 단위 증가;
        ex) 
        1.5R5 : 1.5R5.0
        1.5R6 : 1.5R6.0

        2.2) 함수 변경 및 삭제;
        - 함수 자체 변경 또는 삭제될 경우 해당 함수를 사용 중인 모든 스크립트를 수정해야 하므로 Main 버전의 둘째 단위 증가;
        ex)
        1.5R5.0     ⇒      1.5R5.1
        1.5R6.1     ⇒      1.5R6.2

        2.3) 함수 추가;
        - 새로운 함수가 추가될 경우 기존 스크립트 내용은 변경할 필요 없으나 추가 함수 사용 가능 여부를 알 수 있도록 Sub 버전의 첫째 단위 증가;
        ex) 
        1.5R5.0     ⇒      1.5R5.0
        0.0                 1.0

        2.4) 함수 내부 변경;
        - 함수 내부적 버그 픽스될 경우 스크립트 내용 변경이 필요 없으므로 Sub 버전의 둘째 단위 증가;
        ex)
        1.5R5.1     ⇒      1.5R5.1
        1.0                 1.1
        
    ※ 각 상황별 버전이 증가할 때, 그 이하 버전은 모두 0으로 초기화함.;
    ex)
    1,5R5.3     ⇒      1.5R6.0
    2.7                 0.0
    

    3. 버전 관리 변경에 따른 추가 기능;
        3.1) 기존 스크립트 로드;
        - 실행파일과 맞지 않으면 서버 실행시 에러 메시지와 함께 스크립트 정상 동작하지 않음;

        3.2) 변경 스크립트 로드;
        - 스크립트 버전이 증가하게 되더라도 상황에 따라 다르게 동작;
        - Main Ver : 스크립트에 명시된 버전과 코드에 명시된 버전이 완전히 일치해야 하며 팝업 메시지와 함께 스크립트 로드하지 않음 (기존과 동일);
        - Sub Ver : 서버 실행파일 실행 시 상황에 따라 스크립트 로드하지 않거나 무시할 수 있음;
        실행파일의 Sub Ver이 더 높을 경우 : 실행파일에서 지원하는 함수가 사용되지 않을 경우이므로 스크립트 로드;
        스크립트의 Sub Ver이 더 높을 경우 : 스크립트에서 사용 중인 함수가 실행파일에 포함되지 않을 경우가 발생할 수 있으므로 스크립트 로드하지 않음;

    ※ Ran_16R1_14Y11M 이하의 버전은 다른 방식(구버전)의 버전관리를 사용하기 때문에 버전관리가 힘들 수 있음;
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
        EMINSTANCE_SCRIPT_VISIBLE		= 0x00000001L, // 인던을 보이지 않게;
        EMINSTANCE_SCRIPT_USE				= 0x00000002L, // 인던을 사용하지 않음;
        EMINSTANCE_SCRIPT_JOIN_LOCK	= 0x00000004L, // 인던이 열려있는지?;
		EMINSTANCE_SCRIPT_RELOAD		= 0x00000008L, // reload 중;
    };

	enum EMINSTANCE_TYPE
	{
		// 내부 로직쪽에서 이 값을 가지고 뭔가 따로 처리하지 않는다;
		// 스크립터가 활용할수 있게 하되, 로직쪽에서 뭔갈 하더라도 조금만 하도록 한다;
		// 현재는 아무곳에서도 쓰이지 않는다, 일반인던만 존재하며 설정할 수도 없다;
		EMINSTANCE_NORMAL,		// 일반 인던;
		EMINSTANCE_NSIZE,
	};
	enum EMAUTHORITY
	{	
        EMAUTHORITY_NORMAL, // 일반 플레이어 자격으로 입장;
        EMAUTHORITY_NORMAL_FORCED, // 입장당한 플레이어 자격으로 입장;
		EMAUTHORITY_GM, // GM 권한으로 입장;		
		EMAUTHORITY_OBSERVER, // 관전자 자격으로 입장;
		EMAUTHORITY_NSIZE,
	};

    enum EM_SEND_SERVER_TYPE
    {
        EM_TO_MY_INSTANCE, // 자신이 속한 인던에;
        EM_TO_FIELD,  // 다른 필드서버의 인던 객체으로 (Field Intance);
        EM_TO_AGENT,  // 다른 에이전트서버의 인던 컨텐츠로 (Agent Public);
        EM_TO_AGENT_FROM_FIELD, // 필드서버에서 에이전트 서버로 정보를 보낼때 사용(Field Instance -> Agent Public);
        EM_TO_FIELD_FROM_AGENT, // 에이전트서버에서 필드 서버로 정보를 보낼때 사용(Agent Public -> Field Instance);
    };

    enum EM_CUSTOM_MESSAGE_TYPE
    {
        EM_MESSAGE_USER,                // 특정 유저;
        EM_MESSAGE_MY_INSTANCE,         // 현재 자신이 속한 필드서버 인던 객체 (LevelField);
        EM_MESSAGE_FIELD_BASE,          // 필드서버의 특정 인던 컨텐츠 (PublicField);
        EM_MESSAGE_FIELD_INSTANCE,      // 필드서버의 특정 인던 객체 (LevelField);
        EM_MESSAGE_AGENT_BASE,          // 에이전트서버의 특정 인던 컨텐츠 (PublicAgent);
        EM_MESSAGE_AGENT_INSTANCE,      // 에이전트서버의 특정 인던 객체 (LevelAgent);
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

    // 극히 낮지만 중복 해시값이 나올수있다;
    // 반드시 1차 개발 완료 후 제대로 된 해시맵으로 변경 해주어야 함;
	typedef DWORDVector HashVector; 	

	extern __forceinline const unsigned int __fastcall generateHashKey(const void* const _pKey, const unsigned int _nSize);	
};

