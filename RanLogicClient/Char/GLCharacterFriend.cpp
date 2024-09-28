#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharacter::MsgFriendToFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_2_FRIEND_FB *pNetMsg = (GLMSG::SNETPC_2_FRIEND_FB *)pMsg;
    switch ( pNetMsg->emFB )
    {
    case EM2FRIEND_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FAIL") );
        break;

    case EM2FRIEND_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EM2FRIEND_FB_OK") );
        break;

    case EM2FRIEND_FB_NO_ITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_NO_ITEM") );
        break;

    case EM2FRIEND_FB_FRIEND_CONDITION:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_CONDITION") );
        break;

    case EM2FRIEND_FB_MY_CONDITION:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_MY_CONDITION") );
        break;

    case EM2FRIEND_FB_PK_CONDITION:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_PK_CONDITION") );
        break;

    case EM2FRIEND_FB_MAP_CONDITION:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_MAP_CONDITION") );
        break;

    case EM2FRIEND_FB_FRIEND_CHANNEL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_CHANNEL") );
        break;

    case EM2FRIEND_FB_IMMOVABLE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_IMMOVABLE") );
        break;

    case EM2FRIEND_FB_FRIEND_BLOCK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_BLOCK") );
        break;

    case EM2FRIEND_FB_FRIEND_OFF:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_OFF") );
        break;

    case EM2FRIEND_FB_FRIEND_INSTANTMAP:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_INSTANTMAP") );
        break;
    };
}