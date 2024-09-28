#include "../pch.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLCharacter::MsgChinaGainType(NET_MSG_GENERIC* pMsg)
{
#ifdef CH_PARAM_USEGAIN //chinatest%%%
    GLMSG::SNETPC_CHINA_GAINTYPE *pNetMsg = (GLMSG::SNETPC_CHINA_GAINTYPE *) pMsg;
    if( pNetMsg->dwGainType == 1 )
    {
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CHINA_GAIN_50PERCENT") );
    }else if( pNetMsg->dwGainType == 2 )
    {
        //				m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CHINA_GAIN_0PERCENT") );				
        //				m_pInterface->WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("WAITSERVER_MESSAGE"), WAITSERVER_CLOSEGAME );
        //				m_pInterface->WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("CHINA_GAIN_CLOSEGAME"), WAITSERVER_CLOSEGAME, 30 );
        //m_pInterface->DoModal( ID2GAMEINTEXT("CHINA_GAIN_CLOSEGAME"), UI::MODAL_INFOMATION, UI::OK, UI::MODAL_CLOSEGAME );
    }/*else if( pNetMsg->dwGainType == 3 )
     {
     m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CHINA_GAIN_15MINUTE") );				
     }*/else if( pNetMsg->dwGainType == 4 )
    {
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CHINA_GAIN_1HOUR") );				
    }
#endif
}