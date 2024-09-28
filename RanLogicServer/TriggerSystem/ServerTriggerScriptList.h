#ifndef CServerTriggerScriptList_H
#define CServerTriggerScriptList_H

#include "../../RanLogic/TriggerSystem/TriggerScriptList.h"

namespace TriggerSystem
{

/**	
	�������� ��ũ��Ʈ ����Ʈ �ε��� ó�� �Ѵ�.

 */
class CServerTriggerScriptList
	: public CTriggerScriptList
{
public:
		/**
			�������� ��ũ��Ʈ ���� �ε��� ó�� �Ѵ�.			

			\param file ���ϸ�
		 */
		void	Load(const TSTRING& file);
};

} // end namespace

#endif
