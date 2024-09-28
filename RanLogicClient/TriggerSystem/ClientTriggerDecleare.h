#ifndef ClientTriggerDecleare_H
#define ClientTriggerDecleare_H

#include "../../RanLogic/TriggerSystem/TriggerSystemType.h"
#include "../../RanLogic/TriggerSystem/TCreateLink.h"

/**
	편의를 위해 디파인의 중복되는 코드를 줄인다.

 */
#define DecleareCreateSingle(x, y, z)		DecleareCreateSingleLink(class x, TriggerType, y, z)
#define DecleareCreateDouble(x, y, z, a)	DecleareCreateDoubleLink(class x, TriggerType, y, z, a)

/**
	트리거시스템에서 트리거들을 선언 한다.
	새로 트리거를 클래스로 추가했다면 타입을 추가해 주고
	해당 타입과 연결되는 선언을 추가해 주어야 정상적으로 생성 된다.

	트리거 선언, 생성과 관련된 처리를 한곳으로 모아 이곳에서만 처리하면 추가 할 수 있다.

	클라이언트와 서버가 공통으로 가지는 트리거가 있을 수 있고 그렇지 않을 수도 있게 설계 되었다.

 */
namespace TriggerSystem
{
	/**
		트리거 구분자 선언

	 */
	enum
	{
		TRIGGER_TYPE_MSG,
		TRIGGER_TYPE_LEVEL,
		TRIGGER_TYPE_MAPINFO,
		TRIGGER_TYPE_READY,
		TRIGGER_TYPE_RETRY,
		TRIGGER_TYPE_STARTSTAGE,
        TRIGGER_TYPE_MONSTERCLEAR,
        TRIGGER_TYPE_POSITIONINGCLEAR,
		TRIGGER_TYPE_PORTALCLOSE,
        TRIGGER_TYPE_STAGE,		
		TRIGGER_TYPE_QUANTITY,
	};

	/**
		트리거 생성 헬퍼 타입 선언

	 */
	DecleareCreateSingle(CClientStageTrigger,           TRIGGER_TYPE_STAGE,	            StageTriggerLink);
	DecleareCreateDouble(CClientPortalCloseTrigger,		TRIGGER_TYPE_PORTALCLOSE,		StageTriggerLink,		PortalCloseTriggerLink);
    DecleareCreateDouble(CClientMonsterClearTrigger,    TRIGGER_TYPE_MONSTERCLEAR,	    PortalCloseTriggerLink, PositioningTriggerLink);
    DecleareCreateDouble(CClientPositioningClearTrigger,TRIGGER_TYPE_POSITIONINGCLEAR,	PositioningTriggerLink, MonsterClearTriggerLink);
    DecleareCreateDouble(CClientStartStageTrigger,	    TRIGGER_TYPE_STARTSTAGE,	    MonsterClearTriggerLink,StartStageTriggerLink);
	DecleareCreateDouble(CClientRetryTrigger,		    TRIGGER_TYPE_RETRY,			    StartStageTriggerLink,	RetryTriggerLink);
	DecleareCreateDouble(CClientReadyTrigger,		    TRIGGER_TYPE_READY,			    RetryTriggerLink,		ReadyTriggerLink);
	DecleareCreateDouble(CClientMapInfoTrigger,		    TRIGGER_TYPE_MAPINFO,		    ReadyTriggerLink,		MapInfoTriggerLink);
	DecleareCreateDouble(CClientLevelTrigger,		    TRIGGER_TYPE_LEVEL,			    MapInfoTriggerLink,		LevelTriggerLink);
	DecleareCreateDouble(CClientMsgTrigger,			    TRIGGER_TYPE_MSG,			    LevelTriggerLink,		TriggerCreator);

} // end namespace

/**
	트리거 헤더 파일 나열

 */
#include "ClientMsgTrigger.h"
#include "ClientLevelTrigger.h"
#include "ClientMapInfoTrigger.h"
#include "ClientReadyTrigger.h"
#include "ClientRetryTrigger.h"
#include "ClientStartStageTrigger.h"
#include "ClientMonsterClearTrigger.h"
#include "ClientPositioningClearTrigger.h"
#include "ClientPortalCloseTrigger.h"
#include "ClientStageTrigger.h"

#endif