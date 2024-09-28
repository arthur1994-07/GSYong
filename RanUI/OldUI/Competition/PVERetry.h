#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CUIProgressTimer;
class CBasicTextBox;
class CBasicTextButton;
class EngineDeviceMan;

/**
	재도전 확인 메시지 박스를 정의 한다.	

 */
class CPVERetry
	: public CUIWindowEx
{
public:
		CPVERetry(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CPVERetry();

		enum
		{
			CONTROL_ID_NONE = CUIWindowEx::ET_CONTROL_NEXT + 1,
            CONTROL_ID_TIMER,
			CONTROL_ID_BUTTON_OK,
			CONTROL_ID_BUTTON_CANCEL,
		};

		/**
			재도전 확인 메시지 박스를 생성 한다.
			화면 중앙에 최상위 모달 박스로 생성 한다.
			
			\return 생성 성공 유무
		 */
virtual bool				CreateSubControl();

		/**
			버튼 클릭, 타임 종료 등의 메시지를 처리 한다.

		 */
virtual	void				TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

		/**
			윈도우를 오픈 한다.
            재도전 필요한 자금과 대기 시간을 입력 해 주어야 한다.

		 */
		void				Open(LONGLONG nMoney, int nTime);

		/**
			신규인던용인지 트리거시스템용인지 설정한다;
		 */
		void				SetNewInstance ( bool bNewInstance, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID );

		/**
			버튼을 클릭했을 때 이벤트를 처리 한다.

		 */
		void				OnOk();
		void				OnCancel();

		/**
        	창 닫힘 처리를 수행 한다.

         */
        void                Close();

protected:
        /**
        	보여질지를 설정 한다.
            우리는 보여질때 모달로 처리하고 숨겨질때 모달을 해제 한다.

         */
        void                SetVisibleSingle(BOOL bVisible);

		/**
			버튼을 생성 한다.
			동일한 코드 방지를 위해 사용 한다.
			
		 */
		CBasicTextButton*	CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id);

		/**
			서브 컨트롤들

		 */
		CUIProgressTimer*	m_pTimer;	//@ 타이머 컨트롤
		CBasicTextBox*		m_pText;	//@ 텍스트 표시 컨트롤
		CBasicTextButton*	m_pOk;		//@ 도전 버튼
		CBasicTextButton*	m_pCancel;	//@ 그만 버튼
        CUIControl*			m_pClock;	//@ 시계 이미지

        /**
            외부 인터페이스

         */
        GLGaeaClient*       m_pGaeaClient;

		/**
			신규인던인지 트리거시스템인지 구분하기 위해 존재한다;
		 */
		bool								m_bNewInstance;
		InstanceSystem::InstanceMapID		m_nInstanceID;
		SNATIVEID							m_sKeyMapID;
};
