#include "../pch.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharacter::MsgPostBoxOpenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_POSTBOX_OPEN_FB *pNetMsg = (GLMSG::SNETPC_POSTBOX_OPEN_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    default:
    case EMINVEN_POSTBOX_OPEN_FB_FAIL:
        break;

    case EMINVEN_POSTBOX_OPEN_FB_SUCCESS:
        {
            // 우편함 아이디 등록
            m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_NPC;
            m_sPOSTBOXID	  = pNetMsg->sPostBoxID;

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_PostBox_Open );
        }
        break;
    };
}


void GLCharacter::MsgInvenPostBoxOpenCardFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD_FB *pNetMsg = (GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    default:
    case EMINVEN_POSTBOX_OPEN_CARD_FB_FAIL:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_POSTBOX_OPEN_CARD_FB_FAIL") );
        }
        break;

    case EMINVEN_POSTBOX_OPEN_CARD_FB_NOITEM:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_POSTBOX_OPEN_CARD_FB_NOITEM") );
        }
        break;

    case EMINVEN_POSTBOX_OPEN_CARD_FB_SUCCESS:
        {
            // 우편 카드 아이디 등록
            m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_ITEM_CARD;
            m_sPOSTBOXID	  =  pNetMsg->sPostBoxCardID;

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_PostBox_Open );
        }
        break;
    };
}