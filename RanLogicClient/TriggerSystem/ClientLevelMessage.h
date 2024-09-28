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

	int level;		//@ ���� ����
	int minlevel;   //@ ���� �ּ� ���� ����
	int maxlevel;   //@ ���� �ִ� ���� ����
	bool possible;	//@ ���� ���� ����
};

typedef std::map<int, ClientLevelInfo> TriggerLevelInfo;
typedef TriggerLevelInfo::iterator TriggerLevelInfoIt;
typedef TriggerLevelInfo::const_iterator TriggerLevelInfoCIt;

/**	
	���� ���� ���� �޽���
    �� ���� �޽���(SClientMapInfoMessage)�� ���� �޽����� ������ ���� �Ѵ�.
	
 */
struct SClientLevelMessage
    : public SSystemMessage
{
	SClientLevelMessage(TriggerLevelInfo& ref) : levelinfo(ref) {}

    void Command(CTrigger* pTrigger);

    // ���� ��� ���� ����
	TriggerLevelInfo& levelinfo;
};

} // end namespace

#endif