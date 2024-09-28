#ifndef SServerRewardMessage_H
#define SServerRewardMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/Msg/PVEMsg.h"

namespace TriggerSystem
{

/**
	인던 시간 종료와 파티 파괴 종료에 의한 실패는
	에이전트 서버로 부터 전달 되는 실패이므로 따른 실패와 구분 된다.

 */
enum EREWARD_TYPE
{
    EREWARD_TYPE_STAGE_FAILED_RETRY,	//@ 재도전 실패에 의한 보상 처리
	EREWARD_TYPE_STAGE_FAILED_TIMEOVER,	//@ 인던 시간 종료에 의한 실패에 의한 보상 처리
	EREWARD_TYPE_STAGE_FAILED_FIREPARTY,//@ 파티 파괴 종료에 의한 실패에 의한 보상 처리
    EREWARD_TYPE_STAGE_COMPLETED,		//@ 모든 스테이지 완료, 보상 처리
    EREWARD_TYPE_STAGE_CLEAR,			//@ 하나의 스테이지 완료, 보상은 하지 않음
};

/**	
	보상 정보를 전송 한다.
    우리는 스테이지를 완료 하면 보상 정보가 소멸 되는데
    다음 스테이지에서도 이전 보상 정보가 계속 누적되어 관리되어야 하므로
    에이전트를 통해 보상 정보를 필드에 전송시 이 메시지를 사용 한다.
	
 */
struct SServerRewardAddMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    GLMSG::StageResult results;
};

/**
    보상 처리를 요청 한다.
    마지막 스테이지를 성공적으로 클리어 하거나
    스테이지가 실패(완전 종료) 되면 보상을 요청하게 된다.

 */
struct SServerRewardReqMessage
    : public SMessage
{
    void Command(CTriggerInstance* pInstance);

    EREWARD_TYPE eType;  //@ 보상 타입
};

} // end namespace

#endif