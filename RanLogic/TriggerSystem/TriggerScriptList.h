#ifndef CTriggerScriptList_H
#define CTriggerScriptList_H

#include "../RanLogic/Script/SquirrelMan.h"
#include "TriggerSystemType.h"

namespace TriggerSystem
{

/**	
	Ʈ���� �ý����� ��� ��ũ��Ʈ ������� �̷���� ������
	��ũ��Ʈ ���� ����Ʈ�� ó�� �Ѵ�.

 */
class CTriggerScriptList
	: public SquirrelMan
{
public:
virtual ~CTriggerScriptList() {}

		/**
			����Ʈ ��ũ���� �о� �ε��� ȣ�� �Ѵ�.
			����, Ŭ�� ������� ���� �ڵ�

		 */
		void	ToDo();

		/**
			���� ��ũ��Ʈ ���� �ε��� ó�� �Ѵ�.
			������ Ŭ�� ���� �ٸ� ������� ó�� �Ѵ�.

			\param file ���ϸ�
		 */
virtual void	Load(const TSTRING& file) = 0;
};

} // end namespace

#endif
