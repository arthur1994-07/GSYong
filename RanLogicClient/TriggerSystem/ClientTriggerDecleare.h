#ifndef ClientTriggerDecleare_H
#define ClientTriggerDecleare_H

#include "../../RanLogic/TriggerSystem/TriggerSystemType.h"
#include "../../RanLogic/TriggerSystem/TCreateLink.h"

/**
	���Ǹ� ���� �������� �ߺ��Ǵ� �ڵ带 ���δ�.

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
		TRIGGER_TYPE_RETRY,
		TRIGGER_TYPE_STARTSTAGE,
        TRIGGER_TYPE_MONSTERCLEAR,
        TRIGGER_TYPE_POSITIONINGCLEAR,
		TRIGGER_TYPE_PORTALCLOSE,
        TRIGGER_TYPE_STAGE,		
		TRIGGER_TYPE_QUANTITY,
	};

	/**
		Ʈ���� ���� ���� Ÿ�� ����

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
	Ʈ���� ��� ���� ����

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