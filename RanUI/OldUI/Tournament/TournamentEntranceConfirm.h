#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"
#include "../Competition/PVEEntranceConfirm.h"

class CUIProgressTimer;
class CBasicTextBox;
class CBasicTextButton;
class EngineDeviceMan;

/**
	입장 확인 메시지 박스를 정의 한다.
	CPVEEntranceConfirm 상속받아서 사용한다.

 */
class CTournamentEntranceConfirm
	: public CPVEEntranceConfirm
{
public:
		CTournamentEntranceConfirm(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CTournamentEntranceConfirm();

		/**
			맵 이름을 입력 받는다.

			\param pMapName 확인을 할 맵 이름을 입력 받는다.
		 */
		void				SetMapName(const TCHAR* pMapName);
		/**
			버튼을 클릭했을 때 이벤트를 처리 한다.

		 */
		void				OnOk();
		void				OnCancel();


		/**
        	창 닫힘 처리를 수행 한다.

         */
virtual void                Close();

        /**
        	창 열림 처리를 수행 한다.

         */
virtual void                Open();
};
