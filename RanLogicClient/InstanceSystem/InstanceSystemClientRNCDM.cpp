#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

#include "InstanceSystemClient.h"

#include "../Competition/Competition.h"

namespace InstanceSystem
{
    const bool ManagerClient::_msgProc_RNCDM_ProgressUpdate(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC* pPacket = (GLMSG::NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC*)_pMessage;

		DWORD _HighScore = 0;
		int _HighScoreClub = 0;
		DWORD MyScore = 0;
		int MyClub = 0;
		int MyRank = 1;
		const DWORD _dwMyClubNum = pGaeaClient->GetCharacter()->GetClubID();
		if (pPacket->nClub == 0)
			return false;

		GLWidgetScript& _widgetScript(GLWidgetScript::GetInstance());

		std::vector<InstanceSystem::RnCompetitionResultTitleObj> _vectorInformationClub;
		LuaTable _tableArgument(GLWidgetScript::GetInstance().GetLuaState());

		LuaTable _tableInformationBasic(GLWidgetScript::GetInstance().GetLuaState());
		_tableInformationBasic.set(1, pPacket->nHour);
		_tableInformationBasic.set(2, pPacket->nMin);
		_tableInformationBasic.set(3, pPacket->nSecond);
		_tableInformationBasic.set(4, pPacket->nClub);

		unsigned int _indexTable(1);
		_tableArgument.set(_indexTable++, _tableInformationBasic);
		
		for ( DWORD _index(pPacket->nClub); _index--; )
		{
			const GLMSG::NET_INSTANCE_RNCDM_PROGRESS_UPDATE_FC::clubInfo& _informationClub(pPacket->sClubInfo[_index]);

			LuaTable _tableInformationClub( GLWidgetScript::GetInstance().GetLuaState() );
			_tableInformationClub.set(1, _informationClub.dwClubMakrVer);
			_tableInformationClub.set(2, _informationClub.dwClubDbNum);
			_tableInformationClub.set(3, _informationClub.dwClubScore);
			_tableInformationClub.set(4, _informationClub.szClubName);

			_tableArgument.set(_indexTable++, _tableInformationClub);					
		}	

		GLWidgetCallFuncValue _argumentInformationBasic;
		_argumentInformationBasic.SetTable( &_tableArgument );
		VEC_WIDGET_CALLFUNCVALUE _vectorArgument;
		_vectorArgument.push_back( _argumentInformationBasic );
		_widgetScript.LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Competition_CDM_Status_Board_Update, _vectorArgument);
        
        return true;
    }
    
    const bool ManagerClient::_msgProc_RNCDM_ProcessOff(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_PROGRESS_OFF_FC* pPacket = (GLMSG::NET_INSTANCE_RNCDM_PROGRESS_OFF_FC*)_pMessage;

		GLWidgetScript& _widgetScript(GLWidgetScript::GetInstance());
		_widgetScript.LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Competition_CDM_Status_Board_Close);
        return true;
    }
    
    const bool ManagerClient::_msgProc_RNCDM_RankInfo(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_RANK_INFO_FC NetMsg;

        if (CastNetMsg(_pMessage, NetMsg) == false)
        {
            return false;
        }

		//!! SF_TODO
        //pInterface->SetRankSloatData( NetMsg.sRnCDMRankSlot );

        return true;
    }

    const bool ManagerClient::_msgProc_RNCDM_RankShow(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_RANK_SHOW_FC* pPacket = (GLMSG::NET_INSTANCE_RNCDM_RANK_SHOW_FC*)_pMessage;

		//!! SF_TODO
        //pInterface->RankListUpdate();
        //pInterface->UiShowGroupFocus(RNCDM_RANK_WINDOW);
        return true;
    }

    const bool ManagerClient::_msgProc_RNCDM_MapInfo(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_MAP_INFO_FC NetMsg;

        if (CastNetMsg(_pMessage, NetMsg) == false)
        {
            return false;
        }

		//!! SF_TODO
        /*pInterface->SetCDMMapinfo( NetMsg.strMapName, 
            NetMsg.nTimeLimit, 
            NetMsg.nPlayerLimite, 
            NetMsg.nLevelLimit );*/
        return true;
    }
   
    const bool ManagerClient::_msgProc_RNCDM_MapImageName(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_MAP_IMAGE_FC NetMsg;

        if (CastNetMsg(_pMessage, NetMsg) == false)
        {
            return false;
        }

		//!! SF_TODO
        //pInterface->SetCDMMapImageName( NetMsg.strMapImageName );
        return true;
    }
    
    const bool ManagerClient::_msgProc_RNCDM_BattleState(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_TIME_FC NetMsg;

        if (CastNetMsg(_pMessage, NetMsg) == false)
        {
            return false;
        }

		//!! SF_TODO
        //pInterface->SetCDMBattelState( NetMsg.nStartTime, NetMsg.nEndTime );
        return true;
    }

    const bool ManagerClient::_msgProc_RNCDM_StartLogoShow(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_START_LOGO_SHOW_FC* pPacket = (GLMSG::NET_INSTANCE_RNCDM_START_LOGO_SHOW_FC*)_pMessage;
		
		GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Toggle_NotifyCompetitionTexture, "-n", 0);

        return true;
    }
    
    const bool ManagerClient::_msgProc_RNCDM_EndLogoShow(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_END_LOGO_SHOW_FC* pPacket = (GLMSG::NET_INSTANCE_RNCDM_END_LOGO_SHOW_FC*)_pMessage;

		GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Toggle_NotifyCompetitionTexture, "-n", 1);

        return true;
    }

    const bool ManagerClient::_msgProc_RNCDM_TopClubInfo(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_TOP_CLUB_INFO_FC NetMsg;

        if (CastNetMsg(_pMessage, NetMsg) == false)
        {
            return false;
        }

        //pInterface->SetCDMTopList();
        return true;
    }

	const bool ManagerClient::_msgProc_RNCDM_DoublePoint(const NET_MSG_GENERIC* const _pMessage)
	{
		GLMSG::NET_INSTANCE_RNCDM_DOUBLE_POINT_FC NetMsg;

		if (CastNetMsg(_pMessage, NetMsg) == false)
		{
			return false;
		}

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CDM_ShowDoublePoint,
			"-b", TRUE == NetMsg.bVisible ? true : false );
		
		return true;

	}

    const bool ManagerClient::_msgProc_RNCDM_PreviousInfo(const NET_MSG_GENERIC* const _pMessage)
    {
        GLMSG::NET_INSTANCE_RNCDM_PREVIOUS_INFO_FAC NetMsg;

        if (CastNetMsg(_pMessage, NetMsg) == false)
        {
            return false;
        }

       
		Competition::GetInstance().SetCDMTopList(NetMsg.vecClubDbNum, NetMsg.tStartTime, NetMsg.tEndTime, NetMsg.vecClubRank);

        return true;
    }
}
