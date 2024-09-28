#pragma once

#include "afxwin.h"

#ifndef DEF_SKIN_SYNC
class   CsheetWithTabChar;
#endif

/**
	������ �پ� ���Ǵ� ���������� �⺻ ������
    ��� ����Ǵ� �������̽����� ���� �Ѵ�.
	
 */
class CPieceRelativePage
    : public CPropertyPage
{
public:
        CPieceRelativePage(UINT nIDTemplate);
virtual ~CPieceRelativePage();

#ifndef DEF_SKIN_SYNC
        /**
        	�θ� �������� ���� �Ѵ�.

        	\param *pSheetTab 
         */
        void                SetSheetTab(CsheetWithTabChar *pSheetTab ) { m_pSheetTab = pSheetTab; }
#endif

        /**
        	�θ� ��Ʈ�� �������� Ȱ��ȭ ��Ų��.

        	\param nPage Ȱ��ȭ ��ų ������ �ε��� 
         */
        void                SetActiveParentPage(int nPage);

        /**
        	���� ����Ʈ ���� ���������� �θ� �������� ���� ����.

         */
virtual void                ReturnPage();

protected:
#ifndef DEF_SKIN_SYNC
        CsheetWithTabChar*  m_pSheetTab;
#endif
};
