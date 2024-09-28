#pragma once
#include "../MatchSystem/MatchingDefine.h"

namespace MatchingSystem
{


	// ��¿�� ���� ���� ���� �ڵ��. ���߿� ���� �Ҽ������� ����
	// �� �ڵ��� ���� �ڵ� -> GLInstanceSystem.h -> ScriptManager
	class ScriptManager
	{

	public:

		// ��� ��Ʈ ��ũ��Ʈ�� �����´�.
		// _MatchingID = ��ƿ��� ������ ��ʸ�Ʈ ����
		// �����󿡼� �۾��� ��ȣ.
		// ��� �ν��Ͻ��� �����Ѵ�.
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


	//��Ī�� ��ũ��Ʈ ���� (���� �����ϵ� ��ũ��Ʈ�� �ƴϴ�!)
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