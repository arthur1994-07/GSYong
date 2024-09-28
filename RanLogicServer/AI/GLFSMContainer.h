#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include <tr1/memory>

namespace AISystem
{
	class GLScriptedFSM;

	class GLFSMContainer : public CSingletone< GLFSMContainer >
	{
	protected:
		friend CSingletone;

	private:
		GLFSMContainer ();
		~GLFSMContainer ();

	public:
		bool LoadScriptFile ();

		void Reload ();

	public:
		std::tr1::shared_ptr< GLScriptedFSM > GetFSM( DWORD dwIndex );

	public:
		inline const DWORD GetMaxFSMType() const { return m_dwMaxFSMType; }
		inline const DWORD GetHireSummonLevelInterval() const { return m_dwHireSummonLevelInterval; }
		inline const DWORD GetSpeedUpValue() const { return m_dwSpeedUpValue; }
		inline const DWORD GetEventSkillRange() const { return m_dwEventSkillRange; }
		inline const DWORD GetOwnerRange() const { return m_dwOwnerRange; }

	private:
		std::vector< std::tr1::shared_ptr< GLScriptedFSM > > m_vecScript;

		DWORD m_dwMaxFSMType;

		DWORD m_dwHireSummonLevelInterval;
		DWORD m_dwSpeedUpValue;
		DWORD m_dwEventSkillRange;
		DWORD m_dwOwnerRange;
	};

}