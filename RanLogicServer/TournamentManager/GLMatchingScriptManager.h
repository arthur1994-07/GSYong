#pragma once
#include "../MatchSystem/MatchingDefine.h"

namespace MatchingSystem
{


	// 어쩔수 없이 복사 참조 코드다. 나중에 통합 할수있으면 하자
	// 이 코드의 원조 코드 -> GLInstanceSystem.h -> ScriptManager
	class ScriptManager
	{

	public:

		// 토너 먼트 스크립트를 가져온다.
		// _MatchingID = 루아에서 설정된 토너먼트 순번
		// 로직상에서 작업자 번호.
		// 루아 인스턴스를 배출한다.
		const int getScriptCOPY(const DWORD& _MatchingID,sc::LuaInstance* _scriptReference) const;

		const bool doFile(const std::string& _fileName,const std::string& _paramName);

		const bool doReset();
		
		const int doFileDirect(const std::string& _fileName);

		int GetScriptFileStackSize() { return instanceScriptVector.size(); }

		const bool _destroy(const unsigned int& _instanceMapID);

		

	public:
		ScriptManager();
		~ScriptManager(void);

	private:
		InstanceScriptVector instanceScriptVector;
		TournamentScriptMap TournametScriptMap;
	};


	//메칭용 스크립트 버퍼 (실제 컴파일된 스크립트는 아니다!)
	class MatchingCoreScript
	{
		friend class ScriptManager;
	public:		
		const int doFile(const std::string& _fileName);
		void _destroy(void);

	public:
		~MatchingCoreScript(void);
		MatchingCoreScript(const MatchingCoreScript& _rhs);
		const MatchingCoreScript& operator =(const MatchingCoreScript& _rhs);
		MatchingCoreScript();		


		char* pBuffer;		

	private:
		
		

	private:
		std::string fileName;
		unsigned int nSize;
		
	};
}