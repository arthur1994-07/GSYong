#ifndef ServerTriggerDecleare_H
#define ServerTriggerDecleare_H

#include "../../RanLogic/TriggerSystem/TriggerSystemType.h"
#include "../../RanLogic/TriggerSystem/TCreateLink.h"

/**
	편의를 위해 디파인의 중복되는 파람을 줄인다.

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
		TRIGGER_TYPE_DIFFICULTY,
		TRIGGER_TYPE_LAYER,
		TRIGGER_TYPE_RETRY,
		TRIGGER_TYPE_MONSTERCLEAR,
		TRIGGER_TYPE_POSITIONINGCLEAR,
		TRIGGER_TYPE_WAIT,
		TRIGGER_TYPE_PORTALOPEN,
		TRIGGER_TYPE_PORTALCLOSE,
        TRIGGER_TYPE_STAGE,
        TRIGGER_TYPE_STARTSTAGE,
        TRIGGER_TYPE_POSTSTAGE,
        TRIGGER_TYPE_REWARD,
		TRIGGER_TYPE_PARTY,
		TRIGGER_TYPE_RECOGNIZE,
		TRIGGER_TYPE_QUANTITY,
	};

	/**
		트리거 생성 헬퍼 타입 선언

        트리거 클래스, 트리거 타입, 앞쪽 링크, 뒷쪽 링크 순

	 */
	DecleareCreateSingle(CServerRewardTrigger,		    TRIGGER_TYPE_REWARD,		    RecognizeTriggerLink);
	DecleareCreateDouble(CServerRecognizeTrigger,		TRIGGER_TYPE_RECOGNIZE,			RecognizeTriggerLink,		PartyTriggerLink);
	DecleareCreateDouble(CServerPartyTrigger,			TRIGGER_TYPE_PARTY,				PartyTriggerLink,		    RewardTriggerLink);
    DecleareCreateDouble(CServerPostStageTrigger,		TRIGGER_TYPE_POSTSTAGE,		    RewardTriggerLink,		    PostStageTriggerLink);
    DecleareCreateDouble(CServerStartStageTrigger,		TRIGGER_TYPE_STARTSTAGE,		PostStageTriggerLink,		StartStageTriggerLink);
    DecleareCreateDouble(CServerStageTrigger,		    TRIGGER_TYPE_STAGE,		        StartStageTriggerLink,		StageTriggerLink);
    DecleareCreateDouble(CServerPortalCloseTrigger,		TRIGGER_TYPE_PORTALCLOSE,		StageTriggerLink,		    PortalCloseTriggerLink);
	DecleareCreateDouble(CServerPortalOpenTrigger,		TRIGGER_TYPE_PORTALOPEN,		PortalCloseTriggerLink,		PortalOpenTriggerLink);
	DecleareCreateDouble(CServerWaitTrigger,			TRIGGER_TYPE_WAIT,				PortalOpenTriggerLink,		WaitTriggerLink);
	DecleareCreateDouble(CServerPositioningClearTrigger,TRIGGER_TYPE_POSITIONINGCLEAR,	WaitTriggerLink,			PositioningClearTriggerLink);
	DecleareCreateDouble(CServerMonsterClearTrigger,	TRIGGER_TYPE_MONSTERCLEAR,		PositioningClearTriggerLink,MonsterClearTriggerLink);
	DecleareCreateDouble(CServerRetryTrigger,			TRIGGER_TYPE_RETRY,				MonsterClearTriggerLink,	RetryTriggerLink);
	DecleareCreateDouble(CServerLayerTrigger,			TRIGGER_TYPE_LAYER,				RetryTriggerLink,			LayerTriggerLink);
	DecleareCreateDouble(CServerDifficultyTrigger,		TRIGGER_TYPE_DIFFICULTY,		LayerTriggerLink,			DifficultyTriggerLink);
	DecleareCreateDouble(CServerReadyTrigger,			TRIGGER_TYPE_READY,				DifficultyTriggerLink,		ReadyTriggerLink);
	DecleareCreateDouble(CServerMapInfoTrigger,			TRIGGER_TYPE_MAPINFO,			ReadyTriggerLink,			MapInfoTriggerLink);
	DecleareCreateDouble(CServerLevelTrigger,			TRIGGER_TYPE_LEVEL,				MapInfoTriggerLink,			LevelTriggerLink);
	DecleareCreateDouble(CServerMsgTrigger,				TRIGGER_TYPE_MSG,				LevelTriggerLink,			TriggerCreator);

} // end namespace

/**
	트리거 헤더 파일 나열

 */
#include "ServerMsgTrigger.h"
#include "ServerLevelTrigger.h"
#include "ServerMapInfoTrigger.h"
#include "ServerReadyTrigger.h"
#include "ServerDifficultyTrigger.h"
#include "ServerLayerTrigger.h"
#include "ServerRetryTrigger.h"
#include "ServerMonsterClearTrigger.h"
#include "ServerPositioningClearTrigger.h"
#include "ServerWaitTrigger.h"
#include "ServerPortalOpenTrigger.h"
#include "ServerPortalCloseTrigger.h"
#include "ServerStageTrigger.h"
#include "ServerStartStageTrigger.h"
#include "ServerRewardTrigger.h"
#include "ServerPostStageTrigger.h"
#include "ServerPartyTrigger.h"
#include "ServerRecognizeTrigger.h"

#endif
