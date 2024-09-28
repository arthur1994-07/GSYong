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

	SNATIVEID mapid;        	//@ ��ID
    DWORD gateid;           	//@ ���� ����Ʈ ID
	TSTRING mapname;        	//@ �� �̸�(XML Ű����)
	TSTRING mapObject;			//@ �ʸ�ǥ(XML Ű����) �������� UI��
	TSTRING mapinfo;        	//@ �� ����(XML Ű����)
	TSTRING mapinfoRn;			//@ ������(XML) �������� UI��
	TSTRING mapreward;      	//@ �� ����(XML Ű����)
	int dailylimit;         	//@ ���� ���� ����
	int owntime;            	//@ �ͼ� �ð�(��)
	int person;             	//@ ���� �ο� ����
    int waitentrance;       	//@ ���� Ȯ�� ��� �ð�
	TriggerLevelInfo levelinfo;	//@ ��� ���� ����
};

typedef std::list<SClientTriggerMapInfo> TriggerMapInfoList;
typedef TriggerMapInfoList::iterator TriggerMapInfoListIt;
typedef TriggerMapInfoList::const_iterator TriggerMapInfoListCIt;

/**	
	��ü �� ���� �޽���
	�� ���� Ʈ���ŷ� ���� ��ü �� ������ ���� �Ѵ�.
	
 */
struct SClientMapInfoMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);

	// ��� Ʈ���� �� ����
	TriggerMapInfoList mapinfo;
};

} // end namespace

#endif