
#pragma once

#include "../../EngineLib/GUInterface/UIControlMessage.h"
#include "../../EngineLib/GUInterface/UIWindow.h"

namespace FLYCAMERA_UI_MSG_DEFINE
{
	// Slot->Page Msg
	// 이동, 회전 값 갱신 메시지;
	const DWORD dwFLYCAMERA_UIMSG_VALUE_UPDATE = UIMSG_USER1;//0x01000000,// UIMSG_USER1,	// MAKE_UIMSG

	// Key frame TimeLine-> Key frame page
	// Key 추가;
	const DWORD dwFLYCAMERA_UIMSG_KEY_ADD = UIMSG_USER1;
	const DWORD dwFLYCAMERA_UIMSG_KEY_DEL = UIMSG_USER2;
	const DWORD dwFLYCAMERA_UIMSG_KEY_SELECT = UIMSG_USER3;

	// Key frame List -> Key frame page
	const DWORD dwFLYCAMERA_UIMSG_DETAILPAGE_ACTIVITY = UIMSG_USER1;

	// Key frame page -> Detail page
	const DWORD dwFLYCAMERA_UIMSG_DETAILPAGE_SET_DATA = UIMSG_USER1;
};