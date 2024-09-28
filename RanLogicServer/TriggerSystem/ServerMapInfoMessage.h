#ifndef SServerMapInfoMessage_H
#define SServerMapInfoMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "ServerLevelMessage.h"
#include <map>

namespace TriggerSystem
{

struct SServerTriggerMapInfo
{
	SServerTriggerMapInfo();

	SNATIVEID mapid;    		//@ 맵ID
    DWORD gateid;       		//@ 입장 게이트 ID
	int dailylimit;     		//@ 일일 입장 제한
	int owntime;        		//@ 귀속 시간(초)
	int person;         		//@ 입장 인원 제한
    int waitentrance;			//@ 입장 확인 대기 시간
	TriggerLevelInfo levelinfo;	//@ 모든 레벨 정보
};

typedef std::map<SNATIVEID, SServerTriggerMapInfo> TriggerMapInfoContainer;
typedef TriggerMapInfoContainer::iterator TriggerMapInfoContainerIterator;
typedef TriggerMapInfoContainer::const_iterator TriggerMapInfoContainerConstIterator;

/**	
	전체 맵 정보 메시지
	모든 맵 정보를 추출 한다.
	
 */
struct SServerAllMapInfoMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);

	// 모든 트리거 맵 정보
	TriggerMapInfoContainer mapinfo;
};

/**	
	맵 정보 메시지
	mapinfo.wMainID 에 입력된 하나의 맵 정보를 추출한다.
	
 */
struct SServerMapInfoMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);

	// 트리거 맵 정보
	SServerTriggerMapInfo mapinfo;
};

} // end namespace

#endif