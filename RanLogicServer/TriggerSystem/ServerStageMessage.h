#ifndef SServerStageMessage_H
#define SServerStageMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../enginelib/G-Logic/GLDefine.h"

namespace TriggerSystem
{

/**
    미션 종류

 */
enum MissionType
{
    MISSION_MONSTERCLEAR,   //@ 몬스터 잡기
    MISSION_POSITININGCLEAR,//@ 특정 위치 가기
};

/**	
	하나의 미션이 완료 되면 호출 된다.
    미션이란 몹 잡기, 특정 위치 가기 등이 된다.
    이러한 이유는 미션이 여러개가 있고 이런것이 모두 완료되어야
    스테이지를 통과하게 하기 위함 이다.
    스테이지는 몇개를 완료해야하는지 미션수를 가진다.
	
 */
struct SServerMissionCompletedMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    MissionType eType;
};

/**
     스테이지의 모든 미션이 완료될 때 호출 된다.
     우리는 스테이지의 모든 미션이 완료 될 때
     현재 보상 정보를 에이전트에 전송해 다음 스테이지에서
     해당 정보를 계속 이어서 활용하도록 처리 한다.

 */
struct SServerStageClearMessage
    : public SMessage
{
    void Command(CTriggerInstance* pInstance);

    bool bLast;         //@ 마지막 스테이지 유무
    bool bCompleted;    //@ 스테이지 성공 유무
};

/**
	스테이지에서 탈출 하고자 할 때 사용 된다.
	우리는 보상이 완료 되고 나가기 클릭시 사용 된다.

 */
struct SServerStageExitMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

	DWORD dwCharDbNum;	//@ 탈출을 요청한 캐릭터 DB
};


} // end namespace

#endif