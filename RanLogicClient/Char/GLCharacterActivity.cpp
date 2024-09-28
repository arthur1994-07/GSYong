#include "../pch.h"
#include "../../Sigmacore/String/StringFormat.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLCharacter::MsgActivityUpdateComplate(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED* pNetMsg = (GLMSG::SNET_ACTIVITY_UPDATE_COMPLETED*) nmg;

	for ( INT i = 0; i < pNetMsg->dwArrSize; ++i )
	{
		Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( pNetMsg->arrdwActivityID[i] );
		if ( !pActivity || pActivity->GetStatus() == ACTIVITY_STATUS_COMPLETED ) 
			continue;

		pActivity->SetStatus( ACTIVITY_STATUS_COMPLETED );

		// 보상 처리;
		std::string strTitle = pActivity->GetRewardTitleID();
		int nPoint  = pActivity->GetRewardPoint();

		if ( strTitle != "" )
			m_TitleManager.Add( strTitle, pActivity->m_nClass );   

		if ( nPoint > 0 ) 
			m_llActivityPoint[pActivity->m_nClass] += nPoint;

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );

		// 진행 텍스트 출력;
		if ( GLUseFeatures::GetInstance().IsUsingActivity() )
		{
			std::string strText = sc::string::format( ID2GAMEINTEXT( "ACTIVITY_UPDATE_COMPLATE" ), pActivity->GetTitle() );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::ACTIVITY, strText.c_str() );
		}
	}
    
    // 이펙트 출력;
    STARGETID sTargetID(CROW_PC,m_dwGaeaID, GetPosition());
    DxEffGroupPlayer::GetInstance().NewEffGroup
        (
        GLCONST_CHAR::strCOMPLATE_ACTIVITY_EFFECT.c_str(),
        GetTransMatrix(),
        &sTargetID,
		FALSE, 
		TRUE
        );
}

void GLCharacter::MsgActivityUpdateValue(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ACTIVITY_UPDATE_VALUE* pNetMsg = (GLMSG::SNET_ACTIVITY_UPDATE_VALUE*) nmg;

    Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( pNetMsg->dwActivityID );
    if ( !pActivity ) 
        return;

    if ( pActivity->GetStatus() != ACTIVITY_STATUS_INPROGRESS ) 
        return;

    pActivity->SetValue( pNetMsg->nValue );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );

    // 레벨업이 아닐경우 진행 텍스트 출력
    if ( pActivity->m_id.wMainID != ACTIVITY_MID_LEVEL )
    {
        UINT nValue     = pActivity->GetValue();
        UINT nValueBase = pActivity->m_nValueBase;

		//!! SF_TODO
        //if ( GLUseFeatures::GetInstance().IsUsingActivity() && m_pInterface->IsActivityProgressDisplay() && (nValue != nValueBase) )
        //{
        //    m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_PROGRESS, 
        //        sc::string::format( ID2GAMEINTEXT( "ACTIVITY_UPDATE_PROGRESS_PROGRESSDISPLAY" ), pActivity->GetTitle(), nValue, nValueBase ) );
        //}
		if ( GLUseFeatures::GetInstance().IsUsingActivity() && (nValue != nValueBase) && (strlen(pActivity->GetTitle().c_str()) != 0) )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_StudentRecord_ProgressMessege
				, "-s, -n, -n", pActivity->GetTitle().c_str(), (int)nValue, (int)nValueBase);
		}
    }
}


void GLCharacter::MsgActivityNotifyComplate(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ACTIVITY_NOTIFY_COMPLETED* pNetMsg = (GLMSG::SNET_ACTIVITY_NOTIFY_COMPLETED*) nmg;

    Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( pNetMsg->dwActivityID );
    if ( !pActivity ) 
        return;

    std::string strName = pNetMsg->szChaName;
    std::string strText;

    if ( pActivity->m_nNotifyLevel == ACTIVITY_NOTIFY_CLUB || pActivity->m_nNotifyLevel == ACTIVITY_NOTIFY_CLUBANDAROUND )
    {
        strText = sc::string::format( ID2GAMEINTEXT( "ACTIVITY_NOTIFY_COMPLATE_CLUB" ), strName, pActivity->GetTitle() );
    }
    else
    {
        strText = sc::string::format( ID2GAMEINTEXT( "ACTIVITY_NOTIFY_COMPLATE" ), strName, pActivity->GetTitle() );
    }

    if ( GLUseFeatures::GetInstance().IsUsingActivity() )
    {
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::ITEMMONEY, strText.c_str() );
    }
}


