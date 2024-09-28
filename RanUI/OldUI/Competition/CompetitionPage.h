#pragma once

#include "../Util/UIPage.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

/**
	���� UI �� ���� �� ������ ó���� ���� �Ѵ�.

	���� UI �� ���� �������� PVE �� �̷��� ������ ���� �������� �߰��� �� �ֵ���
	���������� CCompetitionPage �� ��� �޾� �� ������ ó���� ���� �Ѵ�.

	�׷��Ƿ� ��� �� ���������� ���Ǵ� ���� �������̽��� �̰��� �����Ǿ�� �Ѵ�.

 */
class CCompetitionPage
	: public CUIPage
{
public:
		CCompetitionPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CCompetitionPage() = 0 {}

		/**
			�ܺο��� ���� ���� ���°� ���� �Ǹ� ȣ��Ǿ�
			UI �� ������ ���� �� �ִ� ������ ���� �Ѵ�.

		 */
virtual void Update() = 0;
};
