#ifndef ServerTriggerSystem_H
#define ServerTriggerSystem_H

#include "../../RanLogic/TriggerSystem/TriggerSystem.h"

class GLLandMan;
class GLGaeaServer;

/**
	서버 트리거 시스템
	서버쪽에만 국한된 트리거 관련 처리를 수행 한다.

 */
namespace TriggerSystem
{
	/**
		트리거 시스템을 초기화 한다.
		모든 스크립트를 미리 로딩 한다.

		\return 초기화 성공 유무
	 */
	bool			InitServerTriggerSystem();

	/**
		트리거 시스템을 파괴 한다.		
		생성한 자료구조를 파괴 한다.

	 */
	void			DestroyServerTriggerSystem();

	/**
		파일을 통해 트리거를 생성 한다.

		\param file 스크립트 파일명
	 */
	void			CreateServerScript(const TSTRING& file);

	/**
		트리거 작동기를 생성 한다.
		생성기 생성을 한곳에 모아 두면 향후 다양하게(오브젝트풀 등) 확장할 수 있다.

	 */
	SpTriggerFinger CreateServerFinger(GLLandMan* pLandMan, GLGaeaServer* pGLGaeaServer, int nStage, int nLevel, const std::string& strGuid);

} // end namespace

#endif