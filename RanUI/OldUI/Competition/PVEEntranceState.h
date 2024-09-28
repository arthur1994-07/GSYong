#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class EngineDeviceMan;

/**
	입장 상태 UI 를 정의 한다.
	입장 한 파티원과 대기 시간을 표시 한다.

 */
class CPVEEntranceState
	: public CUIGroup
{
public:
		CPVEEntranceState(EngineDeviceMan* pEngineDevice);
virtual ~CPVEEntranceState();

		/**
			입장 상태 컨트롤들을 생성 한다.

		 */
virtual bool				CreateSubControl();

		/**
			입장 상태를 출력 한다.

			\param nCur 현재 입장 파티원수
			\param nAll 전체 입장해야할 파티원수
			\param nSec 시간 입장 대기 시간
		 */
		void				SetEntranceState(int nCur, int nAll, int nSec);

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
			각 상태를 설정 한다.

		 */
		void				SetTime(int nSec);
		void				SetPerson(int nNum, int nAll);

		/**
			시간이 완료되거나 인원이 모두 입장하거나 등
			상태가 완료된 상태에 대한 처리

		 */
		void				OnBeCompleted();

		/**
			서브 컨트롤들

		 */
		CBasicTextBox*		m_pState;	//@ 인원 상태 출력
		CBasicTextBox*		m_pTime;	//@ 시간 출력

		/**
			시간 상태

		 */		
		int					m_nTimer;	//@ 총 시간
		float				m_fCur;		//@ 남은 시간

		/**
			인원 상태

		 */
		int					m_nAll;		//@ 모든 인원
};
