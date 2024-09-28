#include "stdafx.h"
#include "../SigmaCore/Net/NetUtil.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Log/LogMan.h"

#include "../RanLogic/s_NetGlobal.h"
#include "../RanLogic/Msg/GLContrlServerMsg.h"
#include "../RanLogic/Util/s_CClientConsoleMsg.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Util/s_CClientConsoleMsg.h"

#include "EventMan.h"
#include "ServerViewDlg.h"
#include "ServerManager.h"
#include "Winuser.h"
#include "ChatMsgDlg.h"
#include "s_CJobSchedule.h"
#include "ServerStateDlg.h"
#include "ServerManagerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CServerManagerDlg::Notice(int ServerGroup, const char* Notice)
{
    sc::writeLogInfo(Notice);
    //CClientConsoleMessage::GetInstance()->WriteWithTime(Notice);
    NET_CHAT_CTRL2_SVRMANAGER ncc2;
    ncc2.emType		= CHAT_TYPE_CTRL_GLOBAL2;
    ncc2.nSvrGrp	= ServerGroup;
    ncc2.SetMsg(Notice);

    if (m_pNetClient)
        m_pNetClient->Send((char*) &ncc2, ncc2.Size());
}

void CServerManagerDlg::ExpUp(int ServerGroup, float Scale)
{
    std::string Msg(
        sc::string::format(
            "Exp up event. Server Group %1% Scale %2%",
            ServerGroup, Scale));
    sc::writeLogInfo(Msg);
    GLMSG::NET_EVENT_EXP_MS SendMsg(ServerGroup, Scale);
    if (m_pNetClient)
        m_pNetClient->Send((char*) &SendMsg, SendMsg.Size());
}

void CServerManagerDlg::ExpUpEnd(int ServerGroup)
{
    sc::writeLogInfo(sc::string::format("expup end %1%", ServerGroup));
    GLMSG::NET_EVENT_EXP_END_MS SendMsg(ServerGroup);
    if (m_pNetClient)
        m_pNetClient->Send((char*) &SendMsg, SendMsg.Size());
}