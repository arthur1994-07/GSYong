#include "../pch.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLCharacter::MsgVietnamTimeReqFb(NET_MSG_GENERIC* pMsg)
{
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		GLMSG::SNETPC_VIETNAM_TIME_REQ_FB *pNetMsg = (GLMSG::SNETPC_VIETNAM_TIME_REQ_FB *)pMsg;
		m_sVietnamSystem.loginTime = pNetMsg->loginTime;
		m_sVietnamSystem.gameTime  = pNetMsg->gameTime;
		m_VietnamGameTime  = pNetMsg->gameTime;
	}
//#endif
}

void GLCharacter::MsgVietnamGainExp(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_VIETNAM_GAIN_EXP *pNetMsg = (GLMSG::SNETPC_VIETNAM_GAIN_EXP *) pMsg;
    m_lVNGainSysExp = pNetMsg->gainExp;
}

void GLCharacter::MsgVietnamGainMoney(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_VIETNAM_GAIN_MONEY *pNetMsg = (GLMSG::SNETPC_VIETNAM_GAIN_MONEY *) pMsg;
    m_lVNGainSysMoney = pNetMsg->gainMoney;
}


void GLCharacter::MsgVietnamGainType(NET_MSG_GENERIC* pMsg)
{
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		GLMSG::SNETPC_VIETNAM_GAINTYPE *pNetMsg = (GLMSG::SNETPC_VIETNAM_GAINTYPE *) pMsg;
		if( pNetMsg->dwGainType == GAINTYPE_HALF )
		{
			m_dwVietnamGainType = GAINTYPE_HALF;
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VIETNAM_GAINTYPE_HALF") );
		}else if( pNetMsg->dwGainType == GAINTYPE_EMPTY )
		{
			m_dwVietnamGainType = GAINTYPE_EMPTY;
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VIETNAM_GAINTYPE_EMPTY") );
		}
	}
//#endif
}