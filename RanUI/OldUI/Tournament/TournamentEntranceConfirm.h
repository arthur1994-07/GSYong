#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"
#include "../Competition/PVEEntranceConfirm.h"

class CUIProgressTimer;
class CBasicTextBox;
class CBasicTextButton;
class EngineDeviceMan;

/**
	���� Ȯ�� �޽��� �ڽ��� ���� �Ѵ�.
	CPVEEntranceConfirm ��ӹ޾Ƽ� ����Ѵ�.

 */
class CTournamentEntranceConfirm
	: public CPVEEntranceConfirm
{
public:
		CTournamentEntranceConfirm(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CTournamentEntranceConfirm();

		/**
			�� �̸��� �Է� �޴´�.

			\param pMapName Ȯ���� �� �� �̸��� �Է� �޴´�.
		 */
		void				SetMapName(const TCHAR* pMapName);
		/**
			��ư�� Ŭ������ �� �̺�Ʈ�� ó�� �Ѵ�.

		 */
		void				OnOk();
		void				OnCancel();


		/**
        	â ���� ó���� ���� �Ѵ�.

         */
virtual void                Close();

        /**
        	â ���� ó���� ���� �Ѵ�.

         */
virtual void                Open();
};
