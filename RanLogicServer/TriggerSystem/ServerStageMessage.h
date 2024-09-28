#ifndef SServerStageMessage_H
#define SServerStageMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../enginelib/G-Logic/GLDefine.h"

namespace TriggerSystem
{

/**
    �̼� ����

 */
enum MissionType
{
    MISSION_MONSTERCLEAR,   //@ ���� ���
    MISSION_POSITININGCLEAR,//@ Ư�� ��ġ ����
};

/**	
	�ϳ��� �̼��� �Ϸ� �Ǹ� ȣ�� �ȴ�.
    �̼��̶� �� ���, Ư�� ��ġ ���� ���� �ȴ�.
    �̷��� ������ �̼��� �������� �ְ� �̷����� ��� �Ϸ�Ǿ��
    ���������� ����ϰ� �ϱ� ���� �̴�.
    ���������� ��� �Ϸ��ؾ��ϴ��� �̼Ǽ��� ������.
	
 */
struct SServerMissionCompletedMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    MissionType eType;
};

/**
     ���������� ��� �̼��� �Ϸ�� �� ȣ�� �ȴ�.
     �츮�� ���������� ��� �̼��� �Ϸ� �� ��
     ���� ���� ������ ������Ʈ�� ������ ���� ������������
     �ش� ������ ��� �̾ Ȱ���ϵ��� ó�� �Ѵ�.

 */
struct SServerStageClearMessage
    : public SMessage
{
    void Command(CTriggerInstance* pInstance);

    bool bLast;         //@ ������ �������� ����
    bool bCompleted;    //@ �������� ���� ����
};

/**
	������������ Ż�� �ϰ��� �� �� ��� �ȴ�.
	�츮�� ������ �Ϸ� �ǰ� ������ Ŭ���� ��� �ȴ�.

 */
struct SServerStageExitMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

	DWORD dwCharDbNum;	//@ Ż���� ��û�� ĳ���� DB
};


} // end namespace

#endif