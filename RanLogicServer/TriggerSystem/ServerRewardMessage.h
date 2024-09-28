#ifndef SServerRewardMessage_H
#define SServerRewardMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/Msg/PVEMsg.h"

namespace TriggerSystem
{

/**
	�δ� �ð� ����� ��Ƽ �ı� ���ῡ ���� ���д�
	������Ʈ ������ ���� ���� �Ǵ� �����̹Ƿ� ���� ���п� ���� �ȴ�.

 */
enum EREWARD_TYPE
{
    EREWARD_TYPE_STAGE_FAILED_RETRY,	//@ �絵�� ���п� ���� ���� ó��
	EREWARD_TYPE_STAGE_FAILED_TIMEOVER,	//@ �δ� �ð� ���ῡ ���� ���п� ���� ���� ó��
	EREWARD_TYPE_STAGE_FAILED_FIREPARTY,//@ ��Ƽ �ı� ���ῡ ���� ���п� ���� ���� ó��
    EREWARD_TYPE_STAGE_COMPLETED,		//@ ��� �������� �Ϸ�, ���� ó��
    EREWARD_TYPE_STAGE_CLEAR,			//@ �ϳ��� �������� �Ϸ�, ������ ���� ����
};

/**	
	���� ������ ���� �Ѵ�.
    �츮�� ���������� �Ϸ� �ϸ� ���� ������ �Ҹ� �Ǵµ�
    ���� �������������� ���� ���� ������ ��� �����Ǿ� �����Ǿ�� �ϹǷ�
    ������Ʈ�� ���� ���� ������ �ʵ忡 ���۽� �� �޽����� ��� �Ѵ�.
	
 */
struct SServerRewardAddMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    GLMSG::StageResult results;
};

/**
    ���� ó���� ��û �Ѵ�.
    ������ ���������� ���������� Ŭ���� �ϰų�
    ���������� ����(���� ����) �Ǹ� ������ ��û�ϰ� �ȴ�.

 */
struct SServerRewardReqMessage
    : public SMessage
{
    void Command(CTriggerInstance* pInstance);

    EREWARD_TYPE eType;  //@ ���� Ÿ��
};

} // end namespace

#endif