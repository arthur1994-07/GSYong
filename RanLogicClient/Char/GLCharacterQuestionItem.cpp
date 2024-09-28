#include "../pch.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxSound/DxSoundLib.h"

#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharacter::MsgQuestionItemFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_QITEMFACT_BRD *pNetMsg = (GLMSG::SNETPC_QITEMFACT_BRD *)pMsg;

    if ( pNetMsg->sFACT.emType == QUESTION_NONE )
        return;

    DxSoundLib::GetInstance()->PlaySound ( "QITEM_FACT" );
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_QBox_Take,
		"-n, -f", (int) pNetMsg->sFACT.emType, pNetMsg->sFACT.fTime );

	const D3DXVECTOR3& vPosition = GetPosition();
    switch ( pNetMsg->sFACT.emType )
    {
    case QUESTION_SPEED_UP:
    case QUESTION_CRAZY:
    case QUESTION_ATTACK_UP:
    case QUESTION_EXP_UP:
    case QUESTION_LUCKY:
    case QUESTION_SPEED_UP_M:
    case QUESTION_MADNESS:
    case QUESTION_ATTACK_UP_M:
        m_sQITEMFACT.emType = pNetMsg->sFACT.emType;
        m_sQITEMFACT.fTime = pNetMsg->sFACT.fTime;
        m_sQITEMFACT.wParam1 = pNetMsg->sFACT.wParam1;
        m_sQITEMFACT.wParam2 = pNetMsg->sFACT.wParam2;
        break;

    case QUESTION_EXP_GET:
        {
            //	Note : 자기 위치 이펙트 발생시킴.
            STARGETID vTARID(CROW_PC, m_dwGaeaID, vPosition);
            DxEffGroupPlayer::GetInstance().NewEffGroup ( "QI_expget.egp", GetTransMatrix(), &vTARID, FALSE, TRUE );
        }
        break;

    case QUESTION_BOMB:
        {
            //	Note : 자기 위치 이펙트 발생시킴.
            STARGETID vTARID(CROW_PC,m_dwGaeaID, vPosition);
            DxEffGroupPlayer::GetInstance().NewEffGroup ( "QI_bomb.egp", GetTransMatrix(), &vTARID, FALSE, TRUE );
        }
        break;

    case QUESTION_MOBGEN:
        break;

    case QUESTION_HEAL:
        {
            //	Note : 자기 위치 이펙트 발생시킴.
            STARGETID vTARID(CROW_PC,m_dwGaeaID, vPosition);
            DxEffGroupPlayer::GetInstance().NewEffGroup ( "QI_heal.egp", GetTransMatrix(), &vTARID, FALSE, TRUE );
        }
        break;
    };
}


void GLCharacter::MsgQuestionItemFactEndBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_QITEMFACT_END_BRD *pNetMsg = (GLMSG::SNETPC_QITEMFACT_END_BRD *)pMsg;

    m_sQITEMFACT.RESET();
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_QBox_Reset );
}