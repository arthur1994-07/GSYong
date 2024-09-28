#pragma once

#include "../../SigmaCore/Container/MList.h"
struct NET_MSG_GENERIC;

class DxMsgProcessor
{
public:
	virtual void MsgProcess ( NET_MSG_GENERIC* nmg ) = 0;
};
