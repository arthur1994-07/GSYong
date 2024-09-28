#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "./Land/GLLandManClient.h"
#include "./Summon/GLSummonClient.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::SummonMsgProcess(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_SUMMON* pNetMsg = (GLMSG::SNET_SUMMON*) pMsg;
    std::tr1::shared_ptr<GLSummonClient> spSummonClient = m_pLandMClient->GetSummon(pNetMsg->dwGUID);
    if (spSummonClient)
        spSummonClient->MsgProcess(pMsg);
}

void GLGaeaClient::MsgDropSummon(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_SUMMON_DROP_SUMMON* pNetMsg = ( GLMSG::SNET_SUMMON_DROP_SUMMON* ) pMsg;
    SDROPSUMMON& sDropSummon = pNetMsg->Data;

    if ( sDropSummon.m_EventNodeSummonGateID != UINT_MAX )
    {
        CEVENT_NODE_CLIENT* pEvent = m_pLandMClient->GetLandEventNode ( sDropSummon.m_EventNodeSummonGateID );

        if ( pEvent )
        {
            pEvent->m_fTimeCurrent = pEvent->m_fTimeEnd;
        }
    }

    if ( sDropSummon.m_sMapID == m_pLandMClient->GetBaseMapID() )
    {
        m_pLandMClient->DropSummon ( &sDropSummon );
    }
    else
    {
        CDebugSet::ToListView ( "sDropCrow.sMapID != m_pLandMClient->GetMapID()" );
    }
}

void GLGaeaClient::MsgFailedDropSummon(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_SUMMON_FAILED_DROP_SUMMON_FC* pNetMsg =
		static_cast< GLMSG::SNET_SUMMON_FAILED_DROP_SUMMON_FC* >( pMsg );
	if ( !pNetMsg )
		return;

	switch ( pNetMsg->emFC )
	{
	case EMFAILED_FAILED_DROP_SUMMON_NONHIRE:
		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSUMMON_FB_NONSUMMON") );
		break;
	case EMFAILED_FAILED_OVERFLOW_CROWLEVEL:
		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSUMMON_FB_OVERFLOW_CROW_LEVEL") );
		break;
	}
}

void GLGaeaClient::MsgReqUseSummonFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_USE_SUMMON_FB* pNetMsg = ( GLMSG::SNETPC_REQ_USE_SUMMON_FB* ) pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMUSE_SUMMON_FB_OK:
        {
            // »ý¼º
            if ( !FAILED( CreateSummon ( pMsg ) ) )
            {
				//!! SF_TODO
                //m_pInterface->UiShowGroupBottom( SUMMON_HP );	
                //m_pInterface->UiShowGroupBottom( SUMMON_POSION_DISPLAY );
            }

        }
        break;

    case EMUSE_SUMMON_FB_FAIL_INVALIDZONE:
        PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSUMMON_FB_INVALIDZONE") );
        break;
    case EMUSE_SUMMON_FB_FAIL_NODATA:
        PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSUMMON_FB_NODATA") );
        break;
    case EMUSE_SUMMON_FB_FAIL_VEHICLE:
        PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSUMMON_FB_VEHICLE") );
        break;
    case EMUSE_SUMMON_FB_FAIL_CONFRONT:
        PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSUMMON_FB_CONFRONT") );
        break;
    }
}

void GLGaeaClient::MsgReqHireSummonFC(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_REQ_HIRE_BASICSUMMON_FC* pNetMsg =
		static_cast< GLMSG::SNETPC_REQ_HIRE_BASICSUMMON_FC* >( pMsg );
	switch ( pNetMsg->emFC )
	{
	case EMFAILED_HIRE_SUMMON_OK:
		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMFAILED_HIRE_SUMMON_OK_TEXT") );
		break;
	case EMFAILED_HIRE_SUMMON_FAIL:
		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMFAILED_HIRE_SUMMON_FAIL_TEXT") );
		break;
	case EMFAILED_HIRE_SUMMON_DONT_LEARN_PASSIVE:
		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMFAILED_HIRE_SUMMON_DONT_LEARN_TEXT") );
		break;
	case EMFAILED_HIRE_SUMMON_INVALID_CROWDATA:
		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMFAILED_HIRE_SUMMON_INVALID_CROWDATA_TEXT") );
		break;
	case EMFAILED_HIRE_SUMMON_CROWLEVEL_OVERFLOW:
		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMFAILED_HIRE_SUMMON_CROWLEVEL_OVERFLOW_TEXT") );
		break;
	}
}