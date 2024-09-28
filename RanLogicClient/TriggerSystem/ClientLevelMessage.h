#ifndef SClientLevelMessage_H
#define SClientLevelMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/PVE/MapTable.h"
#include <map>

namespace TriggerSystem
{

struct ClientLevelInfo 
{
	ClientLevelInfo();

	int level;		//@ 현재 레벨
	int minlevel;   //@ 입장 최소 레벨 제한
	int maxlevel;   //@ 입장 최대 레벨 제한
	bool possible;	//@ 입장 가능 유무
};

typedef std::map<int, ClientLevelInfo> TriggerLevelInfo;
typedef TriggerLevelInfo::iterator TriggerLevelInfoIt;
typedef TriggerLevelInfo::const_iterator TriggerLevelInfoCIt;

/**	
	레벨 정보 추출 메시지
    맵 정보 메시지(SClientMapInfoMessage)의 서브 메시지로 레벨을 추출 한다.
	
 */
struct SClientLevelMessage
    : public SSystemMessage
{
	SClientLevelMessage(TriggerLevelInfo& ref) : levelinfo(ref) {}

    void Command(CTrigger* pTrigger);

    // 맵의 모든 레벨 정보
	TriggerLevelInfo& levelinfo;
};

} // end namespace

#endif