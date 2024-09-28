#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class EngineDeviceMan;
class CPVEResult;
class CPVEEntranceConfirm;
class CPVERetry;
class CPVEEntranceState;
class CPVEBoard;
class CPVEIndicator;

/**
	PVE의 상태를 출력 하는 UI 를 정의 한다.
	여러 UI 들이 복합적으로 이뤄져 있다.
	하나의 컨텐츠 단위로 묶어 관리의 효율을 높였다.

 */
class CPVEUserInterface
	: public CUIGroup
{
public:
		CPVEUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CPVEUserInterface();

		/**
			입장 상태를 출력 한다.

			\param nCur 현재 입장 파티원수
			\param nAll 전체 입장해야할 파티원수, 0 시 이전에 설정된 전체 파티원수 사용
            \param nSec 남은 시간
			\param bShow 화면에 Visible 유무
		 */
		void				SetEntranceState(int nCur, int nAll, int nSec, bool bShow);

		/**
			스테이지 정보 표시(계속 보이지 않고 잠시 보이는 UI)
			스테이지 표시, 스테이지 메시지 처리

		 */
		void				SetStage(int nStage, bool bShow);
		void				SetMessage(const TCHAR* pText, bool bShow);

		/**
			스테이지 진행 상태 표시(스테이지 유지 동안 계속 표시 되는 UI)
			스테이지 진행 시간(초단위), 스테이지 넘버, 잡은 몬스터, 이동 포인트 순

		 */
		void				SetStageTimeIndicator(int nSec, bool bBlank, bool bShow);
		void				SetStageIndicator(int nStage, int nEnd, int nRange, bool bShow);
		void				SetMonsterIndicator(int nRem, int nAll, bool bShow);
        void				SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow);

		/**
			결과 상태 표시 처리
			성공, 죽음 대기, 실패 가 존재 한다.

		 */
        void                SetResult(int nResult, bool bShow);

		/**
			확인 메시지창 처리
			함수는 각 호출 클래스 함수를 참조 한다.

		 */
		void				SetRetry(LONGLONG nMoney, int nTime, bool bShow);
		void				SetEntrance(bool bShow);
		void				SetNewInstanceRetry(LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID);

		/**
			스테이지 결과를 출력한다.

		 */
		void				SetStageResult();

		/**
			보일지 유무를 설정 한다.
			우리는 보여지지 않을 때 관련 모든 UI 를 보이지 않게 처리 해 준다.

		 */
        void                SetVisibleSingle(BOOL bVisible);

protected:
		/**
			사용할 컨트롤들을 생성 한다.
			
		 */
		bool				CreateSubControl();

		/**
			PVE 컨텐츠 관련 UI

		 */
		CPVEEntranceState*	m_pEntranceState;	//@ 입장 상태 UI 처리기
		CPVEBoard*			m_pBoard;			//@ 스테이지내 메시지 표시기
		CPVEIndicator*		m_pIndicator;		//@ 스테이지 진행 표시기
		CPVEEntranceConfirm*m_pEntranceConfirm;	//@ 입장 확인 메시지 박스
		CPVERetry*			m_pRetry;			//@ 재도전 확인 메시지 박스
		CPVEResult*			m_pResult;			//@ 결과(보상) 윈도우

		/**
			외부 피드백용 클래스들

		 */
		GLGaeaClient*		m_pGaeaClient;
		CInnerInterface*	m_pInterface;

        /**
            Visible 변수

         */
        bool             	m_bVisibleEntranceState;
        bool             	m_bVisibleStage;
        bool             	m_bVisibleMessage;
        bool             	m_bVisibleResult;
        bool             	m_bVisibleStageTimeIndicator;
        bool             	m_bVisibleStageIndicator;
        bool             	m_bVisibleMonsterIndicator;
        bool             	m_bVisiblePositioningIndicator;
		bool				m_bVisibleRetry;
};

class MyPVEUserInterface
	: public IPVEUserInterface
	, private CPVEUserInterface
{
public:
		MyPVEUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual void CreateUIWindowAndRegisterOwnership();
virtual void ToggleShow(bool bShow);
virtual void SetEntranceConfirm(bool bShow);
virtual void SetEntranceState(int nCur, int nAll, int nSec, bool bShow);
virtual void SetStage(int nStage, bool bShow);
virtual void SetMessage(const TCHAR* pText, bool bShow);
virtual void SetMonsterIndicator(int nCur, int nAll, bool bShow);
virtual void SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow);
virtual void SetTimeIndicator( int nSec, bool bShow );
virtual void SetStageIndicator(int nStage, int nEnd, int nRange, int nSec, bool bBlank, bool bShow);
virtual void SetStageState(int nState, bool bShow);
virtual void SetRetry(LONGLONG nMoney, int nTime, bool bShow);
virtual void SetStageResult();
virtual void SetNewInstanceRetry(LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID);
virtual void AllClose();
};
