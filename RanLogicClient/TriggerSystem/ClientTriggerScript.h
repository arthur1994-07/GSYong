#ifndef CClientTriggerScript_H
#define CClientTriggerScript_H

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

namespace TriggerSystem
{

/**
	Ŭ���̾�Ʈ Ʈ���ſ� ����� Ʈ���� ��ũ��Ʈ

 */
class CClientTriggerScript
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
