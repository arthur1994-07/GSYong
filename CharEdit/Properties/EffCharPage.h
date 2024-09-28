#pragma once

#include "PieceRelativePage.h"

class   DxEffChar;
struct  EFFCHAR_PROPERTY;

/**
	ĳ���Ϳ� �ٴ� �����͵��� �����ϴ� �������� �⺻�� �Ǵ� Ŭ����
    �⺻������ ó���� �����̳� ����Ǵ� ������ �������̽��� ���� �Ѵ�.
	
 */
class CEffCharPage
    : public CPieceRelativePage
{
public:
        CEffCharPage(UINT nIDTemplate);
virtual ~CEffCharPage();

        /**
        	�ܺο��� �����ϰ��� �ϴ� �����͸� ���� �Ѵ�.
            
        	\param pEffChar �����ϰ��� �ϴ� ������
         */
        void                SetInstance(DxEffChar* pEffChar) { m_pEffChar = pEffChar; }

        /**
            �ܺο��� �����ϰ��� �ϴ� �������� �Ӽ��� ���� �Ѵ�.
            �ΰ����� �Է½� ���� ������ �ǹ� �Ѵ�.
        	
	        \param *pProp �����ϰ��� �ϴ� ������ �Ӽ�, NULL �� �Է½� �� �Ӽ� ����
         */
virtual void                SetProperty(EFFCHAR_PROPERTY *pProp = NULL) = 0;

        /**
        	���� ����Ʈ ���� ���������� �θ� �������� ���� ����.

         */
virtual void                ReturnPage();

        /**
        	����� ����Ʈ ������ ���� �Ѵ�.

         */
        void                EffectApply();

        /**
            �� �𸣰����� ��� ����Ʈ�� Ư���� �������̽��� �䱸�Ѵ�.            

         */
virtual void                SetSpinControl(int nColorHeight) {}
virtual void                ResetDialog() {}
virtual void                UpdateKeyBoardMouse() {}

protected:
	    DxEffChar*			m_pEffChar;
};
