#ifndef SClientMapInfoMessage_H
#define SClientMapInfoMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "ClientLevelMessage.h"
#include <list>

namespace TriggerSystem
{

struct SClientTriggerMapInfo
{
	SClientTriggerMapInfo();

	SNATIVEID mapid;        	//@ 맵ID
    DWORD gateid;           	//@ 입장 게이트 ID
	TSTRING mapname;        	//@ 맵 이름(XML 키워드)
	TSTRING mapObject;			//@ 맵목표(XML 키워드) 통합전장 UI용
	TSTRING mapinfo;        	//@ 맵 정보(XML 키워드)
	TSTRING mapinfoRn;			//@ 맵정보(XML) 통합전장 UI용
	TSTRING mapreward;      	//@ 맵 보상(XML 키워드)
	int dailylimit;         	//@ 일일 입장 제한
	int owntime;            	//@ 귀속 시간(초)
	int person;             	//@ 입장 인원 제한
    int waitentrance;       	//@ 입장 확인 대기 시간
	TriggerLevelInfo levelinfo;	//@ 모든 레벨 정보
};

typedef std::list<SClientTriggerMapInfo> TriggerMapInfoList;
typedef TriggerMapInfoList::iterator TriggerMapInfoListIt;
typedef TriggerMapInfoList::const_iterator TriggerMapInfoListCIt;

/**	
	전체 맵 정보 메시지
	맵 정보 트리거로 부터 전체 맵 정보를 추출 한다.
	
 */
struct SClientMapInfoMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);

	// 모든 트리거 맵 정보
	TriggerMapInfoList mapinfo;
};

} // end namespace

#endif