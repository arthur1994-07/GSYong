
#include "pch.h"

#include "../RanLogic/Msg/GLContrlMsg.h"

#include "./GLGaeaClient.h"

void GLGaeaClient::SfReqPeriodExtension(WORD wHoldPosX, WORD wHoldPosY, WORD wPosX, WORD wPosY)
{
	//! 기간연장;
	GLMSG::SNET_INVEN_PERIODEXTEND NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.wHoldPosX = wHoldPosX;
	NetMsg.wHoldPosY = wHoldPosY;
	NetMsg.bReconfirm = false;
	NETSENDTOFIELD(&NetMsg);
}

void GLGaeaClient::SfPeriodExtensionReconfirm(WORD wHoldPosX, WORD wHoldPosY, WORD wPosX, WORD wPosY)
{
	//! 기간연장;
	GLMSG::SNET_INVEN_PERIODEXTEND NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.wHoldPosX = wHoldPosX;
	NetMsg.wHoldPosY = wHoldPosY;
	NetMsg.bReconfirm = true;
	NETSENDTOFIELD(&NetMsg);
}