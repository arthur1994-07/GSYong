#ifndef CWorldEditState_H
#define CWorldEditState_H

#include "EditState.h"

/** 
    Frame Edit (형상)
    
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
			현재 경로 및 파일을 백업해 둔다.
			백업해 두는 이유는 메인 프레임 파일을 읽을 때 따로 경로를 설정하지
			않아서 리로딩을 할 때 경로를 찾을 수 없기 때문이다.

            \param bSet 메인 파일(psf, wld) 경로
            \param pFile 메인 파일명
            \return 현재 OS 에 설정된 경로
		 */
		TSTRING			SetBackUpCurrentDirectory(bool bSet, const TCHAR* pFile = 0);		

		TSTRING			m_strPath;	//@ 메인 파일(wld, psf) 경로 백업
        TSTRING			m_strFile;	//@ 메인 파일명 백업
};

#endif