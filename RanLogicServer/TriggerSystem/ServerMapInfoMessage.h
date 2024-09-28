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

	SNATIVEID mapid;    		//@ ��ID
    DWORD gateid;       		//@ ���� ����Ʈ ID
	int dailylimit;     		//@ ���� ���� ����
	int owntime;        		//@ �ͼ� �ð�(��)
	int person;         		//@ ���� �ο� ����
    int waitentrance;			//@ ���� Ȯ�� ��� �ð�
	TriggerLevelInfo levelinfo;	//@ ��� ���� ����
};

typedef std::map<SNATIVEID, SServerTriggerMapInfo> TriggerMapInfoContainer;
typedef TriggerMapInfoContainer::iterator TriggerMapInfoContainerIterator;
typedef TriggerMapInfoContainer::const_iterator TriggerMapInfoContainerConstIterator;

/**	
	��ü �� ���� �޽���
	��� �� ������ ���� �Ѵ�.
	
 */
struct SServerAllMapInfoMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);

	// ��� Ʈ���� �� ����
	TriggerMapInfoContainer mapinfo;
};

/**	
	�� ���� �޽���
	mapinfo.wMainID �� �Էµ� �ϳ��� �� ������ �����Ѵ�.
	
 */
struct SServerMapInfoMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);

	// Ʈ���� �� ����
	SServerTriggerMapInfo mapinfo;
};

} // end namespace

#endif