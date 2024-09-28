#ifndef SServerLevelMessage_H
#define SServerLevelMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/PVE/MapTable.h"
#include <map>

namespace TriggerSystem
{

struct ServerLevelInfo 
{
	ServerLevelInfo();

	int level;		//@ ���� ����
	int minlevel;   //@ ���� �ּ� ���� ����
	int maxlevel;   //@ ���� �ִ� ���� ����	
};

typedef std::map<int, ServerLevelInfo> TriggerLevelInfo;
typedef TriggerLevelInfo::iterator TriggerLevelInfoIt;
typedef TriggerLevelInfo::const_iterator TriggerLevelInfoCIt;

/**	
	���� ���� ���� �޽���
    �� ���� �޽���(SServerAllMapInfoMessage, SServerMapInfoMessage)�� ���� �޽����� ������ ���� �Ѵ�.
	
 */
struct SServerLevelMessage
    : public SSystemMessage
{
	SServerLevelMessage(TriggerLevelInfo& ref) : levelinfo(ref) {}

    void Command(CTrigger* pTrigger);

    // ���� ��� ���� ����
	TriggerLevelInfo& levelinfo;
};

} // end namespace

#endif