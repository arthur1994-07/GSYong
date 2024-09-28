#ifndef CWorldEditState_H
#define CWorldEditState_H

#include "EditState.h"

/** 
    Frame Edit (����)
    
 */
class CFrameEditState
	: public CEditState
{
public:
		CFrameEditState();

		int				GetState();

		void			SetFocus(int nOld);
		void			Reload(CParam& kParam);
		void			Insert(CParam& kParam);
        void            Archive(CParam& kParam);

        void            Asset(CParam& kParam);
        void            EditAsset(CParam& kParam) {}
        void            SelectAsset(CParam& kParam);

private:
		/**
			���� ��� �� ������ ����� �д�.
			����� �δ� ������ ���� ������ ������ ���� �� ���� ��θ� ��������
			�ʾƼ� ���ε��� �� �� ��θ� ã�� �� ���� �����̴�.

            \param bSet ���� ����(psf, wld) ���
            \param pFile ���� ���ϸ�
            \return ���� OS �� ������ ���
		 */
		TSTRING			SetBackUpCurrentDirectory(bool bSet, const TCHAR* pFile = 0);		

		TSTRING			m_strPath;	//@ ���� ����(wld, psf) ��� ���
        TSTRING			m_strFile;	//@ ���� ���ϸ� ���
};

#endif