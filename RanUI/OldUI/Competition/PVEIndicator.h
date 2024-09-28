#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CUINumberTimer;
class CUIStageIndicator;
class EngineDeviceMan;

/**
	PVE 게임 진행에 필요한 UI 를 출력 한다.

 */
class CPVEIndicator
	: public CUIGroup
{
public:
		CPVEIndicator(EngineDeviceMan* pEngineDevice);
virtual ~CPVEIndicator();

		/**
			서브 컨트롤들을 생성 한다.
			우리는 2개 텍스트박스를 생성 한다.
			하나는 상단 텍스트 박스로 스테이지 등을 표시 한다.
			하나는 하단 텍스트 박스로 유저들에게 플레이시 출력할 메시지를 표시 한다.

		 */
virtual bool				CreateSubControl();

		/**
			스테이지 진행 시간을 입력 한다.

			\param nSec 초단위 입력
		 */
		void				SetTime(int nSec, bool bBlank, bool bShow);

		/**
			스테이지를 설정 한다.
			화면에 모든 스테이지 정보를 보여주는 것이 아니라 입력되는 간격만큼만 보여 줌에 주의	
			자세한 사항은 기획서를 참조 바람

			\param nStage 현재 스테이지			
			\param nEnd 잴 마지막 끝 스테이지, 간격에 딱 안맞을 경우도 지원 한다.
			\param nRange 화면에 보여줄 스테이지 간격
		 */
		void				SetStage(int nStage, int nEnd, int nRange, bool bShow);

		/**
			잡은 몬스터를 설정 한다.

			\param nRem 남은 몬스터
			\param nAll 잡아야할 모든 몬스터
		 */
		void				SetMonster(int nRem, int nAll, bool bShow);

		/**
			이동 포인트를 설정 한다.
            이동 포인트란 유저들이 이동 해야할 곳들을 의미한다.
            이를 통해 장애물 통과 등의 이벤트를 할 수 있다.

		 */
		void				SetPositioning(int nX, int nY, int nNow, int nNeed, bool bShow);

protected:
		/**
			텍스트 박스를 만든다.
			중복 코드를 방지 한다.

			\param dwFontSize 폰트 사이즈
			\param dwFlags 폰트 생성 플래그들
			\param pControl 컨트롤명
			\param wUIAlign 컨트롤 정렬 정보
			\param nTextAlign 텍스트 정렬 정보
			\return 생성시 텍스트 박스, 실패시 NULL
		 */
		CBasicTextBox*		CreateTextBox(DWORD dwFontSize, DWORD dwFlags, const TCHAR* pControl, WORD wUIAlign, int nTextAlign);

		/**
			잡을 몹 출력과 이동 포인트 출력을 정렬 한다.
			
		 */
		void				Align();

		CUINumberTimer*		m_pTimerIndicator;	//@ 숫자기반 타이머
		CUIStageIndicator*	m_pStageIndicator;	//@ 스테이지 표시기
		CBasicTextBox*		m_pMobIndicator;	//@ 잡을 몹 수치 표시
        CBasicTextBox*		m_pMoveIndicator;	//@ 이동 포인트 갯수 표시

};
