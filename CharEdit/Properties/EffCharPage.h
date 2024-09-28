#pragma once

#include "PieceRelativePage.h"

class   DxEffChar;
struct  EFFCHAR_PROPERTY;

/**
	캐릭터에 붙는 이펙터들을 설정하는 페이지의 기본이 되는 클래스
    기본적으로 처리할 사항이나 공통되는 사항의 인터페이스를 제공 한다.
	
 */
class CEffCharPage
    : public CPieceRelativePage
{
public:
        CEffCharPage(UINT nIDTemplate);
virtual ~CEffCharPage();

        /**
        	외부에서 수정하고자 하는 이펙터를 설정 한다.
            
        	\param pEffChar 수정하고자 하는 이펙터
         */
        void                SetInstance(DxEffChar* pEffChar) { m_pEffChar = pEffChar; }

        /**
            외부에서 수정하고자 하는 이펙터의 속성을 설정 한다.
            널값으로 입력시 새로 생성을 의미 한다.
        	
	        \param *pProp 수정하고자 하는 이펙터 속성, NULL 값 입력시 새 속성 생성
         */
virtual void                SetProperty(EFFCHAR_PROPERTY *pProp = NULL) = 0;

        /**
        	현재 이펙트 설정 페이지에서 부모 페이지로 돌아 간다.

         */
virtual void                ReturnPage();

        /**
        	변경된 이펙트 설정을 적용 한다.

         */
        void                EffectApply();

        /**
            잘 모르겠지만 몇몇 이펙트는 특별한 인터페이스를 요구한다.            

         */
virtual void                SetSpinControl(int nColorHeight) {}
virtual void                ResetDialog() {}
virtual void                UpdateKeyBoardMouse() {}

protected:
	    DxEffChar*			m_pEffChar;
};
