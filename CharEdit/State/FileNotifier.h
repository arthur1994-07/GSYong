#ifndef FileNotifier_H
#define FileNotifier_H

#include "Param.h"

/**
	파일이 갱신 되었는지를 체크 한다.

 */
class CFileNotifier
{
public:
		CFileNotifier();

        enum
        {
            RETURNCODE_FAIL     = 0x00000001,	//@ 갱신 실패
            RETURNCODE_SUCCESS  = 0x00000002,	//@ 갱신 성공
            RETURNCODE_EXIT     = 0x00000004,	//@ 갱신 체크 종료
        };

		/**
			초기화를 수행한다.
			체크할 파일명과 초기 시간을 가진다.
		 */
virtual	bool			Init();

		/**
			동일한 갱신기 인지를 체크 한다.
		 */
virtual	bool			Classification(CParam& kParam);

		/**
			파일 변경을 체크 한다.

			\return 갱신 유무
		 */
		int				Notify();

        /**
			갱신 되었을 경우 처리 한다.
			
			\return 갱신 처리 코드
		 */
virtual int		        OnNotify() = 0;

        /**
			파람으로 형변환을 지원해
			여러개의 데이터를 담아 둘 수 있다.
		 */
		CParam&         Param() { return m_param; }

		/**
			파일이 변경되었을 경우 호출 된다.
		 */
virtual	void			OnChangeFile(CParam& kParam) {}

		/**
			파일명이 같은지 비교 한다.
		 */
static	bool			IsSameFile(const TCHAR* pFile1, const TCHAR* pFile2);

protected:
	
		/**
			풀 경로 파일명을 입력 한다.
		 */
		bool			SetFile(TSTRING& strFile);

		/**
			순수 파일명을 출력 한다.
		 */
		const TCHAR*	GetFileName();

		/**
			파일이 오픈 가능한지 체크 한다.
			쓰고 있을 때는 읽으면 안되므로 이 때 체크용으로 사용 한다.
		 */
		bool			IsFileOpen();

        CParam	        m_param;    //@ variable values
		TSTRING			m_strFile;	//@ 파일 경로
		time_t			m_time;		//@ 파일 갱신 시간        
};

#endif