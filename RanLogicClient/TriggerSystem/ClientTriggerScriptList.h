#ifndef CClientTriggerScriptList_H
#define CClientTriggerScriptList_H

#include "../../RanLogic/TriggerSystem/TriggerScriptList.h"

namespace TriggerSystem
{

/**	
	Ŭ���̾�Ʈ���� ��ũ��Ʈ ����Ʈ �ε��� ó�� �Ѵ�.

 */
class CClientTriggerScriptList
	: public CTriggerScriptList
{
public:
		/**
			Ŭ���̾�Ʈ���� ��ũ��Ʈ ���� �ε��� ó�� �Ѵ�.			

			\param file ���ϸ�
		 */
		void	Load(const TSTRING& file);
};

} // end namespace

#endif
