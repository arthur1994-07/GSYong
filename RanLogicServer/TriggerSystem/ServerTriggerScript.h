#ifndef CServerTriggerScript_H
#define CServerTriggerScript_H

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

namespace TriggerSystem
{

/**
	���� Ʈ���ſ� ����� Ʈ���� ��ũ��Ʈ

 */
class CServerTriggerScript
	: public CTriggerScript
{
public:
		/**
			��ũ��Ʈ ���� �� ó���� ���� �Ѵ�.
			��ũ��Ʈ���� ���ε� ó���� �����Ѵ�.

		 */
virtual void			Init();
};

} // end namespace

#endif
