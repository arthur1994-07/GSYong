#ifndef CServerTriggerScriptList_H
#define CServerTriggerScriptList_H

#include "../../RanLogic/TriggerSystem/TriggerScriptList.h"

namespace TriggerSystem
{

/**	
	서버에서 스크립트 리스트 로딩을 처리 한다.

 */
class CServerTriggerScriptList
	: public CTriggerScriptList
{
public:
		/**
			서버에서 스크립트 파일 로딩을 처리 한다.			

			\param file 파일명
		 */
		void	Load(const TSTRING& file);
};

} // end namespace

#endif
