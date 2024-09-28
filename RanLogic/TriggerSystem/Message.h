#ifndef SMessage_H
#define SMessage_H

#include "TriggerSystemType.h"

namespace TriggerSystem
{

/**
	Ʈ���� �ν��Ͻ� �޽���
	�۵��⸦ ���� �޽����� ���� �� �� ��� �Ѵ�.
	�δ����� Ʈ���� �ν��Ͻ��� ���� �޽����� ���� �� ��� �Ѵ�.

 */
struct SMessage
{
virtual ~SMessage() {}
virtual	void Command(CTriggerInstance* pInstance) = 0;
template<class T>
inline  bool Cast(T*& ref) { return (ref = dynamic_cast<T*>(this)) != 0; }
};

/**
	Ʈ���� �޽���
	Ʈ���� �ý����� ���� �޽����� ���� �� �� ��� �Ѵ�.
	�ν��Ͻ� �޽����ʹ� �ٸ��� ���� �Ѵ�.
	��뿹�� ���� ���Ǵ� Ʈ���ŵ鿡 ������ ������ �� � ���� �� �ִ�.

 */
struct SSystemMessage
{
virtual ~SSystemMessage() {}
virtual	void Command(CTrigger* pTrigger) = 0;
};

} // end namespace

#endif