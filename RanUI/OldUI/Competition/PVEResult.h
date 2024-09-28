#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CUIListBox;
class CBasicTextButton;
class EngineDeviceMan;

/**
	PVE 스테이지 결과 정보를 화면에 출력 한다.

 */
class CPVEResult
	: public CUIWindowEx
{
public:
		CPVEResult(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CPVEResult();

		enum
		{
			CONTROL_ID_NONE = NO_ID + 1,
			CONTROL_ID_BUTTON_EXIT,
		};

		/**
			서브 컨트롤들을 생성 한다.
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
			시간 경과를 처리 한다.

		 */
virtual void				Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

		/**
			나가기 버튼을 클릭했을 때 호출 된다.

		 */
		void				OnExit();

		/**
			시간이 완료되면 호출 된다.

		 */
		void				OnCompleted();

        /**
            결과 창을 출력 한다.

         */
        void                SetStageResult();

        /**
			기타 정보를 설정 한다.

		 */
		void				SetStage(int nStage, bool bLast);
		void				SetTimer(int nTime);

protected:
		/**
			버튼을 생성 한다.
			동일한 코드 방지를 위해 사용 한다.
			
		 */
		CBasicTextButton*	CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id);

		/**
			리스트 박스를 생성 한다.
			우리는 리스트 박스의 선택과 스크롤은 사용하지 않는다.
			동일한 코드 방지를 위해 사용 한다.
			
		 */
		bool			    CreateListBox(const int nIndex, const TCHAR* pColumn, const TCHAR* pColumnWord, 
                                          const TCHAR* pListBox, const TCHAR* pTextBoxInListBox, float fLine, float fFront, float fBack);

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
			리스트에 정보 추가
			
		 */
		void				AddClass(DWORD dwClass);
		void				AddCharName(const TCHAR* pCharName);
		void				AddDamage(int nDamage);
		void				AddHeal(int nHeal);
		void				AddBonus(bool bExp, bool bMoney);
        void                AddMVP(bool bMVP);

		/**
			모든 리스트 박스를 초기화 시킨다.

		 */
        void                Clear();

		/**
			서브 컨트롤들

		 */
		CBasicTextButton*	m_pExit;	//@ 나가기 버튼
		CBasicTextBox*		m_pTime;	//@ 강제나가기 타이머 텍스트 박스
        CUIOrdinal*         m_pOrdinal; //@ 영어 서수 이미지(스테이지로 표현)
        CUIControl*         m_pClear;   //@ all 클리어 이미지

        enum
        {            
            LB_CLASS,                   //@ 클래스 이미지
            LB_CLASSNAME,               //@ 클래스명
            LB_DAMAGE,                  //@ 데미지량
            LB_HEAL,                    //@ 힐량
            LB_BONUS,                   //@ 추가 보너스 내역
            LB_MVP,                     //@ MVP 이미지
            LB_QUANTITY,
        };

        CUIListBox*         m_pListBox[LB_QUANTITY];
        CBasicTextBox*		m_pColumn[LB_QUANTITY];

		/**
			시간 제어

		 */
		int					m_nTimer;	//@ 총 시간
		float				m_fCur;		//@ 남은 시간

        /**
            외부 인터페이스

         */
		GLGaeaClient*       m_pGaeaClient;
};
