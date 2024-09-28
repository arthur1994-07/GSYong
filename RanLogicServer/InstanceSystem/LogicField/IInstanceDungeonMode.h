#pragma once

#include "./InstanceDungeonModeDefine.h"

namespace InstanceSystem
{
	
	class IInstanceFieldMode;

	class IInstanceDungeonMode
	{
	public:
		// 인던의 기본 Custom Message Flag;
		enum
		{
			EMCUSTOM_MSG_EXIT_BUTTON = 1,
			EMCUSTOM_MSG_NEXT
		};


	protected:
		IInstanceFieldMode*		m_pInstanceField;


	public:
		IInstanceDungeonMode ( IInstanceFieldMode* _pInstanceField )
			: m_pInstanceField ( _pInstanceField ) { }

		~IInstanceDungeonMode ( void ) { }
	};

}