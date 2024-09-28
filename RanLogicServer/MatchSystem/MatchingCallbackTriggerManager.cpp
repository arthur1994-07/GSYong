#pragma once
#include "pch.h"
#include <vector>

#include "../../RanLogic/MatchSystem/GLCallbackTrigger.h"
#include "../../RanLogic/MatchSystem/MatchModel.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"
#include "MatchingCallbackTriggerDefine.h"
#include "MatchingCallbackTriggerManager.h"

using namespace CallbackUtility;

namespace MatchingSystem
{
	MatchingCallbackTriggerManager MatchingCallbackTriggerManager::instance;
}