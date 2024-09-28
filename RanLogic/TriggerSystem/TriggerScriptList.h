#ifndef CTriggerScriptList_H
#define CTriggerScriptList_H

#include "../RanLogic/Script/SquirrelMan.h"
#include "TriggerSystemType.h"

namespace TriggerSystem
{

/**	
	트리거 시스템은 모두 스크립트 기반으로 이루어져 있으며
	스크립트 파일 리스트를 처리 한다.

 */
class CTriggerScriptList
	: public SquirrelMan
{
public:
virtual ~CTriggerScriptList() {}

		/**
			리스트 스크립를 읽어 로딩을 호출 한다.
			서버, 클라 관계없이 공통 코드

		 */
		void	ToDo();

		/**
			실제 스크립트 파일 로딩을 처리 한다.
			서버와 클라 각각 다른 방법으로 처리 한다.

			\param file 파일명
		 */
virtual void	Load(const TSTRING& file) = 0;
};

} // end namespace

#endif
