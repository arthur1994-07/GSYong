#ifndef ServerTriggerSystem_H
#define ServerTriggerSystem_H

#include "../../RanLogic/TriggerSystem/TriggerSystem.h"

class GLLandMan;
class GLGaeaServer;

/**
	���� Ʈ���� �ý���
	�����ʿ��� ���ѵ� Ʈ���� ���� ó���� ���� �Ѵ�.

 */
namespace TriggerSystem
{
	/**
		Ʈ���� �ý����� �ʱ�ȭ �Ѵ�.
		��� ��ũ��Ʈ�� �̸� �ε� �Ѵ�.

		\return �ʱ�ȭ ���� ����
	 */
	bool			InitServerTriggerSystem();

	/**
		Ʈ���� �ý����� �ı� �Ѵ�.		
		������ �ڷᱸ���� �ı� �Ѵ�.

	 */
	void			DestroyServerTriggerSystem();

	/**
		������ ���� Ʈ���Ÿ� ���� �Ѵ�.

		\param file ��ũ��Ʈ ���ϸ�
	 */
	void			CreateServerScript(const TSTRING& file);

	/**
		Ʈ���� �۵��⸦ ���� �Ѵ�.
		������ ������ �Ѱ��� ��� �θ� ���� �پ��ϰ�(������ƮǮ ��) Ȯ���� �� �ִ�.

	 */
	SpTriggerFinger CreateServerFinger(GLLandMan* pLandMan, GLGaeaServer* pGLGaeaServer, int nStage, int nLevel, const std::string& strGuid);

} // end namespace

#endif