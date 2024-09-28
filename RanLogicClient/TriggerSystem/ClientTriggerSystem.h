#ifndef ClientTriggerSystem_H
#define ClientTriggerSystem_H

#include "../../RanLogic/TriggerSystem/TriggerSystem.h"

class GLGaeaClient;

/**
	Ŭ���̾�Ʈ Ʈ���� �ý���
	Ŭ���̾�Ʈ�ʿ��� ���ѵ� Ʈ���� ���� ó���� ���� �Ѵ�.

 */
namespace TriggerSystem
{
	/**
		Ʈ���� �ý����� �ʱ�ȭ �Ѵ�.
		��� ��ũ��Ʈ�� �̸� �ε� �Ѵ�.

		\return �ʱ�ȭ ���� ����
	 */
	bool			InitClientTriggerSystem();

	/**
		Ʈ���� �ý����� �ı� �Ѵ�.		
		������ �ڷᱸ���� �ı� �Ѵ�.

	 */
	void			DestroyClientTriggerSystem();

	/**
		������ ���� Ʈ���Ÿ� ���� �Ѵ�.		

		\param file ��ũ��Ʈ ���ϸ�
	 */
	void			CreateClientScript(const TSTRING& file);

	/**
		Ʈ���� �۵��⸦ ���� �Ѵ�.
		������ ������ �Ѱ��� ��� �θ� ���� �پ��ϰ�(������ƮǮ ��) Ȯ���� �� �ִ�.

	 */
	SpTriggerFinger CreateClientFinger(GLGaeaClient* pGaeaClient, int nStage, int nLevel);

} // end namespace

#endif