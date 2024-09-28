#ifndef ServerTriggerDecleare_H
#define ServerTriggerDecleare_H

#include "../../RanLogic/TriggerSystem/TriggerSystemType.h"
#include "../../RanLogic/TriggerSystem/TCreateLink.h"

/**
	���Ǹ� ���� �������� �ߺ��Ǵ� �Ķ��� ���δ�.

 */
#define DecleareCreateSingle(x, y, z)		DecleareCreateSingleLink(class x, TriggerType, y, z)
#define DecleareCreateDouble(x, y, z, a)	DecleareCreateDoubleLink(class x, TriggerType, y, z, a)

/**
	Ʈ���Žý��ۿ��� Ʈ���ŵ��� ���� �Ѵ�.
	���� Ʈ���Ÿ� Ŭ������ �߰��ߴٸ� Ÿ���� �߰��� �ְ�
	�ش� Ÿ�԰� ����Ǵ� ������ �߰��� �־�� ���������� ���� �ȴ�.

	Ʈ���� ����, ������ ���õ� ó���� �Ѱ����� ��� �̰������� ó���ϸ� �߰� �� �� �ִ�.

	Ŭ���̾�Ʈ�� ������ �������� ������ Ʈ���Ű� ���� �� �ְ� �׷��� ���� ���� �ְ� ���� �Ǿ���.

 */
namespace TriggerSystem
{
	/**
		Ʈ���� ������ ����

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
		Ʈ���� ���� ���� Ÿ�� ����

        Ʈ���� Ŭ����, Ʈ���� Ÿ��, ���� ��ũ, ���� ��ũ ��

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
	Ʈ���� ��� ���� ����

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