void GLCharacter::MsgActivityNotifyClosed(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ACTIVITY_NOTIFY_CLOSED* pNetMsg = (GLMSG::SNET_ACTIVITY_NOTIFY_CLOSED*) nmg;

    Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( pNetMsg->dwActivityID );
    if ( !pActivity ) 
        return;

    if ( pActivity->GetStatus() != ACTIVITY_STATUS_INPROGRESS ) 
        return;

    pActivity->SetStatus( ACTIVITY_STATUS_CLOSED );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );

    std::string strText = sc::string::format( ID2GAMEINTEXT( "ACTIVITY_NOTIFY_CLOSED" ), pActivity->GetTitle() );
    
    if ( GLUseFeatures::GetInstance().IsUsingActivity() )
    {
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, strText.c_str() );
    }
}


void GLCharacter::MsgActivitySync(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ACTIVITY_SYNC* pNetMsg = (GLMSG::SNET_ACTIVITY_SYNC*) nmg;

    if ( pNetMsg->wSeq == 0 )
    {
        ActivityBaseData::GetInstance()->ResetData();
    }

    ActivityBaseData::GetInstance()->SyncActivityStatus( pNetMsg );

    if ( pNetMsg->wSeq == pNetMsg->wLastSeq )
    {
        // 아이템 획득 갯수를 동기화 한다.
        MMACTIVITIES&     mmActivities = ActivityBaseData::GetInstance()->GetData();
        ITER_MMACTIVITIES pos          = mmActivities.begin();
        ITER_MMACTIVITIES pos_end      = mmActivities.end();

        DWORD dwCount = 0;

        for ( ; pos != pos_end; ++pos )
        {
            Activity* pActivity = &pos->second;

            if (pActivity->GetType() == ACTIVITY_CATEGORY_TAKE)
            {
                DWORD dwCount(0);
                for ( size_t i = 0; i < pActivity->m_vecTargetID.size(); i++ )
                {
                    dwCount = max(dwCount, GETHAVEITEM( pActivity->m_vecTargetID[i] ));
                }
                pActivity->SetValue( dwCount );          
            }
        } 

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );
    }
}

void GLCharacter::MsgActivityRewardPoint(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ACTIVITY_REWARD_POINT_SYNC* pNetMsg = (GLMSG::SNET_ACTIVITY_REWARD_POINT_SYNC*) nmg;

    m_llActivityPoint[ACTIVITY_CLASS_LOW] = pNetMsg->llRewardPointLow;
    m_llActivityPoint[ACTIVITY_CLASS_MEDIUM] = pNetMsg->llRewardPointMedium;
    m_llActivityPoint[ACTIVITY_CLASS_HIGH] = pNetMsg->llRewardPointHigh;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );
}

void GLCharacter::MsgActivityRewardBadge(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_ACTIVITY_REWARD_BADGE_SYNC* pNetMsg = (GLMSG::SNET_ACTIVITY_REWARD_BADGE_SYNC*) nmg;

    if ( pNetMsg->wSeq == 0 )
    {
        m_TitleManager.Clear();
    }

    m_TitleManager.SyncTitle( pNetMsg );

    if ( pNetMsg->wSeq == pNetMsg->wLastSeq )
    {
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );

        ReqChangeTitle( RANPARAM::dwCharacterTitleIndex, true );
    }
}

void GLCharacter::MsgActivityChangeBadgeFB( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_ACTIVITY_CHANGE_BADGE_FB* pNetMsg = (GLMSG::SNET_ACTIVITY_CHANGE_BADGE_FB*) nmg;
}

void GLCharacter::MsgActivityLoadCompletedFF( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_ACTIVITY_LOAD_COMPLETED_FF* pNetMsg = (GLMSG::SNET_ACTIVITY_LOAD_COMPLETED_FF*) nmg;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );
}