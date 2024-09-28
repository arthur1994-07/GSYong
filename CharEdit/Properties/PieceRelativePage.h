#pragma once

#include "afxwin.h"

#ifndef DEF_SKIN_SYNC
class   CsheetWithTabChar;
#endif

/**
	조각에 붙어 사용되는 페이지들의 기본 페이지
    몇몇 공통되는 인터페이스들을 정의 한다.
	
 */
class CPieceRelativePage
    : public CPropertyPage
{
public:
        CPieceRelativePage(UINT nIDTemplate);
virtual ~CPieceRelativePage();

#ifndef DEF_SKIN_SYNC
        /**
        	부모 페이지를 설정 한다.

        	\param *pSheetTab 
         */
        void                SetSheetTab(CsheetWithTabChar *pSheetTab ) { m_pSheetTab = pSheetTab; }
#endif

        /**
        	부모 시트의 페이지를 활성화 시킨다.

        	\param nPage 활성화 시킬 페이지 인덱스 
         */
        void                SetActiveParentPage(int nPage);

        /**
        	현재 이펙트 설정 페이지에서 부모 페이지로 돌아 간다.

         */
virtual void                ReturnPage();

protected:
#ifndef DEF_SKIN_SYNC
        CsheetWithTabChar*  m_pSheetTab;
#endif
};
