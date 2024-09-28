#ifndef TriggerSystem_H
#define TriggerSystem_H

#include "TriggerSystemType.h"

/**
	트리거 시스템
	이벤트를 기반으로 진행 되어 지는 시스템
	던전 기반 컨텐츠를 개발 하기 위해 만든 시스템
	향후 다양한 컨텐츠로 사용 가능

 */
namespace TriggerSystem
{
	/**
		트리거 시스템을 초기화 한다.
		외부 설정 파일을 로딩 하여 게임에서 사용할 자료 구조를 만든다.

		\return 초기화 성공 유무
	 */
	bool			InitTriggerSystem();

	/**
		트리거 시스템을 파괴 한다.
		생성한 자료구조를 파괴 한다.

	 */
	void			DestroyTriggerSystem();

	/**
		트리거 시스템에 메시지를 전송 한다.

	 */
	void			SendMessage(struct SSystemMessage* msg);

	/**
		스크립트 파일로 부터 트리거를 생성한다.
		한번 로딩된 스크립트는 재활용 된다.

		\param pName 트리거 파일명
	 */
	template<class T>
	void			CreateScript(const TCHAR* pFile);

	/** <시스템 내부 사용 함수>
		트리거 스크립트를 삽입 한다.
        삽입된 스크립트는 실행 시간동안 계속 재활용 된다.
        또한 스크립트로 부터 임의의 트리거를 추출해 실행 할 수 있다.

		\param pFile 트리거 파일명, 구분자로 사용 하므로 중첩되지 않게 해야 한다.
		\param trigger 생성한 트리거 스크립트
	 */
	void			InsertTrigger(const TCHAR* pFile, SpTriggerScript script);

	/** <시스템 내부 사용 함수>
		시스템에 삽입된 스크립트로 부터
        트리거를 찾는다. 이렇게 찾은 스크립트는 FInger 에 의해
        트리거가 가동될 때 사용 된다.

		\param pFile 스크립트 파일명
        \param pRoot 찾을 트리거명
		\return 찾은 트리거, 없을시 NULL
	 */
	CTrigger*		FindTrigger(const TCHAR* pFile, const TCHAR* pRoot);

    /** <시스템 내부 사용 함수>
		시스템에서 입력되는 파일명을
        가지는 스크립트가 있는지 체크 한다.

		\param pFile 스크립트 파일명
		\return 스크립를 찾았다면 true, 아니라면 false
	 */
    bool            FindScript(const TCHAR* pFile);

} // end namespace

#include "TriggerSystem.inl"

#endif